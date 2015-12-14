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


int vdot_i (int *a, int *b, int length) __O3__ ;
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
               int*: vdot_i,              \
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


void vcart_i (float *c, int *p) __O3__ ;
void vcart_f (float *c, float *p) __O3__ ;
void vcart_d (double *c, double *p) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vcart
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T, typename TT> void vcart (T *c, TT *p);
 *
 * \brief
 *    Calculates the Cartesian coordinates from a polar vector
 *    of size two.
 *
 * \param   c  Pointer to Cartesian vector {x,y}
 * \param   p  Pointer to polar vector {r,th}
 * \return  none
 */
#define vcart(c, p) _Generic((p),         \
               int*: vcart_i,             \
             float*: vcart_f,             \
            double*: vcart_d,             \
            default: vcart_d)(c, p)
#endif   // #ifndef vcart
#endif   // #if __STDC_VERSION__ >= 201112L


void vpolar_i (float *p, int *c) __O3__ ;
void vpolar_f (float *p, float *c) __O3__ ;
void vpolar_d (double *p, double *c) __O3__ ;
void vpolar_ci (float *p, complex_i_t c) __O3__ ;
void vpolar_cf (float *p, complex_f_t c) __O3__ ;
void vpolar_cd (double *p, complex_d_t c) __O3__ ;

#if __STDC_VERSION__ >= 201112L
#ifndef vpolar
/*!
 * A pseudo type-polymorphism mechanism using _Generic macro
 * to simulate:
 *
 * template<typename T, typename TT> void vpolar (T *c, TT *p);
 *
 * \brief
 *    Calculates the polar coordinates from Cartesian vector
 *    of size two, or a complex number representing Cartesian
 *    coordinates.
 *
 * \param   p  Pointer to polar vector {r,th}
 * \param   c  Pointer to Cartesian vector {x,y}, or complex number.
 * \return  none
 */
#define vpolar(p, c) _Generic((c),        \
               int*: vpolar_i,            \
             float*: vpolar_f,            \
            double*: vpolar_d,            \
        complex_i_t: vpolar_ci,           \
        complex_f_t: vpolar_cf,           \
        complex_d_t: vpolar_cd,           \
            default: vpolar_d)(p, c)
#endif   // #ifndef vcart
#endif   // #if __STDC_VERSION__ >= 201112L

#ifdef __cplusplus
}
#endif

#endif // #ifndef __vectors_h__
