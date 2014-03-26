/*
 * SD_spi_stm32.h
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
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
 * Date:       06/2013
 * Version:
 *
 */

#ifndef  __SD_spi_stm32_h__
#define  __SD_spi_stm32_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <sys/types.h>
#include "stm32f10x.h"
#include "stm32f10x_clock.h"
#include "stm32f10x_spi.h"
#include "diskio.h"

/* =================== User Defines ===================== */

#define SD_TIMEBASE_TICKS        (10)                       /*!< Time base periode is 10 msec */
#define SD_WAIT_TIMEOUT          (500/SD_TIMEBASE_TICKS)    /*!< Timeout in msec/SD_TIMEBASE_TICKS for sd card ready */
#define SD_POWER_TIMEOUT         (250/SD_TIMEBASE_TICKS)    /*!< Delay in in msec/SD_TIMEBASE_TICKS after power on */
#define SD_RX_TIMEOUT            (100/SD_TIMEBASE_TICKS)    /*!< Timeout in in msec/SD_TIMEBASE_TICKS for receive data */
#define SD_INIT_TIMEOUT          (2000/SD_TIMEBASE_TICKS)   /*!< Initialization timeout in in msec/SD_TIMEBASE_TICKS */


/* =================== General Defines ===================== */

#define SD_CRON_SET        (SD_GetTickFreq() / (SD_GetTickFreq() / SD_TIMEBASE_TICKS))
 /*!<
  * Define to produse a time base of SD_TIMEBASE_TICKS msec
  * \note
  *   Use this for cron settings in crontab calls, or in any time base
  *   methode is used in the project.
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



typedef uint8_t   sd_dat_t;

typedef void     (*SD_PinOut_t) (uint8_t);
typedef uint8_t  (*SD_PinIn_t) (void);
typedef void     (*SD_SPIClock_en_t) (uint8_t);
typedef clock_t  (*SD_GetFreq_t) (void);


typedef enum
{
   INTERFACE_SLOW,
   INTERFACE_FAST
} SD_speed_en;

typedef volatile struct
{
   SD_PinIn_t     WP;   /*!< Write protect pin */
   SD_PinIn_t     CD;   /*!< Card detect pin */
   SD_PinOut_t    CS;   /*!< Chip Select pin */
   SD_PinOut_t    PW;   /*!< SD Card Power pin */
   SPI_TypeDef*   SPI;  /*!< SPI interface used */
   SD_SPIClock_en_t
                  SPI_CLK_EN;    /*!< SPI Clock enable/disable function */
   SD_GetFreq_t   GetTickFreq;   /*!< Get Tick Frequency function */
   SD_GetFreq_t   GetSPIFreq;    /*!< Get spi driving clock (system) function */
}IO_t;

typedef volatile struct
{
   IO_t        IO;      /*!< Connection to the driver functions */
   SD_speed_en speed;   /*!< speed setting */
   uint8_t     status;  /*!< Disk status */
   uint8_t     type;    /*!< Card type flags */
   uint8_t     pow;     /*!< power on flag */
   uint32_t    t1, t2;  /*!< General decrement timers on time-base \sa SD_timebase() */
}SD_t;


/*!
 * FAT FS Compatible types and defines.
 * \note To use this module with a filesystem like FatFS cast this
 *       to FatFS types inside the tailoring functions
 */
typedef DSTATUS   SD_Status_t;    /*!< Status of SD Functions FAT compatible to cast */

/*!
 * Results of SD Functions. FAT compatible to cast
 * \note To use this module with a filesystem like FatFS cast this
 *       to FatFS types inside the tailoring functions.
 */
typedef DRESULT   SD_Result_t;

inline clock_t SD_GetTickFreq (void);  /*!< Warning: No need to use this, USE \sa SD_CRON_SET insteed. */


void SD_ConnectWP (volatile SD_PinIn_t pfun);
void SD_ConnectCD (volatile SD_PinIn_t pfun);
void SD_ConnectCS (volatile SD_PinOut_t pfun);
void SD_ConnectPW (volatile SD_PinOut_t pfun);
void SD_ConnectSPI (SPI_TypeDef* spi);
void SD_ConnectSPIClock (volatile SD_SPIClock_en_t pfun);
void SD_ConnectGetTickFreq (volatile SD_GetFreq_t pfun);
void SD_ConnectGetSPIFreq (volatile SD_GetFreq_t pfun);


SD_Status_t SD_deinit (int8_t drv);
SD_Status_t SD_init (int8_t drv);
SD_Status_t SD_getstatus (int8_t drv);
SD_Status_t SD_setstatus (int8_t drv, SD_Status_t st);
SD_Result_t SD_read (uint8_t drv, sd_dat_t *buff, uint32_t sector, uint8_t count);
SD_Result_t SD_write (uint8_t drv, const sd_dat_t *buff, uint32_t sector, uint8_t count);
SD_Result_t SD_ioctl (uint8_t drv, sd_dat_t ctrl, void *buff);

void SD_service (void);


#ifdef __cplusplus
 }
#endif

#endif   //#ifndef __SD_spi_stm32_h__
