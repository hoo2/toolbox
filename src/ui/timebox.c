/*!
 * \file timebox.c
 * \brief
 *    A plain and demonised time-box functionality.
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
#include <ui/timebox.h>

/*!
 * \brief
 *    Creates a Time value box between a min-max domain
 *
 * \param   key      User input
 * \param   cap      The Value box caption
 * param    frm      Format string
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
ui_return_t ui_timebox (int key, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t *value)
{
   static time_t cur, t;
   static int ev=1, speedy=0;
   char value_str[UI_TIMEBOX_SIZE];
   struct tm *s;
   int i=0;

   // First (each) time
   if (ev)
   {
      cur = t = *value;
      ev = 0;
      speedy = 0;
      ui_print_caption (cap);
   }

   //Navigating
   if (key == ui_keys.UP)           t += step + step*(speedy++/10);
   else if (key == ui_keys.DOWN)    t -= step + step*(speedy++/10);

   else if (key == ui_keys.ESC || key == ui_keys.LEFT)
   {  // Restore previous value
      *value = cur;
      ev = 1;
      return EXIT_RETURN;
   }
   else if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
   {  // Return the new value.
      *value = t;
      ev = 1;
      return EXIT_RETURN;
   }
   else
      speedy = 0;

   //Cycle the values
   if (t > up)     t = down;
   if (t < down)   t = up;

   // Time Box is painting the screen
   s = sgmtime(&t);
   i = sprintf (&value_str[i], "= ");
   if (frm & UI_TIME_DD)   i += sprintf (&value_str[i], "%dd+ ", s->tm_yday);
   if (frm & UI_TIME_HH)   i += sprintf (&value_str[i], "%02d", s->tm_hour);
   if (frm & UI_TIME_MM)   i += sprintf (&value_str[i], ":%02d", s->tm_min);
   if (frm & UI_TIME_SS)   i += sprintf (&value_str[i], ":%02d\"", s->tm_sec);
   else                    i += sprintf (&value_str[i], "\'");
   ui_print_box (value_str);

   return EXIT_STAY;
}
