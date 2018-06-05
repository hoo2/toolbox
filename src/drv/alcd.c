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
 */

#include <drv/alcd.h>

static  int _inc_x (alcd_t *alcd) __Os__ ;
static  int _dec_x (alcd_t *alcd) __Os__ ;
static void _inc_y (alcd_t *alcd) __Os__ ;
static void _dec_y (alcd_t *alcd) __Os__ ;
static void _set_bus (alcd_t *alcd, int8_t db) __Os__ ;
static void _write_data (alcd_t *alcd, int8_t data) __Os__ ;
static void _command (alcd_t *alcd, uint8_t c) __Os__ ;
static void _character (alcd_t *alcd, uint8_t c) __Os__ ;
static void _set_cursor (alcd_t *alcd, uint8_t x, uint8_t y) __Os__ ;


/*!
 * \brief
 *    increase cursor's x position. Positions start from 1.
 *    If the cursor loops returns 1, else returns 0.
 * \param  alcd   pointer to active alcd.
 * \return        Change line status
 */
static int _inc_x (alcd_t *alcd) {
   int ret=0;

   if (++alcd->c.x > alcd->columns) {
      alcd->c.x = 1;
      ret = 1;
   }
   return ret;
}

/*!
 * \brief
 *    Decrease cursor's x position. Positions start from 1.
 *    If the cursor loops returns 1, else returns 0.
 * \param  alcd   pointer to active alcd.
 * \return none
 */
static int _dec_x (alcd_t *alcd) {
   int ret = 0;

   if (--alcd->c.x < 1) {
      alcd->c.x = alcd->columns;
      ret = 1;
   }
   return ret;
}

/*!
 * \brief
 *    increase cursor's y position. Positions start from 1.
 * \param  alcd   pointer to active alcd.
 * \return none
 */
static void _inc_y (alcd_t *alcd) {
   if (++alcd->c.y > alcd->lines) {
      alcd->c.y = 1;
   }
}

/*!
 * \brief
 *    Decrease cursor's y position. Positions start from 1.
 * \param  alcd   pointer to active alcd.
 * \return none
 */
static void _dec_y (alcd_t *alcd) {
   if (--alcd->c.y < 1) {
      alcd->c.y = alcd->lines;
   }
}

/*!
 * \brief
 *    Update the bus I/O and send it to the device.
 * \param  alcd   pointer to active alcd.
 * \param  db     the bus data.
 * \return none
 */
static void _set_bus (alcd_t *alcd, int8_t db)
{
   alcd->io.db4 (db & 0x01);   //Update port
   alcd->io.db5 (db & 0x02);
   alcd->io.db6 (db & 0x04);
   alcd->io.db7 (db & 0x08);
   jf_delay_us (10);     // Wait to settle

   alcd->io.en (1);      // Pulse out the data
   jf_delay_us (10);
   alcd->io.en (0);
   jf_delay_us (10);     // Data hold
}

/*!
 * \brief
 *    Write the byte date to the bus using _set_bus ()
 * \param  alcd   pointer to active alcd.
 * \param  data   the data byte.
 * \return none
 */
static void _write_data (alcd_t *alcd, int8_t data)
{
   _set_bus (alcd, data >> 4);
   _set_bus (alcd, data & 0x0F);
}

/*!
 * \brief
 *    Send a command to alcd
 * \param  alcd   pointer to active alcd.
 * \param  c      the command byte.
 * \return none
 */
static void _command (alcd_t *alcd, uint8_t c)
{
   alcd->io.rs(0);         // Enter command mode
   jf_delay_us (100);      // Wait
   _write_data (alcd, c);  // Send
}

/*!
 * \brief
 *    Send a character to alcd
 * \param  alcd   pointer to active alcd.
 * \param  c      the character byte.
 * \return none
 */
static void _character (alcd_t *alcd, uint8_t c)
{
   alcd->io.rs(1);         // Enter character mode
   jf_delay_us (100);      // Wait
   _write_data (alcd, c);  // Send
}

