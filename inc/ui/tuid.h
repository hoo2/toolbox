/*!
 * \file tuid.h
 * \brief
 *    A small footprint ui library
 *
 * Copyright (C) 2010-2014 Houtouridis Christos <houtouridis.ch@gmail.com>
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

#ifndef  __tuid_h__
#define  __tuid_h__

#ifdef   __cplusplus
extern "C" {
#endif

#include <ui/tuix.h>
#include <ui/tui_dfns.h>

/*
 * ================= Menud types ===================
 */

typedef ui_return_t (*taskd_ft) (void);      /*!< Menu function Task */
typedef struct _menud_item menud_item_t;

/*!
 * Node union structure for demonised menu
 */
typedef union _noded
{
   taskd_ft       task;    /*!< Pointer to a task function */
   menud_item_t   *menu;   /*!< Pointer to a submenu via the menud_item_t */
}noded_t;

/*!
 * Structure for demonised menu items.
 */
typedef struct _menud_item
{
   menu_id_t      id;
   text_t         text[UI_NUM_OF_LANGUAGES];    /*!< Pointer to caption/frame strings */
   noded_t        node;                         /*!< Pointer to nested menu or the task function to call */
   menu_item_type_en
                  item_type;                    /*!< Menu item type enumerator */
   mm_item_t      mm[4];                        /*!<
                                                 *  Each member holds the bit position in the Menu_mask variable
                                                 *  of the EN/DIS flag for the menu item.
                                                 */
}menud_item_t;

/*!
 * Structure holding the data each menu function needs
 * to push and pop.
 */
typedef struct
{
   menud_item_t*  menu;       /*!< The active menu */
   int            mn_it;      /*!< Custom pointer to active item in menu_item_t array */
   int            mn_frm;     /*!< Custom pointer to first frame item in menu_item_t array */
   int            fb_it;      /*!< Custom pointer to active item in frame buffer array */
   int            fb_frm;     /*!< Custom pointer to first frame item in frame buffer array */
   uint8_t        ev;
   uint8_t        task;
}ui_menud_t;

/*!
 * Structure for a ui_menu_t stack
 */
typedef volatile struct
{
   ui_menud_t  mstack[UI_CALLMENU_SIZE];     /*!< The stack array */
   uint8_t     sp;                           /*!< The stack pointer */
}menud_stack_t;

/*
 * ============ Text UI user type ============
 */

/*!
 * demonised tui type
 */
typedef struct
{
   ui_keys_t      keys;             /*!< Key asignements */
   ui_menud_t     menu_data;        /*!< Current/active menu */
   fb_t           frame_buffer;     /*!< Frame buffer info */
   menud_stack_t  hist;             /*!< Call menu stack */
   uint8_t        menu_mask[UI_MENU_MASK_SIZE/8];
                                    /*!< Variable to addressed by 8bit position mm array */
}tuid_t;



/*
 * =============== Exported API ===================
 */

/*
 * Link and Glue functions
 */
void tuid_link_framebuffer (tuid_t *tuid, uint8_t *fb);

/*
 * Set functions
 */
void   tuid_set_fb_lines (tuid_t *tuid, int l);
void tuid_set_fb_columns (tuid_t *tuid, int c);

void    tuid_set_key_up (tuid_t *tuid, int k);
void  tuid_set_key_down (tuid_t *tuid, int k);
void  tuid_set_key_left (tuid_t *tuid, int k);
void tuid_set_key_right (tuid_t *tuid, int k);
void tuid_set_key_enter (tuid_t *tuid, int k);
void tuid_set_key_enter_l (tuid_t *tuid, int k);
void   tuid_set_key_esc (tuid_t *tuid, int k);

/*
 * User Functions
 */

/*
 * =============== Menu API ===================
 */
void   tui_menud_set_mask (tuid_t *tuid, uint8_t pos);
void tui_menud_clear_mask (tuid_t *tuid, uint8_t pos);

void tui_menud_init (tuid_t *tuid);
menud_item_t* tui_menud_this (tuid_t *tuid);
menud_item_t* tui_menud_id2idx (menud_item_t *mn, menu_id_t id);
menu_id_t     tui_menud_idx2id (menud_item_t *mn);

ui_return_t      tui_menud (tuid_t *tuid, int key, menud_item_t *mn, Lang_en ln);
ui_return_t  tui_comboboxd (tuid_t *tuid, int live, int key, combobox_item_t *items, int *id, Lang_en ln);
ui_return_t  tui_valueboxd (tuid_t *tuid, int live, int key, text_t cap, text_t units, float up, float down, float step, int dec, float *value);
ui_return_t   tui_timeboxd (tuid_t *tuid, int live, int key, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t *value);
ui_return_t   tui_textboxd (tuid_t *tuid, int key, text_t cap, char* str, int size);
ui_return_t    tui_msgboxd (tuid_t *tuid, int key, text_t cap, text_t msg);

ui_return_t tui_line_menud (tuid_t *tuid, int key, menud_item_t *mn, Lang_en ln);
ui_return_t tui_line_valueboxd (tuid_t *tuid, int live, int key, text_t units, float up, float down, float step, int width, int dec, float *value);
ui_return_t tui_line_comboboxd (tuid_t *tuid, int live, int key, combobox_item_t *items, int *id, Lang_en ln);
ui_return_t tui_line_timeboxd  (tuid_t *tuid, int live, int key, uint8_t frm, time_t up, time_t down, time_t step, time_t *value);

#ifdef  __cplusplus
}
#endif

#endif //#ifndef  __tuid_h__
