/*!
 * \file tui_msgboxd.c
 * \brief
 *    A demonised message-box functionality.
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
static void   _mk_frame (tuid_t *tuid, text_t msg);

// Common API
extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);

/*!
 * \brief
 *    Paints the message frame
 * \param tuid    Pointer to the active tuid_t structure
 * \param msg     Pointer to message
 * \param ln      The language to use
 * \return none
 */
static void _mk_frame (tuid_t *tuid, text_t msg)
{
   #define _LINE(_l)    (tuid->frame_buffer.c*(_l))
   int offset;

   // Clear frame
   _tuix_clear_frame (&tuid->frame_buffer);
   // copy string to fb in all lines automatically
   offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(1)], "%s", (char*)msg);
   // discard null termination inside frame buffer
   tuid->frame_buffer.fb[_LINE(1)+offset] = ' ';

   #undef _LINE
}

/*!
 * \brief
 *    Creates a Message box
 *
 * \param   tuid     Pointer to the active tuid_t structure
 * \param   key      User input
 * \param   cap      The message box caption
 * \param   msg      The message box message
 * \param   ln       The language to use
 *
 * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a text box.
 * While the function returns EXIT_STAY it is still in progress. When the function
 * is done returns EXIT_RETURN. This assumes that the caller must handle with return
 * status in order to continues call or not the function.
 *
 * Keys
 * ==========================
 * ENTER   --    Acknowledge and return EXIT_RETURN
 * RIGHT   --        ""             ""
 * LEFT    --        ""             ""
 * ESC     --        ""             ""
 */
ui_return_t tui_msgboxd (tuid_t *tuid, int key, text_t cap, text_t msg)
{
   static uint8_t ev=1;

   // It is the first call
   if (ev) {
      ev = 0;
      // Print caption
      _tuix_mk_caption (&tuid->frame_buffer, cap);
   }
   _mk_frame (tuid, msg);

   // UI handling
   if (key == tuid->keys.ESC || key == tuid->keys.LEFT) {
      ev = 1;
      return EXIT_RETURN;
   }
   if (key == tuid->keys.RIGHT || key == tuid->keys.ENTER) {
      ev = 1;
      return EXIT_RETURN;
   }

   return EXIT_STAY;
}
