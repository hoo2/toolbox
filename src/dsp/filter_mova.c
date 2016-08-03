/*!
 * \file filter_mova.c
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

#include <dsp/filter_mova.h>


/*
 * General defines
 */
#define  _FILTER_MOVA_SAMPLES(_fc)   ( sqrt (0.196196 + _fc*_fc)/_fc )


/*
 * =================== Public API =====================
 */

/*
 * Link and Glue functions
 */


/*
 * Set functions
 */

/*
 * User Functions
 */

/*!
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
uint32_t fir_ma_init_d (fir_ma_d_t* f, double fc)
{
   f->N = _FILTER_MOVA_SAMPLES (fc);

   // Try to allocate memory and check sample points for cutoff frequency
   if ( (f->N != 0) && ( (f->bf = (void*)calloc (f->N, sizeof(double))) != NULL )) {
      // Clear accumulator and cursor
      f->last = f->c = 0;
      return f->N;
   }
   else
      return 0;
}


/*!
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
uint32_t fir_ma_init_f (fir_ma_f_t* f, float fc)
{
   f->N = _FILTER_MOVA_SAMPLES (fc);

   // Try to allocate memory and check sample points for cutoff frequency
   if ( (f->N != 0) && ( (f->bf = (void*)calloc (f->N, sizeof(float))) != NULL )) {
      // Clear accumulator and cursor
      f->last = f->c = 0;
      return f->N;
   }
   else
      return 0;
}


/*!
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
uint32_t fir_ma_init_i32 (fir_ma_i32_t* f, float fc)
{
   f->N = _FILTER_MOVA_SAMPLES (fc);

   // Try to allocate memory and check sample points for cutoff frequency
   if ( (f->N != 0) && ( (f->bf = (void*)calloc (f->N, sizeof(int32_t))) != NULL )) {
      // Clear accumulator and cursor
      f->last = f->c = 0;
      return f->N;
   }
   else
      return 0;
}


/*!
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
uint32_t fir_ma_init_ui32 (fir_ma_ui32_t* f, float fc)
{
   f->N = _FILTER_MOVA_SAMPLES (fc);

   // Try to allocate memory and check sample points for cutoff frequency
   if ( (f->N != 0) && ( (f->bf = (void*)calloc (f->N, sizeof(uint32_t))) != NULL )) {
      // Clear accumulator and cursor
      f->last = f->c = 0;
      return f->N;
   }
   else
      return 0;
}


/*!
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
uint32_t fir_ma_init_cd (fir_ma_cd_t* f, double fc)
{
   f->N = _FILTER_MOVA_SAMPLES (fc);

   // Try to allocate memory and check sample points for cutoff frequency
   if ( (f->N != 0) && ( (f->bf = (void*)calloc (f->N, sizeof(complex_d_t))) != NULL )) {
      // Clear accumulator and cursor
      f->last = f->c = 0;
      return f->N;
   }
   else
      return 0;
}

/*!
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
uint32_t fir_ma_init_cf (fir_ma_cf_t* f, float fc)
{
   f->N = _FILTER_MOVA_SAMPLES (fc);

   // Try to allocate memory and check sample points for cutoff frequency
   if ( (f->N != 0) && ( (f->bf = (void*)calloc (f->N, sizeof(complex_f_t))) != NULL )) {
      // Clear accumulator and cursor
      f->last = f->c = 0;
      return f->N;
   }
   else
      return 0;
}


/*!
 * \brief
 *    Moving Average filter initialization.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalized cutoff frequency [0fs - 0.5fs]
 * \return        The number of points
 */
uint32_t fir_ma_init_ci (fir_ma_ci_t* f, float fc)
{
   f->N = _FILTER_MOVA_SAMPLES (fc);

   // Try to allocate memory and check sample points for cutoff frequency
   if ( (f->N != 0) && ( (f->bf = (void*)calloc (f->N, sizeof(complex_i_t))) != NULL )) {
      // Clear accumulator and cursor
      f->last = f->c = 0;
      return f->N;
   }
   else
      return 0;
}




/*!
 * \brief
 *    Double precision recursive Moving Average filter.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
double fir_ma_d (fir_ma_d_t* f, double in) {
   double dep;

   dep = f->bf[f->c];            /* Save departed point */
   f->bf[f->c] = in;             /* Get new value */
   if ( ++(f->c) >= f->N)        /* Buffer overflow checking */
      f->c = 0;
   return f->last += (double)(in - dep)/f->N;   /* Recursive calculation */
}

/*!
 * \brief
 *    Single precision recursive Moving Average filter.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
float fir_ma_f (fir_ma_f_t* f, float in) {
   float dep;

   dep = f->bf[f->c];            /* Save departed point */
   f->bf[f->c] = in;             /* Get new value */
   if ( ++(f->c) >= f->N)        /* Buffer overflow checking */
      f->c = 0;
   return f->last += (float)(in - dep)/f->N;   /* Recursive calculation */
}

/*!
 * \brief
 *    Signed Integer recursive Moving Average filter returning
 *    single precision float.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
float fir_ma_i32 (fir_ma_i32_t* f, int32_t in) {
   int32_t dep;

   dep = f->bf[f->c];            /* Save departed point */
   f->bf[f->c] = in;             /* Get new value */
   if ( ++(f->c) >= f->N)        /* Buffer overflow checking */
      f->c = 0;
   return f->last += (float)(in - dep)/f->N;   /* Recursive calculation */
}

/*!
 * \brief
 *    Unsigned Integer recursive Moving Average filter returning
 *    single precision float.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
float fir_ma_ui32 (fir_ma_ui32_t* f, uint32_t in) {
   uint32_t dep;

   dep = f->bf[f->c];            /* Save departed point */
   f->bf[f->c] = in;             /* Get new value */
   if ( ++(f->c) >= f->N)        /* Buffer overflow checking */
      f->c = 0;
   return f->last += (float)(in - dep)/f->N;   /* Recursive calculation */
}

/*!
 * \brief
 *    Double precision complex recursive Moving Average filter returning
 *    double precision complex.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
complex_d_t fir_ma_cd (fir_ma_cd_t* f, complex_d_t in) {
   complex_d_t dep;

   dep = f->bf[f->c];            /* Save departed point */
   f->bf[f->c] = in;             /* Get new value */
   if ( ++(f->c) >= f->N)        /* Buffer overflow checking */
      f->c = 0;
   return f->last += (complex_d_t)(in - dep)/f->N;   /* Recursive calculation */
}

/*!
 * \brief
 *    Single precision complex recursive Moving Average filter returning
 *    single precision complex.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
complex_f_t fir_ma_cf (fir_ma_cf_t* f, complex_f_t in) {
   complex_f_t dep;

   dep = f->bf[f->c];            /* Save departed point */
   f->bf[f->c] = in;             /* Get new value */
   if ( ++(f->c) >= f->N)        /* Buffer overflow checking */
      f->c = 0;
   return f->last += (complex_f_t)(in - dep)/f->N;   /* Recursive calculation */
}

/*!
 * \brief
 *    Integer complex recursive Moving Average filter returning
 *    single precision complex.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
complex_f_t fir_ma_ci (fir_ma_ci_t* f, complex_i_t in) {
   complex_i_t dep;

   dep = f->bf[f->c];            /* Save departed point */
   f->bf[f->c] = in;             /* Get new value */
   if ( ++(f->c) >= f->N)        /* Buffer overflow checking */
      f->c = 0;
   return f->last += (complex_f_t)(in - dep)/f->N;   /* Recursive calculation */
}



