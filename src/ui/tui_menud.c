/*!
 * \file tui_menud.c
 * \brief
 *    A demonised menu functionality.
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
static void _push_menu (menud_stack_t* st, ui_menud_t* mn);
static void  _pop_menu (menud_stack_t* st, ui_menud_t* mn);
static void  _esc_menu (menud_stack_t* st, ui_menud_t* mn);

static int _menu_stack_empty (tuid_t *tuid);
static int _menu_item_active (tuid_t *tuid, int it);

static int _next_item (tuid_t *tuid, int *it);
static int _prev_item (tuid_t *tuid, int *it);

static void _mk_caption (tuid_t *tuid, Lang_en ln);
static void   _mk_frame (tuid_t *tuid, Lang_en ln);

// Common API
extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);

/*!
 * \brief
 *    Push the current menu to call menu stack
 * \param  st     Pointer to the stack to use
 * \param  mn     Pointer to the menu instant to push.
 * \return none
 */
__Os__ static void _push_menu (menud_stack_t* st, ui_menud_t* mn)
{
   if (st->sp >= UI_CALLMENU_SIZE)
      return;
   else {
      st->mstack[st->sp] = *mn;
      ++st->sp;
   }
}

/*!
 * \brief
 *    Pop from the call menu stack
 * \param  st     Pointer to the stack to use
 * \param  mn     Pointer to the menu instant.
 * \return none
 */
__Os__ static void _pop_menu (menud_stack_t* st, ui_menud_t* mn)
{
   if (st->sp > 0) {
      --st->sp;
      *mn = st->mstack[st->sp];
   }
   else
      memset ((void*)mn, 0, sizeof (ui_menud_t));
}

/*!
 * \brief
 *    Discard all the stack stored menu and return.
 *    Escape from all sub-menus.
 * \param  st     Pointer to the stack to use
 * \param  mn     Pointer to the poped menu instant.
 * \return none
 */
__O3__ static void _esc_menu (menud_stack_t* st, ui_menud_t* mn)
{
   memset ((void*)st, 0, sizeof (menud_stack_t));
   memset ((void*)mn, 0, sizeof (ui_menud_t));
}

/*!
 * \brief
 *    Check if call menu stack is empty
 * \param  tuid   Pointer to the active tuid_t struct
 * \return        The stack emptiness status
 *    \arg  1     The stack is empty.
 *    \arg  0     The stack is NOT empty.
 */
__O3__ static int _menu_stack_empty (tuid_t *tuid)
{
   return (tuid->hist.sp) ? 0:1;
}

/*!
 * \brief
 *    Check if the menu indexed by it is active (can be shown)
 *    or not.
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  it     The menu indexed item to check ex: tuid->menu_data.menu[it]
 * \return        The active status
 *    \arg  1     The menu is active (enabled).
 *    \arg  0     The menu is not active (disabled).
 */
__Os__ static int _menu_item_active (tuid_t *tuid, int it)
{
   uint8_t i;
   uint8_t  p[4]; // 8bit positions
   uint8_t  _p[4];   // 1bit position

   for (i=0 ; i<4 ; ++i) {
      p[i] = tuid->menu_data.menu[it].mm[i] / 8;
      _p[i] = tuid->menu_data.menu[it].mm[i] % 8;
      p[i] = (tuid->menu_mask[p[i]] & (0x01<<_p[i])) ? 1 : 0;  // Get bit
   }

   return (uint8_t) ( (p[MM_CTRL]
                    || p[MM_OR])
                    && p[MM_AND]
                    && !p[MM_NOT]);
}

/*!
 * \brief
 *    Finds the next active item in the menu array
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  it     The menu indexer item to update ex: tuid->menu_data.menu[*it]
 * \return        The operation's status
 *    \arg  1     The it updated successfully
 *    \arg  0     The it doesn't updated successfully (no items left).
 */
__Os__ static int _next_item (tuid_t *tuid, int *it)
{
   int st = *it;
   do {
      if (! tuid->menu_data.menu[++*it].node.task)
         *it=1;
      if (st == *it)
         return 0;
   } while (!_menu_item_active (tuid, *it));
   return 1;
}

