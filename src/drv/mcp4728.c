/*!
 * \file mcp4728.c
 * \brief
 *    A target independent MCP4728 I2C DAC driver
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2017 Houtouridis Christos (http://www.houtouridis.net)
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

#include <drv/mcp4728.h>

/*
 * ------------ Static API ------------------
 */
static drv_status_en _sda_release (mcp4728_t *mcp);   //!< SDA release BUG workaround

static drv_status_en _wait_busy (mcp4728_t *mcp);
static drv_status_en _send_control (mcp4728_t *mcp, uint8_t rd, uint8_t bsy);
static drv_status_en _send_gen_call (mcp4728_t *mcp, uint8_t bsy);

static drv_status_en _gc_reset (mcp4728_t *mcp);
static drv_status_en _gc_wakeup (mcp4728_t *mcp);
static drv_status_en _gc_soft_update (mcp4728_t *mcp);
static drv_status_en _gc_read_address (mcp4728_t *mcp);

static drv_status_en _cmd_fast_write (mcp4728_t *mcp, int iter);
static drv_status_en _cmd_seq_write (mcp4728_t *mcp, mcp4728_channel_en from);
static drv_status_en _cmd_single_write (mcp4728_t *mcp, mcp4728_channel_en ch);
static drv_status_en _cmd_write_add (mcp4728_t *mcp);

/*!
 * brief
 *    Make sure the SDA bus pin is released after a STOP bit
 * \warning
 *    For some reasons the MCP4728 chip refuses to release SDA pin
 *    after a STOP. A workaround is to repeatedly send STOP to the
 *    bus until it does. Testing shows that it needs 3-5 STOPs to
 *    come to its senses.
 *
 * \param   mcp   Pointer indicate the mcp data stuct to use
 * \return  The status of the operation
 *    \arg  DRV_ERROR   Fail to release SDA
 *    \arg  DRV_READY   SDA released
 */
