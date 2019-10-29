/*!
 * \file queue08.h
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
#ifndef  __queue08_h__
#define  __queue08_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <toolbox_defs.h>
#include <string.h>

#include <cont/deque08.h>

typedef deque08_t    queue08_t;


/*
 *  ============= PUBLIC EE API =============
 */

/*
 * Link and Glue functions
 */
void queue08_link_buffer (queue08_t *q, byte_t* buf);


/*
 * Set functions
 */
void queue08_set_capacity (queue08_t *q, size_t capacity);

/*
 * User Functions
 */
int  queue08_is_full (queue08_t *q);
int  queue08_is_empty (queue08_t *q);
int  queue08_size  (queue08_t *q);
void queue08_flush (queue08_t *q);

void queue08_init (queue08_t *q);
int  queue08_push (queue08_t *q, byte_t b);
int  queue08_pop (queue08_t *q, byte_t *b);


#ifdef __cplusplus
}
#endif


#endif //#ifndef  __queue08_h__
