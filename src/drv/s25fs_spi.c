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
static drv_status_en  _cmd_WREN (s25fs_t *drv);
static drv_status_en  _cmd_WRDI (s25fs_t *drv);
static drv_status_en    _cmd_SE (s25fs_t *drv, s25fs_idx_t idx);

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
   drv->io.cs (ENABLE);
   // Command
   if ( drv->io.spi_write (drv->io.spi, &cmd, 1) != DRV_READY )
      return DRV_ERROR;
   // Address
   if (al && drv->io.spi_write (drv->io.spi, (byte_t*)&_idx, al) != DRV_READY )
      return DRV_ERROR;
   // Data
   if (len && drv->io.spi_read (drv->io.spi, buf, len) != DRV_READY )
      return DRV_ERROR;
   drv->io.cs (DISABLE);

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
   drv->io.cs (ENABLE);
   // Command
   if ( drv->io.spi_write (drv->io.spi, &cmd, 1) != DRV_READY )
      return DRV_ERROR;
   // Address
   if (al && drv->io.spi_write (drv->io.spi, (byte_t*)&_idx, al) != DRV_READY )
      return DRV_ERROR;
   // Data
   if (len && drv->io.spi_write (drv->io.spi, buf, len) != DRV_READY )
      return DRV_ERROR;
   drv->io.cs (DISABLE);

   return DRV_READY;
}


