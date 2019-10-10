/*!
 * \file deque08.h
 * \brief
 *    This file provides double ended queue capability based on a ring buffer
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
#ifndef  __deque08_h__
#define  __deque08_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <toolbox_defs.h>
#include <string.h>

typedef struct {
   byte_t      *m;         /*!< pointer to queue's buffer */
   iterator_t  capacity;   /*!< queue's max item capacity */
   iterator_t  items;      /*!< current item count */
   iterator_t  f, r;       /*!< queue iterators */
}deque08_t;


/*
 *  ============= PUBLIC EE API =============
 */

/*
 * Link and Glue functions
 */
void deque08_link_buffer (deque08_t *q, byte_t* buf);


/*
 * Set functions
 */
void deque08_set_capacity (deque08_t *q, size_t capacity);

/*
 * User Functions
 */
int  deque08_is_full (deque08_t *q);
int  deque08_is_empty (deque08_t *q);
int  deque08_size  (deque08_t *q);
void deque08_flush (deque08_t *q);

void deque08_init (deque08_t *q);
int  deque08_push_front (deque08_t *q, byte_t b);
int  deque08_pop_front (deque08_t *q, byte_t *b);
int  deque08_push_back (deque08_t *q, byte_t b);
int  deque08_pop_back (deque08_t *q, byte_t *b);

#ifdef __cplusplus
}
#endif


#endif //#ifndef  __fifo_h__
