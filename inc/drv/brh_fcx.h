/*!
 * \file brh_fcx.h
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
#ifndef __brh_fcx_h__
#define __brh_fcx_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <drv/sensors_lut.h>

/*
 *  ============= PUBLIC brh_fcx API =============
 */

/*
 * Link and Glue functions
 */

/*
 * Set functions
 */

/*
 * User Functions
 */
lux_t sen_lux_fc8 (res_t r);
lux_t sen_lux_fc13 (res_t r);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __brh_fcx_h__
