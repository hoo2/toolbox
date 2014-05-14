/*!
 * \file _vsprintf.c
 * \brief
 *    A small footprint printf with floating point support
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
#include <std/_vsxprintf.h>

#define  _PRINTF_NUM_OF_TYPES    (16)
#define  _PRINTF_NUM_OF_FLAGS    (7)
/*!
 * The supported type conversion characters.
 * \note They MUST be in the same order as _printf_types_en \sa _printf_types_en
 */
static char pr_let[_PRINTF_NUM_OF_TYPES] = "cdeEfgGilLosuxX";

/*!
 * The supported(or not) flag characters.
 * \note They MUST be in the same order as _printf_flags_en \sa _printf_flags_en
 */
static char pr_flags[_PRINTF_NUM_OF_FLAGS] = "+- #'0";

static inline void _shift_right (unsigned int *i);
static inline void _shift_left (unsigned int *i);
static int _floorlog10 (double d);
static double _pw10(int e);
static _printf_types_en _istype (char c);
static _printf_flags_en _isflag (char c);
static int _inschar (_putc_out_t _out, char* dst, char c);
static int _insnchar (_putc_out_t _out, char *dst, char c, int n);
static int _insstring (_putc_out_t _out, char *dst, const char *psrc, int width);
static int _insuint(_putc_out_t _out, char *dst, char lead, int width, unsigned int value);
static int _insint (_putc_out_t _out, char *dst, char lead, int width, char sign, char min, int value);
static int _inshex (_putc_out_t _out, char *dst, char lead, int width, unsigned char maj, unsigned int value);
static int _inscoredouble (_putc_out_t _out, char *dst, char lead, int width, int frac, char sign, double value);
static int _insfdouble (_putc_out_t _out, char *dst, char lead, int width, int frac, char sign, double value);
static int _insedouble (_putc_out_t _out, char *dst, char lead, int width, int frac, char sign, double value);



/*!
 * Helper functions.
 */

// Shift 1 bit to the right
static inline void _shift_right (unsigned int *i) { *i >>= 1; }
// Shift 1 bit to the left
static inline void _shift_left (unsigned int *i) { *i <<= 1; }


/*!
 * \brief
 *    Calculates the integer floor (log10 (d)) of a double.
 *    For d>1 multiply a number and check it with a double
 *       the number is integer for small doubles  (<1e18)
 *       and double for big ones (>=1e18).
 *    For d<1 multiply the double and check it with an integer
 *
 *  \return  0 for wrong double (d<=0)
 *          -1 for huge values
 */
static int _floorlog10 (double d)
{
   #define _max_dec_digits    1e18
   int lg=0;
   register unsigned long long tenb=1;

   if (d<=0)      // No Log10 for these numbers, return 0
      return 0;
   else if (d<1) {
      while (1 > d) {
         d *= 10;
         --lg;
      }
      return lg+1;
      /*!
       * \note
       * Unfortunately here, we have to multiply the double.
       * A better solution to replace that is well appreciated.
       */
   } else {
      if (d < _max_dec_digits) {
         while (tenb < d)  { tenb *= 10; ++lg; }
         return lg-1;
      } else {
         return -1;
      }
      /*!
       *  \note
       *  We search for the right number by integer multiplications when possible.
       *  If the number is bigger than 1e18 (64 bit max decimal digits),
       *  we do the loop with floating point arithmetic. sorry :(
       */
   }
   #undef _max_dec_digits
}


/*!
 *  \brief Calculates the power 10^(int)e
 */
static double _pw10(int e)
{
   register unsigned long long p=1;
   int s=1;

   // Keep sign
   if (e<0) {
      s = -1;
      e *= s;
   }
   // Calculate the power
   while (e--)
      p *= 10;

   // return it as numerator or denominator.
   if (s == 1) return (double)(p);
   else        return (double)(1.0/p);
}


/*!
 * \brief
 *    Find if the current character is a type conversion
 *    character.
 *
 * \param   The character to check.
 * \return  The type in an enum form.
 */
