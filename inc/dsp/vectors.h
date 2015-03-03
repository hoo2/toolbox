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

#include <stdint.h>
#include <complex.h>
#include <tbx_types.h>
#include <math.h>

/*
 * ================== Public API ====================
 */

void vadd_i (int *y, int *a, int *b, int length);
void vadd_f (float *y, float *a, float *b, int length);
void vadd_d (double *y, double *a, double *b, int length);
void vcadd_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length);
void vcadd_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length);

void vsub_i (int *y, int *a, int *b, int length);
void vsub_f (float *y, float *a, float *b, int length);
void vsub_d (double *y, double *a, double *b, int length);
void vcsub_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length);
void vcsub_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length);

void vemul_i (int *y, int *a, int *b, int length);
void vemul_f (float *y, float *a, float *b, int length);
void vemul_d (double *y, double *a, double *b, int length);
void vcemul_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length);
void vcemul_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length);

int vediv_i (int *y, int *a, int *b, int length);
int vediv_f (float *y, float *a, float *b, int length);
int vediv_d (double *y, double *a, double *b, int length);
int vcediv_i (complex_i_t *y, complex_i_t *a, complex_i_t *b, int length);
int vcediv_d (complex_d_t *y, complex_d_t *a, complex_d_t *b, int length);

int vdot_i (int *a, int *b, int length);
float vdot_f (float *a, float *b, int length);
double vdot_d (double *a, double *b, int length);
complex_i_t vcdot_i (complex_i_t *a, complex_i_t *b, int length);
complex_d_t vcdot_d (complex_d_t *a, complex_d_t *b, int length);

double vnorm_i (int *x, int length);
double vnorm_f (float *x, int length);
double vnorm_d (double *x, int length);
double vcnorm_i (complex_i_t *x, int length);
double vcnorm_d (complex_d_t *x, int length);

#if __STDC_VERSION__ >= 201112L

#ifndef vadd
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void vadd (T *y, T *a, T *b, int length);
 *
 * \note We still have to implement all the functions
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
      _Complex int*: vcadd_i,             \
   _Complex double*: vcadd_d,             \
            default: vadd_i)(y, a, b, length)
#endif   //#ifndef vadd


#ifndef vsub
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void vsub (T *y, T *a, T *b, int length);
 *
 * \note We still have to implement all the functions
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
      _Complex int*: vcsub_i,             \
   _Complex double*: vcsub_d,             \
            default: vsub_i)(y, a, b, length)
#endif   //#ifndef vsub


#ifndef vemul
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> void vemul (T *y, T *a, T *b, int length);
 *
 * \note We still have to implement all the functions
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
      _Complex int*: vcemul_i,               \
   _Complex double*: vcemul_d,               \
            default: vemul_i)(y, a, b, length)
#endif   //#ifndef vemul


#ifndef vediv
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> int vediv (T *y, T *a, T *b, int length);
 *
 * \note We still have to implement all the functions
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
      _Complex int*: vcediv_i,               \
   _Complex double*: vcediv_d,               \
            default: vediv_i)(y, a, b, length)
#endif   //#ifndef vediv


#ifndef vdot
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> T vdot (T *a, T *b, int length);
 *
 * \note We still have to implement all the functions
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
               int*: vdot_i,              \
             float*: vdot_f,              \
            double*: vdot_d,              \
      _Complex int*: vcdot_i,             \
   _Complex double*: vcdot_d,             \
            default: vdot_i)(a, b, length)
#endif   // #ifndef vdot


#ifndef vnorm
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T> T vnorm (T *x, int length);
 *
 * \note We still have to implement all the functions
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
      _Complex int*: vcnorm_i,            \
   _Complex double*: vcnorm_d,            \
            default: vnorm_i)(x, length)
#endif   // #ifndef vnorm

#endif   // #if __STDC_VERSION__ >= 201112L


void vcart (double *c, double *p);
void vccart (complex_d_t *c, double *p);
void vpolar (double *p, double *c);
void vcpolar (double *p, complex_d_t c);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __vectors_h__
