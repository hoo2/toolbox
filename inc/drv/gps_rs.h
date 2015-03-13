/*
 * \file gps_rs.h
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
#ifndef __gps_rs_h__
#define __gps_rs_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <tbx_ioctl.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/*
 * ============ User options ===========
 */

/*
 * ========== NMEA Sentences ==========
 *
 * $GPGGA - Global Positioning System Fix Data
 * $GPGLL - Geographic position, latitude / longitude
 * $GPGSA - GPS DOP and active satellites
 */

/*
 * ============ Data types ============
 */
typedef byte_t (*gps_rs_rx_ft) (void *);
typedef int    (*gps_rs_tx_ft) (void *, byte_t);
typedef
 drv_status_en (*gps_rs_ioctl_ft) (void *, ioctl_cmd_t, ioctl_buf_t);

typedef struct {
   void*             usart;
   gps_rs_rx_ft      rx;
   gps_rs_tx_ft      tx;
   gps_rs_ioctl_ft   ioctl;
   drv_pinout_ft     pwr;
}gps_io_t;

typedef struct {
   byte_t         *buf;
   gps_io_t       io;
   drv_status_en  status;
}gps_t;

typedef struct {
   float    latitude;
   float    lognitude;
}gps_location_t;

/*
 * ============ Public GPS API ============
 */

/*
 * Link and Glue functions
 */
void gps_link_buffer (gps_t *gps, byte_t *b);
void gps_link_usart (gps_t *gps, void* usart);
void gps_link_usart_rx (gps_t *gps, gps_rs_rx_ft rx);
void gps_link_usart_tx (gps_t *gps, gps_rs_tx_ft tx);
void gps_link_usart_ioctl (gps_t *gps, gps_rs_ioctl_ft ioctl);
void gps_link_power (gps_t *gps, drv_pinout_ft pwr);
/*
 * Set functions
 */

/*
 * User Functions
 */
void gps_deinit (gps_t *gps);
drv_status_en gps_init (gps_t *gps);
drv_status_en gps_location (gps_t *gps, gps_location_t *l);
drv_status_en gps_time (gps_t *gps, time_t *t);
drv_status_en gps_ioctl (gps_t *gps, ioctl_cmd_t, ioctl_buf_t);





#ifdef __cplusplus
}
#endif

#endif   // #ifndef __gps_rs_h__
