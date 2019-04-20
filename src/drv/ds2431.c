/*!
 * \file ds2431.c
 * \brief
 *    A target independent 1-wire EEPROM (DS2431 series) driver using
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
#include <drv/ds2431.h>
/*
 *  ============= Static DS2431 API =============
 */
static void _tx_bytes (ds2431_t *ds, byte_t *buf, bytecount_t n);
static void _rx_bytes (ds2431_t *ds, byte_t *buf, bytecount_t n);
static drv_status_en _rst_select (ds2431_t *ds);

static drv_status_en _write_scratchpad (ds2431_t *ds, ds2431_ar_t *ar, byte_t *sp);
static drv_status_en _read_scratchpad (ds2431_t *ds, ds2431_ar_t *ar, byte_t *sp);
static drv_status_en _copy_scratchpad (ds2431_t *ds, ds2431_ar_t *ar);
static drv_status_en _read_memory (ds2431_t *ds, address_t add, byte_t *buf, bytecount_t n);

static int _write_row (ds2431_t *ds, address_t add, byte_t *buf, bytecount_t n);

/*
 * Helper Macros and functions
 */
#define  _ds2431_TA(_ta1_, _ta2_)   ((uint16_t)(_ta1_) | (((uint16_t)(_ta2_)) << 8))
#define  _ds2431_TA1(_ta_)          ((uint8_t)(_ta_))
#define  _ds2431_TA2(_ta_)          ((uint8_t)((_ta_)>>8))
/*!
 * \brief
 *    Transmit a number of bytes to 1-Wire bus
 * \param   ds    Pointer indicate the ds2431 data structure to use
 * \param   buf   Pointer to data to send
 * \param   n     The size of buffer to send
 */
static void _tx_bytes (ds2431_t *ds, byte_t *buf, bytecount_t n)
{
   for (bytecount_t i=0 ; i<n ; ++i)
      ds->io.tx (ds->io.ow, buf[i]);
}

/*!
 * \brief
 *    Receive a number of bytes from 1-Wire bus
 * \param   ds    Pointer indicate the ds2431 data stuct to use
 * \param   buf   Pointer to buffer to store the read data
 * \param   n     The size of buffer to read
 */
static void _rx_bytes (ds2431_t *ds, byte_t *buf, bytecount_t n)
{
   for (bytecount_t i=0 ; i<n ; ++i)
      buf[i] = ds->io.rx (ds->io.ow);
}

/*!
 * \brief
 *    Issue a Reset and select part of a command to the 1-Wire bus
 * \param   ds    Pointer indicate the ds2431 data structure to use
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _rst_select (ds2431_t *ds)
{
   drv_status_en r;

   /* Reset - PD */
   if ( (r = ds->io.ioctl (ds->io.ow, CTRL_RESET, 0)) != DRV_READY)
      return r;
   /* Select */
   switch (ds->conf.bus) {
      case _1WBUS_SINGLEDROP:
         ds->io.tx (ds->io.ow, DS2431_SKIPROM);
         break;
      case _1WBUS_MULTIDROP:
         ds->io.tx (ds->io.ow, DS2431_MATCHROM);
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
 * \param   ds    Pointer indicate the ds2431 data structure to use
 * \param   ar    Pointer to ds2431 Address register data to send
 * \param   sp    Pointer to Scratchpad data to send
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _write_scratchpad (ds2431_t *ds, ds2431_ar_t *ar, byte_t *sp)
{
   drv_status_en r;
   uint16_t crc, sl_crc;

   /* Reset - PD - Select */
   if ( (r = _rst_select (ds)) != DRV_READY)
      return r;

   /* WS */
   ds->io.tx (ds->io.ow, DS2431_WRITESCRATCH);
   /* TA */
   ds->io.tx (ds->io.ow, ar->TA1);
   ds->io.tx (ds->io.ow, ar->TA2);

   /* Write SP */
   _tx_bytes (ds, sp, DS2431_SCRATCHPAD_SIZE);

   /* CRC16 */
   _rx_bytes (ds, (byte_t*)&sl_crc, sizeof (sl_crc));

   crc = 0;
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, DS2431_WRITESCRATCH);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA1);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA2);
   crc = CRC16_buffer (CRC16_IBM_rev, CRC_LSB, crc, (byte_t*)sp, DS2431_SCRATCHPAD_SIZE);
   crc = ~crc;

   if (crc != sl_crc)   return DRV_ERROR;
   else                 return DRV_READY;
}

