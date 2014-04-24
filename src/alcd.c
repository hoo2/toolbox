/*
 * AlphaLCD.c
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Houtouridis Christos. The intellectual
 * and technical concepts contained herein are proprietary to
 * Houtouridis Christos and are protected by copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Houtouridis Christos.
 *
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       05/2013
 * Version:    0.1
 *
 */

#include <drv/alcd.h>

ALCD_t   ALCD;
static   uint8_t _rs=0;

static void ALCD_IncX (void)
{
   if (++ALCD.C.X > LCD_ROWS)
      ALCD.C.X = 1;
}

static void ALCD_IncY (void)
{
   if (++ALCD.C.Y > LCD_LINES)
      ALCD.C.Y = 1;
}

static void ALCD_DecX (void)
{
   if (--ALCD.C.X < 1)
      ALCD.C.X = LCD_ROWS;
}

static void ALCD_SetDB (int8_t  db)
{
   ALCD.IO.DB4 (db & 0x01);   //Update port
   ALCD.IO.DB5 (db & 0x02);
   ALCD.IO.DB6 (db & 0x04);
   ALCD.IO.DB7 (db & 0x08);
   jf_delay_us (10);     // Wait to settle

   ALCD.IO.EN (1);      // Pulse out the data
   jf_delay_us (10);
   ALCD.IO.EN (0);
   jf_delay_us (10);     // Data hold
}

static void ALCD_WriteData (int8_t data)
{
   ALCD_SetDB (data >> 4);
   ALCD_SetDB (data & 0x0F);
}

static void ALCD_Command (uint8_t c)
{
   ALCD.IO.RS(0);
   if (_rs != 0)
      { jf_delay_us (300); _rs = 0; }
   else
      jf_delay_us (100);
   ALCD_WriteData (c);
}

static void ALCD_Character (uint8_t c)
{
   ALCD.IO.RS(1);
   if (_rs != 1)
      { jf_delay_us (300); _rs = 1; }
   else
      jf_delay_us (100);
   ALCD_WriteData (c);
   ALCD_IncX ();
}

//Set the Cursor to LCD's position line (y), column (x) starts from 1,2,...n
static void ALCD_SetCursor (char x, char y)
{
   uint8_t cmd;

   ALCD.C.X = x; //Save position
   ALCD.C.Y = y;

   cmd = 0x80 | (x - 1);   //Calculate command
   cmd |= (0x40 * (y - 1));
   ALCD_Command (cmd); // Command out the LCD
}




void ALCD_BackLight (int8_t on)
{
   (on) ? ALCD.IO.BL (1) : ALCD.IO.BL (0);
}

void ALCD_Enable (uint8_t on)
{
   if (on)
      { ALCD_Command (LCD_DISP_ON); ALCD.IO.BL (1); }
   else
      { ALCD_Command (LCD_DISP_OFF); ALCD.IO.BL (0); }
}

void ALCD_Clrscr (void) {
   ALCD_Command(LCD_CLRSCR);
      jf_delay_us(2000);
   ALCD_Command (LCD_RETHOME);
      jf_delay_us(2000);
}

void ALCD_Shift (int pos)
{
   uint8_t i;
   uint8_t cmd = LCD_SHIFT_LEFT;

   if (pos<0)
   {
      pos = -pos;
      cmd = LCD_SHIFT_RIGHT;
   }
   for (i=0 ; i<pos ; ++i)
   {
      ALCD_Command(cmd);
         jf_delay_us(100);
   }
}

int ALCD_Connect (volatile ALCD_Pin_t *sio, ALCD_Pin_t pfun)
{
   if (!pfun)
      return 0;
   *sio = pfun;
   return 1;
}

void ALCD_DeInit (void)
{
   ALCD_Enable (0);
   ALCD_BackLight (0);
}

void ALCD_Init (void)
{
   ALCD.C.X = ALCD.C.Y = 1;
   _rs = 0;

   ALCD.IO.EN (0);
   ALCD.IO.RS (0);
   jf_delay_us(100000);

   //Pre-Init phace 8bit at this point
   ALCD_SetDB (0x3);
      jf_delay_us(50000);
   ALCD_SetDB (0x3);
      jf_delay_us(5000);
   ALCD_SetDB (0x3);
      jf_delay_us(5000);

   ALCD_SetDB (0x2);              //4bit selection
      jf_delay_us(10000);

   ALCD_Command (LCD_FUNSET);     //4bit selection and Function Set
      jf_delay_us(5000);
   ALCD_Command(LCD_DISP_OFF);    //Display Off Control 4bit for now on
      jf_delay_us(5000);
   ALCD_Command(LCD_CLRSCR);      //Clear Display
      jf_delay_us(5000);
   ALCD_Command(LCD_ENTRYMODE);   //Entry Mode Set

   ALCD_Command(LCD_CLRSCR);
      jf_delay_us(5000);
   ALCD_Command (LCD_RETHOME);
      jf_delay_us(10000);
   ALCD_Command (LCD_DISP_ON);
      jf_delay_us(5000);
      ALCD_BackLight (1);
}

int ALCD_putchar(int ch)
{
   // LCD Character dispatcher
   switch (ch)
   {
      case '\r':
         ALCD.C.X = 1;
         ALCD_SetCursor (ALCD.C.X, ALCD.C.Y);
         break;
      case '\n':
         ALCD_IncY ();
         ALCD.C.X = 1;
         ALCD_SetCursor (ALCD.C.X, ALCD.C.Y);
         break;
      case '\v':
         ALCD.C.X = ALCD.C.Y = 1;
         ALCD_Command (LCD_RETHOME);
         jf_delay_us(2000);
         break;
      case '\f':
         ALCD.C.X = ALCD.C.Y = 1;
         //ALCD_Command (LCD_CLRSCR);
         //jf_delay_us(5000);
         ALCD_Command (LCD_RETHOME);
         //ALCD_SetCursor (ALCD.C.X, ALCD.C.Y);
         jf_delay_us(2000);
         break;
      case '\b':
         ALCD_DecX ();
         ALCD_Character (' ');
         ALCD_DecX ();
         break;
      case 0:
         break;   // discard null termination to device
      default:
         ALCD_Character (ch);
         break;
   }

   //ANSI C (C99) compatible mode
   return ch;
}
