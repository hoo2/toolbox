/*!
 * \file sscanf.c
 * \brief
 *    A small footprint sscanf with floating point support
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
#include <std/sscanf.h>

/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Read formatted data from string into variable argument list.
 *
 * \param src      Source string.
 * \param frmt     Format string.
 * \param ap       Argument list.
 *
 * \return  The function returns the number of items in the argument list successfully filled.
 */
int vsscanf (char *src, const char *frmt, va_list ap)
{
   // Forward call NO buffer ;-)
   return vsxscanf (_getc_dst, src, frmt, ap);
}

/*!
 * \brief
 *    Read formatted data from string.
 *
 * \param src     source string.
 * \param frmt    Format string.
 *
 * \return        The number of items in the argument list successfully filled
 */
int sscanf (char *src, const char *frmt, ...)
{
   va_list ap;
   int result;

   // Forward call to vsscanf
   va_start(ap, (char *)frmt);
   result = vsscanf (src, frmt, ap);
   va_end(ap);

   return result;
}
