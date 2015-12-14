/*!
 * \file fir_wsinc.h
 * \brief
 *    A Windowed sinc filter implementation.
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

#ifndef __fir_wsinc_h__
#define __fir_wsinc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>
#include <dsp/fft.h>
#include <dsp/conv.h>
#include <dsp/vectors.h>
#include <string.h>

/*
 * User defines
 */
#define  FIR_WSINC_MIN_TAPS       (5)

/*
 * General defines
 */
#define  _WSINC_BLACKMAN_TAPS       (5.5)
#define  _WSINC_HAMMING_TAPS        (3.3)
#define  _WSINC_BARLETT_TAPS        (4.)
#define  _WSINC_HANNING_TAPS        (3.1)


/*
 * =================== Data types =====================
 */
typedef double (*window_pt) (uint32_t, uint32_t);
typedef uint32_t (*wsinc_taps_pt) (uint32_t, double);

typedef enum {
   FIR_LOW_PASS = 0,    // Default choice
   FIR_HIGH_PASS,
   FIR_BAND_PASS,
   FIR_BAND_REJECT
}fir_ftype_en;

typedef enum {
   FIR_WSINC_BLACKMAN = 0,    // Default choice
   FIR_WSINC_HAMMING,
   FIR_WSINC_BARLETT,
   FIR_WSINC_HANNING
}fir_wtype_en;



typedef struct {
   /*
    * User option fields
    */
   fir_ftype_en ftype;     //!< The filter type
   uint32_t casc;          //!< Number of cascade filters to implement
   double tb;              //!< transition bandwith
   double fc1, fc2;        //!< The transition frequencies

   /*
    * Inner filter data
    */
   double         *k;   //!< Pointer to filter kernel
   double         *t;   //!< Pointer to temporary array
   uint32_t       T;    //!< The number of taps after cascade the filters in time domain
   uint32_t       N;    //!< The number of kernel points in frequncy complex domain
   window_pt      W;    //!< Pointer to window function
   wsinc_taps_pt  tp;   //!< Pointer to number of taps calculation function
}fir_wsinc_t;


/* =================== Public API ===================== */
/*
 * Link and Glue functions
 */

/*
 * Set functions
 */
void fir_wsinc_set_ftype (fir_wsinc_t *f, fir_ftype_en t);
void fir_wsinc_set_wtype (fir_wsinc_t *f, fir_wtype_en t);
void fir_wsinc_set_fc (fir_wsinc_t *f, double fc1, double fc2);
void fir_wsinc_set_tb (fir_wsinc_t *f, double tb);
void fir_wsic_set_cascade (fir_wsinc_t *f, uint32_t c);

/*
 * User Functions
 */
void fir_wsinc_deinit (fir_wsinc_t* f);
uint32_t fir_wsinc_init (fir_wsinc_t* f);

double fir_wsinc_d (fir_wsinc_t* f, double in) __O3__ ;
float fir_wsinc_f (fir_wsinc_t* f, float in) __O3__ ;
float fir_wsinc_i (fir_wsinc_t* f, int in) __O3__ ;
complex_d_t fir_wsinc_cd (fir_wsinc_t* f, complex_d_t in) __O3__ ;
complex_f_t fir_wsinc_cf (fir_wsinc_t* f, complex_f_t in) __O3__ ;
complex_f_t fir_wsinc_ci (fir_wsinc_t* f, complex_i_t in) __O3__ ;

void fir_wsinc (fir_wsinc_t *f, double *in, double *out, uint32_t n);

#if __STDC_VERSION__ >= 201112L
#ifndef fir_mova
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
#define fir_mova(f, in)    _Generic((in),    \
           complex_d_t: fir_mova_cd,         \
           complex_f_t: fir_mova_cf,         \
           complex_i_t: fir_mova_ci,         \
                double: fir_mova_d,          \
                 float: fir_mova_f,          \
                   int: fir_mova_i,          \
                default: fir_mova_d)(f, in)
#endif   // #ifndef fir_mova
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif   // #ifndef __fir_wsinc_h__