/*!
 * \brief
 *    Read scratchpad command
 * \param   ds    Pointer indicate the ds2431 data structure to use
 * \param   ar    Pointer to ds2431 Address register buffer to store the data from ds2431
 * \param   sp    Pointer to Scratchpad buffer to store the data from ds2431
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _read_scratchpad (ds2431_t *ds, ds2431_ar_t *ar, byte_t *sp)
{
   drv_status_en r;
   uint16_t crc, sl_crc;

   /* Reset - PD - Select */
   if ( (r = _rst_select (ds)) != DRV_READY)
      return r;

   /* RS */
   ds->io.tx (ds->io.ow, DS2431_READSCRATCH);
   /* TA-E/S */
   ar->TA1 = ds->io.rx (ds->io.ow);
   ar->TA2 = ds->io.rx (ds->io.ow);
   ar->ES = ds->io.rx (ds->io.ow);

   /* Read SP */
   _rx_bytes (ds, sp, DS2431_SCRATCHPAD_SIZE);

   /* CRC16 */
   _rx_bytes (ds, (byte_t*)&sl_crc, sizeof (sl_crc));

   crc = 0;
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, DS2431_READSCRATCH);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA1);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->TA2);
   crc = CRC16_byte (CRC16_IBM_rev, CRC_LSB, crc, ar->ES);
   crc = CRC16_buffer (CRC16_IBM_rev, CRC_LSB, crc, (byte_t*)sp, DS2431_SCRATCHPAD_SIZE);
   crc = ~crc;

   if (crc != sl_crc)   return DRV_ERROR;
   else                 return DRV_READY;
}

/*!
 * \brief
 *    Copy scratchpad command
 * \param   ds    Pointer indicate the ds2431 data structure to use
 * \param   ar    Pointer to ds2431 Address register buffer to store the data from ds2431
 * \param   sp    Pointer to Scratchpad buffer to store the data from ds2431
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 * \warning After the copy scratchpad command the master MUST wait until the Tprog elapses
 *          For DS2431 this time has a max value of 10msec
 */
