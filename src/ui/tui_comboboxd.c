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
static void _mk_caption (tuid_t *tuid, combobox_item_t *items, Lang_en ln);
static void   _mk_frame (tuid_t *tuid, combobox_item_t *items, int frame, int item, Lang_en ln);

// Common API
extern void _cmb_next_item (combobox_item_t *items, int *it);
extern void _cmb_prev_item (combobox_item_t *items, int *it);
extern void _cmb_frame_lines (fb_t *fb, combobox_item_t *items, int frame, int item, Lang_en ln);

extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);

/*!
 * \brief
 * Paints the Caption line in the frame buffer
 * \param tuid Pointer to the active tuid_t structure
 * \param items Pointer to the active combobox
 * \param ln The language to use
 * \return none
 */
__O3__ static void _mk_caption (tuid_t *tuid, combobox_item_t *items, Lang_en ln)
{
   _tuix_mk_caption (&tuid->frame_buffer, items[0].text[ln]);
}

/*!
 * \brief
 * Paints the frame in the frame buffer
 * \param tuid Pointer to the active tuid_t structure
 * \param items Pointer to the active combobox
 * \param  frame  The frame buffer's start position
 * \param  item   The frame buffer's active line
 * \param ln The language to use
 * \return none
 */
__O3__ static void _mk_frame (tuid_t *tuid, combobox_item_t *items, int frame, int item, Lang_en ln)
{
   // CLear frame
   if (_tuix_clear_frame (&tuid->frame_buffer))
      return;
   // Print each line
   _cmb_frame_lines (&tuid->frame_buffer, items, frame, item, ln);
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
__Os__ ui_return_t tui_comboboxd (tuid_t *tuid, int live, int key, combobox_item_t *items, int *id, Lang_en ln)
{
   static uint8_t ev=1;
   static int cur;
   static int vi, vfrm, i, frm;

   if (ev) {  // It is the first call of combobox
      // Find cur in combobox table
      for (i=1 ; 1 ; ++i) {
         if (!items[i].text[ln]) {
            i=1;
            break;
         }
         if (items[i].id == *id)
            break;
      }
      // Print caption
      _mk_caption (tuid, items, ln);

      // Update counters
      cur = vi = vfrm = frm = i;
      ev = 0;
   }

   if (live)
      *id = items[i].id;

   // UI loop - Navigating
   if (key == tuid->keys.UP)     { _cmb_prev_item (items, &i); --vi; }
   if (key == tuid->keys.DOWN)   { _cmb_next_item (items, &i); ++vi; }
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
      _cmb_next_item (items, &frm);
   }

   // Printing frame
   _mk_frame (tuid, items, frm, i, ln);

   return EXIT_STAY;
}


__Os__ ui_return_t tui_line_comboboxd (tuid_t *tuid, int live, int key, combobox_item_t *items, int *id, Lang_en ln)
{
   static uint8_t ev=1;
   static int cur;
   static int i;

   if (ev) {  // It is the first call of combobox
      // Find cur in combobox table
      for (i=1 ; 1 ; ++i) {
         if (!items[i].text[ln]) {
            i=1;
            break;
         }
         if (items[i].id == *id)
            break;
      }

      // Update counters
      cur = i;
      ev = 0;
   }

   if (live)
      *id = items[i].id;

   // UI loop - Navigating
   if (key == tuid->keys.UP)     _cmb_next_item (items, &i);
   if (key == tuid->keys.DOWN)   _cmb_prev_item (items, &i);
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

   //Send current line for printing
   sprintf ((char*)tuid->frame_buffer.fb, "%s", (char*)items[i].text[ln]);

   return EXIT_STAY;
}
