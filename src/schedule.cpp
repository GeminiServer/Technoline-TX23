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
#include <schedule.h>
#include <ESP8266WiFi.h>
#include "main.h"
#include "settings.h"

// const int led= 2;  --> Defined in main.h !

void BlinkTask::setup() {
  state = HIGH;
  pinMode(led, OUTPUT);
  pinMode(led, BlinkTask::state);
}

void BlinkTask::loop() {
  BlinkTask::state = BlinkTask::state == HIGH ? LOW : HIGH;
  pinMode(led, BlinkTask::state);
  delay(50);
}

void WifiSignal::loop() {
  WifiSignal::rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
#ifdef SERIAL_DEBUG
  Serial.print("WiFi Signal Strength: ");
  Serial.print(rssi);
  Serial.println("dBm");
  delay(10000);
#endif
}

void MemTask::loop() {
#ifdef SERIAL_DEBUG
  Serial_Log.print("Free Heap: ");
  Serial_Log.print(ESP.getFreeHeap());
  Serial_Log.print(" bytes");
  Serial_Log.print(" - CpuFreq: ");
  Serial_Log.print(ESP.getCpuFreqMHz());
  Serial_Log.println(" MHz");
  delay(10000);
#endif
}
