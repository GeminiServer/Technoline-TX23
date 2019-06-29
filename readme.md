# Technoline TX23 Wind Sensor for ESP WiFi

[Technoline TX23] also known as [LaCrosse TX23], is  a wind speed and wind direction sensor. Maximum value for wind speed ist 51.1 meter per second ( 183.96 km/h or 114.31 miles per hour).

It uses 3 wires for communication and power:

| TX23 Cable/Pin   | Color           | Description |
|------            | --------------- | -----------
|Pin1	             | Brown or Black	 | DATA
|Pin2	             | Red or Pink 		 | Vcc
|Pin3	             | Green					 | N/C
|Pin4	             | Yellow				   | GND


Currently it is unclear of what Voltage the TX23 is normally driven with when connected to its original Weather Station, but it seems to run with 3.3 Volts without any problems. I connected
the DATA pin directly to one of ESP IO ports.

This port an the TX23 [lib] is based on the Creation of Krzysztof Egzmont which is based on the fantastic work of [John]: Big thanks.

#### Features:
- WiFi client
- WebServer (Port: 80)
- WebSocket Communication (Port: 81)
- Example Page, which will print current Sensor Informations
- Separate Job Schedules (loop's) are used. E.g.: read sensor values,  WebServer, WebSocket communication, etc.

### Installation and configuration
Just clone this repo and open it with Atom (PlatformIO) and Build it. Current i added esp01_1m (esp8622 generic) and esp8285 support. Default is esp8285. To change it, just open the platformio.ini an uncomment your device.

The Wifi is in Client mode. So you need to define/set your ap ssid and password.
```sh
#
# Change in main.cpp
define STASSID "YOUR_AP_WIFI_SSID"
define STAPSK  "password"

# Important: Set the TX23 DATA IO Pin
const uint16_t port = 17;
#Optional you can set the LED IO pin
const int led= 16;

#
# Enable (define) or disable (comment out) Console output (Default is enabled!)
# main.h
define SERIAL_DEBUG
```


[John]: <https://www.john.geek.nz/2012/08/la-crosse-tx23u-anemometer-communication-protocol/>
[lib]: <https://github.com/egzumer/Arduino-LaCrosse-TX23-Library>
[LaCrosse TX23]: https://www.lacrossetechnology.com/tx23-wind-sensor
[Technoline TX23]: https://www.lacrossetechnology.com/tx23-wind-sensor
