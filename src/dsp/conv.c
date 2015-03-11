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

#define  _conv_body()                                          \
{                                                              \
   int sy, n, m, m_lo,m_hi;                                    \
                                                               \
   yp = y; /* Init variables */                                \
   sy = sx + sh - 1;                                           \
                                                               \
   for (n=0 ; n<sy ; ++n, ++yp) {                              \
      *yp = 0;    /* Empty Accumulator */                      \
      /* Calculate boundaries */                               \
      m_lo = (m=n-sh+1)>0 ? m : 0;  /* m start */              \
      m_hi = n>sx-1 ? sx-1 : n;     /* m stop */               \
      hp = h + n-m_lo;              /* hp start */             \
      xp = x + m_lo;                /* xp start */             \
      /* Do the convolution sum for the output point n */      \
      for (m=m_hi-m_lo ; m>=0 ; --m) {                         \
         *yp += *xp * *hp;                                     \
         ++xp;                                                 \
         --hp;                                                 \
      }                                                        \
   }                                                           \
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
void conv_i (int *y, int *h, int sh, int *x, int sx) {
   int *yp, *hp, *xp;   // Use pointers for speed
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
void conv_f (float *y, float *h, int sh, float *x, int sx) {
   float *yp, *hp, *xp;   // Use pointers for speed
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
void conv_d (double *y, double *h, int sh, double *x, int sx) {
   double *yp, *hp, *xp;   // Use pointers for speed
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
void cconv_i (complex_i_t *y, complex_i_t *h, int sh, complex_i_t *x, int sx) {
   complex_i_t *yp, *hp, *xp;   // Use pointers for speed
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
void cconv_d (complex_d_t *y, complex_d_t *h, int sh, complex_d_t *x, int sx) {
   complex_d_t *yp, *hp, *xp;   // Use pointers for speed
   _conv_body();
}
#undef _conv_body
