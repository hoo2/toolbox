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
int64_t vdot_i32 (int32_t *a, int32_t *b, int length) { int64_t res; _vdot_body_r(); }
uint64_t vdot_ui32 (uint32_t *a, uint32_t *b, int length) { uint64_t res; _vdot_body_r(); }
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
 *    using double precision
 *
 * \param   p  polar vector {r,th}
 * \return  The Cartesian value {x,y}
 */
inline cart2_d_t vcart_d (polar_d_t p){
   cart2_d_t c;
   c.x = p.abs * cos (p.arg);
   c.y = p.abs * sin (p.arg);
   return c;
}

/*!
 * \brief
 *    Calculates the Cartesian coordinates from a polar vector
 *    using single precision
 *
 * \param   p  polar vector {r,th}
 * \return  The Cartesian value {x,y}
 */
inline cart2_f_t vcart_f (polar_f_t p) {
   cart2_f_t c;
   c.x = p.abs * cosf (p.arg);
   c.y = p.abs * sinf (p.arg);
   return c;
}

/*!
 * \brief
 *    Calculates a complex number in Cartesian form from a polar vector
 *    using double precision
 *
 * \param   p  polar vector {r,th}
 * \return  The Complex number
 */
inline complex_d_t vccart_d (polar_d_t p){
   complex_d_t c;
   real(c) = p.abs * cos (p.arg);
   imag(c) = p.abs * sin (p.arg);
   return c;
}

/*!
 * \brief
 *    Calculates a complex number in Cartesian form from a polar vector
 *    using single precision
 *
 * \param   p  polar vector {r,th}
 * \return  The Complex number
 */
inline complex_f_t vccart_f (polar_f_t p) {
   complex_f_t c;
   realf(c) = p.abs * cosf (p.arg);
   imagf(c) = p.abs * sinf (p.arg);
   return c;
}

/*!
 * \brief
 *    Calculates the polar coordinates from Cartesian coordinates.
 *
 * \param   x  x coordinate
 * \param   y  y coordinate
 * \return     the converted to polar value
 */
polar_f_t vpolar_f (float x, float y) {
   polar_f_t p;
   p.abs = sqrtf (x*x + y*y);
   p.arg = atan2f (y, x);
   return p;
}

/*!
 * \brief
 *    Calculates the polar coordinates from Cartesian coordinates.
 *
 * \param   x  x coordinate
 * \param   y  y coordinate
 * \return     the converted to polar value
 */
polar_d_t vpolar_d (double x, double y) {
   polar_d_t p;
   p.abs = sqrt (x*x + y*y);
   p.arg = atan2 (y, x);
   return p;
}

/*!
 * \brief
 *    Calculates the polar coordinates from  a complex number
 *    representing Cartesian coordinates.
 *    a.k.a transform complex number to polar notation
 *
 * \param   c  The complex in Cartesian representation
 * \return     the converted to polar value
 */
polar_f_t vcpolar_f (complex_f_t c) {
   polar_f_t p;
   p.abs = sqrt (realf(c)*realf(c) + imagf(c)*imagf(c));
   p.arg = atan2 (imagf(c), realf(c));
   return p;
}

/*!
 * \brief
 *    Calculates the polar coordinates from  a complex number
 *    representing Cartesian coordinates.
 *    a.k.a transform complex number to polar notation
 *
 * \param   x  x coordinate
 * \return     the converted to polar value
 */
polar_d_t vcpolar_d (complex_d_t c) {
   polar_d_t p;
   p.abs = sqrt (real(c)*real(c) + imag(c)*imag(c));
   p.arg = atan2 (imag(c), real(c));
   return p;
}


