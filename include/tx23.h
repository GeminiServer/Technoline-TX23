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

#include <Scheduler.h>
#include "language.h"

static const float dirTableDegrees[]                      = { 0, 22.5, 45, 67.5,90, 112.5,135,157.5,
                                                              180,202.5,225,247.5, 270,292.5,315,337.5 };
static const uint8 dirBeatFortNumberRange                 = 12;  // Bgt. range is from 0-12
static const uint8 dirBeatFortNumberSpeedRangeKMh[]       = { 1,5,11,19,28,38,49,61,74,88,102,117,118 };  // Range is in kmh. I.e. Bft '5' is in range of 29-38 km/h!

#ifdef LANGUAGE_GER
  static const String dirTable[]                          = { "N"   ,"NNO" ,"NO"  ,"ONO" ,"O"   ,
                                                              "OSO" ,"SO"  ,"SSO" ,"S"   ,"SSW" ,
                                                              "SW"  ,"WSW" ,"W"   ,"WNW" ,"NW"  ,
                                                              "NNW" };

  static const String dirTableLong[]                      = { "Nord" ,"Nord Nord Ost"  ,"Nord Ost"  ,"Ost Nord Ost"  ,
                                                              "Ost"  ,"Ost Süd Ost"    ,"Süd Ost"   ,"Süd Süd Ost"   ,
                                                              "Süd"  ,"Süd Süd West"   ,"Süd West"  ,"West Süd West" ,
                                                              "West" ,"West Nord West" ,"Nord West" ,"Nord Nord West" };

  static const String dirBeatFortNumberDescription[]    = { "Windstille"      ,"leiser Zug"   ,"leichte Brise"  ,"schwacher Wind"    ,
                                                            "mäßiger Wind"    ,"frischer Wind","starker Wind"   ,"streifer Wind"     ,
                                                            "stürmischer Wind","Sturm"        ,"schwerer Sturm" ,"orkanartiger Sturm",
                                                            "Orkan" };
    #else
    static const String dirTable[]                        = { "N"  ,"NNE" ,"NE" ,"ENE" ,"E"  ,"ESE",
                                                              "SE" ,"SSE" ,"S"  ,"SSW" ,"SW" ,"WSW",
                                                              "W"  ,"WNW" ,"NW" ,"NNW" };

    static const String dirTableLong[]                    = { "North" ,"North North East" ,"North East" ,"East North East",
                                                              "East"  ,"East South East"  ,"South East" ,"South South East",
                                                              "South" ,"South South West" ,"South West" ,"West South West",
                                                              "West"  ,"West North West"  ,"North West" ,"North North West" };

    static const String dirBeatFortNumberDescription[]    = { "Calm"            ,"Light Air"    ,"Light Breeze"  ,"Gentle Breeze",
                                                              "Moderate Breeze" ,"Fresh Breeze" ,"Strong Breeze" ,"Moderate Gale",
                                                              "Fresh Gale"      ,"Strong Gale"  ,"Whole Gale"    ,"Storm"        ,
                                                              "Huricane Force" };
#endif

class TX23 : public Task {
  public:
    void setPin(int pin);               //init pin number with DATA wire connected
    void read();                        //reads sensor data, returns true if success
    String sSpeed, sDirection,sBft;
    String sRawSpeedKMh, sRawSpeedMs, sRawSpeedMi, sRawSpeedKn, sRawSpeedFt;
    String sRawDirection, sRawDirectionLong;
    float fRawDirection;
    int iReadDirection;
    int iDirection;
    uint16 uiSpeedRaw;
    bool bReadState, bLastReadState;
    uint uiErrCnt;
    String sLastError;
    void ReadAnemometer();
    uint8 DirectionOffset(uint8 iDirection);
    uint8 uiBeatFortNumber;
    String sBeatForNumberDescription;
  private:
    int _pin;
    static void pullBits(void *dst, bool *src, int count);
    float speed;
    int direction;
    uint8 GetBftNumber(float fSpeedKMh);
  protected:
    void loop();
};
