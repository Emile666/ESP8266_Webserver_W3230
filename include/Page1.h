// HTML web page to handle Dashboard on main-page
const char htmlPage1[] PROGMEM = R"rawliteral(
  <html><head><style type="text/css">
  <!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
  </style>
  <link rel="stylesheet" type="text/css" href="main.css"></head><body>
  <div style="float:left; width:300px; height: 300px; text-align:center;">
  <h3 style=text-align:center;font-size:200%%;>W3230</h3>
  <h3 style=text-align:center;>Dashboard</h3>
  <button id="Std">IDLE</button>
  %PLACEHOLDER1%
  <h4 style=text-align:center;>
  <a href="/chart1.html">Chart T1</a><br>
  <a href="/chart2.html">Chart T2</a><br>
  <a href="/chart3.html">Chart States</a></h4></div>
  
  <script type='text/javascript' src='https://ajax.googleapis.com/ajax/libs/jquery/1.4.4/jquery.min.js'></script>
  <script type='text/javascript' src='https://www.google.com/jsapi'></script>
  <script type='text/javascript'>
  $.ajaxSetup ({ 
  // Disable caching of AJAX responses
  cache: false 
  });
  var chart1;var chart2;var chart3;var data1;var data2;var data3;google.load('visualization', '1', {packages:['gauge']});
  var button = document.getElementById('Std');
  const stdText = ["IDLE","Heating Delay","Cooling Delay","Now Heating","Now Cooling","No Cooling, T2 LOW","NO Heating, T2 High"];
  google.setOnLoadCallback(initChart);
  
  function displayData1(cValue1) {
	data1.setValue(0, 0, 'T1 \260C');
	data1.setValue(0, 1, cValue1);
	chart1.draw(data1, optionsTempC);}
  
  function displayData2(cValue2) {
  data2.setValue(0, 0, 'T2 \260C');
  data2.setValue(0, 1, cValue2);
  chart2.draw(data2, optionsTempC);}

  function displayData3(cValue3) {
  data3.setValue(0, 0, 'OW \260C');
  data3.setValue(0, 1, cValue3);
  chart3.draw(data3, optionsTempC);}

  function displayData4(cValue4) {
    button.style.backgroundColor = 'inherit';
    button.style.border = 'none';
    button.style.fontWeight = 'bold';
    button.style.fontSize = '20px';
	  if (cValue4 < 7) {
		  button.innerText = stdText[cValue4];
      if (cValue4 == 0) {
        button.style.color = 'black';
      } else if (cValue4 == 1 || cValue4 == 3 || cValue4 == 6) {
        button.style.color = 'red';
      } else {
        button.style.color = 'blue';
      }
	}
  }

  function loadData() {var c1; var c2; var c3; var c4;
  $.getJSON('/data.json', function(data) {
  c1 = data.T1;
  c2 = data.T2;
  c3 = data.OW1;
  c4 = data.HeatCool;
  if(c1){displayData1(c1);}
  if(c2){displayData2(c2);}
  if(c3){displayData3(c3);}
  if(c4){displayData4(c4);}
  });}
  
  function initChart() {
  chart1 = new google.visualization.Gauge(document.getElementById('chart_div1'));  
  data1 = google.visualization.arrayToDataTable([['Label', 'Value'],['T1 \260C', 35],]);
  chart2 = new google.visualization.Gauge(document.getElementById('chart_div2'));  
  data2 = google.visualization.arrayToDataTable([['Label', 'Value'],['T2 \260C', 35],]);
  chart3 = new google.visualization.Gauge(document.getElementById('chart_div3'));  
  data3 = google.visualization.arrayToDataTable([['Label', 'Value'],['OW \260C', 35],]);
  optionsTempC = {width: 250, height: 250, redFrom: 35, redTo: 50,yellowFrom:30, yellowTo: 35, majorTicks: ['0', '10', '20', '30', '40', '50'], minorTicks: 5,min: 0, max: 50};
  loadData();setInterval(loadData,1000);}
  function openNav() {
	document.getElementById("open").style.display = "none";
	document.getElementById("nav").style.width = "100%";}
  function closeNav() {
	document.getElementById("nav").style.width = "0";
	document.getElementById("open").style.display = "block";
  } </script>
  <div id="chart_div1" style="float:left; width:300px; height: 300px;"></div>
  <div style=\"clear:both;\"></div>
  <div id="chart_div2" style="float:left; width:300px; height: 300px;"></div>
  <div id="chart_div3" style="float:left; width:300px; height: 300px;"></div>
  <title>ESP8266 Main Page</title>
  <div class="wrapper">
	<div class="nav" id="nav">
		<a href="javascript:void(0)" class="close" onclick="closeNav()">&times;</a>
		<a href="#">Home</a>
		<a href="/chart1.html">Chart Temp1</a>
		<a href="/chart2.html">Chart Temp2</a>
		<a href="/chart3.html">Chart States</a>
		<a href="/page2">Change Parameters</a>
		<a href="/page3">Change Profile 0</a>
	</div>
    <span onclick="openNav()" class="open" id="open">&#9776</span>
  </div>
</body></html>)rawliteral";
