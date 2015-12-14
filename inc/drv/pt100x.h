/*!
 * \file pt100x.h
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
#ifndef __pt100x_h__
#define __pt100x_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <drv/sensors_lut.h>

/*
 *  ============= PUBLIC pt100x API =============
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
temp_t sen_pt100 (res_t r);
temp_t sen_pt1000 (res_t r);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __pt100x_h__
