/*!
 * \file tui_timebox.c
 * \brief
 *    Time-box functionality for RTOS environments
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2010-2014 Houtouridis Christos (http://www.houtouridis.net)
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
 *
 */
#include <ui/tui.h>

// Static functions
static void _mk_caption (tui_t *tui, text_t cap);
static void _mk_frame (tui_t *tui, time_t t, uint8_t frm);

// Common API
extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);

/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tui    Pointer to the active tui_t structure
 * \param  cap    Pointer to caption string
 * \return none
 */
static void _mk_caption (tui_t *tui, text_t cap)
{
   _tuix_mk_caption (&tui->frame_buffer, cap);
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tui    Pointer to the active tui_t structure
 * \param  t      time value to print
 * \param  frm    Format string
 *    \arg UI_TIME_SS
 *    \arg UI_TIME_MM
 *    \arg UI_TIME_HH
 *    \arg UI_TIME_DD
 * \return none
 */
static void _mk_frame (tui_t *tui, time_t t, uint8_t frm)
{
   #define _LINE(_l)    (tui->frame_buffer.c*(_l))
   int i=0;
   struct tm *s;

   // CLear frame
   if (_tuix_clear_frame (&tui->frame_buffer))
      return;
   // Print value
   s = sgmtime(&t);
   i = sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)], "= ");
   if (frm & UI_TIME_DD)   i += sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)+i], "%dd+ ",   s->tm_yday);
   if (frm & UI_TIME_HH)   i += sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)+i], "%02d",    s->tm_hour);
   if (frm & UI_TIME_MM)   i += sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)+i], ":%02d",   s->tm_min);
   if (frm & UI_TIME_SS)   i += sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)+i], ":%02d\"", s->tm_sec);
   else                    i += sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)+i], "\'");
   // discard null termination inside frame buffer
   tui->frame_buffer.fb[_LINE(1)+i] = ' ';
   #undef _LINE
}

/*!
 * \brief
 *    Creates a Time value box between a min-max domain
 *
 * \param   tui      Pointer to the active tui_t structure
 * \param   cap      The Value box caption
 * \param   frm      Format string
 *    \arg UI_TIME_SS
 *    \arg UI_TIME_MM
 *    \arg UI_TIME_HH
 *    \arg UI_TIME_DD
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 * \param   cur      Current value.
 *
 * * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a Time value box between a min-max domain.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the value by step
 * DOWN     --    Decrease the value by step
 * RIGHT    --    Selected and return the current time value.
 * LEFT     --    Exit with the previous time value
 * ESC      --       "        "        "
 */
time_t tui_timebox (tui_t *tui, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t cur)
{
   time_t t = cur;
   int key, speedy=0;

   _mk_caption (tui, cap);
   while (1) {
      // Printing frame
      _mk_frame (tui, t, frm);
      //Get user choice
      key = (tui->get_key) ? tui->get_key (0) : -1;

      //Navigating
      if (key == tui->keys.UP)           t += step + step*(speedy++/10);
      else if (key == tui->keys.DOWN)    t -= step + step*(speedy++/10);
      else if (key == tui->keys.ESC || key == tui->keys.LEFT)
         return cur;       // Restore previous value
      else if (key == tui->keys.RIGHT || key == tui->keys.ENTER)
         return t;         // Return the new value.
      else
         speedy = 0;

      //Cycle the values
      if (t > up)     t = down;
      if (t < down)   t = up;
   }
}
