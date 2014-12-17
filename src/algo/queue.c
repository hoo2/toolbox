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
 *  ============= PUBLIC EE API =============
 */

/*
 * Link and Glue functions
 */
void queue_link_buffer (queue_t *queue, void* buf) {
   queue->buf = buf;
}


/*
 * Set functions
 */
void queue_set_item_size (queue_t *queue, int size) {
   queue->item_size = size;
}
void queue_set_items (queue_t *queue, int items) {
   queue->items = items;
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
int queue_is_full (queue_t *queue)
{
   if (queue->head == 0)
      return (queue->tail == queue->items) ? 1 : 0;
   else
      return (queue->head == queue->tail+1) ? 1 : 0;
}

/*!
 * \brief
 *    Check if queue is empty
 * \param   queue    Which queue to check
 * \return
 *    \arg  0     Not empty
 *    \arg  1     Empty
 */
int queue_is_empty (queue_t *queue) {
   return (queue->head == queue->tail) ? 1 : 0;
}

/*!
 * \brief
 *    Initialize the queue
 * \param   queue    Which queue to init
 */
void queue_init (queue_t *queue)
{
   queue->head = queue->tail = 0;
}

/*!
  * \brief
  *   This function puts a byte to queue.
  * \param  byte to put
  * \return
  *   \arg  0  Full queue
  *   \arg  1  Done
 */
int queue_put (queue_t *queue, void *b)
{
   if (queue_is_full (queue) == 1)  //full queue
      return 0;
   memcpy ((void*)&queue->buf[queue->tail], b, queue->item_size);
   //rotate pointer
   if ( ++queue->tail >= queue->items )
      queue->tail = 0;
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
int queue_get (queue_t *queue, void *b)
{
   if ( queue_is_empty (queue) )    //Empty queue
      return 0;
   memcpy (b, (const void*)&queue->buf[queue->head], queue->item_size);
   //rotate pointers
   if ( ++queue->head >= queue->items )
      queue->head = 0;
   return 1;
}

/*!
  * \brief
  *   This function returns the head address.
  */
void* queue_head (queue_t *queue){
   return (void*)&queue->buf[queue->head];
}
