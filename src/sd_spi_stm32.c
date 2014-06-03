/*
 * SD_spi_stm32.c : MMC/SDSC/SDHC (in SPI mode) control module for STM32
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 *               based on the AVR MMC module (C)ChaN, 2007
 *               and MMC/SDSC/SDHC example (C) Martin Thomas 2010
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

#include <drv/sd_spi_stm32.h>

static SD_t    SD;


/*!
 * \brief Check if SD Card is write protected.
 * \note This function works only if \a SD.IO.WP is connected.
 *       If not, it always returns false.
 */
static uint8_t _is_write_protected(void)
{
   if (!SD.IO.WP)
      return (uint8_t)0;
   else
      return (uint8_t) ( SD.IO.WP() ? 1:0);
}

/*!
 * \brief Check if SD Card is present.
 * \note This function works only if \sa SD.IO.CD is connected.
 *       If not, it always returns true.
 */
static uint8_t _is_present(void)
{
   if (!SD.IO.CD)
      return (uint8_t)1;
   else
      return (uint8_t)( SD.IO.CD() ? 1:0);
}

/*!
 * \brief Powers up or down the SD Card.
 * \note This function does nothing if there is no SD.IO.PW Connected.
 */
static void _sd_powerpin (uint8_t on)
{
   SD.pow = on;
   if (SD.IO.PW)
      SD.IO.PW(on);
}

/*!
 * \brief Check if SD Card is powered.
 */
static uint8_t _sd_chk_power (void)
{
   return SD.pow;
}

/*!
 * \brief   Card-select control (Platform dependent)
 *            Select: MMC.SD  CS = LOW
 *          DeSelect: MMC.SD  CS = HIGH
 * \param on High to Select, Low to deselect.
 */
static void _sd_select (uint8_t on)
{
   if (!SD.IO.CS) return;
   if (on)        SD.IO.CS (1);
   else           SD.IO.CS (0);
}

/*!
 * \brief
 *    Calculate SPI baud rate divider for the desired baud rate.
 *    The return value can be used as BR[2:0] part of the SPI_CR1
 *    register
 *
 * \param des_clk    Is the desired spi clock
 * \return BR[2:0] of the SPI_CR1 register, zero biased.
 */
static uint8_t _sd_baudrate_div (clock_t des_clk)
{
   uint32_t spi_clk;
   uint32_t clk;
   uint8_t  brdiv = 1;

   if (SD.IO.GetSPIFreq)      // Get SPI freq
      clk = SD.IO.GetSPIFreq ();
   else
      clk = CLOCK;
   // Calculate baud rate divider
   for (brdiv=1 ; brdiv <= 8 ; brdiv<<=1) {
      spi_clk = clk/(0x1 << brdiv);
      if (spi_clk <= des_clk)
         break;
   }
   if (brdiv <= 8)   // Have SPI/brdiv pair to match desired clock
      return brdiv;
   else              // Fail
      return 0;
}

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
static uint32_t _sd_csd2bautrate (uint8_t *csd)
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
 * \brief Set SPI clock speed.
 * \param sp.  The desired speed
 *       \arg  INTERFACE_SLOW    100kHz - 400kHz
 *       \arg  INTERFACE_FAST    fast clock depends on the CSD
 * \param csd  Pointer to CSD data buffer.
 */
static void _sd_iface_speed(SD_speed_en sp, uint8_t *csd)
{
   uint32_t cr1_reg;
   unsigned char  brdiv;  // For Baud rate divider

   cr1_reg = SD.IO.SPI->CR1;
   if ( sp == INTERFACE_SLOW )   // Set slow clock (100k-400k)
      brdiv = _sd_baudrate_div (400000);
   else                          // Set fast clock depending on the CSD
      brdiv = _sd_baudrate_div (_sd_csd2bautrate (csd));

   // Clear and update CR1
   cr1_reg &= ~SPI_BaudRatePrescaler_256;
   cr1_reg |= brdiv << 3;
   SD.IO.SPI->CR1 = cr1_reg;
}

