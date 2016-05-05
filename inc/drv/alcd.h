/*!
 * \file alcd.c
 * \brief
 *    A target independent Alpharithmetic LCD driver
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
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       05/2013
 * Version:    0.1
 *
 */

#ifndef __alcd_h__
#define __alcd_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/jiffies.h>
#include <tbx_ioctl.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


/*
 * General Defines
 */
#define ALCD_CLOCK       (CLOCK)

/*!
 *  ----------------------------------------------------
 *      Hitachi HD44780 - Samsung KS0066U
 *  ----------------------------------------------------
 *
 *
 *  Entry Mode Set -----> 0  0  0  0      0  1  I/D  S
 *  ----------------------------------------------------
 *  I/D = 1 -->Inciment Curs     I/D = 0 Decriment
 *  S = 1 -->Display shift       S = 0 Not
 *
 *
 *  DispOnOffControll -->  0  0  0  0     1  D  C  B
 *  -------------------------------------------------
 *  D = Display On
 *  C = Cursor On
 *  B = Blinking On
 *
 *
 *  Cursor/Display Shift --> 0  0  0  1   S/C R/L  x  x
 *  ---------------------------------------------------
 *  S/C = 1 -->Display Shift  S/C = 0 -->Cursor Shift
 *  R/L = 1 -->Shift Right     R/L = 0 -->Shift left
 *
 *
 *  FunctionSet     ------>  0  0  1 DL   N   F   x  x
 *  ---------------------------------------------------
 *  DL = 1 -->8bit         DL = 0 -->4bit
 *  N = 1 -->2 lines       N = 0 -->1 line
 *  F = 1 -->5x10 dots     F = 0 -->5x8 dots
 *
 */
//#define LCD_LINES                   (4)
//#define LCD_ROWS                    (16)

#define LCD_CLRSCR                  (0x01)      /*!< Clear Srean Command Number */
#define LCD_RETHOME                 (0x02)      /*!< Cursor home Un-Shift display DDRam as is */

#define LCD_ENTRYMODE               (0x06)      /*!< Inc Cursor, Don't shift display */
#define LCD_DISP_ON                 (0x0C)      /*!< No Cursos and Blink */
#define LCD_DISP_OFF                (0x08)
#define LCD_CUR_DISP                (0x14)      /*!< Cursor shift right */
#define LCD_FUNSET                  (0x28)      /*!< 4bit, 2lines, 5x8 dots */

#define LCD_DDRAMMask               (0x80)      /*!< DDRAM ------------> 1   ADD[7..0] */
#define LCD_BFMask                  (0x80)      /*!< IR    ------------> BF   AC[6..0] */
#define LCD_ACMask                  (0x7f)      /*!<    ____________________________| */

#define LCD_SHIFT_RIGHT             (0x1C)
#define LCD_SHIFT_LEFT              (0x18)

/*!
 * Alpharithmetic LCD Cursor
 */
typedef volatile struct
{
   uint8_t     x;
   uint8_t     y;
}alcd_cursor_t;

typedef void (*alcd_pin_t) (uint8_t);

/*!
 * Alpharithmetic LCD Pin assignements.
 * Each one can be called xx.DB4(1); or xx.DB4(0); in order to set
 * or clear the corresponding pin.
 *
 * \note These pointers MUST to be assigned from main application.
 */
typedef volatile struct
{
   //alcd_pin_t  db0;
   //alcd_pin_t  db1;
   //alcd_pin_t  db2;
   //alcd_pin_t  db3;
   alcd_pin_t  db4;     /*!< Pointer for DB4 pin */
   alcd_pin_t  db5;     /*!< Pointer for DB5 pin */
   alcd_pin_t  db6;     /*!< Pointer for DB6 pin */
   alcd_pin_t  db7;     /*!< Pointer for DB7 pin */
   alcd_pin_t  rs;      /*!< Pointer for RS pin */
   alcd_pin_t  en;      /*!< Pointer for EN pin */
   alcd_pin_t  bl;      /*!< Pointer for Back Light pin*/
}alcd_io_t;

/*!
 * Alpharithmetic LCD Public Data struct
 */
typedef volatile struct
{
   alcd_io_t      io;      //!< Link to IO struct
   alcd_cursor_t  c;       //!< Link to Cursor struct
   uint8_t        lines;   //!< The lines of attached lcd
   uint8_t        columns; //!< The columns of attached lcd
   //uint8_t        bus;     //!< Bus length, 4 or 8 bit
   drv_status_en  status;  //!< alcd driver status
}alcd_t;


/*
 *  ============= PUBLIC ALCD API =============
 */

/*
 * Link and Glue functions
 */
void alcd_link_db4 (alcd_t *alcd, alcd_pin_t pfun);
void alcd_link_db5 (alcd_t *alcd, alcd_pin_t pfun);
void alcd_link_db6 (alcd_t *alcd, alcd_pin_t pfun);
void alcd_link_db7 (alcd_t *alcd, alcd_pin_t pfun);
void alcd_link_rs (alcd_t *alcd, alcd_pin_t pfun);
void alcd_link_en (alcd_t *alcd, alcd_pin_t pfun);
void alcd_link_bl (alcd_t *alcd, alcd_pin_t pfun);

int alcd_putchar (alcd_t *alcd, int ch) __Os__;

/*
 * Set functions
 */
void alcd_set_lines (alcd_t *alcd, int lines);
void alcd_set_columns (alcd_t *alcd, int columns);

/*
 * User Functions
 */
void alcd_deinit (alcd_t *alcd) __Os__ ;                /*!< For compatibility */
drv_status_en alcd_init (alcd_t *alcd) __Os__ ;         /*!< For compatibility */

void alcd_backlight (alcd_t *alcd, uint8_t on) __Os__ ; /*!< For compatibility */
void alcd_enable (alcd_t *alcd, uint8_t on) __Os__ ;    /*!< For compatibility */
void alcd_cls (alcd_t *alcd) __Os__ ;                   /*!< For compatibility */
void alcd_shift (alcd_t *alcd, int pos) __Os__ ;        /*!< For compatibility */

drv_status_en  alcd_ioctl (alcd_t *alcd, ioctl_cmd_t cmd, ioctl_data_t buf) __Os__ ;

#ifdef __cplusplus
}
#endif

#endif //#ifndef __alcd_h__
