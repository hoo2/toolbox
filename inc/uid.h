/*!
 * \file ui.h
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

#ifndef  __uid_h__
#define  __uid_h__

#ifdef   __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <printf.h>
#include "stime.h"
#include <ctype.h>

/* ========================== User Defines ============================ */
#define  UI_CALLMENU_SIZE     (6)
#define  UI_TIMEBOX_SIZE      (12)
#define  UI_TEXTBOX_SIZE      (14)
#define  UI_MENU_MASK_SIZE    (128) // multiplier of 8


/* ========================== General Defines ============================ */
#define  UI_NUM_OF_LANGUAGES     (2)
#define  UI_EMPTY                {0}, (0)
#define  UI_BACK                 .node.task=(task_ft)(!NULL), (UI_RETURN)  // This pointer is not used
#define  UI_TASK(_x)             .node.task=(task_ft)(_x), (UI_TASK_ITEM)
#define  UI_MENU(_x)             .node.menu=(menu_item_t*)(_x), (UI_MENU_ITEM)

#define  UI_TIME_SS              (0x01)
#define  UI_TIME_MM              (0x02)
#define  UI_TIME_HH              (0x04)
#define  UI_TIME_DD              (0x08)

/* ========================== UI Data types ============================ */
typedef char* text_t;

typedef volatile struct
{
   int   UP;
   int   DOWN;
   int   ENTER;
   int   RIGHT;
   int   LEFT;
   int   ESC;
}ui_keys_t;
extern ui_keys_t   ui_keys;

typedef enum
{
   EXIT_STAY=0,
   EXIT_RETURN
}ui_return_t;

typedef enum
{
   LANG_EN=0,
   LANG_GR
}Lang_en;

typedef enum
{
   UI_NONE=0,
   UI_RETURN,
   UI_TASK_ITEM,
   UI_MENU_ITEM
}menu_item_type_en;

typedef ui_return_t (*task_ft) (void);           // Menu function Task
typedef struct _menu_item menu_item_t;

typedef union _node
{
   task_ft        task;
   menu_item_t    *menu;
}node_t;

typedef  uint8_t    mm_item_t;  /*!< menu mask item type. Change this for size */


typedef enum
{
   MM_CTRL=0, MM_OR, MM_AND, MM_NOT
}mm_item_en;

typedef struct _menu_item
{
   text_t   text[UI_NUM_OF_LANGUAGES];
   node_t   node;
   menu_item_type_en
            item_type;
   mm_item_t mm[4];
   /*!<
    * Each member holds the bit posision in the Menu_mask variable
    * of the EN/DIS flag for the menu item.
    */
}menu_item_t;

typedef volatile struct
{
   menu_item_t*   mstack[UI_CALLMENU_SIZE];
   uint8_t        cstack[UI_CALLMENU_SIZE];
   uint8_t        sp;
}menu_stack_t;

typedef struct
{
   text_t      text[UI_NUM_OF_LANGUAGES];
   int         id;
}combobox_item_t;

#define  UI_IT_EN                (UI_MENU_MASK_SIZE-1)   /*!< Last position is always enabled */
#define  UI_IT_DIS               (0)                     /*!< First position is always disabled */

#define  UI_MM_EN                {UI_IT_EN, UI_IT_EN, UI_IT_EN, UI_IT_DIS}
#define  UI_MM_DIS               {UI_IT_DIS, UI_IT_DIS, UI_IT_DIS, UI_IT_EN)}


/* ================  Macro Functions  ======================*/
#define  _PREV_ITEM(mn,it)                         \
   do                                              \
      if (!--it) {                                 \
         for (it=1 ; mn[it].node.task ; ++it);     \
            --it;                                  \
      }                                            \
   while (!_menu_item_active (&mn[it]))

#define  _NEXT_ITEM(mn,it)                         \
   do                                              \
      if (!mn[++it].node.task)                     \
         it=1;                                     \
   while (!_menu_item_active (&mn[it]))

#define  _ROLL_2TOP(mn,it)                         \
   do {                                            \
      for (it=1 ; mn[it].node.task ; ++it);        \
      _PREV_ITEM(mn,it);                           \
   }while (0)

#define  _ROLL_2BOTTOM(mn,it)                      \
   do {                                            \
      it=1;                                        \
      _NEXT_ITEM(mn,it);                           \
   }while (0)


/* ================    Exported Functions    ======================*/
extern void ui_print_ctrl (char ch);
extern void ui_print_caption (text_t cap);
extern void ui_print_box (text_t box);
extern void ui_print_frame (text_t fr, size_t step);
extern int ui_getkey (uint8_t wait);

void ui_set_menu_mask (uint8_t pos);
void ui_clear_menu_mask (uint8_t pos);
void ui_menu_init (void);
ui_return_t ui_menu (int key, menu_item_t *mn, Lang_en ln);
menu_item_t* ui_this_menu (void);
ui_return_t ui_combobox (int key, combobox_item_t *items, int *id, Lang_en ln);
ui_return_t ui_valuebox (int key, text_t cap, text_t units, float up, float down, float step, int dec, float *value);
ui_return_t ui_timebox (int key, text_t cap, uint8_t frm, time_t up, time_t down, time_t step, time_t *value);
ui_return_t ui_textbox (int key, text_t cap, char* str, int8_t size, Lang_en ln);


#ifdef  __cplusplus
}
#endif

#endif //#ifndef  __ui_h__
