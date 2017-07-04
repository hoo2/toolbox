/*
 * \file sem.c
 * \brief
 *    This file provides semaphore/mutex capabilities
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
 */

#include <sys/semaphore.h>

/*!
 * \brief
 *    Open/Initialize semaphore.
 *
 * \param s,   Pointer to semaphore to initialize
 * \param v    The initial value of semaphore.
 */
__Os__ static void _sinit (sem_t *s, int v) {
   if (s)
      s->val = v;
}

/*!
 * \brief
 *    Open/Initialize semaphore.
 * \note
 *    The usual init value of a semaphore is 0.
 *
 * \param s,   Pointer to semaphore to initialize
 * \param v    The initial value of semaphore.
 */
__Os__ inline void sem_init (sem_t* s, int v) {
   _sinit (s, v);
}

/*!
 * \brief
 *    Close/De-Initialize a semaphore.
 *
 * \param   s,   Pointer to semaphore to close
 * \return  0
 */
__Os__ int sem_close (sem_t *s) {
   return s->val = 0;
}

/*!
 * \brief
 *    Get semaphore's value without any interaction to it
 *
 * \param  s pointer to semaphore used
 * \return The semaphore value
 */
__O3__ inline int sem_getvalue (sem_t *s) {
   return s->val;
}

/*!
 * \brief
 *    This function checks for a semaphore value. If the semaphore
 *    is positive decreases it and return true. Else return false
 *
 * \param  s pointer to semaphore used
 * \return true for positive semaphore value.
 *
 * \note Thread safe, not reentrant.
 */
__O3__ int sem_check (sem_t *s)
{
   if (s->val>0) {
      --s->val;
      return 1;
   }
   else
      return 0;
}

/*!
 * \brief
 *    This function waits for a semaphore(spin-lock). If the semaphore
 *    is positive decreases it and continue.
 *
 * \param  s pointer to semaphore used
 * \return None
 * \note Thread safe, not reentrant.
 */
__O3__ void sem_wait (sem_t *s)
{
   while (s->val<=0)
      ;
   --s->val;
}

/*!
 * \brief Increase the semaphores value
 */
__O3__ inline void sem_post (sem_t *s) {
   ++s->val;
}






/*!
 * \brief
 *    Open/Initialize mutex which is a binary semaphore for this implementation
 * \note
 *    The usual init value of a mutex is 0 (unlocked)
 * \param s,   Pointer to semaphore to initialize
 * \param v    The initial value of mutex.
*/
__Os__ inline void mut_init (sem_t* m, int v) {
   _sinit (m, v);
}

/*!
 * \brief
 *    Close/De-Initialize a mutex.
 *
 * \param   s,   Pointer to mutex to close
 * \return  0
 */
__Os__ int mut_close (sem_t *m) {
   return m->val = 0;
}

/*!
 * \brief
 *    This function checks for a mutex.
 *    If its zero (unlocked) increases it and return true.
 *    Else return false (already locked)
 *
 * \param  s pointer to mutex used
 * \return the status of the operation
 *    \arg  0  Fail to lock, mutex already locked
 *    \arg  1  Success, mutex is locked by the function
 *
 * \note Thread safe, not reentrant.
 */
__O3__ int mut_trylock (sem_t *m)
{
   if (m->val<=0) {
      m->val = 1;
      return 1;
   }
   else
      return 0;
}

/*!
 * \brief
 *    This function waits for a mutex (spin-lock).
 *    If is zero(unlocked), increases it and continue.
 *    If the mutex is positive(1, already locked) waits.
 *
 * \param  s pointer to mutex used
 * \return None
 * \note Thread safe, not reentrant.
 */
__O3__ inline void mut_lock (sem_t *m)
{
   while (m->val>0)
      ;
   m->val = 1;
}

/*!
 * Unlock (by setting low) the semaphore.
*/
__O3__ inline void mut_unlock (sem_t *m) {
   m->val = 0;
}

