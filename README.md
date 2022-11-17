ESP8266 Web-Server for W3230-STM8S105
==========

This is a web-server for the W3230-STM8S105 temperature controller. The W3230 contains a 8-pin interface for the ESP8266 and uses the UART (TX and RX) to send commands to and from the W3230.

This web-server contains the following features:
- A Wifi-Manager that displays a sorted list of all Wifi-stations found.
- A main-page with 3 temperatures gauges, showing temperature of NTC1, NTC2 and the temperature from the One-Wire Sensor. It also shows the current status of the W3230 (e.g. Cooling, heating, Idle) and the temperature setpoint.
- Two charts, one for NTC1 temperature and one for NTC2 temperature, showing logged temperature of up to 5 hours back.
- A chart for the state of the W3230. This chart shows when the W3230 was heating and cooling.
- A parameter page where all parameters of the W3230 can be adjusted.
- Six web-pages containing the Profile data (10 setpoint temperatures and 9 durations in hours) where the profile data can be adjusted.

Software Development Environment
-----------
- Visual Studio Code with PlatformIO

In order to program the ESP8266, you need to have a ESP8266 programmer with two switches added, one for Reset and one for Program (connected to GPIO0).


