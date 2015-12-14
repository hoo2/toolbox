/*!
 * \file tui_combobox.c
 * \brief
 *    Combo-box functionality for RTOS environments.
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
static void _mk_caption (tui_t *tui, combobox_item_t *items, Lang_en ln);
static void   _mk_frame (tui_t *tui, combobox_item_t *items, int frame, int item, Lang_en ln);

// Common API
extern void _cmb_next_item (combobox_item_t *items, int *it);
extern void _cmb_prev_item (combobox_item_t *items, int *it);
extern void _cmb_frame_lines (fb_t *fb, combobox_item_t *items, int frame, int item, Lang_en ln);

extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);



/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tui    Pointer to the active tui_t structure
 * \param  items  Pointer to the active combo-box
 * \param  ln     The language to use
 * \return none
 */
void _mk_caption (tui_t *tui, combobox_item_t *items, Lang_en ln)
{
   _tuix_mk_caption (&tui->frame_buffer, items[0].text[ln]);
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tui    Pointer to the active tui_t structure
 * \param  items  Pointer to the active combo-box
 * \param  frame  The frame buffer's start position
 * \param  item   The frame buffer's active line
 * \param  ln     The language to use
 * \return none
 */
void _mk_frame (tui_t *tui, combobox_item_t *items, int frame, int item, Lang_en ln)
{
   // CLear frame
   if (_tuix_clear_frame (&tui->frame_buffer))
      return;
   // Print each line
   _cmb_frame_lines (&tui->frame_buffer, items, frame, item, ln);
}

/*!
 * \brief
 *    Creates a Combo box by reading the items table
 *
 * \param   tui      Pointer to the active tuid_t structure
 * \param   items    The Combo Box table
 * \param   ln       The language to use.
 *
 * * \return  On ENTER The selected combo box id
 *            On ESC/LEFT the previous combo box id
 *
 * This function can create a Combo box based on the context of a table.
 *
 * For example:
 * const combobox_item_t cb [] =
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
int tui_combobox (tui_t *tui, combobox_item_t *items, int cur, Lang_en ln)
{
   int i, frm, vi, vfrm;
   int in=0;

   // Find cur in combobox table
   for (i=1 ; 1 ; ++i) {
      if (!items[i].text[ln]) {
         i=1;
         break;
      }
      if (items[i].id == cur)
         break;
   }
   // Update counters
   cur = vi = vfrm = frm = i;

   // Print caption
   _mk_caption (tui, items, ln);
   while (1) {
      // Printing frame
      _mk_frame (tui, items, frm, i, ln);
      //Get user choice
      in = (tui->get_key) ? tui->get_key (0) : -1;

      // UI loop - Navigating
      if (in == tui->keys.UP) {
         _cmb_prev_item (items, &i);
         --vi;
      }
      if (in == tui->keys.DOWN) {
         _cmb_next_item (items, &i);
         ++vi;
      }
      if (in == tui->keys.ESC || in == tui->keys.LEFT)
         return cur;         // Restore previous value
      if (in == tui->keys.RIGHT || in == tui->keys.ENTER)
         return items[i].id;  // Apply the new value

      // Roll frame
      if (vi < vfrm) {
         vfrm = vi;
         frm = i;
      }
      else if (vi - vfrm>= tui->frame_buffer.l - 1) {
         ++vfrm;
         _cmb_next_item (items, &frm);
      }
   }
}
