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
#define IS_ASTERISK(_c)    ((_c) == '*')                 /*!< match '*' symbol */
#define IS_EXP(_c)         ((_c) == 'e' || (_c) == 'E')  /*!< match exponement sympol */

#define _IO_FRACTIONAL_WIDTH        (3)
#define _IO_WIDTH                   (5)
#define _IO_MAX_INT_DIGITS          (15)
#define _IO_MAX_INT32_DIGITS        (15)
#define _IO_MAX_INT64_DIGITS        (22)
#define _IO_MAX_DOUBLE_WIDTH        (20)

#define _IO_MAX_FLOAT               (1e18)
#define _IO_MAX_FLOAT_EXP           (18)
#define _IO_MIN_FLOAT_EXP           (-18)

/*!
 * Enumerator for parser's state machine.
 */
typedef enum
{
   _IO_ST_NONE=0,
   _IO_ST_PC,
   _IO_ST_FLAG,
   _IO_ST_WIDTH,
   _IO_ST_DOT,
   _IO_ST_FRAC,
   _IO_ST_TYPE,
   _IO_ST_ERROR
}_parser_st_t;

/*!
 * Enumerator for supported types.
 * \note
 *   The order must match the one found in pr_let \sa pr_let
 *   const char pr_let[_IO_NUM_OF_TYPES] = "cdeEfgGilLosuxX";
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

/*!
 * Base io format flags
 */
typedef struct {
   uint8_t  plus     :1;   /*!< format specifier has plus + */
   uint8_t  minus    :1;   /*!< format specifier has minus - */
   uint8_t  sharp    :1;   /*!< format specifier has sharp # */
   uint8_t  vwidth   :1;   /*!< format specifier has an asterisk *, for ex "%*d"
                            *   This is variable width field and expects width from va_list
                            *   as next argument
                            */
   uint8_t  vfrac    :1;   /*!< format specifier has an asterisc * after . for ex "%.*f"
                            *   This is variable frac field and expects width from va_list
                            *   as next argument
                            */
   char     lead;
}_io_flags_t;

/*!
 * Base IO format specifier type to hold the information of
 * the current specifier in format string
 */
typedef struct {
   _io_types_en   type;    /*!< The type of the variable */
   _io_flags_t    flags;   /*!< The flags enabled */
   int            width;   /*!< The desired width */
   int            frac;    /*!< The desired fractional part width */
}_io_frm_spec_t;

/*!
 * Base IO object type, to describe a caracter in format string
 * or a format specifier.
 */
typedef union {
   int               character;        /*!< Stream character */
   _io_frm_spec_t    frm_specifier;    /*!< Format specifier */
}_io_frm_obj_t;

/*!
 * The possible types of objects of type \ref _io_frm_obj_t
 */
typedef enum {
   _IO_FRM_STREAM = 0,     /*!< The object is a stram character */
   _IO_FRM_SPECIFIER,      /*!< The object is a Format specifier */
   _IO_FRM_TERMINATOR,     /*!< The object is a string terminator */
   _IO_FRM_CRAP            /*!< The object is crap, we dont know hot to parse it */
}_io_frm_obj_type_en;


#define  __io_init_frm_obj(_obj_)   _obj_ = {      \
      .frm_specifier.type = INT_c,                 \
      .frm_specifier.flags = {0, 0, 0, 0, 0, ' '}, \
      .frm_specifier.width = 0,                    \
      .frm_specifier.frac = 0                      \
}

int __Os__ _io_read (char* frm, _io_frm_obj_t* obj, _io_frm_obj_type_en *obj_type);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __base_io_h__
