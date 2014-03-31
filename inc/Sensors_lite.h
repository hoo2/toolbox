/*

	Sensors_lite.h - Is the sensors input resistance to temperature, lux etx translator
      API header file, for various sensor types.
   includes:
      PT100, PT1000, NTC10k (B25/85: 3977K), PerkinElmer A9060 11 Photocell

   Copyright (C) 2011 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       8/2012
   Version:    0.1
 */

#ifndef __Sensors_lite_h__
#define __Sensors_lite_h__

#include <math.h>
#include <limits.h>
#include <stdint.h>


/* ================        General         ======================*/

// Local type Definition
typedef  float          SensorValue_t;
typedef  float          Measurement_t;
typedef  float          FROM_RefLUT_t;
typedef    int          TO_RefLUT_t;

typedef  float          ResLUT_t;
typedef  float          Resistance_t;

typedef    int          TempLUT_t;
typedef  float          Temperature_t;


// Extern reference at each Driver file
typedef int16_t  adc_t;
extern adc_t Temp_GetIntTemp (void);


/* ================    Static Functions     ======================*/
//static SensorValue_t Get_NC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T);
static SensorValue_t Get_PC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T);


/* ================   Exported Functions    ======================*/

Temperature_t SENSOR_Pt100_Res2Temp (Resistance_t r);
Temperature_t SENSOR_Pt1000_Res2Temp (Resistance_t r);
Temperature_t SENSOR_KTY81_122_Res2Temp (Resistance_t r);

#endif //#ifndef __Sensors_lite_h__

