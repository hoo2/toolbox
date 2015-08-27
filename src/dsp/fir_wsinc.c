/*!
 * \file fir_wsinc.c
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

#include <dsp/fir_wsinc.h>
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

static uint32_t _blackman_taps (uint32_t c, double tb) {
   return (uint32_t)ceil( _WSINC_BLACKMAN_TAPS*c/tb);
}
static uint32_t _hamming_taps (uint32_t c, double tb) {
   return (uint32_t)ceil( _WSINC_HAMMING_TAPS*c/tb);
}
static uint32_t _barlett_taps (uint32_t c, double tb) {
   return (uint32_t)ceil( _WSINC_BARLETT_TAPS*c/tb);
}
static uint32_t _hanning_taps (uint32_t c, double tb) {
   return (uint32_t)ceil( _WSINC_HANNING_TAPS*c/tb);
}


static uint32_t _first_pow2_ge (uint32_t x) {
   uint32_t r;
   for (r=1 ; r<UINT32_MAX ; r<<=1) {
      if (r>x)
         return r;
   }
   return 0;
}


static void _1tran_loop (fir_wsinc_t *f, int sign) {
   uint32_t i, sT, n_2;
   int32_t  n;
   double fact;

   /*
    *  Find how many taps each filter stage is needing.
    *  This integer division will produce an error, that will effect
    *  transition bandwidth. So we update Taps but we accept the
    *  slightly worse rool-off.
    */
   sT = (f->T + f->casc - 1)/f->casc;
   sT += (sT%2) ? 0:1;                    // Tap must be odd number
   f->T = f->casc * sT - (f->casc - 1);   // The final emulated Tap number can be even

   // Calculate kernel and normalise factor
   n_2 = sT>>1;   // Take the half and make it even
   for (fact=i=0 ; i<=n_2 ; ++i) {
      n = i - n_2;
      f->k[i] = _sinc (f->fc1, n) * f->W (i, sT-1);
      fact += f->k[i] * 2;
      f->k[sT - 1 - i] = (f->k[i] *= sign);
   }
   fact -= f->k[n_2]*sign;
   fact = 1/fact;
   // Apply normalise factor
   for (i=0 ; i<=sT ; ++i)
      f->k[i] *= fact;

   // invert symmetry point if needed
   f->k[n_2] += (sign==-1) ? 1:0;
}