/*!
 * \brief Transmit/Receive a byte to MMC via SPI.
 * \param out  The data to send to the SPI bus.
 * \return     The data received from SPI bus
 */
static sd_dat_t _sd_spi_rw(sd_dat_t out)
{
   // Send byte through the SPI peripheral
   SPI_I2S_SendData(SD.IO.SPI, out);
   // Wait to receive a byte
   while (SPI_I2S_GetFlagStatus(SD.IO.SPI, SPI_I2S_FLAG_RXNE) == RESET)
      ;
   // Return the byte read from the SPI bus
   return SPI_I2S_ReceiveData(SD.IO.SPI);
}

/*!
 * \brief Transmit a byte to MMC via SPI
 */
static inline void _sd_tx(sd_dat_t d) {
   _sd_spi_rw (d);
}

/*!
 * \brief Receive a byte from MMC via SPI
 */
static inline sd_dat_t _sd_rx(void) {
   return _sd_spi_rw ((sd_dat_t)0xFF);
}
/*! Alternative macro to receive data fast */
#define _sd_rx_m(_dst)  *(_dst)=_sd_spi_rw(0xFF)

/*!
 * \brief Wait for SD card ready. This function
 * has a timeout of \a SD_WAIT_TIMEOUT
 */
static sd_dat_t _sd_wait_ready (void)
{
   sd_dat_t res;
   SD.t2 = SD_WAIT_TIMEOUT;

   do
      res = _sd_rx();
   while ((res != 0xFF) && SD.t2);

   return res;
}

/*!
 * \brief Deselect SD Card and release SPI bus
 */
static void _sd_release (void)
{
   _sd_select(0);
   _sd_rx();      // Discard data from the bus
}

/*!
 * \brief SD Card's SPI configuration for initialization
 */
static void _sd_spi_init (void)
{
   SPI_InitTypeDef  SPI_InitStructure;

   // Check function pointers first
   if (!SD.IO.SPI)
      return;
   // Clock enable
   //SD.IO.SPI_CLK_EN(1);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

   // Struct preparation
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // As slow as we can get.
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;

   SPI_Init(SD.IO.SPI, &SPI_InitStructure);
   SPI_CalculateCRC(SD.IO.SPI, DISABLE);
   SPI_Cmd(SD.IO.SPI, ENABLE);
}

/*!
 * \brief SD Card's SPI configuration for de-initialization
 */
static void _sd_spi_deinit (void)
{
   // Check function pointers first
   if (!SD.IO.SPI_CLK_EN || !SD.IO.SPI)
      return;

   SPI_I2S_DeInit(SD.IO.SPI);
   SPI_Cmd(SD.IO.SPI, DISABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE);
   //SD.IO.SPI_CLK_EN(0);
}

/*!
 * \brief SD Card's Power Control and interface-initialization
 */
static void _sd_power_on (void)
{
   _sd_powerpin(1);

   SD.t2 = SD_POWER_TIMEOUT;     // Wait for \sa SD_POWER_TIMEOUT
   while (SD.t2)
      ;

   _sd_select(0);       // De-select the Card: Chip Select high
   _sd_spi_init ();     // SPI configuration

   // drain SPI
   while (SPI_I2S_GetFlagStatus(SD.IO.SPI, SPI_I2S_FLAG_TXE) == RESET)
      ;
   SPI_I2S_ReceiveData(SD.IO.SPI);
}

/*!
 * \brief SD Card's Power Control and interface-initialization
 */
static void _sd_power_off (void)
{
   if (!(SD.status & STA_NOINIT))
   {
      _sd_select(1);
      _sd_wait_ready();
      _sd_release();
   }

   _sd_spi_deinit();
   _sd_powerpin(0);
   // Set STA_NOINIT
   SD.status |= STA_NOINIT;
}

/*!
 * \brief      Receive a data packet from MMC/SD
 * \param      buff Pointer to data buffer to store received data
 * \param      n    Byte count (must be multiple of 4)
 * \return     true on success.
 */
