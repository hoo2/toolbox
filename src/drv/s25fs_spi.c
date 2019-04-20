/*!
 * \file s25fs_spi.c
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
#include <drv/s25fs_spi.h>


static drv_status_en  _read (s25fs_t *drv, s25fs_cmd_t cmd, s25fs_idx_t idx, int al, s25fs_data_t *buf, int len);
static drv_status_en _write (s25fs_t *drv, s25fs_cmd_t cmd, s25fs_idx_t idx, int al, s25fs_data_t *buf, int len);

static drv_status_en _cmd_RDSR1 (s25fs_t *drv, byte_t *sr);
static drv_status_en _cmd_WREN (s25fs_t *drv);
static drv_status_en _cmd_WRDI (s25fs_t *drv);
static drv_status_en _cmd_SE (s25fs_t *drv, s25fs_idx_t idx);

static int _wait_ready (s25fs_t *drv);
static int  _writepage (s25fs_t *drv, s25fs_idx_t idx, byte_t *buf, int n);
/*!
 * \brief
 *    Read data from FLASH drive by sending a read command before. The
 *    command must be a 4-byte command.
 *
 * \param   drv   Pointer to the drive to use
 * \param   cmd   The (read kind) command to perform
 * \param   idx   The flash address
 * \param    al   The address byte size
 * \param   buf   Pointer to the buffer to store the data
 * \param   len   The number of bytes to read
 *
 * \return The status of Operation
 *    \arg  DRV_ERROR      A communication error occurred
 *    \arg  DRV_READY      Done, no error
 */
static drv_status_en
 _read (s25fs_t *drv, s25fs_cmd_t cmd, s25fs_idx_t idx, int al, s25fs_data_t *buf, int len)
{
   byte_t _idx[4];

   PUT_UINT32_BE(idx, _idx, 0);     // Get MSB first

   // Read operation
   drv->io.cs (S25FS_EN);
   // Command
   if ( drv->io.spi_write (drv->io.spi, &cmd, 1) != DRV_READY )
      return DRV_ERROR;
   // Address
   if (al && drv->io.spi_write (drv->io.spi, (byte_t*)&_idx, al) != DRV_READY )
      return DRV_ERROR;
   // Data
   if (len && drv->io.spi_read (drv->io.spi, buf, len) != DRV_READY )
      return DRV_ERROR;
   drv->io.cs (S25FS_DIS);

   return DRV_READY;
}

/*!
 * \brief
 *    Writes data to FLASH drive by sending a write command before. The
 *    command must be a 4-byte command.
 *
 * \param   drv   Pointer to the drive to use
 * \param   cmd   The (write kind) command to perform
 * \param   idx   The flash address
 * \param    al   The address byte size
 * \param   buf   Pointer to the buffer to store the data
 * \param   len   The number of bytes to write
 *
 * \return The status of Operation
 *    \arg  DRV_ERROR      A communication error occurred
 *    \arg  DRV_READY      Done, no error
 */
static drv_status_en
 _write (s25fs_t *drv, s25fs_cmd_t cmd, s25fs_idx_t idx, int al, s25fs_data_t *buf, int len)
{
   byte_t _idx[4];

   PUT_UINT32_BE(idx, _idx, 0);     // Get MSB first

   // write operation
   drv->io.cs (S25FS_EN);
   // Command
   if ( drv->io.spi_write (drv->io.spi, &cmd, 1) != DRV_READY )
      return DRV_ERROR;
   // Address
   if (al && drv->io.spi_write (drv->io.spi, (byte_t*)&_idx, al) != DRV_READY )
      return DRV_ERROR;
   // Data
   if (len && drv->io.spi_write (drv->io.spi, buf, len) != DRV_READY )
      return DRV_ERROR;
   drv->io.cs (S25FS_DIS);

   return DRV_READY;
}

/*!
 * \brief
 *    Send a RDSR1 "Read status register 1" command to flash
 *
 * \param   drv   Pointer to the drive to use
 * \param    sr   Pointer to return the SR1 data
 *
 * \return The status of Operation
 *    \arg  DRV_ERROR      A read error occurred
 *    \arg  DRV_READY      Done, no error
 */
