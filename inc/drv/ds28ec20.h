/*!
 * \file ds28ec20.h
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
#ifndef __ds28ec20_h__
#define __ds28ec20_h__

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
#define  DS28EC20_DEFAULT_TIMEOUT   (1000)
#define  DS28EC20_TPROG             (10)     // 10 [msec]


/* ================   General Defines   ====================*/

#define  DS28EC20_SCRATCHPAD_SIZE   (32)     /*!< 8 byte */
#define  DS28EC20_MAX_SIZE          (0xA00)  // 2560 bytes

/*
 * DS2431 Commands
 */
#define DS28EC20_WRITESCRATCH       (0x0F)
#define DS28EC20_READSCRATCH        (0xAA)
#define DS28EC20_COPYSCRATCH        (0x55)
#define DS28EC20_READMEM            (0xF0)
#define DS28EC20_READROM            (0x33)
#define DS28EC20_MATCHROM           (0x55)
#define DS28EC20_SEARCHROM          (0xF0)
#define DS28EC20_SKIPROM            (0xCC)
#define DS28EC20_RESUME             (0xA5)
#define DS28EC20_OVERDRIVESKIP      (0x3C)
#define DS28EC20_OVERDRIVEMATCH     (0x69)


/*
 * ================    General Types    ====================
 */
typedef byte_t (*ds28ec20_rx_ft) (void *);
typedef void   (*ds28ec20_tx_ft) (void *, byte_t);
typedef drv_status_en (*ds28ec20_ioctl_ft) (void *, ioctl_cmd_t, ioctl_buf_t);
typedef void (*ds28ec20_delay_ft) (void);

typedef struct {
   uint8_t  TA1;
   uint8_t  TA2;
   uint8_t  ES;
}ds28ec20_ar_t;

#define  DS28EC20_AR_AA_MASK     (0x80)
#define  DS28EC20_AR_PF_MASK     (0x20)
#define  DS28EC20_AR_EMASK       (0x07)

typedef struct {
   void*                ow;         /*!< void 1-wire type structure - NULL for hardware 1-wire */
   ds28ec20_rx_ft       rx;         /*!< 1-wire read function */
   ds28ec20_tx_ft       tx;         /*!< 1-wire write function */
   ds28ec20_ioctl_ft    ioctl;      /*!< 1-wire ioctl function */
   ds28ec20_delay_ft    delay;      /*!< A delay function for T prog */
}ds28ec20_io_t;

typedef struct {
   byte_t         romid[8];      /*!< ROMID of the chip */
//   uint32_t       timeout;
   enum {
      DS28EC20_1WBUS_SINGLEDROP=0,
      DS28EC20_1WBUS_MULTIDROP
   }bus;
}ds28ec20_conf_t;

typedef struct {
   ds28ec20_io_t     io;
   ds28ec20_conf_t   conf;
   drv_status_en     status;
}ds28ec20_t;



/*
 *  ============= PUBLIC DS2431 API =============
 */

/*
 * Link and Glue functions
 */
void ds28ec20_link_ow (ds28ec20_t *ds28ec20, void* ow);
void ds28ec20_link_rx (ds28ec20_t *ds28ec20, ds28ec20_rx_ft fun);
void ds28ec20_link_tx (ds28ec20_t *ds28ec20, ds28ec20_tx_ft fun);
void ds28ec20_link_ioctl (ds28ec20_t *ds28ec20, ds28ec20_ioctl_ft fun);
void ds28ec20_link_delay (ds28ec20_t *ds28ec20, ds28ec20_delay_ft fun);

/*
 * Set functions
 */
//void ds28ec20_set_timeout (ds28ec20_t *ds28ec20, uint32_t to);

/*
 * User Functions
 */
void ds28ec20_deinit (ds28ec20_t *ds28ec20);          /*!< for compatibility */
drv_status_en ds28ec20_init (ds28ec20_t *ds28ec20);   /*!< for compatibility */

drv_status_en ds28ec20_read (ds28ec20_t *ds28ec20, address_t add, byte_t *buf, bytecount_t n);
drv_status_en ds28ec20_write (ds28ec20_t *ds28ec20, address_t add, byte_t *buf, bytecount_t n);
drv_status_en ds28ec20_ioctl (ds28ec20_t *ds28ec20, ioctl_cmd_t cmd, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __ds28ec20_h__
