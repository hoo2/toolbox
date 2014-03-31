/**

	TnE_C.h - Is the Trial and error controller header file.

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


#ifndef __TnE_C_h__
#define __TnE_C_h__

#include <math.h>


/* ================   User Defines    ======================*/

#define  GAIN_CORRECTION_STEPS         (100)
#define  GAIN_CORRECTION_PRODUCT       (0.01)   //1%



/* ================   Data types   ======================*/ 
typedef volatile struct
{
    float Ks;  // Gain for step
    float Ke;  // Gain for error
    float db;
    float dt;
    float out;
}TnE_t;



/* ================   Exported Functions    ======================*/
void TnE_Init (TnE_t *tne, float Kp, float Ke, float db, float dt);
float TnE_CalcOutput (TnE_t *tne, float sp, float fb);



#endif //#ifndef __TnE_C_h__

