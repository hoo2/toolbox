/*!
 * \file sim_ee.c
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


#include <drv/sim_ee.h>

static see_page_en    _valid_page (see_t *see);
static see_status_en  _try_read (see_t *see, see_add_t page, see_index_t idx, see_data_t *d);
static see_status_en  _try_write (see_t *see, see_add_t page, see_index_t idx, see_data_t *d);
static see_status_en  _erase_page (see_t *see, see_add_t address);
static see_status_en  _page_swap (see_t *see);
static see_status_en  _format (see_t *see);

#define _EE_EMULATED_SIZE ( see->page_size / (sizeof(see_data_t) + sizeof(see_index_t)) )

/*!
 * \brief  Indicates the current valid page
 * \param  none
 * \return The current page
 *    \arg EE_PAGE0
 *    \arg EE_PAGE1
 */
static see_page_en _valid_page (see_t *see)
{
   if ( *(see_add_t*)see->page0_add == EE_PAGE_ACTIVE)
      return EE_PAGE0;
   else
      return EE_PAGE1;
}

/*!
 * \brief
 *    Try to find idx in page and read the data and size.
 *
 * \param  page   Which page to seak
 * \param  idx    What virtual address
 * \param  d      Pointer to store the data
 * \return        The status of operation
 *    \arg EE_NODATA
 *    \arg EE_SUCCESS
 */
static see_status_en _try_read (see_t *see, see_add_t page, see_index_t idx, see_data_t *d)
{
   see_add_t   fp;
   see_index_t i;
   
   /*
    * Seek for the first data and jump from data to data after that
    * Until we find the idx.
    * Do not search Page base address (fp>page)
    */
   for (fp = page + see->page_size - sizeof(see_index_t) ; fp > page ; fp -= sizeof(see_index_t)) {
      see->fl_read (fp, (void*)&i, sizeof (see_index_t));
      if (i < (see_index_t)(-1)) {
         //not empty 0xFF..FF
         if (i == idx)  //first match
            break;
         else
            fp -= sizeof (see_data_t);
      }
   }
   // Check if we got something
   if (fp > page) {
      see->fl_read (fp-sizeof(see_data_t), (void*)d, sizeof(see_data_t));
      return EE_SUCCESS;
   }
   d = (void *)0;  //no data
   return EE_NODATA;
}

/*!
 * \brief
 *    Try to find room to write idx-data in page
 *
 * \param  page   Which page to seak
 * \param  idx    What virtual address
 * \param  d      Pointer of data to save
 * \return        The status of operation
 *    \arg EE_SUCCESS
 *    \arg EE_PAGEFULL
 *    \arg EE_FLASHERROR
 */
static see_status_en _try_write (see_t *see, see_add_t page, see_index_t idx, see_data_t *d)
{
   see_add_t      fp;
   see_index_t    i;
   see_status_en  ee_st;

   /*
    * Seek for the first data
    */
   for (fp = page + see->page_size - sizeof(see_index_t) ; fp >= page ; fp -= sizeof(see_index_t)) {
      see->fl_read (fp, (void*)&i, sizeof (see_index_t));
      if (i < (see_index_t)(-1)) {
         //not empty 0xFF..FF
         if (sizeof(size_t) + sizeof (see_index_t) <= see->page_size+page-fp-sizeof(see_index_t)) {
            //Have room to write
            fp += sizeof(see_index_t);   //go to place 'n break
            break;
         }
         else
            return EE_PAGEFULL;
      }
   }
   /* 
    * - Unlock first
    * - Write the data
    * - Write virtual address last (at the end)
    */
   see->fl_ioctl (CTRL_CMD_UNLOCK, (void*)0);
   ee_st = EE_SUCCESS;  //Try that or prove otherwise
   if ( see->fl_write (fp, (void*)d, sizeof(see_data_t)) != DRV_READY )
      ee_st = EE_FLASHERROR;
   fp += sizeof(see_data_t);
   if ( see->fl_write (fp, (void*)&idx, sizeof(see_index_t)) != DRV_READY )
      ee_st = EE_FLASHERROR;
   // Lock and return
   see->fl_ioctl (CTRL_CMD_LOCK, (void*)0);
   return ee_st;
}

/*!
 * \brief
 *    Erase see page
 *
 * \param  none
 * \return The status of operation
 *    \arg EE_FLASHERROR
 *    \arg EE_SUCCESS
 */
static see_status_en  _erase_page (see_t *see, see_add_t address)
{
   see_add_t add;
   int nop = see->page_size/see->flash_page_size;

   while (nop--) {
      add = address + nop*see->flash_page_size;
      if ( see->fl_ioctl (CTRL_ERASE_PAGE, (ioctl_buf_t*)&add) != DRV_READY )
         return EE_FLASHERROR;
   }
   return EE_SUCCESS;
}

