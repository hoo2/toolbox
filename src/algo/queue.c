/*
 * \file queue.c
 * \brief
 *    This file provides queue capability based on a ring buffer
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
#include <algo/queue.h>


/*
 *  ============= Public Queue API =============
 */

/*
 * Link and Glue functions
 */
void queue_link_buffer (queue_t *q, void* buf) {
   q->buf = buf;
}


/*
 * Set functions
 */
inline void queue_set_item_size (queue_t *q, int size) {
   q->item_size = size;
}
inline void queue_set_items (queue_t *q, int items) {
   q->items = items;
}

/*
 * User Functions
 */


/*!
 * \brief
 *    Check if queue is full
 * \param   queue    Which queue to check
 * \return
 *    \arg  0     Not full
 *    \arg  1     Full
 */
__O3__ int queue_is_full (queue_t *q)
{
   if (q->head == 0)
      return (q->tail == q->items-1) ? 1 : 0;
   else
      return (q->head == q->tail+1) ? 1 : 0;
}

/*!
 * \brief
 *    Check if queue is empty
 * \param   queue    Which queue to check
 * \return
 *    \arg  0     Not empty
 *    \arg  1     Empty
 */
__O3__ int queue_is_empty (queue_t *q) {
   return (q->head == q->tail) ? 1 : 0;
}

/*!
 * \brief
 *    Return the number of items on queue
 * \param   queue    Which queue to check
 */
__O3__ int  queue_waiting (queue_t *q) {
   return (q->tail >= q->head) ? q->tail - q->head : q->items - (q->head - q->tail);
}

/*!
 * \brief
 *    Return the number of items on queue
 * \param   queue    Which queue to check
 */
__Os__ void  queue_flush (queue_t *q) {
   q->tail = q->head = 0;
}

/*!
 * \brief
 *    Initialize the queue
 * \param   queue    Which queue to init
 */
__Os__ void queue_init (queue_t *q) {
   q->head = q->tail = 0;
}

/*!
  * \brief
  *   This function puts a byte to queue.
  * \param  byte to put
  * \return
  *   \arg  0  Full queue
  *   \arg  1  Done
 */
__Os__ int queue_put (queue_t *q, void *b)
{
   if (queue_is_full (q) == 1)  //full queue
      return 0;
   memcpy ((void*)&q->buf[q->tail*q->item_size], b, q->item_size);
   //rotate pointer
   if ( ++q->tail >= q->items )
      q->tail = 0;
   return 1;
}

/*!
  * \brief
  *   This function gets a byte from queue.
  * \param  pointer to byte
  * \return
  *   \arg  0  Empty queue
  *   \arg  1  Done
 */
__Os__ int queue_get (queue_t *q, void *b)
{
   if ( queue_is_empty (q) )    //Empty queue
      return 0;
   memcpy (b, (const void*)&q->buf[q->head*q->item_size], q->item_size);
   //rotate pointers
   if ( ++q->head >= q->items )
      q->head = 0;
   return 1;
}

/*!
  * \brief
  *   This function returns the head address.
  */
inline void* queue_head (queue_t *q){
   return (void*)&q->buf[q->head];
}

/*!
  * \brief
  *   This function returns the tail address.
  */
inline void* queue_tail (queue_t *q){
   return (void*)&q->buf[q->tail];
}
