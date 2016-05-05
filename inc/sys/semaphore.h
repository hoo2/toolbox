/*
 * \file sem.c
 * \brief
 *    This file provides pkernel combatible semaphore/mutex capabilities
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
 * Date:       09/2013
 * Version:
 *
 */


#ifndef __semaphore_h__
#define __semaphore_h__

#include <toolbox_defs.h>
#include <stdlib.h>

/*!
 * Semaphore data type
 */
typedef volatile struct
{
   int val;          /*!< Semaphore value. */
}sem_t;

sem_t* sem_open(int v);
sem_t* mut_open (int v);
int    sem_close (sem_t *s);

int semaphore (sem_t *s);  // Checks
void wait (sem_t *s);      // Wait
void signal (sem_t *s);    // Feed

int mutex (sem_t *m);      // Checks
void lock (sem_t *m);      // wait to lock
void unlock (sem_t *m);    // feed to unlock

#endif //#ifndef __semaphore_h__

