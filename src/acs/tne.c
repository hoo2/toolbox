/*
 * \file   tne.c
 * \brief  Is a try-n-error Controller implementation.
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

#include <acs/tne.h>

/*!
 * \brief
 *    This function De-Initialize all tne parameters to zero.
 *
 * \param  tne: Pointer to tne struct of interest
 * \return none
 */
void  tne_deinit (tne_t* tne)
{
   memset ((void*)tne, 0, sizeof (tne_t));
}

/*!
 * \brief
 *    This function Initialize the TnE with the control parameters.
 *
 * \param  tne: Pointer to TnE struct of interest
 * \param  Kp: Derivative of step gain
 * \param  db: Dead band error
 * \param  dt: Delta t in sec
 * \retval none
 */
void tne_init (tne_t *tne, float Ks, float Ke, float db, float dt)
{
    tne->Ks = Ks;
    tne->Ke = Ke;
    tne->db = db;
    tne->dt = dt;
    tne->max = TNE_DEFAULT_SAT_MAX;    // Add default saturation
    tne->min = TNE_DEFAULT_SAT_MIN;
}

/*!
 * \brief
 *    This function change the saturation levels of the tne.
 *
 * \param  tne: Pointer to tne struct of interest
 * \param  smax: Max
 * \param  smin: min
 *
 * \return none
 */
void  tne_sat (tne_t* tne, float smax, float smin)
{
   tne->max = smax;
   tne->min = smin;
}

/*!
 * \brief
 *    This function Calculates and returns the output value of the TnE Controller.
 *
 * \param  tne: Pointer to tne struct of interest
 * \param  e:   The error, usually sp - fb
 * \retval The TnE output
 */
float tne_out (tne_t* tne, float e)
{
    float step;

   //Calculate the step
    if (fabs(e) > tne->db)
        step = (e>0) ? (tne->Ks * tne->dt + tne->Ke * fabs(e) )
                     : -1 * (tne->Ks * tne->dt + tne->Ke * fabs(e));
    else
        step = 0;

    tne->out += step;
    _SATURATE (tne->out, 1, 0);
 
    return tne->out;
}


