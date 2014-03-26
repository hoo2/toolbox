/*!
 * \file
 *    ee_i2c_pol.c
 * \brief
 *    Is the header file for the EEPROM Emulation
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


#include <ee_i2c_pol.h>

static ee_status_en _sendcontrol (ee_t *ee, uint8_t rd, uint8_t ap);
static ee_status_en _sendaddress (ee_t *ee, idx_t add);
static int _writepage (ee_t *ee, uint8_t *buf, uint32_t num, idx_t add);


/*!
 * \brief
 *    Send control byte and select to use or not ACK polling
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  rd  : Read flag, 1 to read, 0 to write.
 * \param  ap  : Use Ack polling
 *
 * \retval EE_OK (0) or EE_ERROR(1)
 */
static ee_status_en _sendcontrol (ee_t *ee, uint8_t rd, uint8_t ap)
{
   uint8_t  sl_ack;
   uint32_t to = ee->timeout;

   // Cast rd
   if (rd)  rd = 1;

   // Control byte (read/write) with ACK polling or not
   do
   {
      i2c_start(&ee->i2c);
      sl_ack = i2c_tx(&ee->i2c, ee->hw_addr + rd);
      --to;
   }while (!sl_ack && ap && to);

   return (ee_status_en) !sl_ack;
}

/*!
 * \brief
 *    Send the address (internal memory address) to the BUS.
 *
 *
 */
static ee_status_en _sendaddress (ee_t *ee, idx_t add)
{
   if (ee->size == EE_08)
   {
      if (!i2c_tx (&ee->i2c, add))
         return EE_ERROR;
   }
   else
   {
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
 *    till it reaches the end of the EEPROM page.
 *    Returns the number of written bytes, to help \see ee_writebuffer()
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  buf : Pointer to data to write
 * \param  add : The starting address of the EEPROM
 *
 * \return
 *    The number of written bytes
 */
static int _writepage (ee_t *ee, uint8_t *buf, uint32_t num, idx_t add)
{
   // Page start and page offset and num to write
   uint8_t     pg_offset = add % ee->pagesize;
   uint8_t     i, nl = ee->pagesize - pg_offset; // num up saturation

   if (nl > num)  nl = num;   // Cut out the unnecessary bytes

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
 * Callback connect functions
 */
inline void ee_sethwaddress (ee_t *ee, int add) {
   ee->hw_addr = add;
}

inline void ee_setsize (ee_t *ee, int s) {
   ee->size = s;
}

inline void ee_setpagesize (ee_t *ee, int ps) {
   ee->pagesize = ps;
}

inline void ee_setspeed (ee_t *ee, int sp) {
   ee->speed = sp;
}

inline void ee_settimeout (ee_t *ee, uint32_t to) {
   ee->timeout = to;
}


/*!
 * \brief
 *    De-Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 */
void ee_deinit (ee_t *ee)
{
   i2c_deinit (&ee->i2c);
}

/*!
 * \brief
 *    Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \retval None
 */
void ee_init (ee_t *ee)
{
   i2c_init (&ee->i2c, ee->speed);
}

/*!
 * \brief
 *    Reads the byte at current cursor from the EEPROM.
 *
 * \param  byte : Pointer to the byte that receives the data read from the EEPROM.
 * \retval EE_OK (0) if operation is correctly performed, else return EE_ERROR (1)
 */
ee_status_en ee_readcur (ee_t *ee, uint8_t *byte)
{
   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return EE_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == EE_ERROR)
      return EE_ERROR;

   // Read with NACK
   *byte = i2c_rx(&ee->i2c, 0);

   i2c_stop (&ee->i2c);
   return EE_OK;
}

/*!
 * \brief
 *    Reads a byte from the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  byte: Pointer to the byte that receives the data read from the EEPROM.
 * \param  add : EEPROM's internal address to start reading from.
 *
 * \retval EE_OK (0) if operation is correctly performed, else return EE_ERROR (1)
 */
ee_status_en ee_readbyte (ee_t *ee, uint8_t *byte, idx_t add)
{
   // Send Control byte (write) with ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return EE_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return EE_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == EE_ERROR)
      return EE_ERROR;

   // Read with NACK
   *byte = i2c_rx(&ee->i2c, 0);

   i2c_stop (&ee->i2c);
   return EE_OK;
}

/*!
 * \brief
 *    Reads a block of data from the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  buf : Pointer to the buffer that receives the data read from the EEPROM.
 * \param  num : The number of bytes to be read from the EEPROM.
 * \param  add : EEPROM's internal address to start reading from.
 *
 * \retval EE_OK (0) if operation is correctly performed, else return EE_ERROR (1)
 */
ee_status_en ee_readbuffer (ee_t *ee, uint8_t* buf, uint32_t num, idx_t add)
{
   uint8_t  ack;

   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return EE_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return EE_ERROR;

   // Send Control byte (read).
   if (_sendcontrol (ee, EE_READ, 0) == EE_ERROR)
      return EE_ERROR;

   // Seq read bytes with ACK except last one
   do
   {
      if (num>1)  ack = 1;
      else        ack = 0;
      *buf++ = i2c_rx(&ee->i2c, ack);
      --num;
   }while (num);

   i2c_stop (&ee->i2c);
   return EE_OK;
}

/*!
 * \brief
 *    Writes a byte to the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  byte: Pointer to the byte that receives the data read from the EEPROM.
 * \param  add : EEPROM's internal address to write.
 *
 * \retval EE_OK (0) if operation is correctly performed, else return EE_ERROR (1)
 */
ee_status_en ee_writebyte (ee_t *ee, uint8_t byte, idx_t add)
{
   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return EE_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return EE_ERROR;

   if (!i2c_tx (&ee->i2c, byte))
      return EE_ERROR;

   i2c_stop (&ee->i2c);
   return EE_OK;

}

/*!
 * \brief
 *    Write a block of data to the EEPROM.
 *
 * \param  ee  : Pointer indicate the ee data stuct to use
 * \param  buf : Pointer to the buffer that holds the data to write.
 * \param  num : The number of bytes to be read from the EEPROM.
 * \param  add : EEPROM's internal address to start writing to.
 *
 * \retval EE_OK (0) if operation is correctly performed, else return EE_ERROR (1)
 */
ee_status_en ee_writebuffer (ee_t *ee, uint8_t *buf, uint32_t num, idx_t add)
{
   uint32_t    wb=0;    // The written bytes

   // ACK polling
   if (_sendcontrol (ee, EE_WRITE, 1) == EE_ERROR)
      return EE_ERROR;

   if (_sendaddress (ee, add) == EE_ERROR)
      return EE_ERROR;

   do
      wb += _writepage (ee, &buf[wb], num-wb, add+wb);
   while (wb < num);

   return EE_OK;
}

