/*!
 * \file tui_valuebox.c
 * \brief
 *    Value-box functionality for RTOS environments.
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
static void _mk_frame (tui_t *tui, float v, int dec, text_t units);

// Common API
extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);

/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tui   Pointer to the active tui_t struct
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
 * \param  tui   Pointer to the active tui_t struct
 * \param  v      Value to print
 * \param  dec    The number of decimal points to use
 * \param  units  The units string to append.
 * \return none
 */
static void _mk_frame (tui_t *tui, float v, int dec, text_t units)
{
   #define _LINE(_l)    (tui->frame_buffer.c*(_l))
   int offset=0;

   // CLear frame
   if (_tuix_clear_frame (&tui->frame_buffer))
      return;
   // Print value
   switch (dec)
   {
      case 0: offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)], "=%d %s", (int)v, (char*)units);
         break;
      default:
      case 1: offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)], "=%.1f %s", v, (char*)units);
         break;
      case 2: offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)], "=%.2f %s", v, (char*)units);
         break;
      case 3: offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)], "=%.3f %s", v, (char*)units);
         break;
      case 4: offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)], "=%.4f %s", v, (char*)units);
         break;
   }
   // discard null termination inside frame buffer
   tui->frame_buffer.fb[_LINE(1)+offset] = ' ';
   #undef _LINE
}

/*!
 * \brief
 *    Creates a Value box between a min-max domain
 *
 * \param   tui      Pointer to the active tui_t structure
 * \param   cap      The Value box caption
 * \param   units    The units text
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 * \param   dec      Number of decimal digits (0 for integer)
 * \param   value    current value
 *
 * * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a Value box between a min-max domain.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the value by step
 * DOWN     --    Decrease the value by step
 * RIGHT    --    Selected and return the current value.
 * LEFT     --    Exit with the previous value
 * ESC      --       "        "        "
 */
float tui_valuebox (tui_t *tui, text_t cap, text_t units, float up, float down, float step, int dec, float cur)
{
   float v = cur;
   int key, speedy=0;

   // Print caption
   _mk_caption (tui, cap);
   while (1) {
      // Printing frame
      _mk_frame (tui, v, dec, units);
      //Get user choice
      key = (tui->get_key) ? tui->get_key (0) : -1;

      //Navigating
      if (key == tui->keys.UP)
         v += step + step*(speedy++/10);
      else if (key == tui->keys.DOWN)
         v -= step + step*(speedy++/10);
      else if (key == tui->keys.ESC || key == tui->keys.LEFT)
         return cur;       // Restore previous value
      else if (key == tui->keys.RIGHT || key == tui->keys.ENTER)
         return v;         // Return the new value.
      else
         speedy = 0;

      //Cycle the values
      if (v > up)     v = down;
      if (v < down)   v = up;
   }
}
