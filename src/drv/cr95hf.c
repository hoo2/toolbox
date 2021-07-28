/*!
 * \file cr95hf.c
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
 */

#include <drv/cr95hf.h>
#include <drv/cr95hf_defines.h>

//! We depend on an environment with a well defined clock() functionality
extern clock_t clock (void);

#ifdef CR95HF_DEBUG_PRINTS
extern int printf(const char *frm, ...);
extern int puts(const char *dst);
#endif

/*!
 * \def CONTROL_BYTE_SEQUENCE
 * For 14443A:
 * <--- to host ... | CTRL0 | CTRL1 | CTRL2 ]
 *
 *               b7     b6      b5     b4      b3      b2      b1      b0
 *    CTRL0: [   col |      | crcEr | parEr | sigb3 | sigb2 | sigb1 | sigb0 ]
 *          col: Set if collision is detected
 *        crcEr: Set on CRC error, or if no crc is performed or requested
 *        parEr: Set on parity error
 *   sigb[0..3]: Number of significant bits of the first byte
 *
 *   CTRL1: The first BYTE where the collision is detected. Zero if no collision detection occures
 *   CTRL0: The first BIT where the collision is detected. Zero if no collision detection occures
 *
 *
 * For 14443B:
 * <--- to host ... | CTRL0 ]
 *
 *    CTRL0: ?? Zero if CRC is validated ??
 *
 */
#define CONTROL_BYTE_SEQUENCE

//! A small blocking delay for msec
static void delay(clock_t t) {
   clock_t tickstart = clock();

   while((clock() - tickstart) < t)
      ;
}

/*!
 * \param   Protocol  RF protocol (ISO 14443 A or B or ISO15693 or Felica)
 *    \arg  PS_PARAM_CODE_TAG_FIELDOFF
 *    \arg  PS_PARAM_CODE_TAG_ISO15693
 *    \arg  PS_PARAM_CODE_TAG_ISO14443A
 *    \arg  PS_PARAM_CODE_TAG_ISO14443B
 *    \arg  PS_PARAM_CODE_TAG_FELICA
 * \return  true if the protocol is available, otherwise false.
 */
static bool isAnAvailableProtocol (uint8_t Protocol) {
   switch(Protocol) {
      case PROTOCOL_CODE_TAG_FIELDOFF:
      case PROTOCOL_CODE_TAG_ISO15693:
      case PROTOCOL_CODE_TAG_ISO14443A:
      case PROTOCOL_CODE_TAG_ISO14443B:
      case PROTOCOL_CODE_TAG_FELICA:   return true;
      default:                         return false;
   }
}

/*!
 * Check if the result code responded from cr95hf is valid or error
 *
 * \param command    The CR95HF command to check
 * \param code       The code to check
 * \return           True if CR95HF respond with valid result code
 */
static bool isValidCode (uint8_t command, uint8_t code) {
   switch (command) {
      default:
      case IDN:
      case PROTOCOL_SELECT:
      case LISTEN:
      case IDLE:
      case READ_REGISTER:
      case WRITE_REGISTER:
      case BAUD_RATE:      return (code == SUCCESFUL_COMMAND);
      case SEND:
      case SEND_RECEIVE:   return (code == SUCCESFUL_SENDS);
      case ECHO:           return (code == ECHORESPONSE);
   }
}

static size_t transmit(cr95hf_t *cr95hf, uint8_t code, span08_t buffer) {
   size_t r =0;
   uint8_t len = (uint8_t)buffer.size;

   // send command frame
   r += cr95hf->hal.tx(&code, 1);
   r += cr95hf->hal.tx(&len, (code != ECHO) ? 1:0);
   r += cr95hf->hal.tx(buffer.data, (code != ECHO) ? buffer.size:0);

#ifdef CR95HF_DEBUG_PRINTS
   printf (">>>> %02X %02X ", code, buffer.size);
   for (uint8_t i=0 ; i < buffer.size ; ++i )
      printf ("%c%02X", (i==0) ? ':':' ', buffer.data[i]);
   puts("\n");
#endif
   return r;
}

/*
 *  ============= PUBLIC CR95HF API =============
 */

/*
 * Link and Glue functions
 */
