/*!
 * \file uid.c
 * \brief
 *    A small footprint uid library
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
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

#include <ui/uid.h>

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

/*!
 * \brief
 *    Creates a Value box between a min-max domain
 *
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
ui_return_t ui_valuebox (int key, text_t cap, text_t units, float up, float down, float step, int dec, float *value)
{
   static float cur, v;
   static int ev=1, speedy=0;
   char value_str[12];


   // First (each) time
   if (ev)
   {
      cur = v = *value;
      ev = 0;
      speedy = 0;
      ui_print_caption (cap);
   }

   //Navigating
   if (key == ui_keys.UP)           v += step + step*(speedy++/10);
   else if (key == ui_keys.DOWN)    v -= step + step*(speedy++/10);
   else if (key == ui_keys.ESC || key == ui_keys.LEFT)
   {  // Restore previous value
      *value = cur;
      ev = 1;
      return EXIT_RETURN;
   }
   else if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
   {  // Return the new value.
      *value = v;
      ev = 1;
      return EXIT_RETURN;
   }
   else
      speedy = 0;

   //Cycle the values
   if (v > up)     v = down;
   if (v < down)   v = up;

   //User Interface Loop
   switch (dec)
   {
      case 0:
         sprintf (value_str, "=%d %s", (int)v, (char*)units); break;
      default:
      case 1:
         sprintf (value_str, "=%.1f %s", v, (char*)units);    break;
      case 2:
         sprintf (value_str, "=%.2f %s", v, (char*)units);    break;
      case 3:
         sprintf (value_str, "=%.3f %s", v, (char*)units);    break;
   }
   ui_print_box (value_str);

   return EXIT_STAY;
}

/*!
 * \brief
 *    Creates a Time value box between a min-max domain
 *
 * \param   key      User input
 * \param   cap      The Value box caption
 * param    frm      Format string
 * \param   up       The upper value box value
 * \param   down     The lower value box value
 * \param   step     The step to use.
 * \param   value    Pointer to time value (current and returned).
 *
 * * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a Time value box between a min-max domain.
 * While the function returns EXIT_STAY it is still in progress. When the function
 * is done returns EXIT_RETURN. This assumes that the caller must handle with return
 * status in order to continues call or not the function.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the value by step
 * DOWN     --    Decrease the value by step
 * RIGHT    --    Selected and return the current time value.
 * LEFT     --    Exit with the previous time value
 * ESC      --       "        "        "
 */
ui_return_t ui_timebox (int key, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t *value)
{
   static time_t cur, t;
   static int ev=1, speedy=0;
   char value_str[UI_TIMEBOX_SIZE];
   struct tm *s;
   int i=0;

   // First (each) time
   if (ev)
   {
      cur = t = *value;
      ev = 0;
      speedy = 0;
      ui_print_caption (cap);
   }

   //Navigating
   if (key == ui_keys.UP)           t += step + step*(speedy++/10);
   else if (key == ui_keys.DOWN)    t -= step + step*(speedy++/10);

   else if (key == ui_keys.ESC || key == ui_keys.LEFT)
   {  // Restore previous value
      *value = cur;
      ev = 1;
      return EXIT_RETURN;
   }
   else if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
   {  // Return the new value.
      *value = t;
      ev = 1;
      return EXIT_RETURN;
   }
   else
      speedy = 0;

   //Cycle the values
   if (t > up)     t = down;
   if (t < down)   t = up;

   // Time Box is painting the screen
   s = sgmtime(&t);
   i = sprintf (&value_str[i], "= ");
   if (frm & UI_TIME_DD)   i += sprintf (&value_str[i], "%dd+ ", s->tm_yday);
   if (frm & UI_TIME_HH)   i += sprintf (&value_str[i], "%02d", s->tm_hour);
   if (frm & UI_TIME_MM)   i += sprintf (&value_str[i], ":%02d", s->tm_min);
   if (frm & UI_TIME_SS)   i += sprintf (&value_str[i], ":%02d\"", s->tm_sec);
   else                    i += sprintf (&value_str[i], "\'");
   ui_print_box (value_str);

   return EXIT_STAY;
}

/*!
 * \brief
 *    Creates a Textbox between a using Uppercase and digits
 *
 * \param   key      User input
 * \param   cap      The Value box caption
 * \param   str      Pointer to string
 * \param   size     The string's size
 * \param   ln       The language to use. (Currently unused).
 *
 * * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a text box.
 * While the function returns EXIT_STAY it is still in progress. When the function
 * is done returns EXIT_RETURN. This assumes that the caller must handle with return
 * status in order to continues call or not the function.
 *
 * Navigation
 * ==========================
 * UP       --    Increase the current character
 * DOWN     --    Decrease the current character
 * RIGHT    --    Deletes the last character.
 * LEFT     --    Save current character and go to next.
 * ESC      --    Returns the string "as is"
 */
ui_return_t ui_textbox (int key, text_t cap, char* str, int8_t size, Lang_en ln)
{
   static int ev=1;
   static int8_t  i=0;
   char bf[UI_TEXTBOX_SIZE];

   if (ev)
   {
      ev=i=0;
      if (!*str)
      {  // init string if needed
         str[0] = 'A';
         for (i=1 ; i<size ; ++i)
            str[i] = 0;
         i=0;
      }
      else
      {  // Go to last character
         for (i=0 ; str[i] ; ++i);
         --i;
      }
      ui_print_caption (cap);
   }

   if (key == ui_keys.UP)
      do
         str[i]++;
      while ( !isupper ((int)str[i]) &&
              !islower ((int)str[i]) &&
              !isdigit ((int)str[i]) &&
              str[i]!='-' );
   if (key == ui_keys.DOWN)
      do
         str[i]--;
      while ( !isupper ((int)str[i]) &&
              !islower ((int)str[i]) &&
              !isdigit ((int)str[i]) &&
              str[i]!='-' );

   //Navigating
   if (key == ui_keys.LEFT)
   {
      str[i] = 0;
      --i;
      if (i<0)
      {
         ev=1;
         return EXIT_RETURN;
      }
   }
   if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
   {
      if (!str[i+1])
         str[i+1] = str[i];

      ++i;
      if (i>=size)
      {
         ev=1;
         return EXIT_RETURN;
      }
   }
   if (key == ui_keys.ESC)
   {
      ev=1;
      return EXIT_RETURN;
   }

   // Paint the screen
   sprintf (bf, ":%s<", str);
   ui_print_box (bf);

   return EXIT_STAY;
}

