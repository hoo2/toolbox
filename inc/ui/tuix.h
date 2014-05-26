/*!
 * \file tuix.h
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

#ifndef  __tuix_h__
#define  __tuix_h__

#ifdef   __cplusplus
extern "C" {
#endif

#include <std/printf.h>
#include <std/stime.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>


/*
 * ============= Menu ==============
 */
#define  UI_NUM_OF_LANGUAGES     (2)
#define  UI_MENU_MASK_SIZE       (128) // multiplier of 8

/*
 * =========== Text Box ============
 */


/*
 * =========== Time Box ============
 */
#define  UI_TIME_SS              (0x01)
#define  UI_TIME_MM              (0x02)
#define  UI_TIME_HH              (0x04)
#define  UI_TIME_DD              (0x08)

/*
 * ================ Common Data types ===================
 */
typedef char* text_t;

typedef volatile struct
{
   int   UP;
   int   DOWN;
   int   ENTER;
   int   ENTER_L;
   int   RIGHT;
   int   LEFT;
   int   ESC;
}ui_keys_t;
extern ui_keys_t   ui_keys;

typedef enum
{
   LANG_EN=0,
   LANG_GR
}Lang_en;

/*!
 * Frame buffer type.
 * \note
 *    This buffer must to be defined in low level driver.
 */
typedef struct
{
   uint8_t *fb;   /*!< The 2 dimensions array frame buffer pointer */
   int l;         /*!< The lines of each frame buffer */
   int c;         /*!< The columns of each frame buffer */
}fb_t;

typedef enum
{
   EXIT_STAY=0,
   EXIT_RETURN
}ui_return_t;


#ifdef  __cplusplus
}
#endif

#endif //#ifndef  __tuix_h__