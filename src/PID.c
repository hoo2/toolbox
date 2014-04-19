/*
 * \file   pid.c
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

#include <acs/pid.h>

/*!
 * \brief
 *    This function Initialize the pid whith the control parameters.
 *
 * \param  pid: Pointer to PID struct of interest
 * \param  kp: Product gain
 * \param  ki: Integral gain
 * \param  kd: Derivative gain
 * \param  dt: Delta t in sec
 * \param  db: Dead band error for Integration
 *
 * \return none
 */
void  pid_init (pid_t * pid, float kp, float ki, float kd, float dt, float db)
{
   pid->Kp = kp;
   pid->Ki = ki;
   pid->Kd = kd;
   pid->dt = dt;
   pid->ep = pid->Int = 0;
   pid->e_db = db;

   pid->Out = 0;
   pid->sat_max = PID_DEFAULT_SAT_MAX;    // Add default saturation
   pid->sat_min = PID_DEFAULT_SAT_MIN;
}

/*!
 * \brief
 *    This function change the saturation levels of the PID.
 *
 * \param  pid: Pointer to PID struct of interest
 * \param  smax: Max
 * \param  smin: min
 *
 * \return none
 */
void  pid_sat (pid_t * pid, float smax, float smin)
{
   pid->sat_max = smax;
   pid->sat_min = smin;
}

/*!
 * \brief
 *    This function clears the Integral Error of the PID.
 *
 * \param  pid: Pointer to PID struct of interest
 * \return none
 */
void pid_clear (pid_t *pid)
{
   pid->Int = 0;
}

/*!
 * \brief
 *    This function Calculates and returns the output value of the PID.
 *
 * \param  pid: Pointer to PID struct of interest
 * \param  e:   Usually sp - fb
 *
 * \return The PID output
*/
float pid_out (pid_t *pid, float e)
{
   float der;
   float out;	// double buffer the output

   // Calc Integral for significant errors
   if ( e>pid->e_db || e < -(pid->e_db)) 
      pid->Int += e*pid->dt;
   // Calc Derivative
   der = (e - pid->ep)/pid->dt;
   pid->ep = e;   //update the previous error

   // Calc the output
   out = pid->Kp * e + pid->Ki * pid->Int + pid->Kd * der;
   
   // Saturation
   #if PID_ENABLE_SATURATION == 1
      if (out > pid->sat_max)   { out = pid->sat_max; }
      if (out < pid->sat_min)   { out = pid->sat_min; }
   #endif

   return (pid->Out = out);
}


