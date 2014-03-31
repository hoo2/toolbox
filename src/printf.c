/*!
 * \file printf.c
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
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       06/2013
 * Version:
 *
 */

#include <printf.h>

// #define PRINTF_TINY
// #define PRINTF_FILES

/*
 * libc compatibility
 */
#ifdef PRINTF_FILES
struct _reent r = {0, (FILE *) 0, (FILE *) 1, (FILE *) 0};
struct _reent *_impure_ptr = &r;
#endif   //#ifndef PRINTF_TINY


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
#ifndef PRINTF_TINY
static int _floorlog10 (double d);
#endif
static double _pw10(int e);
static _printf_types_en _istype (char c);
static _printf_flags_en _isflag (char c);
static int _inschar (char* pdst, char c);
static int _insnchar(char *pdst, char c, int n);
static int _insstring(char *pdst, const char *psrc, int width);
static int _insuint(char *pdst, char lead, int width, unsigned int value);
static int _insint (char *pdst, char lead, int width, char sign, char min, int value);
#ifndef PRINTF_TINY
static int _inshex (char *pdst, char lead, int width, unsigned char maj, unsigned int value);
#endif
static int _inscoredouble (char *pdst, char lead, int width, int frac, char sign, double value);
#ifndef PRINTF_TINY
static int _insfdouble(char *pdst, char lead, int width, int frac, char sign, double value);
static int _insedouble(char *pdst, char lead, int width, int frac, char sign, double value);
#endif


/*!
 * Helper functions.
 */

// Shift 1 bit to the right
static inline void _shift_right (unsigned int *i) { *i >>= 1; }
// Shift 1 bit to the left
static inline void _shift_left (unsigned int *i) { *i <<= 1; }


#ifndef PRINTF_TINY
/*!
 * \brief
 *    Calculates the integer floor (log10 (d)) of a double.
 *    For d>1 multiply a integer and check it with a double
 *    For d<1 multiply the double and check it with an integer :(
 *
 *  \return 0 for wrong double (d<=0)
 */
static int _floorlog10 (double d)
{
   int lg=0;
   int tenb=1;

   if (d<=0)      // No Log10 for these numbers, return 0
      return 0;
   else if (d<1)
   {
      while (1 > d)
      {
         d *= 10;
         --lg;
      }
      return lg+1;
      /*!
       * \note Unfortunately here, we have to multiply the double
       * A better solution to replace that is well appreciated.
       */
   }
   else  // Search for the right number by integer multiplications.
   {
      while (tenb < d)
      {
         tenb *= 10;
         ++lg;
      }
      return lg-1;
   }
}
#endif   //#ifndef PRINTF_TINY


/*!
 *  \brief Calculates the power 10^(int)e
 */
