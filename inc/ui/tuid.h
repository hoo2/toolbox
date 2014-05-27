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

/*
 * ========================== User Defines ============================
 */
/*
 * =========== Menu ============
 */
#define  UI_CALLMENU_SIZE        (3)


/*
 * =========== Text Box ============
 */
#define  UI_TEXTBOX_SIZE      (14)

/*
 * ============ Time Box ============
 */
#define  UI_TIMEBOX_SIZE      (12)

/*
 * ========================== Helper Defines ============================
 */

/*
 * Node defines
 */
#define  UI_EMPTY                {0}, (0)                                        /*!< Place holder */
#define  UI_BACK                 .node.task=(task_ft)(!NULL), (UI_RETURN)        /*!< Back/return functionality node */
#define  UI_TASK(_x)             .node.task=(task_ft)(_x), (UI_TASK_ITEM)        /*!< Template for task functionality node */
#define  UI_MENU(_x)             .node.menu=(menu_item_t*)(_x), (UI_MENU_ITEM)   /*!< Template for sub-menu functionality node */

/*
 * Mask defines
 */
#define  UI_IT_EN                (UI_MENU_MASK_SIZE-1)                           /*!< EN Mask item. Last position is always enabled */
#define  UI_IT_DIS               (0)                                             /*!< DIS Mask item. First position is always disabled */
#define  UI_MM_EN                {UI_IT_EN, UI_IT_EN, UI_IT_EN, UI_IT_DIS}       /*!< Always enable item */
#define  UI_MM_DIS               {UI_IT_DIS, UI_IT_DIS, UI_IT_DIS, UI_IT_EN)}    /*!< Always disable item */


/*
 * ================= Menud types ===================
 */
/*!
 * Enumerator for the node types
 */
typedef enum
{
   UI_NONE=0,     /*!< The node is empty - place holder */
   UI_RETURN,     /*!< The node is type return, we pop menu */
   UI_TASK_ITEM,  /*!< The node is task (user function) */
   UI_MENU_ITEM   /*!< The node is sub-menu */
}menu_item_type_en;

typedef ui_return_t (*task_ft) (void);       /*!< Menu function Task */
typedef struct _menu_item menu_item_t;

/*!
 * Node union structrure
 */
typedef union _node
{
   task_ft        task;    /*!< Pointer to a task function */
   menu_item_t    *menu;   /*!< Pointer to a submenu via the menu_item_t */
}node_t;

typedef  uint8_t    mm_item_t;  /*!< menu mask item type. Change this for size */
/*!
 * Enum for menu mask system
 */
typedef enum
{
   MM_CTRL=0, MM_OR, MM_AND, MM_NOT
}mm_item_en;

/*!
 * Structure for menu items.
 */
typedef struct _menu_item
{
   text_t      text[UI_NUM_OF_LANGUAGES];    /*!< Pointer to caption/frame strings */
   node_t      node;                         /*!< Pointer to nested menu or the task function to call */
   menu_item_type_en
               item_type;                    /*!< Menu item type enumerator */
   mm_item_t   mm[4];                        /*!< Each member holds the bit posision in the Menu_mask variable
                                                  of the EN/DIS flag for the menu item.
                                              */
}menu_item_t;


/*!
 * Structure holding the data each menu function needs
 * to push and pop.
 */
typedef struct
{
   menu_item_t*   menu;       /*!< The active menu */
   int            mn_it;      /*!< Custom pointer to active item in menu_item_t array */
   int            mn_frm;     /*!< Custom pointer to first frame item in menu_item_t array */
   int            fb_it;      /*!< Custom pointer to active item in frame buffer array */
   int            fb_frm;     /*!< Custom pointer to first frame item in frame buffer array */
}ui_menu_t;

/*!
 * Structure for a ui_menu_t stack
 */
typedef volatile struct
{
   ui_menu_t   mstack[UI_CALLMENU_SIZE];     /*!< The stack array */
   uint8_t     sp;                           /*!< The stack pointer */
}menu_stack_t;

/*
 * ============== Combo Box types ==================
 */
typedef struct
{
   text_t      text[UI_NUM_OF_LANGUAGES];
   int         id;
}combobox_item_t;


/*
 * ============ Text UI user type ============
 */

/*!
 * demonised tui type
 */
typedef struct
{
   ui_keys_t      keys;             /*!< Key asignements */
   ui_menu_t      menu_data;        /*!< Current/active menu */
   fb_t           frame_buffer;     /*!< Frame buffer info */
   menu_stack_t   hist;             /*!< Call menu stack */
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
void tuid_set_fb_lines (int l);
void tuid_set_fb_columns (int c);

void    tuid_set_key_up (int k);
void  tuid_set_key_down (int k);
void  tuid_set_key_left (int k);
void tuid_set_key_right (int k);
void tuid_set_key_enter (int k);
void tuid_set_key_enter_l (int k);
void   tuid_set_key_esc (int k);

/*
 * User Functions
 */

/*
 * =============== Menu API ===================
 */
void   tui_menud_set_mask (tuid_t *tuid, uint8_t pos);
void tui_menud_clear_mask (tuid_t *tuid, uint8_t pos);

void tui_menud_init (tuid_t *tuid);
menu_item_t* tui_menud_this (tuid_t *tuid);

ui_return_t tui_menud (tuid_t *tuid, int key, menu_item_t *mn, Lang_en ln);
ui_return_t tui_comboboxd (tuid_t *tuid, int key, combobox_item_t *items, int *id, Lang_en ln);
ui_return_t valueboxd (int key, text_t cap, text_t units, float up, float down, float step, int dec, float *value);
ui_return_t  timeboxd (int key, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t *value);
ui_return_t  textboxd (int key, text_t cap, char* str, int8_t size, Lang_en ln);


#ifdef  __cplusplus
}
#endif

#endif //#ifndef  __tuid_h__
