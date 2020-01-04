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

// Defines output string LANG. Ucomment for EN, which is default.
//
#define LANGUAGE_GER

#ifdef LANGUAGE_GER
  #define L_SPEED         "Geschwindigkeit"
  #define L_DIRECTION     "Richtung"
  #define L_BEAUFORTSCALE "Beaufortskala"
  #define L_LONG          "Lang"
  #define L_DEGREES       "Grad"
  #define L_MILES         "Meilen"
  #define L_KNOTE         "knoten"
  #define L_FOOT          "Fuß"

#else  // Default EN

  #define L_SPEED         "Speed"
  #define L_DIRECTION     "Direction"
  #define L_BEAUFORTSCALE "Beaufortscale"
  #define L_LONG          "Long"
  #define L_DEGREES       "Degrees"
  #define L_MILES         "Miles"
  #define L_KNOTE         "knote"
  #define L_FOOT          "Foot"
#endif
