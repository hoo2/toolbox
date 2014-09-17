/*
 * \file _base_io.c
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
#include <std/_base_io.h>

/*!
 * The supported type conversion characters.
 * \note They MUST be in the same order as _printf_types_en \sa _printf_types_en
 */
const char pr_let[_IO_NUM_OF_TYPES] = "cdeEfgGilLosuxX";

/*!
 * The supported(or not) flag characters.
 * \note They MUST be in the same order as _printf_flags_en \sa _printf_flags_en
 */
const char pr_flags[IO_NUM_OF_FLAGS] = "+- #'0";

/*!
 * \brief
 *    Find if the current character is a type conversion
 *    character.
 *
 * \param   The character to check.
 * \return  The type in an enum form.
 */
static _io_types_en _istype (char c)
{
   int i;
   for (i=0 ; i<_IO_NUM_OF_TYPES ; ++i)
      if (pr_let[i] == c)
         return (_io_types_en)(i);
   return NO_TYPE;
}

/*!
 * \brief
 *    Find if the current character is a flag character.
 *
 * \param   The character to check.
 * \return  The flag in an enum form.
 */
static _io_flags_en _isflag (char c)
{
   int i;
   for (i=0 ; i<_IO_NUM_OF_FLAGS ; ++i)
      if (pr_flags[i] == c)
         return (_io_flags_en)(i);
   return NO_FLAG;
}


/*!
 * \param   frm         The format string to parse
 * \param   obj         Pointer to return the read object
 * \param   obj_type    Pointer to return the read object type
 *    \arg  _IO_FRM_STREAM
 *    \arg  _IO_FRM_SPECIFIER
 * \return        The number of character that have been read
 */
int _io_read (char* frm, _io_frm_obj_t* obj, _io_frm_obj_type_en *obj_type)
{
   int count = 0;    // The read character

   // String check
   if (*frm == 0 || *frm == -1) {
      obj = obj_type = (void*)0;
      return 0;
   }
   /*
    * Parse string
    * Note: Try to parse all the libc/newlib valid conversions and to
    *       ignore the not supported.
    */
   if (IS_ALL_BUT_PC (*frm)) {
      obj->character = *frm;
      *obj_type = _IO_FRM_STREAM;
      return 1;
   }
   else
      while (1) {
         state = ST_PC;
         switch (state) {
            case ST_PC:
               if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               else if (IS_1TO9(*frm)) {
                  state = ST_WIDTH;
                  width = (width*10) + *frm-'0';
               }
               else if ((cf = _isflag (*frm)) != NO_FLAG)
                  state = ST_FLAG;
               else if (IS_DOT (*frm))
                  state = ST_FRAC;
               else if (IS_PC (*frm)) {
                  state = ST_STREAM;
                  _out (dst++, *frm);  ++size;
               }
               break;
            case ST_FLAG:
               if (cf == FLAG_PLUS)       sign = 1;
               else if (cf == FLAG_SPACE) lead = ' ';
               else if (cf == FLAG_ZERO)  lead = '0';

               if (IS_1TO9(*frm)) {
                  state = ST_WIDTH;
                  width = (width*10) + *frm-'0';
               }
               else if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               else if ((cf = _isflag (*frm)) != NO_FLAG)
                  ;
               break;
            case ST_WIDTH:
               if (IS_0TO9(*frm))
                  width = (width*10) + *frm-'0';
               else if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               else if (IS_DOT (*frm))
                  state = ST_FRAC;
               break;
            case ST_FRAC:
               if (IS_0TO9(*frm))
                  frac = (frac*10) + *frm-'0';
               else if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               break;
            case ST_TYPE:
               ++arg;   //We have next argument

            // Clear supported flags
            width = sign = frac = 0;
            lead = ' ';
            state = ST_STREAM;

            // update pointers
            dst += num;
            size += num;
            break;
         }
      }

   if (state != ST_TYPE)
      ++frm;


   // NULL-terminated (final \0 is not counted)
   _out (dst, 0);
   return size;
}