/*!
 * \brief
 *    Set the Cursor to LCD's position line (y), column (x) starts from 1,2,...n
 * \param  alcd   pointer to active alcd.
 * \param  x      the x position.
 * \param  y      the y position.
 * \return none
 */
static void _set_cursor (alcd_t *alcd, uint8_t x, uint8_t y)
{
   uint8_t cmd;

   alcd->c.x = x;             // Update alcd data
   alcd->c.y = y;

   // Calculate address
   switch (y) {
      default:
      case 1: cmd = 0x0;   break;
      case 2: cmd = 0x40;  break;
      case 3: cmd = 0x0 + alcd->columns;  break;
      case 4: cmd = 0x40 + alcd->columns; break;
   }
   cmd |= (x - 1);

   // Calculate command
   cmd |= LCD_DDRAMMask;

   // Command out the alcd
   _command (alcd, cmd);
}


/*
 * ============================ Public Functions ============================
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief
 *    Link driver's db4 pin function to io struct.
 * \param  alcd   pointer to active alcd.
 * \param  pfun   driver's DB4 pin function
 * \return none
 */
inline void alcd_link_db4 (alcd_t *alcd, alcd_pin_t pfun) { alcd->io.db4 = pfun; }

/*!
 * \brief
 *    Link driver's db4 pin function to io struct.
 * \param  alcd   pointer to active alcd.
 * \param  pfun   driver's DB5 pin function
 * \return none
 */
inline void alcd_link_db5 (alcd_t *alcd, alcd_pin_t pfun) { alcd->io.db5 = pfun; }

/*!
 * \brief
 *    Link driver's db4 pin function to io struct.
 * \param  alcd   pointer to active alcd.
 * \param  pfun   driver's DB6 pin function
 * \return none
 */
inline void alcd_link_db6 (alcd_t *alcd, alcd_pin_t pfun) { alcd->io.db6 = pfun; }

/*!
 * \brief
 *    Link driver's db4 pin function to io struct.
 * \param  alcd   pointer to active alcd.
 * \param  pfun   driver's DB7 pin function
 * \return none
 */
inline void alcd_link_db7 (alcd_t *alcd, alcd_pin_t pfun) { alcd->io.db7 = pfun; }

/*!
 * \brief
 *    Link driver's db4 pin function to io struct.
 * \param  alcd   pointer to active alcd.
 * \param  pfun   driver's RS pin function
 * \return none
 */
inline void alcd_link_rs (alcd_t *alcd, alcd_pin_t pfun) { alcd->io.rs = pfun; }

/*!
 * \brief
 *    Link driver's db4 pin function to io struct.
 * \param  alcd   pointer to active alcd.
 * \param  pfun   driver's EN pin function
 * \return none
 */
inline void alcd_link_en (alcd_t *alcd, alcd_pin_t pfun) { alcd->io.en = pfun; }

/*!
 * \brief
 *    Link driver's db4 pin function to io struct.
 * \param  alcd   pointer to active alcd.
 * \param  pfun   driver's BL pin function
 * \return none
 */
inline void alcd_link_bl (alcd_t *alcd, alcd_pin_t pfun) { alcd->io.bl = pfun; }

/*!
 * \brief
 *    Send an ascii character to alcd.
 * \param  alcd   pointer to active alcd.
 * \param  ch     the character to send
 * \return the character send.
 *
 * \note
 *    This is the driver's "putchar()" functionality to glue.
 *    Tailor this function to redirect stdout to alcd.
 */
