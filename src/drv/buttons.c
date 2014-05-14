/*!
 * \file buttons.c
 * \brief
 *    A target independent direct connect button driver
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
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

#include <drv/buttons.h>

btn_t   BTN;

//extern clock_t  volatile Ticks;

static btn_input_buffer_t     inbuf;

static keys_t _ib_put (keys_t k);
static keys_t _ib_get (void);
static int8_t _ib_capacity (void);
static keys_t _get_buttons (void);

/*!
  * \brief  This function puts a key to buffer.
  * \param  Key to be written
  * \retval BTN_NULL on error
*/
static keys_t _ib_put (keys_t k)
{
   int8_t   c = _ib_capacity();

   if ( c >= INPUT_BUFFER_SIZE ) //full queue
      return BTN_NULL;
   inbuf.ib[inbuf.rear] = (keys_t)k;
   //rotate pointer
   if ( ++inbuf.rear >= INPUT_BUFFER_SIZE )
      inbuf.rear = 0;
   return k;
}
/*!
  * \brief  This function gets a key from buffer.
  * \param  none
  * \retval Character from buffer, or BTN_NULL on empty buffer
*/
static keys_t _ib_get (void)
{
   int8_t   c = _ib_capacity();
   keys_t k;

   if ( c <= 0 )    //Empty queue
      return (keys_t)BTN_NULL;
   k = inbuf.ib[inbuf.front];
   //rotate pointers
   if ( ++inbuf.front >= INPUT_BUFFER_SIZE )
      inbuf.front = 0;
   return k;
}

/*!
  * \brief  Calculates the Input buffer capacity
  * \param  none
  * \retval keys from buffer, or BT_NULL on empty buffer
*/
static int8_t  _ib_capacity (void)
{
   if (inbuf.front == inbuf.rear)
      return 0;
   else if (inbuf.front > inbuf.rear)
      return ( INPUT_BUFFER_SIZE - (inbuf.front - inbuf.rear) );
   else
      return ( inbuf.rear - inbuf.front );
}

/*!
  * \brief
  *    Reads the back-end functions and combine them to synthesize the
  *    key value. Each pin corresponds to one bit in the key variable.
  *    for example:
  *    key: 0x0009 ==> BTN0 and BTN3 are pressed.
  * \param  none
  * \retval key value
*/
static keys_t _get_buttons (void)
{
   keys_t key=0;  // Clear key
   keys_t tmp;

   if (BTN.io.btn0)   key |= BTN.io.btn0();
   if (BTN.io.btn1)   key |= ((tmp = BTN.io.btn1()) << 1);
   if (BTN.io.btn2)   key |= ((tmp = BTN.io.btn2()) << 2);
   if (BTN.io.btn3)   key |= ((tmp = BTN.io.btn3()) << 3);
   if (BTN.io.btn4)   key |= ((tmp = BTN.io.btn4()) << 4);
   if (BTN.io.btn5)   key |= ((tmp = BTN.io.btn5()) << 5);
   if (BTN.io.btn6)   key |= ((tmp = BTN.io.btn6()) << 6);
   if (BTN.io.btn7)   key |= ((tmp = BTN.io.btn7()) << 7);
   if (BTN.io.btn8)   key |= ((tmp = BTN.io.btn8()) << 8);
   if (BTN.io.btn9)   key |= ((tmp = BTN.io.btn9()) << 9);
   if (BTN.io.btn10)  key |= ((tmp = BTN.io.btn10()) << 10);
   if (BTN.io.btn11)  key |= ((tmp = BTN.io.btn11()) << 11);
   if (BTN.io.btn12)  key |= ((tmp = BTN.io.btn12()) << 12);
   if (BTN.io.btn13)  key |= ((tmp = BTN.io.btn13()) << 13);
   if (BTN.io.btn14)  key |= ((tmp = BTN.io.btn14()) << 14);
   if (BTN.io.btn15)  key |= ((tmp = BTN.io.btn15()) << 15);
   return key;
}


