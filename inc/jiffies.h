/*
 * \file jiffies.h
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
#ifndef __jiffies_h__
#define __jiffies_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

typedef int32_t   jiffy_t;    /*!< Jiffy type 4 byte integer */

typedef uint32_t (*jf_setfreq_pt) (uint32_t);   /*!< Pointer to setfreq function \sa setfreq */

/*!
 * Jiffy inner structure,
 * \info
 *    We use jiffies to count small time intervals and usually this is
 *    below SysTick Interrupt. So we use an indepentend counter for that.
 *    Linux names jiffies the Tick per sec. This is toooo big for us.
 *    We name jiffy each tick of the extra timer.
 *    The equalivent of linux jiffies is the return value of clock ().
 */
typedef volatile struct
{
   uint32_t       freq;          /*!< timer's overflow frequency */
   uint32_t       jiffies;       /*!< jiffies max value (timer's max value) */
   jf_setfreq_pt  setfreq;       /*!< Pointer to driver's timer set freq function
                                      \note
                                         This function must get an integer value for timer's desired
                                         frequency and returns the  maximum jiffy value. This usual
                                         refers to timer's auto reload value.
                                  */
   jiffy_t        *value;        /*!< Pointer to timers current value */
   jiffy_t        jpus;          /*!< Variable for the delay function */
}jf_t;



/*
 * Public functions
 */
void jf_connect_setfreq (jf_setfreq_pt pfun);
void jf_connect_value (jiffy_t* v);

void jf_deinit (void);
void jf_init (uint32_t f);

jiffy_t jf_get_jiffies (void);
jiffy_t jf_get_jiffy (void);

// Helper functions
jiffy_t jf_per_msec (void);
jiffy_t jf_per_usec (void);
void jf_delay_us (int32_t usec);

#ifdef __cplusplus
 }
#endif

#endif   //#ifndef __jiffies_h__