static double _pw10(int e)
{
   int p=1, s=1;

   // Keep sign
   if (e<0)
   {
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








/*!
 * Conversion functions.
 */

/*!
 * \brief
 *    Writes a character inside the given string. Returns 1.
 *
 * \param  pdst   Storage string.
 * \param  c      Character to write.
 */
static int _inschar(char *pdst, char c)
{
   *pdst = c;
   return 1;
}

/*!
 * \brief
 *    Writes \a n characters inside the given string and Returns the number
 *    of written characters.
 *
 * \param  pdst Storage string.
 * \param  c    Character to write.
 * \param  n    Number of characters to write.
 */
static int _insnchar(char *pdst, char c, int n)
{
   int nn;
   for (nn=n ; nn ; --nn)
      *pdst++ = c;
   return n;
}

/*!
 * \brief
 *    Writes a string inside the given string.
 *
 * \param  pdst      Storage string.
 * \param  psrc      Source string.
 * \param  width     Minimum string width, or 0 for default.
 * \return The size of the written
 */
static int _insstring(char *pdst, const char *psrc, int width)
{
   int n=0;

   // Send main string
   for (n=0 ; *psrc ; ++n)
      *pdst++ = *psrc++;
   // Send remaining - if any
   if (width && width>n)
      n += _insnchar(pdst, ' ', width-n);
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
 * \param  pdst  Storage string.
 * \param  lead  Leading character.
 * \param  width Minimum integer width.
 * \param  value Integer value.
 *
 * \return The number of written characters.
 */
static int _insuint(char *pdst, char lead, int width, unsigned int value)
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
      num += _inschar(pdst++, lead);

   // Write actual numbers
   for ( ; i ; --i)
      num += _inschar(pdst++, (bf[i-1] - bf[i]*10) + '0');

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
 * \param pdst   Storage string.
 * \param lead   Leading character.
 * \param width  Minimum integer width.
 * \param sign   Always print sign flag.
 * \param min    Is a negative number, used if value is zero.
 * \param value  Signed integer value.
 *
 * \return The number of written characters.
 */
static int _insint(char *pdst, char lead, int width, char sign, char min, int value)
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
               num += _inschar(pdst++, '-');
            else if (sign)
               num += _inschar(pdst++, '+');
            break;
         case 0x02:
         case 0x10:
            // Write lead characters
            for ( ; j<width ; ++j)
               num += _inschar(pdst++, lead);
            break;
         default:
            scr = 0;
            break;
      }
   }

   // Write actual numbers
   for ( ; i ; --i)
      num += _inschar(pdst++, (bf[i-1] - bf[i]*10) + '0');

   return num;
}

#ifndef PRINTF_TINY
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
 * \param pdst   Storage string.
 * \param lead   Leading character.
 * \param width  Minimum integer width.
 * \param maj    Indicates if the letters must be printed in lower- or upper-case.
 * \param value  Hexadecimal value.
 *
 * \return  The number of char written
 */
static int _inshex(char *pdst, char lead, int width, unsigned char maj, unsigned int value)
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
      num += _inschar(pdst++, lead);

   // Write actual numbers
   for ( ; i ; --i)
   {
      if ((bf[i-1] & 0xF) < 0xA)
         num += _inschar(pdst++, (bf[i-1] & 0xF) + '0');
      else if (maj)
         num += _inschar(pdst++, ((bf[i-1] & 0xF) - 0xA) + 'A');
      else
         num += _inschar(pdst++, ((bf[i-1] & 0xF) - 0xA) + 'a');
   }
   return num;
}
#endif   //#ifndef PRINTF_TINY


/*!
 * \brief
 *    Writes an floating point value into a string, using the given lead, width &
 *    sign parameters. The floating point is in decimal format.
 *
 * \param pdst   Storage string.
 * \param lead   Fill character.
 * \param width  Minimum integer width.
 * \param frac   Fractional width.
 * \param sign   Always print sign flag.
 * \param value  double value.
 *
 * \return  The number of char written
 */
static int _inscoredouble (char *pdst, char lead, int width, int frac, char sign, double value)
{
   int num, fr_num, n_int, n_dec, minus=0;
   char *str = pdst;
   double absv, r_absv, scrl;

   if (value<0)
   {
      minus = 1;
      absv = -value;
   }
   else
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
   num = _insint(str, lead, width-frac-1, sign, minus, n_int); // Insert the decimal part
   str += num;

   num += _inschar(str++, '.');  // Insert point

   fr_num = _insint(str, '0', frac, 0, 0, n_dec);  // Insert fractional
   num += fr_num;          // Update counters
   str += fr_num;

   // Write trailing zeros, if any
   if (fr_num < frac)
      num += _insnchar (str, '0', frac-fr_num);
   return num;
}

#ifndef PRINTF_TINY
/*!
 * \brief
 *    Writes an floating point value into a string, using the given lead, width &
 *    sign parameters. The floating point is in decimal format.
 *    Supports also NaN and INF.
 *
 * \param pdst   Storage string.
 * \param lead   Fill character.
 * \param width  Minimum integer width.
 * \param frac   Fractional width.
 * \param sign   Always print sign flag.
 * \param value  double value.
 *
 * \return  The number of char written
 */
