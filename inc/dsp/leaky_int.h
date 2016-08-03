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

#include <dsp/dsp.h>


/* =================== Data types ===================== */

/*!
 * Leaky integrator double precision type
 */
typedef struct {
   double      out;     /*!< The filtered output value */
   double      l;       /*!< The leaky lambda factor */
}iir_li_d_t;

/*!
 * Leaky integrator single precision type
 */
typedef struct {
   float       out;     /*!< The filtered output value */
   float       l;       /*!< The leaky lambda factor */
}iir_li_f_t;

/* =================== Exported Functions ===================== */

void iir_li_d_deinit (iir_li_d_t* li);
void iir_li_d_init (iir_li_d_t* li, double l);
double iir_li_d (iir_li_d_t* li, double value) __O3__ ;

void iir_li_f_deinit (iir_li_f_t* li);
void iir_li_f_init (iir_li_f_t* li, float l);
float iir_li_f (iir_li_f_t* li, float value) __O3__ ;



#if __STDC_VERSION__ >= 201112L
#ifndef iir_li

/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void iir_li_deinit (T *li);
 *
 * \brief
 *    DeInitialize leaky integrator filter pointed by li
 *
 * \param   li,   which filter to de-initialize
 */
#define iir_li_deinit(li) _Generic((li),     \
      iir_li_f_t*: iir_li_f_deinit,          \
      iir_li_d_t*: iir_li_d_deinit,          \
          default: iir_li_f_deinit)(li)

/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void iir_li_init (T *li);
 *
 * \brief
 *    Initialize leaky integrator filter pointed by li
 *
 * \param   li,   which filter to initialize
 * \param   l,    the lambda factor of the filter
 */
#define iir_li_init(li, l) _Generic((li),    \
      iir_li_f_t*: iir_li_f_init,            \
      iir_li_d_t*: iir_li_d_init,            \
          default: iir_li_f_init)(li, l)

/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename TT, typename T> T iir_li (TT *li, T value);
 *
 * \brief
 *    Leaky integrator function
 *
 * \param   li,      which filter to use
 * \param   value,   the input value
 *
 * \return           The filtered value.
 */
#define iir_li(li, v) _Generic((v),    \
             float: iir_li_f,          \
            double: iir_li_d,          \
            default: iir_li_f)(li, v)

#endif   // #ifndef iir_li
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif // #ifndef __leaky_int_h__

