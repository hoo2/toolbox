/*!
 * \file tui_cmn.c
 * \brief
 *    A small tui library.
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
#include <ui/tuid.h>

// Local declarations
void _tuix_mk_caption (fb_t *fb, text_t cap);
int _tuix_clear_frame (fb_t *fb);

void _cmb_frame_lines (fb_t *fb, combobox_item_t *items, int frame, int item, Lang_en ln);
void _cmb_next_item (combobox_item_t *items, int *it);
void _cmb_prev_item (combobox_item_t *items, int *it);

/*!
 * \brief
 *    Common make caption functionality for the tui lib.
 * \param   fb    Pointer to frame buffer
 * \param   cap   Caption string
 * \return  none
 */
void _tuix_mk_caption (fb_t *fb, text_t cap)
{
   int offset;
   if (!fb->fb)   // No frame buffer attached, return
      return;
   // Clear ALL fb's caption first
   memset ((char*)&fb->fb[0], ' ', fb->c-1);

   // Print caption
   offset = sprintf ((char*)&fb->fb[0], "%s", (char*)cap);
   fb->fb[offset] = ' ';   // discard null termination inside frame buffer
   fb->fb[fb->c-1] = 0;    // Keep null termination at end of caption line

}

/*!
 * \brief
 *    Common clear frame functionality for the tui lib.
 * \param   fb    Pointer to frame buffer
 * \return  status
 *    \arg  0  OK
 *    \arg  1  Error
 */
int _tuix_clear_frame (fb_t *fb)
{
   int line;

   if (!fb->fb)   // No frame buffer attached, return
      return 1;
   // Clear fb's frame
   for (line=1 ; line < fb->l ; ++line) {
      memset ((char*)&fb->fb[fb->c*line], ' ', fb->c-1);
      fb->fb[fb->c*(line+1)-1] = 0;
      /*
       * Keep null termination at end of each
       * frame buffer's line
       */
   }
   return 0;
}

/*!
 * \brief
 *    Paints the combobox frame lines in the frame buffer
 * \param  fb     Pointer to the frame buffer
 * \param  items  Pointer to the active combo-box
 * \param  frame  The frame buffer's start position
 * \param  item   The frame buffer's active line
 * \param  ln     The language to use
 * \return none
 */
void _cmb_frame_lines (fb_t *fb, combobox_item_t *items, int frame, int item, Lang_en ln)
{
   #define _LINE(_l)    (fb->c*(_l))
   int line, offset;
   int start;
   char post;

   // Print each line
   start = frame;
   for (line=1 ; line < fb->l ; ++line) {
      offset=0;
      if (frame == item) {
         offset = sprintf ((char*)&fb->fb[_LINE(line)], "[%s", (char*)items[frame].text[ln]);
         post = ']';
      }
      else {
         offset = sprintf ((char*)&fb->fb[_LINE(line)], "%s", (char*)items[frame].text[ln]);
         post = ' ';
      }
      // discard null termination inside frame buffer
      fb->fb[_LINE(line)+offset] = post;

      // Escape if no items left
      _cmb_next_item (items, &frame);
      if (frame == start)
         break;
   }
   #undef _LINE
}

/*!
 * \brief
 *    Increment \a it to point to the next item in combobox and
 *    roll the numbers if necessary.
 * \param  items  Pointer to the active combobox
 * \param  it     Pointer to the item to update
 * \return none
 */
void _cmb_next_item (combobox_item_t *items, int *it)
{
   if (!items[++*it].text[0])
      *it=1;
}

/*!
 * \brief
 *    Decrement \a it to point to the next item in combobox and
 *    roll the numbers if necessary.
 * \param  items  Pointer to the active combobox
 * \param  it     Pointer to the item to update
 * \return none
 */
void _cmb_prev_item (combobox_item_t *items, int *it)
{
   if (!--*it) {
      for (*it=1 ; items[*it].text[0]; ++*it)
         ;
      --*it;
   }
}