static _printf_types_en _istype (char c)
{
   int i;
   for (i=0 ; i<_PRINTF_NUM_OF_TYPES ; ++i)
      if (pr_let[i] == c)
         return (_printf_types_en)(i);
   return NO_TYPE;
}

/*!
 * \brief
 *    Find if the current character is a flag character.
 *
 * \param   The character to check.
 * \return  The flag in an enum form.
 */
static _printf_flags_en _isflag (char c)
{
   int i;
   for (i=0 ; i<_PRINTF_NUM_OF_FLAGS ; ++i)
      if (pr_flags[i] == c)
         return (_printf_flags_en)(i);
   return NO_FLAG;
}


/*
 * Tailoring functions
 */

/*!
 * \brief
 *    inster a char to user defined output function
 */
int _putc_usr (char *dst, const char c)
{
   return __putchar (c);
}

/*!
 * \brief
 *    insert a char to destination string.
 */
int _putc_dst (char *dst, const char c)
{
   *dst = c;
   return 1;
}

/*!
 * \brief
 *    insert a char to file output function
 */
int _putc_fil (char *dst, const char c)
{
   return 1;
}



/*!
 * Conversion functions.
 */

/*!
 * \brief
 *    Writes a character inside the given string. Returns 1.
 *
 * \param  dst    destination string.
 * \param  c      character to write.
 */
static inline int _inschar(_putc_out_t _out, char *dst, char c) {
   return _out (dst, c);
}

/*!
 * \brief
 *    Writes \a n characters inside the given string and Returns the number
 *    of written characters.
 *
 * \param  dst  destination string.
 * \param  c    character to write.
 * \param  n    number of characters to write.
 */
static int _insnchar(_putc_out_t _out, char *dst, char c, int n)
{
   int nn;
   for (nn=n ; nn ; --nn)
      _out (dst++, c);
   return n;
}

/*!
 * \brief
 *    Writes a string inside the given string.
 *
 * \param  dst    destination string.
 * \param  src    source string.
 * \param  width  Minimum string width, or 0 for default.
 * \return The size of the written
 */
static int _insstring(_putc_out_t _out, char *dst, const char *src, int width)
{
   int n=0;

   // Send main string
   for (n=0 ; *src ; ++n)
      _out (dst++, *src++);
   // Send remaining - if any
   if (width && width>n)
      n += _insnchar (_out, dst, ' ', width-n);
   return n;
}


/*!
 * \brief
 *    Writes an unsigned int inside the given string, using the provided
 *    lead character & width parameters. To convert spreads an integer in
 *    to an array discarding one LBS at a time. After that takes the last
 *    digit of each and stream it to string.
 *
 *    For example the number 1234 will expand as:
 *      [0]: 1234
 *      [1]: 123
 *      [2]: 12
 *      [3]: 1
 *      [4]: 0
 *      After that it streams '1', '2', '3', and '4'
 *
 * \param  dst   destination string.
 * \param  lead  Leading character.
 * \param  width Minimum integer width.
 * \param  value Integer value.
 *
 * \return The number of written characters.
 */
static int _insuint(_putc_out_t _out, char *dst, char lead, int width, unsigned int value)
{
   int num = 0, i, j;
   unsigned int bf[PF_MAX_INT_DIGITS];

   // Spread value
   for (i=0, bf[0]=value ; i<PF_MAX_INT_DIGITS-1 ; )
   {
      bf[i+1] = bf[i]/10;
      if (!bf[++i])
         break;
   }

   // Write leading characters
   for (j=i ; j<width ; ++j)
      num += _inschar (_out, dst++, lead);

   // Write actual numbers
   for ( ; i ; --i)
      num += _inschar (_out, dst++, (bf[i-1] - bf[i]*10) + '0');

   return num;
}


