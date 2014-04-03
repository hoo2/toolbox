/**

	EE.c - Is the file for the EEPROM Emulation

   Copyright (C) 2012 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       01/2012
   Version:    0.1

 */


#include <EE.h>

static EE_Page_t        EE_ValidPage (void);
static EE_ExitStatus_t  EE_TryRead (flash_add_t page, EE_Index_t idx, EE_Data_t *d);
static EE_ExitStatus_t  EE_TryWrite (flash_add_t page, EE_Index_t idx, EE_Data_t *d);
static EE_ExitStatus_t  EE_PageSwap (void);
static EE_ExitStatus_t  EE_ErasePage (flash_add_t Page_Address);

/**
  * EE_Page_t EE_ValidPage (void)
  *
  * @brief  Indicates the current valid page
  * @param  None
  * @retval EE_Page_t : EE_PAGE0 of EE_PAGE1
  */
static EE_Page_t EE_ValidPage (void)
{
   if ( *(__IO flash_data_t*)EE_PAGE0_ADDRESS == EE_PAGE_ACTIVE)
      return EE_PAGE0;
   else
      return EE_PAGE1;
}

/**
  * EE_TryRead (EE_Page_t page, EE_Index_t idx, EE_Data_t *d)
  *
  * @brief  Try to find idx in page and read the data and size
  * @param  page  : which page to seak
  *         idx   : what virtual address
  *         *d    : pointer to store the data
  * @retval EE_ExitStatus_t : EE_NODATA or EE_SUCCESS
  */
static EE_ExitStatus_t EE_TryRead (flash_add_t page, EE_Index_t idx, EE_Data_t *d)
{
   flash_add_t  fp;
   EE_Index_t   i;
   
   /*
      Seak for the first data and jump from data to data after that
      Untill we find the idx.
      Do not search Page base address (fp>page)
    */
   for (fp=page+EE_PAGE_SIZE-sizeof(EE_Index_t) ; fp>page ; fp-=sizeof(EE_Index_t))
   {
      FLASH_LoadData(fp, &i, sizeof (i));
      if (i < (EE_Index_t)(-1))   //not empty 0xFF..FF
      {
         if (i == idx)  //first match
            break;
         else
            fp -= sizeof (EE_Data_t);
      }
   }
   // Check if we got something
   if (fp > page)
   {
      FLASH_LoadData(fp-sizeof(EE_Data_t), (flash_data_t *)d, sizeof(EE_Data_t));
      return EE_SUCCESS;
   }
   d = (void *)0;  //no data
   return EE_NODATA;
}

/**
  * EE_TryWrite (EE_Page_t page, EE_Index_t idx, EE_Data_t *d)
  *
  * @brief  Try to find room to write idx-data in page
  * @param  page  : which page to seak
  *         idx   : what virtual address
  *         *d    : pointer of data to save
  * @retval EE_ExitStatus_t : EE_SUCCESS, EE_PAGEFULL, EE_FLASHERROR
  */
static EE_ExitStatus_t EE_TryWrite (flash_add_t page, EE_Index_t idx, EE_Data_t *d)
{
   flash_add_t fp;
   EE_Index_t   i;

   EE_ExitStatus_t ee_st;

   /*
      Seek for the first data
    */
   for (fp=page+EE_PAGE_SIZE-sizeof(EE_Index_t) ; fp>=page ; fp-=sizeof(EE_Index_t))
   {
      FLASH_LoadData(fp, &i, sizeof (i));
      if (i < (EE_Index_t)(-1))   //not empty 0xFF..FF
      {
         if (sizeof(EE_Data_t) + sizeof (EE_Index_t) <= EE_PAGE_SIZE+page-fp-sizeof(EE_Index_t))  //Have room to write
         {
            fp += sizeof(EE_Index_t);   //go to place 'n break
            break;
         }
         else
            return EE_PAGEFULL;
      }
   }
   /* 
      - Unlock first
      - Write the data
      - Write virtual address last (at the end)
   */
   FLASH_Unlock ();
   ee_st = EE_SUCCESS;  //Try that or prove otherwise
   if (FLASH_WriteData(fp, (flash_data_t *)d, sizeof(EE_Data_t)) != FLASH_COMPLETE)
      ee_st = EE_FLASHERROR;
   fp += sizeof(EE_Data_t);
   if (FLASH_WriteData(fp, &idx, sizeof(EE_Index_t)) != FLASH_COMPLETE)
      ee_st = EE_FLASHERROR;
   // Lock and return
   FLASH_Lock();
   return ee_st;
}

/**
  * EE_ExitStatus_t EE_PageSwap (void)
  *
  * @brief  Copy each variable instance from a full page to the emty one
  * @param  None
  * @retval EE_ExitStatus : EE_FLASHERROR or EE_SUCCESS
  */
