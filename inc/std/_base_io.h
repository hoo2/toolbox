/*
 * \file _base_io.h
 * \brief
 *    A small footprint stdio parser with floating point support
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
 *
 * \todo
 * 1. Implement the long long int type
 * 2. Implement the long double type
 * 3. Report the bug and fix the code
 *
 */

#ifndef __base_io_h__
#define __base_io_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <toolbox_defs.h>


#define  _IO_NUM_OF_TYPES    (16)
#define  _IO_NUM_OF_FLAGS    (7)

extern const char pr_let[_IO_NUM_OF_TYPES];
extern const char pr_flags[_IO_NUM_OF_FLAGS];

#define IS_ALL_BUT_PC(_c)  ((_c) != '%')  /*!< match all chars except '%' symbol */
#define IS_PC(_c)          ((_c) == '%')  /*!< match '%' symbol */
#define IS_ZERO(_c)        ((_c) == '0')  /*!< match '0'*/
#define IS_1TO9(_c)        ((_c) >= '1' && (_c) <= '9')  /*!< match 1 to 9 digits */
#define IS_0TO9(_c)        ((_c) >= '0' && (_c) <= '9')  /*!< match all digits */
#define IS_ATOF(_c)        ((_c) >= 'A' && (_c) <= 'F')  /*!< match HEX characters */
#define IS_aTOf(_c)        ((_c) >= 'a' && (_c) <= 'f')  /*!< match hex characters */
#define IS_DOT(_c)         ((_c) == '.')                 /*!< match '.' symbol */
#define IS_PLUS(_c)        ((_c) == '+')                 /*!< match '+' symbol */
#define IS_MINUS(_c)       ((_c) == '-')                 /*!< match '+' symbol */
#define IS_EXP(_c)         ((_c) == 'e' || (_c) == 'E')  /*!< match exponement sympol */

#define _IO_FRACTIONAL_WIDTH        (3)
#define _IO_WIDTH                   (5)
#define _IO_MAX_INT_DIGITS          (15)
#define _IO_MAX_DOUBLE_WIDTH        (20)

#define _IO_MAX_FLOAT               (1e18)
#define _IO_MAX_FLOAT_EXP           (18)
#define _IO_MIN_FLOAT_EXP           (-18)

/*!
 * Enumerator for parser's state machine.
 */
typedef enum
{
   ST_NONE=0,
   ST_PC,
   ST_FLAG,
   ST_WIDTH,
   ST_DOT,
   ST_FRAC,
   ST_TYPE,
   ST_ERROR
}_parser_st_t;

/*!
 * Enumerator for supported types.
 * \note
 *   The order must match the one found in pr_let \sa pr_let
 */
typedef enum
{
   INT_c, INT_d, FL_e, FL_E, FL_f, FL_g, FL_G,
   INT_i, INT_l, FL_L, INT_o, INT_s, INT_u, INT_x, INT_X, NO_TYPE
}_io_types_en;

/*!
 * Enumerator for supported flags.
 * \note
 *   The order must match the one found in pr_flags \sa pr_flags
 */
typedef enum
{
   FLAG_PLUS, FLAG_MINUS, FLAG_SPACE, FLAG_SHARP, FLAG_COMMA, FLAG_ZERO, NO_FLAG
}_io_flags_en;

/*!
 * Implementation dependent workaround to the va_arg() BUG.
 * Using 2x 32bit vars for 1x64bit :(
 */
typedef union
{
   unsigned int i[2];
   double d;
}_double_un_t;




typedef struct
{
   uint8_t  plus     :1;
   uint8_t  minus    :1;
   uint8_t  sharp    :1;
   char     lead;
}_io_flags_t;

typedef struct
{
   _io_types_en   type;
   _io_flags_t    flags;
   int            width;
   int            frac;
}_io_frm_spec_t;

typedef union
{
   int               character;
   _io_frm_spec_t    frm_specifier;
}_io_frm_obj_t;

typedef enum
{
   _IO_FRM_STREAM = 0,
   _IO_FRM_SPECIFIER,
   _IO_FRM_TERMINATOR,
   _IO_FRM_CRAP
}_io_frm_obj_type_en;

int __Os__ _io_read (char* frm, _io_frm_obj_t* obj, _io_frm_obj_type_en *obj_type);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __base_io_h__
