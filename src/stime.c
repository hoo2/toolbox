/*!
 * \file stime.c
 * \brief
 *    Simple and small footprint time keeping library
 *    with very basic functionality.
 *
 * This file is part of toolbox
 *
 * Copyright (C) CC Computer Consultants GmbH, 2007
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
 * \note
 *    Based on ipt_time by Fabrice MARIE <fabrice@netfilter.org>
 *    This is a module which is used for time matching
 *    It is using some modified code from dietlibc (gmtime() function)
 *    that you can find at http://www.fefe.de/dietlibc/
 *
 */

#include <std/stime.h>

/*!
 *  days per month -- nonleap!
 */
static const int _spm[13] =
{
   0,
   (31),
   (31+28),
   (31+28+31),
   (31+28+31+30),
   (31+28+31+30+31),
   (31+28+31+30+31+30),
   (31+28+31+30+31+30+31),
   (31+28+31+30+31+30+31+31),
   (31+28+31+30+31+30+31+31+30),
   (31+28+31+30+31+30+31+31+30+31),
   (31+28+31+30+31+30+31+31+30+31+30),
   (31+28+31+30+31+30+31+31+30+31+30+31),
};

/*!
 *  Time struct for conversions
 */
static struct tm _rt;

/*!
 * \brief
 *    Check if the given year is leap.
 * \param   year  The year to check
 * \return  non zero if the year is leap, zero else
 */
inline int isleap (int year) {
   /*
    * every fourth year is a leap year except for century years that are
    * not dividable by 400.
    */
   return (!(year%4) && ((year%100) || !(year%400)));
}

/*!
 * \brief
 *    Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 *    Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 *    => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 *  This algorithm was first published by Gauss (I think).
 *
 * \param   _t  pointer to a calendar date and time broken down into its components.
 *
 * \return  the UNIX time since EPOCH.
 * \note
 *    This function will overflow on 2106-02-07 06:28:16 on
 *    machines where long is 32-bit!
 *  \note
 *    For the Julian calendar which used
 *       - in Russia before 1917,
 *       - Britain & colonies before 1752,
 *       - anywhere else before 1582,
 *       - and is still in use by some communities
 *    leave out the -year/100+year/400 terms, and add 10.
 */
time_t smktime (struct tm *_t)
{
   unsigned int mon = _t->tm_mon,
                year = _t->tm_year;

   /* 1..12 -> 11,12,1..10 */
   if ((int) (mon -= 2) <= 0) {
      mon += 12;  /* Puts Feb last since it has leap day */
      year -= 1;
   }

   #if TIME_CALENDAR == GREGORIAN_CALENDAR
   // Gregorian leap year
   return (time_t)(((((unsigned long) (year/4 - year/100 + year/400
   #else
   // Julian leap year
   return (time_t)(((((unsigned long) (year/4 + 10
   #endif
                        + 367*mon/12 + _t->tm_mday) + year*365 - 719499 )
                     *24 + _t->tm_hour )     /* add hours */
                     *60 + _t->tm_min  )     /* add minutes */
                     *60 + _t->tm_sec  );    /* finally add seconds */
}

/*!
 * \brief
 *    Uses the value pointed by timer to fill a tm structure with the values
 *    that represent the corresponding time, expressed as a UTC time
 *    (i.e., the time at the GMT timezone).
 *
 * \param  _timer  A pointer to an object of type time_t that contains a time value.
 * \return         A pointer to a tm structure with its members filled with the values that
 *                 correspond to the UTC time representation of timer.
 */
struct tm *gmtime  (const time_t *_timer)
{
   #define _SPD   (86400)
   time_t i, k;
   time_t work=*_timer%_SPD;

   _rt.tm_sec = work%60;    work /= 60;
   _rt.tm_min = work%60;
   _rt.tm_hour = work/60;   work = *_timer/_SPD;
   _rt.tm_wday = (4+work)%7;   // Day one was Thursday
   // Find the year
   for (i=1970; 1 ; ++i)
   {
      k = isleap(i) ? 366:365;
      if (work >= k)
         work -= k;
      else
         break;
   }
   _rt.tm_year = i;
   _rt.tm_yday = work;
   _rt.tm_mday = 1;
   if (isleap(i) && (work>58))
   {
      if (work==59)  // Add leap day
         _rt.tm_mday = 2;
      work -= 1;
   }

   // Find month and month-day
   for (i=11; i && (_spm[i]>work); --i)
      ;
   _rt.tm_mon = i+1;
   _rt.tm_mday += (work - _spm[i]);

   #undef _SPD
   return (struct tm *)&_rt;
}


