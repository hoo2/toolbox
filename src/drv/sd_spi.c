/*
 * \file sd_spi.c
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

#include <drv/sd_spi.h>

// Static functions
static uint32_t _csd2bautrate (uint8_t *csd);

static uint8_t  _is_present (sd_spi_t *sd);
static uint8_t  _is_write_protected (sd_spi_t *sd);
static uint8_t  _power_status (void);
static void     _power_pin (sd_spi_t *sd, uint8_t on);
static void     _select (sd_spi_t *sd, uint8_t on);
static sd_dat_t _spi_rw (sd_spi_t *sd, sd_dat_t out);
static void     _spi_tx (sd_spi_t *sd, sd_dat_t d);
static sd_dat_t _spi_rx (sd_spi_t *sd);
static sd_dat_t _wait_ready (sd_spi_t *sd);
static void     _release (sd_spi_t *sd);
static drv_status_en _spi_deinit (sd_spi_t *sd);
static drv_status_en _spi_init (sd_spi_t *sd);
static drv_status_en _power (sd_spi_t *sd, uint8_t on);
static uint8_t  _rx_datablock (sd_spi_t *sd, sd_dat_t *buff, uint32_t n);
static uint8_t  _tx_datablock (sd_spi_t *sd, const sd_dat_t *buff, sd_dat_t token);
static sd_dat_t _send_command (sd_spi_t *sd, sd_dat_t cmd, uint32_t arg);


/*!
 * \brief
 *    Calculate the maximum data transfer rate per one data line
 *    from the CSD.
 *    TRAN_SPEED is the CSD[103..96]
 *
 *    TRAN_SPEED bit code
 * ---------------------------------------------------
 *    2:0     | transfer rate unit
 *            | 0=100kbit/s, 1=1Mbit/s, 2=10Mbit/s,
 *            | 3=100Mbit/s, 4... 7=reserved
 *  ---------------------------------------------------
 *    6:3     | time value
 *            | 0=reserved, 1=1.0, 2=1.2, 3=1.3, 4=1.5,
 *            | 5=2.0, 6=2.5, 7=3.0, 8=3.5, 9=4.0, A=4.5,
 *            | B=5.0, C=5.5, D=6.0, E=7.0, F=8.0
 *  --------------------------------------------------
 *    7       | Reserved
 *
 * \param      csd  Pointer to CSD array 128bit.
 * \return     The maximum spi baud rate.
 */
static uint32_t _csd2bautrate (uint8_t *csd)
{
   uint8_t  brmul = 0;
   uint32_t br = 100000;     // 100Kbit

   // Mask [2..0] bits of TRAN_SPEED
   brmul = csd[3] & 0x07;
   while (brmul--)
      br *= 10;
   return br;
}

/*!
 * \brief
 *    Check if SD Card is present.
 *
 * \param   sd    Pointer to active sd structure.
 * \return        The sd card present status
 *    \arg  0     Is NOT present
 *    \arg  1     Is present.
 * \note
 *    This function works only if \a sd->sd_io.cd is linked.
 *    If not, it always returns present.
 */
static uint8_t _is_present (sd_spi_t *sd)
{
   if (!sd->sd_io.cd)
      return (uint8_t)1;
      /*
       * If no driver functionality, return present.
       */
   else
      return (uint8_t)( sd->sd_io.cd() ? 1:0);   // Cast to 0-1
}

/*!
 * \brief
 *    Check if SD Card is write protected.
 *
 * \param   sd    Pointer to active sd structure.
 * \return        The write protect status
 *    \arg  0     Is NOT write protected
 *    \arg  1     Is write protected.
 * \note
 *    This function works only if \a sd->sd_io.wp is linked.
 *    If not, it always returns NOT write protected.
 */
static uint8_t _is_write_protected (sd_spi_t *sd)
{
   if (!sd->sd_io.wp)
      return (uint8_t)0;
      /*
       * If no driver functionality, return write enable.
       */
   else
      return (uint8_t)(sd->sd_io.wp() ? 1:0);   // Cast to 0-1
}

/*!
 * \brief
 *    Check if SD Card is powered.
 * \param   sd    Pointer to active sd structure.
 * \return        The power status
 *    \arg  0     The SD is not powered
 *    \arg  1     The SD is powered
 */
static inline uint8_t _power_status (sd_spi_t *sd) {
   return sd->pow;
}

/*!
 * \brief
 *    Powers up or down the SD Card.
 *
 * \param   sd    Pointer to active sd structure.
 * \param   on    On/Off flag.
 * \return        none
 */
