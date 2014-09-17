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
   if ( (c == '\n' || c == -1) && str == pdst)
      return 0;
   else
      return str;
}

/*!
 * \brief
 *    Outputs a formatted string on the stream. Format arguments are given
 *    in a va_list instance.
 *
 * \param pfrmt  Format string.
 * \param ap  Argument list.
 */
inline int vscanf(const char *pfrmt, va_list ap)
{
   // Forward call NO buffer ;-)
   return vsxscanf (_putc_usr, (char*)0, pfrmt, ap);
}

/*!
 * \brief
 *    Outputs a formatted string on the DBGU stream, using a variable number of
 *    arguments.
 *
 * \param  pfrmt  Format string.
 */
int scanf (const char *pfrmt, ...)
{
   va_list ap;
   int result;

   // Forward call to vprintf
   va_start(ap, (char *)pfrmt);
   result = vscanf (pfrmt, ap);
   va_end(ap);

   return result;
}

