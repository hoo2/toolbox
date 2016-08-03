/*
 * \file vectors.h
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

#ifndef __vectors_h__
#define __vectors_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <dsp/dsp.h>

/*
 * ================== Public API ====================
 */

void vadd_i (int *y, int *a, int *b, int length) __O3__ ;
void vadd_f (float *y, float *a, float *b, int length) __O3__ ;
void vadd_d (double *y, double *a, double *b, int length) __O3__ ;
void vadd_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) __O3__ ;
void vadd_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) __O3__ ;
void vadd_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vadd
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void vadd (T *y, T *a, T *b, int length);
 *
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
#define vadd(y, a, b, length) _Generic((y),  \
               int*: vadd_i,              \
             float*: vadd_f,              \
            double*: vadd_d,              \
       complex_i_t*: vadd_ci,             \
       complex_f_t*: vadd_cf,             \
       complex_d_t*: vadd_cd,             \
            default: vadd_d)(y, a, b, length)
#endif   // #ifndef vadd
#endif   // #if __STDC_VERSION__ >= 201112L


void vsub_i (int *y, int *a, int *b, int length) __O3__ ;
void vsub_f (float *y, float *a, float *b, int length) __O3__ ;
void vsub_d (double *y, double *a, double *b, int length) __O3__ ;
void vsub_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) __O3__ ;
void vsub_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) __O3__ ;
void vsub_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vsub
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void vsub (T *y, T *a, T *b, int length);
 *
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
#define vsub(y, a, b, length) _Generic((y),  \
               int*: vsub_i,              \
             float*: vsub_f,              \
            double*: vsub_d,              \
       complex_i_t*: vsub_ci,             \
       complex_f_t*: vsub_cf,             \
       complex_d_t*: vsub_cd,             \
            default: vsub_d)(y, a, b, length)
#endif   // #ifndef vsub
#endif   // #if __STDC_VERSION__ >= 201112L


void vemul_i (int *y, int *a, int *b, int length) __O3__ ;
void vemul_f (float *y, float *a, float *b, int length) __O3__ ;
void vemul_d (double *y, double *a, double *b, int length) __O3__ ;
void vemul_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) __O3__ ;
void vemul_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) __O3__ ;
void vemul_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vemul
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void vemul (T *y, T *a, T *b, int length);
 *
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
#define vemul(y, a, b, length) _Generic((y), \
               int*: vemul_i,                \
             float*: vemul_f,                \
            double*: vemul_d,                \
       complex_i_t*: vemul_ci,               \
       complex_f_t*: vemul_cf,               \
       complex_d_t*: vemul_cd,               \
            default: vemul_d)(y, a, b, length)
#endif   // #ifndef vemul
#endif   // #if __STDC_VERSION__ >= 201112L


int vediv_i (int *y, int *a, int *b, int length) __O3__ ;
int vediv_f (float *y, float *a, float *b, int length) __O3__ ;
int vediv_d (double *y, double *a, double *b, int length) __O3__ ;
int vediv_ci (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length) __O3__ ;
int vediv_cf (complex_f_t *y, complex_f_t *a, complex_f_t *b, int length) __O3__ ;
int vediv_cd (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vediv
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> int vediv (T *y, T *a, T *b, int length);
 *
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
#define vediv(y, a, b, length) _Generic((y), \
               int*: vediv_i,                \
             float*: vediv_f,                \
            double*: vediv_d,                \
       complex_i_t*: vediv_ci,               \
       complex_f_t*: vediv_cf,               \
       complex_d_t*: vediv_cd,               \
            default: vediv_d)(y, a, b, length)
#endif   // #ifndef vediv
#endif   // #if __STDC_VERSION__ >= 201112L


int64_t vdot_i32 (int32_t *a, int32_t *b, int length) __O3__ ;
uint64_t vdot_ui32 (uint32_t *a, uint32_t *b, int length) __O3__ ;
float vdot_f (float *a, float *b, int length) __O3__ ;
double vdot_d (double *a, double *b, int length) __O3__ ;
complex_i_t vdot_ci (complex_i_t *a, complex_i_t *b, int length) __O3__ ;
complex_f_t vdot_cf (complex_f_t *a, complex_f_t *b, int length) __O3__ ;
complex_d_t vdot_cd (complex_d_t *a, complex_d_t *b, int length) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vdot
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> T vdot (T *a, T *b, int length);
 *
 * \brief
 *    Calculates the norm product of a and b
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
#define vdot(a, b, length) _Generic((a),  \
           int32_t*: vdot_i32,            \
          uint32_t*: vdot_ui32,           \
             float*: vdot_f,              \
            double*: vdot_d,              \
       complex_i_t*: vdot_ci,             \
       complex_f_t*: vdot_cf,             \
       complex_d_t*: vdot_cd,             \
            default: vdot_d)(a, b, length)