/*
 * ============================ Public Functions ============================
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief
 *    Links a driver GetKey function to the corresponding pointer in io struct.
 * \param  sio,  The struct pointer to function
 * \param  pfun, The function from the driver.
 * \return none
*/
inline void btn_link (volatile btn_pin_t *sio, btn_pin_t pfun) {
   *sio = pfun;
}

/*
 * Set functions
 */
inline void btn_set_holdtime (clock_t holdtime) { BTN.holdtime = holdtime; }
inline void btn_set_reptime (clock_t reptime) { BTN.reptime = reptime; }
inline void btn_set_repetitive (uint8_t rep) { BTN.repetitive = rep; }

/*
 * User Functions
 */

/*!
  * \brief
  *    Returns the key to the caller.
  *    If we have bt_wait flag set, it waits for the user choice.
  *
  * \param  wait  Wait for key flag
  * \retval key pressed or -1(EOF) if none.
  */
keys_t btn_getkey (uint8_t wait)
{
   // wait for user's action
   while (wait && !_ib_capacity())
      ;
   return _ib_get ();
}

/*!
  * \brief  Flush input buffer
  *
  * \param  none
  * \retval none
  */
inline void btn_flush (void) {
   inbuf.front = inbuf.rear = 0;
}

/*!
  * \brief
  *    This function is the state machine for the Button functionality.
  *    Can be called from Interrupt or from a thread in while() loop.
  *    Detects keys and feeds them to Input Buffer.
  *    Call _ib_get() to read them.
  * \param  none
  * \retval none
  */
void btn_service (void)
{
   static clock_t mark = 0;
   static clock_t rep_mark = 0;
   static btn_state_t state;
   static uint8_t rep_flag = 0;
   static keys_t  key=0,
                  pr_key=0,
                  max_key=0,
                  bounce_bf[2] = {0,0};   // 2 state de-bounce

   clock_t now = clock ();

   // 2 state De-bounce
   bounce_bf[0] = bounce_bf[1];
   bounce_bf[1] = _get_buttons (); // Call the back-end to read the buttons

   if (bounce_bf[0] == bounce_bf[1])
      key = bounce_bf[1];
   else
      key = pr_key;

   /*!
    * \note
    * This state machine waits for key. filters it and put it to inbuf.
    * - For standard presses filters by finding the max value
    * - For long presses it uses the final (long pressed) value
    *
    * This number is the key returned to the user from Input Buffer ( _ib_get ())
    * So any combinations to the keys produces a different key number
    * See also \see _get_buttons ()
    */
   switch (state)
   {
      case BTN_IDLE:
         if (key) {
            state = BTN_PRE;
            mark = rep_mark = now;
         }
         break;
      case BTN_PRE: // Button(s) is/are pressed
         /*
          * Wait until user has the same buttons pressed
          * and count the time to repetitive and long pressed.
          */
         if (key != pr_key)
            mark = now;
         if (now - mark >= BTN.holdtime) {
            // Put Long keys
            state = BTN_LONG;
            _ib_put (key | BTN_LONG_PRE_MASK);
         }
         if (!key) {
            /*
             * Key released
             * \note Here we put the max_key NOT key
             */
            _ib_put (max_key);
            max_key = 0;
            rep_flag = 0;
            state = BTN_IDLE;
         }
         break;
      case BTN_LONG: // Button(s) is/are long pressed
         if (now - mark >= (2*BTN.holdtime))
            rep_flag = 1;
         if (!key) {
            // Long key released
            _ib_put (key | BTN_LONG_REL_MASK);
            max_key = 0;
            rep_flag = 0;
            state = BTN_IDLE;
         }
         break;
   }
   // Filter out any user attempts and hold only the final decision
   if (key > max_key)
      max_key = key;

   // Repetitive capability
   if (rep_flag && BTN.repetitive && (now - rep_mark >= BTN.reptime)) {
      _ib_put (key);
      rep_mark = now;
   }
   pr_key = key;
}


