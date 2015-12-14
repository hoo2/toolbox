/*!
 * \file tui.h
 * \brief
 *    A small tui library.
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

#ifndef  __ui_h__
#define  __ui_h__

#ifdef   __cplusplus
extern "C" {
#endif

#include <ui/tuix.h>
#include <ui/tui_dfns.h>

/*
 * ============ Text UI types ============
 */

/*
 * ============ Menu types ============
 */
typedef void (*task_ft) (void);       /*!< Menu function Task */
typedef struct _menu_item menu_item_t;

/*!
 * Node union structrure for demonised menu
 */
typedef union _node
{
   task_ft        task;    /*!< Pointer to a task function */
   menu_item_t    *menu;   /*!< Pointer to a submenu via the menud_item_t */
}node_t;

/*!
 * Structure for demonised menu items.
 */
typedef struct _menu_item
{
   menu_id_t      id;
   text_t         text[UI_NUM_OF_LANGUAGES];    /*!< Pointer to caption/frame strings */
   node_t         node;                         /*!< Pointer to nested menu or the task function to call */
   menu_item_type_en
                  item_type;                    /*!< Menu item type enumerator */
   mm_item_t      mm[4];                        /*!<
                                                 * Each member holds the bit posision in the Menu_mask variable
                                                 * of the EN/DIS flag for the menu item.
                                                 */
}menu_item_t;

/*
 * ============ Key types ============
 */
typedef int (*tui_get_key_t) (uint8_t);

/*!
 * tui type
 */
typedef struct
{
   ui_keys_t      keys;             /*!< Key asignements */
   tui_get_key_t  get_key;          /*!< Pointer to key input function */
   fb_t           frame_buffer;     /*!< Frame buffer info */
   uint8_t        menu_mask[UI_MENU_MASK_SIZE/8];
                                 /*!< Variable to addressed by 8bit position mm array */
}tui_t;

/*
 * =============== Exported API ===================
 */

/*
 * Link and Glue functions
 */
void tui_link_framebuffer (tui_t *tui, uint8_t *fb);
void tui_link_get_key (tui_t *tui, tui_get_key_t fp);

/*
 * Set functions
 */
void   tui_set_fb_lines (tui_t *tui, int l);
void tui_set_fb_columns (tui_t *tui, int c);

void    tui_set_key_up (tui_t *tui, int k);
void  tui_set_key_down (tui_t *tui, int k);
void  tui_set_key_left (tui_t *tui, int k);
void tui_set_key_right (tui_t *tui, int k);
void tui_set_key_enter (tui_t *tui, int k);
void tui_set_key_enter_l (tui_t *tui, int k);
void   tui_set_key_esc (tui_t *tui, int k);

/*
 * User Functions
 */

/*
 * =============== Menu API ===================
 */
void   tui_menu_set_mask (tui_t *tui, uint8_t pos);
void tui_menu_clear_mask (tui_t *tui, uint8_t pos);

void tui_menu_init (tui_t *tui);
menu_item_t* tui_menu_this (tui_t *tui);
menu_item_t* tui_menu_id2idx (menu_item_t *mn, menu_id_t id);
menu_id_t    tui_menu_idx2id (menu_item_t *mn);

void      tui_menu (tui_t *tui, menu_item_t *menu, Lang_en ln);
int   tui_combobox (tui_t *tui, combobox_item_t *items, int cur, Lang_en ln);
float tui_valuebox (tui_t *tui, text_t cap, text_t units, float up, float down, float step, int dec, float cur);
time_t tui_timebox (tui_t *tui, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t cur);
void   tui_textbox (tui_t *tui, text_t cap, char* str, int size);


#ifdef  __cplusplus
}
#endif

#endif //#ifndef  __ui_h__
