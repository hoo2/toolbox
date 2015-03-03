/*
 * \file vectors.c
 * \brief
 *    A target independent vector basic functionalities
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
#include <dsp/vectors.h>

/*
 * ================== Public API ====================
 */

/*!
 * \brief
 *    Calculates the addition of a and b
 *
 *   y[n] = a[n] + b[n]
 *
 * \param      y  Pointer to output vector
 * \param      a  Pointer to target vector a
 * \param      b  Pointer to target vector b
 * \param length  Size of vectors
 *
 * \return none
 */
#define  _vadd_body() {                                        \
   /* Calculate vadd */                                        \
   for (--length ; length>=0 ; --length) {                     \
      y[length] = a[length] + b[length];                       \
   }                                                           \
}
void vadd_i (int *y, int *a, int *b, int length) { _vadd_body(); }
void vadd_f (float *y, float *a, float *b, int length) {_vadd_body(); }
void vadd_d (double *y, double *a, double *b, int length) { _vadd_body(); }
void vcadd_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) {
   _vadd_body();
}
void vcadd_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) {
   _vadd_body();
}
#undef _vadd_body

/*!
 * \brief
 *    Calculates the substruction of a and b
 *
 *   y[n] = a[n] - b[n]
 *
 * \param      y  Pointer to output vector
 * \param      a  Pointer to target vector a
 * \param      b  Pointer to target vector b
 * \param length  Size of vectors
 *
 * \return none
 */
#define  _vsub_body() {                                        \
   /* Calculate vsub */                                        \
   for (--length ; length>=0 ; --length) {                     \
      y[length] = a[length] - b[length];                       \
   }                                                           \
}
void vsub_i (int *y, int *a, int *b, int length) { _vsub_body(); }
void vsub_f (float *y, float *a, float *b, int length) { _vsub_body(); }
void vsub_d (double *y, double *a, double *b, int length) { _vsub_body(); }
void vcsub_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) {
   _vsub_body();
}
void vcsub_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) {
   _vsub_body();
}
#undef _vsub_body


/*!
 * \brief
 *    Calculates the element-wise multiplication of a and b
 *
 *   y[n] = a[n] .* b[n]
 *
 * \param      y  Pointer to output vector
 * \param      a  Pointer to target vector a
 * \param      b  Pointer to target vector b
 * \param length  Size of vectors
 *
 * \return none
 */
#define  _vemul_body() {                                       \
   /* Calculate vadd */                                        \
   for (--length ; length>=0 ; --length) {                     \
      y[length] = a[length] * b[length];                       \
   }                                                           \
}
void vemul_i (int *y, int *a, int *b, int length) { _vemul_body(); }
void vemul_f (float *y, float *a, float *b, int length) {_vemul_body(); }
void vemul_d (double *y, double *a, double *b, int length) { _vemul_body(); }
void vcemul_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) {
   _vemul_body();
}
void vcemul_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) {
   _vemul_body();
}
#undef _vemul_body


/*!
 * \brief
 *    Calculates the element-wise right division  a / b
 *
 *   y[n] = a[n] ./ b[n]
 *
 * \param      y  Pointer to output vector
 * \param      a  Pointer to target vector a
 * \param      b  Pointer to target vector b
 * \param length  Size of vectors
 *
 * \return The status of operation
 *    \arg  0  Success
 *    \arg  1  Fail, divide by zero
 */
#define  _vediv_body() {                                       \
   /* Calculate vadd */                                        \
   for (--length ; length>=0 ; --length) {                     \
      if (b[length]!= 0)                                       \
         y[length] = a[length] / b[length];                    \
      else                                                     \
         return 1;                                             \
   }                                                           \
   return 0;                                                   \
}
#define  _vcediv_body() {                                      \
   /* Calculate vadd */                                        \
   for (--length ; length>=0 ; --length) {                     \
      if (b[length]!= 0+0j)                                    \
         y[length] = a[length] / b[length];                    \
      else                                                     \
         return 1;                                             \
   }                                                           \
   return 0;                                                   \
}
int vediv_i (int *y, int *a, int *b, int length) { _vediv_body(); }
int vediv_f (float *y, float *a, float *b, int length) {_vediv_body(); }
int vediv_d (double *y, double *a, double *b, int length) { _vediv_body(); }
int vcediv_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) {
   _vcediv_body();
}
int vcediv_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) {
   _vcediv_body();
}
#undef _vediv_body
#undef _vcediv_body


