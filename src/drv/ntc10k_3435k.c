/*!
 * \file ntc10k_3435k.c
 * \brief
 *    A target independent ntc10k_3435k, (like AT103-11) sensor
 *    function using look-up table
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2017 Houtouridis Christos (http://www.houtouridis.net)
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

#include <drv/ntc10k_3435k.h>

/*
 * ========== Sensor Lookup Tables  ===========
 */
static lut_t const _res[] = {
   329500, 247700, 188500, 144100, 111300, 86430, 67770, 53410, 42470, 33900,
    27280,  22050,  17960,  14690,  12090, 10000,  8313,  6940,  5827,  4911,
     4160,   3536,   3020,   2588,  2228,   1924,  1668,  1451,  1266,  1108,
    973.1,  857.2,  757.6
};

static temp_t const _temp[] = {
   -50, -45, -40, -35, -30, -25, -20, -15, -10,  -5,
     0,   5,  10,  15,  20,  25,  30,  35,  40,  45,
    50,  55,  60,  65,  70,  75,  80,  85,  90,  95,
   100, 105, 111, INT_MAX
};


/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    ntc10k_3435k conversion function
 * \param  r   Sensor's resistor value
 * \return temperature result
 */
inline temp_t sen_ntc10k_3435k (res_t r) {
   return (temp_t)sen_nclut(r, _res, _temp);
}
