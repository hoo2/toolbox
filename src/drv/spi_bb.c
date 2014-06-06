/*
 * \file spi_bb.c
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
#include <drv/spi_bb.h>

/*
 * Link and Glue functions
 */

/*!
 * \brief
 *    Links low-level MOSI function.
 * \param   spi   Pointer to active spi data structure.
 * \param   f     drv_pinout_t functionality to link
 * \return  None
 */
inline void spi_link_mosi (spi_bb_t *spi, drv_pinout_t f) {
   spi->mosi = f;
}

/*!
 * \brief
 *    Links low-level MISO function.
 * \param   spi   Pointer to active spi data structure.
 * \param   f     drv_pinin_t functionality to link
 * \return  None
 */
inline void spi_link_miso (spi_bb_t *spi, drv_pinin_t f) {
   spi->miso = f;
}

/*!
 * \brief
 *    Links low-level SCLK function.
 * \param   spi   Pointer to active spi data structure.
 * \param   f     drv_pinout_t functionality to link
 * \return  None
 */
inline void spi_link_sclk (spi_bb_t *spi, drv_pinout_t f) {
   spi->sclk = f;
}

/*!
 * \brief
 *    Links low-level chip select function.
 * \warning
 *    If not used the spi_set_nss (&spi, SPI_NSS_SOFT); must set.
 * \param   spi   Pointer to active spi data structure.
 * \param   f     drv_pinout_t functionality to link
 * \return  None
 */
inline void spi_link_ss (spi_bb_t *spi, drv_pinout_t f) {
   spi->ss = f;
}

/*
 * Set functions
 */

/*!
 * \brief
 *    Set SPI bus speed
 * \param   spi   Pointer to active spi data structure.
 * \param   freq  The frequency to set.
 * \return  None
 */
void spi_set_freq (spi_bb_t *spi, uint32_t freq) {

   spi->clk_delay = 500000 / freq;
   /*!<
    *                                1
    * clock delay = 2 * period = -----------
    *                            2*frequency
    * So:
    *                 1 * 10^6      500000
    * delay (usec) = ----------- = ---------
    *                 2 * freq       freq
    */
   if (!spi->clk_delay)    // Don't go under jiffy value
      spi->clk_delay = 1;
}

/*!
 * \brief
 *    Set SPI bus CPOL option
 * \param   spi   Pointer to active spi data structure.
 * \param   cpol  The CPOL option
 *    \arg  SPI_CPOL_IDLE_LOW    Clock idle low
 *    \arg  SPI_CPOL_IDLE_HIGH   Clock idle high
 * \return  None
 */
inline void spi_set_cpol (spi_bb_t *spi, uint8_t cpol) {
   spi->CPOL = cpol;
}

/*!
 * \brief
 *    Set SPI bus CPHA option
 * \param   spi   Pointer to active spi data structure.
 * \param   cpol  The CPHA option
 *    \arg  SPI_CPHA_1ST_EDGE    Data captured at first edge
 *    \arg  SPI_CPHA_2ND_EDGE    Data captured at second edge
 * \return  None
 */
inline void spi_set_cpha (spi_bb_t *spi, uint8_t cpha) {
   spi->CPHA = cpha;
}

/*!
 * \brief
 *    Set SPI bus SS option. If the option is set the driver
 *    handles SS pin.
 *
 * \param   spi   Pointer to active spi data structure.
 * \param   nss   The SS handle option
 *    \arg  SPI_NSS_SOFT    SS pin handled outside of the spi driver
 *    \arg  SPI_NSS_HARD    SS pin is handled from the spi driver.
 * \return  None
 */
inline void spi_set_nss  (spi_bb_t *spi, uint8_t nss) {
   spi->NSS = nss;
}

/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initialise the spi interface and leave sda pin in input state
 * \param  spi    pointer to active spi structure.
 * \return none
 */
