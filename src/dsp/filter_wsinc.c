/*!
 * \file filter_wsinc.c
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

#include <dsp/filter_wsinc.h>
/*
 * ========= Static ============
 */
static double _sinc (double fc, int32_t x) {
   double _2pifc = M_2PI * fc;
   if (x == 0)    return _2pifc;
   else           return sin (_2pifc*x) / x;
}
static double _blackman (uint32_t i, uint32_t n) {
   double th = 2*M_PI*i/n;
   return 0.42 - 0.5*cos (th) + 0.08*cos(2*th);
}
static double _hamming (uint32_t i, uint32_t n) {
   return 0.54 - 0.46*cos (2*M_PI*i/n);
}
static double _barlett (uint32_t i, uint32_t n) {
   return 1 - (2* fabs (i - (n>>1)) / n);
}
static double _hanning (uint32_t i, uint32_t n) {
   return 0.5 - 0.5*cos (2*M_PI*i/n);
}


static void _1tran_loop (filter_wsinc_t *f, int sign) {
   uint32_t i, cT, n_2;
   int32_t  n;
   double fact;

   cT = f->T * f->casc;
   n_2 = cT>>1;
   // Calculate kernel and normalise factor
   for (fact=i=0 ; i<=n_2 ; ++i) {
      n = i - n_2;
      f->k[i] = _sinc (f->fc1, n) * f->W (i, cT);
      fact += f->k[i] * 2;
      f->k[cT - i] = (f->k[i] *= sign);
   }
   fact -= f->k[n_2]*sign;
   fact = 1/fact;
   // Apply normalise factor
   for (i=0 ; i<=cT ; ++i)
      f->k[i] *= fact;

   // invert symmetry point if needed
   f->k[n_2] += (sign==-1) ? 1:0;
}

static void _2tran_loop (filter_wsinc_t *f, int sign) {
   uint32_t i, cT, n_2;
   int32_t  n;
   double fact;

   cT = f->T * f->casc;
   n_2 = cT>>1;
   // Calculate kernel and normalise factor
   for (fact=i=0 ; i<=n_2 ; ++i) {
      n = i - n_2;
      f->k[i] = ( _sinc (f->fc1, n) * f->W (i, cT) +
                  _sinc (f->fc2, n) * f->W (i, cT) );
      fact += f->k[i] * 2;
      f->k[cT - i] = (f->k[i] *= sign);
   }
   fact -= f->k[n_2]*sign;
   fact = 1/fact;
   // Apply normalise factor
   for (i=0 ; i<=cT ; ++i)
      f->k[i] *= fact;

   // invert symmetry point if needed
   f->k[n_2] += (sign==-1) ? 1:0;
}

/*
 * =================== Public API =====================
 */
/*
 * Link and Glue functions
 */

/*
 * Set functions
 */

/*!
 * \brief
 *    Set the size of kernel data/points.
 *    For ex:
 *       sizeof (double), for double precision numbers
 *
 * \param   f     Which filter to use
 * \param   size  The size in size_t
 * \return        none
*/
//void filter_wsinc_set_item_size (filter_wsinc_t *f, uint32_t size) {
//   f->it_size = size;
//}

/*!
 * \brief
 *    Set the filter type
 *
 * \param   f     Which filter to use
 * \param   t     Filter type
 *    \arg  FILTER_LOW_PASS
 *    \arg  FILTER_HIGH_PASS
 *    \arg  FILTER_BAND_PASS
 *    \arg  FILTER_BAND_REJECT
 * \return        none
*/
void filter_wsinc_set_ftype (filter_wsinc_t *f, filter_ftype_en t) {
   switch (t) {
      case FILTER_LOW_PASS:
      case FILTER_HIGH_PASS:
      case FILTER_BAND_PASS:
      case FILTER_BAND_REJECT:
         f->ftype = t;
         break;
      default:
         f->ftype = FILTER_LOW_PASS;
         break;
   }
}

