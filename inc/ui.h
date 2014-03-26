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

#ifndef  __ui_h__
#define  __ui_h__

#ifdef   __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <time.h>
#include <printf.h>

/* ========================== User Defines ============================ */
 /*   Menu Button Map  */
#define  UP                (1)
#define  UP_L              (65536)
#define  UP_R              (655360)
#define  DOWN              (2)
#define  DOWN_L            (65537)
#define  DOWN_R            (655370)
#define  ENTER             (4)
#define  ENTER_L           (BT_C_L)

#define  RIGHT             (-1)
#define  LEFT              (-2)
#define  ESC               (-3)



/* ========================== User Defines ============================ */

#define UI_NUM_OF_LANGUAGES      (2)
#define UI_EMPTY                 {0}, (0)
#define UI_BACK                  .node.task=(task_ft)(!NULL), (UI_RETURN)  // This pointer is not used
#define UI_TASK(_x)              .node.task=(task_ft)(_x), (UI_TASK_ITEM)
#define UI_MENU(_x)              .node.menu=(menu_item_t*)(_x), (UI_MENU_ITEM)




/* ========================== UI Data types ============================ */
typedef const char* text_t;

typedef enum
{
   LANG_EN=0,
   LANG_GR
}language_en;

typedef enum
{
   UI_NONE=0,
   UI_RETURN,
   UI_TASK_ITEM,
   UI_MENU_ITEM
} menu_item_type_en;

typedef void (*task_ft) (void);           // Menu function Task
typedef struct _menu_item menu_item_t;

typedef union _node
{
   task_ft        task;
   menu_item_t    *menu;
}node_t;

typedef struct _menu_item
{
   text_t   text[UI_NUM_OF_LANGUAGES];
   node_t   node;
   menu_item_type_en
            item_type;
}menu_item_t;

typedef struct
{
   text_t      text[UI_NUM_OF_LANGUAGES];
   int         id;
}combobox_item_t;


/* ================    Exported Functions    ======================*/
extern void ui_print_ctrl (char ch);
extern void ui_print_caption (text_t cap);
extern void ui_print_box (text_t box);
extern void ui_print_frame (text_t fr, size_t step);
extern int ui_getkey (uint8_t wait);

void ui_menu (menu_item_t *menu, language_en ln);
int ui_combobox (combobox_item_t *items, int cur, language_en ln);
float  ui_valuebox (text_t cap, float up, float down, float step, float cur);
time_t ui_timebox (text_t cap, time_t up, time_t down, time_t step, time_t cur);

int8_t   UI_TextBox (text_t cap, char* str, int8_t size);


#ifdef  __cplusplus
 }
#endif

#endif //#ifndef  __ui_h__