__INLINE void cr95hf_link_tx_buffer (cr95hf_t *cr95hf, byte_t* buffer) {
   cr95hf->tx_buffer = buffer;
}
__INLINE void cr95hf_link_rx_buffer (cr95hf_t *cr95hf, byte_t* buffer) {
   cr95hf->rx_buffer = buffer;
}
__INLINE void cr95hf_link_tx (cr95hf_t *cr95hf, cr95hf_tx_ft fun) {
   cr95hf->hal.tx = fun;
}
__INLINE void cr95hf_link_rx (cr95hf_t *cr95hf, cr95hf_rx_ft fun) {
   cr95hf->hal.rx = fun;
}
__INLINE void cr95hf_link_queue (cr95hf_t *cr95hf, queue08_t *queue) {
   cr95hf->hal.queue = queue;
}


/*
 * Set functions
 */
//void cr95hf_set (cr95hf_t *cr95hf) ;


/*
 * User Functions
 */
void cr95hf_deinit (cr95hf_t *cr95hf) {         /*!< for compatibility */
   memset((void*)cr95hf, 0, sizeof(cr95hf_t));
}

drv_status_en cr95hf_init (cr95hf_t *cr95hf) {  /*!< for compatibility */
   queue08_flush (cr95hf->hal.queue);  // Flush the buffer
   return DRV_READY;
}

/*!
 * Send IRQ_IN  low pulse to CR95HF to activate it.
 *
 * \param   cr95hf   Handle to cr95hf
 * \return           DRV_READY
 */
drv_status_en cr95hf_irqin (cr95hf_t *cr95hf) {
   byte_t send_buffer[] = {0x00};
   cr95hf->hal.tx(send_buffer, 1);
   delay (12);
   return DRV_READY;
}

//helper functions
/*!
 * Return the number of control bytes based on selected protocol.
 *
 * CR95HF adds a control byte sequence at the end of "some" communication messages.
 * This is true for selection/anticollision messages but probably its true for others also.
 *
 * The encoding of these bytes is not documented but reverse engineered. \see CONTROL_BYTE_SEQUENCE
 * \param   Protocol    The selected protocol
 * \return              The number of bytes in control byte sequence
 */
uint8_t cr95hf_nbControlBytes (uint8_t Protocol) {
   switch(Protocol) {
      default:
      case PROTOCOL_CODE_TAG_FIELDOFF:    return 0;
      case PROTOCOL_CODE_TAG_ISO14443A:   return 3;
      case PROTOCOL_CODE_TAG_ISO15693:
      case PROTOCOL_CODE_TAG_ISO14443B:
      case PROTOCOL_CODE_TAG_FELICA:      return 1;
   }
}

/*!
 * Receive the CR95HF response and check if it is valid based on response code.
 *
 * \param cr95hf     Handle to cr95hf
 * \param command    The transmitted command
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 * \note
 *    The response span points to handle's rx_buffer
 * \note
 *    The response status is DRV_READY on a successful transaction or DRV_ERROR on any other situation.
 */
cr95hf_resp_t cr95hf_receive (cr95hf_t *cr95hf, uint8_t command) {
   cr95hf_resp_t resp = { DRV_ERROR, 0x00, {0x00, cr95hf->rx_buffer} };
   uint8_t len;

   // Receive response frame (blocking mode)
   do {
      if (cr95hf->hal.rx(&resp.code, 1) == 0)
         break;
      if ((command != ECHO) && cr95hf->hal.rx(&len, 1) == 0)
         break;
      resp.frame.size = (command != ECHO) ? (((size_t)resp.code & 0x60) << 3) | len : 0;
      if (cr95hf->hal.rx(resp.frame.data, resp.frame.size) != resp.frame.size)
         break;
      if (!isValidCode(command, resp.code))   // assert return code
         break;
      resp.status = DRV_READY;
   } while (0);

   // debug output
#ifdef CR95HF_DEBUG_PRINTS
   if (command != ECHO)
      printf ("[%s] %02X %02X ", (resp.status == DRV_READY)? "OK" : "ER", resp.code, resp.frame.size);
   else
      printf ("[%s] %02X ", (resp.status == DRV_READY)? "OK" : "ER", resp.code);
   for (uint8_t i=0 ; i < resp.frame.size ; ++i )
      printf ("%c%02X", (i==0) ? ':':' ', resp.frame.data[i]);
   puts("\n");
#endif
   return resp;
}


/*!
 * Transmit an ECHO and return the response.
 *
 * \param cr95hf     Handle to cr95hf
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 */
cr95hf_resp_t cr95hf_echo (cr95hf_t *cr95hf) {
   transmit (cr95hf, ECHO, (span08_t){0, 0});
   return cr95hf_receive(cr95hf, ECHO);
}

