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
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ================   User Defines   ====================*/


/* ================   General Defines   ====================*/
                                                                
typedef  uint32_t       see_index_t;
typedef  uint32_t       see_add_t;
typedef  uint32_t       see_data_t;

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

typedef drv_status_en (*fl_io_ft) (uint32_t , void*, int);   /*!< Flash I/O function pointer */
typedef drv_status_en (*fl_ioctl_ft) (ioctl_cmd_t, ioctl_buf_t *);   /*!< Flash io control function pointer */

typedef struct {
   fl_io_ft    fl_read;       /*!< Link to FLASH read function */
   fl_io_ft    fl_write;      /*!< Link to FLAH write function */
   fl_ioctl_ft fl_ioctl;      /*!< Link to FLASH io control function */
   see_add_t   page0_add;     /*!< The PAGE0 address, or else the starting address of see */
   see_add_t   page1_add;     /*!< The PAGE1 address */
   uint32_t    page_size;     /*!< The size of each pare */
   uint32_t    flash_page_size;  /*!< The target flash page size */
   uint32_t    size;          /*!< The emulated size of the EEPROM */
   drv_status_en status;
}see_t;

/*
 * ========== Public Simulated EE API ================
 */

/*
 * Link and Glue functions
 */
void see_link_flash_read (see_t *see, fl_io_ft f);
void see_link_flash_write (see_t *see, fl_io_ft f);
void see_link_flash_ioctl (see_t *see, fl_ioctl_ft f);

/*
 * Set functions
 */
void see_set_page0_add (see_t *see, see_add_t address);
void see_set_page1_add (see_t *see, see_add_t address);
void see_set_page_size (see_t *see, uint32_t size);
void see_set_flash_page_size (see_t *see, uint32_t size);

/*
 * User Functions
 */
void see_deinit (see_t *see);          /*!< For compatibility */
drv_status_en see_init (see_t *see);   /*!< For compatibility */

drv_status_en see_read_word (see_t *see, see_index_t idx, see_data_t *d);
drv_status_en see_read (see_t *see, see_index_t idx, see_data_t *d, size_t size);
drv_status_en see_write_word (see_t *see, see_index_t idx, see_data_t *d);
drv_status_en see_write (see_t *see, see_index_t idx, see_data_t *d, size_t size);
drv_status_en see_ioctl (see_t *see, ioctl_cmd_t cmd, ioctl_buf_t *buf);

#endif   //#ifndef __sim_ee_h__
