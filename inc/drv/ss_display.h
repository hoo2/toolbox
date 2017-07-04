/*
 * \file ss_display.h
 * \brief
 *    This is a Seven Segment display module
 *
 * Copyright (C) 2014 Christos Choutouridis (http://www.houtouridis.net)
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
 */
#ifndef __ss_display_h__
#define __ss_display_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <tbx_types.h>
#include <tbx_ioctl.h>
#include <toolbox_defs.h>

/*
 * =========== User Defines ==============
 */

/*
 * General defines
 */
#define  SSD_MAX_DIGITS       (8)   // We use uint8_t for multiplexing, so 8 is the max number
#define  DEL                  (0x7F)

#define  SSD_BLINK_DEF        (0)
#define  SSD_BLINK_TIME_DEF   (500)
#define  SSD_DIGITS_DEF       (3)
#define  SSD_FB_SIZE_DEF      (SSD_DIGITS_DEF+1)


typedef void (*ssd_pin_ft) (uint8_t);

/*!
 * Seven Segment Display Pin assignments.
 * Each one can be called xx.SSA(1); or xx.SSA(0); in order to set
 * or clear the corresponding pin.
 *
 *        (A)
 *      --------
 *     /       /
 *  (F)/  (G)  / (B)
 *     --------
 *    /       /
 * (E)/       / (C)
 *    -------  * <--- (DP)
 *      (D)
 *
 * \note These pointers MUST to be assigned from main application.
 */
typedef struct {
   ssd_pin_ft  bus;     //!< Pointer for SSD Bus pins
   ssd_pin_ft  dis;     //!< Pointer to SSD display pins
}ssd_io_t;

typedef struct {
   byte_t      *buffer;
   byte_t      size;
   byte_t      cursor;
}ssd_fb_t;

/*!
 * Seven Segment Display Public Data struct
 */
typedef struct
{
   ssd_io_t io;         // IO struct
   ssd_fb_t fb;
   uint8_t  digits;
   uint8_t  disp;       // Shows which digit of the display is ON
   uint8_t  power;      // Display power flag
   uint8_t  blink;      // Blink flag
   clock_t  blink_time; // Blink time
   drv_status_en  status;  //!< alcd driver status
}ssd_t;



/*
 *  ============= PUBLIC Seven segment display API =============
 */

/*
 * Link and Glue functions
 */
void ssd_link_bus (ssd_pin_ft fun);
void ssd_link_display (ssd_pin_ft fun);
void ssd_link_buffer (byte_t *b);

/*
 * Set functions
 */
void ssd_set_fb_size (uint8_t s);
void ssd_set_digits (uint8_t d);
void ssd_set_blink_time (clock_t t);


/*
 * User Functions
 */
void ssd_service (void);
int  ssd_putchar (int ch);

void ssd_power (uint8_t en);
void ssd_blink (uint8_t b);

drv_status_en  ssd_ctl (ioctl_cmd_t cmd, ioctl_data_t data) __Os__ ;

#ifdef __cplusplus
}
#endif

#endif //#ifndef __ss_display_h__
