/*!
 * \file brh_fcx.c
 * \brief
 *    A target independent BRAHMA FC8 and FC13 sensor function
 *    using look-up table.
 * \note
 *    The sensors MUST have a 2k7 parallel resistor.
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

#include <drv/brh_fcx.h>


/*
 * ========== Sensor Lookup Tables  ===========
 * \warning
 *    The sensors MUST have a 2k7 parallel resistor.
 */

static lut_t const BRH_FC8_RES[] =
{
   2701, 2581, 2197, 1916, 1495, 1148, 816,  563,  317,  63
};

static lux_t const BRH_FC8_LUX[] =
{
   0,    1,    10,   20,   50,   100,  200,  400,  1000, 10000, INT_MAX
};

static lut_t const BRH_FC13_RES[] =
{
   2701, 2560, 2043, 1758, 1318, 986, 697,  471,  266,  53
};

static lux_t const BRH_FC13_LUX[] =
{
   0,    1,    10,   22,   54,   108,  215,  430,  1076, 10000, INT_MAX
};

/*
 * ============================ Public Functions ============================
 */

/*
 * User Functions
 */

/*!
 * \brief
 *    brahma fc8 conversion function
 * \param  r   Sensor's resistor value
 * \return luminosity result
 */
inline lux_t sen_lux_fc8 (res_t r)
{
   return (lux_t)sen_nclut(r, BRH_FC8_RES, BRH_FC8_LUX);
}

/*!
 * \brief
 *    brahma fc13 conversion function
 * \param  r   Sensor's resistor value
 * \return luminosity result
 */
inline lux_t sen_lux_fc13 (res_t r)
{
   return (lux_t)sen_nclut(r, BRH_FC13_RES, BRH_FC13_LUX);
}