/*!
 * \brief
 *    Copy each variable instance from a full page to the emty one
 *
 * \param  none
 * \return The status of operation
 *    \arg EE_FLASHERROR
 *    \arg EE_SUCCESS
 */
static see_status_en _page_swap (see_t *see)
{
   see_add_t      from, to;
   see_data_t     status;
   see_data_t     data;
   see_index_t    idx;
   see_status_en  ee_st;
 
   // From - To dispatcher
   if ( _valid_page (see) == EE_PAGE0 ) {
      from = see->page0_add;
      to   = see->page1_add;
   } else {
      from = see->page1_add;
      to   = see->page0_add;
   }

   // Unlock first
   see->fl_ioctl (CTRL_CMD_UNLOCK, (ioctl_buf_t*)0);
   // Mark the new Page as RECEIVEDATA
   if ( _erase_page (see, to) != EE_SUCCESS )
      return EE_FLASHERROR;
   status = EE_PAGE_RECEIVEDATA;
   if ( see->fl_write (to, (void*)&status, sizeof(status)) != 0 )
      return EE_FLASHERROR;

   // Copy each idx written on "from" page to their new home
   for (idx=0 ; idx<_EE_EMULATED_SIZE ; ++idx) {
      ee_st = _try_read (see, from, idx, &data);
      if (ee_st == EE_SUCCESS) {
         ee_st = _try_write (see, to, idx, &data);
         if (ee_st != EE_SUCCESS)
            break;
      } else if (ee_st == EE_NODATA)
         continue;
      else
         break;
   }
   // Catch exit status
   switch (ee_st)
   {
      case EE_EEFULL:
      case EE_PAGEFULL:
      case EE_FLASHERROR:  return ee_st;
      case EE_NODATA:
      case EE_SUCCESS:     break;
   }

   /* 
    * - Unlock
    * - Erase old page (auto status as EMPTY = 0xFFFF)
    * - Mark old page as EMPTY
    * - Re - Mark new as ACTIVE (write 0xAAAA on top of 0xFFFF)
    * - Lock Flash before return
    */ 
   see->fl_ioctl (CTRL_CMD_UNLOCK, (void*)0);
   ee_st = EE_SUCCESS;  //Try that or prove otherwise
   if ( _erase_page (see, from) != EE_SUCCESS )
      ee_st = EE_FLASHERROR;

   status = EE_PAGE_EMPTY;     // Mark the new Page as EMPTY
   if ( see->fl_write (from, (void*)&status, sizeof(status)) != 0 )
      ee_st = EE_FLASHERROR;

   // Mark the new Page as ACTIVE
   status = EE_PAGE_ACTIVE;
   if ( see->fl_write (to, (void*)&status, sizeof(status)) != 0 )
      ee_st = EE_FLASHERROR;

   see->fl_ioctl (CTRL_CMD_LOCK, (void*)0);
   return ee_st;
}

/*!
 * \brief
 *    Erases PAGE0 and PAGE1 and writes EE_PAGE_ACTIVE
 *    and EE_PAGE_EMPTY to PAGE0,1
 * \param  None
 * \return Status of operation
 *    \arg EE_FLASHERROR
 *    \arg EE_SUCCESS
 */
static see_status_en  _format (see_t *see)
{
   see_data_t     page0_st, page1_st;
   see_status_en  ee_st;
   int            nop = see->page_size/see->flash_page_size;

   see->fl_ioctl (CTRL_CMD_UNLOCK, (void*)0);
   ee_st = EE_SUCCESS;     //Try that or prove otherwise

   while (nop--) {
      if ( _erase_page (see, see->page0_add + nop*see->flash_page_size) != EE_SUCCESS ) {
         ee_st = EE_FLASHERROR;
         break;
      }
      if ( _erase_page (see, see->page1_add + nop*see->flash_page_size) != EE_SUCCESS ) {
         ee_st = EE_FLASHERROR;
         break;
      }
   }
   if (ee_st!=EE_FLASHERROR) {
      page0_st = EE_PAGE_ACTIVE;
      page1_st = EE_PAGE_EMPTY;

      if ( see->fl_write (see->page0_add, &page0_st, sizeof(page0_st)) != 0)
         ee_st = EE_FLASHERROR;
      if ( see->fl_write (see->page1_add, &page1_st, sizeof(page1_st)) != 0)
         ee_st = EE_FLASHERROR;
   }

   see->fl_ioctl (CTRL_CMD_LOCK, (void*)0);
   return ee_st;
}

