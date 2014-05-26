/*!
 * \file textboxd.c
 * \brief
 *    A demonised text-box functionality.
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
ui_return_t textboxd (int key, text_t cap, char* str, int8_t size, Lang_en ln)
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

