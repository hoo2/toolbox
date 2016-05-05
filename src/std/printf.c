/*!
 * \file printf.c
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
 */
#include <std/printf.h>

/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Outputs a string on stdout.
 *
 * \param pdst  String to output.
 */
__Os__ int puts(const char *dst)
{
   int num = 0;

   while (*dst != 0)
   {
      if (__putchar(*dst) == -1)
         return -1;
      num++;
      dst++;
   }
   return num;
}

/*!
 * \brief
 *    Outputs a formatted string on the DBGU stream. Format arguments are given
 *    in a va_list instance.
 *
 * \param pfrmt  Format string.
 * \param ap  Argument list.
 */
inline int vprintf(const char *frm, __VALIST ap)
{
   // Forward call NO buffer ;-)
   return vsxprintf (_putc_usr, (char *)0, (char *)frm, ap);
}

/*!
 * \brief
 *    Outputs a formatted string on the DBGU stream, using a variable number of
 *    arguments.
 *
 * \param  pfrmt  Format string.
 */
__Os__ int printf(const char *frm, ...)
{
   __VALIST ap;
   int result;

   // Forward call to vprintf
   va_start(ap, (char *)frm);
   result = vprintf(frm, ap);
   va_end(ap);

   return result;
}

