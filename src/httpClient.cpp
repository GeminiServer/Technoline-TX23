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

#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "httpClient.h"
#include "tx23.h"
#include "settings.h"

extern TX23 tx_23;
extern Settings eeprom_settings;

void httpClient::HttpPostSpeed() {
  if (eeprom_settings.tx_23_settings.bMiddlewareEnabled && tx_23.bLastReadState) {
    HTTPClient http;    // Declare object of class HTTPClient
                        // WiFiClient client;

    String sLink = eeprom_settings.tx_23_settings.cMiddleware;
           sLink += eeprom_settings.tx_23_settings.cMiddlewareOperation;
           sLink += tx_23.sRawSpeedKMh;

    #ifdef SERIAL_DEBUG
      Serial_Log.println(sLink);   //Print HTTP return code
    #endif

    if ( http.begin(newWifiClient, sLink)) {     //Specify request destination
      int httpCode = http.GET();              //Send the request
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
         String payload = http.getString();    //Get the response payload
       #ifdef SERIAL_DEBUG
         Serial_Log.println(httpCode);   //Print HTTP return code
         Serial_Log.println(payload);    //Print request response payload
      #endif
     } else {
       #ifdef SERIAL_DEBUG
        Serial_Log.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      #endif
     }
      http.end();  //Close connection
    } else {
    #ifdef SERIAL_DEBUG
      Serial_Log.printf("[HTTP] Unable to connect\n");
    #endif
    }
  }
}

void httpClient::setup() {
}

void httpClient::loop() {
  HttpPostSpeed();
  delay(eeprom_settings.tx_23_settings.uiMiddlewareDelayTime);
}
