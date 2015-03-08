/*
 * \file sd_spi.c
 * \brief
 *    MMC/SDSC/SDHC control module in SPI mode.
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

static sd_spi_t sd;

// Static functions
static uint32_t _csd2bautrate (uint8_t *csd);

static uint8_t  _is_present (int drv);
static uint8_t  _is_write_protected (int drv);
static uint8_t  _power_status (int drv);
static void     _power_pin (int drv, uint8_t on);
static void     _select (int drv, uint8_t on);
static sd_dat_t _spi_rw (int drv, sd_dat_t out);
static void     _spi_tx (int drv, sd_dat_t d);
static sd_dat_t _spi_rx (int drv);
static sd_dat_t _wait_ready (int drv);
static void     _release (int drv);
static drv_status_en _spi_deinit (int drv);
static drv_status_en _spi_init (int drv);
static drv_status_en _power (int drv, uint8_t on);
static uint8_t  _rx_datablock (int drv, sd_dat_t *buf, uint32_t n);
static uint8_t  _tx_datablock (int drv, const sd_dat_t *buf, sd_dat_t token);
static sd_dat_t _send_command (int drv, sd_dat_t cmd, uint32_t arg);

/*
 * tools
 */
#define _bad_drive(_dr)    ((_dr)<0 || (_dr)>=SD_NUMBER_OF_DRIVES) ? 1:0

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
 * \param   drv   The number of physical drive to check
 * \return        The sd card present status
 *    \arg  0     Is NOT present
 *    \arg  1     Is present.
 * \note
 *    This function works only if \a sd.sd_io.cd is linked.
 *    If not, it always returns present.
 */
static uint8_t _is_present (int drv)
{
   if (!sd.sd_io[drv].cd)
      return (uint8_t)1;
      /*
       * If no card detect driver functionality, return present.
       */
   else
      return (uint8_t)( sd.sd_io[drv].cd() ? 1:0);   // Cast to 0-1
}

/*!
 * \brief
 *    Check if SD Card is write protected.
 *
 * \param   drv   The number of physical drive to check
 * \return        The write protect status
 *    \arg  0     Is NOT write protected
 *    \arg  1     Is write protected.
 * \note
 *    This function works only if \a sd.sd_io.wp is linked.
 *    If not, it always returns NOT write protected.
 */
static uint8_t _is_write_protected (int drv)
{
   if (!sd.sd_io[drv].wp)
      return (uint8_t)0;
      /*
       * If no driver functionality, return write enable.
       */
   else
      return (uint8_t)(sd.sd_io[drv].wp() ? 1:0);   // Cast to 0-1
}

/*!
 * \brief
 *    Check if SD Card is powered.
 * \param   drv   The number of physical drive to check
 * \return        The power status
 *    \arg  0     The drive is not powered
 *    \arg  1     The drive is powered
 */
static inline uint8_t _power_status (int drv) {
   return sd.drive[drv].pow;
}

/*!
 * \brief
 *    Powers up or down the SD Card.
 *
 * \param   drv   The number of physical drive.
 * \param   on    On/Off flag.
 * \return        none
 */
static void _power_pin (int drv, uint8_t on)
{
   sd.drive[drv].pow = on;
   if (sd.sd_io[drv].pw)
      sd.sd_io[drv].pw (on);
}

/*!
 * \brief
 *    Card-select control
 *
 * \param   drv   The number of physical drive.
 * \param   on    High to Select, Low to de-select.
 * \return        None.
 */
static void _select (int drv, uint8_t on)
{
   if (sd.sd_io[drv].cs)
      sd.sd_io[drv].cs (on);

}

/*!
 * \brief
 *    Transmit/Receive a byte to SD/MMC via SPI.
 *
 * \param   drv   The number of physical drive.
 * \param   out   The data to send to the SPI bus.
 * \return        The data received from SPI bus.
 */
static inline sd_dat_t _spi_rw (int drv, sd_dat_t out)
{
   return sd.sd_io[drv].spi_rw (sd.sd_io[drv].spi, out);
}

