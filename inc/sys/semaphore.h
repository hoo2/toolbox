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
typedef struct {
   volatile int val;          /*!< Semaphore value. */
}sem_t;

/*
 * Semaphores
 */
void sem_init (sem_t* s, int v);
int  sem_close (sem_t *s);

 int sem_getvalue (sem_t *s); // Reads semaphore's value
 int sem_check (sem_t *s);    // Checks semaphore but return instead of waiting
void sem_wait (sem_t *s);     // Wait
void sem_post (sem_t *s);     // Feed

/*
 * Mutex
 */
void mut_init (sem_t* m, int v);
int  mut_close (sem_t *s);

 int mut_trylock (sem_t *m);  // Try to lock
void mut_lock (sem_t *m);     // wait to lock
void mut_unlock (sem_t *m);   // feed to unlock

#endif //#ifndef __semaphore_h__

