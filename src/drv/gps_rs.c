/*
 * \file gps_rs.c
 * \brief
 *    A target independent gps over serial driver using NMEA 0183
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
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
#include <drv/gps_rs.h>
/*
 * ============ Static API ============
 */
static int _checksum (char* str);

static int _checksum (char* str) {
   int c = 0;

   while (*str)
      c ^= *str++;
   return c;
}

/*
 * ============ Public GPS API ============
 */

/*
 * Link and Glue functions
 */
inline void gps_link_buffer (gps_t *gps, byte_t *b) {
   gps->buf = b;
}
inline void gps_link_usart (gps_t *gps, void* usart) {
   gps->io.usart = usart;
}
inline void gps_link_usart_rx (gps_t *gps, gps_rs_rx_ft rx) {
   gps->io.rx = rx;
}
inline void gps_link_usart_tx (gps_t *gps, gps_rs_tx_ft tx) {
   gps->io.tx = tx;
}
inline void gps_link_usart_ioctl (gps_t *gps, gps_rs_ioctl_ft ioctl) {
   gps->io.ioctl = ioctl;
}
inline void gps_link_power (gps_t *gps, drv_pinout_ft pwr) {
   gps->io.pwr = pwr;
}

/*
 * Set functions
 */

/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initializes gps used by the driver.
 *
 * \param  gps   Pointer indicate the gps data stuct to use
 */
void gps_deinit (gps_t *gps)
{
   if (gps->io.pwr)  gps->io.pwr (0);
   memset ((void*)gps, 0, sizeof (gps_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initializes gps.
 *
 * \param  gps    Pointer indicate the gps data stuct to use
 * \return        The status of the operation
 */
drv_status_en gps_init (gps_t *gps)
{
   #define _bad_link(_link)   (!gps->io._link) ? 1:0

   if (_bad_link (rx))     return gps->status = DRV_ERROR;
   if (_bad_link (tx))     return gps->status = DRV_ERROR;
   if (_bad_link (ioctl))  return gps->status = DRV_ERROR;

   if (gps->status == DRV_BUSY || gps->status == DRV_NODEV)
      return gps->status = DRV_ERROR;

   if (gps->io.pwr)  gps->io.pwr (1);

   return gps->status = DRV_READY;
   #undef _bad_link
}

/*!
 * \brief
 *    Read the current location using GPS NMEA output.
 *
 * \param   gps   Pointer indicate the gps data stuct to use
 * \param   l     Pointer to location data to write
 * \return        The status of the operation
 */
drv_status_en gps_location (gps_t *gps, gps_location_t *l)
{
   int i=0;
   //byte_t p=1;

   // Enable Serial
   //gps->io.ioctl ((void*)gps->io.usart, (ioctl_cmd_t)CTRL_POWER, (ioctl_buf_t)&p);
   // Get characters from input
   do {
      gps->buf[i++] = gps->io.rx ((void*)gps->io.usart);
   }while (gps->buf[i-1] != '\n' && i<120);
   //p=0;
   //gps->io.ioctl ((void*)gps->io.usart, (ioctl_cmd_t)CTRL_POWER, (ioctl_buf_t)&p);
   return   DRV_READY;
}

/*!
 * \brief
 *    Read the current time using GPS NMEA output.
 *
 * \param   gps   Pointer indicate the gps data stuct to use
 * \param   t     Pointer to time data to write
 * \return        The status of the operation
 */
drv_status_en gps_time (gps_t *gps, time_t *t)
{
   int i=0;

   // Get characters from input
   do {
      gps->buf[i++] = gps->io.rx ((void*)gps->io.usart);
   }while (gps->buf[i-1] != '\n');
   return   DRV_READY;
}

drv_status_en gps_send (gps_t *gps, char* msg)
{
   char cmd[20];
   int i;

   sprintf (cmd, "$%s*%2d\r\n", msg, _checksum(msg));

   for (i=0 ; i<20 && cmd[i]; ++i) {
      gps->io.tx (gps->io.usart, cmd[i]);
   }
}

drv_status_en gps_ioctl (gps_t *gps, ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   switch (cmd) {
      case CTRL_DEINIT:
         gps_deinit(gps);
         return DRV_READY;
      case CTRL_INIT:
         if (buf)
            *(drv_status_en*)buf = gps_init (gps);
         else
            gps_init (gps);
         return DRV_READY;
      case CTRL_SEND_CMD:
         gps_send (gps, (char*)buf);
         return DRV_READY;

      default:
         return DRV_ERROR;
   }
}
