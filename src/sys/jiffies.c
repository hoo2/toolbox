/*
 * \file jiffies.c
 * \brief
 *    A target independent jiffy functionality
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
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
#include <sys/jiffies.h>

static jf_t _jf;

#define JF_MAX_TIM_VALUE      (0xFFFF)    // 16bit counters

/*
 * ======================   Public functions   ======================
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief
 *    Connect the Driver's Set frequency function to jiffy struct
 * \note
 *    This function get a freq value and returns the timers max jiffy value
 *    (usual this refers to timer'sauto reload value).
 */
void jf_link_setfreq (jf_setfreq_pt pfun)
{
   if (pfun)
      _jf.setfreq = pfun;
}

/*!
 * \brief
 *    Connect the timer's value to jiffy struct
 */
void jf_link_value (jiffy_t* v)
{
   if (v)
      _jf.value = v;
}



/*
 * User Functions
 */

/*!
 * \brief
 *    Check jiffy's status
 * \return status
 */
inline drv_status_en jf_probe (void) {
   return _jf.status;
}

/*!
 * \brief
 *    De-Initialize the jf data and un-connect the functions
 *    from the driver
 */
void jf_deinit (void)
{
   if (_jf.setfreq) _jf.setfreq (0, 0);
   memset ((void*)&_jf, 0, sizeof (jf_t));
   _jf.status = DRV_NODEV;
}

/*!
 * \brief
 *    Initialise the jf to a desired jiffy frequency \a f
 * \note
 *    This function has no effect if the inner jiffy struct
 *    is un-connected to driver. So you have to call
 *    \sa jf_connect_setfreq() and \sa jf_connect_value() first.
 * \return Zero on success, non zero on error
 */
int jf_init (uint32_t jf_freq, jiffy_t jiffies)
{
   if (_jf.setfreq)
   {
      _jf.status = DRV_NOINIT;
      if ( _jf.setfreq (jf_freq, jiffies) )
         return 1;
      _jf.jiffies = jiffies;
      _jf.freq = jf_freq;
      _jf.jpus = jf_per_usec ();
      _jf.status = DRV_READY;
      return 0;
   }
   _jf.status = DRV_NODEV;
   return 1;
}

/*!
 * \brief
 *    Return the maximum jiffy value.
 */
inline jiffy_t jf_get_jiffies (void){
   return _jf.jiffies;
}

/*!
 * \brief
 *    Return the current jiffy value.
 * \note
 *    Usual this function returns the value of a register from a timer peripheral
 *    in the MCU. Keep in mind that its value is a moving target!
 */
inline jiffy_t jf_get_jiffy (void){
   return *_jf.value;
}

/*!
 * \brief
 *    Return the systems best approximation for jiffies per msec
 * \return
 *    The calculated value or zero if no calculation can apply
 *
 * \note
 *    The result tend to differ as the jiffies and freq values decreasing
 */
jiffy_t jf_per_msec (void)
{
   jiffy_t jf = (jiffy_t)(_jf.freq / 1000);
   /*            1
    * 1000Hz = -----  , Its not a magic number
    *          1msec
    */
   if (jf <= 1)      return 1;
   else              return jf;
}

/*!
 * \brief
 *    Return the systems best approximation for jiffies per usec
 * \return
 *    The calculated value or zero if no calculation can apply
 *
 * \note
 *    The result tend to differ as the jiffies and freq values decreasing
 */
jiffy_t jf_per_usec (void)
{
   jiffy_t jf = (jiffy_t)(_jf.freq / 1000000);
   /*            1
    * 1000000Hz = -----  , Its not a magic number
    *          1usec
    */
   if (jf <= 1)      return 1;
   else              return jf;
}

/*!
 * \brief
 *    A code based delay implementation, using jiffies for timing.
 *    This is NOT accurate but it ensures that the time passed is always
 *    more than the requested value.
 *    The delay values are multiplications of 1 usec.
 * \param
 *    usec     Time in usec for delay
 */
void jf_delay_us (jtime_t usec)
{
   jtime_t m, m2, m1 = (jtime_t)*_jf.value;

   usec *= _jf.jpus;
   if ((jtime_t)(*_jf.value) - m1 > usec) // Very small delays may return here.
      return;

   // Eat the time difference from usec value.
   while (usec>0) {
      m2 = (jtime_t)(*_jf.value);
      m = m2 - m1;
      usec -= (m>=0) ? m : _jf.jiffies + m;
      m1 = m2;
   }
}

/*!
 * \brief
 *    A code based delay implementation, using jiffies for timing.
 *    This is NOT accurate but it ensures that the time passed is always
 *    more than the requested value.
 *    The delay values are multiplications of 1 msec.
 * \param
 *    msec     Time in msec for delay
 */
void jf_delay_ms (jtime_t msec)
{
   jtime_t m, m2, m1 = (jtime_t)*_jf.value;

   msec *= jf_per_msec ();

   // Eat the time difference from msec value.
   while (msec>0) {
      m2 = (jtime_t)(*_jf.value);
      m = m2 - m1;
      msec -= (m>=0) ? m : _jf.jiffies + m;
      m1 = m2;
   }
}

/*!
 * \brief
 *    A code based polling version delay implementation, using jiffies for timing.
 *    This is NOT accurate but it ensures that the time passed is always
 *    more than the requested value.
 *    The delay values are multiplications of 1 usec.
 * \param
 *    usec     Time in usec for delay
 */
int jf_check_usec (jtime_t usec)
{
   static jtime_t m1=-1, cnt;
   jtime_t m, m2;

   if (m1 == -1) {
      m1 = *_jf.value;
      cnt = _jf.jpus * usec;
   }

   // Eat the time difference from usec value.
   if (cnt>0) {
      m2 = (jtime_t)(*_jf.value);
      m = m2-m1;
      cnt -= (m>=0) ? m : _jf.jiffies + m;
      m1 = m2;
      return 1;   // wait
   }
   else {
      m1 = -1;
      return 0;   // do not wait any more
   }
}
