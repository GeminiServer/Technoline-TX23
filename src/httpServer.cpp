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

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <esp.h>
#include "httpServer.h"
#include "tx23.h"
#include "settings.h"
#include "schedule.h"

extern TX23 tx_23;
extern Task task_info;
extern Settings eeprom_settings;

ESP8266WebServer server(WEBSERVER_PORT); //Server on port 80
ESP8266HTTPUpdateServer httpUpdater;
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

void handleRoot() {
     server.send(200, "text/html", "<html>\
      <head> \
      <style> \
        body { background-color:#558865; font-family:'Roboto'; color:#fff; text-align:center } \
        code { display:inline-block; max-width:600px;  padding:80px 0 0; line-height:1.5; font-family:monospace; color:#ccc } \
      </style> \
      <script>\
      var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);\
      connection.onopen = function () {\
        connection.send('Connect ' + new Date())\
      };\
      function ConnectionSendGet() { \
        connection.send('get'); \
        setTimeout(ConnectionSendGet, 1000); \
      } \
      function writeToScreen(message) { \
        var para = document.querySelector('#output span'); \
        if (!para || para.length == 0) { \
          para = document.createElement('span'); \
        } \
         var pre = document.createElement('p'); \
         var vDate = new Date(); \
         var datestring = ('0' +    vDate.getDate()).slice(-2)  + '-' +\
                          ('0' +(vDate.getMonth()+1)).slice(-2) + '-' +\
                           vDate.getFullYear()                  + ' ' +\
                          ('0' +   vDate.getHours()).slice(-2) + ':' + \
                          ('0' + vDate.getMinutes()).slice(-2) + ':' + \
                          ('0' + vDate.getSeconds()).slice(-2); \
         para.innerText = 'Zeit: ' + datestring + message; \
         document.getElementById('output').appendChild(para); \
      };\
      connection.onerror = function (error) { console.log('WebSocket Error ', error);};\
      connection.onmessage = function (e) { \
        console.log('Server: ', e.data);\
        writeToScreen(e.data);\
      };\
      </script></head> \
    <body> \
    <h2>Technoline TX23 - Wind Sensor</h2> \
    <h3>Current Wind Sensor Information</h3> \
    <h4 id = 'output'></h4>\
    <button type='button' id='getNewData' onclick='ConnectionSendGet()'>Daten auslesen</button> \
    <button type='button' id='getHelp' onclick=window.open('help','_blank')>Hilfe</button> \
    </body> \
    </html>");
}

void httpServer::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
    #ifdef SERIAL_DEBUG
      Serial_Log.printf("[%u] Disconnected!\n", num);
    #endif
    } break;

    case WStype_CONNECTED: {
      #ifdef SERIAL_DEBUG
        IPAddress ip = webSocket.remoteIP(num);
        Serial_Log.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      #endif
      webSocket.sendTXT(num, "\nWebSocket: Connected");  // Send message to client
    } break;

    case WStype_TEXT:{
      #ifdef SERIAL_DEBUG
        Serial_Log.printf("[%u] get Text: %s\n", num, String((char*) payload).c_str());
      #endif
      if (payload) {
        if( String((char*) payload).equals( "get") /*0 is equal*/) {
          String sMsgTx= tx_23.bLastReadState ? tx_23.sSpeed +"\n" + tx_23.sDirection +"\n" +tx_23.sBft : tx_23.sLastError;
          String sMsg= "\n\n" + sMsgTx +
                       "\n\n" + "WiFi signal: "   + String(eeprom_settings.wifi_info.iRSSI).c_str() + " dBm" +
                       "\n"   + "System uptime: " + eeprom_settings.sys_uptime.sUptime + 
                       "\n"   + "Version: v" + TX23_VERSION + " (eeprom version: "+TX23_EEPROM_VERSION+")";
          webSocket.sendTXT(num, sMsg);
        }
        if( String((char*) payload).equals( "restart") /*0 is equal*/) {
          ESP.restart();
        }
        if( String((char*) payload).equals( "speed") /*0 is equal*/) {
          webSocket.sendTXT(num, tx_23.bLastReadState ? tx_23.sRawSpeedKMh : tx_23.sLastError);
        }
        if( String((char*) payload).equals( "direction") /*0 is equal*/) {
          webSocket.sendTXT(num, tx_23.bLastReadState ? tx_23.sRawDirection : tx_23.sLastError);
        }
        if( String((char*) payload).equals( "rssi") /*0 is equal*/) {
          webSocket.sendTXT(num,String(eeprom_settings.wifi_info.iRSSI).c_str());
        }
        if( String((char*) payload).equals( "uptime") /*0 is equal*/) {
          webSocket.sendTXT(num,eeprom_settings.sys_uptime.sUptime);
        }
      }
    } break;

    default: {
      //String sMsg= tx_23.bLastReadState ? tx_23.sSpeed +" - " + tx_23.sDirection : tx_23.sLastError;
      //webSocket.sendTXT( num , sMsg );
    } break;
  }
}

