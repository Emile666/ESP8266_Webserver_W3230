/********************************************************************************************
 * This is the ESP8266 Webserver for the W3230 temperature controller.
 ********************************************************************************************/
#include "W3230-esp8266-server.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <algorithm>

//#include "Page1.h"
//#include "Page2.h"
#include "Page3.h"
#include "Chart1_html.h"
#include "Chart2_html.h"
#include "Chart3_html.h"

char rs232_buff[BUFLEN];
byte rs232_idx = 0;

// Define NTP Client to get time
WiFiUDP        ntpUDP;
NTPClient      timeClient(ntpUDP, "pool.ntp.org");
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";

//Variables to save values from HTML form
String ssid;
String pass;

struct wifis {
  String  name;
  int32_t rssi;
}; // struct wifis
wifis wifiList[20]; // List with WiFi stations found
byte  numSsid;      // number of WiFi stations found (max. 20)

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";

IPAddress localIP;
IPAddress localGateway;
IPAddress subnet(255, 255, 0, 0);

const char params[MAXPARAMS][9] = {"sp","hy","hy2","tc","tc2","SA","cd","hd","cf","Kc","Ti","Td","Ts","Fan","Flo","Fhi","Hpl","Hpt","rn"};

uint16_t  pars[MAXPARAMS];                 // parameter values retrieved from temp controller
uint16_t  prof[MAXPROFILES][PROFILE_SIZE]; // profile data values retrieved from temp controller
uint16_t  count = 0;
uint32_t  numberOfRows=0; // size of array
uint32_t  timeKeeper=0;
int16_t  *t1Data;
int16_t  *t2Data;
int8_t   *hcData;
int16_t  *owData;
uint32_t *timeStamp;
boolean   restart = false;

int16_t  t1,t2,ow;   // Temp1, Temp2, One-Wire Temp
uint8_t  hc;         // heating/cooling
uint16_t sp;         // Setpoint temp
bool     pRequest;   // handshake between processor() and serial IO
uint32_t freeRAM;

//----------------------------------------------------------------------------------
// This is the setup() function for the ESP8266.
//----------------------------------------------------------------------------------
void setup(void)
{
  timeKeeper = millis();
  Serial.begin(57600);

  initFS();

  // Load values saved in LittleFS
  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  Serial.print("Trying to connect to ");
  Serial.print(ssid);
  Serial.print(" with password ");
  Serial.println(pass);

  if (initWiFi())
  {
    normalWebPages();
  }
  else
  {
    APPages();
  }                   // if
  timeClient.begin(); // Initialize a NTPClient to get time
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
  setSyncProvider(syncProvider); // Sync millis() with NTP-time
  timeKeeper = millis() + (POLLPERIOD * 1000);
  allocateRam();
  Serial.println("p6"); // send request for parameters to temp. controller
} // setup()

