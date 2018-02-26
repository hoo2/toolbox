/*
 * \file tbx_ioctl.h
 * \brief
 *    A command list for toolbox middleware driver API _ioctl() functions.
 *    There is also a generic drv_status_t to use from middleware and low
 *    level driver API. Any driver compatible with toolbox can use/expand
 *    this file ;-)
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
#ifndef __tbx_ioctl_h__
#define __tbx_ioctl_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Command code for _ioctrl fucntions
 */

/*
 * Generic command
 */
#define CTRL_GET_STATUS          0x00  /*!< Generic status command */
#define CTRL_DEINIT              0x01  /*!< Generic de-init command */
#define CTRL_INIT                0x02  /*!< Generic init command */
#define CTRL_RESET               0x03  /*!< Generic reset command */

#define CTRL_SYNC                0x10  /*!< Flush disk cache (for write functions) used by FatFs*/
#define CTRL_FLUSH               0x10  /*!< Flush buffers */
#define CTRL_GET_SECTOR_COUNT    0x11  /*!< Get media size (for only f_mkfs()) used by FatFs*/
#define CTRL_GET_SECTOR_SIZE     0x12  /*!< Get sector size (for multiple sector size (_MAX_SS >= 1024)) used by FatFs */
#define CTRL_GET_BLOCK_SIZE      0x13  /*!< Get erase block size (for only f_mkfs()) used by FatFs */
#define CTRL_GET_SIZE            0x14
#define CTRL_ERASE_SECTOR        0x15  /*!< Force erased a block of sectors (for only _USE_ERASE) used by FatFs */
#define CTRL_ERASE_PAGE          0x16  /*!< Erase current media page */
#define CTRL_ERASE_ALL           0x17  /*!< Erase all pages */
#define CTRL_FORMAT              0x18  /*!< Physical format the media */

#define CTRL_SEARCH              0x19  /*!< Generic search command */
#define CTRL_ENTER               0x1A  /*!< Generic enter command */
#define CTRL_BACK                0x1B  /*!< Generic back command */
#define CTRL_SELECT              0x1C  /*!< Generic select command */

/*
 * Power commands
 */
#define CTRL_POWER               0x20  /*!< Get/Set power status */
#define CTRL_LOCK                0x21  /*!< Lock/Unlock media removal */
#define CTRL_EJECT               0x22  /*!< Eject media */
#define CTRL_CMD_LOCK            0x23  /*!< Lock the media */
#define CTRL_CMD_UNLOCK          0x24  /*!< Unlock the media */

/*
 * MMC/SDC specific ioctl command
 */
#define CTRL_MMC_GET_TYPE        0x30  /*!< Get card type */
#define CTRL_MMC_GET_CSD         0x31  /*!< Get CSD */
#define CTRL_MMC_GET_CID         0x32  /*!< Get CID */
#define CTRL_MMC_GET_OCR         0x33  /*!< Get OCR */
#define CTRL_MMC_GET_SDSTAT      0x34  /*!< Get SD status */

/*
 * UI specific devices
 */
#define CTRL_CLEAR               0x40
#define CTRL_SHIFT               0x41
#define CTRL_BLINK               0x42
#define CTRL_BACKLIGHT           0x43
#define CTRL_CURSOR              0x44


/*
 * Interface set commands
 */
#define  CTRL_SET_STATUS         0x50
#define  CTRL_SET_TYPE           0x51
#define  CTRL_SET_SPEED          0x52
#define  CTRL_SET_BAUDRATE       0x53
#define  CTRL_SET_FREQ           0x54
#define  CTRL_SET_CLOCK          0x55
#define  CTRL_SET_VALUE          0x56
#define  CTRL_SET_DIR            0x57

/*
 * Comunication commands
 */
#define CTRL_START               0x80  /*!< Generic start command */
#define CTRL_STOP                0x81  /*!< Generic stop command */
#define CTRL_SEND_DATA           0x82
#define CTRL_SEND_CMD            0x83
#define CTRL_SEND_ALARM          0x84


typedef uint32_t  ioctl_cmd_t;   /*!< Generic ioctl command type */
typedef uint32_t  ioctl_data_t;  /*!< Generic ioctl data type */
typedef void*     ioctl_buf_t;   /*!< Generic ioctl buffer data type */


#ifdef __cplusplus
}
#endif

#endif //#ifndef __tbx_ioctl_h__
