/*!
 * \file combobox.c
 * \brief
 *    A plain and demonised combo-box functionality.
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
#include <ui/combobox.h>

/*!
 * \brief
 *    Creates a Combo box by reading the items table
 *
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
 *   {{"ITEMS",      "ANTIKEIMENA"},   0},   <-- Caption with whatever id
 *   {{"[item 1]",   "[antik 1]"},     1},   <-- item and item's id
 *   {{"[item 2]",   "[antik 2]"},     2},         "        "
 *   {{"[item 3]",   "[antik 3]"},     3},         "        "
 *   {{0,0},0}                               <-- Terminator
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
ui_return_t ui_combobox (int key, combobox_item_t *items, int *id, Lang_en ln)
{
   static uint8_t ev=1;
   static int i, cur;

   if (ev)   // It is the first call of combobox
   {
      // Find cur in combobox table
      for (i=1 ; items[i].text[ln]; ++i)
      {
         if (items[i].id == *id)
            break;
         if (!items[i].text[ln])
         {
            i=1;
            break;
         }
      }
      cur = i;
      ev = 0;
   }

   // UI loop
   if (key == ui_keys.UP)     --i;  //Navigating
   if (key == ui_keys.DOWN)   ++i;
   if (key == ui_keys.ESC || key == ui_keys.LEFT)
   {  // Restore previous value
      *id = items[cur].id;
      ev = 1;
      return EXIT_RETURN;
   }
   if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
   {  // Apply the new value
      *id = items[i].id;
      ev = 1;
      return EXIT_RETURN;
   }

   // Roll numbers
   if (!i)
   {
      for (i=1 ; items[i].text[ln]; ++i);
      --i;
   }
   if (!items[i].text[ln])
      i=1;

   // Printing
   ui_print_caption (items[0].text[ln]);
   ui_print_box (items[i].text[ln]);

   return EXIT_STAY;
}
