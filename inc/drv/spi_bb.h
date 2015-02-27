/*
 * \file spi_bb.h
 * \brief
 *    SPI Bus protocol for master, using bit-banging that support CPOL and CPHA
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2013-2014 Houtouridis Christos <houtouridis.ch@gmail.com>
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
#ifndef  __spi_bb_h__
#define  __spi_bb_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_types.h>
#include <stdint.h>
#include <sys/jiffies.h>

/*
 *  ============= USER DEFINES =============
 */
#define  SPI_DEFAULT_SPPED       (100000)

// Helper defines
#define  SPI_CPOL_IDLE_LOW       (0)
#define  SPI_CPOL_IDLE_HIGH      (1)
#define  SPI_CPHA_1ST_EDGE       (0)
#define  SPI_CPHA_2ND_EDGE       (1)
#define  SPI_NSS_SOFT            (0)
#define  SPI_NSS_HARD            (1)


/*!
 * SPI bus protocol data structure
 */
typedef volatile struct
{
   drv_pinout_ft  mosi;       /*!< Link to driver's MOSI function */
   drv_pinin_ft   miso;       /*!< Link to driver's MISO function */
   drv_pinout_ft  sclk;       /*!< Link to driver's SCLK function */
   drv_pinout_ft  ss;         /*!< Link to driver's SS function */
   uint32_t       clk_delay;  /*!< Clock delay to configure SPI frequency */
   drv_status_en  status;     /*!< toolbox driver status */
   uint8_t        CPOL  :1;   /*!< CPOL option setting */
   uint8_t        CPHA  :1;   /*!< CPHA option setting */
   uint8_t        NSS   :1;   /*!< Chip select pin control */
}spi_bb_t;


/*
 *  ============= PUBLIC SPI API =============
 */

/*
 * Link and Glue functions
 */
void spi_link_mosi (spi_bb_t *spi, drv_pinout_ft f);
void spi_link_miso (spi_bb_t *spi, drv_pinin_ft f);
void spi_link_sclk (spi_bb_t *spi, drv_pinout_ft f);
void spi_link_ss (spi_bb_t *spi, drv_pinout_ft f);

/*
 * Set functions
 */
void spi_set_freq (spi_bb_t *spi, uint32_t freq);     /* for compatibility */
void spi_set_cpol (spi_bb_t *spi, uint8_t cpol);      /* for compatibility */
void spi_set_cpha (spi_bb_t *spi, uint8_t cpha);      /* for compatibility */
void spi_set_nss  (spi_bb_t *spi, uint8_t nss);       /* for compatibility */

/*
 * User Functions
 */
void spi_deinit (spi_bb_t *spi);                  /* for compatibility */
drv_status_en spi_init (spi_bb_t *spi);           /* for compatibility */

byte_t spi_rw (spi_bb_t *spi, byte_t out);
drv_status_en spi_rx (spi_bb_t *spi, byte_t *buf, int count);
drv_status_en spi_tx (spi_bb_t *spi, byte_t *buf, int count);

drv_status_en spi_ioctl (spi_bb_t *spi, ioctl_cmd_t ctrl, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif //#ifndef  __spi_bb_h__
