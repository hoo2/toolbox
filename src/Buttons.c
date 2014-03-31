/*
 * BTN16.c
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Houtouridis Christos. The intellectual
 * and technical concepts contained herein are proprietary to
 * Houtouridis Christos and are protected by copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Houtouridis Christos.
 *
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       06/2013
 * Version:
 *
 */

#include <Buttons.h>

BTN_t   BTN;

extern clock_t  volatile Ticks;

static BTN_input_buffer_t     inbuf;

static keys_t BTN_IB_Put (keys_t k);
static keys_t BTN_IB_Get (void);
static int8_t BTN_IB_Capacity (void);
static keys_t BTN_GetButtons (void);

/*!
  * \brief  This function puts a key to buffer.
  * \param  Key to be written
  * \retval BTN_NULL on error
*/
static keys_t BTN_IB_Put (keys_t k)
{
   int8_t   c = BTN_IB_Capacity();

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
static keys_t BTN_IB_Get (void)
{
   int8_t   c = BTN_IB_Capacity();
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
static int8_t  BTN_IB_Capacity (void)
{
   if (inbuf.front == inbuf.rear)
      return 0;
   else if (inbuf.front > inbuf.rear)
      return ( INPUT_BUFFER_SIZE - (inbuf.front - inbuf.rear) );
   else
      return ( inbuf.rear - inbuf.front );
}

/*!
  * \brief  Reads the back-end functions and combine them
  *         to synthesize the key value. Each pin corresponds
  *         to one bit in the key variable.
  *         for example:
  *         key: 0x0009 ==> BTN0 and BTN3 are pressed.
  *
  * \param  none
  * \retval key value
*/
static keys_t BTN_GetButtons (void)
{
   keys_t key=0;  // Clear key
   keys_t tmp;

   if (BTN.IO.BT0)   key |= BTN.IO.BT0();
   if (BTN.IO.BT1)   key |= ((tmp = BTN.IO.BT1()) << 1);
   if (BTN.IO.BT2)   key |= ((tmp = BTN.IO.BT2()) << 2);
   if (BTN.IO.BT3)   key |= ((tmp = BTN.IO.BT3()) << 3);
   if (BTN.IO.BT4)   key |= ((tmp = BTN.IO.BT4()) << 4);
   if (BTN.IO.BT5)   key |= ((tmp = BTN.IO.BT5()) << 5);
   if (BTN.IO.BT6)   key |= ((tmp = BTN.IO.BT6()) << 6);
   if (BTN.IO.BT7)   key |= ((tmp = BTN.IO.BT7()) << 7);
   if (BTN.IO.BT8)   key |= ((tmp = BTN.IO.BT8()) << 8);
   if (BTN.IO.BT9)   key |= ((tmp = BTN.IO.BT9()) << 9);
   if (BTN.IO.BT10)  key |= ((tmp = BTN.IO.BT10()) << 10);
   if (BTN.IO.BT11)  key |= ((tmp = BTN.IO.BT11()) << 11);
   if (BTN.IO.BT12)  key |= ((tmp = BTN.IO.BT12()) << 12);
   if (BTN.IO.BT13)  key |= ((tmp = BTN.IO.BT13()) << 13);
   if (BTN.IO.BT14)  key |= ((tmp = BTN.IO.BT14()) << 14);
   if (BTN.IO.BT15)  key |= ((tmp = BTN.IO.BT15()) << 15);
   return key;
}


/*
 * ============================ Public Functions ============================
 */

/*!
  * \brief  Connects a GetKey function to the corresponding pointer in
  *         IO structure
  * \param  sio,  The struct pointer to function
  * \param  pfun, The function from the driver.
  * \retval 0 on error, none zero on success.
*/
int BTN_Connect (volatile BTN_Pin_t *sio, BTN_Pin_t pfun)
{
   if (!pfun)
      return 0;
   *sio = pfun;
   return 1;
}

/*!
  * \brief  Returns the key to the caller.
  *         If we have bt_wait flag set, it waits for the user choice.
  *
  * \param  wait  Wait for key flag
  * \retval key pressed or -1(EOF) if none.
  */
keys_t BTN_Getkey (uint8_t wait)
{
   // wait for user's action
   while (wait && !BTN_IB_Capacity())
      ;
   return BTN_IB_Get ();
}

/*!
  * \brief  Flush input buffer
  *
  * \param  none
  * \retval none
  */
inline void BTN_Flush (void) {
   inbuf.front = inbuf.rear = 0;
}

/*!
  * \brief  This function is the state machine for the Button functionality
  *         Can be called from Interrupt or from a thread in while() loop.
  *
  *         Detects keys and feeds them to Input Buffer.
  *         Call BTN_IB_Get() to read them.
  *
  * \param  none
  * \retval none
  */
void BTN_Service (void)
{
   static clock_t mark = 0;
   static clock_t rep_mark = 0;
   static keys_t key=0, pr_key=0, max_key=0, bounce_bf[2]={0,0};
   static BTN_State_t state;
   static uint8_t rep_flag = 0;

   pr_key = key;

   // De-bounce
   bounce_bf[0] = bounce_bf[1];
   bounce_bf[1] = BTN_GetButtons(); // Call the back-end to read the buttons

   if (bounce_bf[0] == bounce_bf[1])
      key = bounce_bf[1];
   else
      key = pr_key;

   /*!
    * This state machine waits for key. filters it and put it to inbuf.
    * - For standard presses filters by finding the max value
    * - For long presses it uses the final (long pressed) value
    *
    * This number is the key returned to the user from Input Buffer ( BTN_IB_Get ())
    * So any combinations to the keys produces a different key number
    * See also \see BTN_GetButtons()
    */
   switch (state)
   {
      case BTN_IDLE:
         if (key)
         {
            state = BTN_PRE;
            mark = rep_mark = Ticks;
         }
         break;
      case BTN_PRE: // Button(s) is/are pressed
         /*
          * Wait until user has the same buttons pressed
          * and count the time to repetitive and long pressed.
          */
         if (key != pr_key)
            mark = Ticks;
         if (Ticks - mark >= BTN.holdtime)   // Put Long keys
         {
            state = BTN_LONG;
            BTN_IB_Put (key | BTN_LONG_PRE_MASK);
         }
         if (!key)   // Key released
         {
            // Note: Here we put the max_key NOT key
            BTN_IB_Put (max_key);
            max_key = 0;
            rep_flag = 0;
            state = BTN_IDLE;
         }
         break;
      case BTN_LONG: // Button(s) is/are long pressed
         if (Ticks - mark >= (2*BTN.holdtime))
            rep_flag = 1;
         if (!key)   // Long key released
         {
            BTN_IB_Put (key | BTN_LONG_REL_MASK);
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
   if (rep_flag && BTN.repetitive && (Ticks - rep_mark >= BTN.reptime))
   {
      BTN_IB_Put (key);
      rep_mark = Ticks;
   }

}


