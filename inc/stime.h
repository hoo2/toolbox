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


#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

/* =================== Exported Functions ===================== */

/* determines the processor time used.
 * Returns: the implementation's best approximation to the processor time
 *          used by the program since program invocation. The time in
 *          seconds is the value returned divided by the value of the macro
 *          CLK_TCK or CLOCKS_PER_SEC
 */



time_t smktime (struct tm *_timeptr);
struct tm *slocaltime (const time_t *_timer);


#ifdef __cplusplus
}
#endif

#endif //#ifdef __time_h



