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

#define QTR_A    -0.40528473456935108577
#define QTR_B     1.27323954473516268615
#define QTR_P     0.225

static double _abs (double x) __O3__ ;

static double _abs (double x) {
   return (x<0) ? -x : x;
}

/*!
 * \brief
 *    Calculates the sine of an angle in radians
 *
 *
 * if sin(x) = Ax^2 + Bx + C, then:
 *    sin(0) = 0     |    | A = -4/pi^2
 *    sin(pi/2) = 1  | => | B = 4/pi
 *    sin (pi) = 0   |    | C = 0
 *
 * We now are using the squared parabola
 * Q + P = 1
 * for (Q,P) = (0.775, 0.225) we have maximum absolute error 0.001!!!
 *
 * \param   th The angle in radians
 * \return  The sine
 */
double qsin (double th)
{
   double r;

   if (th>M_PI)   th -= M_2PI;         // angle shift
   r = QTR_A*th*_abs(th) + QTR_B*th;   // r = Ath^2 + Bth + C
   r = r + QTR_P*(r*_abs(r) - r);      // Q = 1-P => r = Qr + Pr^2
   return r;
}

/*!
 * \brief
 *    Calculates the cosine of an angle in radians
 *
 * We use sin algorithm with a angle shift of pi/2
 *
 * \param   th The angle in radians
 * \return  The cosine
 */
double qcos (double th)
{
   double r;

   th += M_PI_2;
   if (th>M_PI)   th -= M_2PI;         // angle shift
   r = QTR_A*th*_abs(th) + QTR_B*th;   // r = Ath^2 + Bth + C
   r = r + QTR_P*(r*_abs(r) - r);      // Q = 1-P => r = Qr + Pr^2
   return r;
}

/*!
 * \brief
 *    Calculates the tangent of an angle in radians
 *
 * \param   th The angle in radians
 * \return  The tangent
 */
double qtan (double th) {
   return qsin(th)/qcos(th);
}

/*!
 * \brief
 *    Calculates the cotangent of an angle in radians
 *
 * \param   th The angle in radians
 * \return  The cotangent
 */
double qcot (double th) {
   return qcos(th)/qsin(th);
}
