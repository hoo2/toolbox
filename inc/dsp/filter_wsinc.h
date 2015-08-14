/*!
 * \file filter_wsinc.h
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

#ifndef __filter_wsinc_h__
#define __filter_wsinc_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>
#include <string.h>

/*
 * General defines
 */
/*!
 * Number of samples calculation
 *           4
 * N = -------------
 *      Transition BW
 */
#define  FILTER_WSINC_SAMPLES(_tr_bw)   ( 4./_tr_bw )


/*
 * =================== Data types =====================
 */
typedef double (*window_pt) (uint32_t, uint32_t);

typedef enum {
   FILTER_LOW_PASS = 0,    // Default choice
   FILTER_HIGH_PASS,
   FILTER_BAND_PASS,
   FILTER_BAND_REJECT
}filter_ftype_en;

typedef enum {
   FILTER_WSINC_BLACKMAN = 0,    // Default choice
   FILTER_WSINC_HAMMING,
   FILTER_WSINC_BARLETT,
   FILTER_WSINC_HANNING
}filter_wtype_en;



typedef struct
{
   double     *kernel;    //!< Pointer to filter kernel
   uint32_t it_size;    //!< Kernel's each item size
   uint32_t N;          //!< The number of kernel samples
   filter_ftype_en ftype;  //!< The filter type
   window_pt   window;  //!< Pointer to window function
   double fc1, fc2;     //!< The transition frequencies
}filter_wsinc_t;


/* =================== Public API ===================== */
/*
 * Link and Glue functions
 */

/*
 * Set functions
 */
void filter_wsinc_set_item_size (filter_wsinc_t *f, uint32_t size);
void filter_wsinc_set_ftype (filter_wsinc_t *f, filter_ftype_en t);
void filter_wsinc_set_wtype (filter_wsinc_t *f, filter_wtype_en t);
void filter_wsinc_set_fc (filter_wsinc_t *f, double fc1, double fc2);
void filter_wsinc_set_trbw (filter_wsinc_t *f, double trbw);

/*
 * User Functions
 */
void filter_wsinc_deinit (filter_wsinc_t* f);
uint32_t filter_wsinc_init (filter_wsinc_t* f);

double filter_wsinc_d (filter_wsinc_t* f, double in) __optimize__ ;
float filter_wsinc_f (filter_wsinc_t* f, float in) __optimize__ ;
float filter_wsinc_i (filter_wsinc_t* f, int in) __optimize__ ;
complex_d_t filter_wsinc_cd (filter_wsinc_t* f, complex_d_t in) __optimize__ ;
complex_f_t filter_wsinc_cf (filter_wsinc_t* f, complex_f_t in) __optimize__ ;
complex_f_t filter_wsinc_ci (filter_wsinc_t* f, complex_i_t in) __optimize__ ;

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

#endif   // #ifndef __filter_wsinc_h__

