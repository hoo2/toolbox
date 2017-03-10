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
const char pr_flags[_IO_NUM_OF_FLAGS] = "+- #'0";

/*!
 * \brief
 *    Find if the current character is a type conversion
 *    character.
 *
 * \param   The character to check.
 * \return  The type in an enum form.
 */
__Os__ static _io_types_en _istype (char c)
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
__Os__ static _io_flags_en _isflag (char c)
{
   int i;
   for (i=0 ; i<_IO_NUM_OF_FLAGS ; ++i)
      if (pr_flags[i] == c)
         return (_io_flags_en)(i);
   return NO_FLAG;
}


/*!
 * \brief
 *    Scan format string and extract stream characters or
 *    format specifiers as objects.
 *
 * \param   frm         The format string to parse
 * \param   obj         Pointer to return the read object
 * \param   obj_type    Pointer to return the read object type
 *    \arg  _IO_FRM_STREAM
 *    \arg  _IO_FRM_SPECIFIER
 * \return        The number of character that have been read
 */
int _io_read (char* frm, _io_frm_obj_t* obj, _io_frm_obj_type_en *obj_type)
{
   #define _skip_char(_c)     do { pc=*_c; ++_c; ++count; }while (0)

   int count;                 // The read character
   _io_types_en   ct=NO_TYPE; // Temporary type holder
   _io_flags_en   cf=NO_FLAG; // Temporary flag holder
   char           pc;         // Previous character
   uint8_t        parse = 1;  // done parsing flag
   _parser_st_t   state;

   /*
    * Parse string
    * Note: Try to parse all the libc/newlib valid conversions and to
    *       ignore the not supported.
    */
   // String check
   if (*frm == 0 || *frm == -1) {
      obj->character = *frm;
      *obj_type = _IO_FRM_TERMINATOR;
      return 0;
   }
   else if (IS_ALL_BUT_PC (*frm)) {
      obj->character = *frm;
      *obj_type = _IO_FRM_STREAM;
      return 1;
   }
   else {
      // Clear and init data
      count = pc = 0;
      *obj_type = _IO_FRM_SPECIFIER;

      state = ST_PC; // init state machine
      while (parse) {
         switch (state) {
            case ST_NONE:
               state = ST_ERROR;
               break;
            case ST_PC:
               if ( IS_PC (pc) ) {
                  state = ST_NONE;
                  obj->character = *frm;
                  *obj_type = _IO_FRM_STREAM;
                  _skip_char (frm);
                  return count;
               }
               _skip_char (frm);    // Skip it

               // State switcher
               if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               else if ((cf = _isflag (*frm)) != NO_FLAG)
                  state = ST_FLAG;
               else if (IS_1TO9(*frm) || IS_ASTERISK(*frm))
                  state = ST_WIDTH;
               else if (IS_DOT (*frm))
                  state = ST_DOT;
               else if ( IS_PC (*frm) )
                  ;  // stay here
               else
                  state = ST_ERROR;
               break;

            case ST_FLAG:
               if (cf == FLAG_PLUS)       obj->frm_specifier.flags.plus = 1;
               else if (cf == FLAG_MINUS) obj->frm_specifier.flags.minus = 1;
               else if (cf == FLAG_SHARP) obj->frm_specifier.flags.sharp = 1;
               else if (cf == FLAG_SPACE) obj->frm_specifier.flags.lead = ' ';
               else if (cf == FLAG_ZERO)  obj->frm_specifier.flags.lead = '0';
               // Ignore all other flags
               _skip_char (frm);       // Skip it

               // State switcher
               if (IS_1TO9(*frm) || IS_ASTERISK(*frm))
                  state = ST_WIDTH;
               else if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               else if ((cf = _isflag (*frm)) != NO_FLAG)
                  ;   // stay here (delete previous flag)
               else if (IS_DOT (*frm))
                  state = ST_DOT;
               else
                  state = ST_ERROR;
               break;

            case ST_WIDTH:
               // dispatch width and variable width
               if (IS_ASTERISK(*frm))
                  obj->frm_specifier.flags.vwidth = 1;
               else
                  obj->frm_specifier.width = (obj->frm_specifier.width*10) + *frm-'0';
               _skip_char (frm);       // Skip it

               // State switcher
               if (IS_0TO9(*frm))
                  ;  //Stay here
               else if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               else if (IS_DOT (*frm))
                  state = ST_DOT;
               else
                  state = ST_ERROR;
               break;

            case ST_DOT:
               _skip_char (frm);       // Skip dot

               // State switcher
               if (IS_0TO9(*frm) || IS_ASTERISK(*frm))
                  state = ST_FRAC;
               else
                  state = ST_ERROR;
               break;

            case ST_FRAC:
               // dispatch width and variable width
               if (IS_ASTERISK(*frm))
                  obj->frm_specifier.flags.vfrac = 1;
               else
                  obj->frm_specifier.frac = (obj->frm_specifier.frac*10) + *frm-'0';
               _skip_char (frm);       // Skip it

               // State switcher
               if (IS_0TO9(*frm))
                  ;  // Stay here
               else if ((ct = _istype (*frm)) != NO_TYPE)
                  state = ST_TYPE;
               else
                  state = ST_ERROR;
               break;

            case ST_TYPE:
               obj->frm_specifier.type = ct;
               _skip_char (frm);       // Skip it
               parse = 0;              // done

               // State switcher
               state = ST_NONE;
               break;

            case ST_ERROR:
               // discard object
               memset ((void*)obj, 0, sizeof (_io_frm_obj_t));
               *obj_type = _IO_FRM_CRAP;
               parse = 0;              // done
               break;
         }
      }
   }

   /*
    *  Return count even in error
    */
   return count;
   #undef _skip_char
}
