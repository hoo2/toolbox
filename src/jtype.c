/*!
 * \file jtype.c
 * \brief
 *    A target independent jtype sensor function using look-up table
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <drv/jtype.h>

/*
 * ========== Sensor Lookup Tables  ===========
 */
static lut_t const J_Type_mV[] =
{
  -7.890, -7.403, -6.821, -6.159, -5.426,
  -4.633, -3.786, -2.893, -1.961, -0.955,
   0.000,  1.019,  2.059,  3.116,  4.187,
   5.269,  6.360,  7.459,  8.562,  9.669,
  10.779, 11.889, 13.000, 14.110, 15.219,
  16.327, 17.434, 18.538, 19.642, 20.745,
  21.848, 22.952, 24.057, 25.164, 26.276,
  27.393, 28.516, 29.647, 30.788, 31.939,
  33.102, 34.279, 35.470, 36.675, 37.896,
  39.132, 40.382, 41.645,
};

static temp_t const J_Type_TEMP[] =
{
  -200,-180,-160,-140,-120,
  -100, -80, -60, -40, -20,
     0,  20,  40,  60,  80,
   100, 120, 140, 160, 180,
   200, 220, 240, 260, 280,
   300, 320, 340, 360, 380,
   400, 420, 440, 460, 480,
   500, 520, 540, 560, 580,
   600, 620, 640, 660, 680,
   700, 720, 740, INT_MAX
};


/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    kty8x-121 conversion function
 * \param  v       Sensor's volt diff
 * \param  contact Sensor's contact temperature
 * \return temperature result
 */
inline temp_t sen_jtype (float volt, float contact) {
   return (temp_t)(contact + sen_pclut(volt*1000, J_Type_mV, J_Type_TEMP));
}
