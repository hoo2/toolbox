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

btn_t  BTN;
keys_t _ib [INPUT_BUFFER_SIZE];
queue_t btn_q;

static keys_t _get_buttons (void);

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
  *   Flush input buffer
  */
inline void btn_flush (void) {
   keys_t null;
   do
      queue_get (&btn_q, (void*)&null);
   while (!queue_is_empty (&btn_q));
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

   keys_t key_long;

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
            key_long = key | BTN_LONG_PRE_MASK;
            queue_put (&btn_q, (void*)&key_long);
         }
         if (!key) {
            /*
             * Key released
             * \note Here we put the max_key NOT key
             */
            queue_put (&btn_q, &max_key);
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
            key_long = key | BTN_LONG_REL_MASK;
            queue_put (&btn_q, (void*)&key_long);
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
      queue_put (&btn_q, &key);
      rep_mark = now;
   }
   pr_key = key;
}

/*!
  * \brief
  *    De-Initialize button interface
  */
void btn_deinit (void)
{
   memset ((void*)&btn_q, 0, sizeof (queue_t));
}

/*!
  * \brief
  *    Initialize button interface
  */
drv_status_en btn_init (void)
{
   queue_link_buffer (&btn_q, (void*)_ib);
   queue_set_items (&btn_q, INPUT_BUFFER_SIZE);
   queue_set_item_size (&btn_q, sizeof (keys_t));
   return DRV_READY;
}

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
   keys_t ret=BTN_NULL;

   // wait for user's action
   while (wait && queue_is_empty (&btn_q))
      ;
   queue_get (&btn_q, (void*)&ret);
   return ret;
}

/*!
 * \brief
 *    buttons ioctl function
 *
 * \param  cmd   specifies the command to FLASH
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_FLUSH
 * \param  buf   pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en btn_ioctl (ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   switch (cmd)
   {
      case CTRL_DEINIT:          /*!< De-init */
        btn_deinit();
        return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = btn_init();
         else
            btn_init();
         return DRV_READY;
      case CTRL_GET_STATUS:            /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = BTN.status;
         return BTN.status = DRV_READY;
      case CTRL_FLUSH:
         btn_flush ();
         return BTN.status = DRV_READY;
      default:                         /*!< Unsupported command, error */
         return DRV_ERROR;
   }
}
