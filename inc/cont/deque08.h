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
#include <stdbool.h>

#include <stdarg.h>

typedef void (*deque_callback_ft)(void);

typedef enum {
   DISABLED =0,   //!< Disable the trigger
   MORE_EQ,       //!< Trigger ONCE when the size is more or equal to the value
   LESS_EQ,       //!< Trigger ONCE when the size is less or equal to the value
   EVERY_VALUE    //!< Trigger EVERY time a specific item arrives on queue.

   //!^ Keep DISABLED equal zero "0" so setting the entire trigger struct
   //! to 0 is equivalent of clearing the trigger.
} trigger_mode_en;

typedef union {
   size_t         size;
   byte_t         content;
}trigger_value_t;

typedef struct {
   deque_callback_ft callback;
   trigger_mode_en   mode;
   trigger_value_t   value;
}deque08_trigger_t;


typedef struct {
   byte_t      *m;         /*!< pointer to queue's buffer */
   iterator_t  capacity;   /*!< queue's max item capacity */
   iterator_t  items;      /*!< current item count */
   iterator_t  f, r;       /*!< queue iterators */
   deque08_trigger_t
               trigger;

}deque08_t;



/*
 *  ============= PUBLIC EE API =============
 */
//Assertion like macros
#define isTriggerMode(m)   (((m)==DISABLED) || ((m)==MORE_EQ) || ((m)==LESS_EQ) || ((m)==EVERY_VALUE) )

/*
 * Link and Glue functions
 */
void deque08_link_buffer (deque08_t *q, byte_t* buf);

/*
 * Set functions
 */
void deque08_set_capacity (deque08_t *q, size_t capacity);
bool deque08_set_trigger (deque08_t *q, deque_callback_ft callback, trigger_mode_en mode, int value);
void deque08_clear_trigger (deque08_t *q);

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

int  deque08_vpush_front (deque08_t *q, size_t num, ...);
int  deque08_vpush_back (deque08_t *q, size_t num, ...);

int  deque08_back (deque08_t *q, byte_t *b);
int  deque08_front (deque08_t *q, byte_t *b);

bool deque08_check_trigger (deque08_t *q);

#ifdef __cplusplus
}
#endif


#endif //#ifndef  __deque08_h__