static void _power_pin (sd_spi_t *sd, uint8_t on)
{
   sd->pow = on;
   if (sd->sd_io.pw)
      sd->sd_io.pw (on);
}

/*!
 * \brief
 *    Card-select control
 *
 * \param   sd    Pointer to active sd structure.
 * \param   on    High to Select, Low to de-select.
 * \return        None.
 */
static void _select (sd_spi_t *sd, uint8_t on)
{
   if (sd->sd_io.cs)
      sd->sd_io.cs (on);

}

/*!
 * \brief
 *    Transmit/Receive a byte to SD/MMC via SPI.
 *
 * \param   sd    Pointer to active sd structure.
 * \param   out   The data to send to the SPI bus.
 * \return        The data received from SPI bus.
 */
static inline sd_dat_t _spi_rw (sd_spi_t *sd, sd_dat_t out)
{
   return sd->sd_io.spi_rw (sd->sd_io.spi, out);
}

/*!
 * \brief
 *    Transmit a byte to SD/MMC via SPI
 *
 * \param   sd    Pointer to active sd structure.
 * \param   d     The data to send to the SPI bus.
 * \return        None
 */
static inline void _spi_tx (sd_spi_t *sd, sd_dat_t d) {
   _spi_rw (sd, d);
}

/*!
 * \brief
 *    Receive a byte to SD/MMC via SPI.
 *
 * \param   sd    Pointer to active sd structure.
 * \return        The data received from SPI bus.
 */
static inline sd_dat_t _spi_rx (sd_spi_t *sd) {
   return (sd_dat_t) _spi_rw (sd, 0xFF);
}


/*!
 * \brief
 *    Wait for SD card ready.
 * \note
 *    This function has a timeout of \a SD_WAIT_TIMEOUT
 * \param   sd    Pointer to active sd structure.
 * return         Ready status as data
 *    \arg  0xFF  Ready
 *    \arg  !OxFF NOT ready.
 */
static sd_dat_t _wait_ready (sd_spi_t *sd)
{
   sd_dat_t res;
   sd->t2 = SD_WAIT_TIMEOUT;

   do
      res = _spi_rx ();
   while ((res != 0xFF) && sd->t2);

   return res;
}

/*!
 * \brief
 *    Deselect SD Card and release SPI bus
 * \param   sd    Pointer to active sd structure.
 * \return        None.
 */
static void _release (sd_spi_t *sd)
{
   _select (sd, 0);
   _spi_rx();     // Discard data from the bus
}


/*!
 * \brief
 *    De-Initialise the SPI bus for the SD/MMC.
 *
 * \param  sd  Pointer to active sd structure.
 * \return     The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static void _sd_spi_deinit (void)
{
   if (sd->sd_io.spi_ioctl)
      return sd->sd_io.spi_ioctl (sd->sd_io.spi, CTRL_DEINIT, NULL);
   else
      return DRV_ERROR;
}

/*!
 * \brief
 *    Initialise the SPI bus for the SD/MMC.
 *
 * \param  sd  Pointer to active sd structure.
 * \return     The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _spi_init (sd_spi_t *sd)
{
   if (sd->sd_io.spi_ioctl)
      return sd->sd_io.spi_ioctl (sd->sd_io.spi, CTRL_INIT, NULL);
   else
      return DRV_ERROR;
}

/*!
 * \brief
 *    SD Card's Power Control and interface-initialisation
 * \param   sd    Pointer to active sd structure.
 * \param   on    The power on/off flag.
 * \return        The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _power (sd_spi_t *sd, uint8_t on)
{
   if (on) {
      _power_pin (sd, 1);
      // Wait for \sa SD_POWER_TIMEOUT
      sd->t2 = SD_POWER_TIMEOUT;
      while (sd->t2)
         ;
      _select (sd, 1);           // Select the Card
      if ( _spi_init (sd) != DRV_READY)
         return sd->status = DRV_ERROR;
      _spi_rx (sd, 0xFF);        // drain SPI
      sd->status = DRV_READY;    // Set Status
   }
   else {
      if (!(sd->status == DRV_BUSY)) {
         _select (sd, 1);
         _wait_ready (sd);
      }
      _release (sd);             // De-select the Card
      _spi_deinit (sd);
      _power_pin (sd, 0);
      sd->status = DRV_NOINIT;   // Set Status
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Receive a data packet from MMC/SD
 *
 * \param   sd    Pointer to active sd structure.
 * \param   buff  Pointer to data buffer to store received data
 * \param   n     Byte count (must be multiple of 4)
 * \return        The operation status
 *    \arg  0     Fail
 *    \arg  1     Success.
 */
