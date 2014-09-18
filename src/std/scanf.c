/*!
 * \file scanf.c
 * \brief
 *    A small footprint scanf with floating point support
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
 */
#include <std/scanf.h>

/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Reads a string from stdin.
 *
 * \param   dst  pointer to input string.
 * \return  The status os operation
 *    \arg  NULL on empty source.
 *    \arg  pointer on string to input string.
 */
char* gets (char *dst)
{
   char c=0, *str = dst;

   do {
      c = __getchar ();    // Get character from stdin
      // Filter input
      if (c == '\r')
         ;  // Get rid of WIN '\r' before '\n'
      else if (c == '\n' || c == -1)
         *dst = '\0';
      else
         *dst++ = c;
   } while (c != '\n' && c != -1);

   // Reset string pointer
   dst = str;

   // ANSI compatibility
   if ( (c == '\n' || c == -1) && str == dst)
      return 0;
   else
      return str;
}

/*!
 * \brief
 *    Outputs a formatted string on the stream. Format arguments are given
 *    in a va_list instance.
 *
 * \param frm     Format string.
 * \param ap      Argument list.
 */
inline int vscanf (const char *frm, __VALIST ap)
{
   // Forward call
   return vsxscanf (_getc_usr, (char*)0, (char *)frm, ap);
}

/*!
 * \brief
 *    Outputs a formatted string on the DBGU stream, using a variable number of
 *    arguments.
 *
 * \param  frm    Format string.
 */
int scanf (const char *frm, ...)
{
   __VALIST ap;
   int result;

   // Forward call to vscanf
   va_start(ap, (char *)frm);
   result = vscanf (frm, ap);
   va_end(ap);

   return result;
}

