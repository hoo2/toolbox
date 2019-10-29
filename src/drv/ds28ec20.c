/*!
 * \file ds28ec20.c
 * \brief
 *    A target independent 1-wire EEPROM (ds28ec20 series) driver using
 *    1-wire in bit-banking or UART mode
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2017 Christos Choutouridis (http://www.houtouridis.net)
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
#include <drv/ds28ec20.h>
/*
 *  ============= Static DS28EC20 API =============
 */
static void _tx_bytes (ds28ec20_t *ds, byte_t *buf, bytecount_t n);
static void _rx_bytes (ds28ec20_t *ds, byte_t *buf, bytecount_t n);
static drv_status_en _rst_select (ds28ec20_t *ds);

static drv_status_en _write_scratchpad (ds28ec20_t *ds, ds28ec20_ar_t *ar, byte_t *sp);
static drv_status_en _read_scratchpad (ds28ec20_t *ds, ds28ec20_ar_t *ar, byte_t *sp);
static drv_status_en _copy_scratchpad (ds28ec20_t *ds, ds28ec20_ar_t *ar);
static drv_status_en _read_memory (ds28ec20_t *ds, address_t add, byte_t *buf, bytecount_t n);

static int _write_row (ds28ec20_t *ds, address_t add, byte_t *buf, bytecount_t n);

/*
 * Helper Macros and functions
 */
#define  _ds28ec20_TA(_ta1_, _ta2_)   ((uint16_t)(_ta1_) | (((uint16_t)(_ta2_)) << 8))
#define  _ds28ec20_TA1(_ta_)          ((uint8_t)(_ta_))
#define  _ds28ec20_TA2(_ta_)          ((uint8_t)((_ta_)>>8))
/*!
 * \brief
 *    Transmit a number of bytes to 1-Wire bus
 * \param   ds    Pointer indicate the ds28ec20 data structure to use
 * \param   buf   Pointer to data to send
 * \param   n     The size of buffer to send
 */
static void _tx_bytes (ds28ec20_t *ds, byte_t *buf, bytecount_t n)
{
   for (bytecount_t i=0 ; i<n ; ++i)
      ds->io.tx (ds->io.ow, buf[i]);
}

/*!
 * \brief
 *    Receive a number of bytes from 1-Wire bus
 * \param   ds    Pointer indicate the ds28ec20 data stuct to use
 * \param   buf   Pointer to buffer to store the read data
 * \param   n     The size of buffer to read
 */
static void _rx_bytes (ds28ec20_t *ds, byte_t *buf, bytecount_t n)
{
   for (bytecount_t i=0 ; i<n ; ++i)
      buf[i] = ds->io.rx (ds->io.ow);
}

/*!
 * \brief
 *    Issue a Reset and select part of a command to the 1-Wire bus
 * \param   ds    Pointer indicate the ds28ec20 data structure to use
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _rst_select (ds28ec20_t *ds)
{
   drv_status_en r;

   /* Reset - PD */
   if ( (r = ds->io.ioctl (ds->io.ow, CTRL_RESET, 0)) != DRV_READY)
      return r;
   /* Select */
   switch (ds->conf.bus) {
      case DS28EC20_1WBUS_SINGLEDROP:
         ds->io.tx (ds->io.ow, DS28EC20_SKIPROM);
         break;
      case DS28EC20_1WBUS_MULTIDROP:
         ds->io.tx (ds->io.ow, DS28EC20_MATCHROM);
         _tx_bytes (ds, ds->conf.romid, 8);
         break;
   }
   return DRV_READY;
}


/*
 * Communication protocol functions
 */

