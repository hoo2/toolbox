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

#include <std/_vsxprintf.h>


/*
 * ============================ Public Functions ============================
 */
int vprintf(const char *pfrmt, va_list ap);
int printf(const char *pfrmt, ...);
int puts(const char *pdst);

/*!
 * Tailor this in order to connect printf functionality
 * to your hardware (stdout).
 */
extern int __putchar(char c);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __printf_h__
