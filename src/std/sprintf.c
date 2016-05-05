/*!
 * \file sprintf.c
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
#include <std/sprintf.h>


/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Stores the result of a formatted string into another string. Format
 *    arguments are given in a va_list instance.
 *
 * \param pdst     Destination string.
 * \param pfrmt    Format string.
 * \param ap       Argument list.
 *
 * \return  The number of characters written.
 */
inline int vsprintf(char *dst, const char *frm, va_list ap)
{
   // Forward call NO buffer ;-)
   return vsxprintf(_putc_dst, dst, (char *)frm, ap);
}



/*!
 * \brief
 *    Writes a formatted string inside another string.
 *
 * \param pdst   storage string.
 * \param pfrmt  Format string.
 */
__Os__ int sprintf(char *dst, const char *frm, ...)
{
   __VALIST ap;
   int result;

   // Forward call to vsprintf
   va_start(ap, (char *)frm);
   result = vsprintf(dst, frm, ap);
   va_end(ap);

   return result;
}

