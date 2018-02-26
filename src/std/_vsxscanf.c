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

static int         _isspace (char c);
static int          _isterm (char c);
static int   _is_int_number (char c);
static int   _is_hex_number (char c);
static int  _is_real_number (char c);

static int _stream_getfirst (_getc_in_t _in, const char *src, char **psrc);
static int     _number_copy (_getc_in_t _in, _number_copy_type_en t, const char *src, char **psrc, char *dst);

static int   _read_char (_getc_in_t _in, const char *src, char **psrc, char *ch);
static int _read_string (_getc_in_t _in, const char *src, char **psrc, char *dst);
static int   _read_uint (_getc_in_t _in, const char *src, char **psrc, unsigned int *dst);
static int    _read_int (_getc_in_t _in, const char *src, char **psrc, int *dst);
static int    _read_hex (_getc_in_t _in, const char *src, char **psrc, unsigned int *dst);
static int _read_ffloat (_getc_in_t _in, const char *src, char **psrc, float *dst);


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
   if (c == '\0' || c == -1)
      return 1;
   else
      return 0;
}

/*!
 * \brief
 *    Return true (1) if the given character \a c is part of a number
 *
 * \param   c  The character to check
 * \return     The status of operation
 *    \arg  0  Non number character
 *    \arg  1  Number character
 */
static int _is_int_number (char c)
{
   if ((c >= '0' && c <= '9') ||
       c == '-' || c == '+'
       )
      return 1;
   else
      return 0;
}

/*!
 * \brief
 *    Return true (1) if the given character \a c is part of a number
 *
 * \param   c  The character to check
 * \return     The status of operation
 *    \arg  0  Non number character
 *    \arg  1  Number character
 */
static int _is_hex_number (char c)
{
   if ((c >= '0' && c <= '9') ||
       (c >= 'A' && c <= 'F') ||
       (c >= 'a' && c <= 'f') ||
       c == '-' || c == '+' || c == 'x'
      )
      return 1;
   else
      return 0;
}

/*!
 * \brief
 *    Return true (1) if the given character \a c is part of a number
 *
 * \param   c  The character to check
 * \return     The status of operation
 *    \arg  0  Non number character
 *    \arg  1  Number character
 */