/*!
 * Transmit an IDN and return the response.
 *
 * \param cr95hf     Handle to cr95hf
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 */
cr95hf_resp_t cr95hf_idn (cr95hf_t *cr95hf) {
   transmit (cr95hf, IDN, (span08_t){0, 0});
   return cr95hf_receive(cr95hf, IDN);
}

/*!
 * \brief
 *    Send a ProtocolSeclect command to CR95HF.
 *
 * \param   cr95hf   The CR95HF handle to use.
 * \param   buffer   Span(len-data) with the data to send
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 */
cr95hf_resp_t cr95hf_protocolSelect (cr95hf_t *cr95hf, span08_t buffer) {
   // assert the function parameters
   if (!isAnAvailableProtocol(buffer.data[0]))
      return (cr95hf_resp_t){DRV_ERROR, 0, {0, 0}};

   transmit (cr95hf, PROTOCOL_SELECT, buffer);
   return cr95hf_receive(cr95hf, PROTOCOL_SELECT);
//   cr95hf_resp_t ret = cr95hf_receive(cr95hf, PROTOCOL_SELECT);
//   delay(10);
//   return ret;
}

/*!
 * \brief
 *    Send a send-receive command to CR95HF.
 *
 * \param   cr95hf   The CR95HF handle to use.
 * \param   buffer   Span(len-data) with the data to send
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 */
cr95hf_resp_t cr95hf_sendRecv (cr95hf_t *cr95hf, span08_t buffer) {
   transmit (cr95hf, SEND_RECEIVE, buffer);
   return cr95hf_receive(cr95hf, SEND_RECEIVE);
}


/*!
 * \brief
 *    Send a Read register command to CR95HF.
 *
 * \param   cr95hf   The CR95HF handle to use.
 * \param   address  The address of the register to read
 * \param   count    The number of registers to read
 * \param   flags    Whether to increment address after register read
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 */
cr95hf_resp_t cr95hf_rdReg (cr95hf_t *cr95hf, uint8_t address, uint8_t count, uint8_t flags) {
   transmit (cr95hf, READ_REGISTER, span08_mk (cr95hf->tx_buffer, 3, address, count, flags));
   return cr95hf_receive(cr95hf, READ_REGISTER);
}

/*!
 * \brief
 *    Send a Read register command to CR95HF.
 *
 * \param   cr95hf   The CR95HF handle to use.
 * \param   buffer   Span(len-data) with the data to send
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 */
cr95hf_resp_t cr95hf_wrRegv (cr95hf_t *cr95hf, span08_t buffer) {
   transmit (cr95hf, WRITE_REGISTER, buffer);
   return cr95hf_receive(cr95hf, WRITE_REGISTER);
}

/*!
 * \brief
 *    Send an idle command to CR95HF.
 *
 * This function sends the IDLE command and response immediately with DRV_AWAIT (async wait) status.
 * When the CR95HF becomes active, it respond with the IDLE response. The user MUST call
 * `cr95hf_receive(&cr95hf, IDLE)` in order to extract the answer from queue buffer. See also
 * \see cr95hf_wait4tag()
 *
 * \param   cr95hf   The CR95HF handle to use.
 * \param   buffer   Span(len-data) with the data to send
 * \param   callback A pointer to \see cr95hf_callback_ft callback to call when the CR95HF becomes
 *                   active and respond.
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 * \note
 *    In order to set the callback we use queue's trigger functionality
 */
cr95hf_resp_t cr95hf_idle (cr95hf_t *cr95hf, span08_t buffer, cr95hf_callback_ft callback) {
   transmit (cr95hf, IDLE, buffer);

   if (callback) {
      queue08_set_trigger(cr95hf->hal.queue, callback, MORE_EQ, 2);
      return (cr95hf_resp_t){DRV_AWAIT, 0, {0, 0}};
   }
   else {
      return cr95hf_receive(cr95hf, IDLE);
   }
}

// Higher level commands

/*!
 * Transmit a Protocol Select with FIELD_OFF and return the response.
 *
 * \param cr95hf     Handle to cr95hf
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 */
cr95hf_resp_t cr95hf_field_off (cr95hf_t *cr95hf) {
   cr95hf_resp_t r= cr95hf_protocolSelect (cr95hf, span08_mk(cr95hf->tx_buffer, 2, PROTOCOL_CODE_TAG_FIELDOFF, 0));
   delay(50);
   return r;
}

