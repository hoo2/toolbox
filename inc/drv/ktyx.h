/*!
 * \file ktyx.h
 * \brief
 *    A target independent kty8x_121, kty8x_122 and kty11 sensor
 *    functions using look-up table.
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
#ifndef __ktyx_h__
#define __ktyx_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <drv/sensors_lut.h>

/*
 *  ============= PUBLIC ktyx API =============
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
temp_t sen_kty8x_121 (res_t r);
temp_t sen_kty8x_122 (res_t r);
temp_t sen_kty11_6 (res_t r);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __ktyx_h__