void httpServer::setup() {
  server.on("/", handleRoot);         // Routine to handle at root location
  server.on("/direction", []()      { String content = tx_23.bLastReadState ? tx_23.sRawDirection : tx_23.sLastError; server.send(200, "text/html", content);   });
  server.on("/direction-lo", []()   { String content = tx_23.bLastReadState ? tx_23.sRawDirectionLong : tx_23.sLastError;  server.send(200, "text/html", content);   });
  server.on("/direction-deg", []()  { String content = tx_23.bLastReadState ? String(tx_23.fRawDirection).c_str() : tx_23.sLastError;  server.send(200, "text/html", content);   });
  server.on("/direction-raw", []()  { String content = tx_23.bLastReadState ? String(tx_23.iReadDirection).c_str() : tx_23.sLastError;  server.send(200, "text/html", content);   });
  server.on("/speed", []()          { String content = tx_23.bLastReadState ? tx_23.sRawSpeedMs : tx_23.sLastError;   server.send(200, "text/html", content);   });
  server.on("/speed-kmh", []()      { String content = tx_23.bLastReadState ? tx_23.sRawSpeedKMh : tx_23.sLastError;    server.send(200, "text/html", content);   });
  server.on("/speed-mi", []()       { String content = tx_23.bLastReadState ? tx_23.sRawSpeedMi : tx_23.sLastError;    server.send(200, "text/html", content);   });
  server.on("/speed-ft", []()       { String content = tx_23.bLastReadState ? tx_23.sRawSpeedFt : tx_23.sLastError;    server.send(200, "text/html", content);   });
  server.on("/speed-kn", []()       { String content = tx_23.bLastReadState ? tx_23.sRawSpeedKn : tx_23.sLastError;    server.send(200, "text/html", content);   });
  server.on("/rssi", []()           { String content = String(eeprom_settings.wifi_info.iRSSI).c_str();                server.send(200, "text/html", content);   });
  server.on("/uptime", []()         { String content = eeprom_settings.sys_uptime.sUptime;                             server.send(200, "text/html", content);   });

  server.on("/settings-get", []() {
    String content= "<!DOCTYPE html><html><head><style>table {font-family: arial, sans-serif;border-collapse:collapse;width:100%;}td,th{border:1px solid #dddddd;text-align: left;padding: 8px;} tr:nth-child(even) { background-color: #dddddd;}</style></head><body><h2>TX23 Current EEPROM settings</h2><table>";
    eeprom_settings.read(eeprom_settings.tx_23_settings);
    content += "<tr><th>middleware_url:  </th><td>" + String(eeprom_settings.tx_23_settings.cMiddleware )       +"</td></tr>";
    content += "<tr><th>middleware_op:   </th><td>" + String(eeprom_settings.tx_23_settings.cMiddlewareOperation)+"</td></tr>";
    content += "<tr><th>direction_offset:</th><td>" + String(eeprom_settings.tx_23_settings.uiOffset_Direction) +"</td></tr>";
    content +="</table></body></html>";
    server.send(200, "text/html", content);
    });

  server.on("/settings-set", []() {
       String content= "TX23 setting write ";
       if(server.hasArg("middleware_url") && server.arg("middleware_url").length() > 1 ) {
         sprintf(eeprom_settings.tx_23_settings.cMiddleware, server.arg("middleware_url").c_str());
         eeprom_settings.tx_23_settings.bEepromWrite= true;
         content += "" + server.arg("middleware_url");
       }
       if(server.hasArg("middleware_op") && server.arg("middleware_op").length() > 1 ) {
         sprintf(eeprom_settings.tx_23_settings.cMiddlewareOperation, server.arg("middleware_op").c_str());
         eeprom_settings.tx_23_settings.bEepromWrite= true;
         content += "" + server.arg("middleware_op");
       }
       if(server.hasArg("direction_offset") && server.arg("direction_offset").length() > 0 ) {
         eeprom_settings.tx_23_settings.uiOffset_Direction= server.arg("direction_offset").toInt();
         eeprom_settings.tx_23_settings.bEepromWrite= true;
         content += "" + server.arg("direction_offset").toInt();
       }

       if(eeprom_settings.tx_23_settings.bEepromWrite) {
         eeprom_settings.write(eeprom_settings.tx_23_settings);
         content += "  - Done!";
       } else content += " - ERROR!";


      server.send(200, "text/html", content);
    });

  server.on("/reset-eeprom", []() {
    for( uint8 i=0; i<sizeof(eeprom_settings.tx_23_settings.cEepromVersion); i++ )  {
      eeprom_settings.tx_23_settings.cEepromVersion[i]='x';
    }
    eeprom_settings.write(eeprom_settings.tx_23_settings);
    server.send(200, "text/html", "EEPROM Reseting to default values done. Please Reboot Device to take effekt.");
  });

  server.on("/clear-eeprom", []() {
    eeprom_settings.ClearEEPROM();
    server.send(200, "text/html", "EEPROM Delete Done!.");
  });

  server.on("/reboot", []() {
    server.send(200, "text/html", "Rebooting...");
    ESP.restart();
  });

  server.on("/help", []() {
    String content= "<!DOCTYPE html><html><head><style>table {font-family: arial, sans-serif;border-collapse:collapse;width:100%;}td,th{border:1px solid #dddddd;text-align: left;padding: 8px;} tr:nth-child(even) { background-color: #dddddd;}</style></head><body><h2>TX23 help</h2><table>";

    content+= "<tr><th><a href='/firmware      ' target='_blank'>/firmware     </a></th><td>Upload a new firmware via web interface </td></tr>";
    content+= "<tr><th><a href='/reboot        ' target='_blank'>/reboot       </a></th><td>!! Reboot the device</td></tr>";
    content+= "<tr><th><a href='/clear-eeprom  ' target='_blank'>/clear-eeprom </a></th><td>!! Clear the whole eeprom with zero</td></tr>";
    content+= "<tr><th><a href='/reset-eeprom  ' target='_blank'>/reset-eeprom </a></th><td>!! Reset the eeprom to default values</td></tr>";
    content+= "<tr><th><a href='/settings-set  ' target='_blank'>/settings-set </a></th><td>Set and save eeprom settings. I.e.: /setting-set?middleware_url=YOUR_URL or /setting-set?direction_offset=THE_OFFSET</td></tr>";
    content+= "<tr><th><a href='/settings-get  ' target='_blank'>/settings-get </a></th><td>Returns the current saved eeprom settings</td></tr>";
    content+= "<tr><th><a href='/direction     ' target='_blank'>/direction    </a></th><td>Plaintext return current direction in short description</td></tr>";
    content+= "<tr><th><a href='/direction-lo  ' target='_blank'>/direction-lo </a></th><td>Plaintext return current direction in long description</td></tr>";
    content+= "<tr><th><a href='/direction-deg ' target='_blank'>/direction-deg</a></th><td>Plaintext return current direction in degrees</td></tr>";
    content+= "<tr><th><a href='/direction-raw ' target='_blank'>/direction-raw</a></th><td>Plaintext return current direction in RAW (0-15). Where default is '0', which ist North! Set the offset to your Nort RAW! I.e. '8' which is the North alignment of your TX23.</td></tr>";
    content+= "<tr><th><a href='/speed         ' target='_blank'>/speed        </a></th><td>Plaintext return current speed in m/s</td></tr>";
    content+= "<tr><th><a href='/speed-kmh     ' target='_blank'>/speed-kmh    </a></th><td>Plaintext return current speed in km/h</td></tr>";
    content+= "<tr><th><a href='/speed-mi      ' target='_blank'>/speed-mi     </a></th><td>Plaintext return current speed in mph</td></tr>";
    content+= "<tr><th><a href='/speed-ft      ' target='_blank'>/speed-ft     </a></th><td>Plaintext return current speed in ft/s</td></tr>";
    content+= "<tr><th><a href='/speed-kn      ' target='_blank'>/speed-kn     </a></th><td>Plaintext return current speed in knot</td></tr>";
    content+= "<tr><th><a href='/rssi          ' target='_blank'>/rssi         </a></th><td>Plaintext return current WiFi rssi in dBm</td></tr>";
    content+= "<tr><th><a href='/uptime        ' target='_blank'>/uptime       </a></th><td>Plaintext return system system uptime</td></tr>";

    content +="</table></body></html>";
    server.send(200, "text/html", content);
  });

  webSocket.begin();                  // Start webSocket server
  webSocket.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) { this->webSocketEvent( num, type, payload, length);} );

  httpUpdater.setup(&server,UPDATE_HTTP_PATH,UPDATE_USER,UPDATE_PASSWD);
#ifdef SERIAL_DEBUG
  Serial_Log.println("# Firmware update server started");
#endif
  server.begin();                     // Start server
#ifdef SERIAL_DEBUG
  Serial_Log.println("# HTTP server started");
#endif
  if(MDNS.begin(HOST_NAME)) {
  #ifdef SERIAL_DEBUG
    Serial_Log.println("# MDNS responder started");
  #endif
  }

  MDNS.addService("http", "tcp", WEBSERVER_PORT); // Add service to MDNS
  MDNS.addService("ws", "tcp", WEBSOCKET_PORT);
}

void httpServer::loop() {
  webSocket.loop();
  server.handleClient();

  ulong t = millis();
  ulong last_sec = 0;
  ulong max_sec=  1000; // ms

  if((t - last_sec) > max_sec) {
    last_sec = millis();
    eeprom_settings.wifi_info.iRSSI = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : -1;
  }
}
