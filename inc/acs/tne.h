/*
 * \file   tne.h
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


#ifndef __tne_h__
#define __tne_h__

#include <math.h>
#include <string.h>
#include <toolbox_defs.h>

/* ================   User Defines    ====================== */

#define  GAIN_CORRECTION_STEPS         (100)
#define  GAIN_CORRECTION_PRODUCT       (0.01)   //1%

/* ================   General Defines    ======================*/
#define  TNE_DEFAULT_SAT_MAX        (1)
#define  TNE_DEFAULT_SAT_MIN        (0)


/* ================   Data types   ====================== */
typedef volatile struct
{
    float Ks;  /*!< Gain for step */
    float Ke;  /*!< Gain for error */
    float db;
    float dt;
    float out;
    float max;
    float min;
}tne_t;



/* ================   Exported Functions    ====================== */
void  tne_deinit (tne_t* tne);
void  tne_init (tne_t* tne, float Kp, float Ke, float db, float dt);
void  tne_sat (tne_t* tne, float smax, float smin);
float tne_out (tne_t* tne, float e);



#endif // #ifndef __tne_h__

