/*!
 * \file ee_i2c.c
 * \brief
 *    A target independent EEPROM (24xx series) driver
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


#include <drv/ee_i2c.h>

static drv_status_en _sendcontrol (ee_t *ee, uint8_t rd, uint8_t ackp);
static drv_status_en _sendaddress (ee_t *ee, address_t add);
static int _writepage (ee_t *ee, address_t add, byte_t *buf, bytecount_t n);


/*!
 * \brief
 *    Send control byte and select to use or not ACK polling
 *
 * \param  ee    Pointer indicate the ee data stuct to use
 * \param  rd    Read flag, 1 to read, 0 to write.
 * \param  ackp  Use Ack polling
 *
 * \return
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _sendcontrol (ee_t *ee, uint8_t rd, uint8_t ackp)
{
   uint8_t  ack;
   uint32_t to = ee->conf.timeout;

   // Cast rd to 0/1
   rd = (rd) ? 1:0;

   // Control byte (read/write) with ACK polling or not
   do {
      ee->io.i2c_ioctl (ee->io.i2c, CTRL_START, (void*)0);
      ack = ee->io.i2c_tx (ee->io.i2c, ee->conf.hw_addr + rd);
      --to;
   }while (!ack && ackp && to);

   return (drv_status_en) (ack)? DRV_READY : DRV_ERROR;
}

/*!
 * \brief
 *    Send the address (internal memory address) to the BUS.
 *
 * \param  ee   Pointer indicate the ee data stuct to use
 * \param  add  The address to send
 * \return
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _sendaddress (ee_t *ee, address_t add)
{
   if (ee->conf.size == EE_08) {
      if (!ee->io.i2c_tx (ee->io.i2c, add))
         return DRV_ERROR;
   } else {
      // MSB of the address first
      if (!ee->io.i2c_tx (ee->io.i2c, (byte_t)((add & 0xFF00)>>8)))
         return DRV_ERROR;
      if (!ee->io.i2c_tx (ee->io.i2c, (byte_t)(add & 0x00FF)))
         return DRV_ERROR;
   }
   return DRV_READY;
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
static int _writepage (ee_t *ee, address_t add, byte_t *buf, bytecount_t n)
{
   // Page start and page offset and num to write
   uint8_t     pg_offset = add % ee->conf.page_size;
   uint8_t     i, nl = ee->conf.page_size - pg_offset; // num up saturation

   if (nl > n)  nl = n;   // Cut out the unnecessary bytes

   // Control byte (write)
   if (_sendcontrol (ee, EE_WRITE, 1) == DRV_ERROR)
      return -1;

   if (_sendaddress (ee, add) == DRV_ERROR)
      return -1;

   // Try to write the data
   for (i=0 ; i<nl ; ++i, ++buf)
      if (!ee->io.i2c_tx (ee->io.i2c, *buf))
         break;

   // Stop and return the number of written bytes.
   ee->io.i2c_ioctl (ee->io.i2c, CTRL_STOP, (void*)0);
   return i;
}




/*
 * =================== PUBLIC FUNCTIONS =======================
 */



/*
 * Link and Glue functions
 */
inline void ee_link_i2c (ee_t *ee, void* i2c) {
   ee->io.i2c = i2c;
}
inline void ee_link_i2c_rx (ee_t *ee, ee_i2c_rx_ft fun) {
   ee->io.i2c_rx = fun;
}
inline void ee_link_i2c_tx (ee_t *ee, ee_i2c_tx_ft fun) {
   ee->io.i2c_tx = fun;
}
inline void ee_link_i2c_ioctl (ee_t *ee, ee_i2c_ioctl_ft fun) {
   ee->io.i2c_ioctl = fun;
}

/*
 * Set functions
 */
inline void ee_set_hwaddress (ee_t *ee, address_t add) {
   ee->conf.hw_addr = add;
}

inline void ee_set_size (ee_t *ee, ee_size_en s) {
   ee->conf.size = s;
}

inline void ee_set_page_size (ee_t *ee, uint32_t ps) {
   ee->conf.page_size = ps;
}

inline void ee_set_sector_size (ee_t *ee, uint32_t ss) {
   ee->conf.sector_size = ss;
}

inline void ee_set_timeout (ee_t *ee, uint32_t to) {
   ee->conf.timeout = to;
}


/*!
 * \brief
 *    De-Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  ee    Pointer indicate the ee data stuct to use
 */
