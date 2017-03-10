/*
 * \file ss_display.c
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
 *
 */

#include <drv/ss_display.h>

extern clock_t get_freq (void);

ssd_t    _ssd;

/* ================ Fonts ================ */
/*
 * Led assignments
 * --------------------
 * A --> 0x01     E --> 0x10
 * B --> 0x02     F --> 0x20
 * C --> 0x04     G --> 0x40
 * D --> 0x08     DP--> 0x80
 */
static uint8_t const _font_digit[10] = {
   0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67 };     //0 -- 9
static uint8_t const _font_ALPHA[26] = {
   0x77, 0x00, 0x39, 0x00, 0x79, 0x71, 0x7D,    /* A - _ - C - _ - E - F - G */
   0x76, 0x30, 0x0E, 0x00, 0x38, 0x00, 0x00,    /* H - I - J - _ - L - _ - _ */
   0x3F, 0x73, 0x00, 0x00, 0x6D, 0x00, 0x3E,    /* O - P - _ - _ - S - _ - U */
   0x00, 0x00, 0x00, 0x6E, 0x00 };              /* _ - _ - _ - Y - Z */
static uint8_t const _font_alpha[26] = {
   0x00, 0x7C, 0x58, 0x5E, 0x7B, 0x00, 0x00,    /* _ - b - c - d - e - _ - _ */
   0x74, 0x10, 0x00, 0x00, 0x30, 0x00, 0x54,    /* h - i - _ - _ - l - _ - n */
   0x5C, 0x00, 0x00, 0x50, 0x00, 0x78, 0x1C,    /* o - _ - _ - r - _ - t - u */
   0x00, 0x00, 0x00, 0x00, 0x00 };              /* _ - _ - _ - _ - _ */
static uint8_t const _font_symbols[3] = {
   0x00,    // ' '
   0x08,    // '_'
   0x40     // '-'
};
static char const _font_dot_mask = 0x80;


/*
 * ================ Static ================
 */
static void _fb_ret_home (void);
static void _fb_clear (void);
static void _fb_del (void);
static void _fb_put_character (char ch);

static uint8_t _font_conv (uint8_t ch);


inline static void _fb_ret_home (void) {
   _ssd.fb.cursor = 0;
}

static void _fb_clear (void) {
   int8_t i;
   for (i=0 ; i<_ssd.fb.size ; ++i)
      _ssd.fb.buffer[i] = 0x00;
}

static void _fb_del (void) {
   if (_ssd.fb.cursor)   --_ssd.fb.cursor;

   _ssd.fb.buffer[_ssd.fb.cursor] = 0x00;
}


static inline void _fb_put_character (char ch)
{
   if (_ssd.fb.cursor>=0 && _ssd.fb.cursor<_ssd.fb.size) {
      if (ch == '.') {
         if (_ssd.fb.cursor == 0)
            _ssd.fb.buffer[_ssd.fb.cursor] = _font_dot_mask;
         else
            _ssd.fb.buffer[_ssd.fb.cursor-1] |= _font_dot_mask;
      }
      else {
         _ssd.fb.buffer[_ssd.fb.cursor++] = _font_conv (ch);
      }
   }
}

static uint8_t _font_conv (uint8_t ch)
{
   if ( ch>='0' && ch<= '9' )
      return _font_digit[ ch - '0' ];
   else if (ch >= 'A' && ch <= 'Z' )
      return _font_ALPHA[ ch - 'A' ];
   else if (ch >= 'a' && ch <= 'z' )
      return _font_alpha[ ch - 'a' ];
   else
      switch (ch) {
         case ' ':   return _font_symbols[0];
         case '_':   return _font_symbols[1];
         case '-':   return _font_symbols[2];
         default:    return 0x00;
      }
}






/*
 *  ============= PUBLIC Seven segment display API =============
 */

/*
 * Link and Glue functions
 */
void ssd_link_bus (ssd_pin_ft fun) {
   _ssd.io.bus = fun;
}
void ssd_link_display (ssd_pin_ft fun) {
   _ssd.io.dis = fun;
}
void ssd_link_buffer (byte_t *b) {
   _ssd.fb.buffer = b;
}

/*
 * Set functions
 */
void ssd_set_fb_size (uint8_t s) {
   _ssd.fb.size = s;
}
void ssd_set_digits (uint8_t d) {
   _ssd.digits = d;
}
void ssd_set_blink_time (clock_t t) {
   _ssd.blink_time = t;
}


/*
 * User Functions
 */

/*!
  * \brief
  *    This function is the bus and display handler for ssd functionality.
  *    MUST be called in a constant interval with Freq >= 20Hz*(Number of Digits).
  * \param  none
  * \return none
  */
