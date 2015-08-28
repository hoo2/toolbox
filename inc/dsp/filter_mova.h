/*!
 * \file filter_mova.h
 * \brief
 *    A recursive moving average filter implementation.
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

#ifndef __filter_mova_h__
#define __filter_mova_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>
#include <string.h>

/*
 * General defines
 */
#define  FILTER_MOVA_SAMPLES(_fc)   ( sqrt (0.196196 + _fc*_fc)/_fc )
#define  FILTER_MOVA_LAST_SIZE      (16)
   //!< 16 bytes sould be enough for supported data types


/*
 * =================== Data types =====================
 */
typedef struct
{
   void     *bf;     //!< Pointer to sample buffer
   uint32_t it_size; //!< Each item size
   byte_t   last[FILTER_MOVA_LAST_SIZE];  //!< Allocated space for the last output
   uint32_t N;       //!< The number of samples / cut-off frequency
   uint32_t c;       //!< Buffer cursor
}filter_mova_t;


/* =================== Public API ===================== */
/*
 * Link and Glue functions
 */

/*
 * Set functions
 */
void filter_mova_set_item_size (filter_mova_t *f, uint32_t size);
void filter_mova_set_fc (filter_mova_t *f, double fc);

/*
 * User Functions
 */
void filter_mova_deinit (filter_mova_t* f);
uint32_t filter_mova_init (filter_mova_t* f);

double filter_mova_d (filter_mova_t* f, double in) __O3__ ;
float filter_mova_f (filter_mova_t* f, float in) __O3__ ;
float filter_mova_i (filter_mova_t* f, int in) __O3__ ;
complex_d_t filter_mova_cd (filter_mova_t* f, complex_d_t in) __O3__ ;
complex_f_t filter_mova_cf (filter_mova_t* f, complex_f_t in) __O3__ ;
complex_f_t filter_mova_ci (filter_mova_t* f, complex_i_t in) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef filter_mova
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T1, typename T2> T2 filter_mova (filter_mova_t *f, T1 in);
 *
 * \brief
 *    Recursive Moving Average filter.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
#define filter_mova(f, in)    _Generic((in),    \
           complex_d_t: filter_mova_cd,         \
           complex_f_t: filter_mova_cf,         \
           complex_i_t: filter_mova_ci,         \
                double: filter_mova_d,          \
                 float: filter_mova_f,          \
                   int: filter_mova_i,          \
                default: filter_mova_d)(f, in)
#endif   // #ifndef filter_mova
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __filter_mova_h__

