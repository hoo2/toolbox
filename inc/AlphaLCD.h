/**
  * AlphaLCD.h
  *
  * Copyright (C) 2013 Houtouridis Christos (http://houtouridis.blogspot.com/)
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
  * Date:       05/2013
  * Version:    0.1
  *
  */

#ifndef __AlphaLCD_h__
#define __AlphaLCD_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <stm32f10x_clock.h>
#include <jiffies.h>

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
#define LCD_LINES                   (2)
#define LCD_ROWS                    (16)

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

typedef void (*ALCD_Pin_t) (int8_t);
typedef int (*ALCD_Remap_ft) (int);

/*!
 * Alpharithmetic LCD Cursor
 */
typedef volatile struct
{
   uint8_t     X;
   uint8_t     Y;
}ALCD_Cursor_t;

/*!
 * Alpharithmetic LCD Pin assignements.
 * Each one can be called xx.DB4(1); or xx.DB4(0); in order to set
 * or clear the corresponding pin.
 *
 * \note These pointers MUST to be assigned from main application.
 */
typedef volatile struct
{
   //ALCD_Pin_t  DB0;
   //ALCD_Pin_t  DB1;
   //ALCD_Pin_t  DB2;
   //ALCD_Pin_t  DB3;
   ALCD_Pin_t  DB4;     /*!< Pointer for DB4 pin */
   ALCD_Pin_t  DB5;     /*!< Pointer for DB5 pin */
   ALCD_Pin_t  DB6;     /*!< Pointer for DB6 pin */
   ALCD_Pin_t  DB7;     /*!< Pointer for DB7 pin */
   ALCD_Pin_t  RS;      /*!< Pointer for RS pin */
   ALCD_Pin_t  EN;      /*!< Pointer for EN pin */
   ALCD_Pin_t  BL;      /*!< Pointer for Back Light pin*/
}ALCD_IO_t;

/*!
 * Alpharithmetic LCD Public Data struct
 */
typedef volatile struct
{
   ALCD_IO_t      IO;         /*!< Link to IO struct */
   ALCD_Cursor_t  C;          /*!< Link to Cursor struct */
   //ALCD_Remap_ft  remap;
   uint8_t  Bus;     /*!< Bus length, 4 or 8 bit */
   uint8_t  BL :1;   /*!< BackLight flag */
   uint8_t  EN :1;   /*!< Display Enable flag */
   //clock_t  tim;     /*!< Timer for delay, used by ALCD service */
}ALCD_t;


extern ALCD_t ALCD;


/*
 * ALCD Control Functions.
 */
void ALCD_BackLight (int8_t on);
void ALCD_Enable (uint8_t on);
void ALCD_Clrscr (void);
void ALCD_Shift (int pos);
int ALCD_Connect (volatile ALCD_Pin_t *sio, ALCD_Pin_t pfun);
void ALCD_setremap (ALCD_Remap_ft fptr);
void ALCD_DeInit (void);
void ALCD_Init (void);

/*
 * ALCD IO Function.
 * Tailor with this the stdio's print function.
 */
int ALCD_putchar(int ch);


#ifdef __cplusplus
 }
#endif

#endif //#ifndef __AlphaLCD_h__
