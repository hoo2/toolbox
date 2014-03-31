/*!
 * \file
 *    i2c_pol.c
 * \brief
 *
 * Copyright (C) 2013 Houtouridis Christos (http://houtouridis.blogspot.com/)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       07/2013
 * Version:    0.1
 */


#include <i2c_pol.h>

/*
 * Callback function input
 */
inline void i2c_setsda(i2c_pol_t *i2c, i2c_pin_ft sda) {
   i2c->sda = sda;
}

inline void i2c_setscl (i2c_pol_t *i2c, i2c_pin_ft scl) {
   i2c->scl = scl;
}

inline void i2c_setsdadir (i2c_pol_t *i2c, i2c_pindir_ft pd) {
   i2c->sda_dir = pd;
}


/*!
 * \brief
 *    De-Initialize the i2c interface and leave sda pin in input state
 * \param   i2c
 *    Pointer to i2c data structure.
 */
void i2c_deinit (i2c_pol_t *i2c)
{
   i2c->clk_delay = 0;
   i2c->sda_dir (0);
   //i2c->sda =(void*)0;
   //i2c->scl =(void*)0;
}

/*!
 * \brief
 *    Initialize the i2c interface and leave pins high
 * \param   i2c
 *    Pointer to i2c data structure.
 */
void i2c_init (i2c_pol_t *i2c, int speed)
{
   i2c->clk_delay = 500000 / speed;
   i2c->sda_dir (1);
   i2c->sda (1);
   i2c->scl (1);
}

/*!
 * \brief
 *    Send a START bit to the bus
 * \param   i2c
 *    Pointer to i2c data structure.
 */
void i2c_start (i2c_pol_t *i2c)
{
   //Initially set pins
   i2c->sda_dir (1);
   i2c->sda (1);
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay>>1);
   i2c->sda (0);
      jf_delay_us (i2c->clk_delay>>1);
   i2c->scl (0);  //Clear Clock
}

/*!
 * \brief
 *    Send a STOP bit to the bus
 * \param   i2c
 *    Pointer to i2c data structure.
 */
void i2c_stop (i2c_pol_t *i2c)
{
   //Stop bit Operation
   i2c->sda_dir (1);
   i2c->sda (0);
   i2c->scl (0);
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay>>1);
   i2c->sda (1);
      jf_delay_us (i2c->clk_delay>>1);
}

/*!
 * \brief
 *    Transmit a byte to the bus.
 * \param   i2c
 *    Pointer to i2c data structure.
 * \param   byte
 *    The byte to send.
 */
uint8_t i2c_tx(i2c_pol_t *i2c, uint8_t byte)
{
   uint8_t i = 8;
   uint8_t ack;

   //Initial conditions
   i2c->scl (0);

   //Send 8 bit data
   while(i--)
   {
      //Send MSB
      i2c->sda (byte & 0x80);
      byte <<= 1;
      i2c->scl (1);
         jf_delay_us (i2c->clk_delay);
      i2c->scl (0);
         jf_delay_us (i2c->clk_delay);
   }
   // Get ACK
   i2c->sda_dir (0);
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay);
   ack = !i2c->sda (0);
   i2c->scl (0);     // Keep the bus busy
      jf_delay_us (i2c->clk_delay);

   i2c->sda_dir (1);
   i2c->sda (0);
   return ack;
}

/*!
 * \brief
 *    Receive a byte from the bus.
 * \param   i2c
 *    Pointer to i2c data structure.
 * \param   ack   Optional ack bit.
 *    1 = ACK the reception
 *    0 = NACK the reception.
 * \param   byte
 *    The byte received.
 */
uint8_t i2c_rx(i2c_pol_t *i2c, uint8_t ack)   // assume SDA and SCL are low at entry
{
   uint8_t i = 8;
   uint8_t byte = 0;

   //Initial conditions
   i2c->scl (0);

   // read 8 data bits
   i2c->sda_dir (0);
   while(i--)
   {
      byte <<= 1;
      i2c->scl (1);
         jf_delay_us (i2c->clk_delay);
      byte |= i2c->sda (0);
      i2c->scl (0);
         jf_delay_us (i2c->clk_delay);
   }

   //Send ACK or NACK
   i2c->sda_dir (1);
   if (ack)       i2c->sda (0);  // ACK
   else           i2c->sda (1);  // NACK
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay);
   i2c->scl (0);     // Keep the bus busy
      jf_delay_us (i2c->clk_delay);

   i2c->sda (0);
   return byte;
}

