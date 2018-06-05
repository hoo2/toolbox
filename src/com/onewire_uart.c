/*!
 * \file onewire_uart.c
 * \brief
 *    A target independent 1-wire implementation using a microprocessor's uart
 *    for bit timing
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Choutouridis Christos (http://www.houtouridis.net)
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
#include <com/onewire_uart.h>

/*
 * ========= Private helper macros ===========
 */

/*!
 * Clear a virtual 64bit unsigned register.
 * \note
 *    We use a uint8_t array[8] to represent that kind
 *    of data for this module
 * \param   _reg  Pointer to register location. Usual the name of
 *                The array
 */
#define  _clear_u64(_reg_)                      \
   memset ((void*)(_reg_), 0, 8*sizeof(uint8_t))

/*!
 * Read a bit value of a virtual 64bit unsigned register.
 * \note
 *    We use a uint8_t array[8] to represent that kind
 *    of data for this module
 * \param   _reg  Pointer to register location. Usual the name of
 *                The array
 * \param   _bit  The bit location we want to read
 */
#define  _read_bit_u64(_reg_, _bit_)            \
   (_reg_[(_bit_)/8] & (1<<((_bit_)%8))) ? 1:0

/*!
 * Write/modify a bit value from a virtual 64bit unsigned register.
 * \note
 *    We use a uint8_t array[8] to represent that kind
 *    of data for this module
 * \param   _reg  Pointer to register location. Usual the name of
 *                The array
 * \param   _bit  The bit location we want to set
 * \param   _v    The value we want to set
 *    \arg  0     Set to zero
 *    \arg  !0    Set to One
 */
#define  _write_bit_u64(_reg_, _bit_, _v_)               \
   do {                                                  \
      if (_v_) _reg_[(_bit_)/8] |=   1 << ((_bit_)%8);   \
      else     _reg_[(_bit_)/8] &= ~(1 << ((_bit_)%8));  \
   } while (0)



/*
 * ============= Private functions ===========
 */
/* Data manipulation functions */
static int _cmp_u64 (uint8_t *reg1, uint8_t *reg2);

/* Set functions */
static drv_status_en _set_baudrate (ow_uart_t *ow, ow_uart_state_en st);

/* Bus functions */
static uint8_t _write_bit (ow_uart_t *ow, uint8_t b);
static uint8_t _read_bit (ow_uart_t *ow);


/*!
 * \brief
 *    Compare two 64bit virtual unsigned registers
 * \note
 *    We use a uint8_t array[8] to represent that kind
 *    of data for this module
 * \param   reg1  Pointer to first register location. Usual the name of
 *                the array
 * \param   reg1  Pointer to 2nd register location. Usual the name of
 *                the array
 * \return  The comparison result
 *    \arg  0     Registers are equal
 *    \arg  1     Register 1 is larger than register 2
 *    \arg  -1    Register 2 is larger than register 1
 */
static __O3__ int _cmp_u64 (uint8_t *reg1, uint8_t *reg2) {
   int i;
   for (i=7 ; i>=0 ; --i) {
      if (reg1[i] > reg2[i])        return 1;   /* reg1 > reg2 */
      else if (reg1[i] < reg2[i])   return -1;  /* reg1 < reg2 */
   }
   return 0;   /* reg1 equal reg2 */
}

/*!
 * \brief
 *    Set UART Baudrate and handle all function calls and data
 *    manipulation
 * \param   ow    Pointer to select 1-Wire structure for the operation.
 * \param   st    The 1-Wire operational state \sa ow_uart_state_en
 * \return        The status of the operation
 *    \arg  DRV_ERROR   Could not set baudrate (callback pointer function error)
 *    \arg  DRV_READY   Success
 */
