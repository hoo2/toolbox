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

#include <uid.h>

ui_keys_t   ui_keys;

static menu_item_t   *menu;
static int           item;
static uint8_t       menu_mask[UI_MENU_MASK_SIZE/8];  /*!< UI_MENU_MASK_SIZE bit "variable" to addressed by 8bit position mm array */

/*
 * Toolbox
 */

// Push the current menu to stack
static void _push_menu (menu_stack_t* st, menu_item_t *mn, int cur)
{
   if (st->sp >= UI_CALLMENU_SIZE)
      return;
   else
   {
      st->mstack[st->sp] = mn;
      st->cstack[st->sp] = cur;
      ++st->sp;
   }
}

// Pop the last menu from stack
static menu_item_t * _pop_menu (menu_stack_t* st, int *cur)
{
   if (st->sp > 0)
   {
      --st->sp;
      *cur = st->cstack[st->sp];
      return st->mstack[st->sp];
   }
   else
   {
      *cur = 0;
      return (void*)0;
   }
}

// Discard the stack stored menu and return
static menu_item_t * _esc_menu (menu_stack_t* st, int *cur)
{
   st->sp = 0;
   *cur = 0;
   return (void*)0;
}

static int  _menu_stack_empty (menu_stack_t* st)
{
   if (!st->sp)   return 1;
   else           return 0;
}

static uint8_t _menu_item_active (menu_item_t *item)
{
   uint8_t i;
   uint8_t  p[4]; // 8bit positions
   uint8_t  _p[4];   // 1bit position

   for (i=0 ; i<4 ; ++i)
   {
      p[i] = item->mm[i] / 8;
      _p[i] = item->mm[i] % 8;
      p[i] = (menu_mask[p[i]] & (0x01<<_p[i])) ? 1 : 0;  // Get bit
   }

   return (uint8_t)
          ( (p[MM_CTRL] || p[MM_OR])
            && p[MM_AND]
            && !p[MM_NOT]);
}



void ui_set_menu_mask (uint8_t pos)
{
   uint8_t  p, _p;

   p = pos / 8;
   _p = pos % 8;
   menu_mask[p] |= (0x01 << _p);
}

void ui_clear_menu_mask (uint8_t pos)
{
   uint8_t  p, _p;

   p = pos / 8;
   _p = pos % 8;
   menu_mask[p] &= ~(0x01 << _p);
}

void ui_menu_init (void)
{
   int i;

   // Clear
   for (i=0 ; i<UI_MENU_MASK_SIZE/8 ; ++i)
      menu_mask[i] = 0;

   menu_mask[0] &= ~0x01;
   menu_mask[UI_MENU_MASK_SIZE/8 - 1] |= 0x80;
}

/*!
 * \brief
 *    A demonized version of menu function. It creates a menu by reading
 *    the mn table.
 *
 * \param   key      User input
 * \param   mn       The menu table
 * \param   ln       The language to use.
 *
 * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 *
 * This function can create a nested menus based on the context of a table. While the function
 * returns EXIT_STAY it is still in progress. When the function is done returns EXIT_RETURN.
 * This assumes that the caller must handle with return status in order to continues call or not
 * the function.
 *
 * For example:
 *
 * ui_return_t task_a (void);
 * ui_return_t info_entrance (void);
 * ui_return_t info_a (void);
 * ui_return_t info_b (void);
 *
 * const menu_item_t  main_menu [];
 * const menu_item_t  info_menu [];
 *
 * const menu_item_t  main_menu [] =
 * {
 *    {{"MENU",            "MENU"},             UI_EMPTY},           <-- Caption
 *    {{">View Info",      ">Info anzeigen"},   UI_MENU(info_menu)}, <-- Items
 *    {{">Task a",         ">Aufgabe a"},       UI_TASK(task_a)},          "  (call task_a)
 *    {{"<BACK",           "<ZURUCK"},          UI_BACK},            <-- special back function
 *    {{0,0}, UI_EMPTY}                                              <-- Terminator
 * };
 *
 * const menu_item_t  info_menu [] =
 * {
 *    {{"Informations", "Information"},   UI_TASK(info_entrance)},   <--Caption and optional info_entrance call on entrance
 *    {{">Info a",      ">Info a"},       UI_TASK(info_a)},          <-- Items (call info_a)
 *    {{">Info b",      ">Info b"},       UI_TASK(info_b)},                "   (call info_b)
 *    {{"<BACK",        "<ZURUCK"},       UI_BACK},                        "
 *    {{0,0}, UI_EMPTY}                                              <-- Terminator
 * };
 *
 * Navigation
 * ==========================
 * UP       --    Previous item on the list (table)
 * DOWN     --    Next item on the list
 * RIGHT    --    Call selected function or submenu
 * LEFT     --    Exit the item (or the entire menu if the current menu is the first call)
 * ESC      --    Exit the entire menu
 *
 */
ui_return_t ui_menu (int key, menu_item_t *mn, Lang_en ln)
{
   static menu_stack_t  hist;
   static uint8_t ev=1, task=EXIT_RETURN;

   if (ev)   // It is the first call of every menu
   {
      item = 0;     // Prepare optional call
      task=EXIT_STAY;

      if (_menu_stack_empty(&hist)) // First menu call
         menu = mn;
      ev = 0;
   }

   if (task == EXIT_STAY)
   {  // We have call
      if (menu[item].node.task)
         task = menu[item].node.task ();
      else
         task = EXIT_RETURN;
      if (!item)  // Clear optional call
         _NEXT_ITEM (menu,item);
   }
   else
   {  // We have menu navigation
      if (key == ui_keys.UP)         _PREV_ITEM (menu,item);
      if (key == ui_keys.DOWN)       _NEXT_ITEM (menu,item);
      if (key == ui_keys.LEFT)
      {
         menu = _pop_menu(&hist, &item);
         if ( !menu )
         {
            ev = 1;
            return EXIT_RETURN;
         }
         return EXIT_STAY;
      }
      if (key == ui_keys.ESC)
      {
         menu = _esc_menu (&hist, &item);
         ev = 1;
         return EXIT_RETURN;
      }
      if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
         switch (menu[item].item_type)
         {
            case UI_NONE:
            case UI_RETURN:
               menu = _pop_menu(&hist, &item);
               if (!menu)
               {
                  ev = 1;
                  return EXIT_RETURN;
               }
               return EXIT_STAY;
            case UI_TASK_ITEM:
               return task = EXIT_STAY;
            case UI_MENU_ITEM:
               _push_menu(&hist, menu, item);
               menu = menu[item].node.menu;
               ev = 1;
               return EXIT_STAY;
         }

      if (!item)
         _ROLL_2TOP (menu, item);
      if (!menu[item].node.task)
         _ROLL_2BOTTOM (menu, item);

      //Send current frame for printing
      ui_print_caption (menu[0].text[ln]);
      ui_print_frame (menu[item].text[ln], sizeof (menu_item_t));
   }

   return EXIT_STAY;
}

/*!
 * \brief
 *    Returns the current menu.
 */
inline menu_item_t* ui_this_menu (void) {
   return (menu_item_t*)& menu[item];
}

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
   s = slocaltime(&t);
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
      if (++i>=size) {
         ev=1;
         return EXIT_RETURN;
      }
      if (!str[i]) str[i] = str[i-1];
      str[i+1] = 0;
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

