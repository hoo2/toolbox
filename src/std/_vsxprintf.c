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

static inline void _shift_right (unsigned int *i);
static inline void _shift_left (unsigned int *i);
static int _floorlog10 (double d);
static double _pw10(int e);

static int _inschar (_putc_out_t _out, char* dst, char c);
static int _insnchar (_putc_out_t _out, char *dst, char c, int n);
static int _insstring (_putc_out_t _out, char *dst, const char *src, int length);
static int _insuint(_putc_out_t _out, char *dst, _io_frm_spec_t *fs, unsigned int value);
static int _insint (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, char min, int value);
static int _inshex (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, unsigned int value);
static int _inscoredouble (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, double value);
static int _insfdouble (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, double value);
static int _insedouble (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, double value);



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
   int lg=0;
   register unsigned long long tenb=1;

   if (d<=0)      // No Log10 for these numbers, return 0
      return 0;
   else if (d<1) {
      do {
         d *= 10;
         --lg;
      } while (d < 1);
      return lg+1;
      /*!
       * \note
       * Unfortunately here, we have to multiply the double.
       * A better solution to replace that is well appreciated.
       */
   }
   else {
      if (d < _IO_MAX_FLOAT) {
         while (tenb < d) {
            tenb *= 10;
            ++lg;
         }
         return lg-1;
      }
      else {
         return -1;
      }
      /*!
       *  \note
       *  We search for the right number by integer multiplications when possible.
       *  If the number is bigger than 1e18 (64 bit max decimal digits),
       *  we do the loop with floating point arithmetic. sorry :(
       */
   }
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


/*
 * Tailoring functions
 */

/*!
 * \brief
 *    inster a char to user defined output function
 */
inline int _putc_usr (char *dst, const char c)
{
   return __putchar (c);
}

/*!
 * \brief
 *    insert a char to destination string.
 */
inline int _putc_dst (char *dst, const char c)
{
   *dst = c;
   return 1;
}

/*!
 * \brief
 *    insert a char to file output function
 */