static drv_status_en _cmd_RDSR1 (s25fs_t *drv, byte_t *sr)
{
   s25fs_data_t cmd = S25FS_RDSR_CMD;

   if ( _read (drv, cmd, ADDRESS_NOT_USED, 0, sr, 1) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

/*!
 * \brief
 *    Send a WREN "Write enable" command to flash
 *
 * \param   drv   Pointer to the drive to use
 *
 * \return The status of Operation
 *    \arg  DRV_ERROR      A read error occurred
 *    \arg  DRV_READY      Done, no error
 */
static drv_status_en _cmd_WREN (s25fs_t *drv)
{
   s25fs_data_t cmd = S25FS_WREN_CMD;

   if ( _write (drv, cmd, ADDRESS_NOT_USED, 0, BUFFER_NOT_USED, 0) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

/*!
 * \brief
 *    Send a WRDI "Write disable" command to flash
 *
 * \param   drv   Pointer to the drive to use
 *
 * \return The status of Operation
 *    \arg  DRV_ERROR      A read error occurred
 *    \arg  DRV_READY      Done, no error
 */
static drv_status_en _cmd_WRDI (s25fs_t *drv)
{
   s25fs_data_t cmd = S25FS_WRDI_CMD;

   if ( _write (drv, cmd, ADDRESS_NOT_USED, 0, BUFFER_NOT_USED, 0) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

/*!
 * \brief
 *    Send a SE "Sector erase" command to flash
 * \note
 *    If the requested address is inside sector range it
 *    still erase the sector from the sector starting address.
 *
 * \param   drv   Pointer to the drive to use
 * \param   idx   The sector address.
 *
 * \return The status of Operation
 *    \arg  DRV_ERROR      A read error occurred
 *    \arg  DRV_READY      Done, no error
 */
static drv_status_en _cmd_SE (s25fs_t *drv, s25fs_idx_t idx)
{
   s25fs_data_t cmd = S25FS_SE_4B_CMD;

   if ( _write (drv, cmd, idx, 4, BUFFER_NOT_USED, 0) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

/*
* \brief
*    Wait for flash to become ready.
* \note
*    This function has a timeout of \a S25FS_TIMEOUT
*
* \param   drv   Pointer indicate the flash data stuct to use
* \return        Ready status
*    \arg  1  Ready
*    \arg  0  NOT ready.
*/
static int _wait_ready (s25fs_t *drv)
{
   int ms=0;      // max msec timeout
   byte_t sr;

   do {
      _cmd_RDSR1 (drv, &sr);
      if (!(sr & 0x01))    return 1;
      if (ms)              jf_delay_ms (1);
   } while (++ms<S25FS_TIMEOUT);

   return 0;
}

/*!
 * \brief
 *    Writes a number of data to the FLASH starting from \c idx
 *    till it reaches the end of the FLASH page, Even if buf contains
 *    more data.
 *    Returns the number of written bytes, to help \see s25fs_write_buffer()
 *
 * \param  drv   Pointer indicate the flash data stuct to use
 * \param  idx   The starting address of the FLASH
 * \param  buf   Pointer to data to write
 * \param    n   The number of bytes to write
 *
 * \return
 *    The number of written bytes
 */
static int _writepage (s25fs_t *drv, s25fs_idx_t idx, byte_t *buf, int n)
{
   // Page start and page offset and num to write
   int pg_offset = idx % drv->conf.write_page_sz;
   int nl = drv->conf.write_page_sz - pg_offset; // num up saturation

   if (nl > n)  nl = n;   // Cut out the unnecessary bytes

   if ( !_wait_ready (drv) )
      return -1;

   if ( _write (drv, S25FS_PP_4B_CMD, idx, 4, buf, nl) != DRV_READY )
      return -1;

   return nl;
}




/*
 *  ============= PUBLIC S25FS API =============
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief
 *    Link write protect pin to driver
 * \note Optional
 */
void s25fs_link_wp (s25fs_t *drv, drv_pinout_ft fun) {
   drv->io.wp = fun;
}
/*!
 * \brief
 *    Link chip select pin to driver
 */
void s25fs_link_cs (s25fs_t *drv, drv_pinout_ft fun) {
   drv->io.cs = fun;
}
/*!
 * \brief
 *    Link spi bus read functionality to driver
 */
void s25fs_link_spi_read (s25fs_t *drv, s25fs_spi_rw_t fun) {
   drv->io.spi_read = fun;
}
/*!
 * \brief
 *    Link spi bus write functionality to driver
 */
void s25fs_link_spi_write (s25fs_t *drv, s25fs_spi_rw_t fun) {
   drv->io.spi_write = fun;
}
/*!
 * \brief
 *    Link application or Low level driver spi data struct to s25fs.
 *    If the application uses a hardware SPI version use NULL.
 *
 * \param  drv    pointer to active s25fs_t structure.
 * \param  spi    Pointer to SPI data struct to link, or NULL for simple
 *                or hardware implementations.
 */
void s25fs_link_spi (s25fs_t *drv, void* spi) {
   drv->io.spi = spi;
}




/*
 * Set functions
 */

/*!
 * \brief
 *    Set flash write page size
 */
void s25fs_set_write_page_sz (s25fs_t *drv, uint32_t size) {
   drv->conf.write_page_sz = size;
}
/*!
 * \brief
 *    Set flash erase sector size
 */
void s25fs_set_erase_page_sz (s25fs_t *drv, uint32_t size) {
   drv->conf.erase_page_sz = size;
}
/*!
 * \brief
 *    Set flash virtual sector size
 */
void s25fs_set_sector_size (s25fs_t *drv, uint32_t size) {
   drv->conf.sector_sz = size;
}




/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initialize s25fs flash pointed by \a drv
 *
 * \param  drv    pointer to active s25fs_t structure.
 */
void s25fs_deinit (s25fs_t *drv)
{
   // Clean port I/O
   if ( drv->io.cs)  drv->io.cs (S25FS_DIS);
   if (drv->io.wp)   drv->io.wp (S25FS_EN);

   memset ((void*)drv, 0, sizeof (s25fs_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialize s25fs flash pointed by \a drv
 *
 * \param  drv    pointer to active s25fs_t structure.
 *
 * \return The status of the init operation.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en s25fs_init (s25fs_t *drv)
{
   #define _bad_link(_link)   (!drv->io._link) ? 1:0

   // Connectivity check
   if (_bad_link(cs))         return drv->status = DRV_ERROR;
   if (_bad_link(spi))        return drv->status = DRV_ERROR;
   //if (_bad_link(spi_ioctl))  return drv->status = DRV_ERROR;
   if (_bad_link(spi_read))   return drv->status = DRV_ERROR;
   if (_bad_link(spi_write))  return drv->status = DRV_ERROR;

   if (drv->status == DRV_BUSY || drv->status == DRV_NODEV)
      return drv->status = DRV_ERROR;

   drv->status = DRV_BUSY;

   // Bus SPI set mode 0
   spi_set_cpha ((spi_bb_t*)drv->io.spi, 0);
   spi_set_cpol ((spi_bb_t*)drv->io.spi, 0);

   // port init
   drv->io.cs (S25FS_DIS);
   if (drv->io.wp)   drv->io.wp (S25FS_EN);

   if (!drv->conf.write_page_sz)    drv->conf.write_page_sz = S25FS_WRITE_PAGE_SZ_DEF;
   if (!drv->conf.erase_page_sz)    drv->conf.erase_page_sz = S25FS_ERASE_PAGE_SZ_DEF;
   if (!drv->conf.sector_sz)        drv->conf.sector_sz = S25FS_SECTOR_SIZE_DEF;

   return drv->status = DRV_READY;
   #undef _bad_link
}

/*!
 * \brief
 *    Erase a sector at address \a idx
 * \note
 *    If the requested address is inside sector range it
 *    still erase the sector from the sector starting address.
 *
 * \param  drv    pointer to active s25fs_t structure.
 * \param  idx    Sector address
 *
 * \return The status of the erase operation.
 *    \arg DRV_READY
 *    \arg DRV_BUSY
 *    \arg DRV_ERROR
 */
drv_status_en  s25fs_erase (s25fs_t *drv, s25fs_idx_t idx)
{
   // Wait last operation
   if ( !_wait_ready (drv) )                return DRV_BUSY;
   // Write enable
   if (drv->io.wp)   drv->io.wp (S25FS_DIS);
   if ( _cmd_WREN (drv) != DRV_READY )      return DRV_ERROR;
   // Execute Sector erase command
   if ( _cmd_SE (drv, idx) != DRV_READY )   return DRV_ERROR;
   // Wait to finish
   if ( !_wait_ready (drv) )                return DRV_BUSY;
   // Write disable again
   if ( _cmd_WRDI (drv) != DRV_READY )      return DRV_ERROR;
   if (drv->io.wp)   drv->io.wp (S25FS_EN);

   return DRV_READY;
}

/*!
 * \brief
 *    Read data from flash at address \a idx
 *
 * \param   drv   Pointer to active s25fs_t structure.
 * \param   idx   Sector address
 * \param   buf   Buffer pointer to store the data from flash
 * \param count   Number of bytes to read
 *
 * \return The status of the erase operation.
 *    \arg DRV_READY
 *    \arg DRV_BUSY
 *    \arg DRV_ERROR
 */
drv_status_en  s25fs_read (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, int count)
{
   if ( !_wait_ready (drv) )
      return DRV_BUSY;
   return _read (drv, S25FS_READ_4B_CMD, idx, 4, buf, count);
}

/*!
 * \brief
 *    Write data to flash at address \a idx
 *
 * \param   drv   Pointer to active s25fs_t structure.
 * \param   idx   Sector address
 * \param   buf   Buffer pointer with the data to write
 * \param count   Number of bytes to write
 *
 * \return The status of the erase operation.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en s25fs_write (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, int count)
{
   uint32_t wb=0;    // The written bytes
   int      ret;

   if (drv->io.wp)   drv->io.wp (S25FS_DIS);
   if ( _cmd_WREN (drv) != DRV_READY )
      return DRV_ERROR;

   do {
      ret = _writepage (drv, idx+wb, &buf[wb], count-wb);
      if (ret == -1)    return DRV_ERROR;
      else              wb += ret;
      /*!
       * \note
       * Each _writepage writes only until the page limit, so we
       * call _writepage until we have no more data to send.
       */
   } while (wb < (uint32_t)count);

   if ( !_wait_ready (drv) )
      return DRV_ERROR;
   if ( _cmd_WRDI (drv) != DRV_READY )
      return DRV_ERROR;
   if (drv->io.wp)   drv->io.wp (S25FS_EN);

   return DRV_READY;
}

/*!
 * \brief
 *    Read data from flash using sector addressing
 *
 * \param   drv   Pointer to active s25fs_t structure.
 * \param   idx   Sector number
 * \param   buf   Buffer pointer to store the data from flash
 * \param count   Number of sectors to read
 *
 * \return The status of the erase operation.
 *    \arg DRV_READY
 *    \arg DRV_BUSY
 *    \arg DRV_ERROR
 */
drv_status_en  s25fs_read_sector (s25fs_t *drv, int sector, s25fs_data_t *buf, int count)
{
   // Virtual sector conversions
   s25fs_idx_t idx = sector * drv->conf.sector_sz;
   count *= drv->conf.sector_sz;

   // Forward call to buffer version
   return s25fs_read (drv, idx, buf, count);
}

/*!
 * \brief
 *    Write data to flash using sector addressing
 *
 * \param   drv   Pointer to active s25fs_t structure.
 * \param   idx   Sector number
 * \param   buf   Buffer pointer with the data to write
 * \param count   Number of sectors to write
 *
 * \return The status of the erase operation.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en s25fs_write_sector (s25fs_t *drv, int sector, s25fs_data_t *buf, int count)
{
   // Virtual sector conversions
   s25fs_idx_t idx = sector * drv->conf.sector_sz;
   count *= drv->conf.sector_sz;

   // Forward call to buffer version
   return s25fs_write (drv, idx, buf, count);
}


/*!
 * \brief
 *    S25FS ioctl function
 *
 * \param  drv    pointer to active s25fs_t structure.
 *
 * \param  ctrl   specifies the command to s25fs and get back the reply.
 *    \arg CTRL_GET_STATUS    Get driver's status, not device status
 *    \arg CTRL_DEINIT        Initialise the flash
 *    \arg CTRL_INIT          De-Initialise the flash
 *    \arg CTRL_CMD_UNLOCK    Request Write enable command to flash
 *    \arg CTRL_CMD_LOCK      Request Write disable command to flash
 *    \arg CTRL_ERASE_PAGE    Request Sector erase command to flash
 *    \arg S25FS_CTRL_RDSR1   Request flash status register SR1
 *    \arg S25FS_CTRL_WREN    CTRL_CMD_UNLOCK
 *    \arg S25FS_CTRL_WRDI    CTRL_CMD_LOCK
 *    \arg S25FS_CTRL_SE      CTRL_ERASE_PAGE
 *
 * \param  buf    pointer to buffer for ioctl
 *
 * \return The status of the operation. Not the driver's or flash device's
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en s25fs_ioctl (s25fs_t *drv, ioctl_cmd_t ctrl, ioctl_buf_t buf)
{
   switch (ctrl)
   {
      case CTRL_GET_STATUS:      /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = drv->status;
         return DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
         s25fs_deinit (drv);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = s25fs_init (drv);
         else
            s25fs_init (drv);
         return DRV_READY;
      case S25FS_CTRL_RDSR1:     /*!< Get SR1 */
         if (buf)
            return _cmd_RDSR1 (drv, (byte_t *)buf);
         else
            return DRV_ERROR;
      case CTRL_CMD_UNLOCK:      /*!< Write Enable */
      case S25FS_CTRL_WREN:
         return _cmd_WREN (drv);
      case CTRL_CMD_LOCK:        /*!< Write Disable */
      case S25FS_CTRL_WRDI:
         return _cmd_WRDI (drv);
      case CTRL_ERASE_PAGE:        /*!< Sector erase */
      case S25FS_CTRL_SE:
         if (buf)
            return s25fs_erase (drv, *(s25fs_idx_t *)buf);
         else
            return DRV_ERROR;
      default:                   /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}
