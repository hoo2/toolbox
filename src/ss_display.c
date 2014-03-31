/*
 * \file ss_display.c
 * \brief
 *    This is a Seven Segment display module
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
 * Date:       11/2013
 * Version:    0.1
 *
 */

#include <ss_display.h>
SSD_t      SSD;


/* ================ Data ================ */
/*
 * Led assignments
 * --------------------
 * A --> 0x01     E --> 0x10
 * B --> 0x02     F --> 0x20
 * C --> 0x04     G --> 0x40
 * D --> 0x08     DP--> 0x80
 */
static uint8_t const digit_lut[10] = {
   0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67 };     //0 -- 9
static uint8_t const ALPHA_lut[26] = {
   0x77, 0x00, 0x39, 0x00, 0x79, 0x71, 0x7D,    //A -- G
   0x76, 0x30, 0x0E, 0x00, 0x38, 0x00, 0x00,    //H -- N
   0x3F, 0x73, 0x00, 0x00, 0x6D, 0x00, 0x3E,    //O -- U
   0x00, 0x00, 0x00, 0x6E, 0x00 };              //V -- Z
static uint8_t const alpha_lut[26] = {
   0x00, 0x7C, 0x58, 0x5E, 0x7B, 0x00, 0x00,    //a -- g
   0x74, 0x10, 0x00, 0x00, 0x30, 0x00, 0x54,    //h -- n
   0x5C, 0x00, 0x00, 0x50, 0x00, 0x78, 0x1C,    //o -- u
   0x00, 0x00, 0x00, 0x00, 0x00 };              //v -- z
static uint8_t const symbols_lut[3] = {
   0x00,    // ' '
   0x08,    // '_'
   0x40     // '-'
};
static char const dot_mask = 0x80;

/* ================ Static ================ */
static void _ret_home (void);
static void _clear (void);
static void _del (void);
static uint8_t _lut (uint8_t ch);
static void _port_update (uint8_t ch);
static void _put_character (char ch);


static void _ret_home (void) {
   SSD.cur = 0;
}

static void _clear (void) {
   int8_t i;
   for (i=0 ; i<SEVEN_SEG_FB_SIZE ; ++i)
      SSD.fb[i] = 0x00;
}
static void _del (void) {
   if (SSD.cur)   --SSD.cur;

   SSD.fb[SSD.cur] = 0x00;
}

static uint8_t _lut (uint8_t ch)
{
   if ( ch>='0' && ch<= '9' )
      return digit_lut[ ch - '0' ];
   else if (ch >= 'A' && ch <= 'Z' )
      return ALPHA_lut[ ch - 'A' ];
   else if (ch >= 'a' && ch <= 'z' )
      return alpha_lut[ ch - 'a' ];
   else
      switch (ch)
      {
         case ' ':   return symbols_lut[0];
         case '_':   return symbols_lut[1];
         case '-':   return symbols_lut[2];
         default:    return 0x00;
      }
}

static void _port_update (uint8_t ds)
{
   if (SSD.IO.SSA)   SSD.IO.SSA (ds & 0x01);
   if (SSD.IO.SSB)   SSD.IO.SSB (ds & 0x02);
   if (SSD.IO.SSC)   SSD.IO.SSC (ds & 0x04);
   if (SSD.IO.SSD)   SSD.IO.SSD (ds & 0x08);
   if (SSD.IO.SSE)   SSD.IO.SSE (ds & 0x10);
   if (SSD.IO.SSF)   SSD.IO.SSF (ds & 0x20);
   if (SSD.IO.SSG)   SSD.IO.SSG (ds & 0x40);
   if (SSD.IO.SSDP)  SSD.IO.SSDP (ds & 0x80);
}

static inline void _put_character (char ch)
{
   if (ch == '.')
   {
      if (!SSD.cur)
         SSD.fb[SSD.cur] = dot_mask;
      else
         SSD.fb[SSD.cur-1] |= dot_mask;
   }
   else
   {
      SSD.fb[SSD.cur] = _lut(ch);

      if (++SSD.cur > SEVEN_SEG_FB_SIZE)
         SSD.cur = 0;
   }
}








/* ================ API ================ */
int SSD_Connect (volatile SSD_Pin_t *sio, SSD_Pin_t pfun)
{
   if (!pfun)
      return 0;
   *sio = pfun;
   return 1;
}

inline void SSD_BlinkEnable (void) { SSD.blink = 1; }
inline void SSD_BlinkDisable (void) { SSD.blink = 0; }


int SSD_putchar(int ch)
{
   // LCD Character dispatcher
   switch (ch)
   {
      case '\r':
         _ret_home ();
         break;
      case '\n':
      case '\v':
      case '\f':
         _clear ();
         _ret_home ();
         break;
      case '\b':
         _del ();
         break;
      default:
         _put_character (ch);
         break;
   }

   //ANSI C (C99) compatible mode
   return ch;
}

void SSD_Service(void)
{
   static clock_t  bl_mark;   //Blink Mark
   static uint8_t on_off = 1;
   uint8_t i;

   // Clear displays
   for (i=0 ; i<SEVEN_SEG_DIGITS ; ++i)
      SSD.IO.SSx[i] (0);

   // Mark-unmark blink machine
   switch ( SSD.blink )
   {
      case 0:    //If not blink leave display on
      default:
         on_off = 1;
         bl_mark = clock ();
         break;
      case 1:
         //Toggle
         if ( (on_off) && (clock () - bl_mark > SSD.blink_time))
         {
            bl_mark = clock ();
            on_off = 0;
         }
         if ( (!on_off) && (clock () - bl_mark > SSD.blink_time))
         {
            bl_mark = clock ();
            on_off = 1;
         }
   }

   // Display Switcher
   if (on_off)
   {
      _port_update (SSD.fb[SSD.disp]);
      SSD.IO.SSx[SSD.disp] (1);
   }

   if (++SSD.disp >= SEVEN_SEG_DIGITS)
      SSD.disp = 0;
}


