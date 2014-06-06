/*!
 * \file ee_i2c_pol.c
 * \brief
 *    A target independent EEPROM (24xx series) driver using i2c_pol
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


#include <drv/ee_i2c_pol.h>

static ee_status_en _sendcontrol (ee_t *ee, uint8_t rd, uint8_t ackp);
static ee_status_en _sendaddress (ee_t *ee, ee_idx_t add);
static int _writepage (ee_t *ee, ee_idx_t add, uint8_t *buf, size_t n);


/*!
 * \brief
 *    Send control byte and select to use or not ACK polling
 *
 * \param  ee    Pointer indicate the ee data stuct to use
 * \param  rd    Read flag, 1 to read, 0 to write.
 * \param  ackp  Use Ack polling
 *
 * \retval EE_OK (0) or EE_ERROR(1)
 */
static ee_status_en _sendcontrol (ee_t *ee, uint8_t rd, uint8_t ackp)
{
   uint8_t  ack;
   uint32_t to = ee->timeout;

   // Cast rd to 0/1
   rd = (rd) ? 1:0;

   // Control byte (read/write) with ACK polling or not
   do {
      i2c_start(&ee->i2c);
      ack = i2c_tx(&ee->i2c, ee->hw_addr + rd);
      --to;
   }while (!ack && ackp && to);

   return (ee_status_en) !ack;
}

/*!
 * \brief
 *    Send the address (internal memory address) to the BUS.
 *
 *
 */
static ee_status_en _sendaddress (ee_t *ee, ee_idx_t add)
{
   if (ee->size == EE_08) {
      if (!i2c_tx (&ee->i2c, add))
         return EE_ERROR;
   } else {
      // MSB of the address first
      if (!i2c_tx (&ee->i2c, (uint8_t)((add & 0xFF00)>>8)))
         return EE_ERROR;
      if (!i2c_tx (&ee->i2c, (uint8_t)(add & 0x00FF)))
         return EE_ERROR;
   }
   return EE_OK;
}

/*!
 * \brief
 *    Writes a number of data to the EEPROM starting from \c add
 *    till it reaches the end of the EEPROM page, Even if buf contains
 *    more data.
 *    Returns the number of written bytes, to help \see ee_writebuffer()
 *
 * \param  ee    Pointer indicate the ee data stuct to use
 * \param  buf   Pointer to data to write
 * \param  add   The starting address of the EEPROM
 *
 * \return
 *    The number of written bytes
 */
static int _writepage (ee_t *ee, ee_idx_t add, uint8_t *buf, size_t n)
{
   // Page start and page offset and num to write
   uint8_t     pg_offset = add % ee->pagesize;
   uint8_t     i, nl = ee->pagesize - pg_offset; // num up saturation

   if (nl > n)  nl = n;   // Cut out the unnecessary bytes

   // Control byte (write)
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return EE_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return EE_ERROR;

   // Try to write the data
   for (i=0 ; i<nl ; ++i, ++buf)
      if (!i2c_tx (&ee->i2c, *buf))
         break;

   // Stop and return the number of written bytes.
   i2c_stop (&ee->i2c);
   return i;
}




/*
 * =================== PUBLIC FUNCTIONS =======================
 */



/*
 * Link and Glue functions
 */

/*
 * Set functions
 */
inline void ee_set_hwaddress (ee_t *ee, int add) {
   ee->hw_addr = add;
}

inline void ee_set_size (ee_t *ee, int s) {
   ee->size = s;
}

inline void ee_set_pagesize (ee_t *ee, int ps) {
   ee->pagesize = ps;
}

inline void ee_set_speed (ee_t *ee, int freq) {
   ee->freq = freq;
   i2c_set_speed (&ee->i2c, freq);
}

inline void ee_set_timeout (ee_t *ee, uint32_t to) {
   ee->timeout = to;
}


/*!
 * \brief
 *    De-Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  ee    Pointer indicate the ee data stuct to use
 */
