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
#include <esp.h>
#include "httpServer.h"
#include "tx23.h"
#include "main.h"
#include "settings.h"
#include "schedule.h"

extern TX23 tx_23;
extern Task task_info;

ESP8266WebServer server(WEBSERVER_PORT); //Server on port 80
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);

void handleRoot() {
     server.send(200, "text/html", "<html>\
      <head> \
      <style> \
        body { background-color:#555888; font-family:'Roboto'; color:#fff; text-align:center } \
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
         para.innerText = 'Time: ' + datestring + message; \
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
    <button type='button' id='getNewData' onclick='ConnectionSendGet()'>Get Data</button> \
    </body> \
    </html>");

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED: {
    #ifdef SERIAL_DEBUG
      Serial_Log.printf("[%u] Disconnected!\n", num);
    #endif
    } break;

    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      #ifdef SERIAL_DEBUG
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
          int32_t rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
          String sMsgTx= tx_23.bLastReadState ? tx_23.sSpeed +"\n" + tx_23.sDirection : tx_23.sLastError;
          String sMsg= "\n" + sMsgTx +"\n\n"+"WiFi Signal: "+String(rssi).c_str() + " dBm";
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
          int32_t rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
          webSocket.sendTXT(num,String(rssi).c_str());
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

  webSocket.begin();                  // Start webSocket server
  webSocket.onEvent(webSocketEvent);

  server.begin();                     // Start server
#ifdef SERIAL_DEBUG
  Serial_Log.println("HTTP server started");
#endif
  if(MDNS.begin("Technoline-TX23")) {
#ifdef SERIAL_DEBUG
    Serial_Log.println("MDNS responder started");
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
    counter++;
    bool ping = (counter % 2);
    int i = webSocket.connectedClients(ping);
    //Serial_Log.printf("%d Connected websocket clients ping: %d\n", i, ping);
    last_sec = millis();
  }

}
