/*!
 * \file ee_i2c.h
 * \brief
 *    A target independent EEPROM (24xx series) driver using i2c
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Christos Choutouridis (http://www.houtouridis.net)
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
#ifndef __ee_i2c_h__
#define __ee_i2c_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <com/i2c_bb.h>
/*
 * =================== User Defines =====================
 */

/* ================   General Defines   ====================*/
#define EE_WRITE     (0x0)
#define EE_READ      (0x1)

#define EE_PAGE_SZ_DEF        (64)           // 64 bytes
#define EE_SECTOR_SIZE_DEF    (512)          // 512 bytes

/* ================    General Types    ====================*/
typedef enum {
   EE_08,
   EE_16,
   EE_32,
   EE_128,
   EE_256
} ee_size_en;


typedef struct
{
   void*             i2c;        /*!< void I2C type structure - NULL for hardware I2C */
   drv_i2c_rx_ft     i2c_rx;     /*!< I2C read function */
   drv_i2c_tx_ft     i2c_tx;     /*!< I2C write function */
   drv_i2c_ioctl_ft  i2c_ioctl;  /*!< I2C ioctl function */
}ee_io_t;

typedef volatile struct
{
   address_t      hw_addr;    /*!< I2C hardware address function */
   ee_size_en     size;
   uint32_t       page_size;
   uint32_t       sector_size;   /*!< The virtual sector size, used in file systems */
   uint32_t       timeout;
}ee_conf_t;

typedef volatile struct
{
   ee_io_t        io;
   ee_conf_t      conf;
   drv_status_en  status;
}ee_t;



/*
 *  ============= PUBLIC EE API =============
 */

/*
 * Link and Glue functions
 */
void ee_link_i2c (ee_t *ee, void* i2c);
void ee_link_i2c_rx (ee_t *ee, drv_i2c_rx_ft fun);
void ee_link_i2c_tx (ee_t *ee, drv_i2c_tx_ft fun);
void ee_link_i2c_ioctl (ee_t *ee, drv_i2c_ioctl_ft fun);

/*
 * Set functions
 */
void ee_set_hwaddress (ee_t *ee, address_t add) ;
void ee_set_size (ee_t *ee, ee_size_en s);
void ee_set_page_size (ee_t *ee, uint32_t ps);
void ee_set_sector_size (ee_t *ee, uint32_t ss);
void ee_set_timeout (ee_t *ee, uint32_t to);

/*
 * User Functions
 */
void ee_deinit (ee_t *ee);          /*!< for compatibility */
drv_status_en ee_init (ee_t *ee);   /*!< for compatibility */

drv_status_en ee_read_cursor (ee_t *ee, byte_t *byte);
drv_status_en   ee_read_byte (ee_t *ee, address_t add, byte_t *byte);
drv_status_en  ee_write_byte (ee_t *ee, address_t add, byte_t byte);

drv_status_en  ee_read (ee_t *ee, address_t add, byte_t *buf, bytecount_t n);
drv_status_en ee_write (ee_t *ee, address_t add, byte_t *buf, bytecount_t n);

drv_status_en  ee_read_sector (ee_t *ee, int sector, byte_t *buf, int count);
drv_status_en ee_write_sector (ee_t *ee, int sector, byte_t *buf, int count);

drv_status_en       ee_ioctl (ee_t *ee, ioctl_cmd_t cmd, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __ee_i2c_pol_h__
