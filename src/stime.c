/*
 * stime.c - Simple and small footprint time keeping library
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
 * Version:    0.11
 *
 */

#include "stime.h"

static struct tm _td;                         /*!< Time struct for conversions */

/*
 * \brief
 *  Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 *  Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 *  => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 *  [For the Julian calendar (which was used in Russia before 1917,
 *  Britain & colonies before 1752, anywhere else before 1582,
 *  and is still in use by some communities) leave out the
 *  -year/100+year/400 terms, and add 10.]
 *
 *  This algorithm was first published by Gauss (I think).
 *
 * \note
 *  This function will overflow on 2106-02-07 06:28:16 on
 *  machines where long is 32-bit!
 */
time_t smktime (struct tm *_timeptr)
{
   unsigned int mon = _timeptr->tm_mon, year = _timeptr->tm_year;

   /* 1..12 -> 11,12,1..10 */
   if (0 >= (int) (mon -= 2))
   {
      mon += 12;      /* Puts Feb last since it has leap day */
      year -= 1;
   }

   return ((((time_t)
               (year/4 - year/100 + year/400 + 367*mon/12 + _timeptr->tm_mday) + year*365 - 719499
            )*24 + _timeptr->tm_hour    /* now have hours */
           )*60 + _timeptr->tm_min      /* now have minutes */
          )*60 + _timeptr->tm_sec;      /* finally seconds */
}



struct tm *slocaltime (const time_t *_timer)
{
   time_t t = *_timer;

   _td.tm_mon = _td.tm_year = 0;
   _td.tm_yday = t/86400;
   _td.tm_hour = (t -= _td.tm_yday*86400)/3600;
   _td.tm_min = (t -= _td.tm_hour*3600)/60;
   _td.tm_sec = (t -= _td.tm_min*60);

   return &_td;
}


