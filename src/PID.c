/**

	PID.c - Is the PID file for PID Controller

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


#include "PID.h"

/**
   void  PID_InitPID (PID_t * pid, float kp, float ki, float kd, float dt, float db);
      
  * @brief  This function Initialize the pid whith the control parameters.
  * @param  pid: Pointer to PID struct of interest
             kp: Product gain
             ki: Integral gain
             kd: Derivative gain
             dt: Delta t in sec
           Omax: Output max saturation value
           Omin: Output min saturation value
             db: Dead band error for Integration
  * @retval none
*/
void  PID_InitPID (PID_t * pid, float kp, float ki, float kd, float dt, float db)
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

/**
   PID_Saturation (PID_t * pid, float smax, float smin)
      
  * @brief  This function change the saturation levels of the PID.
  * @param  pid: Pointer to PID struct of interest
           smax: Max
           smin: min
  * @retval void
*/
void  PID_Saturation (PID_t * pid, float smax, float smin)
{
   pid->sat_max = smax;
   pid->sat_min = smin;
}

/**
   void PID_Clear (PID_t *)
      
  * @brief  This function clears the Integral Error of the PID.
  * @param  pid: Pointer to PID struct of interest
  * @retval void
*/
void PID_Clear (PID_t *pid)
{
   pid->Int = 0;
}

/**
   float PID_CalcOutput (float sp, float fb)
      
  * @brief  This function Calculates and returns the output value of the PID.
  * @param  pid: Pointer to PID struct of interest
             e:  Usually sp - fb
  * @retval The PID output
*/
float PID_CalcOutput (PID_t *pid, float e)
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