static uint8_t _rx_datablock (sd_spi_t *sd, sd_dat_t *buff, uint32_t n)
{
   #define _spi_rx_m(_data)   *(_data)=_spi_rw (sd, 0xFF)
   sd_dat_t token;

   /*!
    * Wait for data packet in timeout of \sa SD_RX_TIMEOUT
    */
   sd->t2 = SD_RX_TIMEOUT;
   do
      token = _spi_rx (sd);
   while ((token == 0xFF) && sd->t2);

   if(token != 0xFE)    // return error for invalid token
      return 0;

   /*!
    * Receive the data block into buffer and make sure
    * we receive multiples of 4
    */
   for (n+=(n%4) ? 4-(n%4):0 ; n>0 ; --n)
      _spi_rx_m (buff++);

   _sd_rx();            // Discard CRC
   _sd_rx();

   return 1;
   #undef _spi_rx_m
}

/*!
 * \brief
 *    Transmit a data block (512bytes) to MMC/SD
 *
 * \param   sd    Pointer to active sd structure.
 * \param   buff  Pointer to 512 byte data block to be transmitted
 * \param   token Data/Stop token
 * \return        The operation status
 *    \arg  0     Fail
 *    \arg  1     Success.
 */
static uint8_t _tx_datablock (sd_spi_t *sd, const sd_dat_t *buff, sd_dat_t token)
{
   #define _spi_txb_m(_data)   _spi_rw (sd, *(_data))
   #define _spi_tx_m(_data)    _spi_rw (sd, (_data))
   sd_dat_t r;
   int wc=512;

   if (_wait_ready(sd) != 0xFF)
      return 0;

   _spi_tx_m (token);  // transmit data token
   if (token != 0xFD) {
      /*
       * If is data token  transmit the 512 byte
       * data block to MMC/SD
       */
      do
         _spi_txb_m (buff++);
      while (--wc);

      _spi_tx_m (0xFF);          // CRC (Dummy)
      _spi_tx_m (0xFF);
      r = _spi_rx (sd);          // Receive data response
      if ((r & 0x1F) != 0x05)    // If not accepted, return with error
         return 0;
   }
   return 1;
   #undef _spi_txb_m
   #undef _spi_tx_m
}

/*!
 * \brief
 *    Send a command packet to SD/MMC
 *
 * \param   sd    Pointer to active sd structure.
 * \param   cmd   Command byte
 * \param   arg   Argument
 * \return        The operation status
 *    \arg  0     Fail
 *    \arg  1     Success.
 */
static sd_dat_t _send_command (sd_spi_t *sd, sd_dat_t cmd, uint32_t arg)
{
   #define _spi_tx_m(_data)    _spi_rw (sd, (_data))
   sd_dat_t n, r;

   if (cmd & 0x80) {
      /*!
       * SD_ACMD<n> is the command sequence of SD_CMD55-SD_CMD<n>
       */
      cmd &= 0x7F;
      r = _sd_send_cmd(SD_CMD55, 0);
      if (r > 1)
         return r;
   }

   // Select the card and wait for ready
   _select (sd, 0);
   _select (sd, 1);
   if (_wait_ready (sd) != 0xFF)
      return 0xFF;      // Don't return the error

   // Send command packet
   _spi_tx_m (cmd);                  // Start + Command index
   _spi_tx_m ((sd_dat_t)(arg>>24));  // Argument [31..24]
   _spi_tx_m ((sd_dat_t)(arg>>16));  // Argument [23..16]
   _spi_tx_m ((sd_dat_t)(arg>>8));   // Argument [15..8]
   _spi_tx_m ((sd_dat_t)arg);        // Argument [7..0]
   n = 0x01;                     // Dummy CRC + Stop
   if (cmd == SD_CMD0) n = 0x95;    // Valid CRC for SD_CMD0(0)
   if (cmd == SD_CMD8) n = 0x87;    // Valid CRC for SD_CMD8(0x1AA)
   _spi_tx_m (n);

   // Receive command response
   if (cmd == SD_CMD12)
      _spi_rx (sd);    // Skip a stuff byte when stop reading

   // Wait for a valid response in timeout of 10 attempts
   n = 10;
   do
      r = _spi_rx (sd);
   while ((r & 0x80) && --n);

   return r;       // Return with the response value
   #undef _spi_tx_m
}



