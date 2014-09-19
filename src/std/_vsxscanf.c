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

static int      _isspace (char c);
static int       _isterm (char c);
static int  _stream_getc (_getc_in_t _in, const char *src);
static int  _stream_copy (_getc_in_t _in, const char *src, char *dst);

static int   _read_char (_getc_in_t _in, const char *src, char *ch);
static int _read_string (_getc_in_t _in, const char *src, char *dst);


/*
 * Helper functions
 */

/*!
 * \brief
 *    Return true (1) if the given character \a c is whitespace.
 *    White space characters are "C" LOCALE:
 *       ' ', '\t', '\n', '\v', '\f' and '\r'
 *
 * \param   c  The character to check
 * \return     The status of operation
 *    \arg  0  Non-whitespace
 *    \arg  1  Whitespace
 */
static int _isspace (char c)
{
   if (c == ' ' || c == '\t' || c == '\n' ||
       c == '\v' || c == '\f' || c == '\r')
      return 1;
   else
      return 0;
}

/*!
 * \brief
 *    Return true (1) if the given character \a c is a terminator.
 *   Terminator characters are:
 *       '\0' and EOF (-1)
 *
 * \param   c  The character to check
 * \return     The status of operation
 *    \arg  0  Non-terminator
 *    \arg  1  Terminator
 */
static int _isterm (char c)
{
   if (c == '\0' || c == -1 )
      return 1;
   else
      return 0;
}

/*!
 * \brief
 *    Writes and return the first non-whitespace character from the stream to "fake"
 *    buffer's head and update the pointers.
 *
 * \param   _in   callback function to use for input streaming
 * \param   src   Destination string (if any).
 *
 * \return        The non-whitespace character from the stream
 */
static int _stream_getc (_getc_in_t _in, const char *src)
{
   int ch;
   // Search for the first non-whitespace character
   ch = _in (src, _GETC_HEAD);
   while ( _isspace (ch) && !_isterm (ch) )
      ch = _in (src, _GETC_NEXT);

   return ch;
}

/*!
 * \brief
 *    Copy the non-whitespace characters from the stream to \a dst until
 *    a whitespace or termination character appears
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param   dst   The pointer to return the first non-whitespace character
 *
 * \return        The number of non-whitespace character from the stream
 */
static int _stream_copy (_getc_in_t _in, const char *src, char *dst)
{
   int ch, n=0;
   // Search for the first whitespace character
   ch = _in (src, _GETC_HEAD);
   while ( ! (_isspace (ch) || _isterm (ch)) ) {
      *dst++ = ch;
      ch = _in (src, _GETC_NEXT);
      ++n;
   }
   *dst = 0;   // Destination string termination
   return n;
}

/*
 * Tailoring functions
 */

inline int _getc_usr (const char *src, _io_getc_read_en mode)
{
   static int buffer, ret=0;

   // Init last
   if (ret == 0)  buffer = __getchar();
   switch (mode) {
      case _GETC_HEAD:  return buffer;
      default:
      case _GETC_READ:  ret = buffer; buffer = __getchar(); return ret;
      case _GETC_NEXT:  return buffer = __getchar();
   }
}
inline int _getc_src (const char *src, _io_getc_read_en mode)
{
   switch (mode) {
      case _GETC_HEAD:  return *src;
      default:
      case _GETC_READ:  return *src++;
      case _GETC_NEXT:  return *++src;
   }
}
inline int _getc_fil (const char *src, _io_getc_read_en mode)
{
   return 0;
}




/*!
 * Conversion functions.
 */

/*!
 * \brief
 *    Copy the input to a single character
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param   ch    The pointer to return the character
 *
 * \return        The number of input stream characters read.
 */
static int _read_char (_getc_in_t _in, const char *src, char *ch)
{
   *ch = _in (src, _GETC_READ);
   return 1;
}

/*!
 * \brief
 *    Copy the input string to a string pointer
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param   dst   The pointer to return the string
 *
 * \return        The number of input stream characters read.
 */
static int _read_string (_getc_in_t _in, const char *src, char *dst)
{
   return _stream_copy (_in, src, dst);
}

/*!
 * \brief
 *    Convert the unsigned int from input stream
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param   obj   The format specifier object
 * \param   dst   The pointer to return the string
 *
 * \return        The number of input stream characters read.
 */
static int _read_uint (_getc_in_t _in, const char *src, _io_frm_obj_t *obj, char *dst)
{
   //_strcpy_tw (in, dst);
   return 0;
}
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
 * \return  The number of characters read.
 */


int vsxscanf (_getc_in_t _in, const char *src, const char *frm, __VALIST ap)
{
   _io_frm_obj_t  obj;              // object place holder
   _io_frm_obj_type_en  obj_type;   // object type place holder
   int arg=0;                       // Number of parsed arguments

   while (*frm != 0) {
      // Read the format string and skip spaces
      while (1) {
         frm += _io_read ((char *)frm, &obj, &obj_type);
         if (obj_type != _IO_FRM_STREAM)
            break;
         else if (!_isspace (obj.character))
            break;
      }

      // Dispatch based on object type
      switch (obj_type) {
         case _IO_FRM_STREAM:
            if (obj.character != _stream_getc (_in, src)) {
               // Matching error
               return arg;
            }
            // Discard matching character
            _in (src, _GETC_NEXT);
            break;

         case _IO_FRM_SPECIFIER:
            if (obj.frm_specifier.type == INT_d ||
                obj.frm_specifier.type == INT_i ||
                obj.frm_specifier.type == INT_l)
               ;//arg += _insint(_out, dst, &obj.frm_specifier, 0, _va_arg(ap, signed int*));
            else if (obj.frm_specifier.type == INT_u)
               ;//arg += _insuint(_out, dst, &obj.frm_specifier, va_arg(ap, unsigned int));
            else if (obj.frm_specifier.type == INT_x ||
                     obj.frm_specifier.type == INT_X ||
                     obj.frm_specifier.type == INT_o)
               ;//arg += _inshex(_out, dst, &obj.frm_specifier, va_arg(ap, unsigned int));
            else if (obj.frm_specifier.type == INT_c)
               _read_char (_in, src, va_arg(ap, char*));
            else if (obj.frm_specifier.type == INT_s)
               _read_string (_in, src, va_arg(ap, char*));
            else if (obj.frm_specifier.type == FL_f ||
                     obj.frm_specifier.type == FL_g ||
                     obj.frm_specifier.type == FL_G ||
                     obj.frm_specifier.type == FL_L)
               ;//arg += _insfdouble (_out, dst, &obj.frm_specifier, va_arg(ap, double)); // XXX
            else if (obj.frm_specifier.type == FL_e ||
                     obj.frm_specifier.type == FL_E)
               ;//arg += _insedouble (_out, dst, &obj.frm_specifier, va_arg(ap, double)); // XXX
            else  // eat the wrong type to unsigned int
               ;//arg += _insuint(_out, dst, &obj.frm_specifier, va_arg(ap, unsigned int));

            ++arg;
            break;
            /*
             * XXX: BUG(s)
             * All printf with double calls MUST HAVE ONLY ONE ARGUMENT, THE double.
             * 1) When va_arg(ap, double) comes in even argument number, it has 4bytes crap in front of it.
             * 2) When va_arg(ap, double) comes in odd argument number, it fails cause it skips 4bytes before reading.
             * Someone have miss-correct a bug i think.
             */

         case _IO_FRM_TERMINATOR:
            return arg;

         case _IO_FRM_CRAP:
            return arg;
      }
   }
   return (int)(0);
}