/*!
 * \brief
 *    Transmit a byte to SD/MMC via SPI
 *
 * \param   drv   The number of physical drive.
 * \param   d     The data to send to the SPI bus.
 * \return        None
 */
static inline void _spi_tx (int drv, sd_dat_t d) {
   _spi_rw (drv, d);
}

/*!
 * \brief
 *    Receive a byte to SD/MMC via SPI.
 *
 * \param   drv   The number of physical drive.
 * \return        The data received from SPI bus.
 */
static inline sd_dat_t _spi_rx (int drv) {
   return (sd_dat_t) _spi_rw (drv, 0xFF);
}


/*!
 * \brief
 *    Wait for SD card ready.
 * \note
 *    This function has a timeout of \a SD_WAIT_TIMEOUT
 * \param   drv   The number of physical drive.
 * return         Ready status as data
 *    \arg  0xFF  Ready
 *    \arg  !OxFF NOT ready.
 */
static sd_dat_t _wait_ready (int drv)
{
   sd_dat_t res;
   sd.drive[drv].t2 = SD_WAIT_TIMEOUT;

   do
      res = _spi_rx (drv);
   while ((res != 0xFF) && sd.drive[drv].t2);

   return res;
}

/*!
 * \brief
 *    Deselect SD Card and release SPI bus
 * \param   drv   The number of physical drive.
 * \return        None.
 */
static void _release (int drv)
{
   _select (drv, 0);
   _spi_rx (drv);     // Discard data from the bus
}


/*!
 * \brief
 *    De-Initialise the SPI bus for the SD/MMC.
 *
 * \param   drv   The number of physical drive.
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _spi_deinit (int drv)
{
   if (sd.sd_io[drv].spi_ioctl)
      return sd.sd_io[drv].spi_ioctl (sd.sd_io[drv].spi, CTRL_DEINIT, NULL);
   else
      return DRV_ERROR;
}

/*!
 * \brief
 *    Initialise the SPI bus for the SD/MMC.
 *
 * \param   drv   The number of physical drive.
 * \return  The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _spi_init (int drv)
{
   if (sd.sd_io[drv].spi_ioctl)
      return sd.sd_io[drv].spi_ioctl (sd.sd_io[drv].spi, CTRL_INIT, NULL);
   else
      return DRV_ERROR;
}

/*!
 * \brief
 *    SD Card's Power Control and interface-initialisation
 * \param   drv   The number of physical drive.
 * \param   on    The power on/off flag.
 * \return        The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _power (int drv, uint8_t on)
{
   if (on) {
      _power_pin (drv, 1);
      // Wait for \sa SD_POWER_TIMEOUT
      sd.drive[drv].t2 = SD_POWER_TIMEOUT;
      while (sd.drive[drv].t2)
         ;
      _select (drv, 0);                // De-Select the Card
      if ( _spi_init (drv) != DRV_READY) {
         _power_pin (drv, 0);
         return sd.drive[drv].status = DRV_ERROR;
      }
      _spi_rw (drv, 0xFF);             // drain SPI
      sd.drive[drv].status = DRV_READY;   // Set Status
   }
   else {
      if (!(sd.drive[drv].status == DRV_BUSY)) {
         _select (drv, 1);
         _wait_ready (drv);
      }
      _release (drv);                  // De-select the Card
      _spi_deinit (drv);
      _power_pin (drv, 0);
      sd.drive[drv].status = DRV_NOINIT;  // Set Status
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Receive a data packet from MMC/SD
 *
 * \param   drv   The number of physical drive.
 * \param   buf   Pointer to data buffer to store received data
 * \param   n     Byte count (must be multiple of 4)
 * \return        The operation status
 *    \arg  0     Fail
 *    \arg  1     Success.
 */
