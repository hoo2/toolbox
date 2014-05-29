/*!
 * \file ui.c
 * \brief
 *    A small footprint ui library
 *
 * Copyright (C) 2011 Houtouridis Christos <houtouridis.ch@gmail.com>
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
 * Date:       07/2011
 * Version:
 *
 */

#include <ui.h>

/*!
 * \brief
 *    Creates a Combo box by reading the items table
 *
 * \param   items    The Combo Box table
 * \param   cur      Current id.
 * \param   ln       The language to use.
 *
 * \return  the selected id
 *
 * This function can create a Combo box based on the context of a table.
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
int ui_combobox (combobox_item_t *items, int cur, language_en ln)
{
   int i;
   int in=0;

   // Find cur in combobox table
   for (i=1 ; items[i].text[ln]; ++i)
   {
      if (items[i].id == cur)
         break;
      if (!items[i].text[ln])
      {
         i=1;
         break;
      }
   }

   // UI loop
   ui_print_ctrl ('\f');
   ui_print_caption (items[0].text[ln]);
   ui_print_ctrl ('\n');
   for ( ; ; )
   {
      //Send current frame for printing
      ui_print_ctrl ('\r');
      ui_print_box (items[i].text[ln]);
      in = ui_getkey (1);                     //Get user choice

      switch (in)
      {
         case UP:    --i;  break;            //Navigating
         case DOWN:  ++i;  break;

         case ESC:   //Actions
         case LEFT:
            return cur;
         case RIGHT:
         case ENTER:
            return items[i].id;
            break;
         default:
            break;
      }
      // Roll numbers
      if (!i)
      {
         for (i=1 ; items[i].text[ln]; ++i);
         --i;
      }
      if (!items[i].text[ln])
         i=1;
   }
   return 0;      //On error exit
}

/*!
 * \brief
 *    Creates a Value box between a min-max domain
 *
 * \param   cap      The Value box caption
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 *
 * \return  The selected value.
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
float ui_valuebox (text_t cap, float up, float down, float step, float cur)
{
   float value = cur;
   int in=0;
   char value_str[12];

   //User Interface Loop
   ui_print_ctrl ('\f');
   ui_print_caption (cap);
   ui_print_ctrl ('\n');
   for ( ; ; )
   {
      sprintf (value_str, "\r=%10.2f", value);
      ui_print_box (value_str);
      in = ui_getkey (1);
      switch (in)
      {
         case UP:       value += step; break;
         case DOWN:     value -= step; break;
         //Actions
         case ESC:
         case LEFT:
            return cur;
         case RIGHT:
         case ENTER:
            return value;
         default:
            break;
      }
      //Cycle the values
      if (value > up)   value = down;
      if (value < down) value = up;
   }
}

/*!
 * \brief
 *    Creates a Time value box between a min-max domain
 *
 * \param   cap      The Value box caption
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 *
 * \return  The selected time value.
 *
 * This function can create a Value box between a min-max domain.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the time value by step
 * DOWN     --    Decrease the time value by step
 * RIGHT    --    Selected and return the current time value.
 * LEFT     --    Exit with the previous time value
 * ESC      --       "        "        "
 */
time_t ui_timebox (text_t cap, time_t up, time_t down, time_t step, time_t cur)
{
   time_t value = cur;
   int in=0;
   char value_str[12];
   struct tm *s;

   //User Interface Loop
   ui_print_ctrl ('\f');
   ui_print_caption (cap);
   ui_print_ctrl ('\n');
   for ( ; ; )
   {
      // Time Box is painting the screen
      s = localtime(&value);
      if (s->tm_mday)
         sprintf (value_str, "\r= %d:%02d:%02d:%02d", s->tm_mday, s->tm_hour, s->tm_min, s->tm_sec);
      else if (s->tm_hour)
         sprintf (value_str, "\r= %02d:%02d:%02d", s->tm_hour, s->tm_min, s->tm_sec);
      else
         sprintf (value_str, "\r= %02d:%02d", s->tm_min, s->tm_sec);
      ui_print_box (value_str);

      in = ui_getkey (1);
      switch (in)
      {
         case UP:       value += step; break;
         case DOWN:     value -= step; break;
         //Actions
         case ESC:
         case LEFT:
            return cur;
         case RIGHT:
         case ENTER:
            return value;
         default:
            break;
      }
      //Cycle the values
      if (value > up)   value = down;
      if (value < down) value = up;
   }
}
