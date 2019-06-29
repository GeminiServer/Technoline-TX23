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


/*************************************************************************************************
  TX23.h - Library for reading LaCrosse TX23 anemometer data.

  LaCrosse TX23 is a wind speed and direction sensor. It uses 3 wires for communication and power:
  Pin1	Brown(Black)	DATA
  Pin2	Red(Pink)			Vcc
  Pin3	Green					N/C
  Pin4	Yellow				GND

  DATA pin is to be connected directly to one of Arduino ports.

  Created by Krzysztof Egzmont, 06.11.2016.
  Based on https://www.john.geek.nz/2012/08/la-crosse-tx23u-anemometer-communication-protocol/
  Big thanks for John.

  Donations:
  Bitcoin - 1JrHQwJWt1JNZepT1EXqPSioXcwD227nnU
  Ethereum - 0xFC9713013e78f6F14bA5064E36f875dbbB32B94a

  BSD license, all text above must be included in any redistribution
*************************************************************************************************/

#include <tx23.h>
#include "main.h"

TX23 tx_23;

void TX23::setPin(int pin= -1)
{
	pinMode(pin, INPUT);
	_pin = pin;
}

void TX23::pullBits(void *dst, bool *src, int count)
{
	uint8_t * d = (uint8_t*) dst;
	for (int i=0; i<count; i++) {
		d[i/8] |= src[i]<<(i%8);
	}
}

void TX23::read() {
	if ( _pin > -1) {
		speed = direction = 0;
	  digitalWrite(_pin,LOW);
		pinMode(_pin,OUTPUT);
		delay(500);
		pinMode(_pin,INPUT);
		pulseIn(_pin,LOW);

		unsigned bitLen = 1200;

		bool data[50];
		bool lastState = 1;
		unsigned long start = micros();
		for(unsigned long t = 0; t<50000; t = micros()-start)
		{
			bool state = digitalRead(_pin);
			unsigned bitNum = t/bitLen;
			if(t%bitLen>bitLen/2) data[bitNum] = state;
			if(state!=lastState)
			{
				unsigned delta = t%bitLen;
				if(delta<100) start -= delta;
				else if(delta>900) start += delta;
				lastState = state;
			}
		}

		uint8_t ctr		= 0; 	pullBits(	&ctr,data+0,5		 );
		uint8_t dir 	= 0; 	pullBits(	&dir,data+5,4		 );
		uint16_t spd 	= 0;	pullBits(	&spd,data+9,12	 );
		uint8_t sum 	= 0; 	pullBits(	&sum,data+21,4	 );
		uint8_t ndir 	= 0;	pullBits(	&ndir,data+25,4	 ); ndir ^= 0x0f;
		uint16_t nspd = 0; 	pullBits(	&nspd,data+29,12 ); nspd ^= 0x0fff;

		const uint8_t csum 	= 0x0f & (dir + (spd&0x0f) + ((spd>>4)&0x0f) + ((spd>>8)&0x0f));

		if(ctr!=27 || csum!=sum || spd!=nspd || dir!=ndir ) {
			bReadState= false;
			if(uiErrCnt++ > 10 ) bLastReadState= bReadState;
			sLastError= "Error Reading TX23.";
			delay(4000); // Some thing goes wrong. Let's wait here a little bit.
		} else {
			speed = spd/10.0;
			direction = dir;
			bReadState= true;
			bLastReadState=bReadState;
			uiErrCnt=0;
			delay(2000); // Delay must be between 2 - 4 Seconds before starting next read request.
									 // Otherwise wind speed will read 0.
		}
	} else sLastError= "TX23 pin ist not Set/Defined!";
}

void TX23::ReadAnemometer()
{
  sSpeed= "Speed: " + String(speed,1) + " m/s" + " (" + String(speed*3.6,1)+ " km/h)";
  sDirection= "Direction: " + dirTable[direction];
}

void TX23::loop() {
 read();
 if( bReadState ) ReadAnemometer();
 if( bLastReadState ) {
	 Serial_Log.println(sSpeed);
 	 Serial_Log.println(sDirection);
 } else {
	 Serial_Log.println(sLastError);
 }
}