static uint8_t _rx_datablock (int drv, sd_dat_t *buf, uint32_t n)
{
   #define _spi_rx_m(_data)   *(_data)=_spi_rw (drv, 0xFF)
   sd_dat_t token;

   /*!
    * Wait for data packet in timeout of \sa SD_RX_TIMEOUT
    */
   sd.drive[drv].t2 = SD_RX_TIMEOUT;
   do
      token = _spi_rx (drv);
   while ((token == 0xFF) && sd.drive[drv].t2);

   if(token != 0xFE)    // return error for invalid token
      return 0;

   /*!
    * Receive the data block into buffer and make sure
    * we receive multiples of 4
    */
   for (n+=(n%4) ? 4-(n%4):0 ; n>0 ; --n)
      _spi_rx_m (buf++);

   _spi_rw (drv, 0xFF);  // Discard CRC
   _spi_rw (drv, 0xFF);

   #undef _spi_rx_m
   return 1;
}

/*!
 * \brief
 *    Transmit a data block (512bytes) to MMC/SD
 *
 * \param   drv   The number of physical drive.
 * \param   buf   Pointer to 512 byte data block to be transmitted
 * \param   token Data/Stop token
 * \return        The operation status
 *    \arg  0     Fail
 *    \arg  1     Success.
 */