/*!
 * \brief
 *    Finds the previous active item in the menu array
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  it     The menu indexer item to update ex: tuid->menu_data.menu[*it]
 * \return        The operation's status
 *    \arg  1     The it updated successfully
 *    \arg  0     The it doesn't updated successfully (no items left).
 */
__Os__ static int _prev_item (tuid_t *tuid, int *it)
{
   int st = *it;
   do {
      if (!--*it) {
         for (*it=1 ; tuid->menu_data.menu[*it].node.task ; ++*it)
            ;
         --*it;
      }
      if (st == *it)
         return 0;
   } while (!_menu_item_active (tuid, *it));
   return 1;
}

/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  ln     The language to use
 * \return none
 */
__O3__ static void _mk_caption (tuid_t *tuid, Lang_en ln)
{
   _tuix_mk_caption (&tuid->frame_buffer, tuid->menu_data.menu[0].text[ln]);
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  ln     The language to use
 * \return none
 */
__Os__ static void _mk_frame (tuid_t *tuid, Lang_en ln)
{
   #define _LINE(_l)    (tuid->frame_buffer.c*(_l))
   int line, offset;
   int start, frame;

   // CLear frame
   if (_tuix_clear_frame (&tuid->frame_buffer))
      return;
   // Print each line
   start = frame = tuid->menu_data.mn_frm;
   for (line=1 ; line<tuid->frame_buffer.l ; ++line) {
      offset=0;
      if (frame == tuid->menu_data.mn_it) {
         if (tuid->menu_data.menu[frame].item_type == UI_RETURN)
            offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(line)], "<%s", (char*)tuid->menu_data.menu[frame].text[ln]);
         else
            offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(line)], ">%s", (char*)tuid->menu_data.menu[frame].text[ln]);
      }
      else
         offset = sprintf ((char*)&tuid->frame_buffer.fb[_LINE(line)], "%s", (char*)tuid->menu_data.menu[frame].text[ln]);

      // discard null termination inside frame buffer body
      tuid->frame_buffer.fb[_LINE(line)+offset] = ' ';
      // Keep null termination at end of each line
      tuid->frame_buffer.fb[_LINE(line)+tuid->frame_buffer.c-1] = 0;

      // Escape if no items left
      if ( !_next_item (tuid, &frame) )
         break;
      if (frame == start)
         break;
   }
   #undef _LINE
}

/*
 * ============================ Public Functions ============================
 */

/*!
 * \brief
 *    Set the menu mask bit in the position pos.
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  pos    The bit position inside the manu_mask to set.
 * \return none
 */
__O3__ void tui_menud_set_mask (tuid_t *tuid, uint8_t pos)
{
   uint8_t  p, _p;

   p = pos / 8;
   _p = pos % 8;
   tuid->menu_mask[p] |= (0x01 << _p);
}

/*!
 * \brief
 *    Clear the menu mask bit in the position pos.
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  pos    The bit position inside the manu_mask to clear.
 * \return none
 */
__O3__ void tui_menud_clear_mask (tuid_t *tuid, uint8_t pos)
{
   uint8_t  p, _p;

   p = pos / 8;
   _p = pos % 8;
   tuid->menu_mask[p] &= ~(0x01 << _p);
}

/*!
 * \brief
 *    Initialise the menu mask array
 * \param  tuid   Pointer to the active tuid_t struct
 * \return none
 */
__Os__ void tui_menud_init (tuid_t *tuid)
{
   int i;

   // Clear
   for (i=0 ; i<UI_MENU_MASK_SIZE/8 ; ++i)
      tuid->menu_mask[i] = 0;

   tuid->menu_mask[0] &= ~0x01;
   tuid->menu_mask[UI_MENU_MASK_SIZE/8 - 1] |= 0x80;

   tuid->menu_data.ev=1;
}

/*!
 * \brief
 *    Returns the current menu.
 * \param  tuid   Pointer to the active tuid_t struct
 * \return        Pointer to the current menu item.
 */
