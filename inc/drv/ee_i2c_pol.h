/*!
 * \file ee_i2c_pol.h
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
#ifndef __ee_i2c_pol_h__
#define __ee_i2c_pol_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <drv/i2c_pol.h>
#include <tbx_ioctl.h>

/* ================   General Defines   ====================*/
#define EE_WRITE     (0x0)
#define EE_READ      (0x1)


/* ================    General Types    ====================*/
typedef enum {EE_OK, EE_ERROR} ee_status_en;
typedef enum {EE_08, EE_16, EE_32, EE_128, EE_256} ee_size_en;

typedef volatile struct
{
   int            hw_addr;
   ee_size_en     size;
   int            pagesize;
   int            freq;
   uint32_t       timeout;
   i2c_pol_t      i2c;
   drv_status_en  status;
}ee_t;

typedef uint32_t  ee_idx_t;



/*
 *  ============= PUBLIC EE API =============
 */

/*
 * Link and Glue functions
 */

/*
 * Set functions
 */
void ee_set_hwaddress (ee_t *ee, int add) ;
void ee_set_size (ee_t *ee, int s);
void ee_set_pagesize (ee_t *ee, int ps);
void ee_set_speed (ee_t *ee, int freq);
void ee_set_timeout (ee_t *ee, uint32_t to);

/*
 * User Functions
 */
void ee_deinit (ee_t *ee);          /*!< for compatibility */
ee_status_en ee_init (ee_t *ee);    /*!< for compatibility */

drv_status_en        ee_read (ee_t *ee, uint8_t *byte);
drv_status_en    ee_readbyte (ee_t *ee, ee_idx_t add, uint8_t *byte);
drv_status_en  ee_readbuffer (ee_t *ee, ee_idx_t add, uint8_t *buf, size_t n);
drv_status_en   ee_writebyte (ee_t *ee, ee_idx_t add, uint8_t byte);
drv_status_en ee_writebuffer (ee_t *ee, ee_idx_t add, uint8_t *buf, size_t n);

drv_status_en       ee_ioctl (ee_t *ee, ioctl_cmd_t cmd, ioctl_buf_t *buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __ee_i2c_pol_h__
