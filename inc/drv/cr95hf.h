/*!
 * \file cr95hf.h
 * \brief
 *    A target independent CR95HF driver over uart communication
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2021 Christos Choutouridis (http://www.houtouridis.net)
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
#ifndef __cr95hf_h__
#define __cr95hf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <sys/types.h>
#include <cont/span08.h>
#include <cont/queue08.h>

/*
 * Function pointer prototypes
 */
typedef size_t (*cr95hf_tx_ft) (byte_t*, size_t);  //!< UART transmit function prototype
typedef size_t (*cr95hf_rx_ft) (byte_t*, size_t);  //!< UART receive function prototype

typedef queue_callback_ft  cr95hf_callback_ft;     //!< Fwd type definition for queue trigger functionality

/*
 * ================    General Types    ====================
 */

/*!
 * This data struct contains the connection with the target
 *
 * We depend on basic UART communication functionalities for transmit and receive.
 *
 *  - transmit \see cr95hf_tx_ft. Takes a pointer to tx buffer and a size and returns
 *    the number of transmitted bytes.
 *  - receive \see cr95hf_tx_ft. Takes a pointer to rx buffer to fill and a required number
 *    of bytes and returns the actual received nymber of bytes.
 *  - for toolbox queue \see queue08_t
 * \note
 *    We also depend locally on clock()
 */
typedef struct {
   cr95hf_tx_ft            tx;      //!< pointer to serial transmit function
   cr95hf_rx_ft            rx;      //!< pointer to serial receive function
   queue08_t               *queue;  //!< pointer to queue used by the LL driver inside UART's irq
}cr95hf_hal_t;

/*!
 * \brief CR95HF handle struct.
 */
typedef struct {
   cr95hf_hal_t   hal;              //!< Target depend pointers
   byte_t         *tx_buffer;       //!< Pointer to a driver wide transmit buffer for all communications
   byte_t         *rx_buffer;       //!< Pointer to a driver wide receive buffer for all communications
   uint8_t        calLow, calHigh;  //!< Calibration result values
}cr95hf_t;

/*!
 * CR95HF's response type
 * <code><len><data>
 */
typedef struct {
   drv_status_en  status;  //!< The status of the transaction
   byte_t         code;    //!< The result code from the CR95HF
   span08_t       frame;   //!< A span (size - buffer pointer) with the received data
                           //!< \note len is embedded to span structure and redundant to hold it outside.
}cr95hf_resp_t;



/*
 *  ============= PUBLIC CR95HF API =============
 */

/*
 * Link and Glue functions
 */
void cr95hf_link_tx_buffer (cr95hf_t *cr95hf, byte_t* buffer);
void cr95hf_link_rx_buffer (cr95hf_t *cr95hf, byte_t* buffer);
void cr95hf_link_tx (cr95hf_t *cr95hf, cr95hf_tx_ft fun);
void cr95hf_link_rx (cr95hf_t *cr95hf, cr95hf_rx_ft fun);
void cr95hf_link_queue (cr95hf_t *cr95hf, queue08_t *queue);
//void cr95hf_link_transmitted (cr95hf_t *cr95hf, cr95hf_transmitted_ft fun);
//void cr95hf_link_received (cr95hf_t *cr95hf, cr95hf_received_ft fun);
//void cr95hf_link_flush (cr95hf_t *cr95hf, cr95hf_flush_ft fun);

/*
 * Set functions
 */

/*
 * User Functions
 */
void cr95hf_deinit (cr95hf_t *cr95hf);          /*!< for compatibility */
drv_status_en cr95hf_init (cr95hf_t *cr95hf);   /*!< for compatibility */

drv_status_en cr95hf_irqin (cr95hf_t *cr95hf);

//helper functions
uint8_t cr95hf_nbControlBytes (uint8_t Protocol);

cr95hf_resp_t cr95hf_receive (cr95hf_t *cr95hf, uint8_t command);

cr95hf_resp_t cr95hf_echo (cr95hf_t *cr95hf);
cr95hf_resp_t cr95hf_idn (cr95hf_t *cr95hf);
cr95hf_resp_t cr95hf_protocolSelect (cr95hf_t *cr95hf, span08_t buffer);
cr95hf_resp_t cr95hf_sendRecv (cr95hf_t *cr95hf, span08_t buffer);

cr95hf_resp_t cr95hf_rdReg (cr95hf_t *cr95hf, uint8_t address, uint8_t count, uint8_t flags);
cr95hf_resp_t cr95hf_wrRegv (cr95hf_t *cr95hf, span08_t buffer);
cr95hf_resp_t cr95hf_idle (cr95hf_t *cr95hf, span08_t buffer, cr95hf_callback_ft callback);

cr95hf_resp_t cr95hf_field_off (cr95hf_t *cr95hf);
drv_status_en cr95hf_sync (cr95hf_t *cr95hf);
drv_status_en cr95hf_calibration (cr95hf_t *cr95hf);
cr95hf_resp_t cr95hf_wait4tag (cr95hf_t *cr95hf, cr95hf_callback_ft callback);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __cr95hf_h__ */
