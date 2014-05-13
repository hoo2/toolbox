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
 */
#ifndef __ee_i2c_pol_h__
#define __ee_i2c_pol_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <drv/i2c_pol.h>

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
}ee_t;

typedef uint32_t  idx_t;



/*
 *  ============= PUBLIC ALCD API =============
 */

/*
 * Link and Glue functions
 */
void ee_link_i2c_start (ee_t *ee);
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
/* Init / De-Init */
void ee_deinit (ee_t *ee);
ee_status_en ee_init (ee_t *ee);

/* I/O Operations */
ee_status_en ee_read (ee_t *ee, uint8_t *byte);
ee_status_en ee_readbyte (ee_t *ee, uint8_t *byte, idx_t add);
ee_status_en ee_readbuffer (ee_t *ee, uint8_t* buf, uint32_t num, idx_t add);
ee_status_en ee_writebyte (ee_t *ee, uint8_t byte, idx_t add);
ee_status_en ee_writebuffer (ee_t *ee, uint8_t *buf, uint32_t num, idx_t add);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __ee_i2c_pol_h__
