/*
 * \file sd_spi.h
 * \brief
 *    MMC/SDSC/SDHC (in SPI mode) control module for STM32
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2013-2014 Houtouridis Christos <houtouridis.ch@gmail.com>
 *
 * based on the AVR MMC module (C)ChaN, 2007
 * and MMC/SDSC/SDHC example (C) Martin Thomas 2010
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
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 *
 */

#ifndef  __SD_spi_h__
#define  __SD_spi_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_iotypes.h>
#include <string.h>
#include <stdint.h>

/* =================== User Defines ===================== */

#define SD_TIMEBASE_TICKS        (10)                       /*!< Time base period is 10 msec */
#define SD_WAIT_TIMEOUT          (500/SD_TIMEBASE_TICKS)    /*!< Timeout in msec/SD_TIMEBASE_TICKS for SD card ready */
#define SD_POWER_TIMEOUT         (250/SD_TIMEBASE_TICKS)    /*!< Delay in in msec/SD_TIMEBASE_TICKS after power on */
#define SD_RX_TIMEOUT            (100/SD_TIMEBASE_TICKS)    /*!< Timeout in in msec/SD_TIMEBASE_TICKS for receive data */
#define SD_INIT_TIMEOUT          (2000/SD_TIMEBASE_TICKS)   /*!< Initialisation timeout in in msec/SD_TIMEBASE_TICKS */

#define SD_NUMBER_OF_DRIVES      (2)

/* =================== General Defines ===================== */

#define SD_CRON_SET        (get_freq() / (get_freq() / SD_TIMEBASE_TICKS))
 /*!<
  * Define to produce a time base of SD_TIMEBASE_TICKS msec
  * \note
  *   Use this for cron settings in crontab calls, or in any time base
  *   method is used in the project.
  * \example
  *   crontab (SD_timerbase, SD_CRON_SET);
  * \sa SD_timerbase
  */




/*!
 *  Definitions for MMC/SDC command
 */
#define SD_CMD0   (0x40+0)    /*!< GO_IDLE_STATE */
#define SD_CMD1   (0x40+1)    /*!< SEND_OP_COND (MMC) */
#define SD_ACMD41 (0xC0+41)   /*!< SEND_OP_COND (SDC) */
#define SD_CMD8   (0x40+8)    /*!< SEND_IF_COND */
#define SD_CMD9   (0x40+9)    /*!< SEND_CSD */
#define SD_CMD10  (0x40+10)   /*!< SEND_CID */
#define SD_CMD12  (0x40+12)   /*!< STOP_TRANSMISSION */
#define SD_ACMD13 (0xC0+13)   /*!< SD_STATUS (SDC) */
#define SD_CMD16  (0x40+16)   /*!< SET_BLOCKLEN */
#define SD_CMD17  (0x40+17)   /*!< READ_SINGLE_BLOCK */
#define SD_CMD18  (0x40+18)   /*!< READ_MULTIPLE_BLOCK */
#define SD_CMD23  (0x40+23)   /*!< SET_BLOCK_COUNT (MMC) */
#define SD_ACMD23 (0xC0+23)   /*!< SET_WR_BLK_ERASE_COUNT (SDC) */
#define SD_CMD24  (0x40+24)   /*!< WRITE_BLOCK */
#define SD_CMD25  (0x40+25)   /*!< WRITE_MULTIPLE_BLOCK */
#define SD_CMD55  (0x40+55)   /*!< APP_CMD */
#define SD_CMD58  (0x40+58)   /*!< READ_OCR */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC    0x01     /* MMC ver 3 */
#define CT_SD1    0x02     /* SD ver 1 */
#define CT_SD2    0x04     /* SD ver 2 */
#define CT_SDC    (CT_SD1|CT_SD2)   /* SD */
#define CT_BLOCK  0x08     /* Block addressing */

typedef uint8_t   sd_dat_t;
typedef uint32_t  sd_idx_t;

typedef drv_status_en (*spi_ioctl_t) (void *, ioctl_cmd_t, ioctl_buf_t);
//typedef uint8_t (*spi_read_t) (void *, int);
//typedef void    (*spi_write_t) (void *, uint8_t, int);
typedef uint8_t (*spi_rw_t) (void *spi, uint8_t data);

typedef volatile struct
{
   drv_pinin_ft   wp;            /*!< Write protect pin */
   drv_pinin_ft   cd;            /*!< Card detect pin */
   drv_pinout_ft  cs;            /*!< Chip Select pin */
   drv_pinout_ft  pw;            /*!< SD Card Power pin */
   void*          spi;           /*!< void SPI type structure */
   spi_ioctl_t    spi_ioctl;     /*!< SPI ioctl function */
   spi_rw_t       spi_rw;        /*!< SPI read/write function */
}sd_io_t;

typedef volatile struct
{
   uint32_t       speed;   /*!< speed setting */
   uint8_t        type;    /*!< Card type flags */
   uint8_t        pow;     /*!< power on flag */
   drv_status_en  status;  /*!< Disk status */
   uint32_t       t1, t2;  /*!< General decrement timers on time-base \sa SD_timebase() */
}sd_data_t;

typedef volatile struct
{
   sd_io_t        sd_io[SD_NUMBER_OF_DRIVES];      /*!< Connection to the driver functions */
   sd_data_t      drive[SD_NUMBER_OF_DRIVES];      /*!< Physical drive table */
}sd_spi_t;


/*!
 * FAT FS Compatible types and defines.
 * \note To use this module with a filesystem like FatFS cast this
 *       to FatFS types inside the tailoring functions
 */
//typedef DSTATUS   SD_Status_t;    /*!< Status of SD Functions FAT compatible to cast */

/*!
 * Results of SD Functions. FAT compatible to cast
 * \note To use this module with a filesystem like FatFS cast this
 *       to FatFS types inside the tailoring functions.
 */
//typedef DRESULT   SD_Result_t;



/*
 *  ============= PUBLIC EE API =============
 */

/*
 * Link and Glue functions
 */
void sd_link_wp (int drv, drv_pinin_ft fun);
void sd_link_cd (int drv, drv_pinin_ft fun);
void sd_link_cs (int drv, drv_pinout_ft fun);
void sd_link_pw (int drv, drv_pinout_ft fun);
void sd_link_spi_ioctl (int drv, spi_ioctl_t fun);
void sd_link_spi_rw (int drv, spi_rw_t fun);
void sd_link_spi (int drv, void* spi);

/*
 * Set functions
 */

/*
 * User Functions
 */
void sd_service (void);

void sd_deinit (int drv);                  /*!< for compatibility */
drv_status_en sd_init (int drv);           /*!< for compatibility */
drv_status_en sd_getstatus (int drv);      /*!< for compatibility */
drv_status_en sd_setstatus (int drv, drv_status_en st);   /*!< for compatibility */

drv_status_en  sd_read (int drv, sd_idx_t sector, sd_dat_t *buf, size_t count);
drv_status_en sd_write (int drv, sd_idx_t sector, const sd_dat_t *buf, size_t count);
drv_status_en sd_ioctl (int drv, ioctl_cmd_t ctrl, ioctl_buf_t buf);

#ifdef __cplusplus
 }
#endif

#endif   //#ifndef __SD_spi_h__