inline int _putc_fil (char *dst, const char c)
{
   return 0;
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
static int _insstring(_putc_out_t _out, char *dst, const char *src, int length)
{
   int n=0;

   // Send main string
   for (n=0 ; *src ; ++n)
      _out (dst++, *src++);
   // Send remaining - if any
   if (length && length>n)
      n += _insnchar (_out, dst, ' ', length-n);
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
static int _insuint(_putc_out_t _out, char *dst, _io_frm_spec_t *fs, unsigned int value)
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
   for (j=i ; j<fs->width ; ++j)
      num += _inschar (_out, dst++, fs->flags.lead);

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
static int _insint (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, char min, int value)
{
   int num = 0, i, j;
   int bf[PF_MAX_INT_DIGITS];
   unsigned int absv, negative=min, scr;
   void (*pshift) (unsigned int*);

   // Compute absolute value
   if (value < 0)
   {
      negative=1;
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
   j = i + (fs->flags.plus || negative);

   /*
    *  Decide to run lead characters code or sign code first.
    */
   if (fs->flags.lead == '0')
      pshift = _shift_left;
   else
      pshift = _shift_right;

   for (scr=0x04 ; scr ; )
   {
      pshift (&scr);
      switch (scr)
      {
         case 0x01:
         case 0x08:
            // Write sign
            if (negative)
               num += _inschar (_out, dst++, '-');
            else if (fs->flags.plus)
               num += _inschar (_out, dst++, '+');
            break;
         case 0x02:
         case 0x10:
            // Write lead characters
            for ( ; j<fs->width ; ++j)
               num += _inschar (_out, dst++, fs->flags.lead);
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
static int _inshex (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, unsigned int value)
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
   for (j=i ; j<fs->width ; ++j)
      num += _inschar (_out, dst++, fs->flags.lead);

   // Write actual numbers
   for ( ; i ; --i)
   {
      if ((bf[i-1] & 0xF) < 0xA)
         num += _inschar (_out, dst++, (bf[i-1] & 0xF) + '0');
      else if (fs->type == INT_X)
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
static int _inscoredouble (_putc_out_t _out, char *dst, _io_frm_spec_t *fs, double value)
{
   int num, fr_num, n_int, n_dec, negative=0;
   double absv, r_absv, scrl;
   _io_frm_spec_t n_int_fs, n_dec_fs;

   if (value<0) {
      negative = 1;
      absv = -value;
   } else
      absv = value;

   // fix width
   if (!fs->width)
      fs->width = PF_WIDTH;
   // fix frac
   if (!fs->frac)
      fs->frac = PF_FRACTIONAL_WIDTH;

   // Calculate the n_int and n_dec parts
   n_int = (int)absv;
   scrl = _pw10 (fs->frac);   // Calculate the scroll multiplier
   absv -= n_int;             // Cut the decimal part
   absv *= scrl;              // Scroll the fractional part frac positions to the left
   r_absv = round (absv);     // Round the scrolled frac part
   if (r_absv >= scrl)
   {  // The rounding result, give as an integer "reminder"
      ++n_int;
      r_absv = 0;
   }
   n_dec = (int)r_absv;

   /*
    * Write the number
    */
   // Prepare n_int
   n_int_fs.width = fs->width-fs->frac-1;
   n_int_fs.frac = 0;
   n_int_fs.flags.lead = fs->flags.lead;
   n_int_fs.flags.plus = fs->flags.plus;
   n_int_fs.flags.minus = n_int_fs.flags.sharp = 0;
   // Prepare n_dec
   n_dec_fs.width = fs->frac;
   n_dec_fs.frac = 0;
   n_dec_fs.flags.lead ='0';
   n_dec_fs.flags.plus = n_int_fs.flags.minus = n_int_fs.flags.sharp = 0;

   num = _insint (_out, dst, &n_int_fs, negative, n_int); // Insert the decimal part
   dst += num;

   num += _inschar (_out, dst++, '.');  // Insert point

   fr_num = _insint (_out, dst, &n_dec_fs, 0, n_dec);  // Insert fractional
   num += fr_num;          // Update counters
   dst += fr_num;

   // Write trailing zeros, if any
   if (fr_num < fs->frac)
      num += _insnchar (_out, dst, '0', fs->frac-fr_num);
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
static int _insfdouble(_putc_out_t _out, char *dst, _io_frm_spec_t *fs, double value)
{
   if ( isinf (value) )          // INF
      return _insstring(_out, dst, "INF", 0);
   else if ( isnan (value) )     // NAN
      return _insstring(_out, dst, "NaN", 0);
   else
      return _inscoredouble (_out, dst, fs, value);
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
static int _insedouble(_putc_out_t _out, char *dst, _io_frm_spec_t *fs, double value)
{
   int exp=0, num=0;
   char exp_str[6];
   int sexp, negative=0;
   _io_frm_spec_t exp_fs;

   if ( isinf (value) )          // INF
      return _insstring(_out, dst, "INF", 0);
   else if ( isnan (value) )     // NAN
      return _insstring(_out, dst, "NaN", 0);
   else if (value == 0)          // there is no such thing as Log10(0)
      return _insstring(_out, dst, "0.0e0", 0);
   else
   {
      if (value < 0) {
         negative = 1;
         exp = _floorlog10(-value);
         if (value <-1 && (exp == -1 || exp > _IO_MAX_FLOAT_EXP))
            return _insstring(_out, dst, "-BIG", 0);
         if (value >-1 && exp < _IO_MIN_FLOAT_EXP)
            return _insstring(_out, dst, "LIM-0", 0);
      }
      else {
         exp = _floorlog10(value);
         if (value >1 && (exp == -1 || exp > _IO_MAX_FLOAT_EXP))
            return _insstring(_out, dst, "+BIG", 0);
         if (value <1 && exp < _IO_MIN_FLOAT_EXP)
            return _insstring(_out, dst, "LIM+0", 0);
      }

      value = value / _pw10(exp);
      /*
       * We don't use pow() and floor(log10()) ;-)
       */

      // Prepare exponential and use _putc_dst() for that.
      exp_fs.width = exp_fs.frac = 0;
      exp_fs.flags.lead = ' ';
      exp_fs.flags.plus = 1;
      exp_fs.flags.minus = exp_fs.flags.sharp = 0;
      sexp = _inschar (_putc_dst, exp_str, 'e');   // Insert e
      sexp += _insint (_putc_dst, &exp_str[1], &exp_fs, 0, exp);
      exp_str[sexp]=0;


      if (!fs->width)  fs->width = PF_WIDTH;      // fix width
      if (!fs->frac)   fs->frac = PF_FRACTIONAL_WIDTH; // fix frac
      if (fs->width < sexp+fs->frac+2+(fs->flags.plus || negative))
         fs->width = sexp+fs->frac+2+(fs->flags.plus || negative);
      fs->width -= sexp;       // significant's width
      /*
       * Calculate width from users request (full number width)
       *
       * full width = sign(1) + int + dot(1) + frac + sexp
       *                        |-------------------|
       *                          significant width
       *  int has minimum 1 size
       */

      // Insert results to string
      num = _inscoredouble (_out, dst, fs, value);
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
 *    First lexicon analysis is made to the frm
 *    Second the proper conversion function is called
 *    The procedure is continued until we reach \see PF_MAX_STRING_SIZE
 *    or NULL character..
 *
 * \param _out    callback function to use for output streaming
 * \param dst     Destination string (if any).
 * \param frm     Format string.
 * \param ap      Argument list.
 *
 * \return  The number of characters written.
 */
int vsxprintf(_putc_out_t _out, char *dst, char *frm, __VALIST ap)
{
   _io_frm_obj_t  obj;              // object place holder
   _io_frm_obj_type_en  obj_type;   // object type place holder
   //_double_un_t du;                 // Bug workaround
   char* dst_start = dst;           // Destination starting address

   while (*frm != 0) {
      frm += _io_read (frm, &obj, &obj_type);
      switch (obj_type) {
         case _IO_FRM_STREAM:
            _out (dst++, obj.character);
            break;
         case _IO_FRM_SPECIFIER:
            if (obj.frm_specifier.type == INT_d ||
                obj.frm_specifier.type == INT_i ||
                obj.frm_specifier.type == INT_l)
               dst += _insint(_out, dst, &obj.frm_specifier, 0, va_arg(ap, signed int));
            else if (obj.frm_specifier.type == INT_u)
               dst += _insuint(_out, dst, &obj.frm_specifier, va_arg(ap, unsigned int));
            else if (obj.frm_specifier.type == INT_x ||
                     obj.frm_specifier.type == INT_X ||
                     obj.frm_specifier.type == INT_o)
               dst += _inshex(_out, dst, &obj.frm_specifier, va_arg(ap, unsigned int));
            else if (obj.frm_specifier.type == INT_c)
               dst += _inschar(_out, dst, va_arg(ap, unsigned int));
            else if (obj.frm_specifier.type == INT_s)
               dst += _insstring(_out, dst, va_arg(ap, char *), obj.frm_specifier.width);
            else if (obj.frm_specifier.type == FL_f ||
                     obj.frm_specifier.type == FL_g ||
                     obj.frm_specifier.type == FL_G ||
                     obj.frm_specifier.type == FL_L)
            {
               /*
                * if (!(arg%2))
                *    va_arg(ap, unsigned int);
                * This works only if the call with double has more than one argument :(
                */
               //du.i[0] = va_arg(ap, unsigned int);
               //du.i[1] = va_arg(ap, unsigned int);
               /*
                * XXX: BUG(s) workaround
                * All printf with double calls MUST HAVE ONLY ONE ARGUMENT, THE double.
                * 1) When double argument comes in even number it has 4bytes crap in front of it.
                * 2) va_arg(ap, double) comes in odd argument number fails cause it skips 4bytes before reading.
                * Someone have miss-correct a bug i think.
                */
               dst += _insfdouble (_out, dst, &obj.frm_specifier, va_arg(ap, double));
            }
            else if (obj.frm_specifier.type == FL_e ||
                     obj.frm_specifier.type == FL_E)
            {
               /*
                * if (!(arg%2))
                *    va_arg(ap, unsigned int);
                * This works only if the call with double has more than one argument :(
                */
                 //du.i[0] = va_arg(ap, unsigned int);
                 //du.i[1] = va_arg(ap, unsigned int);
                 /*
                  * XXX: BUG(s) workaround
                  * All printf with double calls MUST HAVE ONLY ONE ARGUMENT, THE double.
                  * 1) When double argument comes in even number it has 4bytes crap in front of it.
                  * 2) va_arg(ap, double) comes in odd argument number fails cause it skips 4bytes before reading.
                  * Someone have miss-correct a bug i think.
                  */
                 dst += _insedouble (_out, dst, &obj.frm_specifier, va_arg(ap, double));
            }
            else  // eat the wrong type to unsigned int
               dst += _insuint(_out, dst, &obj.frm_specifier, va_arg(ap, unsigned int));
            break;
         case _IO_FRM_TERMINATOR:
            _out (dst++, 0);
            break;
         case _IO_FRM_CRAP:
            break;
      }
   }
   return (int)(dst - dst_start);
}
