				 /*

	Sensors_lite.c - Is the sensors input resistance to temperature, lux etx translator
      API file, for various sensor types.
   includes:
      PT100, PT1000, NTC10k (B25/85: 3977K),
      PerkinElmer A9060 11 Photocell

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

#include "Sensors_lite.h"


/* 
 * =================================================================================   
 *                          Sensor Lookup Tables
 * =================================================================================
 */


//     PT100 and PT1000 Resistance LUT with the temperature correlation
//   ------------------------------------------------------------------------
static ResLUT_t const PT100x_RES[] =
{
   18.52,   39.72,   60.26,   80.31,      // -200 ~ -50 
   100.00,  119.40,  138.51,  157.33,     //    0 ~ 150
   175.86,  194.10,  212.05,  229.72,     //  200 ~  350
   247.09,  264.18,  280.98,  297.49,     //  400 ~  550
   313.71                                 // 600
};
static TempLUT_t const PT100x_TEMP[]=
{
   -200, -150, -100,  -50,
      0,   50,  100,  150,
    200,  250,  300,  350,
    400,  450,  500,  550,
    600, INT_MAX
};


//     KY81-122 PTC Thermistor Resistance LUT and temperature correlation
//   ----------------------------------------------------------------------------
static ResLUT_t const KTY81_122_RES[] = 
{
   495, 520, 573, 630, 690, 755, 823, 895, 971, 1010,
   1050, 1134, 1221, 1312, 1406, 1505, 1607, 1713, 1823, 1934,
   1989, 2044, 2146, 2233
};
static TempLUT_t const KTY81_122_TEMP[] =
{
   -55, -50, -40, -30, -20, -10,   0,  10,  20,  25,
    30,  40,  50,  60,  70,  80,  90, 100, 110, 120,
   130, 140, 150, INT_MAX
};

/* 
 * =================================================================================   
 *                                Static Functions
 * =================================================================================
 */

/*
   SensorValue_t
   Get_PC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T)

  * @brief  This function calculates the Sensor Value
            from the given measured value using a Positive
            coefficient LUT and a first order linear
            interpolation equation
                           v - F[i-1]
            T = T[i] -  ---------------- * (F[i] - F[i-1]) 
                          F[i] - F[i-1]
            example:
                      Rt - R10
            T = 10 - ---------- * (20-10)
                      R20 - R10
            Use this for PT100, PT1000, PTC thermistors etc..

  * @param  Measurement_t v  : The measured value
            FROM_RefLUT_t *V : The LUT Sensor FROM values
            TO_RefLUT_t *S   : The LUT Sensor TO values
  * @retval SensorValue_t    : Sensor return value     
 */
static SensorValue_t
Get_PC_SensorValue (Measurement_t v, const FROM_RefLUT_t * F, const TO_RefLUT_t * T)
{
   int i = 0, j, t =0;

   //Find end point
   for (j=0 ; T[j]!= INT_MAX ; ++j);

   //Boundary cheking
   if (v < F[0] || v > F[j-1])  return FP_NAN;

   //First match (for now)
   while (v >= F[i])
      t=T[i++];

   return (t + ((v - F[i-1]) / (F[i] - F[i-1])) * (T[i] - T[i-1]) );
}


/* 
 * =================================================================================   
 *                      Temperature Sensor Exported Functions
 * =================================================================================
 */



/*
   Temperature_t SENS_xxxxx_Res2Temp(Resistance_t r)

  * @brief  These functions calculate the temperature
            from the resistance of a sensor using
            Get_PC_SensorValue () or Get_NC_SensorValue ()
  * @param  Resistance_t r : The resistance in Ohm
  * @retval Temperature_t Temperature in 'C     
 */
Temperature_t SENSOR_Pt100_Res2Temp (Resistance_t r)
{
    return (Temperature_t)Get_PC_SensorValue(r, PT100x_RES, PT100x_TEMP);
}

Temperature_t SENSOR_Pt1000_Res2Temp (Resistance_t r)
{
    return (Temperature_t)Get_PC_SensorValue(r/10, PT100x_RES, PT100x_TEMP);
}

Temperature_t SENSOR_KTY81_122_Res2Temp (Resistance_t r)
{
    return (Temperature_t)Get_PC_SensorValue(r, KTY81_122_RES, KTY81_122_TEMP);
}


