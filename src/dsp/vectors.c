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
void vadd_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) { _vadd_body(); }
void vadd_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) { _vadd_body(); }
void vadd_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) { _vadd_body(); }
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
void vsub_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) { _vsub_body(); }
void vsub_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) { _vsub_body(); }
void vsub_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) { _vsub_body(); }
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
void vemul_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) { _vemul_body(); }
void vemul_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) { _vemul_body(); }
void vemul_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) { _vemul_body(); }
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
#define  _vediv_body_r() {                                     \
   /* Calculate vadd */                                        \
   for (--length ; length>=0 ; --length) {                     \
      if (b[length]!= 0)                                       \
         y[length] = a[length] / b[length];                    \
      else                                                     \
         return 1;                                             \
   }                                                           \
   return 0;                                                   \
}
#define  _vediv_body_c() {                                     \
   /* Calculate vadd */                                        \
   for (--length ; length>=0 ; --length) {                     \
      if (b[length]!= 0+I*0)                                   \
         y[length] = a[length] / b[length];                    \
      else                                                     \
         return 1;                                             \
   }                                                           \
   return 0;                                                   \
}
int vediv_i (int *y, int *a, int *b, int length) { _vediv_body_r(); }
int vediv_f (float *y, float *a, float *b, int length) {_vediv_body_r(); }
int vediv_d (double *y, double *a, double *b, int length) { _vediv_body_r(); }
int vediv_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) { _vediv_body_c(); }
int vediv_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) { _vediv_body_c(); }
int vediv_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) { _vediv_body_c(); }
#undef _vediv_body_r
#undef _vediv_body_c


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
#define  _vdot_body_r() {                                      \
   /* Calculate vdot */                                        \
   for (res=0,--length ; length>=0 ; --length) {               \
      res += a[length] * b[length];                            \
   }                                                           \
   return res;                                                 \
}

#define  _vdot_body_c() {                                      \
   /* Calculate vcdot */                                       \
   for (res=0,--length ; length>=0 ; --length) {               \
      res += conj(a[length]) * b[length];                      \
   }                                                           \
   return res;                                                 \
}
int vdot_i (int *a, int *b, int length) { int res; _vdot_body_r(); }
float vdot_f (float *a, float *b, int length) {float res;  _vdot_body_r(); }
double vdot_d (double *a, double *b, int length) { double res; _vdot_body_r(); }
complex_i_t vdot_ci (complex_i_t *a, complex_i_t *b, int length) {complex_i_t res; _vdot_body_c(); }
complex_f_t vdot_cf (complex_f_t *a, complex_f_t *b, int length) {complex_f_t res; _vdot_body_c(); }
complex_d_t vdot_cd (complex_d_t *a, complex_d_t *b, int length) {complex_d_t res; _vdot_body_c(); }

#undef _vdot_body_r
#undef _vdot_body_c

/*!
 * \brief
 *    Calculates the norm (length) of a vector
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
#define  _vnorm_body_r() {                                     \
   /* Calculate vnorm */                                       \
   for (res=0,--length ; length>=0 ; --length) {               \
      res += x[length] * x[length];                            \
   }                                                           \
   return sqrt (res);                                          \
}
#define  _vnorm_body_c() {                                     \
   /* Calculate vnorm */                                       \
   for (res=0+I*0,--length ; length>=0 ; --length) {           \
      res += conj(x[length]) * x[length];                      \
   }                                                           \
   return csqrt (res);                                         \
}
float vnorm_i (int *x, int length) { float res; _vnorm_body_r(); }
float vnorm_f (float *x, int length) { float res; _vnorm_body_r(); }
double vnorm_d (double *x, int length) { double res; _vnorm_body_r(); }
complex_f_t vnorm_ci (complex_i_t *x, int length) { complex_f_t res; _vnorm_body_c(); }
complex_f_t vnorm_cf (complex_f_t *x, int length) { complex_f_t res; _vnorm_body_c(); }
complex_d_t vnorm_cd (complex_d_t *x, int length) { complex_d_t res; _vnorm_body_c(); }
#undef _vnorm_body_r
#undef _vnorm_body_c


/*!
 * \brief
 *    Calculates the Cartesian coordinates from a polar vector
 *    of size two.
 *
 * \param   c  Pointer to Cartesian vector {x,y}
 * \param   p  Pointer to polar vector {r,th}
 * \return  none
 */
#define _vcart_body() {          \
   c[0] = p[0] * cos (p[1]);     \
   c[1] = p[0] * sin (p[1]);     \
}
void vcart_i (float *c, int *p) { _vcart_body(); }
void vcart_f (float *c, float *p) { _vcart_body(); }
void vcart_d (double *c, double *p) { _vcart_body(); }
#undef _vcart_body


/*!
 * \brief
 *    Calculates the polar coordinates from Cartesian vector
 *    of size two, or a complex number representing Cartesian
 *    coordinates.
 *
 * \param   p  Pointer to polar vector {r,th}
 * \param   c  Pointer to Cartesian vector {x,y}, or complex number.
 * \return  none
 */
#define _vpolar_body_r() {                \
   p[0] = sqrt (c[0]*c[0] + c[1]*c[1]);   \
   p[1] = atan2 (c[1], c[0]);             \
}
#define _vpolar_body_c() {                   \
   p[0] = sqrt (cc[0]*cc[0] + cc[1]*cc[1]);  \
   p[1] = atan2 (cc[1], cc[0]);              \
}
void vpolar_i (float *p, int *c) { _vpolar_body_r(); }
void vpolar_f (float *p, float *c) { _vpolar_body_r(); }
void vpolar_d (double *p, double *c) { _vpolar_body_r(); }
void vpolar_ci (float *p, complex_i_t c) { int *cc = (int*)&c; _vpolar_body_c(); }
void vpolar_cf (float *p, complex_f_t c) { float *cc = (float*)&c; _vpolar_body_c(); }
void vpolar_cd (double *p, complex_d_t c){ double *cc = (double*)&c; _vpolar_body_c(); }
#undef _vpolar_body_r
#undef _vpolar_body_c