inline menud_item_t* tui_menud_this (tuid_t *tuid) {
   return (menud_item_t*)& tuid->menu_data.menu[tuid->menu_data.mn_it];
}

/*!
 * \brief
 *    Return the menu address with id \b id
 * \param   mn    The top menu to seek
 * \param   id    The search id
 * \return  the menu address, or NULL
 */
__O3__ menud_item_t* tui_menud_id2idx (menud_item_t *mn, menu_id_t id)
{
   int i;
   menud_item_t *r;

   for (i=1 ; mn[i].node.task != 0 ; ++i) {
      if (mn[i].id == id)
         return (menud_item_t*)&mn[i];
      else if (mn[i].item_type == UI_MENU_ITEM) {
         if ( (r = tui_menud_id2idx (mn[i].node.menu, id)) != NULL)
            return r;
      }
   }
   return NULL;
}

/*!
 * \brief
 *    Return the id of the menu \a mn
 */
inline menu_id_t tui_menud_idx2id (menud_item_t *mn) {
   return mn->id;
}

/*!
 * \brief
 *    A demonised version of menu function. It creates a menu by reading
 *    the mn table.
 *
 * This function can create a nested menus based on the context of a table. While the function
 * returns EXIT_STAY it is still in progress. When the function is done returns EXIT_RETURN.
 * This assumes that the caller must handle with return status in order to continues call or not
 * the function.
 *
 * \param   tuid     Pointer to the active tuid_t structure
 * \param   key      User input
 * \param   mn       The menu table
 * \param   ln       The language to use.
 *
 * \return  ui_return_t
 *    \arg  EXIT_RETURN    Indicates that function returns
 *    \arg  EXIT_STAY      Indicates that functions has not returned
 *
 *
 * For example:
 *
 * #define  MM_INFOMENU          (1)
 * #define  MM_TASK_B            (2)
 *
 * ui_return_t task_a (void) {return EXIT_RETURN;}
 * ui_return_t task_b (void) {return EXIT_RETURN;}
 * ui_return_t info_entrance (void) {return EXIT_RETURN;}
 * ui_return_t info_a (void) {return EXIT_RETURN;}
 * ui_return_t info_b (void) {return EXIT_RETURN;}
 *
 * const menud_item_t  main_menu [];
 * const menud_item_t  info_menu [];
 * const menud_item_t  main_menu [] =
 * {
 *    {{"MENU",           "MENU"},            UI_EMPTY,            UI_MM_EN},                                        // <-- Caption
 *    {{"Task a",         "Aufgabe a"},       UI_TASK(task_a),     UI_MM_EN},                                        // <-- Items  (call task_a)
 *    {{"Task b",         "Aufgabe b"},       UI_TASK(task_b),     {MM_TASK_B, UI_IT_DIS, UI_IT_EN, UI_IT_DIS}},     //       "    (call task_b)
 *    {{"View Info",      "Info anzeigen"},   UI_MENU(info_menu),  {MM_INFOMENU, UI_IT_DIS, MM_TASK_B, UI_IT_DIS}},  // <-- Sub-menu
 *    {{"BACK",           "ZURUCK"},          UI_BACK,             UI_MM_EN},                                        // <-- special back function
 *    {{0,0}, UI_EMPTY,                                            UI_MM_EN}                                         // <-- Terminator
 * };
 *
 * const menud_item_t  info_menu [] =
 * {
 *    {{"Informations", "Information"},   UI_TASK(info_entrance),    UI_MM_EN},  // <-- Caption and optional info_entrance
 *                                                                               //     call on entrance
 *    {{"Info a",       "Info a"},        UI_TASK(info_a),           UI_MM_EN},  // <-- Items  (call task_a)
 *    {{"Info b",       "Info b"},        UI_TASK(info_b),           UI_MM_EN},  //       "    (call task_b)
 *    {{"BACK",         "ZURUCK"},        UI_BACK,                   UI_MM_EN},  // <-- special back function
 *    {{0,0}, UI_EMPTY,                                              UI_MM_EN}   // <-- Terminator
 * };
 * //
 * // The task_b is enabled if we call       tui_menud_set_mask (MM_TASK_B);
 * // The info_menu is enabled if we call    tui_menud_set_mask (MM_TASK_B); and tui_menud_set_mask (MM_INFOMENU);
 * // To disable task_a we call              tui_menud_clear_mask (MM_TASK_B)
 * //
 *
 * Navigation
 * ==========================
 * UP       --    Previous item on the list (table)
 * DOWN     --    Next item on the list
 * RIGHT    --    Call selected function or sub-menu
 * LEFT     --    Exit the item (or the entire menu if the current menu is the first call)
 * ESC      --    Exit the entire menu
 *
 */
