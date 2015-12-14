/*

   Complex - Complex lib

   Copyright (C) 2013 Houtouridis Christos (http://houtouridis.blogspot.com/)

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
   Date:       5/2013
   Version:    0.1

*/

#include "Complex.h"

Polar_t Complex2Polar (Complex_t c)
{
   Polar_t RES;
   if (!c.Re) c.Re = 1e-20;  //Div by Zero Supression

   RES.phi = (360.0/_2PI) * atan( (float)c.Im/c.Re );

   if (c.Re < 0)
      RES.phi += 180;
   if (c.Re>0 && c.Im <0)
      RES.phi += 360;
   if (RES.phi >= 360)
      RES.phi -= 360;
   RES.mag = sqrt(c.Re*c.Re + c.Im*c.Im);

   return RES;
}

Complex_t Polar2Complex (Polar_t p)
{
   Complex_t res;

   res.Re = p.mag * cos (p.phi);
   res.Im = p.mag * sin (p.phi);
   return res;
}

Complex_t Cmplx_add (Complex_t c1, Complex_t c2)
{
   Complex_t res;

   res.Re = c1.Re + c2.Re;
   res.Im = c1.Im + c2.Im;

   return res;
}


Complex_t Cmplx_sub (Complex_t c1, Complex_t c2)
{
   Complex_t res;

   res.Re = c1.Re - c2.Re;
   res.Im = c1.Im - c2.Im;

   return res;
}

Complex_t Cmplx_scale (Complex_t c, float p)
{
   Complex_t res;

   res.Re = c.Re*p;
   res.Im = c.Im*p;
   return res;
}