/*!
 * \brief
 *    Calculates the dot product of a and b
 *
 *                     N-1
 *   r = <a[n],b[n]> = Sum {a'[m]*b[n]}
 *                     n=0
 *
 * \param      a  Pointer to target vector a
 * \param      b  Pointer to target vector b
 * \param length  Size of vectors
 *
 * \return The dot product of a and b
 */
#define  _vdot_body() {                                        \
   /* Calculate vdot */                                        \
   for (res=0,--length ; length>=0 ; --length) {               \
      res += a[length] * b[length];                            \
   }                                                           \
   return res;                                                 \
}

#define  _vcdot_body() {                                       \
   /* Calculate vcdot */                                       \
   for (res=0,--length ; length>=0 ; --length) {               \
      res += conj(a[length]) * b[length];                      \
   }                                                           \
   return res;                                                 \
}

int vdot_i (int *a, int *b, int length) { int res; _vdot_body(); }
float vdot_f (float *a, float *b, int length) {float res;  _vdot_body(); }
double vdot_d (double *a, double *b, int length) { double res; _vdot_body(); }
complex_i_t vcdot_i (complex_i_t *a, complex_i_t *b, int length) {complex_i_t res; _vcdot_body(); }
complex_d_t vcdot_d (complex_d_t *a, complex_d_t *b, int length) {complex_d_t res; _vcdot_body(); }

#undef _vdot_body
#undef _vcdot_body

/*!
 * \brief
 *    Calculates the norm (length) of a vactor
 *                      _______________
 *       ||    ||      / N-1
 *   r = ||x[n]||  =  /  Sum x[m]^2
 *       ||    ||2  \/   n=0
 *
 * \param      x  Pointer to target vector a
 * \param length  Size of vector
 *
 * \return The norm of vector
 */
#define  _vnorm_body() {                                       \
   /* Calculate vnorm */                                       \
   for (res=0,--length ; length>=0 ; --length) {               \
      res += x[length] * x[length];                            \
   }                                                           \
   return sqrt (res);                                          \
}
#define  _vcnorm_body() {                                      \
   /* Calculate vnorm */                                       \
   for (res=0,--length ; length>=0 ; --length) {               \
      res += conj(x[length]) * x[length];                      \
   }                                                           \
   return sqrt (creal(res));                                   \
}
double vnorm_i (int *x, int length) { double res; _vnorm_body(); }
double vnorm_f (float *x, int length) { double res; _vnorm_body(); }
double vnorm_d (double *x, int length) { double res; _vnorm_body(); }
double vcnorm_i (complex_i_t *x, int length) { double res; _vcnorm_body(); }
double vcnorm_d (complex_d_t *x, int length) { double res; _vcnorm_body(); }

#undef _vnorm_body
#undef _vcnorm_body


/*!
 * \brief
 *    Calculates the Cartesian coordinates from polar vector
 *
 * \param   c  Pointer to Cartesian vector {x,y}
 * \param   p  Pointer to polar vector {r,th}
 * \return  none
 */
void vcart (double *c, double *p) {
   c[0] = p[0] * cos (p[1]);
   c[1] = p[0] * sin (p[1]);
}

/*!
 * \brief
 *    Calculates the a complex number in Cartesian
 *    coordinates from polar vector
 *
 * \param   c  Pointer to complex
 * \param   p  Pointer to polar vector {r,th}
 * \return  none
 */
void vccart (complex_d_t *c, double *p) {
   *c = p[0] * cos (p[1]) + 1j*(p[0] * sin (p[1]));
}

/*!
 * \brief
 *    Calculates the polar coordinates from Cartesian vector
 *
 * \param   p  Pointer to polar vector {r,th}
 * \param   c  Pointer to Cartesian vector {x,y}
 * \return  none
 */
void vpolar (double *p, double *c) {
   p[0] = sqrt (c[0]*c[0] + c[1]*c[1]);
   p[1] = atan2 (c[1], c[0]);
}

/*!
 * \brief
 *    Calculates the polar coordinates from complex number
 *
 * \param   p  Pointer to polar vector {r,th}
 * \param   c  complex number
 * \return  none
 */
void vcpolar (double *p, complex_d_t c)
{
   double x=creal (c) , y=cimag (c);

   p[0] = sqrt (x*x + y*y);
   p[1] = atan2 (y, x);
}
