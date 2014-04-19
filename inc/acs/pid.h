/*
 * \file   pid.h
 * \brief  Is a PID Controller implementation.
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


#ifndef __pid_h__
#define __pid_h__

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
}pid_t;


void  pid_init (pid_t * pid, float kp, float ki, float kd, float dt, float db);
void  pid_deinit (pid_t* pid);
void  pid_sat (pid_t* pid, float smax, float smin);
void  pid_clear (pid_t* pid);
float pid_out (pid_t* pid, float e);

#ifdef __cplusplus
}
#endif

#endif