/*!
 * \brief
 *    Write scratchpad command
 * \param   ds    Pointer indicate the ds28ec20 data structure to use
 * \param   ar    Pointer to ds28ec20 Address register data to send
 * \param   sp    Pointer to Scratchpad data to send
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _write_scratchpad (ds28ec20_t *ds, ds28ec20_ar_t *ar, byte_t *sp)
{
   drv_status_en r;
   uint16_t crc, sl_crc;

   /* Reset - PD - Select */
   if ( (r = _rst_select (ds)) != DRV_READY)
      return r;

   /* WS */
   ds->io.tx (ds->io.ow, DS28EC20_WRITESCRATCH);
   /* TA */
   ds->io.tx (ds->io.ow, ar->TA1);
   ds->io.tx (ds->io.ow, ar->TA2);

   /* Write SP */
   _tx_bytes (ds, sp, DS28EC20_SCRATCHPAD_SIZE);

   /* CRC16 */
   _rx_bytes (ds, (byte_t*)&sl_crc, sizeof (sl_crc));

   crc = 0;
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, DS28EC20_WRITESCRATCH);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA1);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA2);
   crc = CRC16_buffer (CRC16_IBM_rev, CRC_LSB, crc, (byte_t*)sp, DS28EC20_SCRATCHPAD_SIZE);
   crc = ~crc;

   if (crc != sl_crc)   return DRV_ERROR;
   else                 return DRV_READY;
}

/*!
 * \brief
 *    Read scratchpad command
 * \param   ds    Pointer indicate the ds28ec20 data structure to use
 * \param   ar    Pointer to ds28ec20 Address register buffer to store the data from ds28ec20
 * \param   sp    Pointer to Scratchpad buffer to store the data from ds28ec20
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _read_scratchpad (ds28ec20_t *ds, ds28ec20_ar_t *ar, byte_t *sp)
{
   drv_status_en r;
   uint16_t crc, sl_crc;

   /* Reset - PD - Select */
   if ( (r = _rst_select (ds)) != DRV_READY)
      return r;

   /* RS */
   ds->io.tx (ds->io.ow, DS28EC20_READSCRATCH);
   /* TA-E/S */
   ar->TA1 = ds->io.rx (ds->io.ow);
   ar->TA2 = ds->io.rx (ds->io.ow);
   ar->ES = ds->io.rx (ds->io.ow);

   /* Read SP */
   _rx_bytes (ds, sp, DS28EC20_SCRATCHPAD_SIZE);

   /* CRC16 */
   _rx_bytes (ds, (byte_t*)&sl_crc, sizeof (sl_crc));

   crc = 0;
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, DS28EC20_READSCRATCH);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA1);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA2);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->ES);
   crc = CRC16_buffer (CRC16_IBM_rev, CRC_LSB, crc, (byte_t*)sp, DS28EC20_SCRATCHPAD_SIZE);
   crc = ~crc;

   if (crc != sl_crc)   return DRV_ERROR;
   else                 return DRV_READY;
}

/*!
 * \brief
 *    Copy scratchpad command
 * \param   ds    Pointer indicate the ds28ec20 data structure to use
 * \param   ar    Pointer to ds28ec20 Address register buffer to store the data from ds28ec20
 * \param   sp    Pointer to Scratchpad buffer to store the data from ds28ec20
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 * \warning After the copy scratchpad command the master MUST wait until the Tprog elapses
 *          For DS28EC20 this time has a max value of 10msec
 */
static drv_status_en _copy_scratchpad (ds28ec20_t *ds, ds28ec20_ar_t *ar)
{
   drv_status_en r;

   /* Reset - PD - Select */
   if ( (r = _rst_select (ds)) != DRV_READY)
      return r;

   /* CPS - Copy scratchpad command */
   ds->io.tx (ds->io.ow, DS28EC20_COPYSCRATCH);

   /* TA-E/S */
   ds->io.tx (ds->io.ow, ar->TA1);
   ds->io.tx (ds->io.ow, ar->TA2);
   ds->io.tx (ds->io.ow, ar->ES);

   ds->io.delay ();
   return DRV_READY;
}