/*
 * ========== Public Simulated EE API ================
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief  Link the driver flash read function
 * \param  see   The active see struct.
 * \param  f     The read function
 * \return none
 */
inline void see_link_flash_read (see_t *see, fl_io_ft f) {
   see->fl_read = f;
}

/*!
 * \brief  Link the driver flash write function
 * \param  see   The active see struct.
 * \param  f     The write function
 * \return none
 */
inline void see_link_flash_write (see_t *see, fl_io_ft f) {
   see->fl_write = f;
}

/*!
 * \brief  Link the driver flash ioctl function
 * \param  see   The active see struct.
 * \param  f     The ioctl function
 * \return none
 */
inline void see_link_flash_ioctl (see_t *see, fl_ioctl_ft f) {
   see->fl_ioctl = f;
}

/*
 * Set functions
 */

/*!
 * \brief  Set PAGE0 address
 * \param   see   The active see struct.
 * \param  address   The targets PAGE0 address
 * \return none
 */
inline void see_set_page0_add (see_t *see, see_add_t address) {
   see->page0_add = address;
}

/*!
 * \brief  Set PAGE1 address
 * \param   see   The active see struct.
 * \param  address   The targets PAGE0 address
 * \return none
 */
inline void see_set_page1_add (see_t *see, see_add_t address) {
   see->page1_add = address;
}

/*!
 * \brief  Set virtual PAGE0/1 size
 * \param   see   The active see struct.
 * \param  size   The size
 * \return none
 */
void see_set_page_size (see_t *see, uint32_t size) {
   see->page_size = size;
}

/*!
 * \brief  Set the targets actual flash page size
 * \param   see   The active see struct.
 * \param  size   The size
 * \return none
 */
void see_set_flash_page_size (see_t *see, uint32_t size) {
   see->flash_page_size = size;
}


/*
 * User Functions
 */

/*!
 * \brief
 *    De-initialise the see
 * \param   see   The active see struct.
 * \return  none
 */
void see_deinit (see_t *see)
{
   memset ((void*)see, 0, sizeof (see_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Restore the pages to a known good state in case of page's status
 *    corruption after a power loss.
 *
 * The possibilities are:
 *  1) Pages have the same status
 *    Invalid state, Format :(
 *  2) One page is ACTIVE and one EMPTY
 *       Normal state, do nothing
 *  3) One pare is EE_PAGE_RECEIVEDATA and one EE_PAGE_EMPTY
 *       Power failure during PageSwap just before marking ACTIVE the new page.
 *       We just mark as active the new page.
 *  4) One page is EE_PAGE_ACTIVE and one EE_PAGE_RECEIVEDATA
 *        Power failure during PageSwap before finishing the copy. The data are
 *        intact in old page (Still active). We Re-call the page swap procedure.
 *
 * \param   see   The active see struct.
 * \return The status
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en see_init (see_t *see)
{
   drv_status_en        drv_st;
   see_status_en        ee_st;
   see_page_status_en   PageStatus0, PageStatus1;
   see_data_t           page_st;

   if (!see->fl_ioctl)  return see->status = DRV_ERROR;
   if (!see->fl_read)   return see->status = DRV_ERROR;
   if (!see->fl_write)  return see->status = DRV_ERROR;

   see->status = DRV_NOINIT;
   PageStatus0 = (see_page_status_en)*( see_data_t*)see->page0_add;
   PageStatus1 = (see_page_status_en)*( see_data_t*)see->page1_add;

   if (PageStatus0 == PageStatus1) {
      //Invalid state, Format
      _format (see);
      see->status = DRV_READY;
      return see->status;
   }
   /*
    *  Normal state. Do nothing
    */
   else if (PageStatus0 == EE_PAGE_ACTIVE && PageStatus1 == EE_PAGE_EMPTY) {
      see->status = DRV_READY;
      return see->status;
   }
   else if (PageStatus0 == EE_PAGE_EMPTY && PageStatus1 == EE_PAGE_ACTIVE) {
      see->status = DRV_READY;
      return see->status;
   }
   /*
    * Power failure during PageSwap just before marking ACTIVE the new page.
    * We just mark as active the new page.
    */
   else if (PageStatus0 == EE_PAGE_RECEIVEDATA && PageStatus1 == EE_PAGE_EMPTY)
   {
      see->fl_ioctl (CTRL_CMD_UNLOCK, (void*)0);
      drv_st = DRV_READY;     //Try that or prove otherwise
      page_st = EE_PAGE_ACTIVE;
      if ( see->fl_write (see->page0_add, (void*)&page_st, sizeof(page_st)) != DRV_READY)
         drv_st = DRV_ERROR;
      see->fl_ioctl (CTRL_CMD_LOCK, (void*)0);
      return (see->status = drv_st);
   }
   else if (PageStatus0 == EE_PAGE_EMPTY && PageStatus1 == EE_PAGE_RECEIVEDATA)
   {
      see->fl_ioctl (CTRL_CMD_UNLOCK, (void*)0);
      drv_st = DRV_READY;     //Try that or prove otherwise
      page_st = EE_PAGE_ACTIVE;
      if ( see->fl_write (see->page1_add, &page_st, sizeof(page_st)) != DRV_READY)
         drv_st = DRV_ERROR;
      see->fl_ioctl (CTRL_CMD_LOCK, (void*)0);
      return (see->status = drv_st);
   }
   /*
    * Power failure during PageSwap before finishing the copy.
    * The data are intact in old page (Still active).
    * We Re-call the page swap procedure.
    */
   else if (PageStatus0 == EE_PAGE_ACTIVE && PageStatus1 == EE_PAGE_RECEIVEDATA)
      ee_st = _page_swap (see);
   else if (PageStatus0 == EE_PAGE_RECEIVEDATA && PageStatus1 == EE_PAGE_ACTIVE)
      ee_st = _page_swap (see);

   if (ee_st == EE_SUCCESS)   return see->status = DRV_READY;
   else                       return see->status = DRV_ERROR;
}                                      

