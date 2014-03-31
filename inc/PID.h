/**

	PID.h - Is the PID header file for PID Controller

   Copyright (C) 2011 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       8/2011
   Version:    0.1

 */


#ifndef __PID_h__
#define __PID_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <math.h>

/* ================   User Defines    ======================*/
#define  PID_ENABLE_SATURATION      (1)


/* ================   General Defines    ======================*/

#if PID_ENABLE_SATURATION == 1
 #define  PID_DEFAULT_SAT_MAX        (1)
 #define  PID_DEFAULT_SAT_MIN        (0)
#endif

typedef volatile struct
{
   float    Kp,Ki,Kd;
   float    dt;
   float    Int;
   float    Out;
   float    sat_max;
   float    sat_min;
   float    e_db;
   float    ep;
}PID_t;


void  PID_InitPID (PID_t *, float , float , float , float , float );
void  PID_Saturation (PID_t *, float /* smax */, float /* smin */);
void  PID_Clear (PID_t *);
float PID_CalcOutput (PID_t * /* pid */, float /* e */);

#ifdef __cplusplus
}
#endif

#endif
