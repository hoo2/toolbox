/*!
 * \file ds2431.h
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
#ifndef __ds2431_h__
#define __ds2431_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_types.h>
#include <algo/crc.h>
#include <string.h>

/*
 * =================== User Defines =====================
 */
#define  DS2431_DEFAULT_TIMEOUT     (1000)
#define  DS2431_TPROG               (10)     // 10 [msec]


/* ================   General Defines   ====================*/

#define  DS2431_SCRATCHPAD_SIZE     (8)      // 8 bytes
#define  DS2431_MAX_SIZE            (128)    // 128 bytes

/*
 * DS2431 Commands
 */
#define DS2431_WRITESCRATCH         (0x0F)
#define DS2431_READSCRATCH          (0xAA)
#define DS2431_COPYSCRATCH          (0x55)
#define DS2431_READMEM              (0xF0)
#define DS2431_READROM              (0x33)
#define DS2431_MATCHROM             (0x55)
#define DS2431_SEARCHROM            (0xF0)
#define DS2431_SKIPROM              (0xCC)
#define DS2431_RESUME               (0xA5)
#define DS2431_OVERDRIVESKIP        (0x3C)
#define DS2431_OVERDRIVEMATCH       (0x69)


/*
 * ================    General Types    ====================
 */
typedef byte_t (*ds2431_rx_ft) (void *);
typedef void   (*ds2431_tx_ft) (void *, byte_t);
typedef drv_status_en (*ds2431_ioctl_ft) (void *, ioctl_cmd_t, ioctl_buf_t);
typedef void (*ds2431_delay_ft) (void);

typedef struct {
   uint8_t  TA1;
   uint8_t  TA2;
   uint8_t  ES;
}ds2431_ar_t;

#define  DS2431_AR_AA_MASK       (0x80)
#define  DS2431_AR_PF_MASK       (0x20)
#define  DS2431_AR_EMASK         (0x07)

typedef struct {
   void*             ow;         /*!< void 1-wire type structure - NULL for hardware 1-wire */
   ds2431_rx_ft      rx;         /*!< 1-wire read function */
   ds2431_tx_ft      tx;         /*!< 1-wire write function */
   ds2431_ioctl_ft   ioctl;      /*!< 1-wire ioctl function */
   ds2431_delay_ft   delay;      /*!< A delay function for T prog */
}ds2431_io_t;

typedef struct {
   byte_t         romid[8];      /*!< ROMID of the chip */
//   uint32_t       timeout;
   enum {
      _1WBUS_SINGLEDROP=0,
      _1WBUS_MULTIDROP
   }bus;
}ds2431_conf_t;

typedef struct {
   ds2431_io_t    io;
   ds2431_conf_t  conf;
   drv_status_en  status;
}ds2431_t;



/*
 *  ============= PUBLIC DS2431 API =============
 */

/*
 * Link and Glue functions
 */
void ds2431_link_ow (ds2431_t *ds2431, void* ow);
void ds2431_link_rx (ds2431_t *ds2431, ds2431_rx_ft fun);
void ds2431_link_tx (ds2431_t *ds2431, ds2431_tx_ft fun);
void ds2431_link_ioctl (ds2431_t *ds2431, ds2431_ioctl_ft fun);
void ds2431_link_delay (ds2431_t *ds2431, ds2431_delay_ft fun);

/*
 * Set functions
 */
//void ds2431_set_timeout (ds2431_t *ds2431, uint32_t to);

/*
 * User Functions
 */
void ds2431_deinit (ds2431_t *ds2431);          /*!< for compatibility */
drv_status_en ds2431_init (ds2431_t *ds2431);   /*!< for compatibility */

drv_status_en  ds2431_read (ds2431_t *ds2431, address_t add, byte_t *buf, bytecount_t n);
drv_status_en ds2431_write (ds2431_t *ds2431, address_t add, byte_t *buf, bytecount_t n);
drv_status_en ds2431_ioctl (ds2431_t *ds2431, ioctl_cmd_t cmd, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __ds2431_h__