static uint8_t _tx_datablock (int drv, const sd_dat_t *buf, sd_dat_t token)
{
   #define _spi_txb_m(_data)   _spi_rw (drv, *(_data))
   #define _spi_tx_m(_data)    _spi_rw (drv, (_data))
   sd_dat_t r;
   int wc=512;

   if (_wait_ready (drv) != 0xFF)
      return 0;

   _spi_tx_m (token);  // transmit data token
   if (token != 0xFD) {
      /*
       * If is data token  transmit the 512 byte
       * data block to MMC/SD
       */
      do
         _spi_txb_m (buf++);
      while (--wc);

      _spi_tx_m (0xFF);          // CRC (Dummy)
      _spi_tx_m (0xFF);
      r = _spi_rx (drv);          // Receive data response
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
 * \param   drv   The number of physical drive.
 * \param   cmd   Command byte
 * \param   arg   Argument
 * \return        The operation status
 *    \arg  0     Fail
 *    \arg  1     Success.
 */
static sd_dat_t _send_command (int drv, sd_dat_t cmd, uint32_t arg)
{
   #define _spi_tx_m(_data)    _spi_rw (drv, (_data))
   sd_dat_t n, r;

   if (cmd & 0x80) {
      /*!
       * SD_ACMD<n> is the command sequence of SD_CMD55-SD_CMD<n>
       */
      cmd &= 0x7F;
      r = _send_command (drv, SD_CMD55, 0);
      if (r > 1)
         return r;
   }

   // Select the card and wait for ready
   _select (drv, 0);
   _select (drv, 1);
   if (_wait_ready (drv) != 0xFF)
      return 0xFF;      // Don't return the error

   // Send command packet
   _spi_tx_m (cmd);                 // Start + Command index
   _spi_tx_m ((sd_dat_t)(arg>>24)); // Argument [31..24]
   _spi_tx_m ((sd_dat_t)(arg>>16)); // Argument [23..16]
   _spi_tx_m ((sd_dat_t)(arg>>8));  // Argument [15..8]
   _spi_tx_m ((sd_dat_t)arg);       // Argument [7..0]
   n = 0x01;                        // Dummy CRC + Stop
   if (cmd == SD_CMD0) n = 0x95;    // Valid CRC for SD_CMD0(0)
   if (cmd == SD_CMD8) n = 0x87;    // Valid CRC for SD_CMD8(0x1AA)
   _spi_tx_m (n);

   // Receive command response
   if (cmd == SD_CMD12)
      _spi_rx (drv);    // Skip a stuff byte when stop reading

   // Wait for a valid response in timeout of 10 attempts
   n = 10;
   do
      r = _spi_rx (drv);
   while ((r & 0x80) && --n);

   return r;       // Return with the response value
   #undef _spi_tx_m
}



/*============================   Public Functions   ============================ */


/*
 * Link and Glue functions
 */
inline void sd_link_wp (int drv, drv_pinin_ft fun) {
   if (_bad_drive(drv))
      return;
   sd.sd_io[drv].wp = fun;
}
inline void sd_link_cd (int drv, drv_pinin_ft fun) {
   if (_bad_drive(drv))
      return;
   sd.sd_io[drv].cd = fun;
}
inline void sd_link_cs (int drv, drv_pinout_ft fun) {
   if (_bad_drive(drv))
      return;
   sd.sd_io[drv].cs = fun;
}
inline void sd_link_pw (int drv, drv_pinout_ft fun) {
   if (_bad_drive(drv))
      return;
   sd.sd_io[drv].pw = fun;
}
inline void sd_link_spi_ioctl (int drv, spi_ioctl_t fun) {
   if (_bad_drive(drv))
      return;
   sd.sd_io[drv].spi_ioctl = fun;
}
inline void sd_link_spi_rw (int drv, spi_rw_t fun) {
   if (_bad_drive(drv))
      return;
   sd.sd_io[drv].spi_rw = fun;
}
inline void sd_link_spi (int drv, void* spi) {
   if (_bad_drive(drv))
      return;
   sd.sd_io[drv].spi = spi;
}

/*
 * User Functions
 */

/*!
 * \brief
 *    * Inner timer functionality
 *    * present status update
 *
 * \note This function must be called in period of \sa SD_TIMEBASE_TICKS
 */
void sd_service (void)
{
   int i;   // Drives
   static uint8_t pr[SD_NUMBER_OF_DRIVES];  // Inner present flag

   // Roll all drives
   for (i=0 ; i<SD_NUMBER_OF_DRIVES ; ++i) {
      // Time base decrement timers
      if (sd.drive[i].t1)     --sd.drive[i].t1;
      if (sd.drive[i].t2)     --sd.drive[i].t2;

      // Get current status
      if (pr[i] == 1 && !_is_present (i)) {
         sd.drive[i].status = DRV_NODEV;
         pr[i] = 0;
      }
      if (pr[i] == 0 && _is_present (i)) {
         sd.drive[i].status = DRV_NOINIT;
         pr[i] = 1;
      }
   }
}

/*!
 * \brief
 *    De-Initialize SD Drive.
 *
 * \param   drv   The number of physical drive.
 * \return  None
 */
void sd_deinit (int drv)
{
   if (_bad_drive(drv))
      return;
   _power (drv, 0);
   memset ((void*)&sd.sd_io[drv], 0, sizeof (sd_io_t));
   memset ((void*)&sd.drive[drv], 0, sizeof (sd_data_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialise SD Drive.
 *
 * \param   drv   The number of physical drive.
 * \return  The status of the operation
 *    \arg  DRV_ERROR   On error.
 *    \arg  DRV_READY   On success.
 */
drv_status_en sd_init (int drv)
{
   #define _bad_link(_link)   (!sd.sd_io[drv]._link) ? 1:0

   uint32_t clk;
   uint8_t n, cmd, type, ocr[4];
   uint8_t csd[16];

   if (_bad_drive(drv))       return DRV_ERROR;
   if (_bad_link(cs))         return DRV_ERROR;
   if (_bad_link(spi))        return DRV_ERROR;
   if (_bad_link(spi_ioctl))  return DRV_ERROR;
   if (_bad_link(spi_rw))     return DRV_ERROR;

   _power_pin (drv, 0);                      // Initially power off the card
   if (!_is_present (drv)) {                 // No card in the socket
      sd.drive[drv].status = DRV_NODEV;
      return DRV_ERROR;
   }
   sd.drive[drv].status = DRV_BUSY;
   if (_power (drv, 1) == DRV_ERROR) { // Force socket power on and initialise interface
      return DRV_ERROR;
   }
   clk = 400000;      // Start at lower clk
   sd.sd_io[drv].spi_ioctl (sd.sd_io[drv].spi, CTRL_SET_CLOCK, (ioctl_buf_t)&clk);
   for (n=10; n; --n)   // 80 dummy clocks
      _spi_rx (drv);

   type = 0;
   if (_send_command (drv, SD_CMD0, 0) == 1) {
      // Enter Idle state
      if (_send_command (drv, SD_CMD8, 0x1AA) == 1) {
         // SDHC
         for (n-0 ; n<4 ; ++n)   // Get trailing return value of R7 response
            ocr[n] = _spi_rx (drv);
         if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
            /*
             * The card can work at VDD range of 2.7-3.6V
             * Wait for leaving idle state (SD_ACMD41 with HCS bit)
             */
            sd.drive[drv].t1 = SD_INIT_TIMEOUT;    // Initialisation timeout
            while (sd.drive[drv].t1 && _send_command (drv, SD_ACMD41, 1UL << 30))
               ;
            if (sd.drive[drv].t1 && _send_command (drv, SD_CMD58, 0) == 0) {
               // Check CCS bit in the OCR
               for (n=0; n < 4; ++n)
                  ocr[n] = _spi_rx (drv);
               type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
            }
         }
      } else {
         // SDSC or MMC
         if (_send_command (drv, SD_ACMD41, 0) <= 1) {  // SDSC
            type = CT_SD1; cmd = SD_ACMD41;
         } else { // MMC
            type = CT_MMC; cmd = SD_CMD1;
         }
         // Wait for leaving idle state
         sd.drive[drv].t1 = SD_INIT_TIMEOUT;    // Initialisation timeout
         while (sd.drive[drv].t1 && _send_command (drv, cmd, 0))
            ;
         // Set R/W block length to 512 (For FAT compatibility)
         if (!sd.drive[drv].t1 || _send_command (drv, SD_CMD16, 512) != 0)
            type = 0;
      }
   }
   sd.drive[drv].type = type;
   _release (drv);    // Initialisation ended

   if (type) {
      // Initialisation succeeded
      sd.drive[drv].status = DRV_READY;
      // Reads the maximum data transfer rate from CSD
      sd_ioctl (drv, CTRL_MMC_GET_CSD, csd);
      clk = _csd2bautrate (csd);
      sd.sd_io[drv].spi_ioctl (sd.sd_io[drv].spi, CTRL_SET_CLOCK, (ioctl_buf_t)&clk);
   }
   else {
      // Initialisation failed
      _power (drv, 0);
      sd.drive[drv].status = DRV_ERROR;
   }
   return sd.drive[drv].status;
}

/*!
 * \brief
 *    Get Disk Status.
 *
 * \param   drv   The number of physical drive.
 * \return  The status.
 */
inline drv_status_en sd_getstatus (int drv) {
   if (_bad_drive(drv))
      return DRV_ERROR;
   return sd.drive[drv].status;
}

/*!
 * \brief
 *    Set Disk Status.
 *
 * \param   drv   The number of physical drive.
 * \param   st    Disk status
 * \return  The updated status.
 */
inline drv_status_en sd_setstatus (int drv, drv_status_en st) {
   if (_bad_drive(drv))
      return DRV_ERROR;
   return (sd.drive[drv].status = st);
}

/*!
 * \brief
 *    Read Sector(s)
 *
 * \param   drv    The number of physical drive.
 * \param   sector Start sector number (LBA)
 * \param   buf    Pointer to the data buffer to store read data
 * \param   count  Sector (512 bytes) count (1..255)
 * \return  The status of the operation
 *    \arg  DRV_ERROR   On error.
 *    \arg  DRV_READY   On success.
 */
drv_status_en sd_read (int drv, sd_idx_t sector, sd_dat_t *buf, size_t count)
{
   if (_bad_drive(drv))             return DRV_ERROR;
   if (!count)                      return DRV_ERROR;
   if (sd.drive[drv].status != DRV_READY)
      return DRV_ERROR;

   sd.drive[drv].status = DRV_BUSY;
   if (!(sd.drive[drv].type & CT_BLOCK)) // Convert to byte address if needed
      sector *= 512;

   if (count == 1) { //Single block read
      if (_send_command (drv, SD_CMD17, sector) == 0)     // READ_SINGLE_BLOCK
         if (_rx_datablock (drv, buf, 512))
            count = 0;
   } else {          // Multiple block read
      if (_send_command (drv, SD_CMD18, sector) == 0) {   // READ_MULTIPLE_BLOCK
         do {
            if (!_rx_datablock (drv, buf, 512))
               break;
            buf += 512;
         } while (--count);
         _send_command (drv, SD_CMD12, 0);                // STOP_TRANSMISSION
      }
   }
   _release (drv);
   return (drv_status_en) (sd.drive[drv].status = count ? DRV_ERROR : DRV_READY);
}

/*!
 * \brief
 *    Write Sector(s)
 *
 * \param   drv   The number of physical drive.
 * \param   sector Start sector number (LBA)
 * \param   buf    Pointer to the data to be written
 * \param   count  Sector(512 bytes) count (1..255)
 * \return  The status of the operation
 *    \arg  DRV_ERROR   On error.
 *    \arg  DRV_READY   On success.
 */
drv_status_en sd_write (int drv, sd_idx_t sector, const sd_dat_t *buf, size_t count)
{
   if (_bad_drive(drv))             return DRV_ERROR;
   if (_is_write_protected (drv))   return DRV_ERROR;
   if (!count)                      return DRV_ERROR;
   if (sd.drive[drv].status != DRV_READY)
      return DRV_ERROR;

   sd.drive[drv].status = DRV_BUSY;
   if (!(sd.drive[drv].type & CT_BLOCK)) // Convert to byte address if needed
      sector *= 512;

   if (count == 1) {    // Single block write
      if ((_send_command (drv, SD_CMD24, sector) == 0) // WRITE_BLOCK
         && _tx_datablock (drv, buf, 0xFE))
         count = 0;
   } else {             // Multiple block write
      if (sd.drive[drv].type & CT_SDC)
         _send_command (drv, SD_ACMD23, count);
      if (_send_command (drv, SD_CMD25, sector) == 0) { // WRITE_MULTIPLE_BLOCK
         do {
            if (!_tx_datablock (drv, buf, 0xFC))
               break;
            buf += 512;
         } while (--count);
         if (!_tx_datablock (drv, 0, 0xFD)) // STOP_TRAN token
            count = 1;
      }
   }
   _release (drv);
   return (drv_status_en) (sd.drive[drv].status = count ? DRV_ERROR : DRV_READY);
}

/*!
 * \brief
 *    Write Sector(s)
 *
 * \param   drv   The number of physical drive.
 * \param  cmd    specifies the command to SD/MMC and get back the reply.
 *    \arg CTRL_POWER
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_SYNC
 *    \arg CTRL_GET_SECTOR_COUNT
 *    \arg CTRL_GET_SECTOR_SIZE
 *    \arg CTRL_GET_BLOCK_SIZE
 *    \arg CTRL_MMC_GET_TYPE
 *    \arg CTRL_MMC_GET_CSD
 *    \arg CTRL_MMC_GET_CID
 *    \arg CTRL_MMC_GET_OCR
 *    \arg CTRL_MMC_GET_SDSTAT
 * \param  buf    Pointer to buffer for ioctl
 * \return  The status of the operation
 *    \arg  DRV_ERROR   On error.
 *    \arg  DRV_READY   On success.
 */
drv_status_en sd_ioctl (int drv, ioctl_cmd_t ctrl, ioctl_buf_t buf)
{
   drv_status_en res = DRV_ERROR;
   uint8_t n, csd[16], *ptr = buf;
   uint32_t csize;

   if (_bad_drive(drv))
      return DRV_ERROR;
   if ( !(sd.drive[drv].status == DRV_READY || sd.drive[drv].status == DRV_NOINIT) )
      return DRV_ERROR;
   //if (ctrl != CTRL_POWER && sd.drive[drv].status == DRV_NOINIT)
   //   return DRV_ERROR;

   switch (ctrl)
   {
      case CTRL_POWER:
         switch (*ptr)
         {
            case 0:     /* Sub control code == 0 (POWER_OFF) */
               if (_power_status (drv))
                  *(ptr+1) = _power (drv, 0);   /* Power off */
               return DRV_READY;
            case 1:     /* Sub control code == 1 (POWER_ON) */
               *(ptr+1) = _power (drv, 1);      /* Power on */
               return DRV_READY;
            case 2:     /* Sub control code == 2 (POWER_GET) */
               *(ptr+1) = _power_status (drv);
               return DRV_READY;
            default:
               return DRV_ERROR;
         }
         break;

      case CTRL_DEINIT:
         sd_deinit (drv);
         return DRV_READY;

      case CTRL_INIT:
         res = sd_init (drv);
         if (buf)
            *(drv_status_en*)buf = res;
         return res;

      case CTRL_SYNC :     // Make sure that no pending write process
         _select (drv, 1);
         if (_wait_ready (drv) == 0xFF)
            res = DRV_READY;
         else
            res = DRV_BUSY;
         if (buf)
            *(drv_status_en*)buf = res;
         return res;

      case CTRL_GET_SECTOR_COUNT:      // Get number of sectors on the disk (uint32_t)
         if ((_send_command (drv, SD_CMD9, 0) == 0) && _rx_datablock (drv, csd, 16)) {
            if ((csd[0] >> 6) == 1) {
               // SDC version 2.00
               csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
               *(uint32_t*)buf = (uint32_t)csize << 10;
            }
            else {
               // SDC version 1.XX or MMC
               n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
               csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
               *(uint32_t*)buf = (uint32_t)csize << (n - 9);
            }
            res = DRV_READY;
         }
         return res;

      case CTRL_GET_SECTOR_SIZE: // Get R/W sector size (uin16_t)
         *(uint16_t*)buf = 512;
         return DRV_READY;

      case CTRL_GET_BLOCK_SIZE:  // Get erase block size in unit of sector (uint32_t)
         if (sd.drive[drv].type & CT_SD2) {
            // SDC version 2.00
            if (_send_command (drv, SD_ACMD13, 0) == 0) {
               /* Read SD status */
               _spi_rx (drv);
               if (_rx_datablock (drv, csd, 16)) {  /* Read partial block */
                  for (n = 64 - 16; n; n--)        /* Purge trailing data */
                     _spi_rx (drv);
                  *(uint32_t*)buf = 16UL << (csd[10] >> 4);
                  res = DRV_READY;
               }
            }
         }
         else {
            // SDC version 1.XX or MMC
            if ((_send_command (drv, SD_CMD9, 0) == 0) && _rx_datablock (drv, csd, 16)) { // Read CSD
               if (sd.drive[drv].type & CT_SD1)  // SDC version 1.XX
                  *(uint32_t*)buf = (((csd[10] & 63) << 1) + ((uint16_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
               else                    // MMC
                  *(uint32_t*)buf = ((uint16_t)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
               res = DRV_READY;
            }
         }
         return res;

      case CTRL_MMC_GET_TYPE:    // Get card type flags (1 byte)
         *ptr = sd.drive[drv].type;
         return DRV_READY;

      case CTRL_MMC_GET_CSD:     // Receive CSD as a data block (16 bytes)
         if (_send_command (drv, SD_CMD9, 0) == 0 && _rx_datablock (drv, ptr, 16))    // READ_CSD
            res = DRV_READY;
         return res;

      case CTRL_MMC_GET_CID :    // Receive CID as a data block (16 bytes)
         if (_send_command (drv, SD_CMD10, 0) == 0 && _rx_datablock (drv, ptr, 16))   // READ_CID
            res = DRV_READY;
         return res;

      case CTRL_MMC_GET_OCR :    // Receive OCR as an R3 response (4 bytes)
         if (_send_command (drv, SD_CMD58, 0) == 0) {   // READ_OCR
            for (n = 4; n; --n)
               *ptr++ = _spi_rx (drv);
            res = DRV_READY;
         }
         return res;

      case CTRL_MMC_GET_SDSTAT : // Receive SD status as a data block (64 bytes)
         if (_send_command (drv, SD_ACMD13, 0) == 0) {  // SD_STATUS
            _spi_rx (drv);
            if (_rx_datablock (drv, ptr, 64))
               res = DRV_READY;
         }
         return res;

      default:
         return DRV_ERROR;
   }

   _release (drv);
   return DRV_ERROR;
}

#undef _bad_drive