void ee_deinit (ee_t *ee)
{
   drv_status_en st;
   i2c_ioctl (&ee->i2c, CTRL_GET_STATUS, (ioctl_buf_t*)&st);
   if (st == DRV_READY)
      // When all its OK and we had i2c, cleanup the bus also.
      i2c_ioctl (&ee->i2c, CTRL_DEINIT, (ioctl_buf_t*)0);
   memset ((void*)ee, 0, sizeof (ee_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  ee    Pointer indicate the ee data stuct to use
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
ee_status_en ee_init (ee_t *ee)
{
   drv_status_en st;

   // Dispatch status
   i2c_ioctl (&ee->i2c, CTRL_GET_STATUS, (ioctl_buf_t*)&st);
   switch (st)
   {
      case DRV_NOINIT:
         i2c_ioctl (&ee->i2c, CTRL_INIT, (ioctl_buf_t*)&st);
         if ( st != DRV_READY)
            return ee->status = DRV_ERROR;
         return ee->status = DRV_READY;
      case DRV_READY:
         return ee->status = DRV_READY;
      default:
         return DRV_ERROR;
   }
}

/*!
 * \brief
 *    Reads the byte at current cursor from the EEPROM.
 *
 * \param  byte : Pointer to the byte that receives the data read from the EEPROM.
 * \return The driver status after write.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ee_read (ee_t *ee, uint8_t *byte)
{
   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return ee->status = DRV_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == EE_ERROR)
      return ee->status = DRV_ERROR;

   // Read with NACK
   *byte = i2c_rx(&ee->i2c, 0);

   i2c_stop (&ee->i2c);
   return DRV_READY;
}

/*!
 * \brief
 *    Reads a byte from the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  add : EEPROM's internal address to start reading from.
 * \param  byte: Pointer to the byte that receives the data read from the EEPROM.
 *
 * \return The driver status after write.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ee_readbyte (ee_t *ee, ee_idx_t add, uint8_t *byte)
{
   // Send Control byte (write) with ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return ee->status = DRV_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return ee->status = DRV_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == EE_ERROR)
      return ee->status = DRV_ERROR;

   // Read with NACK
   *byte = i2c_rx(&ee->i2c, 0);

   i2c_stop (&ee->i2c);
   return DRV_READY;
}

/*!
 * \brief
 *    Reads a block of data from the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  add : EEPROM's internal address to start reading from.
 * \param  buf : Pointer to the buffer that receives the data read from the EEPROM.
 * \param  n   : The number of bytes to be read from the EEPROM.
 *
 * \return The driver status after write.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ee_readbuffer (ee_t *ee, ee_idx_t add, uint8_t *buf, size_t n)
{
   uint8_t  ack;

   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return ee->status = DRV_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return ee->status = DRV_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == EE_ERROR)
      return ee->status = DRV_ERROR;;

   // Seq read bytes with ACK except last one
   do {
      ack = (n>1) ? 1:0;
      *buf++ = i2c_rx(&ee->i2c, ack);
      --n;
   }while (n);

   i2c_stop (&ee->i2c);
   return DRV_READY;
}

/*!
 * \brief
 *    Writes a byte to the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  add : EEPROM's internal address to write.
 * \param  byte: Pointer to the byte that receives the data read from the EEPROM.
 *
 * \return The driver status after write.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ee_writebyte (ee_t *ee, ee_idx_t add, uint8_t byte)
{
   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return ee->status = DRV_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return ee->status = DRV_ERROR;

   if (!i2c_tx (&ee->i2c, byte))
      return ee->status = DRV_ERROR;

   i2c_stop (&ee->i2c);
   return DRV_READY;

}

/*!
 * \brief
 *    Write a block of data to the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  add : EEPROM's internal address to start writing to.
 * \param  buf : Pointer to the buffer that holds the data to write.
 * \param  n   : The number of bytes to be read from the EEPROM.
 *
 * \return The driver status after write.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ee_writebuffer (ee_t *ee, ee_idx_t add, uint8_t *buf, size_t n)
{
   uint32_t    wb=0;    // The written bytes

   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return ee->status = DRV_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return ee->status = DRV_ERROR;

   do
      wb += _writepage (ee, add+wb, &buf[wb], n-wb);
      /*!
       * \note
       * Each _writepage writes only until the page limit, so we
       * call _writepage until we have no more data to send.
       */
   while (wb < n);

   return DRV_READY;
}

/*!
 * \brief
 *    EEPROM ioctl function
 *
 * \param  ee     The active see struct.
 * \param  cmd    specifies the command to i2c and get back the reply.
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_GET_SIZE
 *    \arg CTRL_ERASE_PAGE
 *    \arg CTRL_ERASE_ALL
 *    \arg CTRL_FORMAT
 * \param  buf    pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ee_ioctl (ee_t *ee, ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   switch (cmd)
   {
      case CTRL_GET_STATUS:      /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = ee->status;
         return DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
         ee_deinit(ee);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = ee_init(ee);
         else
            ee_init(ee);
         return DRV_READY;
      case CTRL_GET_SIZE:        /*!< EEPROM size */
         if (buf)
            *(drv_status_en*)buf = ee->size;
         return DRV_READY;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}

