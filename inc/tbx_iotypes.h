/*
 * \file tbx_iotypes.h
 * \brief
 *    An In Out type definition for the toolbox.
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
#ifndef __tbx_iotypes_h__
#define __tbx_iotypes_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint8_t  byte_t;         /*!< 8 bits wide */
typedef uint16_t word_t;         /*!< 16 bits wide */
typedef uint32_t dword_t;        /*!< 32 bits wide */

/*!
 * Pin function pointers
 * \note
 *    These function pointers do not correspond to pin levels.
 *    They correspond to the enable/disable functionality of that pin.
 */
typedef    void (*drv_pinout_ft) (uint8_t on);
typedef uint8_t (*drv_pinin_ft)  (void);
typedef    void (*drv_pindir_ft) (uint8_t on);

/*!
 * Analog input function pointer
 */
typedef  float (*drv_ain_f_ft) (void);
typedef    int (*drv_ain_i_ft) (void);

/*!
 * Digital I/O function pointers
 * \note
 *    These function pointers do not correspond to circuit/port/pin levels.
 *    They correspond to the enable/disable functionality.
 */
typedef  uint8_t (*drv_din_ft)   (void);
typedef     void (*drv_dout_ft)  (uint8_t);
typedef     void (*drv_out_i_ft) (int);
typedef     void (*drv_out_f_ft) (float);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __tbx_iotypes_h__
