/*!
 * \file moving_av.h
 * \brief
 *    A moving average filter implementation.
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

#ifndef __moving_av_h__
#define __moving_av_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>


/* =================== Data types ===================== */
// Data type for Buffer filtering
typedef volatile struct
{
   float       *pb;  // pointer to buffer
   uint8_t     size;
   uint8_t     c;    // cursor
}moving_av_t;


/* =================== Exported Functions ===================== */

void moving_av_deinit (moving_av_t* filter);
int moving_av_init (moving_av_t* filter, uint8_t points);    // Init the Moving average points
float moving_av (moving_av_t* filter, float value) __optimize__ ;   // return the filtered value

#ifdef __cplusplus
}
#endif

#endif

