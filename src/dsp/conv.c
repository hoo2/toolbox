/*
 * \file conv.c
 * \brief
 *    A target independent convolution functionality
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
#include <dsp/conv.h>


#define  _conv_body() {                               \
   int n, k, sy, kmin, kmax;                          \
                                                      \
   sy = sx + sh - 1;                                  \
   --sx; --sh; /* Convert to last point */            \
   for (n=0; n<sy; ++n) {                             \
      /* Find convolution range - zero padding */     \
      kmax = (n - sh < 0)       ? n : sh;             \
      kmin = ((k = n - sx) > 0) ? k : 0;              \
      for (y[n]=0, k=kmin; k<=kmax; ++k)              \
         y[n] += h[k] * x[n-k];  /* Do the sum */     \
  }                                                   \
}



/*!
 * \brief
 *    Calculates the convolution of int h and x
 *              ______
 *             |      |
 *    x[n]---> | h[h] | ---> y[n]
 *             |______|
 *
 *   y[n] = x[n] * h[n]
 *
 *            N-1
 * (x*h)[n] = Sum (h[m]*x[n-m])
 *            m=0
 * n: [0 .. sizoef(x)+sizeof(h)-2]
 *
 * \param   y  Pointer to output vector
 * \param   h  Pointer to system vector, or signal 1
 * \param  sh  Size of vector h
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void conv_i (int *y, int *h, uint32_t sh, int *x, uint32_t sx) {
   _conv_body();
}

/*!
 * \brief
 *    Calculates the convolution of float h and x
 *              ______
 *             |      |
 *    x[n]---> | h[h] | ---> y[n]
 *             |______|
 *
 *   y[n] = x[n] * h[n]
 *
 *            N-1
 * (x*h)[n] = Sum (h[m]*x[n-m])
 *            m=0
 * n: [0 .. sizoef(x)+sizeof(h)-2]
 *
 * \param   y  Pointer to output vector
 * \param   h  Pointer to system vector, or signal 1
 * \param  sh  Size of vector h
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void conv_f (float *y, float *h, uint32_t sh, float *x, uint32_t sx) {
   _conv_body();
}

/*!
 * \brief
 *    Calculates the convolution of double h and x
 *              ______
 *             |      |
 *    x[n]---> | h[h] | ---> y[n]
 *             |______|
 *
 *   y[n] = x[n] * h[n]
 *
 *            N-1
 * (x*h)[n] = Sum (h[m]*x[n-m])
 *            m=0
 * n: [0 .. sizoef(x)+sizeof(h)-2]
 *
 * \param   y  Pointer to output vector
 * \param   h  Pointer to system vector, or signal 1
 * \param  sh  Size of vector h
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void conv_d (double *y, double *h, uint32_t sh, double *x, uint32_t sx) {
   _conv_body();
}

/*!
 * \brief
 *    Calculates the convolution of complex int h and x
 *              ______
 *             |      |
 *    x[n]---> | h[h] | ---> y[n]
 *             |______|
 *
 *   y[n] = x[n] * h[n]
 *
 *            N-1
 * (x*h)[n] = Sum (h[m]*x[n-m])
 *            m=0
 * n: [0 .. sizoef(x)+sizeof(h)-2]
 *
 * \param   y  Pointer to output vector
 * \param   h  Pointer to system vector, or signal 1
 * \param  sh  Size of vector h
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void conv_ci (complex_i_t *y, complex_i_t *h, uint32_t sh, complex_i_t *x, uint32_t sx) {
   _conv_body();
}

/*!
 * \brief
 *    Calculates the convolution of complex float h and x
 *              ______
 *             |      |
 *    x[n]---> | h[h] | ---> y[n]
 *             |______|
 *
 *   y[n] = x[n] * h[n]
 *
 *            N-1
 * (x*h)[n] = Sum (h[m]*x[n-m])
 *            m=0
 * n: [0 .. sizoef(x)+sizeof(h)-2]
 *
 * \param   y  Pointer to output vector
 * \param   h  Pointer to system vector, or signal 1
 * \param  sh  Size of vector h
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void conv_cf (complex_f_t *y, complex_f_t *h, uint32_t sh, complex_f_t *x, uint32_t sx) {
   _conv_body();
}

/*!
 * \brief
 *    Calculates the convolution of complex double h and x
 *              ______
 *             |      |
 *    x[n]---> | h[h] | ---> y[n]
 *             |______|
 *
 *   y[n] = x[n] * h[n]
 *
 *            N-1
 * (x*h)[n] = Sum (h[m]*x[n-m])
 *            m=0
 * n: [0 .. sizoef(x)+sizeof(h)-2]
 *
 * \param   y  Pointer to output vector
 * \param   h  Pointer to system vector, or signal 1
 * \param  sh  Size of vector h
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void conv_cd (complex_d_t *y, complex_d_t *h, uint32_t sh, complex_d_t *x, uint32_t sx) {
   _conv_body();
}
#undef _conv_body

