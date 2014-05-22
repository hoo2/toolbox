/*!
 * \file timebox.h
 * \brief
 *    A plain and demonised time-box functionality.
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

#ifndef  __timebox_h__
#define  __timebox_h__

#ifdef   __cplusplus
extern "C" {
#endif

#include <ui/uid.h>


ui_return_t ui_timebox (int key,
                     text_t cap,
                    uint8_t frm,
                     time_t up,
                     time_t down,
                     time_t step,
                     time_t *value);

#ifdef   __cplusplus
}
#endif

#endif //#ifndef  __timebox_h__