int alcd_putchar (alcd_t *alcd, int ch)
{
   alcd->status = DRV_BUSY;
   // LCD Character dispatcher
   switch (ch) {
      case 0:
         // don't send null termination to device
         break;
      case '\n':
         _inc_y (alcd);
         //break; This "no break" is intentional
      case '\r':
         _set_cursor (alcd, 1, alcd->c.y);
         break;
      case '\v':
         alcd->c.x = alcd->c.y = 1;
         _command (alcd, LCD_RETHOME);
         jf_delay_us(2000);
         break;
      case '\f':
         //alcd->c.x = alcd->c.y = 1;
         _set_cursor (alcd, 1, 1);
           //_command (alcd, LCD_CLRSCR);
           //jf_delay_us(5000);
         //_command (alcd, LCD_RETHOME);
           //_set_cursor (alcd, alcd->c.x, alcd->c.y);
         jf_delay_us(2000);
         break;
      case '\b':
         if (_dec_x (alcd))   _dec_y (alcd);
         _set_cursor (alcd, alcd->c.x, alcd->c.y);
         _character (alcd, ' ');
         _set_cursor (alcd, alcd->c.x, alcd->c.y);
         break;
      default:
         _character (alcd, ch);
         // Increase cursor and loop inside the same line
         if (_inc_x (alcd))   _set_cursor (alcd, alcd->c.x, alcd->c.y);
         break;
   }

   // Restore status
   alcd->status = DRV_READY;

   //ANSI C (C99) compatible mode
   return ch;
}


/*
 * Set functions
 */

/*!
 * \brief
 *    Set the number of lines for the attached lcd display
 * \param  alcd   pointer to active alcd.
 * \param  lines  The number of lines (usually 2 or 4)
 * \return None.
 */
void alcd_set_lines (alcd_t *alcd, int lines) {
   alcd->lines = lines;
}

/*!
 * \brief
 *    Set the number of columns for the attached lcd display
 * \param  alcd   pointer to active alcd.
 * \param  lines  The number of columns (usually 16 or 20)
 * \return None.
 */
void alcd_set_columns (alcd_t *alcd, int columns) {
   alcd->columns = columns;
}

/*
 * User Functions
 */

/*!
 * \brief
 *    De-initialize the alcd.
 * \param  alcd   pointer to active alcd.
 * \return none
 */
