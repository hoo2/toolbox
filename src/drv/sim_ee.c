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

static see_page_en   _valid_page (see_t *see);
static see_idx_t      _find_last (see_t *see, see_idx_t page);
static see_status_en _erase_page (see_t *see, see_idx_t page);
static see_status_en  _page_swap (see_t *see);
static see_status_en     _format (see_t *see);
static see_status_en   _try_read (see_t *see, see_idx_t page, see_idx_t idx, byte_t *word);
static see_status_en  _try_write (see_t *see, see_idx_t page, see_idx_t idx, byte_t *word);

static see_status_en  _read_word (see_t *see, see_idx_t idx, byte_t *buf);
static see_status_en _write_word (see_t *see, see_idx_t idx, byte_t *buf);

/*!
 * \brief
 *    Indicates the current valid page
 * \param  see   The active see struct.
 * \return The current page
 *    \arg EE_PAGE0
 *    \arg EE_PAGE1
 */
static see_page_en _valid_page (see_t *see)
{
   see_page_status_en   PageStatus;

   see->io.fl_read (see->io.flash, see->conf.page0_add, &PageStatus, sizeof (see_page_status_en));
   if ( PageStatus == EE_PAGE_ACTIVE)
      return EE_PAGE0;
   else
      return EE_PAGE1;
}

/*!
 * \brief
 *    Try to find last written address item in flash
 *
 * \param  see    The active see struct.
 * \param  page   Which page to seek
 * \return        The flash address of the last written data
 */
static see_idx_t _find_last (see_t *see, see_idx_t page)
{
   byte_t      bf[SEE_FIND_LAST_BUFFER_SIZE];
   see_idx_t   fp;
   uint32_t    i, pairs, bts;
   uint32_t    bfp, bfp_next;
   see_idx_t   *last;
   uint8_t     seek = 0;
   static see_idx_t
               page_cur = (see_idx_t)-1,
               page_pr  = (see_idx_t)-1;

   // Job filter
   if (page_cur == (see_idx_t)-1) {
      // First time
      page_cur = page;
      seek = 1;
      last = (see_idx_t*)&see->last_cur;
   }
   else if (page == page_cur) {
      // Repeat during current read/write
      seek = 0;
      last = (see_idx_t*)&see->last_cur;
   }
   else {
      // Not current
      if (page == page_pr) {
         // Repeat for "from page" during page swap
         seek = 0;
         last = (see_idx_t*)&see->last_pr;
      }
      else {
         // New current page during page swap (seek new page)
         page_pr = page_cur;
         page_cur = page;
         seek = 1;
         see->last_pr = see->last_cur;
         last = (see_idx_t*)&see->last_cur;
      }
   }
   if (!seek)
      return *last;
   else {
      // Calculate counters
      pairs = SEE_FIND_LAST_BUFFER_SIZE / (see->iface.word_size + sizeof (see_idx_t));
      bts = pairs * (see->iface.word_size + sizeof (see_idx_t));
      fp = sizeof (see_page_status_en)+page;

      // Loop entire flash page
      for ( ; fp < page+see->conf.page_size - bts ; fp += (bfp - see->iface.word_size)) {
         // Load buffer
         if ( see->io.fl_read (see->io.flash, fp, (void*)bf, bts) != DRV_READY)
            return *last = 0;
         // Seek into buffer
         bfp=0;
         bfp_next = see->iface.word_size;
         // One action outside
         if ( *(see_idx_t*)(bf+bfp_next) == (see_idx_t)-1 )
            return *last = (see_idx_t)(fp-sizeof (see_idx_t));
         bfp = bfp_next;
         bfp_next += see->iface.word_size + sizeof (see_idx_t);
         // Loop the rest
         for (i=1 ; i<pairs ; ++i) {
            if ( *(see_idx_t*)(bf+bfp_next) == (see_idx_t)-1 )
               return *last = (see_idx_t)(fp+bfp);
            bfp = bfp_next;
            bfp_next += see->iface.word_size + sizeof (see_idx_t);
         }
      }
      return *last = 0;
   }
}

/*!
 * \brief
 *    Erase see page
 *
 * \param  see    The active see struct.
 * \param  page   The page address
 * \return The status of operation
 *    \arg EE_FLASHERROR
 *    \arg EE_SUCCESS
 */
