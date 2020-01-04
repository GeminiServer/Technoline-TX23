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
#include "settings.h"

void BlinkTask::setup() {
  state = HIGH;
  pinMode(LED_INDICATION_PIN, OUTPUT);
  pinMode(LED_INDICATION_PIN, BlinkTask::state);
}

void BlinkTask::loop() {
  BlinkTask::state = BlinkTask::state == HIGH ? LOW : HIGH;
  pinMode(LED_INDICATION_PIN, BlinkTask::state);
  delay(50);
}

void WifiSignal::loop() {
  WifiSignal::rssi = (WiFi.status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
#ifdef SERIAL_DEBUG
  Serial_Log.print("WiFi Signal Strength: ");
  Serial_Log.print(rssi);
  Serial_Log.println("dBm");
#endif
  delay(10000);
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