void ee_deinit (ee_t *ee)
{
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
drv_status_en ee_init (ee_t *ee)
{
   #define _bad_link(_link)   (!ee->io._link) ? 1:0

   if (_bad_link (i2c))       return ee->status = DRV_ERROR;
   if (_bad_link (i2c_rx))    return ee->status = DRV_ERROR;
   if (_bad_link (i2c_tx))    return ee->status = DRV_ERROR;
   if (_bad_link (i2c_ioctl)) return ee->status = DRV_ERROR;

   if (ee->status == DRV_BUSY || ee->status == DRV_NODEV)
      return ee->status = DRV_ERROR;

   ee->status = DRV_BUSY;

   if (!ee->conf.page_size)   ee->conf.page_size = EE_PAGE_SZ_DEF;
   if (!ee->conf.sector_size) ee->conf.sector_size = EE_SECTOR_SIZE_DEF;

   return ee->status = DRV_READY;
   #undef _bad_link
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
drv_status_en ee_read_cursor (ee_t *ee, byte_t *byte)
{
   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == DRV_ERROR)
      return DRV_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == DRV_ERROR)
      return DRV_ERROR;

   // Read with NACK
   *byte = ee->io.i2c_rx (ee->io.i2c, 0);

   ee->io.i2c_ioctl (ee->io.i2c, CTRL_STOP, (void*)0);
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
drv_status_en ee_read_byte (ee_t *ee, address_t add, byte_t *byte)
{
   // Send Control byte (write) with ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == DRV_ERROR)
      return DRV_ERROR;

   if (_sendaddress (ee, add) == DRV_ERROR)
      return DRV_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == DRV_ERROR)
      return DRV_ERROR;

   // Read with NACK
   *byte = ee->io.i2c_rx (ee->io.i2c, 0);

   ee->io.i2c_ioctl (ee->io.i2c, CTRL_STOP, (void*)0);
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
drv_status_en ee_write_byte (ee_t *ee, address_t add, byte_t byte)
{
   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == DRV_ERROR)
      return DRV_ERROR;

   if (_sendaddress (ee, add) == DRV_ERROR)
      return DRV_ERROR;

   if (!ee->io.i2c_tx (ee->io.i2c, byte))
      return DRV_ERROR;

   ee->io.i2c_ioctl (ee->io.i2c, CTRL_STOP, (void*)0);
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
drv_status_en ee_read (ee_t *ee, address_t add, byte_t *buf, bytecount_t n)
{
   uint8_t  ack;

   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == DRV_ERROR)
      return DRV_ERROR;

   if (_sendaddress (ee, add) == DRV_ERROR)
      return DRV_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == DRV_ERROR)
      return DRV_ERROR;

   // Seq read bytes with ACK except last one
   do {
      ack = (n>1) ? 1:0;
      *buf++ = ee->io.i2c_rx (ee->io.i2c, ack);
      --n;
   }while (n);

   ee->io.i2c_ioctl (ee->io.i2c, CTRL_STOP, (void*)0);
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
drv_status_en ee_write (ee_t *ee, address_t add, byte_t *buf, bytecount_t n)
{
   uint32_t wb=0;    // The written bytes
   int      ret;

   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == DRV_ERROR)
      return ee->status = DRV_ERROR;

   if (_sendaddress (ee, add) == DRV_ERROR)
      return ee->status = DRV_ERROR;

   do {
      ret = _writepage (ee, add+wb, &buf[wb], n-wb);
      if (ret == -1)    return DRV_ERROR;
      else              wb += ret;
      /*!
       * \note
       * Each _writepage writes only until the page limit, so we
       * call _writepage until we have no more data to send.
       */
   } while (wb < n);

   return DRV_READY;
}


drv_status_en  ee_read_sector (ee_t *ee, int sector, byte_t *buf, int count) {
   return DRV_ERROR;
}

drv_status_en ee_write_sector (ee_t *ee, int sector, byte_t *buf, int count) {
   return DRV_ERROR;
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
 *    \arg CTRL_GET_SECTOR_SIZE
 *    \arg CTRL_ERASE_PAGE    **
 *    \arg CTRL_ERASE_ALL     **
 *    \arg CTRL_FORMAT        **
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
            *(drv_status_en*)buf = ee->conf.size;
         return DRV_READY;
      case CTRL_GET_SECTOR_SIZE: /*!< EEPROM sector size */
         if (buf)
            *(drv_status_en*)buf = ee->conf.sector_size;
         return DRV_READY;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}