static drv_status_en _cmd_RDSR1 (s25fs_t *drv, byte_t *sr)
{
   s25fs_data_t cmd = S25FS_SPI_RDSR_CMD;

   if ( _read (drv, cmd, ADDRESS_NOT_USED, 0, sr, 1) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

static drv_status_en _cmd_WREN (s25fs_t *drv)
{
   s25fs_data_t cmd = S25FS_SPI_WREN_CMD;

   if ( _write (drv, cmd, ADDRESS_NOT_USED, 0, BUFFER_NOT_USED, 0) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

static drv_status_en _cmd_WRDI (s25fs_t *drv)
{
   s25fs_data_t cmd = S25FS_SPI_WRDI_CMD;

   if ( _write (drv, cmd, ADDRESS_NOT_USED, 0, BUFFER_NOT_USED, 0) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

static drv_status_en _cmd_SE (s25fs_t *drv, s25fs_idx_t idx)
{
   s25fs_data_t cmd = S25FS_SPI_SE_4B_CMD;

   if ( _write (drv, cmd, idx, 4, BUFFER_NOT_USED, 0) != DRV_READY )
      return DRV_ERROR;
   return DRV_READY;
}

/*
* \brief
*    Wait for SD card ready.
* \note
*    This function has a timeout of \a SD_WAIT_TIMEOUT
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

   if ( _write (drv, S25FS_SPI_PP_4B_CMD, idx, 4, buf, nl) != DRV_READY )
      return -1;

   return nl;
}
/*
 *  ============= PUBLIC S25FS API =============
 */

/*
 * Link and Glue functions
 */
void s25fs_link_wp (s25fs_t *drv, drv_pinout_ft fun) {
   drv->io.wp = fun;
}
void s25fs_link_cs (s25fs_t *drv, drv_pinout_ft fun) {
   drv->io.cs = fun;
}
void s25fs_link_spi_ioctl (s25fs_t *drv, s25fs_spi_ioctl_t fun) {
   drv->io.spi_ioctl = fun;
}
void s25fs_link_spi_read (s25fs_t *drv, s25fs_spi_rw_t fun) {
   drv->io.spi_read = fun;
}
void s25fs_link_spi_write (s25fs_t *drv, s25fs_spi_rw_t fun) {
   drv->io.spi_write = fun;
}
void s25fs_link_spi (s25fs_t *drv, void* spi) {
   drv->io.spi = spi;
}

/*
 * Set functions
 */
void s25fs_set_write_page_sz (s25fs_t *drv, uint32_t size) {
   drv->conf.write_page_sz = size;
}
void s25fs_set_erase_page_sz (s25fs_t *drv, uint32_t size) {
   drv->conf.erase_page_sz = size;
}
void s25fs_set_sector_size (s25fs_t *drv, uint32_t size) {
   drv->conf.sector_sz = size;
}

/*
 * User Functions
 */

void s25fs_deinit (s25fs_t *drv)
{
   // Clean port I/O
   if ( drv->io.cs)  drv->io.cs (DISABLE);
   if (drv->io.wp)   drv->io.wp (ENABLE);

   memset ((void*)drv, 0, sizeof (s25fs_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

drv_status_en s25fs_init (s25fs_t *drv)
{
   #define _bad_link(_link)   (!drv->io._link) ? 1:0

   // Connectivity check
   if (_bad_link(cs))         return drv->status = DRV_ERROR;
   if (_bad_link(spi))        return drv->status = DRV_ERROR;
   if (_bad_link(spi_ioctl))  return drv->status = DRV_ERROR;
   if (_bad_link(spi_read))   return drv->status = DRV_ERROR;
   if (_bad_link(spi_write))  return drv->status = DRV_ERROR;

   if (drv->status == DRV_BUSY || drv->status == DRV_NODEV)
      return drv->status = DRV_ERROR;

   drv->status = DRV_BUSY;

   // Bus SPI set mode 0
   spi_set_cpha ((spi_bb_t*)drv->io.spi, 0);
   spi_set_cpol ((spi_bb_t*)drv->io.spi, 0);

   // port init
   drv->io.cs (DISABLE);
   if (drv->io.wp)   drv->io.wp (ENABLE);

   if (!drv->conf.write_page_sz)    drv->conf.write_page_sz = S25FS_WRITE_PAGE_SZ_DEF;
   if (!drv->conf.erase_page_sz)    drv->conf.erase_page_sz = S25FS_ERASE_PAGE_SZ_DEF;
   if (!drv->conf.sector_sz)        drv->conf.sector_sz = S25FS_SECTOR_SIZE_DEF;

   return drv->status = DRV_READY;
   #undef _bad_link
}

drv_status_en  s25fs_erase (s25fs_t *drv, s25fs_idx_t idx)
{
   // Wait last operation
   if ( !_wait_ready (drv) )                return DRV_BUSY;
   // Write enable
   if (drv->io.wp)   drv->io.wp (DISABLE);
   if ( _cmd_WREN (drv) != DRV_READY )      return DRV_ERROR;
   // Execute Sector erase command
   if ( _cmd_SE (drv, idx) != DRV_READY )   return DRV_ERROR;
   // Wait to finish
   if ( !_wait_ready (drv) )                return DRV_BUSY;
   // Write disable again
   if ( _cmd_WRDI (drv) != DRV_READY )      return DRV_ERROR;
   if (drv->io.wp)   drv->io.wp (ENABLE);

   return DRV_READY;
}

drv_status_en  s25fs_read (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, int count)
{
   if ( !_wait_ready (drv) )
      return DRV_BUSY;
   return _read (drv, S25FS_SPI_READ_4B_CMD, idx, 4, buf, count);
}

drv_status_en s25fs_write (s25fs_t *drv, s25fs_idx_t idx, s25fs_data_t *buf, int count)
{
   uint32_t wb=0;    // The written bytes
   int      ret;

   if (drv->io.wp)   drv->io.wp (DISABLE);
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
   } while (wb < count);

   if ( !_wait_ready (drv) )
      return DRV_ERROR;
   if ( _cmd_WRDI (drv) != DRV_READY )
      return DRV_ERROR;
   if (drv->io.wp)   drv->io.wp (ENABLE);

   return DRV_READY;
}

drv_status_en  s25fs_read_sector (s25fs_t *drv, int sector, s25fs_data_t *buf, int count)
{
   // Virtual sector conversions
   s25fs_idx_t idx = sector * drv->conf.sector_sz;
   count *= drv->conf.sector_sz;

   // Forward call to buffer version
   return s25fs_read (drv, idx, buf, count);
}

drv_status_en s25fs_write_sector (s25fs_t *drv, int sector, s25fs_data_t *buf, int count)
{
   // Virtual sector conversions
   s25fs_idx_t idx = sector * drv->conf.sector_sz;
   count *= drv->conf.sector_sz;

   // Forward call to buffer version
   return s25fs_write (drv, idx, buf, count);
}

drv_status_en s25fs_ioctl (s25fs_t *drv, ioctl_cmd_t ctrl, ioctl_buf_t buf);
