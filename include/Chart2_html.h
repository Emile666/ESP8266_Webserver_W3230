// HTML web page to handle web-site /chart.html
const char chart2_html[] PROGMEM = R"rawliteral(<html><head><title>Chart</title>
  <script type="text/javascript" src="https://www.google.com/jsapi?autoload={'modules':[{'name':'visualization','version':'1','packages':['corechart']}]}"></script>
  <script type="text/javascript">
  google.charts.load('current', {packages: ['corechart', 'line']});
  google.charts.setOnLoadCallback(drawChart);
  var button = document.getElementById('change-chart');function drawChart() {var data = google.visualization.arrayToDataTable([
  ['Local Time', 'T2 \260C'],
  %PLACEHOLDER1%
  ]);
  function drawChart() {
	  var t2view = new google.visualization.DataView(data);
	  t2view.setColumns([0,1]);
      chart.draw(t2view, optionsTemp);}
  var formatter = new google.visualization.DateFormat({ formatType: 'short',timeZone: 0});
  formatter.format(data, 0);
  var xTicks = [];
  for (var i = 0; i < data.getNumberOfRows(); i++) {
     xTicks.push({
	  v: data.getValue(i, 0),
      f: data.getFormattedValue(i, 0) });
  }
  var optionsTemp = {'height': 320,chartArea:{top:20, height:"60%%"},hAxis:{gridlines:{color:'transparent'},
                     ticks:xTicks,slantedText: true,slantedTextAngle :70,textStyle:{fontSize: 11} },vAxis:{format:"##.# \260C"},
                     series:{1:{curveType:'function'},0:{color:'red'}},legend:{position: 'none'},title:'T2 temperature in \260C' };
  var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));drawChart();}
  </script>
  <font color="#000000"><body><meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=yes">
  <h1>W3230 T2 Chart</h1><a href="/">Back</a><BR><BR>
  <div id="curve_chart" style="width: 800px; height: 300px"></div><BR><BR>
  %PLACEHOLDER2%
  <BR></body></html>
)rawliteral";
  