void spi_deinit (spi_bb_t *spi)
{
   if (spi->sclk)             spi->sclk (spi->CPOL);
   if (spi->NSS && spi->ss)   spi->ss (0);

   // Clear data
   memset ((void*)spi, 0, sizeof (spi_bb_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialise the spi interface
 * \param  spi    pointer to active spi structure.
 * \return The driver status after init.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en spi_init (spi_bb_t *spi)
{
   if (!spi->mosi)            return DRV_ERROR;
   if (!spi->miso)            return DRV_ERROR;
   if (!spi->sclk)            return DRV_ERROR;
   if (spi->NSS && !spi->ss)  return DRV_ERROR;
   if (jf_probe () != DRV_READY)
      return spi->status = DRV_ERROR;

   // Init the bus
   spi->status = DRV_BUSY;
   spi->sclk (spi->CPOL);        // Release clock;
   if (spi->NSS)  spi->ss (0);   // Release slave
   return spi->status = DRV_READY;
}

/*!
 * \brief
 *    Transmit and receive a byte from the bus.
 * We support both CPOL and CPHA:
 *  ____
 *      |__________________________________   SS
 *              ________          ______
 *  XXX________|        |________|            CPOL:0
 *  XXX________          ________
 *             |________|        |______      CPOL:1
 *    |
 *    |        |        |        |
 *    |        |        |        |
 *  CI-D  DL  CA   DL CI-D  DL ...            CPHA:0
 *   CI   DL CA-D  DL CI-D  DL ...            CPHA:1
 *
 * CI: Clock idle
 * CA: Clock active
 * DL: Clock delay
 * D:  Delay
 *
 * \param  spi    pointer to active spi structure.
 * \param  out    pointer to the input buffer.
 * \return        the byte received from the bus.
 */
uint8_t spi_rw (spi_bb_t *spi, uint8_t out)
{
   int i;
   uint8_t  rc=0;

   // Prepare bus
   spi->sclk (spi->CPOL);              // Clock idle
   if (spi->NSS)  spi->ss (1);         // Select chip

   /*
    * Transmit/receive 8 bit MSB first
    */
   for (i=0 ; i<8 ; ++i) {
      if (!spi->CPHA) {                // Data (CPHA:0)
         spi->mosi (out & 0x80);  out<<=1;
         rc<<=1;  rc |= (spi->miso ()) ? 1:0;
      }
      jf_delay_us (spi->clk_delay);    // Delay
      spi->sclk (!spi->CPOL);          // Clock active
      if (spi->CPHA) {                 // Data (CPHA:1)
         spi->mosi (out & 0x80);  out<<=1;
         rc<<=1;  rc |= (spi->miso ()) ? 1:0;
      }
      jf_delay_us (spi->clk_delay);    // Delay
      spi->sclk (spi->CPOL);           // Clock idle
   }
   if (spi->NSS)  spi->ss (0);         // De-Select chip
   return rc;
}

/*!
 * \brief
 *    Receive data from spi to buffer.
 *
 * \param  spi    pointer to active spi structure.
 * \param  buf    pointer to the buffer.
 * \return        the status of operation.
 */
drv_status_en spi_rx (spi_bb_t *spi, spi_dat_t *buf, int count)
{
   uint8_t nss;

   if (count == 0)                  return DRV_ERROR;
   if (spi->status != DRV_READY)    return DRV_ERROR;

   spi->status = DRV_BUSY;
   // We take over NSS to control it once for entire operation
   nss = spi->NSS;
   spi->NSS = 0;

   if (nss) spi->ss (1);   // Select chip when ss is ours
   while (count--) {
      *buf++ = spi_rw (spi, 0xFF);
   }
   if (nss) spi->ss (0);   // De-Select chip when ss is ours

   spi->NSS = nss;         // Reset NSS option
   return DRV_READY;
}

/*!
 * \brief
 *    Transmit data from buffer to spi.
 *
 * \param  spi    pointer to active spi structure.
 * \param  buf    pointer to the buffer.
 * \return        the status of operation.
 */
drv_status_en spi_tx (spi_bb_t *spi, spi_dat_t *buf, int count)
{
   uint8_t nss;

   if (count == 0)                  return DRV_ERROR;
   if (spi->status != DRV_READY)    return DRV_ERROR;

   spi->status = DRV_BUSY;
   // We take over NSS to control it once for entire operation
   nss = spi->NSS;
   spi->NSS = 0;

   if (nss) spi->ss (1);   // Select chip when ss is ours
   while (count--) {
      spi_rw (spi, *buf++);
   }
   if (nss) spi->ss (0);   // De-Select chip when ss is ours

   spi->NSS = nss;         // Reset NSS option
   return DRV_READY;
}

/*!
 * \brief
 *    SPI ioctl function
 *
 * \param  spi    pointer to active spi structure.
 * \param  cmd    specifies the command to spi and get back the reply.
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_SET_CLOCK
 * \param  buf    pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en spi_ioctl (spi_bb_t *spi, ioctl_cmd_t ctrl, ioctl_buf_t buf)
{
   switch (ctrl)
   {
      case CTRL_GET_STATUS:      /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = spi->status;
         return DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
        spi_deinit(spi);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = spi_init(spi);
         else
            spi_init(spi);
         return DRV_READY;
      case CTRL_SET_CLOCK:        /*!< EEPROM size */
         spi_set_freq (spi, *(uint32_t*)buf);
         return DRV_READY;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}
