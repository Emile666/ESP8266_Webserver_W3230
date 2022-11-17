// HTML web page to handle all parameters
const char htmlPage3[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <style>
  .container { width: 300px; clear: both; }
  .container input { width: 100px; clear: both; }
  </style>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h2>Profile Pr0</h2>
  <div class="container">
  <form action="/get">
    <label><b>SP0</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh0 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP1</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh1 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP2</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh2 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP3</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh3 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP4</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh4 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP5</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh5 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP6</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh6 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP7</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh7 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP8</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label>dh8 (hours):</label><br \>
    <input type="number" name="dh0" min="0" max="99">
    <input type="submit" value="Submit">
  </form>
  <form action="/get">
    <label><b>SP9</b> (E-1 &deg;C):</label><br \>
    <input type="number" name="sp0" min="0" max="100" step="0.1">
    <input type="submit" value="Submit">
  </form>
  </div>
</body></html>)rawliteral";
