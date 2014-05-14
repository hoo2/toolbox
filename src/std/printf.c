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
int puts(const char *pdst)
{
   int num = 0;

   while (*pdst != 0)
   {
      if (__putchar(*pdst) == -1)
         return -1;
      num++;
      pdst++;
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
inline int vprintf(const char *pfrmt, va_list ap)
{
   // Forward call NO buffer ;-)
   return vsxprintf (_putc_usr, (char*)0, pfrmt, ap);
}

/*!
 * \brief
 *    Outputs a formatted string on the DBGU stream, using a variable number of
 *    arguments.
 *
 * \param  pfrmt  Format string.
 */
int printf(const char *pfrmt, ...)
{
   va_list ap;
   int result;

   // Forward call to vprintf
   va_start(ap, (char *)pfrmt);
   result = vprintf(pfrmt, ap);
   va_end(ap);

   return result;
}

