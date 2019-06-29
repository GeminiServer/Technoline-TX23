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
#include "httpServer.h"
#include "tx23.h"
#include "main.h"

extern TX23 tx_23;

ESP8266WebServer server(80); //Server on port 80
WebSocketsServer webSocket = WebSocketsServer(81);

void handleRoot() {
     server.send(200, "text/html", "<html>\
      <head><script>\
      var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);\
      connection.onopen = function () {\
      connection.send('Connect ' + new Date())\
      };\
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
         para.innerText = datestring +' --> '+message; \
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
    <h3>Current Wind Sensor Information:</h3> \
    <h4 id = 'output'></h4>\
    </body> \
    </html>");

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
      Serial_Log.printf("[%u] Disconnected!\n", num);
    } break;

    case WStype_CONNECTED: {

      IPAddress ip = webSocket.remoteIP(num);
      Serial_Log.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      webSocket.sendTXT(num, "Connected");  // Send message to client
    } break;

    case WStype_TEXT:{
    Serial_Log.printf("[%u] get Text: %s\n", num, payload);
    } break;

    default: {
      String sMsg= tx_23.bLastReadState ? tx_23.sSpeed +" - " + tx_23.sDirection : tx_23.sLastError;
      webSocket.sendTXT( 0 , sMsg );
    } break;
  }
}

void httpServer::setup() {
  server.on("/", handleRoot);         // Routine to handle at root location

  webSocket.begin();                  // Start webSocket server
  webSocket.onEvent(webSocketEvent);

  server.begin();                     // Start server
  Serial_Log.println("HTTP server started");
  if(MDNS.begin("Technoline-TX23")) {
    Serial_Log.println("MDNS responder started");
  }

  MDNS.addService("http", "tcp", 80); // Add service to MDNS
  MDNS.addService("ws", "tcp", 81);
}

void httpServer::loop() {

  webSocket.loop();
  server.handleClient();

  ulong t = millis();
  ulong last_sec = 0;
  ulong max_sec=  1000; // ms

  if((t - last_sec) > max_sec) {
    counter++;
    bool ping = (counter % 2);
    int i = webSocket.connectedClients(ping);
    //Serial_Log.printf("%d Connected websocket clients ping: %d\n", i, ping);
    last_sec = millis();
  }
}
