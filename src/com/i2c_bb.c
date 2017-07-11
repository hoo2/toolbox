/*!
 * \file i2c_bb.c
 * \brief
 *    A target independent i2c using bit-banging driver with ACK/NACK support.
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


#include <com/i2c_bb.h>

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
inline void i2c_link_sda(i2c_bb_t *i2c, drv_pinio_ft sda) {
   i2c->sda = (drv_pinio_ft)(sda != 0) ? sda : 0;
}

/*!
 * \brief
 *    Link i2c's scl pointer to target SCL function.
 * \param  i2c    pointer to active i2c.
 * \param  scl    pointer to target's SCL function
 * \return none
 */
inline void i2c_link_scl (i2c_bb_t *i2c, drv_pinout_ft scl) {
   i2c->scl = (drv_pinout_ft)(scl != 0) ? scl : 0;
}

/*!
 * \brief
 *    Link i2c's sda_dir pointer to target SDA direction function.
 * \param  i2c    pointer to active i2c.
 * \param  pd     pointer to target's SDA direction function
 * \return none
 */
inline void i2c_link_sdadir (i2c_bb_t *i2c, drv_pindir_ft pd) {
   i2c->sda_dir = (drv_pindir_ft)(pd != 0) ? pd : 0;
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
void i2c_set_speed (i2c_bb_t *i2c, uint32_t freq)
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
 *    De-Initialize the i2c interface and leave sda pin in input state
 * \param  i2c    pointer to active i2c.
 * \return none
 */
void i2c_deinit (i2c_bb_t *i2c)
{
   // Free bus
   if (i2c->sda_dir) i2c->sda_dir (0);
   if (i2c->scl)     i2c->scl(1);

   // Clear data
   memset ((void*)i2c, 0, sizeof (i2c_bb_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialise the i2c interface and leave pins high
 * \param  i2c    pointer to active i2c.
 * \return The driver status after init.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en i2c_init (i2c_bb_t *i2c)
{
   // Check requirements
   if (!i2c->sda_dir)   return i2c->status = DRV_ERROR;
   if (!i2c->sda)       return i2c->status = DRV_ERROR;
   if (!i2c->scl)       return i2c->status = DRV_ERROR;
   if (jf_probe () != DRV_READY)
      return i2c->status = DRV_ERROR;

   // Init the bus
   i2c->status = DRV_BUSY;

   if (!i2c->clk_delay)    i2c->clk_delay = 500000 / I2C_FREQ_DEF;
   i2c->sda_dir (1);
   i2c->sda (1);
   i2c->scl (1);
   return i2c->status = DRV_READY;
}

/*!
 * \brief
 *    Send a START bit to the bus
 * \param  i2c    pointer to active i2c.
 * \return none
 */
void i2c_start (i2c_bb_t *i2c)
{
   //Initially set pins
   i2c->sda_dir (1);
   i2c->sda (1);
      jf_delay_us (i2c->clk_delay);
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay);
   i2c->sda (0);
      jf_delay_us (i2c->clk_delay);
   i2c->scl (0);  //Clear Clock
}

/*!
 * \brief
 *    Send a STOP bit to the bus
 * \param  i2c    pointer to active i2c.
 * \return none
 */
void i2c_stop (i2c_bb_t *i2c)
{
   //Stop bit Operation
   i2c->sda_dir (1);
   i2c->sda (0);
   i2c->scl (0);
   i2c->scl (1);
      jf_delay_us (i2c->clk_delay);
   i2c->sda (1);
      jf_delay_us (i2c->clk_delay);
}

/*!
 * \brief
 *    Receive a byte from the i2c bus.
 * \param  i2c    pointer to active i2c.
 * \param  ack    Optional ack bit.
 *    \arg 1  ACK the reception
 *    \arg 0  Don't ACK the reception.
 * \param   seq   The operation sequence to execute
 *    \arg  I2C_RXSEQ_BYTE       Receive only the byte, do not send ack clock
 *    \arg  I2C_RXSEQ_ACK        Send only the ack bit
 *    \arg  I2C_RXSEQ_BYTE_ACK   Receive the byte and send the ack bit
 * \return   The byte received.
 */
byte_t i2c_rx (i2c_bb_t *i2c, uint8_t ack, i2c_bb_seq_en seq)
{
   uint8_t b, a;
   byte_t byte = 0;

   switch (seq) {
      default:                b=0;  a=0;  break;
      case I2C_SEQ_BYTE:      b=1;  a=0;  break;
      case I2C_SEQ_ACK:       b=0;  a=1;  break;
      case I2C_SEQ_BYTE_ACK:  b=1;  a=1;  break;
   }
   //Initial conditions
   i2c->scl (0);
   i2c->sda (0);  // Clear output port register

   if (b != 0) {
      // read 8 data bits
      i2c->sda_dir (0);
      for (int i=0 ; i<8 ; ++i) {
         byte <<= 1;
         i2c->scl (1);
            jf_delay_us (i2c->clk_delay);
         byte |= i2c->sda (0);
         i2c->scl (0);
            jf_delay_us (i2c->clk_delay);
      }
   }
   if (a != 0) {
      //Send or not ACK
      i2c->sda_dir (1);
      if (ack)       i2c->sda (0);  // ACK
      else           i2c->sda (1);  // Don't ACK
      i2c->scl (1);
         jf_delay_us (i2c->clk_delay);
      i2c->scl (0);     // Keep the bus busy
         jf_delay_us (i2c->clk_delay);
      i2c->sda (0);
   }
   return byte;
}

/*!
 * \brief
 *    Transmit a byte to the i2c bus.
 * \param  i2c    pointer to active i2c.
 * \param  byte   The byte to send.
 * \param   seq   The operation sequence to execute
 *    \arg  I2C_TXSEQ_BYTE       Transmit only the byte, do not read ack bit
 *    \arg  I2C_TXSEQ_ACK        Read only the ack bit
 *    \arg  I2C_TXSEQ_BYTE_ACK   Transmit the byte and read the ack bit
 * \return Slave's ACK bit
 *    \arg 0 Slave didn't ACK
 *    \arg 1 Slave did ACK
 */
int i2c_tx (i2c_bb_t *i2c, byte_t byte, i2c_bb_seq_en seq)
{
   uint8_t b, a;
   int ack = 0;

   switch (seq) {
      default:                b=0;  a=0;  break;
      case I2C_SEQ_BYTE:      b=1;  a=0;  break;
      case I2C_SEQ_ACK:       b=0;  a=1;  break;
      case I2C_SEQ_BYTE_ACK:  b=1;  a=1;  break;
   }

   //Initial conditions
   i2c->scl (0);
   if (b != 0) {
      //Send 8 bit data
      for (int i=0 ; i<8 ; ++i) {
         //Send MSB
         i2c->sda (byte & 0x80);
         byte <<= 1;
         i2c->scl (1);
            jf_delay_us (i2c->clk_delay);
         i2c->scl (0);
            jf_delay_us (i2c->clk_delay);
      }
      i2c->sda (0);     // Clear output port register
   }
   if (a != 0) {
      // Get ACK
      i2c->sda_dir (0);
      i2c->scl (1);
         jf_delay_us (i2c->clk_delay);
      ack = !i2c->sda (0);
      i2c->scl (0);     // Keep the bus busy
      i2c->sda_dir (1);
      i2c->sda (0);
         jf_delay_us (i2c->clk_delay);
   }
   return ack;
}

/*!
 * \brief
 *    i2c ioctl function
 *
 * \param  i2c    The active see struct.
 * \param  cmd    specifies the command to i2c and get back the replay.
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_START
 *    \arg CTRL_STOP
 * \param  buf    pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en i2c_ioctl (i2c_bb_t *i2c, ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   switch (cmd)
   {
      case CTRL_GET_STATUS:      /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = i2c->status;
         return DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
         i2c_deinit(i2c);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = i2c_init(i2c);
         else
            i2c_init(i2c);
         return DRV_READY;
      case CTRL_START:
         i2c_start (i2c);
         return DRV_READY;
      case CTRL_STOP:
         i2c_stop (i2c);
         return DRV_READY;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}
