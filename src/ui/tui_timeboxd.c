/*!
 * \file tui_timeboxd.c
 * \brief
 *    A demonised time-box functionality.
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
#include <ui/tuid.h>

// Static functions
static void _mk_caption (tuid_t *tuid, text_t cap);
static void _mk_frame (tuid_t *tuid, time_t t, uint8_t frm);

// Common API
extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);

/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  cap    Pointer to caption string
 * \return none
 */
static void _mk_caption (tuid_t *tuid, text_t cap)
{
   _tuix_mk_caption (&tuid->frame_buffer, cap);
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  t      time value to print
 * \param  frm    Format string
 *    \arg UI_TIME_SS
 *    \arg UI_TIME_MM
 *    \arg UI_TIME_HH
 *    \arg UI_TIME_DD
 * \return none
 */
static void _mk_frame (tuid_t *tuid, time_t t, uint8_t frm)
{
   #define _LINE(_l)    (tuid->frame_buffer.c*(_l))
   int i=0;
   struct tm *s;

   // CLear frame
   if (_tuix_clear_frame (&tuid->frame_buffer))
      return;
   // Print value
   s = sgmtime(&t);
   i = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)], "= ");
   if (frm & UI_TIME_DD)   i += sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)+i], "%dd+ ",   s->tm_yday);
   if (frm & UI_TIME_HH)   i += sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)+i], "%02d",    s->tm_hour);
   if (frm & UI_TIME_MM)   i += sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)+i], ":%02d",   s->tm_min);
   if (frm & UI_TIME_SS)   i += sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)+i], ":%02d\"", s->tm_sec);
   else                    i += sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)+i], "\'");
   // discard null termination inside frame buffer
   tuid->frame_buffer.fb[_LINE(1)+i] = ' ';
   #undef _LINE
}
/*!
 * \brief
 *    Creates a Time value box between a min-max domain
 *
 * \param   tuid     Pointer to the active tuid_t structure
 * \param   key      User input
 * \param   cap      The Value box caption
 * \param   frm      Format string
 *    \arg UI_TIME_SS
 *    \arg UI_TIME_MM
 *    \arg UI_TIME_HH
 *    \arg UI_TIME_DD
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 * \param   value    Pointer to time value (current and returned).
 *
 * * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a Time value box between a min-max domain.
 * While the function returns EXIT_STAY it is still in progress. When the function
 * is done returns EXIT_RETURN. This assumes that the caller must handle with return
 * status in order to continues call or not the function.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the value by step
 * DOWN     --    Decrease the value by step
 * RIGHT    --    Selected and return the current time value.
 * LEFT     --    Exit with the previous time value
 * ESC      --       "        "        "
 */
ui_return_t tui_timeboxd (tuid_t *tuid, int key, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t *value)
{
   static time_t cur, t;
   static int ev=1, speedy=0;

   // First (each) time
   if (ev) {
      cur = t = *value;
      ev = 0;
      speedy = 0;
      _mk_caption (tuid, cap);
   }

   //Navigating
   if (key == tuid->keys.UP)           t += step + step*(speedy++/10);
   else if (key == tuid->keys.DOWN)    t -= step + step*(speedy++/10);
   else if (key == tuid->keys.ESC || key == tuid->keys.LEFT) {
      // Restore previous value
      *value = cur;
      ev = 1;
      return EXIT_RETURN;
   }
   else if (key == tuid->keys.RIGHT || key == tuid->keys.ENTER) {
      // Return the new value.
      *value = t;
      ev = 1;
      return EXIT_RETURN;
   }
   else
      speedy = 0;

   //Cycle the values
   if (t > up)     t = down;
   if (t < down)   t = up;

   _mk_frame (tuid, t, frm);
   return EXIT_STAY;
}