//----------------------------------------------------------------------------------
// This function executes a single command received via the UART.
//----------------------------------------------------------------------------------
uint8_t execute_single_command(char *s)
{
   const char delim[] = ", ";
   uint8_t    num  = atoi(&s[1]); // convert number in command (until space is found)
   uint8_t    rval = 0;
   uint8_t    i;
   char       s2[40];
   char       *p;
   
   switch (s[0])
   {
     case 'e': 
        if (num > 1) 
           rval = 1;
        else if (!num)
        { // e0: get date & time
          timeClient.update();
          unsigned long epochTime = timeClient.getEpochTime();
          // Get a time structure
          struct tm *ptm = gmtime ((time_t *)&epochTime); 
          sprintf(s2,"e0 %02d-%02d-%4d.",ptm->tm_mday, ptm->tm_mon+1, ptm->tm_year+1900);
          Serial.print(s2);
          Serial.println(timeClient.getFormattedTime());
        } // if
        else timeClient.begin(); // e1: init. again
        break;

     //--------------------------------------------------------
     // The data from the W3230 Temperature Controller is sent
     // Every minute to the ESP8266 via the Serial Port.
     //--------------------------------------------------------
     case 'l':
        hc  = num;                // current state of W3230
        p   = strtok(&s[3],delim); // get the first temp.
        t1  = atoi(p);
        p   = strtok(NULL,delim);  // get the second temp.
        t2  = atoi(p);
        p   = strtok(NULL,delim);  // get the OW-temp.
        ow  = atoi(p);
        p   = strtok(NULL,delim);  // get the setpoint
        sp  = atoi(p);
        break;
        
     case 'p':
        if (num < MAXPROFILES)   
        {  // One of the profiles
           i = 0;
           p = strtok(&s[3],delim);
           prof[num][i++] = atoi(p);
           while (i < PROFILE_SIZE)
           {
              p              = strtok(NULL,delim);
              prof[num][i++] = atoi(p);
           } // while
           pRequest = true; // flag to signal that data has been read
        } // if
        else if (num == MAXPROFILES)
        { // The parameter section
           i = 0;
           p = strtok(&s[3],delim);
           pars[i++] = atoi(p);
           while (i < PROFILE_SIZE)
           {
              p         = strtok(NULL,delim);
              pars[i++] = atoi(p);
           } // while
           pRequest = true; // flag to signal that data has been read
        } // else if
        break;

     case 's': // s0: get version info
        if (!num)
        {
          Serial.println("esp8266_ntp_v010");
        } // if
        break;
     default: rval = 1; // ERR
              break;
   } // switch
   return rval; 
} // execute_single_command()

//----------------------------------------------------------------------------------
// This is the main loop() function for the ESP8266.
//----------------------------------------------------------------------------------
void loop(void) {
  if (restart){
    delay(5000);
    ESP.restart();
  } // if
  
  if (Serial.available() > 0)
  {
    byte c = Serial.read();
    if ((rs232_idx < sizeof rs232_buff) && (c != '\n'))
    {
      rs232_buff[rs232_idx++] = c;
      if (c == '\r') // (CR,13) end of word
      {
        execute_single_command(rs232_buff);
        rs232_idx = 0;  
      } // if
    } // if
    else rs232_idx = 0; // reset index
  } // if

  freeRAM = system_get_free_heap_size();

  //--------------------------------------
  // Check if poll period has expired
  //--------------------------------------
  if (millis()>=timeKeeper)
  { // Update timeKeeper with the latest time + poll period (multiply by 1000 as millis() is milliseconds)
    timeKeeper = millis() + (POLLPERIOD * 1000);   
    uint32_t currentTime = now();
    if (count < numberOfRows)
    {   // Update each row in the array until it is full
        t1Data[count]    = t1;           // temp1_ntc
        t2Data[count]    = t2;           // temp2_ntc
        owData[count]    = ow;           // ow-temp.
        hcData[count]    = hc;           // Heat/Cool
        timeStamp[count] = currentTime;  //Current time
        count++;
    } // if
    else
	{	  // count >= numberOfRows
      for (uint16_t i = 0; i<numberOfRows-1 ; i++)
      {	// Cycle the array. Move everything forward by one and add the new values to the end
        t1Data[i]    = t1Data[i+1];
        t2Data[i]    = t2Data[i+1];
        owData[i]    = owData[i+1];
        hcData[i]    = hcData[i+1];
        timeStamp[i] = timeStamp[i+1];
      } // for i
      t1Data[numberOfRows-1]    = t1;
      t2Data[numberOfRows-1]    = t2;
      owData[numberOfRows-1]    = ow;
      hcData[numberOfRows-1]    = hc;
      timeStamp[numberOfRows-1] = currentTime;
    } // else
  } // if
} // loop()