__Os__ void alcd_deinit (alcd_t *alcd)
{
   memset ((void*)alcd, 0, sizeof (alcd_t));
   /*!<
    * This leaves the status DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialize the alcd.
 * \param  alcd   pointer to active alcd.
 * \return Zero on success, non zero on error
 */
__Os__ drv_status_en alcd_init (alcd_t *alcd, alcd_funset_en fs)
{
   #define _lcd_assert(_x)  if (!_x) return alcd->status = DRV_ERROR;

   drv_status_en st = jf_probe ();

   if (st == DRV_NODEV || st == DRV_BUSY)
      return alcd->status = DRV_ERROR;
   _lcd_assert (alcd->io.db4);
   _lcd_assert (alcd->io.db5);
   _lcd_assert (alcd->io.db6);
   _lcd_assert (alcd->io.db7);
   _lcd_assert (alcd->io.rs);
   _lcd_assert (alcd->io.en);
   //_lcd_assert (alcd->io.bl);

   /*
    * We are here, so all its OK. We can (re)initialize alcd.
    */
   alcd->status = DRV_NOINIT;
   alcd->c.x = alcd->c.y = 1;
   alcd->io.en (0);
   alcd->io.rs (0);
   jf_delay_us (100000);

   //Pre-Init phase 8bit at this point
   _set_bus (alcd, 0x3);
      jf_delay_us(50000);
   _set_bus (alcd, 0x3);
      jf_delay_us(5000);
   _set_bus (alcd, 0x3);
      jf_delay_us(5000);

   _set_bus (alcd, 0x2);              //4bit selection
      jf_delay_us(10000);

   _command (alcd, fs);     //4bit selection and Function Set
      jf_delay_us(5000);
   _command (alcd, LCD_DISP_OFF);    //Display Off Control 4bit for now on
      jf_delay_us(5000);
   _command (alcd, LCD_CLRSCR);      //Clear Display
      jf_delay_us(5000);
   _command (alcd, LCD_ENTRYMODE);   //Entry Mode Set
      jf_delay_us(5000);

   _command (alcd, LCD_RETHOME);
      jf_delay_us(10000);
   _command (alcd, LCD_DISP_ON);
      jf_delay_us(5000);
   //alcd_backlight (alcd, 1);
   return alcd->status = DRV_READY;

   #undef _lcd_assert
}

/*!
 * \brief
 *    Enables and disables the lcd backlight.
 * \param  alcd   pointer to active alcd.
 * \param  on
 *    \arg 0      disable the backlight
 *    \arg 1      enable the backlight
 * \return none
 */
__Os__ void alcd_backlight (alcd_t *alcd, uint8_t on) {
   if (alcd->io.bl)
      alcd->io.bl ((on)?1:0);
}

/*!
 * \brief
 *    Enables and disables the entire lcd (+backlight).
 * \param  alcd   pointer to active alcd.
 * \param  on
 *    \arg 0      disable the backlight
 *    \arg 1      enable the backlight
 * \return none
 */
__Os__ void alcd_enable (alcd_t *alcd, uint8_t on)
{
   if (on) {
      _command (alcd, LCD_DISP_ON);
      alcd_backlight (alcd, 1);
   } else {
      _command (alcd, LCD_DISP_OFF);
      alcd_backlight (alcd, 0);
   }
}

/*!
 * \brief
 *    Clears screen and returns cursor at home position (1,1).
 * \param  alcd   pointer to active alcd.
 * \return none
 */
__Os__ void alcd_cls (alcd_t *alcd)
{
   _command(alcd, LCD_CLRSCR);
   jf_delay_us(2000);
   _command (alcd, LCD_RETHOME);
   jf_delay_us(2000);
}

/*!
 * \brief
 *    Shift alcd left or right for a \a pos characters.
 * \param  alcd   pointer to active alcd.
 * \param  pos    The number of position to shift.
 *    A positive number shifts lcd data to left, so screen shows the data in the right.
 *    A negative number shifts lcd data to right, so screen shows the data in the left.
 * \return none
 */
__Os__ void alcd_shift (alcd_t *alcd, int pos)
{
   uint8_t i, cmd = LCD_SHIFT_LEFT;

   if (pos<0) {
      pos = -pos;
      cmd = LCD_SHIFT_RIGHT;
   }
   for (i=0 ; i<pos ; ++i) {
      _command (alcd, cmd);
      jf_delay_us(100);
   }
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
__Os__ void alcd_createChar (alcd_t *alcd, uint8_t location, uint8_t charmap[])
{
   location &= 0x7;        // we only have 8 locations 0-7
   _command (alcd, LCD_SETCGRAMADDR | (location << 3));

   for (int i=0; i<8; i++) {
      _character (alcd, charmap[i]);
   }
}

/*!
 * \brief
 *    Alpharithmetic LCD ioctl function
 *
 * \param  alcd  The active alcd struct.
 * \param  cmd   specifies the command to FLASH
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_POWER
 *    \arg CTRL_BACKLIGHT
 *    \arg CTRL_CLEAR_ALL
 *    \arg CTRL_SHIFT
 * \param  data  data for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
__Os__ drv_status_en  alcd_ioctl (alcd_t *alcd, ioctl_cmd_t cmd, ioctl_data_t data)
{
   switch (cmd) {
      case CTRL_GET_STATUS:            /*!< Probe function */
         if (data)
            data = (drv_status_en)alcd->status;
         return DRV_READY;
      case CTRL_DEINIT:                /*!< De-init */
         alcd_deinit (alcd);
         return DRV_READY;
      case CTRL_INIT:                  /*!< Init */
         if (data)
            data = (drv_status_en)alcd_init (alcd, LCD_FUNSET_2L8);
         else
            alcd_init (alcd, LCD_FUNSET_2L8);
         return DRV_READY;
      case CTRL_POWER:                 /*!< Enable/disable */
         alcd_enable (alcd, (uint8_t)data);
         return alcd->status = DRV_READY;
      case CTRL_BACKLIGHT:             /*!< Backlight on/off */
         alcd_backlight (alcd, (uint8_t)data);
         return alcd->status = DRV_READY;
      case CTRL_CLEAR:                 /*!< BClear screen and return home */
         alcd_cls (alcd);
         return alcd->status = DRV_READY;
      case CTRL_SHIFT:                 /*!< Shift lcd data */
         alcd_shift (alcd, (int)data);
         return alcd->status = DRV_READY;
      default:                         /*!< Unsupported command, error */
         return DRV_ERROR;
   }
}
