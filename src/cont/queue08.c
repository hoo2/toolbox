/*!
 * \file queue08.c
 * \brief
 *    This file provides queue capability based on a deque
 *
 * Copyright (C) 2014 Houtouridis Christos <houtouridis.ch@gmail.com>
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
#include <cont/queue08.h>


/*
 *  ============= Public Queue API =============
 */

/*
 * Link and Glue functions
 */
inline void queue08_link_buffer (queue08_t *q, byte_t* buf) {
   deque08_link_buffer(q, buf);
}

/*
 * Set functions
 */
inline void queue08_set_capacity (queue08_t *q, size_t capacity) {
   deque08_set_capacity(q, capacity);
}

inline bool queue08_set_trigger (queue08_t *q, deque_callback_ft callback, trigger_mode_en mode, size_t value) {
   return deque08_set_trigger (q, callback, mode, value);
}

inline void queue08_clear_trigger (queue08_t *q) {
   deque08_clear_trigger(q);
}

/*
 * User Functions
 */

/*!
 * \brief
 *    Check if queue is full
 * \param   q     Which queue to check
 * \return
 *    \arg  0     Not full
 *    \arg  1     Full
 */
__O3__ int queue08_is_full (queue08_t *q) {
   return deque08_is_full (q);
}

/*!
 * \brief
 *    Check if queue is empty
 * \param   q     Which queue to check
 * \return
 *    \arg  0     Not empty
 *    \arg  1     Empty
 */
__O3__ int queue08_is_empty (queue08_t *q) {
   return deque08_is_empty (q);
}

/*!
 * \brief
 *    Return the number of items on queue
 * \param   q     Which queue to check
 */
__O3__ int  queue08_size (queue08_t *q) {
   return deque08_size (q);
}

/*!
 * \brief
 *    Return the number of items on queue
 * \param   q     Which queue to check
 */
__Os__ void  queue08_flush (queue08_t *q) {
   deque08_flush (q);
}



/*!
 * \brief
 *    Initialize the queue
 * \param   q     Which queue to init
 */
__Os__ void queue08_init (queue08_t *q) {
   deque08_init (q);
}

/*!
  * \brief
  *   This function push a byte in the back of queue.
  * \param  q  Pointer to queue to use
  * \param  b  byte to push
  * \return
  *   \arg  0  Full queue
  *   \arg  1  Done
 */
__Os__ int queue08_push (queue08_t *q, byte_t b) {
   return deque08_push_back(q, b);
}

/*!
 * \brief
 *   This function push an arbitrary number of bytes to the back of queue.
 * \param  q     Pointer to deque to use
 * \param  num   number of bytes to push
 * \return
 *   \arg  0  Full queue
 *   \arg  1  Done
 */
__Os__ int queue08_vpush (queue08_t *q, size_t num, ...) {
   va_list args;
   va_start(args, num);
   int ret = deque08_vpush_back(q, num, args);
   va_end(args);
   return ret;
}

/*!
  * \brief
  *   This function pops a byte from the queue.
  * \param  q  Pointer to queue to use
  * \param  b  Pointer to byte to return
  * \return
  *   \arg  0  Empty queue
  *   \arg  1  Done
 */
__Os__ int queue08_pop (queue08_t *q, byte_t *b) {
   return deque08_pop_front(q, b);
}

/*!
  * \brief
  *   This function gives the last item in the back of deque.
  * \param  q  Pointer to deque to use
  * \param  b  Pointer to byte to return
  * \return
  *   \arg  0  Empty queue
  *   \arg  1  Done
 */
__Os__ int queue08_back (queue08_t *q, byte_t *b) {
   return deque08_back (q, b);
}

/*!
  * \brief
  *   This function gives the first item in the front of deque.
  * \param  q  Pointer to deque to use
  * \param  b  Pointer to byte to return
  * \return
  *   \arg  0  Empty queue
  *   \arg  1  Done
 */
__Os__ int queue08_front (queue08_t *q, byte_t *b) {
   return deque08_front (q, b);
}

__Os__ inline bool queue08_check_trigger (queue08_t *q) {
   return deque08_check_trigger(q);
}