static drv_status_en _copy_scratchpad (ds2431_t *ds, ds2431_ar_t *ar)
{
   drv_status_en r;

   /* Reset - PD - Select */
   if ( (r = _rst_select (ds)) != DRV_READY)
      return r;

   /* CPS - Copy scratchpad command */
   ds->io.tx (ds->io.ow, DS2431_COPYSCRATCH);

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
 * \param   ds    Pointer indicate the ds2431 data structure to use
 * \param   add   ds2431 Address to store read data from
 * \param   buf   Pointer to buffer to store the data from ds2431
 * \param   n     The size of data in bytes
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _read_memory (ds2431_t *ds, address_t add, byte_t *buf, bytecount_t n)
{
   drv_status_en r;

   /* Reset - PD - Select */
   if ( (r = _rst_select(ds)) != DRV_READY)
      return r;

   /* RM - Read memory command */
   ds->io.tx (ds->io.ow, DS2431_READMEM);
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
 *    Returns the number of written bytes, to help \ref ds2431_write()
 *
 * \param ds   Pointer indicate the ds2431 data structure to use
 * \param add  The EEPROM address to write to
 * \param buf  Pointer to data buffer to store to the EEPROM
 * \param n    The size in bytes of data to write
 * \return     The number of written bytes
 */
static int _write_row (ds2431_t *ds, address_t add, byte_t *buf, bytecount_t n)
{
   byte_t      sp [DS2431_SCRATCHPAD_SIZE];
   byte_t      sp_ret [DS2431_SCRATCHPAD_SIZE];
   ds2431_ar_t ar, ar_ret = {0, 0, 0};
   uint16_t    row = (add / DS2431_SCRATCHPAD_SIZE) * DS2431_SCRATCHPAD_SIZE;
   uint16_t    offset = add % DS2431_SCRATCHPAD_SIZE;
   uint16_t    nl = DS2431_SCRATCHPAD_SIZE - offset;

   if (nl > n)  nl = n;   // Cut out the unnecessary bytes

   if ((offset != 0) || (nl+offset < DS2431_SCRATCHPAD_SIZE))
      if (_read_memory (ds, row, sp, DS2431_SCRATCHPAD_SIZE) != DRV_READY)
         return 0;
   memcpy ((void*)&sp [offset], (const void*)buf, nl);
   ar.TA1 = _ds2431_TA1 (row);
   ar.TA2 = _ds2431_TA2 (row);

   // Write with verification
   if (_write_scratchpad (ds, &ar, sp) != DRV_READY)
      return 0;
   else {
      _read_scratchpad (ds, &ar_ret, sp_ret);
      // Verify data
      if (memcmp ((const void*)sp, (const void*)sp_ret, DS2431_SCRATCHPAD_SIZE) != 0)
         return 0;
      if ((ar.TA1 != ar_ret.TA1) || (ar.TA2 != ar_ret.TA2))
         return 0;
      if ((ar_ret.ES & DS2431_AR_PF_MASK) != 0)
         return 0;
      if (_copy_scratchpad (ds, &ar_ret) != DRV_READY)
         return 0;
      return nl;
   }
}


/*
 *  ============= PUBLIC DS2431 API =============
 */

/*
 * Link and Glue functions
 */
__INLINE void ds2431_link_ow (ds2431_t *ds2431, void* ow) {
   ds2431->io.ow = (ow != NULL) ? ow:NULL;
}
__INLINE void ds2431_link_rx (ds2431_t *ds2431, ds2431_rx_ft fun) {
   ds2431->io.rx = (fun != NULL) ? fun:NULL;
}
__INLINE void ds2431_link_tx (ds2431_t *ds2431, ds2431_tx_ft fun) {
   ds2431->io.tx = (fun != NULL) ? fun:NULL;
}
__INLINE void ds2431_link_ioctl (ds2431_t *ds2431, ds2431_ioctl_ft fun) {
   ds2431->io.ioctl = (fun != NULL) ? fun:NULL;
}
__INLINE void ds2431_link_delay (ds2431_t *ds2431, ds2431_delay_ft fun) {
   ds2431->io.delay = (fun != NULL) ? fun:NULL;
}

/*
 * Set functions
 */
//__INLINE void ds2431_set_timeout (ds2431_t *ds2431, uint32_t to) {
//   ds2431->conf.timeout = to;
//}



/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initializes peripherals used by the DS2431 EEPROM driver.
 *
 * \param  ds2431    Pointer indicate the ds2431 data structure to use
 */
void ds2431_deinit (ds2431_t *ds2431)
{
   memset ((void*)ds2431, 0, sizeof (ds2431_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initializes peripherals used by the DS2431 EEPROM driver.
 *
 * \param  ds2431    Pointer indicate the ds2431 data structure to use
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ds2431_init (ds2431_t *ds2431)
{
   #define _bad_link(_link)   (!ds2431->io._link) ? 1:0

   if (_bad_link (ow))     return ds2431->status = DRV_ERROR;
   if (_bad_link (rx))     return ds2431->status = DRV_ERROR;
   if (_bad_link (tx))     return ds2431->status = DRV_ERROR;
   if (_bad_link (ioctl))  return ds2431->status = DRV_ERROR;
   if (_bad_link (delay))  return ds2431->status = DRV_ERROR;

   if (ds2431->status == DRV_BUSY || ds2431->status == DRV_NODEV)
      return ds2431->status = DRV_ERROR;

   ds2431->status = DRV_BUSY;

//   if (!ds2431->conf.timeout)   ds2431->conf.timeout = DS2431_DEFAULT_TIMEOUT;

   return ds2431->status = DRV_READY;
   #undef _bad_link
}

/*!
 * \brief
 *    Reads a number of bytes from a specific address in EEPROM
 * \param ds2431  Pointer indicate the ds2431 data structure to use
 * \param add     The EEPROM address to read from
 * \param buf     Pointer to data buffer to store the data from EEPROM
 * \param n       The size of data to read in bytes
 * \return     The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
__INLINE drv_status_en  ds2431_read (ds2431_t *ds2431, address_t add, byte_t *buf, bytecount_t n) {
   return _read_memory (ds2431, add, buf, n);
}

/*!
 * \brief
 *    Writes a number of bytes to a specific address in EEPROM
 * \param ds2431  Pointer indicate the ds2431 data structure to use
 * \param add     The EEPROM address to write to
 * \param buf     Pointer to data buffer to store to the EEPROM
 * \param n       The size of data to write in bytes
 * \return     The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ds2431_write (ds2431_t *ds2431, address_t add, byte_t *buf, bytecount_t n)
{
   uint32_t wb=0;    // The written bytes
   int      ret;

   do {
      ret = _write_row (ds2431, add+wb, &buf[wb], n-wb);
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

drv_status_en ds2431_ioctl (ds2431_t *ds2431, ioctl_cmd_t cmd, ioctl_buf_t buf);
