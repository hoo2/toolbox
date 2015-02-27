/*
 * \file corr.c
 * \brief
 *    A target independent cross-correlation functionality
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
#include <dsp/corr.h>


#define  _rcorr_body()                                         \
{                                                              \
   int sy, n, m, m_lo,m_hi;                                    \
                                                               \
   yp = y; /* Init variables */                                \
   sy = sx + st - 1;                                           \
                                                               \
   for (n=0 ; n<sy ; ++n, ++yp) {                              \
      *yp = 0;    /* Empty Accumulator */                      \
      /* Calculate boundaries */                               \
      m_lo = (m=n-st+1)>0 ? m:0;    /* x start */              \
      m_hi = n>sx-1 ? sx-1 : n;     /* x stop */               \
      tp = (st-n>0) ? t + st-n-1:t; /* tp start */             \
      xp = x + m_lo;                /* xp start */             \
      /* Do the correlation sum for the output point n */      \
      for (m=m_hi-m_lo ; m>=0 ; --m) {                         \
         *yp += *xp * *tp;                                     \
         ++xp;                                                 \
         ++tp;                                                 \
      }                                                        \
   }                                                           \
}

#define  _ccorr_body()                                         \
{                                                              \
   int sy, n, m, m_lo,m_hi;                                    \
                                                               \
   yp = y; /* Init variables */                                \
   sy = sx + st - 1;                                           \
                                                               \
   for (n=0 ; n<sy ; ++n, ++yp) {                              \
      *yp = 0;    /* Empty Accumulator */                      \
      /* Calculate boundaries */                               \
      m_lo = (m=n-st+1)>0 ? m : 0;  /* m start */              \
      m_hi = n>sx-1 ? sx-1 : n;     /* m stop */               \
      tp = (st-n>0) ? t + st-n-1:t; /* tp start */             \
      xp = x + m_lo;                /* xp start */             \
      /* Do the correlation sum for the output point n */      \
      for (m=m_hi-m_lo ; m>=0 ; --m) {                         \
         *yp += *xp * conj(*tp);                               \
         ++xp;                                                 \
         ++tp;                                                 \
      }                                                        \
   }                                                           \
}

/*!
 * \brief
 *    Calculates the cross-correlation of int t and x
 *
 *   y[n] = t[n] (x) x[n]
 *
 *            N-1
 * (t*x)[n] = Sum {t[m]*x[n+m]}
 *            m=0
 * n: [0 .. sizoef(t)+sizeof(x)-2]
 *
 * \param   y  Pointer to output vector
 * \param   t  Pointer to target vector, or signal 1
 * \param  st  Size of vector t
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void xcorr_i (int *y, int *t, int st, int *x, int sx) {
   int *yp, *tp, *xp;   // Use pointers for speed
   _rcorr_body();
}

/*!
 * \brief
 *    Calculates the cross-correlation of float t and x
 *
 *   y[n] = t[n] (x) x[n]
 *
 *            N-1
 * (t*x)[n] = Sum {t[m]*x[n+m]}
 *            m=0
 * n: [0 .. sizoef(t)+sizeof(x)-2]
 *
 * \param   y  Pointer to output vector
 * \param   t  Pointer to target vector, or signal 1
 * \param  st  Size of vector t
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void xcorr_f (float *y, float *t, int st, float *x, int sx) {
   float *yp, *tp, *xp;   // Use pointers for speed
   _rcorr_body();
}

/*!
 * \brief
 *    Calculates the cross-correlation of double t and x
 *
 *   y[n] = t[n] (x) x[n]
 *
 *            N-1
 * (t*x)[n] = Sum {t[m]*x[n+m]}
 *            m=0
 * n: [0 .. sizoef(t)+sizeof(x)-2]
 *
 * \param   y  Pointer to output vector
 * \param   t  Pointer to target vector, or signal 1
 * \param  st  Size of vector t
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void xcorr_d (double *y, double *t, int st, double *x, int sx) {
   double *yp, *tp, *xp;   // Use pointers for speed
   _rcorr_body();
}

/*!
 * \brief
 *    Calculates the cross-correlation of complex int t and x
 *
 *   y[n] = t[n] (x) x[n]
 *
 *            N-1
 * (t*x)[n] = Sum {t'[m]*x[n+m]}
 *            m=0
 * n: [0 .. sizoef(t)+sizeof(x)-2]
 *
 * \param   y  Pointer to output vector
 * \param   t  Pointer to target vector, or signal 1
 * \param  st  Size of vector t
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void cxcorr_i (_Complex int *y, _Complex int *t, int st, _Complex int *x, int sx) {
   _Complex int *yp, *tp, *xp;   // Use pointers for speed
   _ccorr_body();
}

/*!
 * \brief
 *    Calculates the cross-correlation of complex double t and x
 *
 *   y[n] = t[n] (x) x[n]
 *
 *            N-1
 * (t*x)[n] = Sum {t'[m]*x[n+m]}
 *            m=0
 * n: [0 .. sizoef(t)+sizeof(x)-2]
 *
 * \param   y  Pointer to output vector
 * \param   t  Pointer to target vector, or signal 1
 * \param  st  Size of vector t
 * \param   x  Pointer to input signal, or signal 2
 * \param  sx  Size of input signal
 *
 * \return none
 */
void cxcorr_d (_Complex double *y, _Complex double *t, int st, _Complex double *x, int sx) {
   _Complex double *yp, *tp, *xp;   // Use pointers for speed
   _ccorr_body();
}
#undef _rcorr_body
#undef _ccorr_body

