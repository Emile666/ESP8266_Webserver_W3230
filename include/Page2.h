// HTML web page to handle all parameters
const char htmlPage2[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <style>
  .container { width: 300px; clear: both; }
  .container input { width: 100px; clear: both; }
  </style>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h2>Parameters</h2>
  <div class="container">
  <form action="/get">
    <label><b>SP</b> (E-1 &deg;C) Setpoint value:</label><br \>
    <input type="number" name="sp" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>hy (E-1 &deg;C) Hysteresis:</label><br \>
    <input type="number" name="hy" min="0" max="10" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>hy2 (E-1 &deg;C) Hysteresis 2:</label><br \>
    <input type="number" name="hy2" min="0" max="10" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>tc (E-1 &deg;C) Temperature Correction:</label><br \>
    <input type="number" name="tc" min="-5" max="5" step="0.1">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>tc2 (E-1 &deg;C) Temperature Correction 2:</label><br \>
    <input type="number" name="tc2" min="-5" max="5" step="0.1">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>SA Show Alarm On/Off:</label><br \>
    <input type="number" name="SA" min="0" max="1">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>St (0-8) Current Profile Step:</label><br \>
    <input type="number" name="St" min="0" max="8">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>dh (hours) Current Profile Duration:</label><br \>
    <input type="number" name="dh" min="0" max="999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>cd (min.) Cooling Delay:</label><br \>
    <input type="number" name="cd" min="0" max="60">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>hd (min.) Heating Delay:</label><br \>
    <input type="number" name="hd" min="0" max="60">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>rp Ramping On/Off:</label><br \>
    <input type="number" name="rp" min="0" max="1">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>CF Celsius/Fahrenheit:</label><br \>
    <input type="number" name="cf" min="0" max="1">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Pb2 1=Use 2nd Temp. Probe:</label><br \>
    <input type="number" name="pb2" min="0" max="1">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Kc (%/&deg;C) PID gain:</label><br \>
    <input type="number" name="Kc" min="0" max="9999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Ti (sec) PID integral time-constant:</label><br \>
    <input type="number" name="Ti" min="0" max="9999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Td (sec) PID diff. time-constant:</label><br \>
    <input type="number" name="Td" min="0" max="9999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Ts (sec) PID sample-time:</label><br \>
    <input type="number" name="Ts" min="0" max="9999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Fan 1=Use One-Wire sensor for Fan-Control:</label><br \>
    <input type="number" name="Fan" min="0" max="1">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Flo Lower-Limit Temp. for Fan-Control:</label><br \>
    <input type="number" name="Flo" min="0" max="999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Fhi Upper-Limit Temp. for Fan-Control:</label><br \>
    <input type="number" name="Fhi" min="0" max="999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Hpl Heating Power-Limit:</label><br \>
    <input type="number" name="Hpl" min="0" max="999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>Hpt Total Heating Power:</label><br \>
    <input type="number" name="Hpt" min="0" max="999">
    <input type="submit" value="Submit">
  </form>
 <form action="/get">
    <label>rn Set Run-mode 0-5, 6=th:</label><br \>
    <input type="number" name="rn" min="0" max="6">
    <input type="submit" value="Submit">
  </form>
  </div>
</body></html>)rawliteral";