static see_status_en  _erase_page (see_t *see, see_idx_t page)
{
   see_idx_t sector;
   int nop = see->conf.page_size/see->conf.fl_sector_size;

   while (nop--) {
      sector = page + nop*see->conf.fl_sector_size;
      if ( see->io.fl_ioctl (see->io.flash, CTRL_ERASE_PAGE, (ioctl_buf_t)&sector) != DRV_READY )
         return EE_FLASHERROR;
   }
   return EE_SUCCESS;
}

/*!
 * \brief
 *    Copy each last word instance from a full page to the empty one
 *
 * \param  see    The active see struct.
 * \return The status of operation
 *    \arg EE_FLASHERROR
 *    \arg EE_SUCCESS
 */
static see_status_en _page_swap (see_t *see)
{
   see_idx_t     from, to;
   see_page_status_en status;
   byte_t        data;
   see_idx_t     idx;
   see_status_en ee_st = EE_SUCCESS;
 
   // From - To dispatcher
   if ( _valid_page (see) == EE_PAGE0 ) {
      from = see->conf.page0_add;
      to   = see->conf.page1_add;
   } else {
      from = see->conf.page1_add;
      to   = see->conf.page0_add;
   }

   // Mark the new Page as RECEIVEDATA
   if ( _erase_page (see, to) != EE_SUCCESS )
      return EE_FLASHERROR;
   status = EE_PAGE_RECEIVEDATA;
   if ( see->io.fl_write (see->io.flash, to, (void*)&status, sizeof(status)) != DRV_READY )
      return EE_FLASHERROR;

   // Copy each word written on "from" page to their new home
   for (idx=0 ; idx<see->iface.size ; idx+=see->iface.word_size) {
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
   switch (ee_st) {
      case EE_EEFULL:
      case EE_PAGEFULL:
      case EE_FLASHERROR:  return ee_st;
      case EE_NODATA:
      case EE_SUCCESS:     break;
   }

   /* 
    * - Erase old page (auto status as EMPTY = 0xFFFF)
    * - Mark old page as EMPTY
    * - Re - Mark new as ACTIVE (write 0xAAAA on top of 0xFFFF)
    */ 
   ee_st = EE_SUCCESS;  //Try that or prove otherwise
   if ( _erase_page (see, from) != EE_SUCCESS )
      ee_st = EE_FLASHERROR;

   status = EE_PAGE_EMPTY;     // Mark the new Page as EMPTY
   if ( see->io.fl_write (see->io.flash, from, (void*)&status, sizeof(see_page_status_en)) != DRV_READY )
      ee_st = EE_FLASHERROR;

   // Mark the new Page as ACTIVE
   status = EE_PAGE_ACTIVE;
   if ( see->io.fl_write (see->io.flash, to, (void*)&status, sizeof(see_page_status_en)) != DRV_READY )
      ee_st = EE_FLASHERROR;

   return ee_st;
}

/*!
 * \brief
 *    Erases PAGE0 and PAGE1 and writes EE_PAGE_ACTIVE
 *    and EE_PAGE_EMPTY to PAGE0,1
 * \param  see    The active see struct.
 * \return Status of operation
 *    \arg EE_FLASHERROR
 *    \arg EE_SUCCESS
 */
static see_status_en  _format (see_t *see)
{
   see_page_status_en  page0_st, page1_st;
   see_status_en  ee_st;

   ee_st = EE_SUCCESS;     //Try that or prove otherwise

   if ( _erase_page (see, see->conf.page0_add) != EE_SUCCESS ) {
      ee_st = EE_FLASHERROR;
   }
   if ( _erase_page (see, see->conf.page1_add) != EE_SUCCESS ) {
      ee_st = EE_FLASHERROR;
   }

   if (ee_st!=EE_FLASHERROR) {
      page0_st = EE_PAGE_ACTIVE;
      page1_st = EE_PAGE_EMPTY;

      if ( see->io.fl_write (see->io.flash, see->conf.page0_add, (void *)&page0_st, sizeof(see_page_status_en)) != DRV_READY)
         ee_st = EE_FLASHERROR;
      if ( see->io.fl_write (see->io.flash, see->conf.page1_add, (void *)&page1_st, sizeof(see_page_status_en)) != DRV_READY)
         ee_st = EE_FLASHERROR;
   }
   return ee_st;
}

/*!
 * \brief
 *    Try to find idx in page and read the word data.
 *
 * \param  see    The active see struct.
 * \param  page   Which page to seek
 * \param  idx    What virtual address
 * \param  word   Pointer to store the data
 * \return        The status of operation
 *    \arg EE_NODATA
 *    \arg EE_SUCCESS
 *    \arg EE_FLASHERROR
 */
static see_status_en _try_read (see_t *see, see_idx_t page, see_idx_t idx, byte_t *word)
{
   see_idx_t fp;        // Actual flash pointer
   see_idx_t i;         // Read index from flash

   /*
    * Seek for the first data and jump from data to data after that
    * Until we find the idx.
    * Do not search Page base address (fp>page)
    */
   fp = _find_last (see, page);
   while ( fp > page ) {
      // Read index data
      if ( see->io.fl_read (see->io.flash, fp, (void *)&i, sizeof (see_idx_t)) != DRV_READY) {
         // Error, no data
         return EE_FLASHERROR;
      }
      if (i == idx)  //first match
         break;
      fp -= (sizeof(see_idx_t) + see->iface.word_size);
   }
   // Check if we got something
   if (fp > page) {
      // Go in place
      fp -= see->iface.word_size;
      if ( see->io.fl_read (see->io.flash, fp, (void *)word, see->iface.word_size) != DRV_READY) {
         // Read error, no data
         return EE_FLASHERROR;
      }
      return EE_SUCCESS;
   }
   // No error, no data
   return EE_NODATA;
}

/*!
 * \brief
 *    Try to find room to write [word-idx] pair in page
 *
 * \param  see    The active see struct.
 * \param  page   Which page to seek
 * \param  idx    What virtual address
 * \param  word   Pointer of data to save
 * \return        The status of operation
 *    \arg EE_SUCCESS
 *    \arg EE_PAGEFULL
 *    \arg EE_FLASHERROR
 */
static see_status_en _try_write (see_t *see, see_idx_t page, see_idx_t idx, byte_t *word)
{
   see_idx_t fp;
   see_status_en ee_st;

   /*
    * Seek for the first data
    */
   fp = _find_last (see, page);
   if ( see->iface.word_size + sizeof (see_idx_t) <= see->conf.page_size+page-fp-sizeof(see_idx_t)) {
      //Have room to write
      fp += sizeof(see_idx_t);   //go to place 'n break
   }
   else
      return EE_PAGEFULL;

   /*
    * - Write the data
    * - Write virtual address last (at the end)
    * - Update "last" variable
    */
   ee_st = EE_SUCCESS;  //Try that or prove otherwise
   if ( see->io.fl_write (see->io.flash, fp, (void*)word, see->iface.word_size) != DRV_READY )
      ee_st = EE_FLASHERROR;
   fp += see->iface.word_size;
   if ( see->io.fl_write (see->io.flash, fp, (void*)&idx, sizeof(see_idx_t)) != DRV_READY )
      ee_st = EE_FLASHERROR;
   see->last_cur = fp;  // We only write in current page
   return ee_st;
}

/*!
 * \brief
 *    Try to read a single word data from EEPROM to the pointer buf.
 *    If there isn't data word with index a in EEPROM, the pointers buf get NULL
 * \param  see    The active see struct.
 * \param  idx    The virtual address(index) of data
 * \param  buf    Pointer to data
 * \return        The status of operation
 *    \arg EE_NODATA
 *    \arg EE_SUCCESS
 *    \arg EE_FLASHERROR
 */
static see_status_en _read_word (see_t *see, see_idx_t idx, byte_t *word)
{
   see_idx_t page;

   // From - To dispatcher
   if ( _valid_page (see) == EE_PAGE0 )   page = see->conf.page0_add;
   else                                   page = see->conf.page1_add;

   return _try_read (see, page, idx, word);
}

/*!
 * \brief
 *    Try to write a single word data to EEPROM pointed by buf.
 *    If there isn't room in EEPROM, return EE_PAGEFULL
 * \param  see  The active see struct.
 * \param  idx  The virtual address(index) of data
 * \param  word Pointer to word data
 * \return      The status of operation
 *    \arg EE_SUCCESS
 *    \arg EE_PAGEFULL
 *    \arg EE_FLASHERROR
 */
static see_status_en _write_word (see_t *see, see_idx_t idx, byte_t *word)
{
   see_idx_t page;
   see_status_en ee_st;

   // From - To dispatcher
   if ( _valid_page (see) == EE_PAGE0 )   page = see->conf.page0_add;
   else                                   page = see->conf.page1_add;

   /*
    * Try to write the data.
    * If the page is full, swap the page and try again.
    * If both pages are full, then the EEPROM is full
    */
   if ( (ee_st = _try_write (see, page, idx, word)) == EE_PAGEFULL) {
      if (_page_swap (see) == EE_FLASHERROR)
         return EE_FLASHERROR;
      if (page == see->conf.page0_add)
         page = see->conf.page1_add;
      else
         page = see->conf.page0_add;

      if ( _try_write (see, page, idx, word) == EE_PAGEFULL)
         return EE_PAGEFULL;
      else
         return EE_SUCCESS;
   }
   else
      return ee_st;
}




/*
 * ========== Public Simulated EE API ================
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief
 *    Link the driver flash read function
 *
 * \param  see   The active see struct.
 * \param  f     The read function
 * \return none
 */
inline void see_link_flash (see_t *see, void* flash) {
   see->io.flash = flash;
}

/*!
 * \brief
 *    Link the driver flash read function
 *
 * \param  see   The active see struct.
 * \param  f     The read function
 * \return none
 */
inline void see_link_flash_read (see_t *see, fl_io_ft f) {
   see->io.fl_read = f;
}

/*!
 * \brief
 *    Link the driver flash write function
 *
 * \param  see   The active see struct.
 * \param  f     The write function
 * \return none
 */
inline void see_link_flash_write (see_t *see, fl_io_ft f) {
   see->io.fl_write = f;
}

/*!
 * \brief
 *    Link the driver flash ioctl function
 *
 * \param  see   The active see struct.
 * \param  f     The ioctl function
 * \return none
 */
inline void see_link_flash_ioctl (see_t *see, fl_ioctl_ft f) {
   see->io.fl_ioctl = f;
}

/*
 * Set functions
 */

/*!
 * \brief
 *    Set PAGE0 address
 * \param   see   The active see struct.
 * \param  address   The targets PAGE0 address
 * \return none
 */
inline void see_set_page0_add (see_t *see, see_idx_t address) {
   see->conf.page0_add = address;
}

/*!
 * \brief
 *    Set PAGE1 address
 * \param   see   The active see struct.
 * \param  address   The targets PAGE0 address
 * \return none
 */
inline void see_set_page1_add (see_t *see, see_idx_t address) {
   see->conf.page1_add = address;
}

/*!
 * \brief
 *    Set virtual PAGE0/1 size
 * \param   see   The active see struct.
 * \param  size   The size
 * \return none
 */
void see_set_page_size (see_t *see, uint32_t size) {
   see->conf.page_size = size;
}

/*!
 * \brief
 *    Set the targets actual inner flash sector size
 * \param   see   The active see struct.
 * \param  size   The size
 * \return none
 */
void see_set_flash_sector_size (see_t *see, uint32_t size) {
   see->conf.fl_sector_size = size;
}

/*!
 * \brief
 *    Set the size of simulated memory words.
 * \param   see   The active see struct.
 * \param  size   The desired size
 * \return none
 */
void see_set_word_size (see_t *see, uint8_t size) {
   if (size > SEE_MAX_WORD_SIZE)
      size = SEE_MAX_WORD_SIZE;
   see->iface.word_size = size;
}

/*!
 * \brief
 *    Set the size of simulated sector
 * \param   see   The active see struct.
 * \param  size   The desired size
 * \return none
 */
void see_set_sector_size (see_t *see, uint32_t size) {
   see->iface.sector_size = size;
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
   see_status_en        ee_st = EE_SUCCESS;
   see_page_status_en   pg0_st, pg1_st;
   see_page_status_en   page_st;

   if (!see->io.fl_ioctl)  return see->status = DRV_ERROR;
   if (!see->io.fl_read)   return see->status = DRV_ERROR;
   if (!see->io.fl_write)  return see->status = DRV_ERROR;

   see->iface.size = (see->conf.page_size / (see->iface.word_size + sizeof(see_idx_t))) * see->iface.word_size;
   see->last_cur = see->last_pr = (see_idx_t)-1;

   /*!
    * \note
    *    Initiate a _read_last search routine in the first call.
    */
   see->status = DRV_NOINIT;
   see->io.fl_read (see->io.flash, see->conf.page0_add, &pg0_st, sizeof (see_page_status_en));
   see->io.fl_read (see->io.flash, see->conf.page1_add, &pg1_st, sizeof (see_page_status_en));

   if (pg0_st == pg1_st) {
      //Invalid state, Format
      _format (see);
      see->status = DRV_READY;
      return see->status;
   }
   /*
    *  Normal state. Do nothing
    */
   else if (pg0_st == EE_PAGE_ACTIVE && pg1_st == EE_PAGE_EMPTY) {
      see->status = DRV_READY;
      return see->status;
   }
   else if (pg0_st == EE_PAGE_EMPTY && pg1_st == EE_PAGE_ACTIVE) {
      see->status = DRV_READY;
      return see->status;
   }
   /*
    * Power failure during PageSwap just before marking ACTIVE the new page.
    * We just mark as active the new page.
    */
   else if (pg0_st == EE_PAGE_RECEIVEDATA && pg1_st == EE_PAGE_EMPTY) {
      drv_st = DRV_READY;     //Try that or prove otherwise
      page_st = EE_PAGE_ACTIVE;
      if ( see->io.fl_write (see->io.flash, see->conf.page0_add, (void*)&page_st, sizeof(page_st)) != DRV_READY)
         drv_st = DRV_ERROR;
      return (see->status = drv_st);
   }
   else if (pg0_st == EE_PAGE_EMPTY && pg1_st == EE_PAGE_RECEIVEDATA) {
      drv_st = DRV_READY;     //Try that or prove otherwise
      page_st = EE_PAGE_ACTIVE;
      if ( see->io.fl_write (see->io.flash, see->conf.page1_add, &page_st, sizeof(page_st)) != DRV_READY)
         drv_st = DRV_ERROR;
      return (see->status = drv_st);
   }
   /*
    * Power failure during PageSwap before finishing the copy.
    * The data are intact in old page (Still active).
    * We Re-call the page swap procedure.
    */
   else if (pg0_st == EE_PAGE_ACTIVE && pg1_st == EE_PAGE_RECEIVEDATA)
      ee_st = _page_swap (see);
   else if (pg0_st == EE_PAGE_RECEIVEDATA && pg1_st == EE_PAGE_ACTIVE)
      ee_st = _page_swap (see);

   if (ee_st == EE_SUCCESS)   return see->status = DRV_READY;
   else                       return see->status = DRV_ERROR;
}                                      



/*!
 * \brief
 *    Try to read data of size size, from EEPROM to the pointer d, for data.
 *    If there isn't data with index a in EEPROM, the pointers d and s get NULL
 * \param  see    The active see struct.
 * \param  idx    The virtual address(index) of data
 * \param  buf    Pointer to data
 * \param  size   number of bytes
 * \return Status of operation, not drivers
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en see_read (see_t *see, see_idx_t idx, byte_t *buf, bytecount_t size)
{
   see_idx_t   fl_idx;     // Index counters
   uint8_t     ofs_idx;
   bytecount_t i, words;   // Word counters
   uint8_t rem;
   uint8_t n;              // helper variable
   byte_t bf[SEE_MAX_WORD_SIZE];  // Buffer for not-aligned data

   if ( see->status != DRV_READY )
      return see->status = DRV_ERROR;

   see->status = DRV_BUSY;

   // Calculate counters
   ofs_idx = idx % see->iface.word_size;     // Index offset, if any
   fl_idx = idx - ofs_idx;                   // Actual written flash index

   // If we have unaligned starting data
   if (ofs_idx) {
      memset ((void *)bf, 0, SEE_MAX_WORD_SIZE);
      if ( _read_word (see, fl_idx, bf) == EE_FLASHERROR ) {
         see->status = DRV_READY;
         return DRV_ERROR;
      }
      // Update counters
      n = see->iface.word_size - ofs_idx;
      if (size>=n)   size -= n;
      else           n = size;
      // Copy and update pointers
      memcpy ((void *)buf, (const void *)(bf+ofs_idx), n);
      buf += n;
      fl_idx += see->iface.word_size;
   }

   // Calculate the rest of counters
   words = size / see->iface.word_size;      // How many words
   rem = size % see->iface.word_size;        // How many remaining bytes

   // Read aligned data
   for (i=0 ; i<words ; ++i) {
      if ( _read_word (see, fl_idx, buf) == EE_FLASHERROR ) {
         see->status = DRV_READY;
         return DRV_ERROR;
      }
      buf += see->iface.word_size;
      fl_idx += see->iface.word_size;
   }
   // Read remaining unaligned data
   if (rem) {
      // Take last data
      memset ((void *)bf, 0, SEE_MAX_WORD_SIZE);
      if ( _read_word (see, fl_idx, bf) == EE_FLASHERROR ) {
         see->status = DRV_READY;
         return DRV_ERROR;
      }
      memcpy ((void *)buf, (const void *)bf, rem);
   }
   return see->status = DRV_READY;
}

/*!
 * \brief
 *    Try to write data to EEPROM.
 *    If there isn't room in EEPROM, return DRV_ERROR
 *
 * \note
 *    We don't allow unaligned writes. This way the reminder
 *    data will have always empty room before the next written
 *    index.
 *
 *    For ex:
 *     0A      0F|10 13|14      19|1A 1D|1E      23|24 27|28      2D|2E 31|32
 *    -----------------------------------------------------------------------
 *    |   data   |     |   data   |     | data     |     |   data   |     |
 *    |<--6byte->| idx |<--6byte->| idx |<4byte>---| idx |   6byte  | idx |
 *    |xxxxxxxxxx| 0x0 |xxxxxxxxxx| 0x6 |xxxxxxx***| 0xC |    xx    | 0x12|
 *    ------------------------------------------------------------------------
 *    ^                                        ^ ^                      ^
 *    |                                        | |                      |
 *   start                                   end empty           always aligned
 *
 * \param  see  The active see struct.
 * \param  idx  The virtual address(index) of data
 * \param  buf  Pointer to data
 * \param size  size of data
 *
 * \return Status of operation, not drivers
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en see_write (see_t *see, see_idx_t idx, byte_t *buf, bytecount_t size)
{
   bytecount_t i, words;   // Word counters
   uint8_t rem;
   byte_t bf[SEE_MAX_WORD_SIZE];  // Buffer for not-aligned data

   if ( see->status != DRV_READY )
      return see->status = DRV_ERROR;

   see->status = DRV_BUSY;

   if (idx % see->iface.word_size) {
      see->status = DRV_READY;
      return DRV_ERROR;
      /*
       * We don't allow unaligned writes.
       */
   }

   // Calculate counters
   words = size / see->iface.word_size;      // How many words
   rem = size % see->iface.word_size;        // How many remaining bytes

   // Write aligned data
   for (i=0 ; i<words ; ++i) {
      if ( _write_word (see, idx, buf) == EE_FLASHERROR ) {
         see->status = DRV_READY;
         return DRV_ERROR;
      }
      buf += see->iface.word_size;
      idx += see->iface.word_size;
   }
   // Write remaining unaligned data
   if (rem) {
      memset ((void *)bf, 0, SEE_MAX_WORD_SIZE);
      memcpy ((void *)bf, (const void *)buf, rem);
      // Write last data
      if ( _write_word (see, idx, bf) == EE_FLASHERROR ) {
         see->status = DRV_READY;
         return DRV_ERROR;
      }
   }
   return see->status = DRV_READY;
}

/*!
 * \brief
 *    Simulated EEPROM ioctl function
 *
 * \param  see    The active see struct.
 * \param  cmd   specifies the command to FLASH
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_GET_SIZE
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
drv_status_en see_ioctl (see_t *see, ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   switch (cmd)
   {
      case CTRL_GET_STATUS:      /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = see->status;
         return DRV_READY;
      case CTRL_GET_SIZE:       /*!< Get size */
         if (buf)
            *(drv_status_en*)buf = see->iface.size;
         return see->status = DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
         see_deinit(see);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = see_init(see);
         else
            see_init(see);
         return DRV_READY;
      case CTRL_LOCK:
      case CTRL_CMD_LOCK:
      case CTRL_CMD_UNLOCK:      /*!< Forward call to linked driver */
         see->io.fl_ioctl (see->io.flash, cmd, buf);
         return see->status = DRV_READY;
      case CTRL_FORMAT:          /*!< Format flash */
         if (_format(see) == EE_FLASHERROR ) return see->status = DRV_ERROR;
         else                                return see->status = DRV_READY;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}