__Os__ ui_return_t tui_menud (tuid_t *tuid, int key, menud_item_t *mn, Lang_en ln)
{
   if (tuid->menu_data.ev) {
      // It is the first call of every menu
      tuid->menu_data.mn_it = 0;
      tuid->menu_data.mn_frm = 0;
      tuid->menu_data.fb_it = 0;
      tuid->menu_data.fb_frm = 0;
      tuid->menu_data.task=EXIT_STAY;   // Prepare optional call

      if (_menu_stack_empty (tuid)) // First menu call
         tuid->menu_data.menu = mn;
      tuid->menu_data. ev = 0;
   }

   if (tuid->menu_data.task == EXIT_STAY) {
      // We have call
      if (tuid->menu_data.menu[tuid->menu_data.mn_it].node.task)
         tuid->menu_data.task = tuid->menu_data.menu[tuid->menu_data.mn_it].node.task ();
      else
         tuid->menu_data.task = EXIT_RETURN;
      if (!tuid->menu_data.mn_it) {
         // Clear optional call and init items
         _next_item (tuid, &tuid->menu_data.mn_it);
         _next_item (tuid, &tuid->menu_data.mn_frm);
         tuid->menu_data.fb_frm = tuid->menu_data.fb_it = 1;
      }
   }
   else {
      // We have menu navigation
      if (key == tuid->keys.UP)
         tuid->menu_data.fb_it -= _prev_item (tuid, &tuid->menu_data.mn_it);
      if (key == tuid->keys.DOWN)
         tuid->menu_data.fb_it += _next_item (tuid, &tuid->menu_data.mn_it);

      if (key == tuid->keys.LEFT) {
         _pop_menu (&tuid->hist, &tuid->menu_data);
         if ( !tuid->menu_data.menu ) {
            tuid->menu_data.ev = 1;
            return EXIT_RETURN;
         }
         return EXIT_STAY;
      }
      if (key == tuid->keys.ESC) {
         _esc_menu (&tuid->hist, &tuid->menu_data);
         tuid->menu_data.ev = 1;
         return EXIT_RETURN;
      }
      if (key == tuid->keys.RIGHT || key == tuid->keys.ENTER)
         switch (tuid->menu_data.menu[tuid->menu_data.mn_it].item_type)
         {
            case UI_NONE:
            case UI_RETURN:
               _pop_menu (&tuid->hist, &tuid->menu_data);
               if (!tuid->menu_data.menu) {
                  tuid->menu_data.ev = 1;
                  return EXIT_RETURN;
               }
               return EXIT_STAY;
            case UI_TASK_ITEM:
               return tuid->menu_data.task = EXIT_STAY;
            case UI_MENU_ITEM:
               _push_menu (&tuid->hist, &tuid->menu_data);
               tuid->menu_data.menu = tuid->menu_data.menu[tuid->menu_data.mn_it].node.menu;
               tuid->menu_data.ev = 1;
               return EXIT_STAY;
         }

      // Roll frame
      if (tuid->menu_data.fb_it < tuid->menu_data.fb_frm) {
         tuid->menu_data.fb_frm = tuid->menu_data.fb_it;
         tuid->menu_data.mn_frm = tuid->menu_data.mn_it;
      }
      if (tuid->menu_data.fb_it - tuid->menu_data.fb_frm >= tuid->frame_buffer.l - 1) {
         ++tuid->menu_data.fb_frm;
         _next_item (tuid, &tuid->menu_data.mn_frm);
      }
      //Send current window for printing
      _mk_caption (tuid, ln);
      _mk_frame (tuid, ln);
   }
   return EXIT_STAY;
}

