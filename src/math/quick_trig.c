/*
 * \file quick_trig.c
 * \brief
 *    A target independent fast trigonometric functions, using
 *    parabolic approximation.
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
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
#include <math/quick_trig.h>

#define B      (19900)
#define C      (3516)
#define qN     (13)
#define qN_    (15)
#define qA     (12)

static double _abs (double x) __O3__ ;

static double _abs (double x) {
   return (x<0) ? -x : x;
}


/*!
 * \brief
 *    A sine approximation via a fourth-order cosine approx.
 *
 * \param   x  Angle (with 2^15 units/circle)
 * \return     Sine value (Q12)
 */
int32_t isin_S4 (int32_t x)
{
    int c, y;

    c= x<<(30-qN);            // Semi-circle info into carry.
    x -= 1<<qN;               // sine -> cosine calc

    x= x<<(31-qN);            // Mask with PI
    x= x>>(31-qN);            // Note: SIGNED shift! (to qN)
    x= x*x>>(2*qN-14);        // x=x^2 To Q14

    y= B - (x*C>>14);         // B - x^2*C
    y= (1<<qA)-(x*y>>16);     // A - x^2*(B-x^2*C)

    return (c>=0) ? y : -y;
}

/*!
 * \brief
 *    A fourth-order cosine approximation.
 *
 * \param   x  Angle (with 2^15 units/circle)
 * \return     Sine value (Q12)
 */
int32_t icos_S4 (int32_t x)
{
   int c, y;

   c = 1<<qN;                 // pi/2 to 3pi/2 into carry
   c = ((x<=c) || (x>=3*c)) ? 1 : -1;

   x= x<<(31-qN);             // Mask with PI
   x= x>>(31-qN);             // Note: SIGNED shift! (to qN)
   x= x*x>>(2*qN-14);         // x=x^2 To Q14

   y= B - (x*C>>14);          // B - x^2*C
   y= (1<<qA)-(x*y>>16);      // A - x^2*(B-x^2*C)

   return (c>=0) ? y : -y;
}