/*!
 * Initialize a valid (synchronized) communication with UART. It transmits as manu ECHO as needed
 * until a valid ECHORESPONCE comes back from CR95HF.
 * \note
 *    See datasheet cr95hf section 4.1 for details
 *
 * \param cr95hf     Handle to cr95hf
 * \return           The status of the operation
 *    \arg           DRV_READY   Synchronization achieved
 *    \arg           DRV_ERROR   CR95HF is not responding :(
 * \warning
 *    A failure on this call is fatal to the rest of communication
 */
drv_status_en cr95hf_sync (cr95hf_t *cr95hf) {
   cr95hf_resp_t resp;
   for (size_t i=0 ; i<564 ; ++i) {
      transmit(cr95hf, ECHO, (span08_t){0, 0});
      delay(2);
      if (queue08_size(cr95hf->hal.queue) >= 1) {
         resp = cr95hf_receive(cr95hf, ECHO);
         if (resp.code == ECHORESPONSE) {
            delay(2);
            queue08_flush(cr95hf->hal.queue);
            return DRV_READY;
         }
      }
   }
   return DRV_ERROR;
}

/*!
 * Initiate an antenna calibration process in blocking mode. The result calibration high and low
 * values are stored for future \see cr95hf_wait4tag() calls.
 *
 * * \warning
 *    This function MUST be called before any \see cr95hf_wait4tag() calls.
 *
 * \param cr95hf     Handle to cr95hf
 * \return           The status of the operation
 *    \arg           DRV_READY   Calibration succeed
 *    \arg           DRV_ERROR   Calibration failed
 */
drv_status_en cr95hf_calibration (cr95hf_t *cr95hf) {
   cr95hf_resp_t resp;
   uint8_t calVal = 0xFC;

   for (uint8_t step =calVal ; step > 0x00 ; step >>= 1) {
      resp = cr95hf_idle (cr95hf, span08_mk(cr95hf->tx_buffer, 14,
            0x03,
            0xA1, 0x00,
            0xF8, 0x01,
            0x18, 0x00,
            0x08,
            0x60,
            0x60,
            0x00, calVal,
            0x3F, 0x01), NULL);
      if (resp.status == DRV_ERROR)
         return DRV_ERROR;
      if (resp.frame.data[0] == 0x02)  calVal += step;   // detect
      else                             calVal -= step;   // timeout (or anything else)
   }
   cr95hf->calLow = calVal - 0x08;
   cr95hf->calHigh= calVal + 0x08;
   return DRV_READY;
}

/*!
 * \brief
 *    Send an IDLE command to CR95HF with wait for tag configuration.
 *
 * This function sends the IDLE command and response immediately with DRV_AWAIT (async wait) status.
 * When the CR95HF becomes active(a tag is detected), it respond with the IDLE response. The user MUST call
 * `cr95hf_receive(&cr95hf, IDLE)` in order to extract the answer from queue buffer.
 *
 * \param   cr95hf   The CR95HF handle to use.
 * \param   buffer   Span(len-data) with the data to send
 * \param   callback A pointer to \see cr95hf_callback_ft callback to call when the CR95HF becomes
 *                   active and respond. Usually to feed a semaphore or signal a task.
 * \return           The CR95HF response using value semantics \see cr95hf_resp_t
 * \note
 *    In order to set the callback we use queue's trigger functionality
 * \code
 * cr95hf_t cr95hf;
 * sem_t s;
 * void ready(void) { sem_post(&s); }
 *
 * int main (void) {
 *    // ...
 *    cr95hf_wait4tag (&cr95hf, ready);
 *    sem_wait(&s);
 *    cr95hf_receive(&cr95hf, IDLE);
 *    // ...
 * }
 * \endcode
 */
cr95hf_resp_t cr95hf_wait4tag (cr95hf_t *cr95hf, cr95hf_callback_ft callback) {
   return cr95hf_idle (cr95hf, span08_mk(cr95hf->tx_buffer, 14,
         0x02,
         0x21, 0x00,
         0x79, 0x01,
         0x18, 0x00,
         0x08,
         0x60,
         0x60,
         cr95hf->calLow, cr95hf->calHigh,
         0x3F, 0x01), callback);
}

