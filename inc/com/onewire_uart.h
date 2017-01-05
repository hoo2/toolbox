/*!
 * \file onewire_uart.h
 * \brief
 *    A target independent 1-wire implementation using a microprocessor's uart
 *    for bit timing
 * \note
 *    This 1-wire implementation is based on MCU UART io functionality. In order
 *    to work it needs:
 *    1) A Open drain tx and floating(or pull-up) rx UART pin configuration with both pins
 *       connected to the 1-wire bus wire
 *    2) A Transmit function even in blocking/polling mode
 *    3) A receive function even in blocking/polling mode
 *    4) A baudrate set function
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2016 Choutouridis Christos (http://www.houtouridis.net)
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
#ifndef __onewire_uart_h__
#define __onewire_uart_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_types.h>
#include <toolbox_defs.h>
#include <string.h>

/* ================   General Defines   ====================*/

typedef uint8_t (*ow_uart_tx_ft) (uint8_t);  /*!< UART transmit function pointer */
typedef uint8_t (*ow_uart_rx_ft) (void);     /*!< UART receive function pointer */
typedef drv_status_en (*ow_uart_br_ft) (uint32_t);    /*!< UART baudrate modify function pointer */

/*!
 * 1-Wire operational state
 */
typedef enum {
   OWS_RESET = 0,       /*!< 1-Wire bus is during in Reset state */
   OWS_OPER             /*!< 1-Wire bus is during normal operation */
}ow_uart_state_en;

/*!
 * 1-Wire UART baudrate table
 */
typedef struct {
   uint32_t    reset;      /*!< Baudrate during reset */
   uint32_t    oper;       /*!< Baudrate during normal operation */
   uint32_t    current;    /*!< The current baudrate to use as flag */
}ow_uart_br_t;

/*!
 * 1-Wire driver function callback pointers
 * \note
 *    The function in this structure provided from the low level driver (usually).
 */
typedef struct {
   ow_uart_tx_ft  tx;      /*!< Pointer UART transmit function */
   ow_uart_rx_ft  rx;      /*!< Pointer UART receive function */
   ow_uart_br_ft  br;      /*!< Pointer to UART baudrate function */
}ow_uart_io_t;

/*!
 * 1-Wire Data type
 */
typedef struct {
   ow_uart_io_t      io;         /*!< Callback pointers and direction state */
   uint32_t          timing;     /*!< The selected timing mode */
   ow_uart_br_t      baudrate;   /*!< The current baudrate configuration */
   drv_status_en     status;     /*!< Toolbox driver status */
}ow_uart_t;



/*
 * ============ Helper Macros =============
 */

/*!
 * 1-Wire speed
 */
#define  OW_UART_T_STANDARD         (0)
#define  OW_UART_T_OVERDRIVE        (1)
/*!
 * Check if the timing is a valid \ref ow_uart_timing_en value
 */
#define  _ow_uart_is_timings(_t_)     ( ((_t_) == OW_UART_T_STANDARD) || ((_t_) == OW_UART_T_OVERDRIVE) )

/*
 *                   Timing Diagram
 * --------------------------------------------------
 * Based on Application Note 214 (www.maxim-ic.com)
 *
 *           ---                      ----  -  -  -  -------
 * Reset        \                    /    \ X  X  X /
 *               --------------------      -  -  - -
 *  RS:          |   |   |   |   |   |   |   |   |   |   |
 *  bit:          st   0   1   2   3   4   5   6   7   st
 *               < ---------- 1024/174 usec ------------->
 *
 *  8 bits, no parity, 1 stop
 *  Standard:                          Overdrive :
 *  BR: 9600,                          BR: 57600
 *  TX: 0xF0,                          TX: 0xF8
 *  RX: 0xF0 not present               RX: 0xF8 not present
 *      less if present                    less if present
 *
 *
 *           ---       --------------------------------------
 * Write 1      \     /
 *               ----
 *  RS:          |   |   |   |   |   |   |   |   |   |   |
 *  bit:          st   0   1   2   3   4   5   6   7   st
 *               < ------------- 87/11 usec ------------->
 *  8 bits, no parity, 1 stop
 *  standard:  BR: 115200              Overdrive: BR: 921600
 *  TX: 0xFF
 *  RX: 0xFF
 *
 *           ---                                       ------
 * Write 0      \                                     /
 *               -------------------------------------
 *  RS:          |   |   |   |   |   |   |   |   |   |   |
 *  bit:          st   0   1   2   3   4   5   6   7   st
 *               < ------------- 87/11 usec ------------->
 *  8 bits, no parity, 1 stop
 *  standard:  BR: 115200              Overdrive: BR: 921600
 *  TX: 0x00
 *  RX: 0x00
 *
 *           ---       -  -  -  -  -  -  -  -  -  -  - ------
 * Read         \     / X X X X X X X X X X X X X X X /
 *               ----  -  -  -  -  -  -  -  -  -  - -
 *  RS:          |   |   |   |   |   |   |   |   |   |   |
 *  bit:          st   0   1   2   3   4   5   6   7   st
 *               < ------------- 87/11 usec ------------->
 *                     ^
 *                     |
 *                Master sample
 *
 *  8 bits, no parity, 1 stop
 *  standard:  BR: 115200              Overdrive: BR: 921600
 *  TX: 0xFF
 *  RX: {1 - 0xFF,   0 - [0x00 - 0xFE] }
 *
 */
#define  _ow_baudrate_standard(_br_)   \
   do {                       \
      _br_.reset = 9600;      \
      _br_.oper = 115200;     \
      _br_.current = 9600;    \
   } while (0)

#define  _ow_baudrate_overdrive(_br_)  \
   do {                          \
      _br_.reset = 115200;       \
      _br_.oper = 921600;        \
      _br_.current = 115200;     \
   } while (0)

/*
 *  ============= PUBLIC ALCD API =============
 */

/*
 * Link and Glue functions
 */
void ow_uart_link_tx (ow_uart_t *ow, ow_uart_tx_ft tx);     /*!< link driver's transmit function */
void ow_uart_link_rx (ow_uart_t *ow, ow_uart_rx_ft rx);    /*!< link driver's receive function */
void ow_uart_link_br (ow_uart_t *ow, ow_uart_br_ft br);    /*!< link driver's baudrate function*/

/*
 * Set functions
 */
void ow_uart_set_timing (ow_uart_t *ow, uint32_t owt);      /*!< set 1-wire timing mode */

/*
 * User Functions
 */
void ow_uart_deinit (ow_uart_t *ow);             /*!< for compatibility */
drv_status_en ow_uart_init (ow_uart_t *ow);      /*!< for compatibility */

drv_status_en
         ow_uart_reset (ow_uart_t *ow);
uint8_t  ow_uart_rx (ow_uart_t *ow);
void     ow_uart_tx (ow_uart_t *ow, byte_t byte);
uint8_t  ow_uart_rw (ow_uart_t *ow, byte_t byte);
drv_status_en
         ow_uart_search (ow_uart_t *ow, uint8_t *romid);

drv_status_en ow_uart_ioctl (ow_uart_t *ow, ioctl_cmd_t cmd, ioctl_buf_t buf);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef __onewire_uart_h__ */