/*!
 * \brief
 *    Writes a signed int inside the given string, using the provided
 *    lead characters & width parameters. To convert spreads an integer in
 *    to an array discarding one LBS at a time. After that takes the last
 *    digit of each and stream it to string.
 *
 *    For example the number 1234 will expand as:
 *      [0]: 1234
 *      [1]: 123
 *      [2]: 12
 *      [3]: 1
 *      [4]: 0
 *      After that it streams '1', '2', '3', and '4'
 *
 * \param dst    destination string.
 * \param lead   Leading character.
 * \param width  Minimum integer width.
 * \param sign   Always print sign flag.
 * \param min    Is a negative number, used if value is zero.
 * \param value  Signed integer value.
 *
 * \return The number of written characters.
 */
static int _insint (_putc_out_t _out, char *dst, char lead, int width, char sign, char min, int value)
{
   int num = 0, i, j;
   int bf[PF_MAX_INT_DIGITS];
   unsigned int absv, minus=min, scr;
   void (*pshift) (unsigned int*);

   // Compute absolute value
   if (value < 0)
   {
      minus=1;
      absv = -value;
   }
   else
      absv = value;

   // Spread absolute value
   for (i=0, bf[0]=absv ; i<PF_MAX_INT_DIGITS-1 ; )
   {
      bf[i+1] = bf[i]/10;
      if (!bf[++i])
         break;
   }
   j = i + (sign || minus);

   /*
    *  Decide to run lead characters code or sign code first.
    */
   if (lead == '0')  pshift = _shift_left;
   else              pshift = _shift_right;

   for (scr=0x04 ; scr ; )
   {
      pshift (&scr);
      switch (scr)
      {
         case 0x01:
         case 0x08:
            // Write sign
            if (minus)
               num += _inschar (_out, dst++, '-');
            else if (sign)
               num += _inschar (_out, dst++, '+');
            break;
         case 0x02:
         case 0x10:
            // Write lead characters
            for ( ; j<width ; ++j)
               num += _inschar (_out, dst++, lead);
            break;
         default:
            scr = 0;
            break;
      }
   }

   // Write actual numbers
   for ( ; i ; --i)
      num += _inschar (_out, dst++, (bf[i-1] - bf[i]*10) + '0');

   return num;
}

/*!
 * \brief
 *    Writes an hexadecimal value into a string, using the given lead
 *    character width & capital parameters. To convert spreads an integer in
 *    to an array discarding one LBS at a time (4bits). After that takes the last
 *    digit of each and stream it to string.
 *
 *    For example the number 1234 will expand as:
 *      [0]: 0x1234
 *      [1]: 0x123
 *      [2]: 0x12
 *      [3]: 0x1
 *      [4]: 0x0
 *      After that it streams '1', '2', '3', and '4'
 *
 * \param dst    destination string.
 * \param lead   Leading character.
 * \param width  Minimum integer width.
 * \param maj    Indicates if the letters must be printed in lower- or upper-case.
 * \param value  Hexadecimal value.
 *
 * \return  The number of char written
 */
static int _inshex (_putc_out_t _out, char *dst, char lead, int width, unsigned char maj, unsigned int value)
{
   int num = 0, i, j;
   unsigned int bf[PF_MAX_INT_DIGITS];

   // Spread value
   for (i=0, bf[0]=value ; i<PF_MAX_INT_DIGITS-1 ; )
   {
      bf[i+1] = bf[i]>>4;
      if (!bf[++i])
         break;
   }

   // Write lead characters
   for (j=i ; j<width ; ++j)
      num += _inschar (_out, dst++, lead);

   // Write actual numbers
   for ( ; i ; --i)
   {
      if ((bf[i-1] & 0xF) < 0xA)
         num += _inschar (_out, dst++, (bf[i-1] & 0xF) + '0');
      else if (maj)
         num += _inschar (_out, dst++, ((bf[i-1] & 0xF) - 0xA) + 'A');
      else
         num += _inschar (_out, dst++, ((bf[i-1] & 0xF) - 0xA) + 'a');
   }
   return num;
}


/*!
 * \brief
 *    Writes an floating point value into a string, using the given lead, width &
 *    sign parameters. The floating point is in decimal format.
 *
 * \param dst    destination string.
 * \param lead   Fill character.
 * \param width  Minimum integer width.
 * \param frac   Fractional width.
 * \param sign   Always print sign flag.
 * \param value  double value.
 *
 * \return  The number of char written
 */
