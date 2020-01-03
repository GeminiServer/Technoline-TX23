// Technoline TX23 - ESP
//
// Copyright (C) 2019  Erkan Çolak
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


// Set LED pin to indicate the TX23 state.
//
const int led= 2;

// Set IO-Port for TX23 Data IO
//
const uint16_t tx_io_port = 0;

// Define your WiFi SSID and PSK
//
#define STASSID "Your-SSID"
#define STAPSK  "Your-SSID-Password-PSK"

#define WEBSERVER_PORT 80
#define WEBSOCKET_PORT 81

// Define middleware url
//
#define MIDDLEWARE_URL    "http://11.11.0.15/middleware.php/data/68097fe0-2cdb-11ea-9c47-e96e7c8f072a.json"
#define MIDDLEWARE_OPERATION  "?operation=add&value="
