/*!
 * \file leaky_int.h
 * \brief
 *    A leaky integrator filter implementation.
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

#ifndef __leaky_int_h__
#define __leaky_int_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <math.h>

/* =================== Data types ===================== */

typedef volatile struct
{
   double      out;
   double      lambda;
}leaky_int_t;


/* =================== Exported Functions ===================== */

void leaky_int_deinit (leaky_int_t* li);
void leaky_int_init (leaky_int_t* li, double l);
double leaky_int (leaky_int_t* li, double value);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __leaky_int_h__