static int _insfdouble(char *pdst, char lead, int width, int frac, char sign, double value)
{
   if ( isinf (value) )          // INF
      return _insstring(pdst, "INF", 0);
   else if ( isnan (value) )     // NAN
      return _insstring(pdst, "NaN", 0);
   else
      return _inscoredouble (pdst, lead, width, frac, sign, value);
   return 0;
}



/*!
 * \brief
 *    Writes an floating point value into a string, using the given lead, width &
 *    sign parameters. The floating point is in scientific (exp) format.
 *    Supports also NaN and INF.
 *
 * \param pdst   Storage string.
 * \param lead   Fill character.
 * \param width  Minimum integer width.
 * \param frac   Fractional width.
 * \param sign   Always print sign flag.
 * \param value  double value.
 *
 * \return  The number of char written
 */
static int _insedouble(char *pdst, char lead, int width, int frac, char sign, double value)
{
   int exp=0, num=0;
   char *str = pdst, exp_str[6];
   int sexp, minus=0;

   if ( isinf (value) )          // INF
      return _insstring(pdst, "INF", 0);
   else if ( isnan (value) )     // NAN
      return _insstring(pdst, "NaN", 0);
   else if (value == 0)          // We have no Log10(0)
      return _insstring(pdst, "0.0e0", 0);
   else
   {
      if (value <0)
      {
         minus = 1;
         exp = _floorlog10(-value);
      }
      else
         exp = _floorlog10(value);

      // fix width
      if (!width)
         width = PF_WIDTH;

      value = value / _pw10(exp);
      /*
       * We don't use pow() and floor(log10()) ;-)
       * XXX: "value" may requires rounding.
       */

      // Prepare exponential.
      sexp = _inschar(exp_str, 'e');   // Insert e
      sexp += _insint(&exp_str[1], ' ', 0, 1, 0, exp);
      exp_str[sexp]=0;

      // fix width, calculate minimum width.
      if (width < sexp+3+(sign || minus))
         width = sexp+3+(sign || minus);
      width -= sexp;       // keep only the significant's width
      // frac fix
      if (!frac)
         frac = PF_FRACTIONAL_WIDTH;
      if ((frac + (sign || minus) + 2) > width)
         frac = width - 2 - (sign || minus);
      if (frac < 1)
         frac = 1;
      // Insert results to string
      num = _inscoredouble (str, lead, width, frac, sign, value);
      str += num;
      num += _insstring(str, exp_str, 0);
      return num;
   }
   return 0;
}
#endif   //#ifndef PRINTF_TINY

/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Stores the result of a formatted string into another string. Format
 *    arguments are given in a va_list instance.
 *    First lexicon analisis is made to the pfrm
 *    Second the proper conversion function is called
 *    The procedure is continued until we reach \see PF_MAX_STRING_SIZE
 *    or NULL character..
 *
 * \param pdst    Destination string.
 * \param length  Length of Destination string.
 * \param pfrm    Format string.
 * \param ap      Argument list.
 *
 * \return  The number of characters written.
 */
