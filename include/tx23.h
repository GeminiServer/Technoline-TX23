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

#include "Arduino.h"
#include <Scheduler.h>

static const String dirTable[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE","S","SSW","SW","WSW","W","WNW","NW","NNW"};
/*
static const String dirTableLongEN[]= { "North","North North East","North East","East North East","East","East South East",
																				"South East","South South East","South","South South West","South West",
																				"West South West","West","West North West","North West","North North West" };
*/
//static const uint16 dirTableDegrees[]= {0 /*== 360!*/,22.5,45,67.5,90,112.5,135,157.5,180,202.5,225,247.5,270,292.5,315,337.5};


class TX23 : public Task {
	public:
		//init pin number with DATA wire connected
		void setPin(int pin);
		//reads sensor data, returns true if success
		void read();
		String sSpeed;
	  String sDirection;
		bool bReadState, bLastReadState;
		uint uiErrCnt;
		String sLastError;
		void ReadAnemometer();
	private:
		int _pin;
		static void pullBits(void *dst, bool *src, int count);
		float speed;
		int direction;

	protected:
	    void loop();
};
