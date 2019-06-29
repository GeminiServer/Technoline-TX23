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

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include "schedule.h"
#include "tx23.h"
#include "httpServer.h"
#include "main.h"


extern TX23 tx_23;

BlinkTask blink_task;
httpServer http_server;
#ifdef SERIAL_DEBUG
  MemTask mem_task;
#endif

#define STASSID "YOUR_AP_WIFI_SSID"
#define STAPSK  "password"

const char* ssid     = STASSID;
const char* password = STAPSK;

const uint16_t port = 17;
const int led= 16;

void setup() {
  pinMode(led, OUTPUT);
  Serial_Log.begin(115200);
  Serial_Log.println();
  Serial_Log.println();
  Serial_Log.print("Connecting to ");
  Serial_Log.println(ssid);
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial_Log.print("BOOTING");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial_Log.print(".");
  }

  Serial_Log.println("");
  Serial_Log.println("WiFi connected");
  Serial_Log.println("IP address: ");
  Serial_Log.println(WiFi.localIP());

  Scheduler.start(&http_server);   // Start http and webSocket server in scheduler
  Scheduler.start(&blink_task);    // Start led blind in scheduler
  Scheduler.start(&mem_task);      // Start mem info in scheduler

  tx_23.setPin(12);                // DATA wire connected to GPIO port 12
  Scheduler.start(&tx_23);         // Start reading tx23 data in scheduler

  Scheduler.begin();
}

void loop() {}                     // Nothin to do here, since we are in schedule mode!
