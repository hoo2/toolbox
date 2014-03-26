 /*

	Sensors.h - Is the sensors input resistance to temperature, lux etx translator
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
   Date:       8/2011
   Version:    0.1
 */

#ifndef __Sensors_h__
#define __Sensors_h__

#include <math.h>
#include <limits.h>
#include <stdint.h>


/* ================        General         ======================*/

#define  TEMP_ERROR    (-274.0) // Bellow Absolute zero


// Local type Definition
typedef  float          SensorValue_t;
typedef  float          Measurement_t;
typedef  float          FROM_RefLUT_t;
typedef    int          TO_RefLUT_t;

typedef  float          mVLUT_t;
typedef  float          mV_t;

typedef  float          ResLUT_t;
typedef  float          Res_t;

typedef    int          TempLUT_t;
typedef  float          Temp_t;

typedef    int          LuxLUT_t;
typedef  float          Lux_t;


// Extern reference at each Driver file
typedef int16_t  adc_t;
extern adc_t Temp_GetIntTemp (void);


/* ================   Exported Functions    ======================*/
Temp_t SEN_J_Type_mV2Temp (int Vadc, float g, int Vrefadc, float Vref);

Temp_t SEN_PT100 (Res_t r);
Temp_t SEN_PT1000 (Res_t r);
Temp_t SEN_KTY81_121 (Res_t r);
Temp_t SEN_KTY81_122 (Res_t r);
Temp_t SEN_KTY11_6 (Res_t r);
Temp_t SEN_NTC10k (Res_t r);

Lux_t SENS_A906011_Res2Lux(Res_t r);

#endif //#ifndef __Sensors_h__

