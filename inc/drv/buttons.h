/*!
 * \file buttons.h
 * \brief
 *    A target independent direct connect button driver
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
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
 */
#ifndef __buttons_h__
#define __buttons_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <algo/queue.h>
#include <stdint.h>
#include <time.h>

typedef  int         keys_t;

#define BTN_NULL              (-1)

#define BTN_DEF_HOLDTIME      (2000)      // 2000 Ticks
#define BTN_NUMBER            (16)
#define INPUT_BUFFER_SIZE     (10)



#define BTN_LONG_PRE_MASK     (1 << BTN_NUMBER)
#define BTN_LONG_REL_MASK     (1 << (BTN_NUMBER+1))

typedef uint8_t (*btn_pin_t) (void);

/*!
 * Button Pin assignements.
 * Each one can be called xx.BTN4(); or xx.BTN4(); in order to set
 * or clear the corresponding pin.
 *
 * \note These pointers MUST to be assigned from main application.
 */
typedef volatile struct
{
   btn_pin_t  btn0;      /*!< Pointer for BTN0 pin */
   btn_pin_t  btn1;      /*!< Pointer for BTN1 pin */
   btn_pin_t  btn2;      /*!< Pointer for BTN2 pin */
   btn_pin_t  btn3;      /*!< Pointer for BTN3 pin */
   btn_pin_t  btn4;      /*!< Pointer for BTN4 pin */
   btn_pin_t  btn5;      /*!< Pointer for BTN5 pin */
   btn_pin_t  btn6;      /*!< Pointer for BTN6 pin */
   btn_pin_t  btn7;      /*!< Pointer for btN7 pin */
   btn_pin_t  btn8;      /*!< Pointer for BTN8 pin */
   btn_pin_t  btn9;      /*!< Pointer for BTN9 pin */
   btn_pin_t  btn10;     /*!< Pointer for BTN10 pin */
   btn_pin_t  btn11;     /*!< Pointer for BTN11 pin */
   btn_pin_t  btn12;     /*!< Pointer for BTN12 pin */
   btn_pin_t  btn13;     /*!< Pointer for BTN13 pin */
   btn_pin_t  btn14;     /*!< Pointer for BTN14 pin */
   btn_pin_t  btn15;     /*!< Pointer for BTN15 pin */
}btn_io_t;

/*!
 * Alpharithmetic LCD Public Data struct
 */
typedef volatile struct
{
   btn_io_t       io;            /*!< Link to IO struct */
   clock_t        holdtime;
   clock_t        reptime;
   uint8_t        repetitive  :1;
   drv_status_en  status;
}btn_t;

extern btn_t BTN;

typedef enum
{
   BTN_IDLE=0, BTN_PRE, BTN_LONG
}btn_state_t;



/*
 *  ============= PUBLIC Buttons API =============
 */

/*
 * Link and Glue functions
 */
void btn_link (volatile btn_pin_t *sio, btn_pin_t pfun);
#define  btn_link_btn0(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn0, (btn_pin_t)_pfun)
#define  btn_link_btn1(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn1, (btn_pin_t)_pfun)
#define  btn_link_btn2(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn2, (btn_pin_t)_pfun)
#define  btn_link_btn3(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn3, (btn_pin_t)_pfun)
#define  btn_link_btn4(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn4, (btn_pin_t)_pfun)
#define  btn_link_btn5(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn5, (btn_pin_t)_pfun)
#define  btn_link_btn6(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn6, (btn_pin_t)_pfun)
#define  btn_link_btn7(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn7, (btn_pin_t)_pfun)
#define  btn_link_btn8(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn8, (btn_pin_t)_pfun)
#define  btn_link_btn9(_pfun)    btn_link ((volatile btn_pin_t*)&BTN.io.btn9, (btn_pin_t)_pfun)
#define  btn_link_btn10(_pfun)   btn_link ((volatile btn_pin_t*)&BTN.io.btn10, (btn_pin_t)_pfun)
#define  btn_link_btn11(_pfun)   btn_link ((volatile btn_pin_t*)&BTN.io.btn11, (btn_pin_t)_pfun)
#define  btn_link_btn12(_pfun)   btn_link ((volatile btn_pin_t*)&BTN.io.btn12, (btn_pin_t)_pfun)
#define  btn_link_btn13(_pfun)   btn_link ((volatile btn_pin_t*)&BTN.io.btn13, (btn_pin_t)_pfun)
#define  btn_link_btn14(_pfun)   btn_link ((volatile btn_pin_t*)&BTN.io.btn14, (btn_pin_t)_pfun)
#define  btn_link_btn15(_pfun)   btn_link ((volatile btn_pin_t*)&BTN.io.btn15, (btn_pin_t)_pfun)

/*
 * Set functions
 */
void btn_set_holdtime (clock_t holdtime);
void btn_set_reptime (clock_t reptime);
void btn_set_repetitive (uint8_t rep);

/*
 * User Functions
 */
void btn_flush (void);        /*!< for compatibility */
void btn_service (void);

void btn_deinit (void);
drv_status_en btn_init (void);

keys_t btn_getkey (uint8_t wait);
drv_status_en btn_ioctl (ioctl_cmd_t cmd, ioctl_buf_t buf);


#ifdef __cplusplus
}
#endif

#endif   //#ifndef __buttons_h__
