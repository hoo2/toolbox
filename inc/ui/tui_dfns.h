/*!
 * \file tui_dfns.h
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
#ifndef  __tui_dfns_h__
#define  __tui_dfns_h__

#ifdef   __cplusplus
extern "C" {
#endif


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

#define  UID_EMPTY               {0}, (0)                                        /*!< Place holder */
#define  UID_BACK                .node.task=(taskd_ft)(!NULL), (UI_RETURN)       /*!< Back/return functionality node */
#define  UID_TASK(_x)            .node.task=(taskd_ft)(_x), (UI_TASK_ITEM)       /*!< Template for demonised task functionality node */
#define  UID_MENU(_x)            .node.menu=(menud_item_t*)(_x), (UI_MENU_ITEM)  /*!< Template for demonised sub-menu functionality node */

/*
 * Mask defines
 */
#define  UI_IT_EN                (UI_MENU_MASK_SIZE-1)                           /*!< EN Mask item. Last position is always enabled */
#define  UI_IT_DIS               (0)                                             /*!< DIS Mask item. First position is always disabled */
#define  UI_MM_EN                {UI_IT_EN, UI_IT_EN, UI_IT_EN, UI_IT_DIS}       /*!< Always enable item */
#define  UI_MM_DIS               {UI_IT_DIS, UI_IT_DIS, UI_IT_DIS, UI_IT_EN}     /*!< Always disable item */

#ifdef   __cplusplus
}
#endif

#endif //#ifndef  __tui_dfns_h__