/*============================   Public Functions   ============================ */


/*
 * Link and Glue functions
 */
inline void sd_link_wp (sd_spi_t *sd, drv_pinin_t fun) {
   sd->sd_io.wp = fun;
}
inline void sd_link_cd (sd_spi_t *sd, drv_pinin_t fun) {
   sd->sd_io.cd = fun;
}
inline void sd_link_cs (sd_spi_t *sd, drv_pinout_t fun) {
   sd->sd_io.cs = fun;
}
inline void sd_link_pw (sd_spi_t *sd, drv_pinout_t fun) {
   sd->sd_io.pw = fun;
}
inline void sd_link_spi_ioctl (sd_spi_t *sd, spi_ioctl_t fun) {
   sd->sd_io.spi_ioctl = fun;
}
inline void sd_link_spi_rw (sd_spi_t *sd, spi_rw_t fun) {
   sd->sd_io.spi_rw = fun;
}
inline void sd_link_spi (sd_spi_t *sd, void* spi) {
   sd->sd_io.spi = spi;
}

/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initialize SD Drive.
 *
 * \param   sd    Pointer to active sd structure.
 * \return  None
 */
void sd_deinit (sd_spi_t *sd)
{
   _power (sd, 0);
   memset ((sd)ee, 0, sizeof (sd_spi_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialise SD Drive.
 *
 * \param   sd    Pointer to active sd structure.
 * \return  None
 */
drv_status_en sd_init (sd_spi_t *sd)
{
   uint32_t speed;
   uint8_t n, cmd, type, ocr[4];
   uint8_t csd[16];

   _power_pin (sd, 0);           // Initially power off the card
   if (sd->status == DRV_NODEV)  // No card in the socket
      return sd->status;

   _power (sd, 1);      // Force socket power on and initialise interface
   speed = 400000;      // Start at lower speed
   sd->sd_io.spi_ioctl (sd->sd_io.spi, CTRL_SET_SPEED, (ioctl_buf_t*)&speed);
   for (n=10; n; --n)   // 80 dummy clocks
      _spi_rx (sd);

   type = 0;
   if (_send_command (sd, SD_CMD0, 0) == 1) {
      // Enter Idle state
      if (_send_command (sd, SD_CMD8, 0x1AA) == 1) {
         // SDHC
         for (n-0 ; n<4 ; ++n)   // Get trailing return value of R7 response
            ocr[n] = _spi_rx (sd);
         if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
            /*
             * The card can work at VDD range of 2.7-3.6V
             * Wait for leaving idle state (SD_ACMD41 with HCS bit)
             */
            sd->t1 = SD_INIT_TIMEOUT;    // Initialisation timeout
            while (sd->t1 && _send_command (sd, SD_ACMD41, 1UL << 30))
               ;
            if (sd->t1 && _send_command (sd, SD_CMD58, 0) == 0) {
               // Check CCS bit in the OCR
               for (n=0; n < 4; ++n)
                  ocr[n] = _spi_rx (sd);
               type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
            }
         }
      } else {
         // SDSC or MMC
         if (_send_command (sd, SD_ACMD41, 0) <= 1) {  // SDSC
            type = CT_SD1; cmd = SD_ACMD41;
         } else { // MMC
            type = CT_MMC; cmd = SD_CMD1;
         }
         // Wait for leaving idle state
         sd->t1 = SD_INIT_TIMEOUT;    // Initialisation timeout
         while (sd->t1 && _send_command (sd, cmd, 0))
            ;
         // Set R/W block length to 512 (For FAT compatibility)
         if (!sd->t1 || _send_command (sd, SD_CMD16, 512) != 0)
            type = 0;
      }
   }
   sd->type = type;
   _release (sd);    // Initialisation ended

   if (type) {
      // Initialisation succeeded
      sd->status = DRV_READY;
      // Reads the maximum data transfer rate from CSD
      sd_ioctl (sd, CTRL_MMC_GET_CSD, csd);
      speed = _csd2bautrate (csd);
      sd->sd_io.spi_ioctl (sd->sd_io.spi, CTRL_SET_SPEED, (ioctl_buf_t*)&speed);
   }
   else {
      // Initialisation failed
      _power (sd, 0);
      sd->status = DRV_ERROR;
   }
   return sd->status;
}

/*!
 * \brief
 *    Get Disk Status.
 *
 * \param   sd    Pointer to active sd structure.
 * \return  The status.
 */
inline drv_status_en sd_getstatus (sd_spi_t *sd) {
   return sd->status;
}

/*!
 * \brief
 *    Set Disk Status.
 *
 * \param   sd    Pointer to active sd structure.
 * \param   st    Disk status
 * \return  The updated status.
 */
inline drv_status_en sd_setstatus (sd_spi_t *sd, drv_status_en st) {
   return (sd->status = st);
}

/*!
 * \brief Read Sector(s)
 *
 * \param   drv   Physical drive number (0)
 * \param   buff  Pointer to the data buffer to store read data
 * \param   sector Start sector number (LBA)
 * \param   count  Sector count (1..255)
 */
SD_Result_t SD_read (uint8_t drv, sd_dat_t *buff, uint32_t sector, uint8_t count)
{
   if (drv || !count) return RES_PARERR;
   if (SD.status & STA_NOINIT) return RES_NOTRDY;

   if (!(SD.type & CT_BLOCK)) // Convert to byte address if needed
      sector *= 512;

   if (count == 1) { //Single block read
      if (_sd_send_cmd(SD_CMD17, sector) == 0)     // READ_SINGLE_BLOCK
         if (_sd_rx_datablock(buff, 512))
            count = 0;
   } else {          // Multiple block read
      if (_sd_send_cmd(SD_CMD18, sector) == 0) {   // READ_MULTIPLE_BLOCK
         do {
            if (!_sd_rx_datablock(buff, 512))
               break;
            buff += 512;
         } while (--count);
         _sd_send_cmd(SD_CMD12, 0);                // STOP_TRANSMISSION
      }
   }
   _sd_release();
   return (SD_Result_t) (count ? RES_ERROR : RES_OK);
}

/*!
 * \brief Write Sector(s)
 *
 * \param   drv   Physical drive number (0)
 * \param   buff  Pointer to the data to be written
 * \param   sector Start sector number (LBA)
 * \param   count  Sector count (1..255)
 */
SD_Result_t SD_write (uint8_t drv, const sd_dat_t *buff, uint32_t sector, uint8_t count)
{
   if (drv || !count) return RES_PARERR;
   if (SD.status & STA_NOINIT) return RES_NOTRDY;
   if (SD.status & STA_PROTECT) return RES_WRPRT;

   if (!(SD.type & CT_BLOCK)) // Convert to byte address if needed
      sector *= 512;

   if (count == 1) {    // Single block write
      if ((_sd_send_cmd(SD_CMD24, sector) == 0)  // WRITE_BLOCK
         && _sd_tx_datablock(buff, 0xFE))
         count = 0;
   } else {             // Multiple block write
      if (SD.type & CT_SDC)
         _sd_send_cmd(SD_ACMD23, count);
      if (_sd_send_cmd(SD_CMD25, sector) == 0) { // WRITE_MULTIPLE_BLOCK
         do {
            if (!_sd_tx_datablock(buff, 0xFC))
               break;
            buff += 512;
         } while (--count);
         if (!_sd_tx_datablock(0, 0xFD)) // STOP_TRAN token
            count = 1;
      }
   }
   _sd_release();
   return (SD_Result_t) (count ? RES_ERROR : RES_OK);
}

/*!
 * \brief Write Sector(s)
 *
 * \param   drv   Physical drive number (0)
 * \param   ctrl  Control code
 * \param   buff  Buffer to send/receive control data
 */
SD_Result_t SD_ioctl (uint8_t drv, sd_dat_t ctrl, void *buff)
{
   SD_Result_t res;
   uint8_t n, csd[16], *ptr = buff;
   uint32_t csize;

   if (drv)
      return RES_PARERR;

   res = RES_ERROR;

   if (ctrl == CTRL_POWER) {
      switch (*ptr)
      {
         case 0:     /* Sub control code == 0 (POWER_OFF) */
            if (_sd_chk_power())
               _sd_power_off();      /* Power off */
            res = RES_OK;
            break;
         case 1:     /* Sub control code == 1 (POWER_ON) */
            _sd_power_on();          /* Power on */
            res = RES_OK;
            break;
         case 2:     /* Sub control code == 2 (POWER_GET) */
            *(ptr+1) = (BYTE)_sd_chk_power();
            res = RES_OK;
            break;
         default :
            res = RES_PARERR;
      }
   } else {
      if (SD.status & STA_NOINIT) return RES_NOTRDY;

      switch (ctrl)
      {
         // Make sure that no pending write process
         case CTRL_SYNC :
            _sd_select(1);
            if (_sd_wait_ready() == 0xFF)
               res = RES_OK;
            break;
         // Get number of sectors on the disk (uint32_t)
         case CTRL_GET_SECTOR_COUNT :
            if ((_sd_send_cmd(SD_CMD9, 0) == 0) && _sd_rx_datablock(csd, 16)) {
               if ((csd[0] >> 6) == 1) { // SDC version 2.00
                  csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
                  *(uint32_t*)buff = (uint32_t)csize << 10;
               } else { // SDC version 1.XX or MMC
                  n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                  csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
                  *(uint32_t*)buff = (uint32_t)csize << (n - 9);
               }
               res = RES_OK;
            }
            break;
         // Get R/W sector size (uin16_t)
         case CTRL_GET_SECTOR_SIZE :
            *(uint16_t*)buff = 512;
            res = RES_OK;
            break;
         // Get erase block size in unit of sector (uint32_t)
         case CTRL_GET_BLOCK_SIZE :
            if (SD.type & CT_SD2) {  // SDC version 2.00
               if (_sd_send_cmd(SD_ACMD13, 0) == 0) {  /* Read SD status */
                  _sd_rx();
                  if (_sd_rx_datablock(csd, 16)) {  /* Read partial block */
                     for (n = 64 - 16; n; n--)     /* Purge trailing data */
                        _sd_rx();
                     *(uint32_t*)buff = 16UL << (csd[10] >> 4);
                     res = RES_OK;
                  }
               }
            } else {       // SDC version 1.XX or MMC
               if ((_sd_send_cmd(SD_CMD9, 0) == 0) && _sd_rx_datablock(csd, 16)) { // Read CSD
                  if (SD.type & CT_SD1)  // SDC version 1.XX
                     *(uint32_t*)buff = (((csd[10] & 63) << 1) + ((uint16_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
                  else                    // MMC
                     *(uint32_t*)buff = ((uint16_t)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                  res = RES_OK;
               }
            }
            break;
         // Get card type flags (1 byte)
         case CTRL_MMC_GET_TYPE :
            *ptr = SD.type;
            res = RES_OK;
            break;
         // Receive CSD as a data block (16 bytes)
         case CTRL_MMC_GET_CSD :
            if (_sd_send_cmd(SD_CMD9, 0) == 0      // READ_CSD
               && _sd_rx_datablock(ptr, 16))
               res = RES_OK;
            break;
         // Receive CID as a data block (16 bytes)
         case CTRL_MMC_GET_CID :
            if (_sd_send_cmd(SD_CMD10, 0) == 0     // READ_CID
               && _sd_rx_datablock(ptr, 16))
               res = RES_OK;
            break;
         // Receive OCR as an R3 response (4 bytes)
         case CTRL_MMC_GET_OCR :
            if (_sd_send_cmd(SD_CMD58, 0) == 0) {   // READ_OCR
               for (n = 4; n; --n)
                  *ptr++ = _sd_rx();
               res = RES_OK;
            }
            break;
         // Receive SD status as a data block (64 bytes)
         case CTRL_MMC_GET_SDSTAT :
            if (_sd_send_cmd(SD_ACMD13, 0) == 0) {  // SD_STATUS
               _sd_rx();
               if (_sd_rx_datablock(ptr, 64))
                  res = RES_OK;
            }
            break;

         default:
            res = RES_PARERR;
      }

      _sd_release();
   }
   return res;
}

/*!
 * \brief
 *    Wrapper to \sa SD.IO.GetTickFreq. Exporting this pointer
 *    will allow the user to use \sa SD_CRON_SET macro.
 */
inline clock_t  SD_GetTickFreq (void) {
   return (SD.IO.GetTickFreq) ? SD.IO.GetTickFreq() : 0;
}

/*!
 * \brief
 *    Inner timer functionality
 *    status update
 *
 * \note This function must be called in period of \sa SD_TIMEBASE_TICKS
 */
void SD_service (void)
{
   // Time base decrement timers
   if (SD.t1)     --SD.t1;
   if (SD.t2)     --SD.t2;

   // Get current status
   if ( _is_present() )
      SD.status &= ~STA_NODISK;
   else
      SD.status |= (STA_NODISK | STA_NOINIT);

   if (_is_write_protected())
      SD.status |= STA_PROTECT;
   else
      SD.status &= ~STA_PROTECT;
}

