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
#include "main.h"

const int led= 16;

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


void MemTask::loop() {
  Serial_Log.print("Free Heap: ");
  Serial_Log.print(ESP.getFreeHeap());
  Serial_Log.println(" bytes");
#ifdef SERIAL_DEBUG
  delay(10000);
#endif
}
