/*
 * \file usys.h
 * \brief
 *    A target independent micro system implementation
 * Provides:
 *    time(), clock(), cron(), ...
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
#ifndef __usys_h__
#define __usys_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

/*
 * ======= User defines =============
 */
#define ST_MAX_CRONTAB_ENTRIES      (10)

/*
 * ===== Data types ========
 */

/*
 * Also defined in types.h
 */
#ifndef  _CLOCK_T_
#define  _CLOCK_T_   unsigned long     /* clock() */
typedef _CLOCK_T_ clock_t;             /*!< CPU time type */
#endif
#ifndef _TIME_T_
#define  _TIME_T_ long                 /* time() */
typedef _TIME_T_ time_t;               /*!< date/time in unix secs past 1-Jan-70 type for 68 years*/
#endif

/*!
 * Pointer to void function (void) to use with cron
 */
typedef void (*cronfun_t) (void);

/*!
 * Cron Table data type
 */
typedef struct {
   cronfun_t   fun;
   clock_t     tic;
}crontab_t;

/*
 * extern declarations (from a HAL or Driver)
 */
extern clock_t get_freq (void);

/* ======== OS like Functionalities ============ */

/*!
 * \brief
 *    micro-system service.
 *
 * This service implements the SysTick callback function in order
 * to provide micro system - os like functionalities to an application
 * without RTOS
 */
void SysTick_Callback (void);

clock_t clock (void);
clock_t sclock (clock_t c);

time_t time(time_t *timer);
int stime(const time_t *t);

void service_add (cronfun_t pfun, clock_t interval);
void service_rem (cronfun_t pfun);


#ifdef __cplusplus
}
#endif

#endif // #ifndef __usys_h__
