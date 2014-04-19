/*

	stime.h - Simple time keaping library header

   Copyright (C) 2012 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       07/2012
   Version:    0.1

*/

#ifndef __stime_h__
#define __stime_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdint.h>

/* =================== User Defines ====================== */
#define JULIAN_CALENDAR       (0)
#define GREGORIAN_CALENDAR    (1)

/* =============== General defines =============== */
// Default calendar is Gregorian
#ifndef TIME_CALENDAR
#define TIME_CALENDAR (GREGORIAN_CALENDAR)
#endif
/* =================== Exported Functions ===================== */

int isleap(int year);
time_t smktime (struct tm *_timeptr);
struct tm *sgmtime (const time_t *_timer);

#ifdef __cplusplus
}
#endif

#endif //#ifdef __time_h



