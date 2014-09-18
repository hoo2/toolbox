/*!
 * \file _vsxscanf.c
 * \brief
 *    A small footprint scanf with floating point support
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
 */
#include <std/_vsxscanf.h>




/*
 * Tailoring functions
 */

inline int _getc_usr (const char *src)
{
   return (int)__getchar();
}
inline int _getc_src (const char *src)
{
   return (int)*src;
}
inline int _getc_fil (const char *src)
{
   return 0;
}




/*!
 * Conversion functions.
 */



/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Stores the result of a formatted string into another string. Format
 *    arguments are given in a va_list instance.
 *    First lexicon analysis is made to the pfrm
 *    Second the proper conversion function is called
 *    The procedure is continued until we reach \see PF_MAX_STRING_SIZE
 *    or NULL character..
 *
 * \param _in     callback function to use for input streaming
 * \param src     Destination string (if any).
 * \param frm     Format string.
 * \param ap      Argument list.
 *
 * \return  The number of characters written.
 */
int vsxscanf (_getc_in_t _in, const char *src, const char *frm, __VALIST ap)
{
   return 0;
}
