/**

	EE.h - Is the header file for the EEPROM Emulation

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
   Date:       1/2012
   Version:    0.1

*/

/*
   *** NOTE ***
   For now the EE has no inode table in flash
   and search for an extern defined inode table
 */

#ifndef __EE_h__
#define __EE_h__

#include <Flash.h>
#include <stdlib.h>
#include <stddef.h>

/* ================   User Defines   ====================*/

// See datasheet for the page range
#ifdef STM32F100C4
 #define    EE_PAGE0_ADDRESS           ((uint32_t)0x08003800)     //PAGE 14
 #define    EE_PAGE1_ADDRESS           ((uint32_t)0x08003C00)     //PAGE 15
#endif

#ifdef STM32F100C6
 #define    EE_PAGE0_ADDRESS           ((uint32_t)0x08007000)     //PAGE 28
 #define    EE_PAGE1_ADDRESS           ((uint32_t)0x08007800)     //PAGE 30
 #define    EE_PAGE_SIZE               (0x0800)    // 2Kb
 #define    ARM_PAGE_SIZE              (0x0400)    // 1Kb
#endif

#ifdef STM32F100R6
 #define    EE_PAGE0_ADDRESS           ((uint32_t)0x08007000)     //PAGE 28
 #define    EE_PAGE1_ADDRESS           ((uint32_t)0x08007800)     //PAGE 30
 #define    EE_PAGE_SIZE               (0x0800)    // 2Kb
 #define    ARM_PAGE_SIZE              (0x0400)    // 1Kb
#endif

#ifdef STM32F100C8
 #define    EE_PAGE0_ADDRESS           ((uint32_t)0x0800F800)     //PAGE 62
 #define    EE_PAGE1_ADDRESS           ((uint32_t)0x0800FC00)     //PAGE 63
 #define    EE_PAGE_SIZE               (0x0400)    // 1Kb
 #define    ARM_PAGE_SIZE              (0x0400)    // 1Kb
#endif

#ifdef STM32F100R8
 #define    EE_PAGE0_ADDRESS           ((uint32_t)0x0800F000)     //PAGE 60
 #define    EE_PAGE1_ADDRESS           ((uint32_t)0x0800F800)     //PAGE 62
 #define    EE_PAGE_SIZE               (0x0800)    // 2Kb
 #define    ARM_PAGE_SIZE              (0x0400)    // 1Kb
#endif

/* ================   General Defines   ====================*/

#define  EE_EMULATED_SIZE              ( EE_PAGE_SIZE / (sizeof(EE_Data_t) + sizeof(EE_Index_t)) )
                                                                

typedef  uint16_t       EE_Index_t;
typedef  size_t         EE_Data_t;     // Make each data a size_t size


typedef enum
{
   EE_SUCCESS = 0,
   EE_NODATA,
   EE_PAGEFULL,
   EE_FLASHERROR,
   EE_EEFULL
}EE_ExitStatus_t;

typedef enum
{
   EE_PAGE_ACTIVE = 0,
   EE_PAGE_RECEIVEDATA = 0xAAAA,
   EE_PAGE_EMPTY = 0xFFFF
}EE_PageStatus_t;

typedef enum
{
   EE_PAGE0, EE_PAGE1
}EE_Page_t;

EE_ExitStatus_t   EE_Init (void);
EE_ExitStatus_t   EE_Format (void);
EE_ExitStatus_t   EE_Read (EE_Index_t idx, EE_Data_t *d);
EE_ExitStatus_t   EE_Write (EE_Index_t idx, EE_Data_t *d);

#endif   //#ifndef __EE_h__