//----------------------------------------------------------------------------------
// This function is a Helper function to convert UNIX time to a String.
//----------------------------------------------------------------------------------
void timeAndDate (time_t tt, String& htmlContent ){
  if (hour(tt) < 10) // If hour is less than ten, it will only be one character. This would mean we would need to place a '0' in front of the character.
    htmlContent += ("0" + String(hour(tt))+ ":");
  else
    htmlContent += (String(hour(tt)) + ":");
 if (minute(tt) < 10)
    htmlContent += ("0" + String(minute(tt)) + ":");
 else
    htmlContent += (String(minute(tt)) + ":");
 if (second(tt) < 10)
   htmlContent += ("0" + String(second(tt)) + " ");
 else
   htmlContent += (String(second(tt)) + " ");
 
 if (day(tt) < 10)
   htmlContent += ("0" + String(day(tt))+ "/");
 else
  htmlContent += (String(day(tt)) + "/");
 if (month(tt) < 10)
    htmlContent += ("0" + String(month(tt))+ "/");
  else
    htmlContent += (String(month(tt)) + "/");
  htmlContent += (String(year(tt)));
} // timeAndDate()

//--------------------------------------------------------------------------------------
// This function allocates sufficient RAM for the data from the temperature controller.
//--------------------------------------------------------------------------------------
void allocateRam(void)
{
  //We will allocate as much RAM as we can to track historical data. This means, we need to work out how much RAM we will needed for the TCP stack and enough for the ESP8266 to run. 
  //Once we get this value, we can allocate the rest of the RAM to the array used to store the temperature and pressure values.
  //
  //Each TCP session expires every 2mins. (Default MSL is 2mins for Windows https://technet.microsoft.com/en-us/library/cc938217.aspx)
  //If using the refresh function on the main page with gauges, then there needs to be enough RAM left to accommodate the TCP sessions from the refresh as well as a little extra for the ESP8266 to run.
  //196KB RAM per refresh. 
  //To calculate the amount to leave free;
  // (number of refreshes in 2 minutes x RAM per REFRESH) + SPARE RAM = ALLOCATED_RAM
  // 
  //E.g. For a refresh of every 1 second
  //(120x196KB) + 10,000 = 33,520
  //For a refresh of every 3 seconds
  //(40x196KB) + 10,000 = 17,840
  //
  //The less frequent the refresh will result in a smaller value needed to be reserved. And this also mean more RAM can be used to store historical data for a longer period.
  //If refresh isnt used, then leave ALLOCATED_RAM to 10,000
  
  //Get free RAM in bytes
  uint32_t free=system_get_free_heap_size() - ALLOCATED_RAM;
  
  // Divide the free RAM by the size of the variables used to store the data. 
  // This will allow us to work out the maximum number of records we can store. 
  // All while keeping some RAM free which is specified in ALLOCATED_RAM
  numberOfRows = free / (sizeof(uint8_t)*45); // temp1 (2), temp2 (2), HorC (1), pid_out (1), One (2), Time (4), each data-line in html (31), spare(2). 
  
  //re-declare the arrays with the number of elements
  t1Data    = new int16_t  [numberOfRows];
  t2Data    = new int16_t  [numberOfRows];
  hcData    = new int8_t   [numberOfRows];
  owData    = new int16_t  [numberOfRows];
  timeStamp = new uint32_t [numberOfRows];

  if ( t1Data==NULL || t2Data==NULL || hcData==NULL || owData==NULL || timeStamp==NULL)
  {
    numberOfRows=0;
    Serial.println("Error in memory allocation!");
  } // if
  else
  {
    Serial.print("Allocated storage for ");
    Serial.print(numberOfRows);
    Serial.println(" data points.");
  } // else
  delay(2000);
} // allocateRam()

//----------------------------------------------------------------------------------
// This function replaces the placeholder text in the html source with actual data.
// It is used for adding variables to the gauges on the main web-page.
//----------------------------------------------------------------------------------
String wifiMgrProcessor(const String& var)
{
  String html;
  
  if (var == "PLACEHOLDER1")
  {
    //Here we add the combobox with the sorted WiFi access-points found
    html  = "";
    for (int i = 0; i < numSsid; i++)
    {
      html += "<option value = \"" + wifiList[i].name + "\">"+ wifiList[i].name + " (" + String(wifiList[i].rssi) + " dBm)</option>";
    } // for i
    html += "</select><BR>";
    return html;
  } // if
  return String();
} // wifiMgrProcessor()