#endif   // #ifndef vdot
#endif   // #if __STDC_VERSION__ >= 201112L


float vnorm_i (int *x, int length) __O3__ ;
float vnorm_f (float *x, int length) __O3__ ;
double vnorm_d (double *x, int length) __O3__ ;
complex_f_t vnorm_ci (complex_i_t *x, int length) __O3__ ;
complex_f_t vnorm_cf (complex_f_t *x, int length) __O3__ ;
complex_d_t vnorm_cd (complex_d_t *x, int length) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vnorm
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T, typename TT> TT vnorm (T *x, int length);
 *
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
#define vnorm(x, length) _Generic((x),    \
               int*: vnorm_i,             \
             float*: vnorm_f,             \
            double*: vnorm_d,             \
       complex_i_t*: vnorm_ci,            \
       complex_f_t*: vnorm_cf,            \
       complex_d_t*: vnorm_cd,            \
            default: vnorm_d)(x, length)
#endif   // #ifndef vnorm

#endif   // #if __STDC_VERSION__ >= 201112L



cart2_d_t vcart_d (polar_d_t p) __O3__ ;
cart2_f_t vcart_f (polar_f_t p) __O3__ ;
complex_d_t vccart_d (polar_d_t p) __O3__;
complex_f_t vccart_f (polar_f_t p) __O3__;

#if __STDC_VERSION__ >= 201112L
#ifndef vcart
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T, typename TT> T vcart (TT p);
 *
 * \brief
 *    Calculates the Cartesian coordinates from a polar vector
 *
 * \param   p  polar vector {r,th}
 * \return  The Cartesian value {x,y}
 */
#define vcart(p) _Generic((p),            \
          polar_f_t: vcart_f,             \
          polar_d_t: vcart_d,             \
            default: vcart_d)(p)
#endif   // #ifndef vcart

#ifndef vccart
 /*!
  * A pseudo type-polymorphism mechanism using _Generic macro
  * to simulate:
  *
  * template<typename T, typename TT> T vccart (TT p);
  *
  * \brief
 *    Calculates a complex number in Cartesian form from a polar vector
  *
  * \param   p  polar vector {r,th}
  * \return  The Complex number
  */
 #define vccart(p) _Generic((p),           \
           polar_f_t: vccart_f,            \
           polar_d_t: vccart_d,            \
             default: vccart_d)(p)
#endif // #ifndef vccart

#endif   // #if __STDC_VERSION__ >= 201112L


polar_f_t vpolar_f (float x, float y) __O3__ ;
polar_d_t vpolar_d (double x, double y) __O3__ ;
polar_f_t vcpolar_f (complex_f_t c) __O3__ ;
polar_d_t vcpolar_d (complex_d_t c) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vpolar

/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T, typename T> polar_x_t vpolar (T x, T y);
 *
 *    Calculates the polar coordinates from Cartesian coordinates.
 *
 * \param   x  x coordinate
 * \param   y  y coordinate
 * \return     the converted to polar value
 */
#define vpolar(x, y) _Generic((x),        \
             float: vpolar_f,             \
            double: vpolar_d,             \
            default: vpolar_d)(x, y)
#endif   // #ifndef vpolar
#endif   // #if __STDC_VERSION__ >= 201112L


#if __STDC_VERSION__ >= 201112L
#ifndef vcpolar

/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T, typename T> polar_x_t vcpolar (T c);
 *
 * \brief
 *    Calculates the polar coordinates from  a complex number
 *    representing Cartesian coordinates.
 *    a.k.a transform complex number to polar notation
 *
 * \param   c  The complex in Cartesian representation
 * \return     the converted to polar value
 */
#define vcpolar(c) _Generic((c),          \
       complex_f_t: vcpolar_f,            \
       complex_d_t: vcpolar_d,            \
           default: vcpolar_d)(c)
#endif   // #ifndef vcpolar
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif // #ifndef __vectors_h__
