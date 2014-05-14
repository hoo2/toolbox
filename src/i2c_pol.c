/*!
 * \file i2c_pol.c
 * \brief
 *    A target independent i2c poling driver with ACK/NACK support.
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


#include <drv/i2c_pol.h>

/*
 * Link and glue functions
 */
/*!
 * \brief
 *    Link i2c's sda pointer to target SDA function.
 * \param  i2c    pointer to active i2c.
 * \param  sda    pointer to target's SDA function
 * \return none
 */
inline void i2c_link_sda(i2c_pol_t *i2c, i2c_pin_ft sda) {
   i2c->sda = sda;
}

/*!
 * \brief
 *    Link i2c's scl pointer to target SCL function.
 * \param  i2c    pointer to active i2c.
 * \param  scl    pointer to target's SCL function
 * \return none
 */
inline void i2c_link_scl (i2c_pol_t *i2c, i2c_pin_ft scl) {
   i2c->scl = scl;
}

/*!
 * \brief
 *    Link i2c's sda_dir pointer to target SDA direction function.
 * \param  i2c    pointer to active i2c.
 * \param  pd     pointer to target's SDA direction function
 * \return none
 */
inline void i2c_link_sdadir (i2c_pol_t *i2c, i2c_pindir_ft pd) {
   i2c->sda_dir = pd;
}

/*
 * Set functions
 */

/*!
 * \brief
 *    Set i2c's speed/frequency.
 * \param  i2c    pointer to active i2c.
 * \param  freq   desired freq
 * \return none
 * \note
 *    This call has immediate effect
 */
void i2c_set_speed (i2c_pol_t *i2c, int freq)
{
   i2c->clk_delay = 500000 / freq;
   /*!<
    *                                1
    * clock delay = 2 * period = -----------
    *                            2*frequency
    * So:
    *                 1 * 10^6      500000
    * delay (usec) = ----------- = ---------
    *                 2 * freq       freq
    */
}

/*
 * User functions
 */

/*!
 * \brief
 *    Check i2c status
 * \param  i2c    pointer to active i2c.
 * \return status
 */
inline drv_status_t i2c_probe (i2c_pol_t *i2c)
{
   return i2c->status;
}

/*!
 * \brief
 *    De-Initialize the i2c interface and leave sda pin in input state
 * \param  i2c    pointer to active i2c.
 * \return none
 */
void i2c_deinit (i2c_pol_t *i2c)
{
   // Free bus
   if (i2c->sda_dir) i2c->sda_dir (0);
   if (i2c->scl)     i2c->scl(1);
   // Clear data
   memset ((void*)i2c, 0, sizeof (i2c_pol_t));
   /*!<
    * This leaves the status DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialize the i2c interface and leave pins high
 * \param  i2c    pointer to active i2c.
 * \return Zero on success, non zero on error
 */
int i2c_init (i2c_pol_t *i2c)
{
   if (!i2c->sda_dir)   return 1;
   if (!i2c->sda)       return 1;
   if (!i2c->scl)       return 1;
   i2c->sda_dir (1);
   i2c->sda (1);
   i2c->scl (1);
   i2c->status = DRV_READY;
   return 0;
}

/*!
 * \brief
 *    Send a START bit to the bus
 * \param  i2c    pointer to active i2c.
 * \return Zero on success, non zero on error
 */
int i2c_start (i2c_pol_t *i2c)
{
   //Update status
   if (i2c->status != DRV_READY)
      return 1;
   i2c->status = DRV_BUSY;
   //Initially set pins
   i2c->sda_dir (1);
   i2c->sda (1);
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay);
   i2c->sda (0);
      jf_delay_us (i2c->clk_delay);
   i2c->scl (0);  //Clear Clock
   return 0;
}

/*!
 * \brief
 *    Send a STOP bit to the bus
 * \param  i2c    pointer to active i2c.
 * \return none
 */
void i2c_stop (i2c_pol_t *i2c)
{
   //Stop bit Operation
   i2c->sda_dir (1);
   i2c->sda (0);
   i2c->scl (0);
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay);
   i2c->sda (1);
      jf_delay_us (i2c->clk_delay);
   //Update status
   i2c->status = DRV_READY;
}

/*!
 * \brief
 *    Transmit a byte to the i2c bus.
 * \param  i2c    pointer to active i2c.
 * \param  byte   The byte to send.
 * \return Slave's ACK bit
 *    \arg 0 Slave didn't ACK
 *    \arg 1 Slave did ACK
 */
int i2c_tx(i2c_pol_t *i2c, uint8_t byte)
{
   uint8_t i = 8;
   int ack;

   //Initial conditions
   i2c->scl (0);

   //Send 8 bit data
   while(i--) {
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
 *    Receive a byte from the i2c bus.
 * \param  i2c    pointer to active i2c.
 * \param  ack    Optional ack bit.
 *    \arg 1  ACK the reception
 *    \arg 0  Don't ACK the reception.
 * \param  byte   The byte received.
 */
uint8_t i2c_rx(i2c_pol_t *i2c, uint8_t ack)
{
   uint8_t i = 8;
   uint8_t byte = 0;

   //Initial conditions
   i2c->scl (0);

   // read 8 data bits
   i2c->sda_dir (0);
   while(i--) {
      byte <<= 1;
      i2c->scl (1);
         jf_delay_us (i2c->clk_delay);
      byte |= i2c->sda (0);
      i2c->scl (0);
         jf_delay_us (i2c->clk_delay);
   }

   //Send or not ACK
   i2c->sda_dir (1);
   if (ack)       i2c->sda (0);  // ACK
   else           i2c->sda (1);  // Don't ACK
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay);
   i2c->scl (0);     // Keep the bus busy
      jf_delay_us (i2c->clk_delay);
   i2c->sda (0);
   return byte;
}
