/*!
 * \file sim_ee.h
 * \brief
 *    A target independent simulated EEPROM functionality. The algorithm use
 *    a flash API, and 2 flash regions in order to simulate an eeprom behaviour.
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
#ifndef __sim_ee_h__
#define __sim_ee_h__

#include <tbx_ioctl.h>
#include <tbx_types.h>
#include <toolbox_defs.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/*
 * ================   User Defines   ====================
 */

/*!
 * Is the size of each virtual word in SEE
 */
#define SEE_MAX_WORD_SIZE           (8)      // 8 bytes
#define SEE_FIND_LAST_BUFFER_SIZE   (64)

/*!
 * Use this define to select 16bit addressing scheme (max 64KBytes)
 */
//#define SEE_16BIT_ADDRESSING

/*!
 * Use this define to select 32bit addressing scheme (max 4TBytes)
 */
#define SEE_32BIT_ADDRESSING



/*
 * ================   General Defines   ====================
 */

#ifdef SEE_16BIT_ADDRESSING
typedef  uint16_t    see_idx_t;     /*!< SEE byte addressing */
#endif

#ifdef SEE_32BIT_ADDRESSING
typedef  uint32_t    see_idx_t;     /*!< SEE byte addressing */
#endif
/*!<
 * \note
 *    The virtual EEPROM is byte addressed ONLY. This is true even
 *    if the word size is different. So The actual idx written
 *    in flash media is in products of word_size but we can read any
 *    idx address! ;-)
 */



typedef enum {
   EE_SUCCESS = 0,
   EE_NODATA,
   EE_PAGEFULL,
   EE_FLASHERROR,
   EE_EEFULL
}see_status_en;

typedef enum {
   EE_PAGE_ACTIVE = 0,
   EE_PAGE_RECEIVEDATA = 0xAAAA,
   EE_PAGE_EMPTY = 0xFFFF
}see_page_status_en;

typedef enum {
   EE_PAGE0, EE_PAGE1
}see_page_en;

typedef drv_status_en (*fl_io_ft) (void*, see_idx_t, void*, int);        /*!< Flash I/O function pointer */
typedef drv_status_en (*fl_ioctl_ft) (void*, ioctl_cmd_t, ioctl_buf_t); /*!< Flash io control function pointer */

/*!
 * The drivers link data struct.
 */
typedef volatile struct {
   void *      flash;         /*!< void flash type structure - NULL for hardware or simple flash implementations */
   fl_io_ft    fl_read;       /*!< Link to FLASH read function */
   fl_io_ft    fl_write;      /*!< Link to FLAH write function */
   fl_ioctl_ft fl_ioctl;      /*!< Link to FLASH io control function */
}see_io_t;

/*!
 * The see configuration and settings struct
 */
typedef volatile struct {
   see_idx_t   page0_add;     /*!< The PAGE0 address, or else the starting address of see */
   see_idx_t   page1_add;     /*!< The PAGE1 address */
   uint32_t    page_size;     /*!< The size of each page0, page1 */
   uint32_t    fl_sector_size;  /*!< The target flash page size (usualy erase sector size)*/
}see_conf_t;

/*!
 * The see interface type
 */
typedef volatile struct {
   uint8_t     word_size;     /*!< The simulated word size in bytes */
   uint32_t    size;          /*!< The simulated size of the EEPROM in bytes */
   uint32_t    sector_size;   /*!< The simulated virtual sector size to use in file systems */
}see_iface_t;

/*!
 * The see driver data type.
 */
typedef volatile struct {
   see_io_t       io;         /*!< driver links */
   see_conf_t     conf;       /*!< Configuration and settings */
   see_iface_t    iface;      /*!< Interface */
   see_idx_t      last_cur;   /*!< Holds the last write flash address of current page */
   see_idx_t      last_pr;    /*!< Holds the last write flash address of previous page */
   drv_status_en  status;     /*!< see driver status, NOT the device status */
}see_t;

/*
 * ========== Public Simulated EE API ================
 */

/*
 * Link and Glue functions
 */
void see_link_flash (see_t *see, void* flash);
void see_link_flash_read (see_t *see, fl_io_ft f);
void see_link_flash_write (see_t *see, fl_io_ft f);
void see_link_flash_ioctl (see_t *see, fl_ioctl_ft f);

/*
 * Set functions
 */
void see_set_page0_add (see_t *see, see_idx_t address);
void see_set_page1_add (see_t *see, see_idx_t address);
void see_set_page_size (see_t *see, uint32_t size);
void see_set_flash_sector_size (see_t *see, uint32_t size);
void see_set_word_size (see_t *see, uint8_t size);
void see_set_sector_size (see_t *see, uint32_t size);

/*
 * User Functions
 */
void see_deinit (see_t *see);          /*!< For compatibility */
drv_status_en see_init (see_t *see);   /*!< For compatibility */


drv_status_en see_read (see_t *see, see_idx_t idx, byte_t *buf, bytecount_t size);
drv_status_en see_write (see_t *see, see_idx_t idx, byte_t *buf, bytecount_t size);
drv_status_en see_ioctl (see_t *see, ioctl_cmd_t cmd, ioctl_buf_t buf);

#endif   //#ifndef __sim_ee_h__