/*!
 * \brief
 *    Set the window type
 *
 * \param   f     Which filter to use
 * \param   w     Window type
 *    \arg  FILTER_WSINC_BLACKMAN
 *    \arg  FILTER_WSINC_HAMMING
 *    \arg  FILTER_WSINC_BARLETT
 *    \arg  FILTER_WSINC_HANNING
 * \return        none
*/
void filter_wsinc_set_wtype (filter_wsinc_t *f, filter_wtype_en w) {
   switch (w) {
      default:
      case FILTER_WSINC_BLACKMAN:   f->W = _blackman; break;
      case FILTER_WSINC_HAMMING:    f->W = _hamming;  break;
      case FILTER_WSINC_BARLETT:    f->W = _barlett;  break;
      case FILTER_WSINC_HANNING:    f->W = _hanning;  break;
   }
}

/*!
 * \brief
 *    Set the normalised transition frequencies.
 *
 * The range of the frequencies is 0 to 0.5, and represent the
 * half of the sampling frequency.
 *
 * \param   f     Which filter to use
 * \param   fc1   Transition frequency 1
 * \param   fc2   Transition frequency 2
 * \return        none
*/
void filter_wsinc_set_fc (filter_wsinc_t *f, double fc1, double fc2) {
   f->fc1 = fc1;
   f->fc2 = fc2;
}

/*!
 * \brief
 *    Set the transition bandwidth.
 *
 * The range of the frequenciy is 0 to 0.5, and represent the
 * half of the sampling frequency.
 *
 * \param   f     Which filter to use
 * \param   trbw  Transition frequency 1
 * \return        None
*/
void filter_wsinc_set_tb (filter_wsinc_t *f, double tb) {
   f->tb = tb;
}

/*!
 * \brief
 *    Set the number of cascading filter to implement. This way
 *    The kernel length and the gain is increased.
 *
 * \param   f     Which filter to use
 * \param   c     How many filters to cascade
 * \return        None
*/
void filter_wsic_set_cascade (filter_wsinc_t *f, uint32_t c) {
   f->casc = c;
}

/*
 * User Functions
 */

/*!
 * \brief
 *    Windowed sinc filter de-initialisation.
 *
 * \param  f      Which filter to free
 * \return none
*/
void filter_wsinc_deinit (filter_wsinc_t* f) {
   if ( f->k )
      free ((void*)f->k);
   memset ((void*)f, 0, sizeof (filter_wsinc_t));
}


/*!
 * \brief
 *    Windowed sinc filter initialisation.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalised cutoff frequency [0fs - 0.5fs]
 * \return        None
 */
uint32_t filter_wsinc_init (filter_wsinc_t* f)
{
   uint32_t i, cT;

   // Calculate taps and cascade taps in time domain
   f->T = (uint32_t)FILTER_WSINC_SAMPLES (f->tb, f->casc);
   f->T -= (f->T%2) ? 1:0;
   if (f->T < WSINC_MIN_TAPS)    f->T = WSINC_MIN_TAPS;

   // Calculate kernel points in frequency domain
   cT = (f->T * f->casc + 1)*2;
   for (i=1 ; i<UINT32_MAX ; i<<=1) {
      if (i>=cT) {
         f->N = i;
         break;
      }
   }

   // Try to allocate kernel in memory
   if ( (f->k = (void*)calloc (2*f->N, sizeof (double))) != NULL ) {
      // Despatch based on filter type
      switch (f->ftype) {
         default:
         case FILTER_LOW_PASS:
            _1tran_loop (f, 1);
            break;
         case FILTER_HIGH_PASS:
            _1tran_loop (f, -1);
            break;
         case FILTER_BAND_REJECT:
            _2tran_loop (f, 1);
            break;
         case FILTER_BAND_PASS:
            _2tran_loop (f, -1);
            break;
      }
      // Go to Frequency domain
      fft_r (f->k, (complex_d_t*)f->k, f->N);

      // Cascade filters
      for (i=0 ; i<f->casc ; ++i)
         vemul_cd ((complex_d_t*)f->k, (complex_d_t*)f->k, (complex_d_t*)f->k, f->N);

      return f->N;
   }
   else
      return 0;
}


void filter_wsinc (filter_wsinc_t *f, double *in, double *out, uint32_t n)
{
   fft_r (in, (complex_d_t*)out, n);

   vemul_cd ((complex_d_t*)out, (complex_d_t*)out, (complex_d_t*)f->k, n);
   ifft_r ((complex_d_t*)out, out, n);
}