static uint8_t _sd_rx_datablock (sd_dat_t *buff, uint32_t n)
{
   sd_dat_t token;

   /*!
    * Wait for data packet in timeout of \sa SD_RX_TIMEOUT
    */
   SD.t2 = SD_RX_TIMEOUT;
   do
      token = _sd_rx();
   while ((token == 0xFF) && SD.t2);

   if(token != 0xFE)    // return error for invalid token
      return 0;

   // Receive the data block into buffer
   do {
      _sd_rx_m(buff++);
      _sd_rx_m(buff++);
      _sd_rx_m(buff++);
      _sd_rx_m(buff++);
   } while (n -= 4);

   _sd_rx();            // Discard CRC
   _sd_rx();

   return 1;
}

/*!
 * \brief      Receive a data packet from MMC/SD
 * \param      buff 512 byte data block to be transmitted
 * \param      token Data/Stop token
 * \return     true on success.
 */
static uint8_t _sd_tx_datablock (const sd_dat_t *buff, sd_dat_t token)
{
   sd_dat_t r;
   int wc=512;

   if (_sd_wait_ready() != 0xFF)
      return 0;

   _sd_tx (token);  // transmit data token
   if (token != 0xFD) {
      /*
       * If is data token  transmit the 512 byte
       * data block to MMC/SD
       */
      do
         _sd_tx (*buff++);
      while (--wc);

      _sd_tx (0xFF);             // CRC (Dummy)
      _sd_tx (0xFF);
      r = _sd_rx();              // Receive data response
      if ((r & 0x1F) != 0x05)    // If not accepted, return with error
         return 0;
   }
   return 1;
}

/*!
 * \brief      Send a command packet to MMC/SD
 * \param      cmd  Command byte
 * \param      arg  Argument
 * \return     true on success.
 */
static sd_dat_t _sd_send_cmd (sd_dat_t cmd, uint32_t arg)
{
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
   _sd_select(0);
   _sd_select(1);
   if (_sd_wait_ready() != 0xFF)
      return 0xFF;      // Don't return the error

   // Send command packet
   _sd_tx(cmd);                  // Start + Command index
   _sd_tx((sd_dat_t)(arg>>24));  // Argument [31..24]
   _sd_tx((sd_dat_t)(arg>>16));  // Argument [23..16]
   _sd_tx((sd_dat_t)(arg>>8));   // Argument [15..8]
   _sd_tx((sd_dat_t)arg);        // Argument [7..0]
   n = 0x01;                     // Dummy CRC + Stop
   if (cmd == SD_CMD0) n = 0x95;    // Valid CRC for SD_CMD0(0)
   if (cmd == SD_CMD8) n = 0x87;    // Valid CRC for SD_CMD8(0x1AA)
   _sd_tx(n);

   // Receive command response
   if (cmd == SD_CMD12)
      _sd_rx();    // Skip a stuff byte when stop reading

   // Wait for a valid response in timeout of 10 attempts
   n = 10;
   do
      r = _sd_rx();
   while ((r & 0x80) && --n);

   return r;       // Return with the response value
}



/*============================   Public Functions   ============================ */


/*
 * Connect functions
 * --------------------------
 * Assign a driver function to IO struct.
 */
void SD_ConnectWP (volatile SD_PinIn_t pfun) {
   SD.IO.WP = pfun;
}
void SD_ConnectCD (volatile SD_PinIn_t pfun) {
   SD.IO.CD = pfun;
}
void SD_ConnectCS (volatile SD_PinOut_t pfun) {
   SD.IO.CS = pfun;
}
void SD_ConnectPW (volatile SD_PinOut_t pfun) {
   SD.IO.PW = pfun;
}
void SD_ConnectSPI (SPI_TypeDef* spi) {
   SD.IO.SPI = spi;
}
void SD_ConnectSPIClock (volatile SD_SPIClock_en_t pfun) {
   SD.IO.SPI_CLK_EN = pfun;
}
void SD_ConnectGetTickFreq (volatile SD_GetFreq_t pfun) {
   SD.IO.GetTickFreq = pfun;
}
void SD_ConnectGetSPIFreq (volatile SD_GetFreq_t pfun) {
   SD.IO.GetSPIFreq = pfun;
}



