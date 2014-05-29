/*!
 * \file tui_textboxd.c
 * \brief
 *    Text-box functionality for RTOS environments.
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
static void _mk_frame (tui_t *tui, char* str);
static int _strcpy (char* to, const char* from, int size);

/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tui    Pointer to the active tui_t struct
 * \param  cap    Pointer to caption string
 * \return none
 */
static void _mk_caption (tui_t *tui, text_t cap)
{
   if (!tui->frame_buffer.fb)
      return;
   // Clear ALL fb's caption first
   memset ((char*)&tui->frame_buffer.fb[0], ' ', tui->frame_buffer.c-1);
   tui->frame_buffer.fb[tui->frame_buffer.c-1] = 0; // Keep null termination at end of line

   // Print caption
   sprintf ((char*)&tui->frame_buffer.fb[0], "%s", (char*)cap);
   tui->frame_buffer.fb[strlen ((const char*)cap)] = ' ';
   /*
    * discard null termination inside frame buffer
    */
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tui    Pointer to the active tui_t struct
 * \param  str    The string to print
 * \return none
 */
static void _mk_frame (tui_t *tui, char* str)
{
   #define _LINE(_l)    (tui->frame_buffer.c*(_l))
   int line, offset=0;

   if (!tui->frame_buffer.fb)
      return;
   // Clear fb's frame first
   for (line=1 ; line<tui->frame_buffer.l ; ++line) {
      memset ((char*)&tui->frame_buffer.fb[_LINE(line)], ' ', tui->frame_buffer.c-1);
      tui->frame_buffer.fb[_LINE(line+1)-1] = 0;
      /*
       * Keep null termination at end of each
       * frame buffer's line
       */
   }
   // Print text
   offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(1)], ":%s<", str);
   // discard null termination inside frame buffer
   tui->frame_buffer.fb[_LINE(1)+offset] = ' ';
   #undef _LINE
}

/*!
 * \brief
 *    Copy the from-string to to-string and stops at:
 *    - from string end
 *    - size limit
 *    - UI_TEXTBOX_SIZE size limit
 *    Whatever comes first.
 * \param   to    destination string
 * \param   from  source string
 * \param   size  the size limit
 * \return        The copied numbers.
 */
static int _strcpy (char* to, const char* from, int size)
{
   int i;
   for (i=0 ; from[i] && i<size && i<UI_TEXTBOX_SIZE; ++i)
      to[i] = from[i];
   to[i] = 0;
   return i;
}

/*!
 * \brief
 *    Creates a Textbox using lowercase, uppercase, digits and '-'
 *
 * \param   tui     Pointer to the active tui_t structure
 * \param   cap      The Value box caption
 * \param   str      Pointer to string
 * \param   size     The string's size
 *
 * * \return  none
 *
 * This function can create a text box.
 *
 * Navigation
 * ==========================
 * UP      --    Increase the current character
 * DOWN    --    Decrease the current character
 * LEFT    --    Deletes the last character.
 * RIGHT   --    Copy the current character to next and go to next (at end returns)
 * ENTER   --       "  "
 * ENTER_L --    Return the string as is (don't need to reach the end).
 * ESC     --    Returns the string "as is"
 */
void tui_textbox (tui_t *tui, text_t cap, char* str, int size)
{
   int key, i=0;
   char bf[UI_TEXTBOX_SIZE];


   if (!*str) {
      // init string if needed
      _strcpy (bf, "A", 1);
   }
   else {
      // Copy and go to last character
      i = _strcpy (bf, str, size) - 1;
   }
   // Print caption
   _mk_caption (tui, cap);
   while (1) {
      // Printing frame
      _mk_frame (tui, bf);
      //Get user choice
      key = (tui->get_key) ? tui->get_key (0) : -1;

      //Navigating
      if (key == tui->keys.UP)      // Increment character
         do
            bf[i]++;
         while ( !isalnum ((int)bf[i]) &&
                 bf[i]!='-' &&
                 bf[i]!='_' );
      if (key == tui->keys.DOWN)    // Decrement character
         do
            bf[i]--;
         while ( !isalnum ((int)bf[i]) &&
                 bf[i]!='-' &&
                 bf[i]!='_' );
      if (key == tui->keys.LEFT) {     // Manual backspace
         bf[i] = 0;
         if (--i<0) return;
      }
      if (key == tui->keys.RIGHT || key == tui->keys.ENTER) {
         // Next or OK
         if (++i>=size) {     // At the end of the string we return the string
            _strcpy (str, bf, size);
            return;
         }
         // Copy the previous character
         if (!bf[i])  bf[i] = bf[i-1];
         bf[i+1] = 0;
      }
      if (key == tui->keys.ENTER_L) {  // Return this string
         _strcpy (str, bf, size);
         return;
      }
      if (key == tui->keys.ESC)
         return;
   }
}

