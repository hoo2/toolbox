/*!
 * \file deque08.c
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
#include <cont/deque08.h>


/*
 *  ============= Private Queue API =============
 */
__O3__ static iterator_t _preInc(deque08_t *q, iterator_t* it) {
   return (++*it >= q->capacity) ? *it=0 : *it;
}

__O3__ static iterator_t _preDec(deque08_t *q, iterator_t* it) {
   return (--*it < 0) ? *it=q->capacity-1 : *it;
}

__O3__ static iterator_t _postInc(deque08_t *q, iterator_t* it) {
   iterator_t ret = *it;
   if (++*it >= q->capacity) *it=0;
   return ret;
}

__O3__ static iterator_t _postDec(deque08_t *q, iterator_t* it) {
   iterator_t ret = *it;
   if (--*it < 0) *it=q->capacity-1;
   return ret;
}

static bool _check_sizeTrigger (deque08_t *q) {
   switch (q->trigger.mode) {
      default:
      case DISABLED: return false;

      case MORE_EQ:
         if (q->items >= q->trigger.value.size) {
            q->trigger.callback();
            deque08_clear_trigger(q);
            return  true;
         }
         else
            return false;

      case LESS_EQ:
         if (q->items <= q->trigger.value.size) {
            q->trigger.callback();
            deque08_clear_trigger(q);
            return true;
         }
         else
            return false;
   }
}

static bool _check_valueTrigger (deque08_t *q, byte_t b) {
   switch (q->trigger.mode) {
      default:
      case DISABLED: return false;
      case EVERY_VALUE:
         if (b == (byte_t)q->trigger.value.content) {
            q->trigger.callback();
            return true;
         }
         else
            return false;
   }
}


/*
 *  ============= Public Queue API =============
 */

/*
 * Link and Glue functions
 */
__O3__ void deque08_link_buffer (deque08_t *q, byte_t* buf) {
   q->m = buf;
}

/*
 * Set functions
 */
__O3__ void deque08_set_capacity (deque08_t *q, size_t capacity) {
   q->capacity = capacity;
}

bool deque08_set_trigger (deque08_t *q, deque_callback_ft callback, trigger_mode_en mode, int value) {
   if (!isTriggerMode(mode))  return false;

   q->trigger.mode = mode;
   switch (q->trigger.mode) {
      default:
      case DISABLED: return false;
      case MORE_EQ:
      case LESS_EQ:     q->trigger.value.size    = (size_t)value; break;
      case EVERY_VALUE: q->trigger.value.content = (byte_t)value; break;
   }
   q->trigger.callback = callback;
   deque08_check_trigger(q);  // check for any event before the trigger setup.
   return true;
}

__O3__ void deque08_clear_trigger (deque08_t *q) {
   deque08_trigger_t t = {0};
   q->trigger = t;
}

/*
 * User Functions
 */


/*!
 * \brief
 *    Check if deque is full
 * \param   q     Which deque to check
 * \return
 *    \arg  0     Not full
 *    \arg  1     Full
 */
__O3__ int deque08_is_full (deque08_t *q) {
   return (q->items == q->capacity) ? 1:0;
}

/*!
 * \brief
 *    Check if deque is empty
 * \param   q     Which deque to check
 * \return
 *    \arg  0     Not empty
 *    \arg  1     Empty
 */
__O3__ int deque08_is_empty (deque08_t *q) {
   return (q->items) ? 0:1;
}

/*!
 * \brief
 *    Return the number of items on deque
 * \param   q     Which deque to check
 */
__O3__ int  deque08_size (deque08_t *q) {
   return q->items;
}

/*!
 * \brief
 *    Discard all items in deque
 * \param   q     Which deque to check
 */
__Os__ void  deque08_flush (deque08_t *q) {
   deque08_init(q);
}



/*!
 * \brief
 *    Initialize the queue
 * \param   queue    Which queue to init
 */
__Os__ void deque08_init (deque08_t *q) {
   q->f = 0;
   q->r = -1;
   q->items =0;
}

/*!
  * \brief
  *   This function push a byte in front of deque.
  * \param  q  Pointer to deque to use
  * \param  b  byte to push
  * \return
  *   \arg  0  Full queue
  *   \arg  1  Done
 */
int deque08_push_front (deque08_t *q, byte_t b) {
   if (deque08_is_full (q) == 1)  //full queue
      return 0;
   q->m [_preDec (q, &q->f)] = b;
   ++q->items;
   _check_valueTrigger(q, b);
   _check_sizeTrigger(q);
   return 1;
}

/*!
  * \brief
  *   This function pops a byte from the front of the deque.
  * \param  q  Pointer to deque to use
  * \param  b  Pointer to byte to return
  * \return
  *   \arg  0  Empty queue
  *   \arg  1  Done
 */
int deque08_pop_front (deque08_t *q, byte_t *b) {
   if (deque08_is_empty (q) == 1)  //empty queue
      return 0;
   *b = q->m [_postInc (q, &q->f)];
   --q->items;
   _check_sizeTrigger(q);
   return 1;
}

/*!
  * \brief
  *   This function push a byte in the back of deque.
  * \param  q  Pointer to deque to use
  * \param  b  byte to push
  * \return
  *   \arg  0  Full queue
  *   \arg  1  Done
 */
int deque08_push_back (deque08_t *q, byte_t b) {
   if (deque08_is_full (q) == 1)  //full queue
      return 0;
   q->m [_preInc (q, &q->r)] = b;
   ++q->items;
   _check_valueTrigger(q, b);
   _check_sizeTrigger(q);
   return 1;
}

/*!
  * \brief
  *   This function pops a byte from the back of the deque.
  * \param  q  Pointer to deque to use
  * \param  b  Pointer to byte to return
  * \return
  *   \arg  0  Empty queue
  *   \arg  1  Done
 */
int deque08_pop_back (deque08_t *q, byte_t *b) {
   if (deque08_is_empty (q) == 1)  //empty queue
      return 0;
   *b = q->m [_postDec (q, &q->r)];
   --q->items;
   _check_sizeTrigger(q);
   return 1;
}

/*!
  * \brief
  *   This function push an arbitrary number of bytes to the front of the deque.
  * \param  q     Pointer to deque to use
  * \param  num   number of bytes to push
  * \return
  *   \arg  0  Full queue
  *   \arg  1  Done
 */
int deque08_vpush_front (deque08_t *q, size_t num, ...) {
   va_list args;
   int ret;

   va_start(args, num);
   do
      ret = deque08_push_front(q, (byte_t)va_arg(args, int32_t));
   while (ret);
   va_end(args);

   return ret;
}

/*!
  * \brief
  *   This function push an arbitrary number of bytes to the back of the deque.
  * \param  q     Pointer to deque to use
  * \param  num   number of bytes to push
  * \return
  *   \arg  0  Full queue
  *   \arg  1  Done
 */
int deque08_vpush_back (deque08_t *q, size_t num, ...) {
   va_list args;
   int ret;

   va_start(args, num);
   do
      ret = deque08_push_back(q, (byte_t)va_arg(args, int32_t));
   while (ret);
   va_end(args);

   return ret;
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
int deque08_back (deque08_t *q, byte_t *b) {
   if (deque08_is_empty (q) == 1)  //empty queue
      return 0;
   *b = q->m [q->r];
   return 1;
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
int deque08_front (deque08_t *q, byte_t *b) {
   if (deque08_is_empty (q) == 1)  //empty queue
      return 0;
   *b = q->m [q->f];
   return 1;
}

bool deque08_check_trigger (deque08_t *q) {
   return _check_sizeTrigger(q);
}
