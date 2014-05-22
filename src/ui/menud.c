/*!
 * \file menud.c
 * \brief
 *    A small footprint demonised menu functionality
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2008-2014 Houtouridis Christos (http://www.houtouridis.net)
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

#include <ui/menud.h>

ui_keys_t   ui_keys;

static ui_menu_t     _md;
static uint8_t       menu_mask[UI_MENU_MASK_SIZE/8];  /*!< UI_MENU_MASK_SIZE bit "variable" to addressed by 8bit position mm array */

// Static functions
static void _push_menu (menu_stack_t* st, ui_menu_t* mn);
static void  _pop_menu (menu_stack_t* st, ui_menu_t* mn);
static void  _esc_menu (menu_stack_t* st, ui_menu_t* mn);
static int _menu_stack_empty (menu_stack_t* st);
static int _menu_item_active (menu_item_t *item);


// Push the current menu to stack
static void _push_menu (menu_stack_t* st, ui_menu_t* mn)
{
   if (st->sp >= UI_CALLMENU_SIZE)
      return;
   else {
      st->mstack[st->sp] = *mn;
      ++st->sp;
   }
}

// Pop the last menu from stack
static void _pop_menu (menu_stack_t* st, ui_menu_t* mn)
{
   if (st->sp > 0) {
      --st->sp;
      *mn = st->mstack[st->sp];
   }
   else
      memset ((void*)mn, 0, sizeof (ui_menu_t));
}

// Discard the stack stored menu and return
static void _esc_menu (menu_stack_t* st, ui_menu_t* mn)
{
   memset ((void*)st, 0, sizeof (menu_stack_t));
   memset ((void*)mn, 0, sizeof (ui_menu_t));
}

static int _menu_stack_empty (menu_stack_t* st)
{
   return (st->sp) ? 0:1;
}

static int _menu_item_active (menu_item_t *item)
{
   uint8_t i;
   uint8_t  p[4]; // 8bit positions
   uint8_t  _p[4];   // 1bit position

   for (i=0 ; i<4 ; ++i) {
      p[i] = item->mm[i] / 8;
      _p[i] = item->mm[i] % 8;
      p[i] = (menu_mask[p[i]] & (0x01<<_p[i])) ? 1 : 0;  // Get bit
   }

   return (uint8_t) ( (p[MM_CTRL]
                    || p[MM_OR])
                    && p[MM_AND]
                    && !p[MM_NOT]);
}


/*
 * ============================ Public Functions ============================
 */

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
 *
 * \param   key      User input
 * \param   mn       The menu table
 * \param   ln       The language to use.
 *
 * \return  ui_return_t
 *    \arg  EXIT_RETURN    :  Indicates that function returns
 *    \arg  EXIT_STAY      :  Indicates that functions has not returned
 */
ui_return_t ui_menu (int key, menu_item_t *mn, Lang_en ln)
{
   static menu_stack_t  hist;
   static uint8_t ev=1, task=EXIT_RETURN;

   if (ev) {
      // It is the first call of every menu
      _md.item = 0;     // Prepare optional call
      task=EXIT_STAY;

      if (_menu_stack_empty (&hist)) // First menu call
         _md.menu = mn;
      ev = 0;
   }

   if (task == EXIT_STAY) {
      // We have call
      if (_md.menu[_md.item].node.task)
         task = _md.menu[_md.item].node.task ();
      else
         task = EXIT_RETURN;
      if (!_md.item)  // Clear optional call
         _NEXT_ITEM (_md.menu,_md.item);
   }
   else {
      // We have menu navigation
      if (key == ui_keys.UP)         _PREV_ITEM (_md.menu,_md.item);
      if (key == ui_keys.DOWN)       _NEXT_ITEM (_md.menu,_md.item);
      if (key == ui_keys.LEFT) {
         _pop_menu (&hist, &_md);
         if ( !_md.menu ) {
            ev = 1;
            return EXIT_RETURN;
         }
         return EXIT_STAY;
      }
      if (key == ui_keys.ESC) {
         _esc_menu (&hist, &_md);
         ev = 1;
         return EXIT_RETURN;
      }
      if (key == ui_keys.RIGHT || key == ui_keys.ENTER)
         switch (_md.menu[_md.item].item_type)
         {
            case UI_NONE:
            case UI_RETURN:
               _pop_menu (&hist, &_md);
               if (!_md.menu) {
                  ev = 1;
                  return EXIT_RETURN;
               }
               return EXIT_STAY;
            case UI_TASK_ITEM:
               return task = EXIT_STAY;
            case UI_MENU_ITEM:
               _push_menu (&hist, &_md);
               _md.menu = _md.menu[_md.item].node.menu;
               ev = 1;
               return EXIT_STAY;
         }

      if (!_md.item)
         _ROLL_2TOP (_md.menu, _md.item);
      if (!_md.menu[_md.item].node.task)
         _ROLL_2BOTTOM (_md.menu, _md.item);

      //Send current frame for printing
      ui_print_caption (_md.menu[0].text[ln]);
      ui_print_frame (_md.menu[_md.item].text[ln], sizeof (menu_item_t));
   }

   return EXIT_STAY;
}

/*!
 * \brief
 *    Returns the current menu.
 */
inline menu_item_t* ui_this_menu (void) {
   return (menu_item_t*)& _md.menu[_md.item];
}
