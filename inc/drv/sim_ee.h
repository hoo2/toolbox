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

#include <stdlib.h>
#include <stddef.h>
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

typedef struct {
   see_add_t page0_add;     /*!< The PAGE0 address, or else the starting address of see */
   see_add_t page1_add;     /*!< The PAGE1 address */
   uint32_t  page_size;     /*!< The size of each pare */
   uint32_t  flash_page_size;  /*!< The target flash page size */
}see_t;

/*
 * ========== Public Simulated EE API ================
 */

/*
 * Link and Glue functions
 */
extern void flash_unlock (void);
extern void flash_lock (void);

extern int flash_erase_page (uint32_t page);
extern int flash_write (uint32_t address, void *data, int size);
extern void flash_read (uint32_t address, void *data, int size);
/*!<
 * \note Tailor these function to glue sim_ee with target FLASH
 */

/*
 * Set functions
 */
void see_set_page0_add (see_add_t address);
void see_set_page1_add (see_add_t address);
void see_set_page_size (uint32_t size);
void see_set_flash_page_size (uint32_t size);

/*
 * User Functions
 */
void see_deinit (void);
see_status_en see_init (void);
see_status_en see_format (void);
see_status_en see_read (see_index_t idx, see_data_t *d);
see_status_en see_write (see_index_t idx, see_data_t *d);

#endif   //#ifndef __sim_ee_h__
