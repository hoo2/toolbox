/**

	TnE_C.c - Is the Trial and error controller file.

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


#include "TnE_C.h"

/**
   void TnE_Init (TnE_t *tne, float DKp, float db, float dt)
      
  * @brief  This function Initialize the TnE whith the control parameters.
  * @param  tne: Pointer to TnE struct of interest
             DKp: Derivative of step gain
             db: Dead band error
             dt: Delta t in sec
  * @retval none
*/
void TnE_Init (TnE_t *tne, float Ks, float Ke, float db, float dt)
{
    tne->Ks = Ks;
    tne->Ke = Ke;
    tne-> db = db;
    tne->dt = dt;
}


/**
   float TnE_CalcOut (TnE_t *tne, float sp, float fb)
      
  * @brief  This function Calculates and returns the output value of the TnE Controller.
  * @param  tne: Pointer to TnE struct of interest
  * @param   sp: The Set point
  * @param   fb: The feedback value
  * @retval The TnE output
*/
float TnE_CalcOutput (TnE_t *tne, float sp, float fb)
{
    float e = sp - fb;
    float step;

   //Calculate the step
    if (fabs(e) > tne->db)
        step = (e>0) ? (tne->Ks * tne->dt + tne->Ke * fabs(e) )
                     : -1 * (tne->Ks * tne->dt + tne->Ke * fabs(e));
    else
        step = 0;

    tne->out += step;
    if (tne->out > 1)  tne->out = 1;
    if (tne->out < 0)  tne->out = 0;
 
    return tne->out;
}