static drv_status_en _sda_release (mcp4728_t *mcp)
{
   // Check SDA status before begin to send STOPs
   mcp->io.i2c->sda_dir (0);
   for (int i=0 ; ! mcp->io.i2c->sda (0) ; ) {
      mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
      mcp->io.i2c->sda_dir (0);
      if (++i > MCP4728_SDA_RELEASE_TRIES) {
         return DRV_ERROR;
      }
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Wait busy pin, if its pointer is available. If no busy
 *    pin pointer available, return DRV_READY
 *
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \return
 *    \arg DRV_BUSY
 *    \arg DVR_READY
 */
static drv_status_en _wait_busy (mcp4728_t *mcp)
{
   // If instruct so, check busy pin (if available)
   if ( mcp->io.bsy == 0)
      return DRV_READY;

   // Check busy pin in a loop with configured timeout
   for (int i=0 ; mcp->io.bsy () ; ) {
      jf_delay_us (10);
      if (++i >= mcp->conf.timeout * 100)
         return DRV_BUSY;
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Send control byte. When requested, if LLD's BSY function
 *    is available then its used to determine the MCP status.
 * \note
 *    If any error occurred, sends STOP to release the bus before return
 *
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  rd     Read flag
 *    \arg  MCP4728_READ      (1)
 *    \arg  MCP4728_WRITE     (0)
 * \param  bsy    Busy request check flag
 *    \arg  0     Do not check busy pin
 *    \arg  1     Check busy pin if available
 * \return
 *    \arg DRV_BUSY
 *    \arg DRV_ERROR    (slave doesn't acknowledge)
 *    \arg DVR_READY
 */
static drv_status_en _send_control (mcp4728_t *mcp, uint8_t rd, uint8_t bsy)
{
   // Cast rd to [0, 1]
   rd = (rd) ? MCP4728_READ : MCP4728_WRITE;

   // If instruct so, check busy pin
   if (bsy && (_wait_busy (mcp) != DRV_READY))
      return DRV_BUSY;

   // Send start
   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_START, (void*)0);
   // Send control and check ack bit
   if (!mcp->io.i2c_tx (mcp->io.i2c, ((MCP4728_ADDRESS_MASK | (mcp->conf.cur_addr << 1)) | rd), I2C_SEQ_BYTE_ACK)) {
      mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Send General call. When requested, if LLD's BSY function
 *    is available then its used to determine the MCP status.
 * \note
 *    If any error occurred, sends STOP to release the bus before return
 *
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  bsy    Busy request check flag
 *    \arg  0     Do not check busy pin
 *    \arg  1     Check busy pin if available
 * \return
 *    \arg DRV_BUSY
 *    \arg DRV_ERROR    (slave doesn't acknowledge)
 *    \arg DVR_READY
 */
static drv_status_en _send_gen_call (mcp4728_t *mcp, uint8_t bsy)
{
   // If instruct so, check busy pin
   if (bsy && (_wait_busy (mcp) != DRV_READY))
      return DRV_BUSY;

   // Send start
   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_START, (void*)0);
   // Send zero control for general call and check ack bit
   if (!mcp->io.i2c_tx (mcp->io.i2c, 0, I2C_SEQ_BYTE_ACK)) {
      mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }
   return DRV_READY;

}

/*!
 * \brief
 *    Send a General Call Reset
 * \param  mcp   Pointer indicate the mcp data stuct to use
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _gc_reset (mcp4728_t *mcp)
{
   drv_status_en ret = DRV_READY;   // have faith

   // Send general call as control
   if ((ret = _send_gen_call (mcp, 0)) != DRV_READY)
      return ret;
   // Send RESET command
   if (!mcp->io.i2c_tx (mcp->io.i2c, MCP4728_GEN_RESET, I2C_SEQ_BYTE_ACK))
      ret = DRV_ERROR;

   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
   return ret;
}

/*!
 * \brief
 *    Send a General Call Wake-Up
 * \param  mcp   Pointer indicate the mcp data stuct to use
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _gc_wakeup (mcp4728_t *mcp)
{
   drv_status_en ret;

   // Send general call as control
   if ((ret = _send_gen_call (mcp, 0)) != DRV_READY)
      return ret;

   // Send WAKE-UP command
   if (!mcp->io.i2c_tx (mcp->io.i2c, MCP4728_GEN_WAKE_UP, I2C_SEQ_BYTE_ACK)) {
      mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
      return DRV_ERROR;
   }
   mcp->conf.pwr[0] = mcp->conf.pwr[1] = mcp->conf.pwr[2] = mcp->conf.pwr[3] = MCP4728_PD_Normal;
   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
   return DRV_READY;
}


/*!
 * \brief
 *    Send a General Call Software update
 * \param  mcp   Pointer indicate the mcp data stuct to use
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _gc_soft_update (mcp4728_t *mcp)
{
   drv_status_en ret = DRV_READY;   // have faith

   // Send general call as control
   if ((ret = _send_gen_call (mcp, 0)) != DRV_READY)
      return ret;

   // Send SOFT UPDATE command
   if (!mcp->io.i2c_tx (mcp->io.i2c, MCP4728_GEN_SOFT_UPDATE, I2C_SEQ_BYTE_ACK))
      ret = DRV_ERROR;

   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
   return ret;
}


/*!
 * \brief
 *    Read device address functionality using general call
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _gc_read_address (mcp4728_t *mcp)
{
   byte_t b, ea, da;

   // Send general call as control
   if (_send_gen_call (mcp, 0) != DRV_READY)
      return DRV_BUSY;

   // Send READ ADDRESS command without ack bit
   mcp->io.i2c_tx (mcp->io.i2c, MCP4728_GEN_READ_ADD, I2C_SEQ_BYTE);

   // Select chip via ldac and read ack bit
   jf_delay_us (10);                                           // Wait a moment
   mcp->io.ldac (1);                                           // select chip
   if (!mcp->io.i2c_tx (mcp->io.i2c, MCP4728_GEN_READ_ADD, I2C_SEQ_ACK)) {    // Transmit ack clock
      mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, (void*)0);    // STOP before die
      mcp->io.ldac (0);                                        // Clear ldac before die
      return DRV_ERROR;
   }

   // Send a restart to the bus
   if (_send_control (mcp, MCP4728_READ, 0) != DRV_READY) {
      mcp->io.ldac (0);                                        // Clear ldac before die
      return DRV_ERROR;
   }

   b = mcp->io.i2c_rx (mcp->io.i2c, 1, I2C_SEQ_BYTE_ACK);      // Receive with ack
   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);           // STOP

   // Make sure the bus released before exit
   if (_sda_release (mcp) != DRV_READY) {
      mcp->io.ldac (0);
      return DRV_ERROR;
   }
   mcp->io.ldac (0);                                           // Release LDAC now

   // Address validation
   if ((b & MCP4728_GEN_RA_VALID_MASK) == MCP4728_GEN_RA_VALID_PATTERN) {
      /*
       *  Has to be in the form bellow
       *  [A2 A1 A0  1 A2 A1 A0  0]
       */
      ea = (b & MCP4728_GEN_RA_EEPROM_MASK) >> 5;  // Extract eeprom address
      da = (b & MCP4728_GEN_RA_DACREG_MASK) >> 1;  // Extract data register address
      if (ea == da) {
         // Update address if valid
         mcp->conf.cur_addr = ea;
         return DRV_READY;
      }
     else
        return DRV_ERROR;
   }
   else
      return DRV_ERROR;

   return DRV_READY;
}

/*!
 * \brief
 *    Send a fast write command
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  iter   How many channels to send (staring from ch A)
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _cmd_fast_write (mcp4728_t *mcp, int iter)
{
   int i;
   word_t   w[4] = {0, 0, 0, 0};
   drv_status_en ret;

   _SATURATE (iter, 4, 1);

   // Prepare data
   for (i=0 ; i<iter ; ++i) {
      //w[i] |= MCP4728_FAST_WRITE;
      w[i] = mcp->conf.pwr[i] << 12;
      w[i] |= mcp->vout[i];
   }

   // Send control byte
   if ((ret = _send_control (mcp, MCP4728_WRITE, 1)) != DRV_READY)
      return ret;

   // Send channels
   for (i=0 ; i<iter ; ++i) {
      mcp->io.i2c_tx (mcp->io.i2c, (uint8_t)(w[i]>>8), I2C_SEQ_BYTE_ACK);
      if (!mcp->io.i2c_tx (mcp->io.i2c, (uint8_t)(w[i] && 0x00FF), I2C_SEQ_BYTE_ACK)) {
         mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
         return DRV_ERROR;
      }
   }
   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
   return DRV_READY;
}

/*!
 * \brief
 *    Send a Sequential write command with update request
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  from   From witch channel to start
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _cmd_seq_write (mcp4728_t *mcp, mcp4728_channel_en from)
{
   int i;
   byte_t  b = 0;
   word_t  w[4] = {0, 0, 0, 0};
   drv_status_en ret;

   if (from < MCP4728_CH_A || from > MCP4728_CH_D)
      return DRV_ERROR;

   // Send control byte
   if ((ret = _send_control (mcp, MCP4728_WRITE, 1)) != DRV_READY)
      return ret;

   // Data preparation and send
   b = MCP4728_SEQ_WRITE;
   b |= from << 1;
   b |= MCP4728_UDAC_UPDATE;
   mcp->io.i2c_tx (mcp->io.i2c, b, I2C_SEQ_BYTE_ACK);

   for (i=from ; i<=MCP4728_CH_D ; ++i) {
      // Fill registers
      w[i] |= mcp->conf.vref [i]  << 15;
      w[i] |= mcp->conf.pwr [i]   << 13;
      w[i] |= mcp->conf.gain [i]  << 12;
      w[i] |= (mcp->vout[i] & 0x0FFF);

      // Send current data
      mcp->io.i2c_tx (mcp->io.i2c, (uint8_t)(w[i]>>8), I2C_SEQ_BYTE_ACK);
      if (!mcp->io.i2c_tx (mcp->io.i2c, (uint8_t)(w[i] && 0x00FF), I2C_SEQ_BYTE_ACK)) {
         mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, (void*)0);
         return DRV_ERROR;
      }
   }
   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, (void*)0);
   return DRV_READY;
}

/*!
 * \brief
 *    Send a single write command with update request
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  ch    Witch channel to send
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _cmd_single_write (mcp4728_t *mcp, mcp4728_channel_en ch)
{
   byte_t    w[3] = {0, 0, 0};
   drv_status_en ret;

   if (ch < MCP4728_CH_A || ch > MCP4728_CH_D)
      return DRV_ERROR;

   // Data preparation
   w[0] = MCP4728_SINGLE_WRITE;
   w[0] |= ch << 1;
   w[0] |= MCP4728_UDAC_UPDATE;

   w[1] |= mcp->conf.vref [ch]  << 7;
   w[1] |= mcp->conf.pwr [ch]   << 5;
   w[1] |= mcp->conf.gain [ch]  << 4;
   w[1] |= (mcp->vout[ch] & 0x0FFF) >> 8;

   w[2] |= (uint8_t)(mcp->vout[ch] & 0x00FF);

   // Send control byte
   if ((ret = _send_control (mcp, MCP4728_WRITE, 1)) != DRV_READY)
      return ret;

   // Transmit channels data
   mcp->io.i2c_tx (mcp->io.i2c, w[0], I2C_SEQ_BYTE_ACK);
   mcp->io.i2c_tx (mcp->io.i2c, w[1], I2C_SEQ_BYTE_ACK);
   if (!mcp->io.i2c_tx (mcp->io.i2c, w[2], I2C_SEQ_BYTE_ACK)) {
      mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }

   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, (void*)0);
   return DRV_READY;
}


/*!
 * \brief
 *    Send a write address command
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  ch    Witch channel to send
 * \return
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
static drv_status_en _cmd_write_add (mcp4728_t *mcp)
{
   byte_t w[3] = {0, 0, 0};

   // Data preparation
   w[0] = (MCP4728_ADD_WRITE | 0x01) | (mcp->conf.cur_addr << 2);
   w[1] = (MCP4728_ADD_WRITE | 0x02) | (mcp->conf.usr_add  << 2);
   w[2] = (MCP4728_ADD_WRITE | 0x03) | (mcp->conf.usr_add  << 2);

   // Send control byte
   if (_send_control (mcp, MCP4728_WRITE, 1) != DRV_READY)
      return DRV_ERROR;

   mcp->io.i2c_tx (mcp->io.i2c, w[0], I2C_SEQ_BYTE);  // Send current ADDRESS command without ack bit
   jf_delay_us (10);                                  // Wait a moment
   mcp->io.ldac (1);                                  // select chip
   mcp->io.i2c_tx (mcp->io.i2c, w[0], I2C_SEQ_ACK);   // Transmit ack clock

   // Send the rest of the data and check the last ack bit
   mcp->io.i2c_tx (mcp->io.i2c, w[1], I2C_SEQ_BYTE_ACK);
   if (!mcp->io.i2c_tx (mcp->io.i2c, w[2], I2C_SEQ_BYTE_ACK)) {
      mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);
      mcp->io.ldac (0);
      return DRV_ERROR;
   }

   mcp->io.i2c_ioctl (mcp->io.i2c, CTRL_STOP, NULL);  // STOP
   if (_sda_release (mcp) != DRV_READY) {             // Make sure the bus released before exit
      mcp->io.ldac (0);
      return DRV_ERROR;
   }
   mcp->io.ldac (0);                                  // Release LDAC now

   if (_wait_busy (mcp) != DRV_READY)                 // Check busy pin
      return DRV_BUSY;

   return DRV_READY;
}





/*
 * ============ Public MCP4728 API ============
 */


/*
 * Link and Glue functions
 */
void mcp4728_link_i2c (mcp4728_t *mcp, void* i2c)                    { mcp->io.i2c = i2c; }
void mcp4728_link_i2c_rx (mcp4728_t *mcp, drv_i2c_rx_ft fun)         { mcp->io.i2c_rx = fun; }
void mcp4728_link_i2c_tx (mcp4728_t *mcp, drv_i2c_tx_ft fun)         { mcp->io.i2c_tx = fun; }
void mcp4728_link_i2c_ioctl (mcp4728_t *mcp, drv_i2c_ioctl_ft fun)   { mcp->io.i2c_ioctl = fun; }
void mcp4728_link_ldac (mcp4728_t *mcp, drv_pinout_ft fun)           { mcp->io.ldac = fun; }
void mcp4728_link_bsy (mcp4728_t *mcp, drv_pinin_ft fun)             { mcp->io.bsy = fun; }



/*
 * Set functions
 */
void mcp4728_set_address (mcp4728_t *mcp, uint8_t add) {
   mcp->conf.usr_add = add;
}
void mcp4728_set_vref (mcp4728_t *mcp, mcp4728_channel_en ch, mcp4728_vref_en vref)
{
   if (ch == MCP4728_CH_ALL) {
      mcp->conf.vref [0] = mcp->conf.vref [1] = mcp->conf.vref [2] = mcp->conf.vref [3] = vref;
   }
   else
      mcp->conf.vref [ch] = vref;
}
void mcp4728_set_pwr (mcp4728_t *mcp, mcp4728_channel_en ch, mcp4728_pwr_en pwr)
{
   if (ch == MCP4728_CH_ALL) {
      mcp->conf.pwr [0] = mcp->conf.pwr [1] = mcp->conf.pwr [2] = mcp->conf.pwr [3] = pwr;
   }
   else
      mcp->conf.pwr [ch] = pwr;
}
void mcp4728_set_gain (mcp4728_t *mcp, mcp4728_channel_en ch, mcp4728_gain_en gain)
{
   if (ch == MCP4728_CH_ALL) {
      mcp->conf.gain [0] = mcp->conf.gain [1] = mcp->conf.gain [2] = mcp->conf.gain [3] = gain;
   }
   else
      mcp->conf.gain [ch] = gain;
}
void mcp4728_set_timeout (mcp4728_t *mcp, uint32_t to) {
   mcp->conf.timeout = to;
}




/*
 * User Functions
 */
/*!
 * \brief
 *    De-Initializes peripherals used by the driver.
 *
 * \param  mcp       Pointer indicate the mcp data stuct to use
 */
void mcp4728_deinit (mcp4728_t *mcp)
{
   memset ((void*)mcp, 0, sizeof (mcp4728_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}


/*!
 * \brief
 *    Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  mcp       Pointer indicate the mcp data stuct to use
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en mcp4728_init (mcp4728_t *mcp)
{
   #define _bad_link(_link)   (!mcp->io._link) ? 1:0
   drv_status_en ra_ret;

   if (_bad_link (i2c))       return mcp->status = DRV_ERROR;
   //!^ Only with i2c bit banging
   if (_bad_link (i2c_rx))    return mcp->status = DRV_ERROR;
   if (_bad_link (i2c_tx))    return mcp->status = DRV_ERROR;
   if (_bad_link (i2c_ioctl)) return mcp->status = DRV_ERROR;
   if (_bad_link (ldac))      return mcp->status = DRV_ERROR;

   if (mcp->status == DRV_BUSY || mcp->status == DRV_NODEV)
      return mcp->status = DRV_ERROR;
   if (jf_probe () != DRV_READY)
      return mcp->status = DRV_ERROR;

   mcp->status = DRV_BUSY;
   mcp->io.ldac (0);

   // Make sure Bus is released
   _sda_release (mcp);

   _gc_reset (mcp);
   if ((ra_ret = _gc_read_address (mcp)) != DRV_READY)
      return DRV_ERROR;

   if (mcp->conf.cur_addr != mcp->conf.usr_add) {
      if (_cmd_write_add (mcp) != DRV_READY)
         return mcp->status = DRV_ERROR;
      mcp->conf.cur_addr = mcp->conf.usr_add;
   }

   return mcp->status = DRV_READY;
   #undef _bad_link
}

/*!
 * \brief
 *    Update DAC's output, without eeprom save using fast write commands
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  ch     The channel to update
 *    \arg  MCP4728_CH_A   For channel A
 *    \arg  MCP4728_CH_B   For channel B
 *    \arg  MCP4728_CH_C   For channel C
 *    \arg  MCP4728_CH_D   For channel D
 *    \arg  MCP4728_CH_ALL For all at once
 * \param vout    Pointer to data to use. The size of buffer must match
 * \return        The status of the operation
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
drv_status_en mcp4728_ch_write (mcp4728_t *mcp, mcp4728_channel_en ch, int16_t *vout)
{
   switch (ch) {
      default:
         return DRV_ERROR;
      case MCP4728_CH_A:
      case MCP4728_CH_B:
      case MCP4728_CH_C:
      case MCP4728_CH_D:
         mcp->vout[ch] = *vout & 0x0FFF;
         break;
      case MCP4728_CH_ALL:
         for (int i=0 ; i<4 ; ++i)
            mcp->vout[i] = vout[i] & 0x0FFF;
         break;
   }
   return _cmd_fast_write (mcp, 4);
}

/*!
 * \brief
 *    Update DAC's output and save eeprom using _cmd_single_write() or
 *    _cmd_seq_write () command.
 *
 * \param  mcp    Pointer indicate the mcp data stuct to use
 * \param  ch     The channel to update
 *    \arg  MCP4728_CH_A   For channel A
 *    \arg  MCP4728_CH_B   For channel B
 *    \arg  MCP4728_CH_C   For channel C
 *    \arg  MCP4728_CH_D   For channel D
 *    \arg  MCP4728_CH_ALL For all at once
 * \param vout    Pointer to data to use. The size of buffer must match
 * \return        The status of the operation
 *    \arg  DRV_BUSY
 *    \arg  DRV_ERROR
 *    \arg  DRV_READY
 */
drv_status_en mcp4728_ch_save (mcp4728_t *mcp, mcp4728_channel_en ch, int16_t *vout)
{
   switch (ch) {
      default:
         return DRV_ERROR;
      case MCP4728_CH_A:
      case MCP4728_CH_B:
      case MCP4728_CH_C:
      case MCP4728_CH_D:
         mcp->vout[ch] = *vout & 0x0FFF;
         return _cmd_single_write (mcp, ch);
      case MCP4728_CH_ALL:
         for (int i=0 ; i<4 ; ++i)
            mcp->vout[i] = vout[i] & 0x0FFF;
         return _cmd_seq_write (mcp, MCP4728_CH_A);
   }
}

/*!
 * \brief
 *    MCP4728 ioctl function
 *
 * \param  mcp    The active mcp struct.
 * \param  cmd    specifies the command to i2c and get back the reply.
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_RESET
 *    \arg MCP_CTRL_WAKEUP
 *    \arg MCP_CTRL_SOFT_UPDATE
 * \param  buf    pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_BUSY
 *    \arg DRV_ERROR
 */
drv_status_en mcp4728_ioctl (mcp4728_t *mcp, ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   drv_status_en ret;

   switch (cmd) {
      case CTRL_DEINIT:          /*!< De-init */
         mcp4728_deinit (mcp);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         ret = mcp4728_init (mcp);
         if (buf) *(drv_status_en*)buf = ret;
         return DRV_READY;
      case CTRL_RESET:
         ret = _gc_reset (mcp);
         if (buf) *(drv_status_en*)buf = ret;
         return DRV_READY;
      case MCP_CTRL_WAKEUP:
         ret = _gc_wakeup (mcp);
         if (buf) *(drv_status_en*)buf = ret;
         return DRV_READY;
      case MCP_CTRL_SOFT_UPDATE:
         ret = _gc_soft_update (mcp);
         if (buf) *(drv_status_en*)buf = ret;
         return DRV_READY;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}