//----------------------------------------------------------------------------------
// This function replaces the placeholder text in the html source with actual data.
// It is used for adding variables to the gauges on the main web-page.
//----------------------------------------------------------------------------------
String processor(const String& var)
{
  String html, html1;
  
  if (var == "PLACEHOLDER1")
  {
    //Here we loop through the data to place it into the html source code
    html  = "<h3 style=text-align:center;>Free RAM = " + String(freeRAM) + " Bytes</h3>";
    return html;
  } // if
  return String();
} // processor()

//----------------------------------------------------------------------------------
// This function replaces the placeholder text in the html source with actual data.
// It is used for adding variables to the gauges on the main web-page.
//----------------------------------------------------------------------------------
String paramProcessor(const String& var)
{
  if      (var == "NR0")  return String(pars[0]);
  else if (var == "NR1")  return String(pars[1]);
  else if (var == "NR2")  return String(pars[2]);
  else if (var == "NR3")  return String(pars[3]);
  else if (var == "NR4")  return String(pars[4]);
  else if (var == "NR5")  return String(pars[5]);
  else if (var == "NR6")  return String(pars[6]);
  else if (var == "NR7")  return String(pars[7]);
  else if (var == "NR8")  return String(pars[8]);
  else if (var == "NR9")  return String(pars[9]);
  else if (var == "NR10") return String(pars[10]);
  else if (var == "NR11") return String(pars[11]);
  else if (var == "NR12") return String(pars[12]);
  else if (var == "NR13") return String(pars[13]);
  else if (var == "NR14") return String(pars[14]);
  else if (var == "NR15") return String(pars[15]);
  else if (var == "NR16") return String(pars[16]);
  else if (var == "NR17") return String(pars[17]);
  else if (var == "NR18") return String(pars[18]);
  return String();
} // paramProcessor()

//----------------------------------------------------------------------------------
// This function replaces the placeholder text in the html source with actual data.
// It is used for adding variables to the Temp1 Chart.
//----------------------------------------------------------------------------------
String chart1Processor(const String& var)
{
	String html = "";
	if (var == "PLACEHOLDER1")
	{
	    //Here we loop through the data to place it into the html source code
		for (int i = 0; i< count ; i++)
		{
			html += ("[new Date(" + String(timeStamp[i]) +  "000)," + String(t1Data[i]*0.1) + "]");
		    if (i < count-1) html += ",\n";
		} // for i
		return html;
	} // if
	else if (var == "PLACEHOLDER2")
	{
	    html = ("Number of readings=" + String(count) + "<BR>Max allowed readings=" + String(numberOfRows) + "<BR>");
		//Display the data and time for first and last reading
		html += ("<BR><BR>First reading at : ");
		timeAndDate(timeStamp[0],html);    
    if (count > 1)
    {
      html += ("<BR>Most recent reading : ");
      timeAndDate(timeStamp[count-1],html);     
    } // if
		return html;
	} // else
	return String();
} // chart1Processor()

//----------------------------------------------------------------------------------
// This function replaces the placeholder text in the html source with actual data.
// It is used for adding variables to the Temp2 Chart.
//----------------------------------------------------------------------------------
String chart2Processor(const String& var)
{
	String html = "";
	if (var == "PLACEHOLDER1")
	{
	    //Here we loop through the data to place it into the html source code
		for (int i = 0; i< count ; i++)
		{
			html += ("[new Date(" + String(timeStamp[i]) +  "000)," + String(t2Data[i]*0.1) + "]");
		    if (i < count-1) html += ",\n";
		} // for i
		return html;
	} // if
	else if (var == "PLACEHOLDER2")
	{
	    html = ("Number of readings=" + String(count) + "<BR>Max allowed readings=" + String(numberOfRows) + "<BR>");
		//Display the data and time for first and last reading
		html += ("<BR><BR>First reading at : ");
		timeAndDate(timeStamp[0],html);    
    if (count > 1)
    {
      html += ("<BR>Most recent reading : ");
      timeAndDate(timeStamp[count-1],html);     
    } // if
		return html;
	} // else
	return String();
} // chart2Processor()