/*!
 * \brief De-Initialize SD Drive.
 *
 * \param   drv  Physical drive number (0)
 */
SD_Status_t SD_deinit (int8_t drv)
{
   _sd_power_off();
   SD.status |= STA_NOINIT;

   return SD.status;
}

/*!
 * \brief Initialize SD Drive.
 *
 * \param   drv  Physical drive number (0)
 */
SD_Status_t SD_init (int8_t drv)
{
   uint8_t n, cmd, type, ocr[4];
   uint8_t csd[16];

   _sd_powerpin(0);     // Initially power off the card
   if (drv)       // Supports only single drive
      return (SD.status |= STA_NOINIT);
   if (SD.status & STA_NODISK)  // No card in the socket
      return SD.status;

   _sd_power_on();   // Force socket power on and initialize interface
   _sd_iface_speed(INTERFACE_SLOW, (void*)0);
   for (n=10; n; --n) // 80 dummy clocks
      _sd_rx();

   type = 0;
   if (_sd_send_cmd(SD_CMD0, 0) == 1) { // Enter Idle state
      if (_sd_send_cmd(SD_CMD8, 0x1AA) == 1) { // SDHC
         for (n-0 ; n<4 ; ++n)   // Get trailing return value of R7 response
            ocr[n] = _sd_rx();
         if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
            // The card can work at VDD range of 2.7-3.6V
            // Wait for leaving idle state (SD_ACMD41 with HCS bit)
            SD.t1 = SD_INIT_TIMEOUT;    // Initialization timeout
            while (SD.t1 && _sd_send_cmd(SD_ACMD41, 1UL << 30))
               ;
            if (SD.t1 && _sd_send_cmd(SD_CMD58, 0) == 0){
               // Check CCS bit in the OCR
               for (n=0; n < 4; ++n)
                  ocr[n] = _sd_rx();
               type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
            }
         }
      } else { // SDSC or MMC
         if (_sd_send_cmd(SD_ACMD41, 0) <= 1) {  // SDSC
            type = CT_SD1; cmd = SD_ACMD41;
         } else { // MMC
            type = CT_MMC; cmd = SD_CMD1;
         }
         // Wait for leaving idle state
         SD.t1 = SD_INIT_TIMEOUT;    // Initialization timeout
         while (SD.t1 && _sd_send_cmd(cmd, 0))
            ;
         // Set R/W block length to 512 (For FAT compatibility)
         if (!SD.t1 || _sd_send_cmd(SD_CMD16, 512) != 0)
            type = 0;
      }
   }
   SD.type = type;
   _sd_release();    // Initialization ended

   if (type) {  // Initialization succeeded
      SD.status &= ~STA_NOINIT;    // Clear STA_NOINIT
      // Reads the maximum data transfer rate from CSD
      SD_ioctl(drv, CTRL_MMC_GET_CSD, csd);
      _sd_iface_speed(INTERFACE_FAST, csd);
   }
   else        // Initialization failed
      _sd_power_off();

   return SD.status;
}

/*!
 * \brief Get Disk Status
 *
 * \param   drv  Physical drive number (0)
 */
SD_Status_t SD_getstatus (int8_t drv)
{
   if (drv) // Supports only single drive
      return STA_NOINIT;
   return SD.status;
}

/*!
 * \brief Set Disk Status
 *
 * \param   drv   Physical drive number (0)
 * \param   st    Disk status
 */
SD_Status_t SD_setstatus (int8_t drv, SD_Status_t st)
{
   if (drv) // Supports only single drive
      return STA_NOINIT;
   return (SD.status = st);
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

