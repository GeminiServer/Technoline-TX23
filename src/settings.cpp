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
#include <EEPROM.h>
#include "settings.h"

void Settings::Setup()
{
  #ifdef SERIAL_DEBUG
    Serial_Log.println("EEPROM Startup: Reading Settings...");
  #endif
  read(tx_23_settings);
  if(String(tx_23_settings.cEepromVersion).compareTo(TX23_EEPROM_VERSION) != 0) {
    #ifdef SERIAL_DEBUG
      Serial_Log.print("EEPROM Version missmatch detected!!! Version is: v");
      Serial_Log.print(tx_23_settings.cEepromVersion);
      Serial_Log.print(" - Should be: v"); Serial_Log.println(TX23_EEPROM_VERSION);
      Serial_Log.println("EEPROM Clearing and writing default values.");
    #endif
    ClearEEPROM();
    sprintf(tx_23_settings.cEepromVersion, TX23_EEPROM_VERSION);

    tx_23_settings.uiOffset_Direction     = OFFSET_DIRECTION;
    tx_23_settings.iOffset_WindSpeed      = OFFSET_SPEED;
    tx_23_settings.bMiddlewareEnabled     = true;
    tx_23_settings.uiMiddlewareDelayTime  = MIDDLEWARE_DELAY_TIME;
    tx_23_settings.bEepromWrite           = false;

    sprintf(tx_23_settings.cMiddleware,MIDDLEWARE_URL);
    sprintf(tx_23_settings.cMiddlewareOperation, MIDDLEWARE_OPERATION);

    write(tx_23_settings);
  }
  #ifdef SERIAL_DEBUG
    else {
        Serial_Log.print("EEPROM Version is: v"); Serial_Log.println(tx_23_settings.cEepromVersion);
    }
    Serial_Log.println("EEPROM Startup: Done. ");
    Serial_Log.println("################################################");
  #endif
}

bool Settings::write(TX_23Settings &tx_23_Settings) {
  EEPROM.begin(sizeof(TX_23Settings));
  #ifdef SERIAL_DEBUG
    Serial_Log.print("EEPROM - Writing....");
  #endif
  EEPROM.put(EEPROM_START_ADRESS, tx_23_Settings);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure
  #ifdef SERIAL_DEBUG
    Serial_Log.println("DONE!");
  #endif
  tx_23_Settings.bEepromWrite=false;
  return true;
}
bool Settings::read(TX_23Settings &tx_23_Settings) {
  EEPROM.begin(sizeof(TX_23Settings));
  EEPROM.get(EEPROM_START_ADRESS,tx_23_Settings);
  EEPROM.end();
  return true;
}

void Settings::ClearEEPROM() {
  EEPROM.begin(sizeof(TX_23Settings));
  for (uint i = EEPROM_START_ADRESS; i < sizeof(TX_23Settings); i++) EEPROM.write(i, 0);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  delay(200);
  EEPROM.end();                         // Free RAM copy of structure
}

// Uptime - Makes a count of the total up time since last start
void Settings::Uptime() {
  if(millis() <= 100000 && (sys_uptime.bHighMillis= millis() >= 3000000000))
  {
    sys_uptime.iRollover++; 
    sys_uptime.bHighMillis= false;
  }
  uint32_t secsUp = millis()/1000;
  sys_uptime.iSecond = secsUp % 60;
  sys_uptime.iMinute = (secsUp/60) % 60;
  sys_uptime.iHour   = (secsUp/(60*60)) % 24;
  sys_uptime.lDay    = (sys_uptime.iRollover*50)+(secsUp/(60*60*24));
  sys_uptime.sUptime =  String(sys_uptime.lDay)    +  (sys_uptime.lDay > 1 ?     " Days "    : " Day ")      +
                        String(sys_uptime.iHour)   +  (sys_uptime.iHour > 1 ?    " Hours "    : " Hour ")    + 
                        String(sys_uptime.iMinute) +  (sys_uptime.iMinute > 1 ?  " Minutes "  : " Minute ")  + 
                        String(sys_uptime.iSecond) +  (sys_uptime.iSecond > 1 ?  " Seconds."  : " Second.");
}