//----------------------------------------------------------------------------------
// This function replaces the placeholder text in the html source with actual data.
// It is used for adding variables to the Temp-OW Chart.
//----------------------------------------------------------------------------------
String chart3Processor(const String& var)
{
	String html = "";
	if (var == "PLACEHOLDER1")
	{
	    //Here we loop through the data to place it into the html source code
		for (int i = 0; i< count ; i++)
		{
			html += ("[new Date(" + String(timeStamp[i]) +  "000)," + String(hcData[i]) + "]");
		    if (i < count-1) html += ",\n";
		} // for i
		return html;
	} // if
	else if (var == "PLACEHOLDER2")
	{
	    html = ("Number of readings=" + String(count) + "<BR>Max allowed readings=" + String(numberOfRows) + "<BR>");
		//Display the data and time for first and last reading
		html += ("<BR><BR>First reading at : ");
		timeAndDate(timeStamp[0],html);    
    if (count > 1)
    {
      html += ("<BR>Most recent reading : ");
      timeAndDate(timeStamp[count-1],html);     
    } // if
		return html;
	} // else
	return String();
} // chart3Processor()

//----------------------------------------------------------------------------------
// This function syncs the internal time with the time from the time-server.
//----------------------------------------------------------------------------------
time_t syncProvider(void)
{
  timeClient.update();
  return timeClient.getEpochTime();
} // syncProvider()

//----------------------------------------------------------------------------------
// This function initializes the ESP8266 File-system, which is LittleFS.
//----------------------------------------------------------------------------------
void initFS(void) 
{
  if (!LittleFS.begin()) {
    Serial.println("\nAn error has occurred while mounting LittleFS");
  } // if
  else{
    Serial.println("\nLittleFS mounted successfully");
  } // else
} // initFS()

//----------------------------------------------------------------------------------
// This function reads a File from LittleFS.
//----------------------------------------------------------------------------------
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
} // readFile()

//----------------------------------------------------------------------------------
// This function writes a file to LittleFS.
//----------------------------------------------------------------------------------
void writeFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
  file.close();
} // writeFile()

//----------------------------------------------------------------------------------
// This function initializes WiFi
//----------------------------------------------------------------------------------
bool initWiFi(void) 
{
  if (ssid=="" || pass==""){
    Serial.println("Undefined SSID or password");
    return false;
  } // if

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.println("WiFi Failed!");
    return false;
  } // if
  Serial.println();
  Serial.print("WiFi Connected, IP Address: ");
  Serial.println(WiFi.localIP());
  return true;
} // initWiFi()

//----------------------------------------------------------------------------------
// This function is executed when a web-page is not found.
//----------------------------------------------------------------------------------
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
} // notFound()