__Os__ ui_return_t tui_line_menud (tuid_t *tuid, int key, menud_item_t *mn, Lang_en ln)
{
   if (tuid->menu_data.ev) {
      // It is the first call of every menu
      tuid->menu_data.mn_it = 0;
      tuid->menu_data.mn_frm = 0;
      tuid->menu_data.fb_it = 0;
      tuid->menu_data.fb_frm = 0;
      tuid->menu_data.task=EXIT_STAY;   // Prepare optional call

      if (_menu_stack_empty (tuid)) // First menu call
         tuid->menu_data.menu = mn;
      tuid->menu_data.ev = 0;
   }

   if (tuid->menu_data.task == EXIT_STAY) {
      // We have call
      if (tuid->menu_data.menu[tuid->menu_data.mn_it].node.task)
         tuid->menu_data.task = tuid->menu_data.menu[tuid->menu_data.mn_it].node.task ();
      else
         tuid->menu_data.task = EXIT_RETURN;
      if (!tuid->menu_data.mn_it) {
         // Clear optional call and init items
         _next_item (tuid, &tuid->menu_data.mn_it);
         _next_item (tuid, &tuid->menu_data.mn_frm);
         tuid->menu_data.fb_frm = tuid->menu_data.fb_it = 1;
      }
   }
   else {
      // We have menu navigation
      if (key == tuid->keys.UP)
         tuid->menu_data.fb_it -= _prev_item (tuid, &tuid->menu_data.mn_it);
      if (key == tuid->keys.DOWN)
         tuid->menu_data.fb_it += _next_item (tuid, &tuid->menu_data.mn_it);

      if (key == tuid->keys.LEFT) {
         _pop_menu (&tuid->hist, &tuid->menu_data);
         if ( !tuid->menu_data.menu ) {
            tuid->menu_data.ev = 1;
            return EXIT_RETURN;
         }
         return EXIT_STAY;
      }
      if (key == tuid->keys.ESC) {
         _esc_menu (&tuid->hist, &tuid->menu_data);
         tuid->menu_data.ev = 1;
         return EXIT_RETURN;
      }
      if (key == tuid->keys.RIGHT || key == tuid->keys.ENTER)
         switch (tuid->menu_data.menu[tuid->menu_data.mn_it].item_type)
         {
            case UI_NONE:
            case UI_RETURN:
               _pop_menu (&tuid->hist, &tuid->menu_data);
               if (!tuid->menu_data.menu) {
                  tuid->menu_data.ev = 1;
                  return EXIT_RETURN;
               }
               return EXIT_STAY;
            case UI_TASK_ITEM:
               return tuid->menu_data.task = EXIT_STAY;
            case UI_MENU_ITEM:
               _push_menu (&tuid->hist, &tuid->menu_data);
               tuid->menu_data.menu = tuid->menu_data.menu[tuid->menu_data.mn_it].node.menu;
               tuid->menu_data.ev = 1;
               return EXIT_STAY;
         }

      // Roll frame
      if (tuid->menu_data.fb_it < tuid->menu_data.fb_frm) {
         tuid->menu_data.fb_frm = tuid->menu_data.fb_it;
         tuid->menu_data.mn_frm = tuid->menu_data.mn_it;
      }
      if (tuid->menu_data.fb_it - tuid->menu_data.fb_frm >= tuid->frame_buffer.l - 1) {
         ++tuid->menu_data.fb_frm;
         _next_item (tuid, &tuid->menu_data.mn_frm);
      }
      //Send current line for printing
      sprintf ((char*)tuid->frame_buffer.fb, "%s", (char*)tuid->menu_data.menu[tuid->menu_data.mn_it].text[ln]);
   }
   return EXIT_STAY;
}

