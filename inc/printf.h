/*
 * \file printf.h
 * \brief
 *    A small footprint printf with floating point support
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
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
 */

/*!
 * \mainpage printf
 *
 * \section Description
 *    A small footprint printf with floating point support
 *
 * \section Supported Conversions
 *
 * The library understands all (I think) valid conversion exept the 2 letter types.
 * Some conversions are interpreted as is, some interpreted different and some others ignored.
 *
 * This library supports 2 levels of conversions.
 * * The full level.
 *    Just compile the dahm think
 * * Tiny level. (#define PRINTF_TINY)
 *    Define PRINTF_TINY in compile time and see the code and data size to srink.
 *<pre>
 * ================================================================
 * TYPE     FULL  FULL_CONVERSION      TINY  TINY_CONVERSION
 * ----------------------------------------------------------------
 * %i      YES     singed int         YES    singed int
 * %d      YES     singed int         YES    singed int
 * %u      YES    unsinged int        YES   unsinged int
 * %l      NO     (%d)                NO    (%d)
 * %x      YES    unsinged int        NO    (%u)
 * %X      YES    unsinged int        NO    (%u)
 * %o      NO     (%x)                NO    (%u)
 * ----------------------------------------------------------------
 * %c      YES    char                YES   char
 * %s      YES    char *              YES   char *
 * ----------------------------------------------------------------
 * %f      YES    double              YES   double
 * %L      NO     (%f)                NO    (%f)
 * %e      YES    double              NO    (%f)
 * %E      NO     (%e)                NO    (%f)
 * %g      NO     (%f)                NO    (%f)
 * %G      NO     (%f)                NO    (%f)
 * NaN     YES    "NaN"               NO     --
 * INF     YES    "INF"               NO     --
 * OTHERS  NO     (%u)                NO    (%u)  (NOTE)
 *</pre>
 *
 * NOTE: ALL other convertion types are interpreted as unsigned int (%u).
 *
 * <pre>
 * Flags:
 * -----------
 * '+'      Always include a plus or minus sign in the result.
 * '0'      Pad the field with zeros instead of spaces. The zeros are placed after any indication of sign or base.
 *
 * ' '      If the result doesn't start with a plus or minus sign, prefix it with a space instead.
 *          Since the '+' flag ensures that the result includes a sign, this flag is ignored if you supply both of them.
 *
 * '-'      Not supported, ignored
 * '''      Not supported, ignored
 * '#'      Not supported, ignored
 * '*'      Not supported, ignored
 *</pre>
 *
 * \section Supported API
 *
 * This library supports 2 level of API functions
 *
 * 1) Base
 *    * int vsnprintf(char *pdst, size_t length, const char *pfrm, va_list ap);
 *    * int snprintf(char *pString, size_t length, const char *pfrmt, ...);
 *    * int vsprintf(char *pString, const char *pfrmt, va_list ap);
 *    * int vprintf(const char *pfrmt, va_list ap);
 *    * int printf(const char *pfrmt, ...);
 *    * int sprintf(char *pdst, const char *pfrmt, ...);
 *    * int puts(const char *pdst);
 *
 * 2) File functions
 *    --All the above -- plus
 *    * int vfprintf(FILE *fp, const char *pfrmt, va_list ap);
 *    * int fprintf(FILE *fp, const char *pfrmt, ...);
 *    * int fputc(int c, FILE *fp);
 *    * int fputs(const char *pdst, FILE *fp);
 *
 * To enable these just define PRINTF_FILES. This "#define" also changes the behavor
 * of puts and vprintf, so they use fputs().
 *
 * \section Tailing
 *
 * In order to link and work YOU MUST DEFINE a __putchar()
 * function somewhere in your code. See the function below as
 * an example.
 *
 * <pre>
 * int  __putchar(char c)
 * {
 *    int ch = USART_Send(c);    //This is example
 *    return ch;                 //return the written character
 * }
 * </pre>
 *
 * \todo
 * 1. Implement the long long int type
 * 2. Implement the long double type
 * 3. Report the bug and fix the code
 *
 */

#ifndef __printf_h__
#define __printf_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

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
 *  Maximum string size allowed (in bytes).
 */
#define PF_MAX_STRING_SIZE          (0x20)   // 36 characters

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
 * Tailor this in order to connect printf functionality
 * to your hardware.
 */
extern int __putchar(char c);

/*
 * ============================ Public Functions ============================
 */

int vsnprintf(char *pdst, size_t length, const char *pfrm, va_list ap);
int snprintf(char *pString, size_t length, const char *pfrmt, ...);
int vsprintf(char *pString, const char *pfrmt, va_list ap);
int vprintf(const char *pfrmt, va_list ap);
int printf(const char *pfrmt, ...);
int sprintf(char *pdst, const char *pfrmt, ...);
int puts(const char *pdst);

#ifdef PRINTF_FILES
int vfprintf(FILE *fp, const char *pfrmt, va_list ap);
int fprintf(FILE *fp, const char *pfrmt, ...);
int fputc(int c, FILE *fp);
int fputs(const char *pdst, FILE *fp);
#endif   //#ifdef PRINTF_FILES


#ifdef __cplusplus
 }
#endif

#endif //#ifndef __printf_h__
