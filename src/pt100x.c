/*!
 * \file pt100x.c
 * \brief
 *    A target independent pt100 and pt1000 sensor function
 *    using look-up table.
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

#include <drv/pt100x.h>


/*
 * ========== Sensor Lookup Tables  ===========
 */
static lut_t const PT100x_RES[] =
{
    185.2,  271.0,  355.4,  438.8,  521.1,  // -200 ~ -120
    602.6,  683.3,  763.3,  842.7,  921.6,  // -100 ~  -20
   1000.0, 1077.9, 1155.4, 1232.4, 1309.0,  //    0 ~   80
   1385.1, 1460.7, 1535.8, 1610.5, 1684.8,  //  100 ~  180
   1758.6, 1831.9, 1904.7, 1977.1, 2049.0,  //  200 ~  280
   2120.5, 2191.5, 2262.1, 2332.1, 2401.8,  //  300 ~  380
   2470.9, 2539.6, 2607.8, 2675.6, 2742.9,  //  400 ~  480
   2809.8, 2876.2, 2942.1, 3007.5, 3072.5,  //  500 ~  580
   3137.1
};

static temp_t const PT100x_TEMP[]=
{
   -200, -180, -160, -140, -120,
   -100,  -80,  -60,  -40,  -20,
      0,   20,   40,   60,   80,
    100,  120,  140,  160,  180,
    200,  220,  240,  260,  280,
    300,  320,  340,  360,  380,
    400,  420,  440,  460,  480,
    500,  520,  540,  560,  580,
    600, INT_MAX
};


/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    pt100 conversion function
 * \param  r   Sensor's resistor value
 * \return temperature result
 */
inline temp_t sen_pt100 (res_t r) {
    return (temp_t)sen_pclut(r*10, PT100x_RES, PT100x_TEMP);
}

/*!
 * \brief
 *    pt1000 conversion function
 * \param  r   Sensor's resistor value
 * \return temperature result
 */
inline temp_t sen_pt1000 (res_t r) {
    return (temp_t)sen_pclut(r, PT100x_RES, PT100x_TEMP);
}

