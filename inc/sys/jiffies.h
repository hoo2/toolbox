/*
 * \file jiffies.h
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
#ifndef __jiffies_h__
#define __jiffies_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <tbx_types.h>
#include <toolbox_defs.h>

typedef uint16_t     jiffy_t;       //!< Jiffy type 2 byte unsigned integer
typedef int32_t      jtime_t;        //!< Jiffy time type for delay functionalities usec/msec
typedef int          (*jf_setfreq_pt) (uint32_t, uint32_t);   //!< Pointer to setfreq function \sa setfreq

/*!
 * Jiffy inner structure,
 * \info
 *    We use jiffies to count small time intervals and usually this is
 *    below SysTick Interrupt. So we use an indepentend counter for that.
 *    Linux names jiffies the Tick per sec. This is toooo big for us.
 *    We name jiffy each tick of the extra timer.
 *    The equalivent of linux jiffies is the return value of clock ().
 */
typedef volatile struct {
   jf_setfreq_pt  setfreq;       /*!< Pointer to driver's timer set freq function */
   /*
    * \note
    *   This function must get an integer value for timer's desired
    *   frequency and returns the  maximum jiffy value. This usual
    *   refers to timer's auto reload value.
    */
   jiffy_t        *value;        /*!< Pointer to timers current value */
   uint32_t       freq;          /*!< timer's  frequency */
   jiffy_t        jiffies;       /*!< jiffies max value (timer's max value) */
   jiffy_t        jp1ms;         /*!< Jiffies per 1 msec to use in delay function */
   jiffy_t        jp1us;         /*!< Jiffies per 1 usec to use in delay function */
   jiffy_t        jp100ns;       /*!< Jiffies per 100 nsec to use in delay function */
   drv_status_en  status;
}jf_t;


/*
 *  ============= PUBLIC jiffy API =============
 */

/*
 * Link and Glue functions
 */
void jf_link_setfreq (jf_setfreq_pt pfun);
void jf_link_value (jiffy_t* v);

/*
 * Set functions
 */

/*
 * User Functions
 */
drv_status_en jf_probe (void);
void jf_deinit (void);
drv_status_en jf_init (uint32_t jf_freq, jiffy_t jiffies);

jiffy_t jf_get_jiffies (void);
jiffy_t jf_get_jiffy (void);
jiffy_t jf_per_msec (void);
jiffy_t jf_per_usec (void);
jiffy_t jf_per_100nsec (void);

void jf_delay_ms (jtime_t msec);
void jf_delay_us (jtime_t usec);
void jf_delay_100ns (jtime_t _100nsec);
int jf_check_msec (jtime_t msec);
int jf_check_usec (jtime_t usec);
int jf_check_100nsec (jtime_t _100nsec);

/*!
 * \note
 * The Jiffy lib has no jiffy_t target pointer in the API. This means
 * that IT CAN BE ONLY ONE jiffy timer per application.
 */



#ifdef __cplusplus
 }
#endif

#endif   //#ifndef __jiffies_h__
