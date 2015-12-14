/*!
 * \file ntc3997k.c
 * \brief
 *    A target independent ntc3997k sensor function using look-up table
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <drv/ntc3997k.h>

/*
 * ========== Sensor Lookup Tables  ===========
 */
static lut_t const NTC10k_RES[] =
{
   332100, 240000, 175200, 129300, 96360, 72500, 55050, 42160, 32560, 25340,
    19870,  15700,  12490,  10000,  8059,  6535,  5330,  4372,  3606,  2989,
     2490,   2084,   1753,   1481,  1256,  1070, 915.4, 786.0, 677.3, 585.8,
    508.3,  442.6,  386.6,  338.7, 297.7, 262.4, 231.9, 205.5, 182.6
};
static temp_t const NTC10k_TEMP[] =
{
   -40, -35, -30, -25, -20, -15, -10,  -5,   0,   5,
    10,  15,  20,  25,  30,  35,  40,  45,  50,  55,
    60,  65,  70,  75,  80,  85,  90,  95, 100, 105,
   110, 115, 120, 125, 130, 135, 140, 145, 150, INT_MAX
};


/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    kty8x-121 conversion function
 * \param  r   Sensor's resistor value
 * \return temperature result
 */
inline temp_t sen_ntc3997k (res_t r) {
   return (temp_t)sen_nclut(r, NTC10k_RES, NTC10k_TEMP);
}
