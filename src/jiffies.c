/*
 * \file jiffies.c
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Houtouridis Christos. The intellectual
 * and technical concepts contained herein are proprietary to
 * Houtouridis Christos and are protected by copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Houtouridis Christos.
 *
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       06/2013
 * Version:    0.1
 *
 */
#include <jiffies.h>

static jf_t _jf;

#define JF_MAX_TIM_VALUE      (0xFFFF)    // 16bit counters

/*
 * ======================   Public functions   ======================
 */

/*!
 * \brief
 *    Connect the Driver's Set frequency function to jiffy struct
 * \note
 *    This function get a freq value and returns the timers max jiffy value
 *    (usual this refers to timer'sauto reload value).
 */
void jf_connect_setfreq (jf_setfreq_pt pfun)
{
   if (pfun)
      _jf.setfreq = pfun;
}

/*!
 * \brief
 *    Connect the timer's value to jiffy struct
 */
void jf_connect_value (jiffy_t* v)
{
   if (v)
      _jf.value = v;
}

/*!
 * \brief
 *    De-Initialize the jf data and un-connect the functions
 *    from the driver
 */
void jf_deinit (void)
{
   _jf.setfreq = (void*)0;
   _jf.value = (void*)0;
   _jf.freq = _jf.jiffies = _jf.jpus = 0;
}

/*!
 * \brief
 *    Initialize the jf to a desired jiffy frequency \a f
 * \note
 *    This function has no effect if the inner jiffy struct
 *    is un-connected to driver. So you have to call
 *    \sa jf_connect_setfreq() and \sa jf_connect_value() first.
 */
void jf_init (uint32_t f)
{
   if (_jf.setfreq)
   {
      _jf.jiffies = _jf.setfreq (f);
      _jf.freq = f;
      _jf.jpus = jf_per_usec ();
   }
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
   if (_jf.freq <=1000)
      return (_jf.jiffies * _jf.freq) / 1000;
      // We can not count beyond timer's reload
   else
      return 0;
   /*            1
    * 1000Hz = -----  , Its not a magic number
    *          1msec
    */
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
   if (_jf.freq <=1000000) // 1000000Hz = 1/1usec
      return (_jf.jiffies * _jf.freq) / 1000000;
      // We can not count beyond timer's reload
   else
      return 0;
   /*               1
    * 1000000Hz = -----  , Its not a magic number
    *             1usec
    */
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
void jf_delay_us (int32_t usec)
{
   jiffy_t m2, m1 = *_jf.value;

   usec *= _jf.jpus;
   if (*_jf.value - m1 > usec) // Very small delays will return here.
      return;

   // Delay loop: Eat the time difference from usec value.
   while (usec>0)
   {
      m2 = *_jf.value;
      if (m2 > m1)
      {
         usec -= (m2 - m1);
         m1 = m2;
      }
      else if (m2 < m1)
      {
         // Overflow
         usec -= _jf.jiffies - m1 + m2;
         m1 = m2;
      }
   }
}