static EE_ExitStatus_t EE_PageSwap (void)
{
   flash_add_t       from, to;
   flash_data_t      status;
   EE_Data_t         data;
   EE_Index_t        idx;
   EE_ExitStatus_t   ee_st;
 
   // From - To dispatcher
   if ( EE_ValidPage () == EE_PAGE0 )
   {
      from = EE_PAGE0_ADDRESS;
      to = EE_PAGE1_ADDRESS;
   }
   else
   {
      from = EE_PAGE1_ADDRESS;
      to = EE_PAGE0_ADDRESS;
   }

   // Unlock first
   FLASH_Unlock ();
   // Mark the new Page as RECEIVEDATA
   if (EE_ErasePage (to) != EE_SUCCESS)
      return EE_FLASHERROR;
   status = EE_PAGE_RECEIVEDATA;
   if (FLASH_WriteData(to, &status, sizeof(status)) != FLASH_COMPLETE)
      return EE_FLASHERROR;


   // Copy each idx writen on "from" page to their new home
   for (idx=0 ; idx<EE_EMULATED_SIZE ; ++idx)
   {
      ee_st = EE_TryRead (from, idx, &data);
      if (ee_st == EE_SUCCESS)
      {
         ee_st = EE_TryWrite (to, idx, &data);
         if (ee_st != EE_SUCCESS)
            break;
      }
      else if (ee_st == EE_NODATA)    continue;
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
      - Unlock
      - Erase old page (auto status as EMPTY = 0xFFFF)
      - Mark old page as EMPTY
      - Re - Mark new as ACTIVE (write 0xAAAA on top of 0xFFFF)
      - Lock Flash before return
    */ 
   FLASH_Unlock ();
   ee_st = EE_SUCCESS;  //Try that or prove otherwise
   if ( EE_ErasePage(from) != EE_SUCCESS )
      ee_st = EE_FLASHERROR;
   
   status = EE_PAGE_EMPTY;     // Mark the new Page as EMPTY
   if (FLASH_WriteData(from, &status, sizeof(status)) != FLASH_COMPLETE)
      ee_st = EE_FLASHERROR;

  
   // Mark the new Page as ACTIVE
   status = EE_PAGE_ACTIVE;
   if (FLASH_WriteData(to, &status, sizeof(status)) != FLASH_COMPLETE)
      ee_st = EE_FLASHERROR;

   FLASH_Lock();
   return ee_st;
}



static EE_ExitStatus_t  EE_ErasePage (flash_add_t Page_Address)
{
   uint8_t  nop = EE_PAGE_SIZE/ARM_PAGE_SIZE;

   while (nop--)
      if ( FLASH_ErasePage(Page_Address + nop*ARM_PAGE_SIZE)!= FLASH_COMPLETE )
         return EE_FLASHERROR;
   return EE_SUCCESS;
}

/**
  * EE_ExitStatus_t   EE_Init (void)
  *
  * @brief  Restore the pages to a known good state in case of page's status
  *         corruption after a power loss.
  * @param  None
  * @retval EE_ExitStatus : EE_FLASHERROR or EE_SUCCESS
  */
EE_ExitStatus_t   EE_Init (void)
{
   EE_ExitStatus_t   ee_st;
   EE_PageStatus_t   PageStatus0, PageStatus1; 
   flash_data_t      page_st;

   PageStatus0 = (EE_PageStatus_t)*(__IO flash_data_t*)EE_PAGE0_ADDRESS;
   PageStatus1 = (EE_PageStatus_t)*(__IO flash_data_t*)EE_PAGE1_ADDRESS;

   if (PageStatus0 == PageStatus1)  //Invalid state, Format
      EE_Format ();
   /*
      Normal state. Do nothing
   */
   else if (PageStatus0 == EE_PAGE_ACTIVE && PageStatus1 == EE_PAGE_EMPTY)
      return EE_SUCCESS;                                                          
   else if (PageStatus0 == EE_PAGE_EMPTY && PageStatus1 == EE_PAGE_ACTIVE)
      return EE_SUCCESS;
   /*
      Power failure during PageSwap just before marking ACTIVE the new page.
      We just mark as active the new page.
   */
   else if (PageStatus0 == EE_PAGE_RECEIVEDATA && PageStatus1 == EE_PAGE_EMPTY)
   {
      FLASH_Unlock ();
      ee_st = EE_SUCCESS;     //Try that or prove otherwise
      page_st = EE_PAGE_ACTIVE;
      if ( FLASH_WriteData (EE_PAGE0_ADDRESS, &page_st, sizeof(page_st)) != FLASH_COMPLETE)
         ee_st = EE_FLASHERROR;
      FLASH_Lock ();
      return ee_st;  
   }
   else if (PageStatus0 == EE_PAGE_EMPTY && PageStatus1 == EE_PAGE_RECEIVEDATA)
   {
      FLASH_Unlock ();
      ee_st = EE_SUCCESS;     //Try that or prove otherwise
      page_st = EE_PAGE_ACTIVE;
      if ( FLASH_WriteData (EE_PAGE1_ADDRESS, &page_st, sizeof(page_st)) != FLASH_COMPLETE)
         ee_st = EE_FLASHERROR;
      FLASH_Lock ();
      return ee_st;  
   }
   /*
      Power failure during PageSwap before finishing the copy.
      The data are intact in old page (Still active).
      We Re-call the page swap procedure.
   */
   else if (PageStatus0 == EE_PAGE_ACTIVE && PageStatus1 == EE_PAGE_RECEIVEDATA)
      return EE_PageSwap ();
   else if (PageStatus0 == EE_PAGE_RECEIVEDATA && PageStatus1 == EE_PAGE_ACTIVE)
      return EE_PageSwap ();

   return EE_SUCCESS;
}                                      



/**
  * EE_ExitStatus_t EE_Format (void)
  *
  * @brief  Erases PAGE0 and PAGE1 and writes EE_PAGE_ACTIVE
            and EE_PAGE_EMPTY to PAGE0,1
  * @param  None
  * @retval EE_ExitStatus : EE_FLASHERROR or EE_SUCCESS
  */
EE_ExitStatus_t EE_Format (void)
{
   flash_data_t page0_st, page1_st;
   EE_ExitStatus_t   ee_st;
   uint8_t  nop = 2*EE_PAGE_SIZE/ARM_PAGE_SIZE;

   FLASH_Unlock ();
   ee_st = EE_SUCCESS;     //Try that or prove otherwise

   while (nop--)
   {
      if (FLASH_ErasePage (EE_PAGE0_ADDRESS + nop*ARM_PAGE_SIZE) != FLASH_COMPLETE)
      {
         ee_st = EE_FLASHERROR;
         break;
      }
   }      
   if (ee_st!=EE_FLASHERROR)
   {
      page0_st = EE_PAGE_ACTIVE;
      page1_st = EE_PAGE_EMPTY;
         
      if ( FLASH_WriteData (EE_PAGE0_ADDRESS, &page0_st, sizeof(page0_st)) != FLASH_COMPLETE)
         ee_st = EE_FLASHERROR;
      if ( FLASH_WriteData (EE_PAGE1_ADDRESS, &page1_st, sizeof(page1_st)) != FLASH_COMPLETE)
         ee_st = EE_FLASHERROR;
   }

   FLASH_Lock();
   return ee_st;
}



/**
  * EE_ExitStatus_t   EE_Read (EE_Index_t idx, flash_data_t *d)
  *
  * @brief  Try to read data from EEPROM to the pointer d, for data.
  *         If there isn't data with index a in EEPROM, the pointers d and s get NULL
  * @param  idx: The virtual address(index) of data
  *         *d: Pointer to data
  * @retval EE_ExitStatus : EE_NODATA or EE_SUCCESS
  */
EE_ExitStatus_t   EE_Read (EE_Index_t idx, EE_Data_t *d)
{
   flash_add_t       page;

   // From - To dispatcher
   if ( EE_ValidPage () == EE_PAGE0 )  page = EE_PAGE0_ADDRESS;
   else                                page = EE_PAGE1_ADDRESS;

   return EE_TryRead (page, idx, d);
}

/**
  * EE_ExitStatus_t   EE_Write (EE_Index_t idx, flash_data_t *d)
  *
  * @brief  Try to write data to EEPROM. The data are d with size s.
  *         If there isn't room in EEPROM, return EE_EEFULL
  * @param  idx: The virtual address(index) of data
  *         *d: Pointer to data
  * @retval EE_ExitStatus : EE_SUCCESS, EE_FULL, EE_FLASHERROR
  */
EE_ExitStatus_t   EE_Write (EE_Index_t idx, EE_Data_t *d)
{
   flash_add_t page;
   EE_ExitStatus_t ee_st;

   // From - To dispatcher
   if ( EE_ValidPage () == EE_PAGE0 )  page = EE_PAGE0_ADDRESS;
   else                                page = EE_PAGE1_ADDRESS;

   /*
      Try to write the data.
      If the page is full, swap the page and try again.
      If both pages are full, then the EEPROM is full
   */
   ee_st = EE_TryWrite (page, idx, d);
   if ( ee_st == EE_PAGEFULL)
   {
      if (EE_PageSwap () == EE_FLASHERROR)   return EE_FLASHERROR;
      if (page == EE_PAGE0_ADDRESS)          page = EE_PAGE1_ADDRESS;
      else                                   page = EE_PAGE0_ADDRESS;

      ee_st = EE_TryWrite (page, idx, d);
      if ( ee_st == EE_PAGEFULL)
         return EE_EEFULL;
      else
         return ee_st;
   }
   else
      return ee_st;
}