static int _inscoredouble (_putc_out_t _out, char *dst, char lead, int width, int frac, char sign, double value)
{
   int num, fr_num, n_int, n_dec, minus=0;
   double absv, r_absv, scrl;

   if (value<0) {
      minus = 1;
      absv = -value;
   } else
      absv = value;

   // fix width
   if (!width)
      width = PF_WIDTH;
   // fix frac
   if (!frac)
      frac = PF_FRACTIONAL_WIDTH;

   // Calculate the parts
   n_int = (int)absv;
   scrl = _pw10 (frac);    // Calculate the scroll multiplier
   absv -= n_int;          // Cut the decimal part
   absv *= scrl;           // Scroll the fractional part frac positions to the left
   r_absv = round (absv);  // Round the scrolled frac part
   if (r_absv >= scrl)
   {  // The rounding result, give as an integer "reminder"
      ++n_int;
      r_absv = 0;
   }
   n_dec = (int)r_absv;

   /*
    * Write the number
    */
   num = _insint (_out, dst, lead, width-frac-1, sign, minus, n_int); // Insert the decimal part
   dst += num;
   num += _inschar (_out, dst++, '.');  // Insert point

   fr_num = _insint (_out, dst, '0', frac, 0, 0, n_dec);  // Insert fractional
   num += fr_num;          // Update counters
   dst += fr_num;

   // Write trailing zeros, if any
   if (fr_num < frac)
      num += _insnchar (_out, dst, '0', frac-fr_num);
   return num;
}

/*!
 * \brief
 *    Writes an floating point value into a string, using the given lead, width &
 *    sign parameters. The floating point is in decimal format.
 *    Supports also NaN and INF.
 *
 * \param dst    destination string.
 * \param lead   Fill character.
 * \param width  Minimum integer width.
 * \param frac   Fractional width.
 * \param sign   Always print sign flag.
 * \param value  double value.
 *
 * \return  The number of char written
 */
static int _insfdouble(_putc_out_t _out, char *dst, char lead, int width, int frac, char sign, double value)
{
   if ( isinf (value) )          // INF
      return _insstring(_out, dst, "INF", 0);
   else if ( isnan (value) )     // NAN
      return _insstring(_out, dst, "NaN", 0);
   else
      return _inscoredouble (_out, dst, lead, width, frac, sign, value);
   return 0;
}



/*!
 * \brief
 *    Writes an floating point value into a string, using the given lead, width &
 *    sign parameters. The floating point is in scientific (exp) format.
 *    Supports also NaN and INF.
 *
 * \param dst    destination string.
 * \param lead   Fill character.
 * \param width  Minimum integer width.
 * \param frac   Fractional width.
 * \param sign   Always print sign flag.
 * \param value  double value.
 *
 * \return  The number of char written
 */
