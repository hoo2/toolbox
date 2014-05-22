/*!
 * \file valuebox.c
 * \brief
 *    A plain and demonised value-box functionality.
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
#include <ui/valuebox.h>

/*!
 * \brief
 *    Creates a Value box between a min-max domain
 *
 * \param   key      User input
 * \param   cap      The Value box caption
 * \param   units    The units text
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 * \param   dec      Number of decimal digits (0 for integer)
 * \param   value    Pointer to value (current and returned).
 *
 * * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a Value box between a min-max domain.
 * While the function returns EXIT_STAY it is still in progress. When the function
 * is done returns EXIT_RETURN. This assumes that the caller must handle with return
 * status in order to continues call or not the function.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the value by step
 * DOWN     --    Decrease the value by step
 * RIGHT    --    Selected and return the current value.
 * LEFT     --    Exit with the previous value
 * ESC      --       "        "        "
 */
ui_return_t ui_valuebox (int key, text_t cap, text_t units, float up, float down, float step, int dec, float *value)
{
   static float cur, v;
   static int ev=1, speedy=0;
   char value_str[12];


   // First (each) time
   if (ev)
   {
      cur = v = *value;
      ev = 0;
      speedy = 0;
      ui_print_caption (cap);
   }

   //Navigating
   if (key == ui_keys.UP)           v += step + step*(speedy++/10);
   else if (key == ui_keys.DOWN)    v -= step + step*(speedy++/10);
   else if (key == ui_keys.ESC || key == ui_keys.LEFT)
   {  // Restore previous value
      *value = cur;
      ev = 1;
      return EXIT_RETURN;
   }
   else if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
   {  // Return the new value.
      *value = v;
      ev = 1;
      return EXIT_RETURN;
   }
   else
      speedy = 0;

   //Cycle the values
   if (v > up)     v = down;
   if (v < down)   v = up;

   //User Interface Loop
   switch (dec)
   {
      case 0:
         sprintf (value_str, "=%d %s", (int)v, (char*)units); break;
      default:
      case 1:
         sprintf (value_str, "=%.1f %s", v, (char*)units);    break;
      case 2:
         sprintf (value_str, "=%.2f %s", v, (char*)units);    break;
      case 3:
         sprintf (value_str, "=%.3f %s", v, (char*)units);    break;
   }
   ui_print_box (value_str);

   return EXIT_STAY;
}