void ssd_service (void)
{
   static clock_t  bl_mark;   // blink mark
   static uint8_t on_off = 1;
   clock_t now = clock ();

   // Clear displays
   _ssd.io.bus (0);
   _ssd.io.dis (0);

   // Power flag does not stop service
   if (_ssd.power == 0) {
      _ssd.disp = 1;
      return;
   }

   // Mark-unmark blink machine
   switch ( _ssd.blink ) {
      default:
      case 0:    //If not blink leave display on
         on_off = 1;
         bl_mark = now;
         break;
      case 1:
         //Toggle
         if ( (on_off) && (now - bl_mark > _ssd.blink_time)) {
            bl_mark = now;
            on_off = 0;
         }
         if ( (!on_off) && (now - bl_mark > _ssd.blink_time)) {
            bl_mark = now;
            on_off = 1;
         }
         break;
   }

   // Display Switcher 1-1 for now (no shift implemented)
   if (on_off) {
      _ssd.io.bus (_ssd.fb.buffer[_ssd.disp]);
      _ssd.io.dis (0x01 << _ssd.disp);
   }

   if (++_ssd.disp >= _ssd.digits)
      _ssd.disp = 0;
}

/*!
 * \brief
 *    De-Initialize the ssd driver
 */
void ssd_deinit (void)
{
   memset ((void*)&_ssd, 0, sizeof (ssd_t));
   _ssd.status = DRV_NODEV;
}

/*!
 * \brief
 *    Initialize the ssd driver
 */
drv_status_en ssd_init (void)
{
   #define _ssd_assert(_x)  if (!_x) return _ssd.status = DRV_ERROR;

   _ssd_assert (_ssd.io.bus);
   _ssd_assert (_ssd.io.dis);
   _ssd_assert (_ssd.fb.buffer);
   /*
    * We are here, so all its OK. We can (re)initialize alcd.
    */
   _ssd.status = DRV_NOINIT;

   if (_ssd.blink_time == 0)  _ssd.blink_time = (clock_t)(SSD_BLINK_TIME_DEF * get_freq ());
   if (_ssd.digits == 0)      _ssd.digits = SSD_DIGITS_DEF;
   if (_ssd.fb.size == 0)     _ssd.fb.size = SSD_FB_SIZE_DEF;

   return _ssd.status = DRV_READY;

   #undef _ssd_assert
}

/*!
 * \brief
 *    Send an ascii character to ssd.
 * \param  ch     the character to send
 * \return the character send.
 *
 * \note
 *    This is the driver's "putchar()" functionality to glue.
 *    Tailor this function to redirect stdout to ssd.
 */
int ssd_putchar (int ch)
{
   // ssd character dispatcher
   switch (ch) {
      case 0:
         // don't send null termination to device
         break;
      case '\r':
         _fb_ret_home ();
         break;
      case '\n':
      case '\v':
      case '\f':
         _fb_clear ();
         _fb_ret_home ();
         break;
      case '\b':
         _fb_del ();
         break;
      default:
         _fb_put_character (ch);
         break;
   }

   //ANSI C (C99) compatible mode
   return ch;
}

inline void ssd_power (uint8_t en) {
   _ssd.power = en;
}

inline void ssd_blink (uint8_t b) {
   _ssd.blink = b;
}


/*!
 * \brief
 *    sevn segment display ioctl function
 *
 * \param  cmd   specifies the command to FLASH
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_POWER
 *    \arg CTRL_CLEAR_ALL
 *    \arg CTRL_SHIFT
 * \param  data  data for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en  ssd_ctl (ioctl_cmd_t cmd, ioctl_data_t data)
{
   switch (cmd) {
      case CTRL_GET_STATUS:            /*!< Probe function */
         if (data)
            data = (drv_status_en)_ssd.status;
         return DRV_READY;
      case CTRL_DEINIT:                /*!< De-init */
         ssd_deinit ();
         return DRV_READY;
      case CTRL_INIT:                  /*!< Init */
         if (data)
            data = (drv_status_en)ssd_init ();
         else
            ssd_init ();
         return DRV_READY;
      case CTRL_POWER:                 /*!< Enable/disable */
         ssd_power ((uint8_t)data);
         return _ssd.status = DRV_READY;
      case CTRL_CLEAR:                 /*!< Clear screen and return home */
         _fb_clear ();
         _fb_ret_home ();
         return _ssd.status = DRV_READY;
      case CTRL_SHIFT:                 /*!< Shift lcd data */
         return DRV_READY;
      default:                         /*!< Unsupported command, error */
         return DRV_ERROR;
   }
}