static int _insedouble(_putc_out_t _out, char *dst, char lead, int width, int frac, char sign, double value)
{
   int exp=0, num=0;
   char exp_str[6];
   int sexp, minus=0;

   if ( isinf (value) )          // INF
      return _insstring(_out, dst, "INF", 0);
   else if ( isnan (value) )     // NAN
      return _insstring(_out, dst, "NaN", 0);
   else if (value == 0)          // there is no such thing as Log10(0)
      return _insstring(_out, dst, "0.0e0", 0);
   else
   {
      if (value <0)
      {
         minus = 1;
         exp = _floorlog10(-value);
         if (exp < 0)
            return _insstring(_out, dst, "-BIG", 0);
      }
      else {
         exp = _floorlog10(value);
         if (exp < 0)
            return _insstring(_out, dst, "+BIG", 0);
      }

      value = value / _pw10(exp);
      /*
       * We don't use pow() and floor(log10()) ;-)
       */

      // Prepare exponential and use _putc_dst() for that.
      sexp = _inschar (_putc_dst, exp_str, 'e');   // Insert e
      sexp += _insint (_putc_dst, &exp_str[1], ' ', 0, 1, 0, exp);
      exp_str[sexp]=0;


      if (!width)  width = PF_WIDTH;      // fix width
      if (!frac)   frac = PF_FRACTIONAL_WIDTH; // fix frac
      if (width < sexp+frac+2+(sign || minus))
         width = sexp+frac+2+(sign || minus);
      width -= sexp;       // significant's width
      /*
       * Calculate width from users request (full number width)
       *
       * full width = sign(1) + int + dot(1) + frac + sexp
       *                        |-------------------|
       *                          significant width
       *  int has minimum 1 size
       */

      // Insert results to string
      num = _inscoredouble (_out, dst, lead, width, frac, sign, value);
      dst += num;
      num += _insstring(_out, dst, exp_str, 0);
      return num;
   }
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
 * \param dst     Destination string (if any).
 * \param pfrm    Format string.
 * \param ap      Argument list.
 *
 * \return  The number of characters written.
 */
int vsxprintf(_putc_out_t _out, char *dst, const char *pfrm, va_list ap)
{
   char lead=' ', sign=0;           // Supported flags
   unsigned char width=0, frac=0;
   int num=0, size=0, arg=1;        // argument count starts with 1.
   _printf_types_en ct = NO_TYPE;   // Type holder
   _printf_flags_en cf = NO_FLAG;   // Flag holder
   _double_un_t du;                 // Bug workaround
   _parser_st_t  state;

   // Clear the string
   //_out (dst, 0);

   /*
    * Parse string
    * Note: Try to parse all the libc/newlib valid conversions and to
    *       ignore the not supported.
    */
   state = ST_STREAM;
   while (*pfrm != 0)
   {
      switch (state)
      {
         case ST_STREAM:
            if (IS_ALL_BUT_PC (*pfrm)) {
               _out (dst++, *pfrm);  ++size;
            }
            else
               state = ST_PC;
            break;
         case ST_PC:
            if ((ct = _istype (*pfrm)) != NO_TYPE)
               state = ST_TYPE;
            else if (IS_1TO9(*pfrm)) {
               state = ST_WIDTH;
               width = (width*10) + *pfrm-'0';
            }
            else if ((cf = _isflag (*pfrm)) != NO_FLAG)
               state = ST_FLAG;
            else if (IS_DOT (*pfrm))
               state = ST_FRAC;
            else if (IS_PC (*pfrm)) {
               state = ST_STREAM;
               _out (dst++, *pfrm);  ++size;
            }
            break;
         case ST_FLAG:
            if (cf == FLAG_PLUS)       sign = 1;
            else if (cf == FLAG_SPACE) lead = ' ';
            else if (cf == FLAG_ZERO)  lead = '0';

            if (IS_1TO9(*pfrm)) {
               state = ST_WIDTH;
               width = (width*10) + *pfrm-'0';
            }
            else if ((ct = _istype (*pfrm)) != NO_TYPE)
               state = ST_TYPE;
            else if ((cf = _isflag (*pfrm)) != NO_FLAG)
               ;
            break;
         case ST_WIDTH:
            if (IS_0TO9(*pfrm))
               width = (width*10) + *pfrm-'0';
            else if ((ct = _istype (*pfrm)) != NO_TYPE)
               state = ST_TYPE;
            else if (IS_DOT (*pfrm))
               state = ST_FRAC;
            break;
         case ST_FRAC:
            if (IS_0TO9(*pfrm))
               frac = (frac*10) + *pfrm-'0';
            else if ((ct = _istype (*pfrm)) != NO_TYPE)
               state = ST_TYPE;
            break;
         case ST_TYPE:
         #ifndef PRINTF_TINY
            ++arg;   //We have next argument
            if (ct == INT_d || ct == INT_i || ct == INT_l)
               num = _insint(_out, dst, lead, width, sign, 0, va_arg(ap, signed int));
            else if (ct == INT_u)
               num = _insuint(_out, dst, lead, width, va_arg(ap, unsigned int));
            else if (ct == INT_x || ct == INT_o)
               num = _inshex(_out, dst, lead, width, 0, va_arg(ap, unsigned int));
            else if (ct == INT_X)
               num = _inshex(_out, dst, lead, width, 1, va_arg(ap, unsigned int));
            else if (ct == INT_c)
               num = _inschar(_out, dst, va_arg(ap, unsigned int));
            else if (ct == INT_s)
               num = _insstring(_out, dst, va_arg(ap, char *), width);
            else if (ct == FL_f || ct == FL_g || ct == FL_G || ct == FL_L)
            {
               /*
                * if (!(arg%2))
                *    va_arg(ap, unsigned int);
                * This works only if the call with double has more than one argument :(
                */
               du.i[0] = va_arg(ap, unsigned int);
               du.i[1] = va_arg(ap, unsigned int);
               /*
                * XXX: BUG(s) workaround
                * All printf with double calls MUST HAVE ONLY ONE ARGUMENT, THE double.
                * 1) When double argument comes in even number it has 4bytes crap in front of it.
                * 2) va_arg(ap, double) comes in odd argument number fails cause it skips 4bytes before reading.
                * Someone have miss-correct a bug i think.
                */
               num = _insfdouble (_out, dst, lead, width, frac, sign, du.d);
            }
            else if (ct == FL_e || ct == FL_E)
            {
               /*
                * if (!(arg%2))
                *    va_arg(ap, unsigned int);
                * This works only if the call with double has more than one argument :(
                */
                 du.i[0] = va_arg(ap, unsigned int);
                 du.i[1] = va_arg(ap, unsigned int);
                 /*
                  * XXX: BUG(s) workaround
                  * All printf with double calls MUST HAVE ONLY ONE ARGUMENT, THE double.
                  * 1) When double argument comes in even number it has 4bytes crap in front of it.
                  * 2) va_arg(ap, double) comes in odd argument number fails cause it skips 4bytes before reading.
                  * Someone have miss-correct a bug i think.
                  */
                 num = _insedouble (_out, dst, lead, width, frac, sign, du.d);
            }
            else  // eat the wrong type to unsigned int
               num = _insuint(_out, dst, lead, width, va_arg(ap, unsigned int));
         #else
            ++arg;   //We have next argument
            if (ct == INT_d || ct == INT_i || ct == INT_l)
               num = _insint(_out, dst, lead, width, sign, 0, va_arg(ap, signed int));
            else if (ct == INT_u || ct == INT_x || ct == INT_o || ct == INT_X)
               num = _insuint(_out, dst, lead, width, va_arg(ap, unsigned int));
            else if (ct == INT_c)
               num = _inschar(_out, dst, va_arg(ap, unsigned int));
            else if (ct == INT_s)
               num = _insstring(_out, dst, va_arg(ap, char *), width);
            else if (ct == FL_f || ct == FL_g || ct == FL_G || ct == FL_L || ct == FL_e || ct == FL_E)
            {
               if (!(arg%2))
                  va_arg(ap, unsigned int);
               /*
                * This works only if the call with double has more than one argument :(
                */
               du.i[0] = va_arg(ap, unsigned int);
               du.i[1] = va_arg(ap, unsigned int);
               /*
                * XXX: BUG(s) workaround
                * All printf with double calls MUST HAVE ONLY ONE ARGUMENT, THE double.
                * 1) When double argument comes in even number it has 4bytes crap in front of it.
                * 2) va_arg(ap, double) comes in odd argument number fails cause it skips 4bytes before reading.
                * Someone have miss-correct a bug i think.
                */
               num = _inscoredouble (_out, dst, lead, width, frac, sign, du.d);
            }
            else  // eat the wrong type to unsigned int
               num = _insuint(_out, dst, lead, width, va_arg(ap, unsigned int));
         #endif   //#ifndef PRINTF_TINY
            // Clear supported flags
            width = sign = frac = 0;
            lead = ' ';
            state = ST_STREAM;

            // update pointers
            dst += num;
            size += num;
            break;
      }
      if (state != ST_TYPE)
         ++pfrm;
   }

   // NULL-terminated (final \0 is not counted)
   _out (dst, 0);
   return size;
}
