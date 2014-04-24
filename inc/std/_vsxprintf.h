/*
 * \file _vsxprintf.h
 * \brief
 *    A small footprint printf with floating point support
 *
 * this file is part of toolbox (std part)
 *
 * Copyright (C) 2014 Houtouridis Christos <houtouridis.ch@gmail.com>
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
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       06/2013
 * Version:
 *
 *
 * \todo
 * 1. Implement the long long int type
 * 2. Implement the long double type
 * 3. Report the bug and fix the code
 *
 */

#ifndef __vsxprintf_h__
#define __vsxprintf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>

#define IS_ALL_BUT_PC(_c)  ((_c) != '%')  /*!< match all chars except '%' symbol */
#define IS_PC(_c)          ((_c) == '%')  /*!< match '%' symbol */
#define IS_ZERO(_c)        ((_c) == '0')  /*!< match '0'*/
#define IS_1TO9(_c)        ((_c) >= '1' && (_c)<='9') /*!< match 1 to 9 digits */
#define IS_0TO9(_c)        ((_c) >= '0' && (_c)<='9') /*!< match all digits */
#define IS_DOT(_c)         ((_c) == '.')              /*!< match '.' symbol */
#define IS_PLUS(_c)        ((_c) == '+')              /*!< match '+' symbol */

#define PF_FRACTIONAL_WIDTH         (3)
#define PF_WIDTH                    (5)
#define PF_MAX_INT_DIGITS           (15)

/*!
 * Enumerator for supported types.
 * \note
 *   The order must match the one found in pr_let \sa pr_let
 */
typedef enum
{
   INT_c, INT_d, FL_e, FL_E, FL_f, FL_g, FL_G,
   INT_i, INT_l, FL_L, INT_o, INT_s, INT_u, INT_x, INT_X, NO_TYPE
}_printf_types_en;

/*!
 * Enumerator for supported flags.
 * \note
 *   The order must match the one found in pr_flags \sa pr_flags
 */
typedef enum
{
   FLAG_PLUS, FLAG_MINUS, FLAG_SPACE, FLAG_SHARP, FLAG_COMMA, FLAG_ZERO, NO_FLAG
}_printf_flags_en;

/*!
 * Enumerator for parser's state machine.
 */
typedef enum
{
   ST_STREAM=0,
   ST_PC,
   ST_FLAG,
   ST_WIDTH,
   ST_FRAC,
   ST_TYPE,
}_parser_st_t;

/*!
 * Implementation depentend workaround to the va_arg() BUG.
 * Using 2x 32bit vars for 1x64bit :(
 */
typedef union
{
   unsigned int i[2];
   double d;
}_double_un_t;


/*!
 * callback function pointer to use in:
 *
 *  1) user's __putchar ()
 *  2) destination string for sprintf family
 *  3) file destination (fprintf family)
 */
typedef int (*_putc_out_t) (char *, const char );

int _putc_usr (char *dst, const char c);  /*!< back end for user's device stdout */
int _putc_dst (char *dst, const char c);  /*!< back end for sprintf family */
int _putc_fil (char *dst, const char c);  /*!< back end for file printf family */

/*
 * ============================ Public Functions ============================
 */

int vsxprintf(_putc_out_t _putc_out, char *dst, const char *pfrm, va_list ap);

/*!
 * Tailor this in order to connect printf functionality
 * to your hardware (stdout).
 */
extern int __putchar(char c);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __vsxprintf_h__