static int _is_real_number (char c)
{
   if ((c >= '0' && c <= '9') ||
       c == '.' ||
       c == 'e' || c == 'E' ||
       c == '-' || c == '+'
      )
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
 * \param  psrc   Caller's source string pointer address
 *
 * \return        The non-whitespace character from the stream
 */
static int _stream_getfirst (_getc_in_t _in, const char *src, char **psrc)
{
   int ch;
   // Search for the first non-whitespace character
   ch = _in (src, (char**)&src, _GETC_HEAD);
   while ( _isspace (ch) && !_isterm (ch) )
      ch = _in (src, (char**)&src, _GETC_NEXT);

   // Update caller's source pointer
   *psrc = (char *)src;
   return ch;
}

/*!
 * \brief
 *    Copy the number characters from the stream to \a dst until
 *    a whitespace or termination character appears
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param   dst   The pointer to return the first non-whitespace character
 *
 * \return        The number of number character from the stream
 */
static int _number_copy (_getc_in_t _in, _number_copy_type_en t, const char *src, char **psrc, char *dst)
{
   int ch, n=0;
   int (*_isnumber) (char);

   // Number type dispatch
   switch (t) {
      default:
      case _INT:     _isnumber = _is_int_number;   break;
      case _HEX:     _isnumber = _is_hex_number;   break;
      case _FLOAT:   _isnumber = _is_real_number;  break;
   }

   // Search for the first whitespace character
   ch = _in (src, (char**)&src, _GETC_HEAD);
   ++n;
   while ( _isnumber (ch) ) {
      *dst++ = ch;
      ch = _in (src, (char**)&src, _GETC_NEXT);
      ++n;
   }
   *dst = 0;               // Destination string termination
   *psrc = (char *)src;    // Update caller source pointer
   return n-1;
}

/*
 * Tailoring functions
 */

inline int _getc_usr (const char *src, char **psrc, _io_getc_read_en mode)
{
   static int buffer, ret=0;

   // Init last
   if (ret == 0)
      ret = buffer = __getchar();
   switch (mode) {
      case _GETC_HEAD:  return ret = buffer;
      default:
      case _GETC_READ:  ret = buffer; buffer = __getchar(); return ret;
      case _GETC_NEXT:  return ret = buffer = __getchar();
   }
}

inline int _getc_src (const char *src, char **psrc, _io_getc_read_en mode)
{
   int ret;

   switch (mode) {
      case _GETC_HEAD:  return *src;
      default:
      case _GETC_READ:  ret = *src; ++*psrc; return ret;
      case _GETC_NEXT:  ++*psrc; return *++src;
   }
}

inline int _getc_fil (const char *src, char **psrc, _io_getc_read_en mode)
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
 * \param  psrc   Caller's source string pointer address
 * \param   ch    The pointer to return the character
 *
 * \return        The number of input stream characters read.
 */
static int _read_char (_getc_in_t _in, const char *src, char **psrc, char *ch)
{
   *ch = _in (src, psrc, _GETC_READ);
   return 1;
}

/*!
 * \brief
 *    Copy the input string to a string pointer
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param  psrc   Caller's source string pointer address
 * \param   dst   The pointer to return the string
 *
 * \return        The number of input stream characters read.
 */
static int _read_string (_getc_in_t _in, const char *src, char **psrc, char *dst)
{
   int ch, n=0;
   // Search for the first whitespace character
   ch = _in (src, (char**)&src, _GETC_HEAD);
   ++n;
   while ( ! (_isspace (ch) || _isterm (ch)) ) {
      *dst++ = ch;
      ch = _in (src, (char**)&src, _GETC_NEXT);
      ++n;
   }
   *dst = 0;               // Destination string termination
   *psrc = (char *)src;    // Update caller source pointer
   return n-1;
}

/*!
 * \brief
 *    Convert the unsigned int from input stream
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param  psrc   Caller's source string pointer address
 * \param   dst   The pointer to return the number
 *
 * \return        The number of input stream characters read.
 */
static int _read_uint (_getc_in_t _in, const char *src, char **psrc, unsigned int *dst)
{
   char num_str[_IO_MAX_INT_DIGITS];      // number string
   unsigned int result = 0;               // The converted value
   int str_pos, num_pos;   // fake pointers
   int n = 0;              // The read characters
   char got_crap=0;        // Got crap flag

   // Init pointers
   num_pos = 1;
   str_pos = _number_copy (_in, _INT, src, psrc, num_str);
   if (str_pos == 0)
      return 0;
   else
      --str_pos;

   // Conversion loop
   do {
      if ( got_crap && IS_0TO9 (num_str[str_pos]) ) {
         // Start over
         got_crap = 0;
         num_pos = 1;
         result = (num_str[str_pos] - '0');
      }
      else if ( !got_crap && IS_0TO9 (num_str[str_pos]) )
         result += (num_str[str_pos] - '0') * num_pos;
      else {
         // crap, start over
         got_crap = 1;
      }
      // update pointers
      num_pos *= 10;
      ++n;
   } while (str_pos--);

   *dst = result;
   return n;
}

/*!
 * \brief
 *    Convert the signed int from input stream
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param  psrc   Caller's source string pointer address
 * \param   dst   The pointer to return the number
 *
 * \return        The number of input stream characters read.
 */
static int _read_int (_getc_in_t _in, const char *src, char **psrc, int *dst)
{
   char num_str[_IO_MAX_INT_DIGITS];   // number string
   int result = 0;                     // The converted value
   int str_pos, num_pos;   // fake pointers
   int n = 0;              // The read characters
   int sign = 1;           // The sign, start positive
   char got_crap=0;        // Got crap flag

   // Init pointers
   num_pos = 1;
   str_pos = _number_copy (_in, _INT, src, psrc, num_str);
   if (str_pos == 0)
      return 0;
   else
      --str_pos;

   // Conversion loop
   do {
      if ( got_crap && IS_0TO9 (num_str[str_pos]) ) {
         // Start over
         got_crap = 0;
         num_pos = sign = 1;
         result = (num_str[str_pos] - '0');
      }
      else if ( !got_crap && IS_0TO9 (num_str[str_pos]) )
         result += (num_str[str_pos] - '0') * num_pos;
      else if (!got_crap && IS_MINUS (num_str[str_pos]) )
         sign *= -1;
      else if (!got_crap && IS_PLUS (num_str[str_pos]) )
         ;  // Do not change sign
      else {
         // crap, start over
         got_crap = 1;
      }
      // update pointers
      num_pos *= 10;
      ++n;
   } while (str_pos--);

   *dst = result * sign;
   return n;
}

/*!
 * \brief
 *    Convert the unsigned int (hexadimal) from input stream
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param  psrc   Caller's source string pointer address
 * \param   dst   The pointer to return the number
 *
 * \return        The number of input stream characters read.
 */
static int _read_hex (_getc_in_t _in, const char *src, char **psrc, unsigned int *dst)
{
   char num_str[_IO_MAX_INT_DIGITS];      // number string
   unsigned int result = 0;               // The converted value
   int str_pos, num_pos;   // fake pointers
   int n = 0;              // The read characters
   enum {NORMAL, _X, _0, CRAP} state = NORMAL;

   // Init pointers
   num_pos = 1;
   str_pos = _number_copy (_in, _HEX, src, psrc, num_str);
   if (str_pos == 0)
      return 0;
   else
      --str_pos;

   // Conversion loop
   do {
      switch (state) {
         default:
         case NORMAL:
            if ( IS_0TO9 (num_str[str_pos]) )
               result += (num_str[str_pos] - '0') * num_pos;
            else if ( IS_ATOF (num_str[str_pos]) )
               result += (num_str[str_pos] - 'A' + 0x0A) * num_pos;
            else if ( IS_aTOf (num_str[str_pos]) )
               result += (num_str[str_pos] - 'a' + 0x0A) * num_pos;
            else if (num_str[str_pos] == 'x')
               state = _X;
            else
               state = CRAP;
            break;
         case _X:
            if (num_str[str_pos] == '0')
               state = _0;
            else
               state = CRAP;
            break;
         case _0:
            // More character? CRAP
            state = CRAP;
            /*
             * Do not break. Switch to CRAP NOW!!
             */
         case CRAP:
            if ( IS_0TO9 (num_str[str_pos]) ) {
               // Start over
               state = NORMAL;
               num_pos = 1;
               result = (num_str[str_pos] - '0');
            }
            else if ( IS_ATOF (num_str[str_pos]) ) {
               // Start over
               state = NORMAL;
               num_pos = 1;
               result = (num_str[str_pos] - 'A' + 0x0A);
            }
            else if ( IS_aTOf (num_str[str_pos]) ) {
               // Start over
               state = NORMAL;
               num_pos = 1;
               result = (num_str[str_pos] - 'a' + 0x0A);
            }
            break;
      }

      // update pointers
      num_pos *= 0x10;
      ++n;
   } while (str_pos--);

   *dst = result;
   return n;
}

/*!
 * \brief
 *    Convert the double from input stream
 *
 * \param   _in   Callback function to use for input streaming
 * \param   src   Destination string (if any).
 * \param  psrc   Caller's source string pointer address
 * \param   dst   The pointer to return the number
 *
 * \return        The number of input stream characters read.
 */
static int _read_ffloat (_getc_in_t _in, const char *src, char **psrc, float *dst)
{
   char num_str[_IO_MAX_DOUBLE_WIDTH];    // number string
   int n;                                 // The read characters
   int f;                                 // fractional index counters
   int n_dec=0;                           // Decimal part
   float n_frac=0;                        // Fractional part
   float frac_pos = 0.1;                  // Fractional pos/multiplier

   // Get string
   n = _number_copy (_in, _FLOAT, src, psrc, num_str);
   if (n == 0) return 0;
   else        --n;

   // Find dot if any
   for (f=0; f<_IO_MAX_DOUBLE_WIDTH; ++f) {
      if ( IS_DOT (num_str[f]) )
         break;
   }

   // Conversions
   if (f != _IO_MAX_DOUBLE_WIDTH) {
      // Fractional part conversion loop
      ++f;
      n_frac = 0;
      do {
         if ( IS_0TO9 (num_str[f]) )
            n_frac += (num_str[f] - '0') * frac_pos;
         else
            break;
         // update pointers
         frac_pos *= 0.1;
         ++f;
      } while (num_str[f] && f<_IO_MAX_DOUBLE_WIDTH);
   }
   _read_int (_getc_src, num_str, (char **)&num_str, &n_dec);

   if (n_dec<0)
      *dst = n_dec - n_frac;
   else
      *dst = n_dec + n_frac;
   return n;
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
   _io_frm_obj_t obj;               /* object place holder */
   _io_frm_obj_type_en  obj_type;   /* object type place holder */
   int arg=0;                       /* Number of parsed arguments */
   int ch=0;

   while (*frm != 0) {
      // Read the format string and skip spaces
      while (1) {
         frm += _io_read ((char *)frm, &obj, &obj_type);
         if (obj_type != _IO_FRM_STREAM)
            break;
         else if (!_isspace (obj.character))
            break;
      }
      // Skip source string's spaces
      if ((ch = _stream_getfirst (_in, src, (char**)&src)) == 0)
         return arg;

      // Dispatch based on object type
      switch (obj_type) {
         case _IO_FRM_STREAM:
            if (obj.character != ch) {
               // Matching error
               return arg;
            }
            // Discard matching character
            _in (src, (char**)&src, _GETC_NEXT);
            break;

         case _IO_FRM_SPECIFIER:
            // Variable width reading
            if (obj.frm_specifier.flags.vwidth)
               obj.frm_specifier.width = va_arg(ap, signed int);
            if (obj.frm_specifier.flags.vfrac)
               obj.frm_specifier.frac = va_arg(ap, signed int);

            // Type dispatcher
            if (obj.frm_specifier.type == INT_d ||
                obj.frm_specifier.type == INT_i ||
                obj.frm_specifier.type == INT_l)
               _read_int (_in, src, (char**)&src, va_arg(ap, int*));
            else if (obj.frm_specifier.type == INT_u)
               _read_uint (_in, src, (char**)&src, va_arg(ap, unsigned int*));
            else if (obj.frm_specifier.type == INT_x ||
                     obj.frm_specifier.type == INT_X ||
                     obj.frm_specifier.type == INT_o)
               _read_hex (_in, src, (char**)&src, va_arg(ap, unsigned int*));
            else if (obj.frm_specifier.type == INT_c)
               _read_char (_in, src, (char**)&src, va_arg(ap, char*));
            else if (obj.frm_specifier.type == INT_s)
               _read_string (_in, src, (char**)&src, va_arg(ap, char*));
            else if (obj.frm_specifier.type == FL_f ||
                     obj.frm_specifier.type == FL_g ||
                     obj.frm_specifier.type == FL_G ||
                     obj.frm_specifier.type == FL_L)
               _read_ffloat (_in, src, (char**)&src, va_arg(ap, float*));
            else if (obj.frm_specifier.type == FL_e ||
                     obj.frm_specifier.type == FL_E)
               _read_ffloat (_in, src, (char**)&src, va_arg(ap, float*));
            else  // eat the wrong type to unsigned int
               _read_uint (_in, src, (char**)&src, va_arg(ap, unsigned int*));

            ++arg;
            break;

         case _IO_FRM_TERMINATOR:
            return arg;

         case _IO_FRM_CRAP:
            return arg;
      }
   }
   return (int)arg;
}
