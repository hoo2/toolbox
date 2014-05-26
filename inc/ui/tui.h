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

#ifndef  __ui_h__
#define  __ui_h__

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

/*
 * =========== Text Box ============
 */

/*
 * ============ Time Box ============
 */






/*
 * ============ Text UI types ============
 */


/*
 * tui type
 */
typedef struct
{
   ui_keys_t      keys;             /*!< Key asignements */
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
void  tui_link_framebuffer (tui_t *tui, uint8_t *fb);

/*
 * Set functions
 */


/*
 * User Functions
 */


#ifdef  __cplusplus
}
#endif

#endif //#ifndef  __ui_h__
