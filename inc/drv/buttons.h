/**
  * BTN16.h
  *
  * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
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
  * Date:       06/2013
  * Version:
  *
  */
#ifndef __Buttons_h__
#define __Buttons_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <time.h>


typedef  int         keys_t;
typedef  uint8_t     bt_t;

#define BTN_NULL              (-1)

#define BTN_DEF_HOLDTIME      (2000)      // 2000 Ticks
#define BTN_NUMBER            (16)
#define INPUT_BUFFER_SIZE     (10)



#define BTN_LONG_PRE_MASK     (1 << BTN_NUMBER)
#define BTN_LONG_REL_MASK     (1 << (BTN_NUMBER+1))

typedef bt_t (*BTN_Pin_t) (void);

/*!
 * Button Pin assignements.
 * Each one can be called xx.BTN4(); or xx.BTN4(); in order to set
 * or clear the corresponding pin.
 *
 * \note These pointers MUST to be assigned from main application.
 */
typedef volatile struct
{
   BTN_Pin_t  BT0;      /*!< Pointer for BTN0 pin */
   BTN_Pin_t  BT1;      /*!< Pointer for BTN1 pin */
   BTN_Pin_t  BT2;      /*!< Pointer for BTN2 pin */
   BTN_Pin_t  BT3;      /*!< Pointer for BTN3 pin */
   BTN_Pin_t  BT4;      /*!< Pointer for BTN4 pin */
   BTN_Pin_t  BT5;      /*!< Pointer for BTN5 pin */
   BTN_Pin_t  BT6;      /*!< Pointer for BTN6 pin */
   BTN_Pin_t  BT7;      /*!< Pointer for BTN7 pin */
   BTN_Pin_t  BT8;      /*!< Pointer for BTN8 pin */
   BTN_Pin_t  BT9;      /*!< Pointer for BTN9 pin */
   BTN_Pin_t  BT10;     /*!< Pointer for BTN10 pin */
   BTN_Pin_t  BT11;     /*!< Pointer for BTN11 pin */
   BTN_Pin_t  BT12;     /*!< Pointer for BTN12 pin */
   BTN_Pin_t  BT13;     /*!< Pointer for BTN13 pin */
   BTN_Pin_t  BT14;     /*!< Pointer for BTN14 pin */
   BTN_Pin_t  BT15;     /*!< Pointer for BTN15 pin */
}BTN_IO_t;

/*!
 * Alpharithmetic LCD Public Data struct
 */
typedef volatile struct
{
   BTN_IO_t       IO;            /*!< Link to IO struct */
   clock_t        holdtime;
   clock_t        reptime;
   uint8_t        repetitive  :1;
}BTN_t;

extern BTN_t BTN;

typedef volatile struct
{
   keys_t   ib [INPUT_BUFFER_SIZE];
   int8_t   front, rear;     //queue pointers
}BTN_input_buffer_t;

typedef enum
{
   BTN_IDLE=0, BTN_PRE, BTN_LONG
}BTN_State_t;

int BTN_Connect (volatile BTN_Pin_t *sio, BTN_Pin_t pfun);
keys_t BTN_Getkey (uint8_t wait);
void BTN_Flush (void);
void BTN_Service (void);


#ifdef __cplusplus
 }
#endif

#endif   //#ifndef __Buttons_h__