//----------------------------------------------------------------------------------
// This function initializes the normal web-pages.
//----------------------------------------------------------------------------------
void normalWebPages(void) 
{
  //---------------------------------------------
  // Send web page with input fields to client
  //---------------------------------------------
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send_P(200, "text/html", htmlPage1, processor);
	request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  server.serveStatic("/", LittleFS, "/");

  //-----------------------
  // Parameters Page
  //-----------------------
  server.on("/page2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/params.html", "text/html", false, paramProcessor);
  });

  //-----------------------
  // Profile Pr0 Page
  //-----------------------
  server.on("/page3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", htmlPage3);
  });

  //-----------------------
  // /data.json Page
  //-----------------------
  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request){
    
  String html;
  html  = ("{\"T1\":\"" + String(t1*0.1)  + "\",\"T2\":\""   + String(t2*0.1) + "\",\"HeatCool\":\"" + String(hc) + "\",");
  html += ("\"SP\":\"" + String(sp*0.1) + "\",\"OW1\":\"" + String(ow*0.1) + "\"}");
    request->send_P(200, "application/json", html.c_str());
  });

  //-----------------------
  // /chart1.html Page
  //-----------------------
  server.on("/chart1.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", chart1_html, chart1Processor);
  });
  
  //-----------------------
  // /chart2.html Page
  //-----------------------
  server.on("/chart2.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", chart2_html, chart2Processor);
  });

  //-----------------------
  // /chart3.html Page
  //-----------------------
  server.on("/chart3.html", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/html", chart3_html, chart3Processor);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String  inputMessage;
    String  inputParam;
    String  parName;
    int     i = 0;
    int16_t oldVal;

    while (i < TOTALPARS)
    { // GET input value on <ESP_IP>/get?inputx=<inputMessage>
      parName = "nr" + String(i);
      uint8_t prNr    = (i / PROFILE_SIZE); // Profile nr, 6 = parameters
      uint8_t parNr   = (i % PROFILE_SIZE); // Number within profile
      if (request->hasParam(parName)) 
      {
        int16_t parVal = request->getParam(parName)->value().toInt();
        inputMessage   = "v" + String(i) + " " + String(parVal); // value
        if (i < PROFILE_SIZE*MAXPROFILES)
        { // One of the Profile parameters
          oldVal = prof[prNr][parNr];
          if (oldVal != parVal) Serial.println(inputMessage);
          prof[prNr][parNr] = parVal;
        } // if
        else 
        { // One of the Parameters
          oldVal = pars[parNr];
          if (oldVal != parVal) Serial.println(inputMessage);
          pars[parNr] = parVal;
        } // else
      } // if
      i++;
    } // while    
    request->send(LittleFS, "/index.html", "text/html", false, processor); // Back to Home Page
    //request->send(200, "text/html", "/"); // Return to Home Page
    //                                 + inputParam + ") with value: " + inputMessage +
    //                                 "<br><a href=\"/\">Return to Home Page</a>");
  });

  server.onNotFound(notFound);
  server.begin();
  Serial.println("HTTP Server started");
} // normalWebPages()

//----------------------------------------------------------------------------------
// This function initializes the web-pages for the Wifi-Manager.
//----------------------------------------------------------------------------------
void APPages(void)
{
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-WIFI-MANAGER", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 
    
    numSsid = WiFi.scanNetworks();
    Serial.print("Number of available WiFi networks discovered:");
    Serial.println(numSsid);
    if (numSsid > 20) numSsid = 20;

    // print the network number and name for each network found:
    for (int i = 0; i<numSsid; i++) 
    {
      char s[10];
      sprintf(s,"%02d) ",i);
      Serial.print(s);
      Serial.print(WiFi.SSID(i));
      Serial.print(", ");
      Serial.print(WiFi.RSSI(i));
      Serial.println(" dBm");
      wifiList[i].name = WiFi.SSID(i);
      wifiList[i].rssi = WiFi.RSSI(i);
    } // for i
    std::sort(wifiList,wifiList+numSsid,[] (const wifis &wifi1, const wifis &wifi2)
    {
      if (wifi1.rssi > wifi2.rssi) return true;
      else if (wifi1.rssi == wifi2.rssi)
      {
        if (wifi1.name > wifi2.name) return true;
      } // else if
      return false;
    });

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifimanager.html", "text/html", false, wifiMgrProcessor);
    });
    
    server.serveStatic("/", LittleFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) 
    {
      int params = request->params();
      for (int i=0;i<params;i++)
      {
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) 
          {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          } // if
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) 
          {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          } // if
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } // if
      } // for i
      restart = true;
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and check IP address.");
    }); // server.on()
    server.begin();
} // APPages()
  
