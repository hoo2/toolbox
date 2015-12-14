/*!
 * \file s25fs_spi.h
 * \brief
 *    A target independent spansion s25fs spi flash memory
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
#ifndef __s25fs_spi_h__
#define __s25fs_spi_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_types.h>
#include <stddef.h>
#include <drv/spi_bb.h>
#include <sys/jiffies.h>
#include <crypt/cryptint.h>

/*
 * =================== User Defines =====================
 */
#define S25FS_TIMEOUT      (1000)      // 1000 msec


/*
 * =================== General Defines =====================
 */
#define S25FS_WRITE_PAGE_SZ_DEF        (256)          // 256 bytes
#define S25FS_ERASE_PAGE_SZ_DEF        (0x10000)      // 64k
#define S25FS_SECTOR_SIZE_DEF          (512)          // 512 bytes

/*!
 * SPI Flash Commands info
 */
#define S25FS_WRDI_CMD                 (0x04)
#define S25FS_RDSR_CMD                 (0x05)
#define S25FS_WREN_CMD                 (0x06)
#define S25FS_RDSR2_CMD                (0x07)  // Read Status Register-2
#define S25FS_PP_4B_CMD                (0x12)
#define S25FS_READ_4B_CMD              (0x13)
#define S25FS_BE1_CMD                  (0x60)  // Bulk Erase
#define S25FS_RDAR_CMD                 (0x65)  // Read Any Register
#define S25FS_SE_4B_CMD                (0xDC)
#define S25FS_EES_CMD                  (0xD0)  //Evaluate Erase Status


/*!
 * SPI.read and SPI.write functions needed defines
 */
#define ADDRESS_NOT_USED 0xFFFFFFFF
#define BUFFER_NOT_USED  (byte_t*)0

#ifndef S25FS_EN
#define S25FS_EN    (1)
#endif
#ifndef S25FS_DIS
#define S25FS_DIS   (0)
#endif

/*!
 * ============== S25FS ioctl commands =================
 */
#define S25FS_CTRL_RDSR1         (0x80)      /*!< Request flash status register SR1 */
#define S25FS_CTRL_WREN          (0x81)      /*!< Request Write enable command to flash */
#define S25FS_CTRL_WRDI          (0x82)      /*!< Request Write disable command to flash */
#define S25FS_CTRL_SE            (0x83)      /*!< Request Sector erase command to flash */

/*
 * =================== Data types =====================
 */

/*!
 * LLD System Specific Typedefs
 */
typedef uint32_t     s25fs_idx_t ;        /*!< Used for system level addressing */
typedef byte_t       s25fs_data_t;        /*!< Used for system data addressing */

/*!
 * SLLD Internal Data Types
 */
typedef uint32_t     bytecount_t;            /*!< used for multi-byte operations */


/*!
 * Flash Software protect status
 */
typedef enum {
    S25FS_UNPROTECTED = 0,
    S25FS_PROTECTED
}s25fs_sp_status_en;

/*!
 * Flash embedded operation status
 */
typedef enum {
   S25FS_STATUS_UNKNOWN = 0,
   S25FS_NOT_BUSY,
   S25FS_PROGRAM_ERROR,
   S25FS_ERASE_ERROR,
   S25FS_SUSPEND,
   S25FS_BUSY
}s25fs_devstatus_en;


typedef drv_status_en (*s25fs_spi_ioctl_t) (void *, ioctl_cmd_t, ioctl_buf_t);
typedef drv_status_en (*s25fs_spi_rw_t)  (void *, byte_t *, int);


typedef enum {
   S25FS_4B_ADDR_BAR = 0,
   S25FS_4B_ADDR_CMDS
}s25fs_add_mode_en;

typedef uint8_t   s25fs_cmd_t;

/*!
 * The drivers link data struct.
 */
typedef volatile struct {
   drv_pinout_ft  wp;            /*!< Write protect pin - Optional */
   drv_pinout_ft  cs;            /*!< Chip Select pin */
   void*          spi;           /*!< void SPI type structure - NULL for hardware SPI */
   s25fs_spi_ioctl_t
                  spi_ioctl;     /*!< SPI ioctl function */
   s25fs_spi_rw_t spi_read;      /*!< SPI read/write function */
   s25fs_spi_rw_t spi_write;     /*!< SPI read/write function */
}s25fs_io_t;

/*!
 * The s25fs configuration and settings struct
 */
typedef volatile struct {
   uint32_t write_page_sz;    /*!< The flash write page buffer size */
   uint32_t erase_page_sz;    /*!< The flash erase page size */
   uint32_t sector_sz;        /*!< The virtual sector size, used in file systems */
}s25fs_conf_t;

/*!
 * The s25fs driver data type. Each one refers to
 * each flash chip in the PCB.
 */
typedef volatile struct {
   s25fs_io_t     io;         /*!< driver links */
   s25fs_conf_t   conf;       /*!< Configuration and settings */
   drv_status_en  status;     /*!< Flash driver status, NOT the device status */
}s25fs_t;


/*
 *  ============= PUBLIC S25FS API =============
 */

/*
 * Link and Glue functions
 */
void s25fs_link_wp (s25fs_t *drv, drv_pinout_ft fun);
void s25fs_link_cs (s25fs_t *drv, drv_pinout_ft fun);
void s25fs_link_spi_read (s25fs_t *drv, s25fs_spi_rw_t fun);
void s25fs_link_spi_write (s25fs_t *drv, s25fs_spi_rw_t fun);
void s25fs_link_spi (s25fs_t *drv, void* spi);

/*
 * Set functions
 */
void s25fs_set_write_page_sz (s25fs_t *drv, uint32_t size);
void s25fs_set_erase_page_sz (s25fs_t *drv, uint32_t size);
void   s25fs_set_sector_size (s25fs_t *drv, uint32_t size);

/*
 * User Functions
 */
void s25fs_deinit (s25fs_t *drv);                  /*!< for compatibility */
drv_status_en s25fs_init (s25fs_t *drv);           /*!< for compatibility */

drv_status_en         s25fs_read (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, int count);
drv_status_en        s25fs_write (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, int count);
drv_status_en  s25fs_read_sector (s25fs_t *drv, int sector, s25fs_data_t *buf, int count);
drv_status_en s25fs_write_sector (s25fs_t *drv, int sector, s25fs_data_t *buf, int count);
drv_status_en        s25fs_ioctl (s25fs_t *drv, ioctl_cmd_t ctrl, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __s25fs_spi_h__