/*!
 * \brief
 *    Read Memory command
 * \param   ds    Pointer indicate the ds28ec20 data structure to use
 * \param   add   ds28ec20 Address to store read data from
 * \param   buf   Pointer to buffer to store the data from ds28ec20
 * \param   n     The size of data in bytes
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _read_memory (ds28ec20_t *ds, address_t add, byte_t *buf, bytecount_t n)
{
   drv_status_en r;

   /* Reset - PD - Select */
   if ( (r = _rst_select(ds)) != DRV_READY)
      return r;

   /* RM - Read memory command */
   ds->io.tx (ds->io.ow, DS28EC20_READMEM);
   /* TA  - Read address LSB->MSB */
   ds->io.tx (ds->io.ow, (uint8_t) (0x00FF & add));
   ds->io.tx (ds->io.ow, (uint8_t) ((0xFF00 & add) >> 8));

   /* Read memory */
   for (bytecount_t i=0 ; i<n ; ++i) {
      buf [i] = ds->io.rx (ds->io.ow);
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Writes a number of data to the EEPROM starting from \b add
 *    till it reaches the end of the EEPROM row, Even if buf contains
 *    more data.
 *    Returns the number of written bytes, to help \ref ds28ec20_write()
 *
 * \param ds   Pointer indicate the ds28ec20 data structure to use
 * \param add  The EEPROM address to write to
 * \param buf  Pointer to data buffer to store to the EEPROM
 * \param n    The size in bytes of data to write
 * \return     The number of written bytes
 */
static int _write_row (ds28ec20_t *ds, address_t add, byte_t *buf, bytecount_t n)
{
   byte_t      sp [DS28EC20_SCRATCHPAD_SIZE];
   byte_t      sp_ret [DS28EC20_SCRATCHPAD_SIZE];
   ds28ec20_ar_t ar, ar_ret = {0, 0, 0};
   uint16_t    row = (add / DS28EC20_SCRATCHPAD_SIZE) * DS28EC20_SCRATCHPAD_SIZE;
   uint16_t    offset = add % DS28EC20_SCRATCHPAD_SIZE;
   uint16_t    nl = DS28EC20_SCRATCHPAD_SIZE - offset;

   if (nl > n)  nl = n;   // Cut out the unnecessary bytes

   if ((offset != 0) || (nl+offset < DS28EC20_SCRATCHPAD_SIZE))
      if (_read_memory (ds, row, sp, DS28EC20_SCRATCHPAD_SIZE) != DRV_READY)
         return 0;
   memcpy ((void*)&sp [offset], (const void*)buf, nl);
   ar.TA1 = _ds28ec20_TA1 (row);
   ar.TA2 = _ds28ec20_TA2 (row);

   // Write with verification
   if (_write_scratchpad (ds, &ar, sp) != DRV_READY)
      return 0;
   else {
      _read_scratchpad (ds, &ar_ret, sp_ret);
      // Verify data
      if (memcmp ((const void*)sp, (const void*)sp_ret, DS28EC20_SCRATCHPAD_SIZE) != 0)
         return 0;
      if ((ar.TA1 != ar_ret.TA1) || (ar.TA2 != ar_ret.TA2))
         return 0;
      if ((ar_ret.ES & DS28EC20_AR_PF_MASK) != 0)
         return 0;
      if (_copy_scratchpad (ds, &ar_ret) != DRV_READY)
         return 0;
      return nl;
   }
}


/*
 *  ============= PUBLIC DS28EC20 API =============
 */

/*
 * Link and Glue functions
 */
__INLINE void ds28ec20_link_ow (ds28ec20_t *ds28ec20, void* ow) {
   ds28ec20->io.ow = (ow != NULL) ? ow:NULL;
}
__INLINE void ds28ec20_link_rx (ds28ec20_t *ds28ec20, ds28ec20_rx_ft fun) {
   ds28ec20->io.rx = (fun != NULL) ? fun:NULL;
}
__INLINE void ds28ec20_link_tx (ds28ec20_t *ds28ec20, ds28ec20_tx_ft fun) {
   ds28ec20->io.tx = (fun != NULL) ? fun:NULL;
}
__INLINE void ds28ec20_link_ioctl (ds28ec20_t *ds28ec20, ds28ec20_ioctl_ft fun) {
   ds28ec20->io.ioctl = (fun != NULL) ? fun:NULL;
}
__INLINE void ds28ec20_link_delay (ds28ec20_t *ds28ec20, ds28ec20_delay_ft fun) {
   ds28ec20->io.delay = (fun != NULL) ? fun:NULL;
}

/*
 * Set functions
 */
//__INLINE void ds28ec20_set_timeout (ds28ec20_t *ds28ec20, uint32_t to) {
//   ds28ec20->conf.timeout = to;
//}



/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initializes peripherals used by the DS28EC20 EEPROM driver.
 *
 * \param  ds28ec20    Pointer indicate the ds28ec20 data structure to use
 */
void ds28ec20_deinit (ds28ec20_t *ds28ec20)
{
   memset ((void*)ds28ec20, 0, sizeof (ds28ec20_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initializes peripherals used by the DS28EC20 EEPROM driver.
 *
 * \param  ds28ec20    Pointer indicate the ds28ec20 data structure to use
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ds28ec20_init (ds28ec20_t *ds28ec20)
{
   #define _bad_link(_link)   (!ds28ec20->io._link) ? 1:0

   if (_bad_link (ow))     return ds28ec20->status = DRV_ERROR;
   if (_bad_link (rx))     return ds28ec20->status = DRV_ERROR;
   if (_bad_link (tx))     return ds28ec20->status = DRV_ERROR;
   if (_bad_link (ioctl))  return ds28ec20->status = DRV_ERROR;
   if (_bad_link (delay))  return ds28ec20->status = DRV_ERROR;

   if (ds28ec20->status == DRV_BUSY || ds28ec20->status == DRV_NODEV)
      return ds28ec20->status = DRV_ERROR;

   ds28ec20->status = DRV_BUSY;

//   if (!ds28ec20->conf.timeout)   ds28ec20->conf.timeout = DS28EC20_DEFAULT_TIMEOUT;

   return ds28ec20->status = DRV_READY;
   #undef _bad_link
}

/*!
 * \brief
 *    Reads a number of bytes from a specific address in EEPROM
 * \param ds28ec20  Pointer indicate the ds28ec20 data structure to use
 * \param add     The EEPROM address to read from
 * \param buf     Pointer to data buffer to store the data from EEPROM
 * \param n       The size of data to read in bytes
 * \return     The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
__INLINE drv_status_en  ds28ec20_read (ds28ec20_t *ds28ec20, address_t add, byte_t *buf, bytecount_t n) {
   if ((add+n) > DS28EC20_MAX_SIZE)
      return DRV_ERROR;
   return _read_memory (ds28ec20, add, buf, n);
}

/*!
 * \brief
 *    Writes a number of bytes to a specific address in EEPROM
 * \param ds28ec20  Pointer indicate the ds28ec20 data structure to use
 * \param add     The EEPROM address to write to
 * \param buf     Pointer to data buffer to store to the EEPROM
 * \param n       The size of data to write in bytes
 * \return     The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ds28ec20_write (ds28ec20_t *ds28ec20, address_t add, byte_t *buf, bytecount_t n) {

   if ((add+n) > DS28EC20_MAX_SIZE)
      return DRV_ERROR;

   uint32_t wb=0;    // The written bytes
   int      ret;

   do {
      ret = _write_row (ds28ec20, add+wb, &buf[wb], n-wb);
      if (ret == 0)     return DRV_ERROR;
      else              wb += ret;
      /*!
       * \note
       * Each _write_row writes only until the page limit, so we
       * call _write_row until we have no more data to send.
       */
   } while (wb < n);

   return DRV_READY;
}

drv_status_en ds28ec20_ioctl (ds28ec20_t *ds28ec20, ioctl_cmd_t cmd, ioctl_buf_t buf);
