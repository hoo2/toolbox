/*!
 * \file tui_comboboxd.c
 * \brief
 *    A demonised combo-box functionality.
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
static void _next_item (combobox_item_t *items, int *it);
static void _prev_item (combobox_item_t *items, int *it);

static void _mk_caption (tuid_t *tuid, combobox_item_t *items, Lang_en ln);
static void   _mk_frame (tuid_t *tuid, combobox_item_t *items, int frame, int item, Lang_en ln);

/*!
 * \brief
 *    Increment it to point to the next item in combobox and
 *    roll the numbers if necessary.
 * \param  items  Pointer to the active combobox
 * \param  it     Pointer to the item to update
 * \return none
 */
static void _next_item (combobox_item_t *items, int *it)
{
   if (!items[++*it].text[0])
      *it=1;
}

/*!
 * \brief
 *    Decrement it to point to the next item in combobox and
 *    roll the numbers if necessary.
 * \param  items  Pointer to the active combobox
 * \param  it     Pointer to the item to update
 * \return none
 */
static void _prev_item (combobox_item_t *items, int *it)
{
   if (!--*it) {
      for (*it=1 ; items[*it].text[0]; ++*it)
         ;
      --*it;
   }
}

/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  items  Pointer to the active combobox
 * \param  ln     The language to use
 * \return none
 */
static void _mk_caption (tuid_t *tuid, combobox_item_t *items, Lang_en ln)
{
   if (!tuid->frame_buffer.fb)
      return;
   // Clear ALL fb's caption first
   memset ((char*)&tuid->frame_buffer.fb[0], ' ', tuid->frame_buffer.c-1);
   tuid->frame_buffer.fb[tuid->frame_buffer.c-1] = 0; // Keep null termination at end of line

   // Print caption
   sprintf ((char*)&tuid->frame_buffer.fb[0], "%s", (char*)items[0].text[ln]);
   tuid->frame_buffer.fb[strlen ((const char*)items[0].text[ln])] = ' ';
   /*
    * discard null termination inside frame buffer
    */
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  items  Pointer to the active combobox
 * \param  ln     The language to use
 * \return none
 */
static void _mk_frame (tuid_t *tuid, combobox_item_t *items, int frame, int item, Lang_en ln)
{
   #define _LINE(_l)    (tuid->frame_buffer.c*(_l))
   int line, offset;
   int start;
   char post;

   if (!tuid->frame_buffer.fb)
      return;
   // Clear fb's frame first
   for (line=1 ; line<tuid->frame_buffer.l ; ++line) {
      memset ((char*)&tuid->frame_buffer.fb[_LINE(line)], ' ', tuid->frame_buffer.c-1);
      tuid->frame_buffer.fb[_LINE(line+1)-1] = 0;
      /*
       * Keep null termination at end of each
       * frame buffer's line
       */
   }

   // Print each line
   start = frame;
   for (line=1 ; line < tuid->frame_buffer.l ; ++line) {
      offset=0;
      if (frame == item) {
         offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(line)], "[%s", (char*)items[frame].text[ln]);
         post = ']';
      }
      else {
         offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(line)], "%s", (char*)items[frame].text[ln]);
         post = ' ';
      }
      // discard null termination inside frame buffer
      tuid->frame_buffer.fb[_LINE(line)+offset] = post;

      // Escape if no items left
      _next_item (items, &frame);
      if (frame == start)
         break;
   }
   #undef _LINE
}

/*!
 * \brief
 *    Creates a Combo box by reading the items table
 *
 * \param   tuid     Pointer to the active tuid_t structure
 * \param   key      User input
 * \param   items    The Combo Box table
 * \param   id       Pointer to combo box id (current and returned)
 * \param   ln       The language to use.
 *
 * * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a Combo box based on the context of a table.
 * While the function returns EXIT_STAY it is still in progress. When the function
 * is done returns EXIT_RETURN. This assumes that the caller must handle with return
 * status in order to continues call or not the function.
 *
 * For example:
 * const combobox_item_t      cb [] =
 * {
 *   {{"ITEMS",   "ANTIKEIMENA"},   0},   // <-- Caption with whatever id
 *   {{"item 1",  "antik 1"},       1},   // <-- item and item's id
 *   {{"item 2",  "antik 2"},       2},   //       "        "
 *   {{"item 3",  "antik 3"},       3},   //       "        "
 *   {{0,0},0}                            // <-- Terminator strings must be NULL
 * };
 *
 * Navigation
 * ==========================
 * UP       --    Previous item on the list (table)
 * DOWN     --    Next item on the list
 * RIGHT    --    Selected and return the item.
 * LEFT     --    Exit with the previous selected item
 * ESC      --       "        "        "
 */
ui_return_t tui_comboboxd (tuid_t *tuid, int key, combobox_item_t *items, int *id, Lang_en ln)
{
   static uint8_t ev=1;
   static int cur;
   static int vi, vfrm, i, frm;

   if (ev) {  // It is the first call of combobox
      // Find cur in combobox table
      for (i=1 ; items[i].text[ln]; ++i) {
         if (items[i].id == *id)
            break;
         if (!items[i].text[ln]) {
            i=1;
            break;
         }
      }
      // Print caption
      _mk_caption (tuid, items, ln);

      // Update counters
      cur = vi = vfrm = frm = i;
      ev = 0;
   }

   // UI loop - Navigating
   if (key == tuid->keys.UP)     { _prev_item (items, &i); --vi; }
   if (key == tuid->keys.DOWN)   { _next_item (items, &i); ++vi; }
   if (key == tuid->keys.ESC || key == tuid->keys.LEFT) {
      // Restore previous value
      *id = items[cur].id;
      ev = 1;
      return EXIT_RETURN;
   }
   if (key == tuid->keys.RIGHT || key == tuid->keys.ENTER) {
      // Apply the new value
      *id = items[i].id;
      ev = 1;
      return EXIT_RETURN;
   }

   // Roll frame
   if (vi < vfrm) {
      vfrm = vi;
      frm = i;
   }
   else if (vi - vfrm>= tuid->frame_buffer.l - 1) {
      ++vfrm;
      _next_item (items, &frm);
   }

   // Printing frame
   _mk_frame (tuid, items, frm, i, ln);

   return EXIT_STAY;
}