static void _2tran_loop (fir_wsinc_t *f, int sign) {
   uint32_t i, sT, n_2;
   int32_t  n;
   double fact;

   /*
    *  Find how many taps each filter stage is needing.
    *  This integer division will produce an error, that will effect
    *  transition bandwidth. So we update Taps but we accept the
    *  slightly worse rool-off.
    */
   sT = (f->T + f->casc - 1)/f->casc;
   f->T = f->casc * sT - (f->casc - 1);

   // Calculate kernel and normalise factor
   n_2 = sT>>1;   // Take the half and make it even
   for (fact=i=0 ; i<=n_2 ; ++i) {
      n = i - n_2;
      f->k[i] = ( _sinc (f->fc1, n) * f->W (i, sT-1) +
                  _sinc (f->fc2, n) * f->W (i, sT-1) );
      fact += f->k[i] * 2;
      f->k[sT - 1 - i] = (f->k[i] *= sign);
   }
   fact -= f->k[n_2]*sign;
   fact = 1/fact;
   // Apply normalise factor
   for (i=0 ; i<=sT ; ++i)
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
//void filter_wsinc_set_item_size (fir_wsinc_t *f, uint32_t size) {
//   f->it_size = size;
//}

/*!
 * \brief
 *    Set the filter type
 *
 * \param   f     Which filter to use
 * \param   t     Filter type
 *    \arg  FIR_LOW_PASS
 *    \arg  FIR_HIGH_PASS
 *    \arg  FIR_BAND_PASS
 *    \arg  FIR_BAND_REJECT
 * \return        none
*/
void fir_wsinc_set_ftype (fir_wsinc_t *f, fir_ftype_en t) {
   switch (t) {
      case FIR_LOW_PASS:
      case FIR_HIGH_PASS:
      case FIR_BAND_PASS:
      case FIR_BAND_REJECT:
         f->ftype = t;
         break;
      default:
         f->ftype = FIR_LOW_PASS;
         break;
   }
}

/*!
 * \brief
 *    Set the window type
 *
 * \param   f     Which filter to use
 * \param   w     Window type
 *    \arg  FIR_WSINC_BLACKMAN
 *    \arg  FIR_WSINC_HAMMING
 *    \arg  FIR_WSINC_BARLETT
 *    \arg  FIR_WSINC_HANNING
 * \return        none
*/
void fir_wsinc_set_wtype (fir_wsinc_t *f, fir_wtype_en w) {
   switch (w) {
      default:
      case FIR_WSINC_BLACKMAN:
         f->W = _blackman;
         f->tp = _blackman_taps;
         break;
      case FIR_WSINC_HAMMING:
         f->W = _hamming;
         f->tp = _hamming_taps;
         break;
      case FIR_WSINC_BARLETT:
         f->W = _barlett;
         f->tp = _barlett_taps;
         break;
      case FIR_WSINC_HANNING:
         f->W = _hanning;
         f->tp = _hanning_taps;
         break;
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
void fir_wsinc_set_fc (fir_wsinc_t *f, double fc1, double fc2) {
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
void fir_wsinc_set_tb (fir_wsinc_t *f, double tb) {
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
void fir_wsic_set_cascade (fir_wsinc_t *f, uint32_t c) {
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
void fir_wsinc_deinit (fir_wsinc_t* f) {
   if ( f->k )
      free ((void*)f->k);
   memset ((void*)f, 0, sizeof (fir_wsinc_t));
}


/*!
 * \brief
 *    Windowed sinc filter initialisation.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalised cutoff frequency [0fs - 0.5fs]
 * \return        None
 */
uint32_t fir_wsinc_init (fir_wsinc_t* f)
{
   uint32_t i, j, sT;
   double t;

   // Calculate taps in time domain
   f->T = f->tp(f->casc, f->tb);
   if (f->T < FIR_WSINC_MIN_TAPS)    f->T = FIR_WSINC_MIN_TAPS;

   // Calculate kernel points in frequency domain
   f->N = _first_pow2_ge (2*f->T);

   // Try to allocate kernel in memory
   if ( (f->k = (void*)calloc (2*f->N, sizeof (double))) != NULL &&
        (f->t = (void*)calloc (2*f->N, sizeof (double))) != NULL ) {
      // Despatch based on filter type
      switch (f->ftype) {
         default:
         case FIR_LOW_PASS:
            _1tran_loop (f, 1);
            break;
         case FIR_HIGH_PASS:
            _1tran_loop (f, -1);
            break;
         case FIR_BAND_REJECT:
            _2tran_loop (f, 1);
            break;
         case FIR_BAND_PASS:
            _2tran_loop (f, -1);
            break;
      }

      // Go to Frequency domain
      fft_r (f->k, (complex_d_t*)f->k, f->N);

      // Cascade filters
      for (i=1 ; i<f->casc ; ++i)
         vemul_cd ((complex_d_t*)f->k, (complex_d_t*)f->k, (complex_d_t*)f->k, f->N);

      return f->N;
   }
   else
      return 0;
}


void fir_wsinc (fir_wsinc_t *f, double *in, double *out, uint32_t n)
{
   uint32_t i, j, seg, out_sz;

   // Calculate segment and clear output signal
   seg = f->N - f->T + 1;
   out_sz = _first_pow2_ge(n);
   memset ((void*)out, 0, out_sz*sizeof (double));

   // Loop the filter
   for (i=0 ; i<n ; i+=seg) {
      memset ((void*)f->t, 0, 2*f->N*sizeof (double));         // Clear temporary table
      if (i+seg-1<=n)                                          // Copy input segment
         memcpy ((void*)f->t, (void*)&in[i], seg * sizeof (double));
      else  // last segment
         memcpy ((void*)f->t, (void*)&in[i], (n-i) * sizeof (double));
      fft_r (f->t, (complex_d_t*)f->t, f->N);                  // Transform input signal
      vemul_cd ((complex_d_t*)f->t, (complex_d_t*)f->t, (complex_d_t*)f->k, f->N);
                                                               // Frequency domain multiplication
      ifft_r ((complex_d_t*)f->t, f->t, f->N);                 // Transform back to time domain
      for (j=0 ; j<f->N && i+j<out_sz; ++j)                    // Output data
         out[j+i] += f->t[j];
   }
}

