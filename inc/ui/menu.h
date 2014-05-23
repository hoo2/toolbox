/*!
 * \file menu.h
 * \brief
 *    A plain and demonised menu functionality.
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

#ifndef  __menu_h__
#define  __menu_h__

#ifdef   __cplusplus
extern "C" {
#endif

#include <ui/uid.h>
#include <string.h>

/* ========================== User Defines ============================ */
#define  UI_CALLMENU_SIZE     (3)
#define  UI_MENU_MASK_SIZE    (128) // multiplier of 8


/* ========================== General Defines ============================ */
#define  UI_EMPTY                {0}, (0)
#define  UI_BACK                 .node.task=(task_ft)(!NULL), (UI_RETURN)        // This pointer is not used
#define  UI_TASK(_x)             .node.task=(task_ft)(_x), (UI_TASK_ITEM)
#define  UI_MENU(_x)             .node.menu=(menu_item_t*)(_x), (UI_MENU_ITEM)


/* ========================== UI Data types ============================ */

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
   text_t      text[UI_NUM_OF_LANGUAGES];
   node_t      node;
   menu_item_type_en
               item_type;
   mm_item_t   mm[4];
   /*!<
    * Each member holds the bit posision in the Menu_mask variable
    * of the EN/DIS flag for the menu item.
    */
}menu_item_t;

typedef struct
{
   menu_item_t*   menu;
   int            item;
   int            frame_item;
}ui_menu_t;

typedef volatile struct
{
   ui_menu_t   mstack[UI_CALLMENU_SIZE];
   uint8_t     sp;
}menu_stack_t;

#define  UI_IT_EN                (UI_MENU_MASK_SIZE-1)   /*!< Last position is always enabled */
#define  UI_IT_DIS               (0)                     /*!< First position is always disabled */

#define  UI_MM_EN                {UI_IT_EN, UI_IT_EN, UI_IT_EN, UI_IT_DIS}
#define  UI_MM_DIS               {UI_IT_DIS, UI_IT_DIS, UI_IT_DIS, UI_IT_EN)}


/* ================  Macro Functions  ======================*/
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

void ui_set_menu_mask (uint8_t pos);
void ui_clear_menu_mask (uint8_t pos);
void ui_menu_init (void);
ui_return_t ui_menu (int key, menu_item_t *mn, Lang_en ln);
menu_item_t* ui_this_menu (void);


#ifdef   __cplusplus
}
#endif

#endif //#ifndef  __menu_h__