/*!
 * \brief
 *    Try to read data from EEPROM to the pointer d, for data.
 *    If there isn't data with index a in EEPROM, the pointers d and s get NULL
 * \param  see    The active see struct.
 * \param  idx    The virtual address(index) of data
 * \param  d      Pointer to data
 * \retval Status
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en see_read (see_t *see, see_index_t idx, see_data_t *d)
{
   see_add_t page;

   // From - To dispatcher
   if ( _valid_page (see) == EE_PAGE0 )   page = see->page0_add;
   else                                   page = see->page1_add;

   if ( _try_read (see, page, idx, d) == EE_SUCCESS)
      return see->status = DRV_READY;
   else
      return see->status = DRV_ERROR;
}

/*!
 * \brief
 *    Try to write data to EEPROM. The data are d with size s.
 *    If there isn't room in EEPROM, return EE_EEFULL
 * \param  see  The active see struct.
 * \param  idx  The virtual address(index) of data
 * \param  d    Pointer to data
 * \return Status
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en see_write (see_t *see, see_index_t idx, see_data_t *d)
{
   see_add_t page;
   see_status_en ee_st;

   // From - To dispatcher
   if ( _valid_page (see) == EE_PAGE0 )   page = see->page0_add;
   else                                   page = see->page1_add;

   /*
    * Try to write the data.
    * If the page is full, swap the page and try again.
    * If both pages are full, then the EEPROM is full
    */
   ee_st = _try_write (see, page, idx, d);
   if ( ee_st == EE_PAGEFULL) {
      if (_page_swap (see) == EE_FLASHERROR)
         return see->status = DRV_ERROR;
      if (page == see->page0_add)
         page = see->page1_add;
      else
         page = see->page0_add;

      ee_st = _try_write (see, page, idx, d);
      if ( ee_st == EE_PAGEFULL)
         return see->status = DRV_ERROR;
      else
         return see->status = DRV_READY;
   }
   else if (ee_st == EE_FLASHERROR)
      return see->status = DRV_ERROR;
   else
      return see->status = DRV_READY;
}

/*!
 * \brief
 *    Simulated EEPROM ioctl function
 *
 * \param  see    The active see struct.
 * \param  cmd   specifies the command to FLASH
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_LOCK
 *    \arg CTRL_CMD_LOCK
 *    \arg CTRL_CMD_UNLOCK
 *    \arg CTRL_FORMAT
 * \param  buf   pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en see_ioctl (see_t *see, ioctl_cmd_t cmd, ioctl_buf_t *buf)
{
   switch (cmd)
   {
      case CTRL_GET_STATUS:      /*!< Probe function */
         *(drv_status_en*)buf = see->status;
         return see->status = DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
         see_deinit(see);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         return see_init(see);   // status assignment implied
      case CTRL_LOCK:
      case CTRL_CMD_LOCK:
      case CTRL_CMD_UNLOCK:      /*!< Forward call to linked driver */
         see->fl_ioctl (cmd, buf);
         return see->status = DRV_READY;
      case CTRL_FORMAT:          /*!< Format flash */
         if (_format(see) == EE_FLASHERROR ) return see->status = DRV_ERROR;
         else                                return see->status = DRV_READY;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}

#undef _EE_EMULATED_SIZE