static __O3__ drv_status_en _set_baudrate (ow_uart_t *ow, ow_uart_state_en st)
{
   uint32_t       st_br;

   /* Get the desired baudrate */
   switch (st) {
      case OWS_RESET:   st_br = ow->baudrate.reset;   break;
      default:
      case OWS_OPER:    st_br = ow->baudrate.oper;    break;
   }

   if (ow->baudrate.current != st_br) {
      if (ow->io.br (st_br) != DRV_READY)    return DRV_ERROR;
      ow->baudrate.current = st_br;
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Send a 1-Wire write bit
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
 * \param   ow    Pointer to select 1-Wire structure for the operation.
 * \param   b     The bit to send
 * \return        Status of the operation
 *    \arg  0     Success
 *    \arg  1     Fail
 */
static uint8_t _write_bit (ow_uart_t *ow, uint8_t b)
{
   uint8_t  w;
   uint16_t r;

   /*
    * Make sure we are at the right baudrate
    */
   if (_set_baudrate (ow, OWS_OPER) != DRV_READY)
      return 1;

   /* Select frame to send and check the bus by evaluating the return value */
   w = (b) ? 0xFF : 0x00;
   r = ow->io.rw (w);

   if (r != w)    return 1;
   else           return 0;
}

/*!
 * \brief
 *    Read a bit from the 1-Wire bus, return it and provide
 *    the recovery time.
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
 * \return  The answer
 *    \arg  0  Read 0
 *    \arg  1  Read 1 (This is also returned on transition error).
 */
static uint8_t _read_bit (ow_uart_t *ow)
{
   uint16_t  r;

   /*
    * Make sure we are at the right baudrate
    */
   if (_set_baudrate (ow, OWS_OPER) != DRV_READY)
      return 1;

   /* Send frame for read */
   r = ow->io.rw (0xFF);

   /* Dispatch answer */
   if (r < 0xFF)  return 0;
   else           return 1;
}


/*
 *  ============= PUBLIC 1-Wire API =============
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief   link driver's UART read-write function
 * \param   ow    pointer to select 1-Wire structure for the operation.
 * \param   tx    ow_uart_rx_ft pointer to drivers UART rx function
 */
void ow_uart_link_rw (ow_uart_t *ow, ow_uart_rw_ft rw) {
   ow->io.rw = (ow_uart_rw_ft)((rw != 0) ? rw : 0);
}

/*!
 * \brief   link driver's UART baudrate function
 * \param   ow    pointer to select 1-Wire structure for the operation.
 * \param   tx    ow_uart_tx_ft pointer to drivers UART baudrate function
 */
void ow_uart_link_br (ow_uart_t *ow, ow_uart_br_ft br) {
   ow->io.br = (ow_uart_br_ft)((br != 0) ? br : 0);
}


/*
 * Set functions
 */

/*!
 * \brief  set 1-wire timing mode and update baudrate table.
 *          If the owt parameter is not a valid ow_uart_timing_en
 *          then set timings to OW_STANDTARD.
 * \param  ow     pointer to select 1-Wire structure for the operation.
 * \param  owt    Timing type
 *    \arg  OWT_STANDARD      Use standard timing
 *    \arg  OWT_OVERDRIVE     Use overdrive timing
 */
void ow_uart_set_timing (ow_uart_t *ow, uint32_t owt) {
   ow->timing = (_ow_uart_is_timings(owt)) ? owt : OW_UART_T_STANDARD;

   switch (owt) {
      case OW_UART_T_STANDARD:   _ow_baudrate_standard (ow->baudrate);  break;
      case OW_UART_T_OVERDRIVE:  _ow_baudrate_overdrive (ow->baudrate); break;
   }
}

/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initialize the 1-Wire interface and leave bus pin in input state
 * \param  ow    pointer to select 1-Wire structure for the operation.
 * \return none
 */
void ow_uart_deinit (ow_uart_t *ow)
{
   // Clear data
   memset ((void*)ow, 0, sizeof (ow_uart_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialize the 1-Wire interface and leave bus high
 * \param  ow    pointer to select 1-Wire structure for the operation.
 * \return The driver status after init.
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ow_uart_init (ow_uart_t *ow)
{
   // Check requirements
   if (!ow->io.rw)      return ow->status = DRV_ERROR;
   if (!ow->io.br)      return ow->status = DRV_ERROR;

   // Init the bus
   ow->status = DRV_BUSY;
   if ( _ow_uart_is_timings(ow->timing) != 1) {
      ow->timing = OW_UART_T_STANDARD;
      _ow_baudrate_standard (ow->baudrate);
   }
   switch (ow->timing) {
      case OW_UART_T_STANDARD:   _ow_baudrate_standard (ow->baudrate);  break;
      case OW_UART_T_OVERDRIVE:  _ow_baudrate_overdrive (ow->baudrate); break;
   }
   if (_set_baudrate (ow, OWS_RESET) != DRV_READY)
      return ow->status = DRV_ERROR;
   return ow->status = DRV_READY;
}

/*!
 * \brief
 *    Generate a 1-wire reset
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
 * \note    Does not handle alarm presence from DS2404/DS1994
 * \param   None
 * \return  The status of the operation
 *    \arg  DRV_ERROR      Error, callback baudrate error or bus error
 *    \arg  DRV_NODEV      If no presence detect was found
 *    \arg  DRV_READY      Otherwise
 */
drv_status_en ow_uart_reset (ow_uart_t *ow)
{
   uint8_t  w;
   uint16_t r;

   /*
    * Make sure we are at the write baudrate
    */
   if (_set_baudrate (ow, OWS_RESET) != DRV_READY)
      return DRV_ERROR;

   /* Select frame to send */
   w = (ow->timing == OW_UART_T_OVERDRIVE) ? 0xF8 : 0xF0;

   r = ow->io.rw (w);
   r = ow->io.rw (w);   // Send it twice to make sure

   if (w>r)       return DRV_READY;
   else if (w==r) return DRV_NODEV;
   else           return DRV_ERROR;
}

/*!
 * \brief
 *    Read a byte from 1-Wire bus
 * \param  ow    pointer to select 1-Wire structure for the operation.
 * \return  The byte received.
 */
uint8_t ow_uart_rx (ow_uart_t *ow)
{
   uint8_t  i;
   byte_t byte;

   for (i=8, byte=0 ; i>0 ; --i) {
      byte >>= 1;       /* shift bits to right as LSB comes first */
      if (_read_bit (ow) != 0)  byte |= 0x80;  /* Mask bit to MSB */
   }
   return byte;
}

/*!
 * \brief
 *    Write a byte to 1-Wire bus
 * \param  ow    pointer to select 1-Wire structure for the operation.
 * \param  b:    The byte to write
 * \return  none
 */
void ow_uart_tx (ow_uart_t *ow, byte_t byte)
{
   uint8_t  i;

   for (i=8 ; i>0 ; --i) {
      _write_bit (ow, byte & 0x01);    /* Send LSB */
      byte >>= 1;           /* shift bits to right */
   }
}

/*!
 * \brief
 *    Write a byte to 1-Wire bus and read the response
 * \param  ow    Pointer to select 1-Wire structure for the operation.
 * \param   byte  The byte to write
 * \return  The byte received.
 */
uint8_t ow_uart_rw (ow_uart_t *ow, byte_t byte)
{
   uint8_t  i;
   byte_t ret;

   for (i=8, ret=0 ; i>0 ; --i) {
      ret >>= 1;     /* shift read bits to right as LSB comes first */
      if ((byte & 0x01) != 0)    ret |= (_read_bit (ow) != 0) ? 0x80 : 0x0;
      else                       _write_bit (ow, 0);
      byte >>= 1;    /* shift bits to right */

      /*!<
       * If the bit is 1 we use the read sequence, as it has the same
       * waveform with write-1 and we get the slave response
       * If the bit is 0, we can not read the slave response so we just write-0
       */
   }
   return byte;
}


/*!
 * \brief
 *    1-Wire search algorithm based on maxim-ic application note 187
 *
 * \param   ow       Pointer to select 1-Wire structure for the operation.
 * \param   romid    Pointer to romid to return. If the search is success
 *                   this points to and 64bit long array with ROM ID
 * \return  The status of the search
 *    \arg  DRV_NODEV (-1) Search was failed, No device found
 *    \arg  DRV_READY (1)  Search is complete, all ROM IDs was found. This was the last
 *    \arg  DRV_BUSY  (2)  Search is succeed, plus there are more ROM IDs to found
 *    \arg  DRV_ERROR (3)  Search failed, Reading error
 */
drv_status_en ow_uart_search (ow_uart_t *ow, uint8_t *romid)
{
   static uint8_t dec[8];  /*!<
                            * Hold the algorithm's select bit when a discrepancy
                            * is detected. We use this variable to navigate to the
                            * ROM tree as we store the path we take each time (0-1).
                            * Each bit represent a bit position in the ROM ID.
                            */
   static uint8_t pos[8];  /*!<
                            * Hold the discrepancy position. We use this variable to
                            * navigate to the ROM tree as we store the crossroads(1) we encounter.
                            * Each bit represent a bit position in the ROM ID.
                            */
   uint8_t i, cur[8];      /* Current pass bit position, in a pos[8] like representation of it */
   uint8_t  b, b1, b2;     /* bit helper vars */

   if (ow_uart_reset (ow) != DRV_READY)
      return DRV_NODEV;
   ow_uart_tx (ow, 0xF0);    /* Issue search command */

   for (i=0 ; i<64 ; ++i) {
      /* Get response pair bits */
      b1 = _read_bit (ow);
      b2 = _read_bit (ow);

      switch (b1 | (b2<<1)) {
         case 0x00:  /* 00 - We have discrepancy */
            _write_bit_u64 (cur, i, 1);
            switch (_cmp_u64 (pos, cur)) {
               default:
               case -1: b = 0;
                        _write_bit_u64 (pos, i, 1);   break;
               case 0:  b = 1;
                        _write_bit_u64 (dec, i, 1);   break;
               case 1:  b = _read_bit_u64 (dec, i);   break;
               /*<
                * -1) pos < cur: This discrepancy is the most far for now.
                *       Mark position and select 0.
                *  0) pos == cur: This was the last discrepancy in the last pass.
                *       Select the other branch this time (1).
                *  1) pos > cur: We had a discrepancy in a MSB than that, in a previous pass.
                *       Continue with the last pass decision.
                */
            }

            /* Send selection and update romid */
            _write_bit (ow, b);
            _write_bit_u64 (romid, i, b);
            break;

         case 0x01:  /* 01 - All bits of all ROMs are 1s */
            _write_bit (ow, 1);
            _write_bit_u64 (romid, i, 1);   break;

         case 0x02:  /* 10 - All bits of all ROMs are 0s */
            _write_bit (ow, 0);
            _write_bit_u64 (romid, i, 0);   break;

         default:
         case 0x03:  /* 11 - No device on the bus */
            _clear_u64 (romid);
            return DRV_NODEV;
      }  /* switch (b1 | (b2<<1)) */
   }  /* for */

   switch (_cmp_u64 (dec, pos)) {
      case -1: return DRV_BUSY;
      case 0:  _clear_u64 (dec);
               _clear_u64 (pos);
               return DRV_READY;
      default:
      case 1:  _clear_u64 (dec);
               _clear_u64 (pos);
               return DRV_ERROR;
      /*<
       * -1) des < pos:  We have more leafs(ROMs) to found
       *  0) des == pos: We have found all the leafs(ROMs)
       *  1) des > pos:  We have more decision that discrepancies ?!?!?, Error.
       */
   }
}

/*!
 * \brief
 *    1-Wire ioctl function
 *
 * \param  ow    pointer to select 1-Wire structure for the operation.
 * \param  cmd    specifies the command to 1-Wire and get back the replay.
 *    \arg CTRL_GET_STATUS
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg CTRL_SEARCH
 *    \arg CTRL_RESET
 * \param  buf    pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg  DRV_ERROR      Error
 *    \arg  DRV_NODEV      If no presence detect was found
 *    \arg  DRV_READY      Success
 * When the command is CTRL_SEARCH then return the status of the search which is the same
 * plus DRV_BUSY. The meaning of the return status is now:
 *    \arg  DRV_NODEV (-1) Search was failed, No device found
 *    \arg  DRV_READY (1)  Search is complete, all ROM IDs was found. This was the last
 *    \arg  DRV_BUSY  (2)  Search is succeed, plus there are more ROM IDs to found
 *    \arg  DRV_ERROR (3)  Search failed, Reading error
 */
__Os__ drv_status_en ow_uart_ioctl (ow_uart_t *ow, ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   switch (cmd) {
      case CTRL_GET_STATUS:      /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = ow->status;
         return DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
         ow_uart_deinit (ow);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = ow_uart_init (ow);
         else
            ow_uart_init (ow);
         return DRV_READY;
      case CTRL_SEARCH:         /*!< Search */
         return ow_uart_search (ow, (uint8_t*)buf);
      case CTRL_RESET:
         return ow_uart_reset (ow);
      default:                  /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}


#undef  _clear_u64
#undef  _read_bit_u64
#undef  _write_bit_u64
#undef  _ow_is_timings
