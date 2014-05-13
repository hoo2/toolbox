 /*

	Sensors.c - Is the sensors input resistance to temperature, lux etx translator
      API file, for various sensor types.
   includes:
      J-type Thermocouple, PT100, PT1000, NTC10k (B25/85: 3977K),
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
   Date:       8/2011
   Version:    0.1
 */

#include <drv/sensors_lut.h>

/* 
 * =====================    Functions    ==========================
 */

/*!
 * \brief
 *    This function calculates the Sensor Value from the given
 *    measured value using a negative coefficient LUT and a first
 *    order linear interpolation equation.
 *                         v - F[i-1]
 *          T = T[i] -  ---------------- * (F[i] - F[i-1])
 *                        F[i] - F[i-1]
 *          example:
 *                    Rt - R10
 *          T = 10 - ---------- * (20-10)
 *                    R20 - R10
 *          Use this for NTC Thermistors etc..
 *
 * \param  v   The measured value
 * \param  F   Pointer to the LUT Sensor FROM array
 * \param  T   Pointer to the LUT Sensor TO array
 * \retval     The converted sensor value
 */
float sen_nclut (float v, const lut_t *F, const float *T)
{
   int t = T[0], i = 0, j;

   //Find end point
   for (j=0 ; T[j]!= INT_MAX ; ++j)
      ;
   //Boundary checking
   if (v < F[j-1] || v > F[0])
      return TEMP_ERROR;
   //First match (for now)
   while (v <= F[i])
      t=T[i++];
   return (t + ((F[i-1] - v) / (F[i-1] - F[i])) * (T[i] - T[i-1]) );
}


/*!
 * \brief
 *    This function calculates the Sensor Value from the given
 *    measured value using a positive coefficient LUT and a first
 *    order linear interpolation equation.
 *                         v - F[i-1]
 *          T = T[i] -  ---------------- * (F[i] - F[i-1])
 *                        F[i] - F[i-1]
 *          example:
 *                    Rt - R10
 *          T = 10 - ---------- * (20-10)
 *                    R20 - R10
 *          Use this for PT100, PT1000, PTC thermistors etc..
 *
 * \param  v   The measured value
 * \param  F   Pointer to the LUT Sensor FROM array
 * \param  T   Pointer to the LUT Sensor TO array
 * \retval     The converted sensor value
 */
float sen_pclut (float v, const lut_t *F, const float *T)
{
   int i = 0, j, t =0;

   //Find end point
   for (j=0 ; T[j]!= INT_MAX ; ++j)
      ;
   //Boundary checking
   if (v < F[0] || v > F[j-1])
      return TEMP_ERROR;
   //First match (for now)
   while (v >= F[i])
      t=T[i++];
   return (t + ((v - F[i-1]) / (F[i] - F[i-1])) * (T[i] - T[i-1]) );
}




