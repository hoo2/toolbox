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
 * =================== Data types =====================
 */

/*!
 * Moving average filter make define
 *
 * bf    Pointer to sample buffer
 * last  Allocated space for the last output
 * N     The number of samples / cut-off frequency
 * c     Buffer cursor
 */
#define _fir_mova_mktype(_type, _rtype, _type_name)  \
typedef struct {        \
      _type   *bf;      \
      _rtype   last;    \
      uint32_t N;       \
      uint32_t c;       \
}_type_name

_fir_mova_mktype (double, double, fir_ma_d_t);        /*!< * Moving average filter double precision */
_fir_mova_mktype (float, float, fir_ma_f_t);         /*!< * Moving average filter single precision */
_fir_mova_mktype (int32_t, float, fir_ma_i32_t);     /*!< * Moving average filter signed int32 */
_fir_mova_mktype (uint32_t, float, fir_ma_ui32_t);   /*!< * Moving average filter unsigned int32 */
_fir_mova_mktype (complex_d_t, complex_d_t, fir_ma_cd_t);  /*!< * Moving average filter double precision complex */
_fir_mova_mktype (complex_f_t, complex_f_t, fir_ma_cf_t);  /*!< * Moving average filter signed precision complex */
_fir_mova_mktype (complex_i_t, complex_f_t, fir_ma_ci_t);  /*!< * Moving average filter signed int32 complex */


/* =================== Public API ===================== */

/*
 * Link and Glue functions
 */

/*
 * Set functions
 */

/*
 * User Functions
 */
uint32_t fir_ma_init_d (fir_ma_d_t* f, double fc);
uint32_t fir_ma_init_f (fir_ma_f_t* f, float fc);
uint32_t fir_ma_init_i32 (fir_ma_i32_t* f, float fc);
uint32_t fir_ma_init_ui32 (fir_ma_ui32_t* f, float fc);
uint32_t fir_ma_init_cd (fir_ma_cd_t* f, double fc);
uint32_t fir_ma_init_cf (fir_ma_cf_t* f, float fc);
uint32_t fir_ma_init_ci (fir_ma_ci_t* f, float fc);


double fir_ma_d (fir_ma_d_t* f, double in) __O3__ ;
float fir_ma_f (fir_ma_f_t* f, float in) __O3__ ;
float fir_ma_i32 (fir_ma_i32_t* f, int32_t in) __O3__ ;
float fir_ma_ui32 (fir_ma_ui32_t* f, uint32_t in) __O3__ ;
complex_d_t fir_ma_cd (fir_ma_cd_t* f, complex_d_t in) __O3__ ;
complex_f_t fir_ma_cf (fir_ma_cf_t* f, complex_f_t in) __O3__ ;
complex_f_t fir_ma_ci (fir_ma_ci_t* f, complex_i_t in) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef filter_mova

/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T1, typename T2> uint32_t fir_ma_init (T1 *f, T2 fc);
 *
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
#define fir_ma_init(f, in)    _Generic((f),       \
           fir_ma_d_t*: fir_ma_init_d,            \
           fir_ma_f_t*: fir_ma_init_f,            \
         fir_ma_i32_t*: fir_ma_init_i32,          \
        fir_ma_ui32_t*: fir_ma_init_ui32,         \
          fir_ma_cd_t*: fir_ma_init_cd,           \
          fir_ma_cf_t*: fir_ma_init_cf,           \
          fir_ma_ci_t*: fir_ma_init_ci,           \
               default: fir_ma_init_f)(f, in)

/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T1, typename T2> T2 fir_ma (T1 *f, T1 in);
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
#define fir_ma(f, in)    _Generic((in),      \
                double: fir_ma_d,            \
                 float: fir_ma_f,            \
               int32_t: fir_ma_i32,          \
              uint32_t: fir_ma_ui32,         \
           complex_d_t: fir_ma_cd,           \
           complex_f_t: fir_ma_cf,           \
           complex_i_t: fir_ma_ci,           \
               default: fir_ma_f)(f, in)
#endif   // #ifndef filter_mova
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __filter_mova_h__