int vsnprintf(char *pdst, size_t length, const char *pfrm, va_list ap)
{
   char lead=' ', sign=0;           // Supported flags
   unsigned char width=0, frac=0;
   int num=0, size=0, arg=1;        // argument count starts with 1.
   _printf_types_en ct = NO_TYPE;   // Type holder
   _printf_flags_en cf = NO_FLAG;   // Flag holder
   _double_un_t du;                 // Bug workaround
   _parser_st_t  state;

   // Clear the string
   if (pdst) *pdst = 0;

   /*
    * Parse string
    * Note: Try to parse all the libc/newlib valid conversions and to
    *       ignore the not supported.
    */
   state = ST_STREAM;
   while (*pfrm != 0 && size < length)
   {
      switch (state)
      {
         case ST_STREAM:
            if (IS_ALL_BUT_PC (*pfrm)) {
               *pdst++ = *pfrm;  ++size;
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
               *pdst++ = *pfrm;  ++size;
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
               num = _insint(pdst, lead, width, sign, 0, va_arg(ap, signed int));
            else if (ct == INT_u)
               num = _insuint(pdst, lead, width, va_arg(ap, unsigned int));
            else if (ct == INT_x || ct == INT_o)
               num = _inshex(pdst, lead, width, 0, va_arg(ap, unsigned int));
            else if (ct == INT_X)
               num = _inshex(pdst, lead, width, 1, va_arg(ap, unsigned int));
            else if (ct == INT_c)
               num = _inschar(pdst, va_arg(ap, unsigned int));
            else if (ct == INT_s)
               num = _insstring(pdst, va_arg(ap, char *), width);
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
               num = _insfdouble (pdst, lead, width, frac, sign, du.d);
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
                 num = _insedouble (pdst, lead, width, frac, sign, du.d);
              }
            else  // eat the wrong type to unsigned int
               num = _insuint(pdst, lead, width, va_arg(ap, unsigned int));
         #else
            ++arg;   //We have next argument
            if (ct == INT_d || ct == INT_i || ct == INT_l)
               num = _insint(pdst, lead, width, sign, 0, va_arg(ap, signed int));
            else if (ct == INT_u || ct == INT_x || ct == INT_o || ct == INT_X)
               num = _insuint(pdst, lead, width, va_arg(ap, unsigned int));
            else if (ct == INT_c)
               num = _inschar(pdst, va_arg(ap, unsigned int));
            else if (ct == INT_s)
               num = _insstring(pdst, va_arg(ap, char *), width);
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
               num = _inscoredouble (pdst, lead, width, frac, sign, du.d);
            }
            else  // eat the wrong type to unsigned int
               num = _insuint(pdst, lead, width, va_arg(ap, unsigned int));
         #endif   //#ifndef PRINTF_TINY
            // Clear supported flags
            width = sign = 0;
            lead = ' ';
            state = ST_STREAM;

            // update pointers
            pdst += num;
            size += num;
            break;
      }
      if (state != ST_TYPE)
         ++pfrm;
   }

   // NULL-terminated (final \0 is not counted)
   if (size < length)
      *pdst = 0;
   else
   {
      *(--pdst) = 0;
      size--;
   }
   return size;
}


/*!
 * \brief
 *    Stores the result of a formatted string into another string. Format
 *    arguments are given in a va_list instance.
 *
 * \param pdst    Destination string.
 * \param length  Length of Destination string.
 * \param pfrmt   Format string.
 * \param ...     Other arguments
 *
 * \return  The number of characters written.
 */
int snprintf(char *pString, size_t length, const char *pfrmt, ...)
{
   va_list ap;
   int     rc;

   va_start(ap, (char *)pfrmt);
   rc = vsnprintf(pString, length, pfrmt, ap);
   va_end(ap);

   return rc;
}


/*!
 * \brief
 *    Stores the result of a formatted string into another string. Format
 *    arguments are given in a va_list instance.
 *
 * \param pString  Destination string.
 * \param length   Length of Destination string.
 * \param pfrmt    Format string.
 * \param ap       Argument list.
 *
 * \return  The number of characters written.
 */
int vsprintf(char *pString, const char *pfrmt, va_list ap)
{
   return vsnprintf(pString, PF_MAX_STRING_SIZE, pfrmt, ap);
}

#ifdef PRINTF_FILES
/*!
 * \brief
 *    Outputs a formatted string on the given stream. Format arguments are given
 *    in a va_list instance.
 *
 * \param fp      Output stream.
 * \param pfrmt   Format string
 * \param ap      Argument list.
 */
