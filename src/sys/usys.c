/*
 * \file usys.c
 * \brief
 *    A target independent micro system implementation
 * Provides:
 *    time(), clock(), ...
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2016 Choutouridis Christos (http://www.houtouridis.net)
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
#include <sys/usys.h>

/* ================== Static Time Data ======================= */

static clock_t volatile __ticks;       /*!< CPU time */
static time_t  volatile __now;         /*!< Time in unix secs past 1-Jan-70 */


/*!
 *  Cron Table holds all requested entries for periodic function calls.
 *  \note
 *    All the entries will run in privileged mode and will use the main
 *    stack.
 */
static crontab_t  _crontab[ST_MAX_CRONTAB_ENTRIES];

void SysTick_Callback (void)
{
   int i;

   // Time
   ++__ticks;
   if ( !(__ticks % get_freq ()) )
      ++__now;

   // Cron
   for (i=0 ; i<ST_MAX_CRONTAB_ENTRIES ; ++i) {
      if (_crontab[i].fun && !(__ticks %_crontab[i].tic))
         _crontab[i].fun ();
   }
}


/*!
 * \brief
 *  determines the processor time used.
 * \return
 *  the implementation's best approximation to the processor time
 *  used by the program since program invocation. The time in
 *  seconds is the value returned divided by the value of the macro
 *  CLK_TCK or CLOCKS_PER_SEC
 */
inline clock_t clock (void) {
   return (clock_t) __ticks;
}

/*!
 * \brief
 *    Set the processor time used.
 * \param
 *    clk   Pointer to new CPU time value
 * \return
 *    The implementation's best approximation to the processor time
 *    used by the program since program invocation. The time in
 *    seconds is the value returned divided by the value of the macro
 *    CLK_TCK or CLOCKS_PER_SEC
 */
inline clock_t sclock (clock_t c) {
   return __ticks = c;
}

/*!
 * \brief
 *  determines the current calendar time. The encoding of the value is
 *  unspecified.
 * \return
 *  The implementations best approximation to the current calendar
 *  time. If timer is not a null pointer, the return value
 *  is also assigned to the object it points to.
 */
time_t time(time_t *timer)
{
   if (timer)
      *timer = (time_t)__now;
   return (time_t)__now;
}


/*!
 * \brief
 *    Sets the system's idea of the time and date.  The time,
 *    pointed to by t, is measured in seconds since the Epoch, 1970-01-01
 *    00:00:00 +0000 (UTC).
 * \param
 *    t     Pointer to new system's time and date.
 * \return
 *    On success, zero is returned.  On error, -1 is returned
 */
int stime(const time_t *t) {
   if (t) {
      __now = *t;
      return 0;
   }
   else
      return -1;
}

/*!
 * \brief
 *    Add a function to cron
 *
 * \param   pfun  Pointer to function
 * \param   tic   Tick period. Cron will run the \a pfun every \b tic Ticks
 *
 * \note
 *    All the entries will run in privileged mode and will use the main
 *    stack.
 */
void service_add (cronfun_t pfun, clock_t tic)
{
   uint32_t i;
   for (i=0 ; i<ST_MAX_CRONTAB_ENTRIES ; ++i)
      if (!_crontab[i].fun) {
         _crontab[i].fun = pfun;
         _crontab[i].tic = tic;
         return;
      }
}

/*!
 * \brief
 *    Remove a function from cron
 *
 * \param   pfun Pointer to function
 */
void service_rem (cronfun_t pfun)
{
   int i;
   for (i=0 ; i<ST_MAX_CRONTAB_ENTRIES ; ++i)
      if (_crontab[i].fun == pfun)
         _crontab[i].fun = (void*)0;
}
