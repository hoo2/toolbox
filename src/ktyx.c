/*!
 * \file ktyx.c
 * \brief
 *    A target independent kty8x_121, kty8x_122 and kty11 sensor
 *    functions using look-up table.
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

#include <drv/ktyx.h>

/*
 * ========== Sensor Lookup Tables  ===========
 */
static lut_t const KTY81_121_RES[] =
{
   485, 510, 562, 617, 677, 740, 807, 877, 951, 990,
   1029, 1111, 1196, 1286, 1378, 1475, 1575, 1679, 1786, 1896,
   2003, 2103, 2189, 0
};

static lut_t const KTY81_122_RES[] =
{
   495, 520, 573, 630, 690, 755, 823, 895, 971, 1010,
   1050, 1134, 1221, 1312, 1406, 1505, 1607, 1713, 1823, 1934,
   2044, 2146, 2233, 0
};

static lut_t const KTY11_6_RES[] =
{
   950, 1036, 1140, 1250, 1370, 1496, 1630, 1772, 1922, 2000,
   2080, 2245, 2418, 2599, 2788, 2984, 3188, 3400, 3620, 3847,
   4083, 4256, 4469, 0
};

static temp_t const KTYx_TEMP[] =
{
   -55, -50, -40, -30, -20, -10,   0,  10,  20,  25,
    30,  40,  50,  60,  70,  80,  90, 100, 110, 120,
   130, 140, 150, INT_MAX
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
inline temp_t sen_kty8x_121 (res_t r) {
   return (temp_t)sen_pclut(r, KTY81_121_RES, KTYx_TEMP);
}

/*!
 * \brief
 *    kty8x-122 conversion function
 * \param  r   Sensor's resistor value
 * \return temperature result
 */
temp_t sen_kty8x_122 (res_t r){
   return (temp_t)sen_pclut(r, KTY81_122_RES, KTYx_TEMP);
}

/*!
 * \brief
 *    kty11-6 conversion function
 * \param  r   Sensor's resistor value
 * \return temperature result
 */
temp_t sen_kty11_6 (res_t r){
   return (temp_t)sen_pclut(r, KTY11_6_RES, KTYx_TEMP);
}



