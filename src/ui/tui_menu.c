/*!
 * \file tui_menu.c
 * \brief
 *    Menu functionality for RTOS environments
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
static int _menu_item_active (tui_t *tui, menu_item_t *menu);
static int _next_item (tui_t *tui, menu_item_t *menu, int *it);
static int _prev_item (tui_t *tui, menu_item_t *menu, int *it);

static void _mk_caption (tui_t *tui, menu_item_t *menu, Lang_en ln);
static void   _mk_frame (tui_t *tui, menu_item_t *menu, int frame, int item, Lang_en ln);

// Common API
extern int _tuix_clear_frame (fb_t *fb);
extern void _tuix_mk_caption (fb_t *fb, text_t cap);

/*!
 * \brief
 *    Check if the menu indexed by it is active (can be shown)
 *    or not.
 * \param  tui    Pointer to the active tuid_t structure
 * \param  menu   Pointer to current menu ITEM for ex: &menu[i]
 * \return        The active status
 *    \arg  1     The menu is active (enabled).
 *    \arg  0     The menu is not active (disabled).
 */
static int _menu_item_active (tui_t *tui, menu_item_t *menu)
{
   uint8_t i;
   uint8_t p[4];  // 8bit positions
   uint8_t _p[4]; // 1bit position

   for (i=0 ; i<4 ; ++i) {
      p[i] = menu->mm[i] / 8;
      _p[i] = menu->mm[i] % 8;
      p[i] = (tui->menu_mask[p[i]] & (0x01<<_p[i])) ? 1:0;  // Get bit
   }

   return (int)((p[MM_CTRL] || p[MM_OR]) && p[MM_AND] && !p[MM_NOT]);
}

/*!
 * \brief
 *    Finds the next active item in the menu array
 * \param  tui    Pointer to the active tuid_t structure
 * \param  menu   Pointer to current menu ARRAY for ex: main_menu
 * \param  it     Pointer to the menu index item to update ex: main_menu[*it]
 * \return        The operation's status
 *    \arg  1     The it updated successfully
 *    \arg  0     The it doesn't updated successfully (no items left).
 */
static int _next_item (tui_t *tui, menu_item_t *menu, int *it)
{
   int st = *it;
   do {
      if ( !menu[++*it].node.task )
         *it=1;
      if (st == *it)
         return 0;
   } while (!_menu_item_active (tui, &menu[*it]));
   return 1;
}

/*!
 * \brief
 *    Finds the previous active item in the menu array
 * \param  tui    Pointer to the active tuid_t structure
 * \param  menu   Pointer to current menu ARRAY for ex: main_menu
 * \param  it     Pointer to the menu index item to update ex: main_menu[*it]
 * \return        The operation's status
 *    \arg  1     The it updated successfully
 *    \arg  0     The it doesn't updated successfully (no items left).
 */
static int _prev_item (tui_t *tui, menu_item_t *menu, int *it)
{
   int st = *it;
   do {
      if (!--*it) {
         for (*it=1 ; menu[*it].node.task ; ++*it)
            ;
         --*it;
      }
      if (st == *it)
         return 0;
   } while (!_menu_item_active (tui, &menu[*it]));
   return 1;
}

/*!
 * \brief
 *    Paints the Caption line in the frame buffer
 * \param  tui    Pointer to the active tui_t structure
 * \param  menu   Pointer to current menu ARRAY
 * \param  ln     The language to use
 * \return none
 */
static void _mk_caption (tui_t *tui, menu_item_t *menu, Lang_en ln)
{
   _tuix_mk_caption (&tui->frame_buffer, menu[0].text[ln]);
}

/*!
 * \brief
 *    Paints the frame in the frame buffer
 * \param  tui    Pointer to the active tui_t structure
 * \param  menu   Pointer to current menu ARRAY
 * \param  ln     The language to use
 * \return none
 */
static void   _mk_frame (tui_t *tui, menu_item_t *menu, int frame, int item, Lang_en ln)
{
   #define _LINE(_l)    (tui->frame_buffer.c*(_l))
   int line, offset;
   int start;

   // CLear frame
   if (_tuix_clear_frame (&tui->frame_buffer))
      return;
   start = frame;
   for (line=1 ; line < tui->frame_buffer.l ; ++line) {
      offset=0;
      if (frame == item) {
         if (menu[frame].item_type == UI_RETURN)
            offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(line)], "<%s", (char*)menu[frame].text[ln]);
         else
            offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(line)], ">%s", (char*)menu[frame].text[ln]);
      }
      else
         offset = sprintf ((char*)&tui->frame_buffer.fb[_LINE(line)], "%s", (char*)menu[frame].text[ln]);
      // discard null termination inside frame buffer
      tui->frame_buffer.fb[_LINE(line)+offset] = ' ';

      // Escape if no items left
      if ( !_next_item (tui, menu, &frame) )
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
void tui_menu_set_mask (tui_t *tui, uint8_t pos)
{
   uint8_t  p, _p;

   p = pos / 8;
   _p = pos % 8;
   tui->menu_mask[p] |= (0x01 << _p);
}

/*!
 * \brief
 *    Clear the menu mask bit in the position pos.
 * \param  tuid   Pointer to the active tuid_t struct
 * \param  pos    The bit position inside the manu_mask to clear.
 * \return none
 */
void tui_menu_clear_mask (tui_t *tui, uint8_t pos)
{
   uint8_t  p, _p;

   p = pos / 8;
   _p = pos % 8;
   tui->menu_mask[p] &= ~(0x01 << _p);
}

/*!
 * \brief
 *    Initialise the menu mask array
 * \param  tuid   Pointer to the active tuid_t struct
 * \return none
 */
