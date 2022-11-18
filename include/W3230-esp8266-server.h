#ifndef W3230ESP8266SERVER_H
#define W3230ESP8266SERVER_H

#include <Arduino.h>
#include <cstdio>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TimeLib.h>
#include <LittleFS.h>

#define VERSION       "ESP8266_W3230_v012"
#define POLLPERIOD       (60) /* How often (in seconds) to poll for data which is used in the graph */
#define ALLOCATED_RAM (30000) 

#define BUFLEN          (100) /* Buffer length for UART input buffer */
#define MAXPARAMS        (19) /* Number of parameters in PAR menu */
#define PROFILE_SIZE     (19) /* Number of values in a temp-time profile */
#define MAXPROFILES       (6) /* Number of Profiles in temp. controller */
#define TOTALPARS     (MAXPROFILES*PROFILE_SIZE + MAXPARAMS) /* Total number of inputs */

void    setup(void);
uint8_t execute_single_command(char *s);
void    loop(void);
void    timeAndDate (time_t tt, String& htmlContent );
void    allocateRam(void);
String  processor(const String& var); 
String  chart1Processor(const String& var);
String  chart2Processor(const String& var);
String  chart3Processor(const String& var);
time_t  syncProvider(void);
void    initFS(void);
String  readFile(fs::FS &fs, const char * path) ;
void    writeFile(fs::FS &fs, const char * path, const char * message);
bool    initWiFi(void);
void    notFound(AsyncWebServerRequest *request);
void    normalWebPages(void);
void    APPages(void);

#endif