int vfprintf(FILE *fp, const char *pfrmt, va_list ap)
{
   char pdst[PF_MAX_STRING_SIZE];

   // Write formatted string in buffer
   if (vsprintf(pdst, pfrmt, ap) >= PF_MAX_STRING_SIZE)
      while (1); // Trap

   return fputs(pdst, fp);  // Display string
}
#endif   //#ifdef PRINTF_FILES

/*!
 * \brief
 *    Outputs a formatted string on the DBGU stream. Format arguments are given
 *    in a va_list instance.
 *
 * \param pfrmt  Format string.
 * \param ap  Argument list.
 */
int vprintf(const char *pfrmt, va_list ap)
{
#ifdef PRINTF_FILES
   return vfprintf(stdout, pfrmt, ap);
#else
   char pdst[PF_MAX_STRING_SIZE];

   // Write formatted string in buffer
   if (vsprintf(pdst, pfrmt, ap) >= PF_MAX_STRING_SIZE)
      while (1); // Trap

   return puts(pdst);  // Display string
#endif   //#ifdef PRINTF_FILES
}

#ifdef PRINTF_FILES
/*!
 * \brief
 *    Outputs a formatted string on the given stream, using a variable
 *    number of arguments.
 *
 * \param fp      Output stream.
 * \param pfrmt   Format string.
 */
int fprintf(FILE *fp, const char *pfrmt, ...)
{
   va_list ap;
   int result;

   /* Forward call to vfprintf */
   va_start(ap, (char *)pfrmt);
   result = vfprintf(fp, pfrmt, ap);
   va_end(ap);

   return result;
}
#endif   //#ifdef PRINTF_FILES

/*!
 * \brief
 *    Outputs a formatted string on the DBGU stream, using a variable number of
 *    arguments.
 *
 * \param  pfrmt  Format string.
 */
int printf(const char *pfrmt, ...)
{
   va_list ap;
   int result;

   /* Forward call to vprintf */
   va_start(ap, (char *)pfrmt);
   result = vprintf(pfrmt, ap);
   va_end(ap);

   return result;
}


/*!
 * \brief
 *    Writes a formatted string inside another string.
 *
 * \param pdst     storage string.
 * \param pfrmt  Format string.
 */
int sprintf(char *pdst, const char *pfrmt, ...)
{
   va_list ap;
   int result;

   // Forward call to vsprintf
   va_start(ap, (char *)pfrmt);
   result = vsprintf(pdst, pfrmt, ap);
   va_end(ap);

   return result;
}


/*!
 * \brief
 *    Outputs a string on stdout.
 *
 * \param pdst  String to output.
 */
int puts(const char *pdst)
{
#ifdef PRINTF_FILES
   return fputs(pdst, stdout);
#else
   int num = 0;

   while (*pdst != 0)
   {
      if (__putchar(*pdst) == -1)
         return -1;
      num++;
      pdst++;
   }

   return num;
#endif   //#ifndef PRINTF_TINY
}


#ifdef PRINTF_FILES
/*!
 * \brief
 *    Implementation of fputc using the DBGU as the standard output. Required
 *    for printf().
 *
 * \param c    Character to write.
 * \param fp   Output stream.
 * \param
 *    The character written if successful, or -1 if the output stream is
 *    not stdout or stderr.
 */
int fputc(int c, FILE *fp)
{
   if ((fp == stdout) || (fp == stderr))
      return __putchar(c);
   else
      return EOF;
}


/*!
 * \brief
 *    Implementation of fputs using the DBGU as the standard output. Required
 *    for printf().
 *
 * \param pdst String to write.
 * \param fp   Output stream.
 *
 * \return
 *    Number of characters written if successful, or -1 if the output
 *    stream is not stdout or stderr.
 */
int fputs(const char *pdst, FILE *fp)
{
   int num = 0;

   while (*pdst != 0)
   {
      if (fputc(*pdst, fp) == -1)
         return -1;
      num++;
      pdst++;
   }

   return num;
}
#endif   //#ifdef PRINTF_FILES