void tui_menu_init (tui_t *tui)
{
   int i;

   // Clear
   for (i=0 ; i<UI_MENU_MASK_SIZE/8 ; ++i)
      tui->menu_mask[i] = 0;

   tui->menu_mask[0] &= ~0x01;
   tui->menu_mask[UI_MENU_MASK_SIZE/8 - 1] |= 0x80;
}

/*!
 * \brief
 *    Return the menu address with id \b id
 * \param   mn    The top menu to seek
 * \param   id    The search id
 * \return  the menu address, or NULL
 */
menu_item_t* tui_menu_id2idx (menu_item_t *mn, menu_id_t id)
{
   int i;
   menu_item_t *r;

   for (i=1 ; mn[i].node.task != 0 ; ++i) {
      if (mn[i].id == id)
         return (menu_item_t*)&mn[i];
      else if (mn[i].item_type == UI_MENU_ITEM) {
         if ( (r = tui_menu_id2idx (mn[i].node.menu, id)) != NULL)
            return r;
      }
   }
   return NULL;
}

/*!
 * \brief
 *    Return the id of the menu \a mn
 */
inline menu_id_t tui_menud_idx2id (menu_item_t *mn) {
   return mn->id;
}

/*!
 * \brief
 *    A menu function for RTOS environments. It creates a menu by reading
 *    the mn table.
 *
 * This function can create a nested menus based on the context of a table.
 *
 * \param   tui      Pointer to the active tui_t structure
 * \param   mn       The menu table
 * \param   ln       The language to use.
 *
 * \return  none
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
 * const menu_item_t  main_menu [];
 * const menu_item_t  info_menu [];
 * const menu_item_t  main_menu [] =
 * {
 *    {{"MENU",           "MENU"},            UI_EMPTY,            UI_MM_EN},                                        // <-- Caption
 *    {{"Task a",         "Aufgabe a"},       UI_TASK(task_a),     UI_MM_EN},                                        // <-- Items  (call task_a)
 *    {{"Task b",         "Aufgabe b"},       UI_TASK(task_b),     {MM_TASK_B, UI_IT_DIS, UI_IT_EN, UI_IT_DIS}},     //       "    (call task_b)
 *    {{"View Info",      "Info anzeigen"},   UI_MENU(info_menu),  {MM_INFOMENU, UI_IT_DIS, MM_TASK_B, UI_IT_DIS}},  // <-- Sub-menu
 *    {{"BACK",           "ZURUCK"},          UI_BACK,             UI_MM_EN},                                        // <-- special back function
 *    {{0,0}, UI_EMPTY,                                            UI_MM_EN}                                         // <-- Terminator
 * };
 *
 * const menu_item_t  info_menu [] =
 * {
 *    {{"Informations", "Information"},   UI_TASK(info_entrance),    UI_MM_EN},  // <-- Caption and optional info_entrance
 *                                                                               //     call on entrance
 *    {{"Info a",       "Info a"},        UI_TASK(info_a),           UI_MM_EN},  // <-- Items  (call task_a)
 *    {{"Info b",       "Info b"},        UI_TASK(info_b),           UI_MM_EN},  //       "    (call task_b)
 *    {{"BACK",         "ZURUCK"},        UI_BACK,                   UI_MM_EN},  // <-- special back function
 *    {{0,0}, UI_EMPTY,                                              UI_MM_EN}   // <-- Terminator
 * };
 * //
 * // The task_b is enabled if we call       tui_menu_set_mask (MM_TASK_B);
 * // The info_menu is enabled if we call    tui_menu_set_mask (MM_TASK_B); and tui_menu_set_mask (MM_INFOMENU);
 * // To disable task_a we call              tui_menu_clear_mask (MM_TASK_B)
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
void tui_menu (tui_t *tui, menu_item_t *menu, Lang_en ln)
{
   int i, frm, vi, vfrm;
   int in=0;
   static uint8_t esc=0, calls=0;
   /*!<
    * \note
    * DON'T PANIC! We use these variables as static, (yes I know what I'm doing)
    * to count recursive calls. We increase <calls> at every call and decrease it
    * at every return. This way we know how to deal with ESC functionality.
    */

   if (menu[0].node.task)  // Optional function call
      menu[0].node.task();

   _mk_caption (tui, menu, ln);
   i = vi = frm = vfrm = 1;
   while ( menu[i].node.task ) {
      //Send current frame for printing
      _mk_frame (tui, menu, vfrm, i, ln);

      //Get user choice
      in = (tui->get_key) ? tui->get_key (0) : -1;

      //Navigating
      if (in == tui->keys.UP)          vi -= _prev_item (tui, menu, &i);
      else if (in == tui->keys.DOWN)   vi += _next_item (tui, menu, &i);
      //Actions
      else if (in == tui->keys.ESC) {
         calls -= (calls) ? 1:0;
         esc=1;
         return;
      }
      else if (in == tui->keys.LEFT) {
         calls -= (calls) ? 1:0;
         return;
      }
      else if (in == tui->keys.RIGHT || in == tui->keys.ENTER) {
         switch (menu[i].item_type)
         {
            case UI_NONE:
            case UI_RETURN:
               calls -= (calls) ? 1:0;
               return;
            case UI_TASK_ITEM:
               menu[i].node.task ();
               break;
            case UI_MENU_ITEM:
               ++calls;
               tui_menu (tui, menu[i].node.menu, ln);
               break;
         }
      }
      // Roll frame
      if (vi < vfrm) {
         vfrm = vi;
         frm = i;
      }
      else if (vi - vfrm>= tui->frame_buffer.l - 1) {
         ++vfrm;
         _next_item (tui, menu, &frm);
      }
      // Escape flag unrolls all sub-menus.
      if (esc) {
         if (calls)  --calls;
         else        esc = 0;
         return;
      }
   }
}
