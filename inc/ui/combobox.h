/*!
 * \file combobox.h
 * \brief
 *    A plain and demonised combo-box functionality.
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

#ifndef  __combobox_h__
#define  __combobox_h__

#ifdef   __cplusplus
extern "C" {
#endif

#include <ui/uid.h>

typedef struct
{
   text_t      text[UI_NUM_OF_LANGUAGES];
   int         id;
}combobox_item_t;



ui_return_t ui_combobox (int key, combobox_item_t *items, int *id, Lang_en ln);

#ifdef   __cplusplus
}
#endif

#endif //#ifndef  __comboboxd_h__
