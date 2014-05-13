/*!
 * \file ntc3997k.h
 * \brief
 *    A target independent ntc3997k sensor function using look-up table
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
#ifndef __ntc3997k_h__
#define __ntc3997k_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <drv/sensors_lut.h>

/*
 *  ============= PUBLIC ntc3997k API =============
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
temp_t sen_ntc3997k (res_t r);


#ifdef __cplusplus
}
#endif

#endif   //#ifndef __ntc3997k_h__
