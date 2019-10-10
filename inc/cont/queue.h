/*
 * \file queue.h
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
#ifndef  __queue_h__
#define  __queue_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <toolbox_defs.h>
#include <string.h>

typedef struct {
   byte_t   *buf;             /*!< pointer to queue's buffer */
   int      items;            /*!< queue's max item capacity */
   int      item_size;        /*!< each item size */
   int      head, tail;       /*!< queue item counters/pointers */
}queue_t;
/*!<
 * \note
 *    We implement the queue based on a byte_t pointer. (a.k.a. char).
 *    The API (put/get) pointer is a void* and so we need to know the item's
 *    size, in order to do pointer arithmetic.
 */

/*
 *  ============= PUBLIC EE API =============
 */

/*
 * Link and Glue functions
 */
void queue_link_buffer (queue_t *q, void* buf);


/*
 * Set functions
 */
void queue_set_item_size (queue_t *q, int size);
void queue_set_items (queue_t *q, int items);

/*
 * User Functions
 */
int  queue_is_full (queue_t *q);
int queue_is_empty (queue_t *q);
int  queue_waiting (queue_t *q);
void   queue_flush (queue_t *q);

void  queue_init (queue_t *q);
int    queue_put (queue_t *q, void *b);
int    queue_get (queue_t *q, void *b);
int    queue_top (queue_t *q, void *b);
void* queue_head (queue_t *q);
void* queue_tail (queue_t *q);

#ifdef __cplusplus
}
#endif


#endif //#ifndef  __fifo_h__
