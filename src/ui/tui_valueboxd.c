/*!
 * \file tui_valueboxd.c
 * \brief
 *    A demonised value-box functionality.
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
static void _mk_frame (tuid_t *tuid, float v, int dec, text_t units);
static void _mk_line (tuid_t *tuid, float v, int width, int dec, text_t units);

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
__O3__ static void _mk_caption (tuid_t *tuid, text_t cap) {
   _tuix_mk_caption (&tuid->frame_buffer, cap);
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  v      Value to print
 * \param  dec    The number of decimal points to use
 * \param  units  The units string to append.
 * \return none
 */
__Os__ static void _mk_frame (tuid_t *tuid, float v, int dec, text_t units)
{
   #define _LINE(_l)    (tuid->frame_buffer.c*(_l))
   int offset=0;

   // CLear frame
   if (_tuix_clear_frame (&tuid->frame_buffer))
      return;
   // Print value
   if (dec == 0)
      offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)], "=%d", (int)v);
   else
      offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)], "=%.*f", dec, v);

   offset += sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)+offset], " %s", (char*)units);

   // discard null termination inside frame buffer
   tuid->frame_buffer.fb[_LINE(1)+offset] = ' ';
   // Keep null termination at end of each line
   tuid->frame_buffer.fb[_LINE(1)+tuid->frame_buffer.c-1] = 0;
   #undef _LINE
}


/*!
 * \brief
 *    Paints the line in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  v      Value to print
 * \param  dec    The number of decimal points to use
 * \param  units  The units string to append.
 * \return none
 */
__Os__ static void _mk_line (tuid_t *tuid, float v, int width, int dec, text_t units)
{
   int offset=0;

   // CLear frame
   memset ((char*)tuid->frame_buffer.fb, ' ', tuid->frame_buffer.c-1);

   // Print value
   if (dec == 0)
      offset = sprintf ((char*)tuid->frame_buffer.fb, "%*d", width, (int)v);
   else
      offset = sprintf ((char*)tuid->frame_buffer.fb, "%*.*f", width, dec, v);

   if (*units != 0)
      offset += sprintf ((char*)(tuid->frame_buffer.fb+offset), " %s", (char*)units);

   // discard null termination inside frame buffer
   tuid->frame_buffer.fb[offset] = ' ';
   // Keep null termination at end of each line
   tuid->frame_buffer.fb[tuid->frame_buffer.c-1] = 0;
}

/*!
 * \brief
 *    Creates a Value box between a min-max domain
 *
 * \param   tuid     Pointer to the active tuid_t structure
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
__Os__ ui_return_t tui_valueboxd (tuid_t *tuid, int live, int key, text_t cap, text_t units, float up, float down, float step, int dec, float *value)
{
   static float cur, v;
   static int ev=1, speedy=0;

   // First (each) time
   if (ev) {
      cur = v = *value;
      ev = 0;
      speedy = 0;
      _mk_caption (tuid, cap);
   }

   if (live)
      *value = v;

   //Navigating
   if (key == tuid->keys.UP)
      v += step + step*(speedy++/10);
   else if (key == tuid->keys.DOWN)
      v -= step + step*(speedy++/10);
   else if (key == tuid->keys.ESC || key == tuid->keys.LEFT) {
      // Restore previous value
      *value = cur;
      ev = 1;
      return EXIT_RETURN;
   }
   else if (key == tuid->keys.RIGHT || key == tuid->keys.ENTER) {
      // Return the new value.
      *value = v;
      ev = 1;
      return EXIT_RETURN;
   }
   else
      speedy = 0;

   //Cycle the values
   if (v > up)     v = down;
   if (v < down)   v = up;

   // Print frame
   _mk_frame (tuid, v, dec, units);

   return EXIT_STAY;
}

__Os__ ui_return_t tui_line_valueboxd (tuid_t *tuid, int live, int key, text_t units, float up, float down, float step, int width, int dec, float *value)
{
   static float cur, v;
   static int ev=1, speedy=0;

   // First (each) time
   if (ev) {
      cur = v = *value;
      ev = 0;
      speedy = 0;
   }

   if (live)
      *value = v;

   //Navigating
   if (key == tuid->keys.UP)
      v += step + step*(speedy++/10);
   else if (key == tuid->keys.DOWN)
      v -= step + step*(speedy++/10);
   else if (key == tuid->keys.ESC || key == tuid->keys.LEFT) {
      // Restore previous value
      *value = cur;
      ev = 1;
      return EXIT_RETURN;
   }
   else if (key == tuid->keys.RIGHT || key == tuid->keys.ENTER) {
      // Return the new value.
      *value = v;
      ev = 1;
      return EXIT_RETURN;
   }
   else
      speedy = 0;

   //Cycle the values
   if (v > up)     v = down;
   if (v < down)   v = up;

   // Send line for printing
   _mk_line (tuid, v, width, dec, units);

   return EXIT_STAY;
}
