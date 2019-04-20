/*!
 * \file onewire_bb.c
 * \brief
 *    A target independent 1-wire using bit-banging implementation driver.
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
#include <com/onewire_bb.h>

/*
 * ========= Private helper macros ===========
 */

#define  _delay(_time_)    jf_delay_100ns (_time_)    /*!< Delay using 100nsec time base */
#define  _waiting(_time_)  jf_check_100nsec (_time_)  /*!< Polling version delay using 100nsec time base */


/*!
 * \brief
 *    Change bus pin direction to output  macro, to ensure we wont forget
 *    to mark the dir_state every time, we change direction
 * \note
 *    The order of the operations is reversed from the one used for input.
 *    This is to improve timing in the 1-Wire module
 * \param   _ow_     Pointer to select 1-Wire structure for the operation.
 */
#define  _ow_dir_output(_ow_)                \
   do {                                      \
      _ow_->io.dir_state = drv_pin_output;   \
      _ow_->io.dir (drv_pin_output);         \
   } while (0)

/*!
 * \brief
 *    Change bus pin direction to input  macro, to ensure we wont forget
 *    to mark the dir_state every time, we change direction
 * \note
 *    The order of the operations is reversed from the one used for output.
 *    This is to improve timing in the 1-Wire module
 * \param   _ow_     Pointer to select 1-Wire structure for the operation.
 */
#define  _ow_dir_input(_ow_)              \
   do {                                   \
      _ow_->io.dir (drv_pin_input);       \
      _ow_->io.dir_state = drv_pin_input; \
   } while (0)

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

/* Bus functions */
static void _write_bit (ow_bb_t *ow, uint8_t b);
static uint8_t _read_bit (ow_bb_t *ow);


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
 *    Send a 1-Wire write bit and provide the recovery time
 *
 *     M       set     release
 *     S                      sample
 *
 *           ---        --------------------------
 * Write 1      \      /
 *               ------
 *              <  A  ><          B           >
 *
 *           ---                          --------
 * Write 0      \                        /
 *               ------------------------
 *              <         C             ><  D >
 *
 * \param   ow    Pointer to select 1-Wire structure for the operation.
 * \param   b     The bit to send
 * \return  None
 */
static __O3__ void _write_bit (ow_bb_t *ow, uint8_t b)
{
   /*
    * Drive low but first make sure bus is configured as output
    */
   if (ow->io.dir_state != drv_pin_output) { ow->io.out (0);
                                             _ow_dir_output (ow);
   }
   else {                                    ow->io.out(0);
   }
   /*
    * Select timing and release the bus by keeping the pin in output mode
    */
   switch (b) {
      case 0:
         _delay (_ow_time_C (ow));
         ow->io.out (1);
         _delay (_ow_time_D (ow));
         break;

      default: /* Act as C does, anything none zero is true */
      case 1:
         _delay (_ow_time_A (ow));
         ow->io.out (1);
         _delay (_ow_time_B (ow));
         break;
   }
}

/*!
 * \brief
 *    Read a bit from the 1-Wire bus, return it and provide
 *    the recovery time.
 *
 *     M       set     rel    sample
 *     S                s/r
 *
 *           ---         - - - - - - - - - - - - -
 * Read         \      /   X X X X X X X X X X X X
 *               ------  - - - - - - - - - - - - -
 *              <  A  ><   E   > <      F      >
 *
 * \param   ow    Pointer to select 1-Wire structure for the operation.
 * \return  None
 */
static __O3__ uint8_t _read_bit (ow_bb_t *ow)
{
   uint8_t r;

   /*
    * Drive low but first make sure bus is configured as output
    */
   if (ow->io.dir_state != drv_pin_output) { ow->io.out (0);
                                             _ow_dir_output (ow);
   }
   else {                                    ow->io.out(0);
   }
   _delay (_ow_time_A (ow));        /* Keep read timing */

   ow->io.out (1);                  /* Release to read after a while */
   _waiting (_ow_time_E (ow));
   _ow_dir_input (ow);
   while  (_waiting (_ow_time_E (ow)));

   r = ow->io.in ();                /* Sample the bus */

   _waiting (_ow_time_F (ow));      /* Leave bus as output - released */
   _ow_dir_output (ow);
   while  (_waiting (_ow_time_F (ow)));
   return r;
}


/*
 *  ============= PUBLIC 1-Wire API =============
 */

/*
 * Link and Glue functions
 */

/*!
 * \brief   link driver's input pin wire function
 * \param   ow    pointer to select 1-Wire structure for the operation.
 * \param   wire  drv_pinin_ft pointer to drivers 1-Wire bus pin I/O function
 */
void ow_bb_link_in (ow_bb_t *ow, drv_pinin_ft in) {
   ow->io.in = (drv_pinin_ft)((in != 0) ? in : 0);
}

/*!
 * \brief   link driver's output pin wire function
 * \param   ow    Pointer to select 1-Wire structure for the operation.
 * \param   wire  drv_pinout_ft pointer to drivers 1-Wire bus pin I/O function
 */
void ow_bb_link_out (ow_bb_t *ow, drv_pinout_ft out) {
   ow->io.out = (drv_pinout_ft)((out != 0) ? out : 0);
}
/*!
 * \brief   link driver's pin wire direction function
 * \param   ow    pointer to select 1-Wire structure for the operation.
 * \param   dir   drv_pindir_ft pointer to drivers 1-Wire bus pin direction configuration function
 */
void ow_bb_link_dir (ow_bb_t *ow, drv_pindir_ft dir) {
   ow->io.dir = (drv_pindir_ft)((dir != 0) ? dir : 0);
}

/*
 * Set functions
 */

/*!
 * \brief  set 1-wire timing mode
 * \param  ow     pointer to select 1-Wire structure for the operation.
 * \param  owt    Timing type
 *    \arg  OW_BB_T_STANDARD      Use standard timing
 *    \arg  OW_BB_T_OVERDRIVE     Use overdrive timing
 */
void ow_bb_set_timing (ow_bb_t *ow, uint32_t owt)
{
   switch (owt) {
      default:
      case OW_BB_T_STANDARD:   _ow_timings_standard (ow);    break;
      case OW_BB_T_OVERDRIVE:  _ow_timings_overdrive (ow);   break;
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
void ow_bb_deinit (ow_bb_t *ow)
{
   // Free bus
   _ow_dir_input (ow);

   // Clear data
   memset ((void*)ow, 0, sizeof (ow_bb_t));
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
drv_status_en ow_bb_init (ow_bb_t *ow)
{
   // Check requirements
   //if (!ow->io.dir)     return ow->status = DRV_ERROR;
   if (!ow->io.in)      return ow->status = DRV_ERROR;
   if (!ow->io.out)     return ow->status = DRV_ERROR;
   if (jf_probe () != DRV_READY)
      return ow->status = DRV_ERROR;

   // Init the bus
   ow->status = DRV_BUSY;
   ow->io.out (1);
   _ow_dir_output (ow);
   if (ow->timings.state != _ow_have_timings)
      _ow_timings_standard (ow);
   return ow->status = DRV_READY;
}

/*!
 * \brief
 *    Generate a 1-wire reset
 *
 *     M              set           release
 *     S                                 set   release
 *           ---------                ----       -------------
 * Reset       |      \              /    \     /
 *             |       --------------      -----
 *              <  G ><      H      ><   I  > <  J   >
 *                                           ^
 *                                           |
 *                                       Master Sample
 *
 * \note    Does not handle alarm presence from DS2404/DS1994
 * \param   None
 * \return  The status of the operation
 *    \arg  DRV_ERROR      Error, the bus is reserved
 *    \arg  DRV_NODEV      If no presence detect was found
 *    \arg  DRV_READY      Otherwise
 */
__O3__ drv_status_en ow_bb_reset (ow_bb_t *ow)
{
   drv_status_en  r;

   /*
    * Release the bus and poll for high state
    */
   if (ow->io.dir_state != drv_pin_output)
      { ow->io.out (1); _ow_dir_output (ow); }
   else
      { ow->io.out(1); }
   do
      if ((r = ow->io.in ()) == 1)   break;
   while (_waiting (_ow_time_G (ow))) ;

   if (r != 1)
      return DRV_ERROR;
   else {
      ow->io.out (0);               /* Drive low */
      _delay (_ow_time_H (ow));

      ow->io.out (1);               /* Release the bus */

      _waiting (_ow_time_I (ow));   /* Change direction to input while waiting */
      _ow_dir_input (ow);
      while  (_waiting (_ow_time_I (ow))) ;

      r = ow->io.in ();             /* Sample the slave's response, if any */

      _waiting (_ow_time_J (ow));   /* Leave bus as output - released while waiting */
      _ow_dir_output (ow);
      while  (_waiting (_ow_time_J (ow))) ;
      return (r == 0) ? DRV_READY : DRV_NODEV;
   }
}

/*!
 * \brief
 *    Read a byte from 1-Wire bus
 * \param  ow    pointer to select 1-Wire structure for the operation.
 * \return  The byte received.
 */
uint8_t ow_bb_rx (ow_bb_t *ow)
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
void ow_bb_tx (ow_bb_t *ow, byte_t byte)
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
uint8_t  ow_bb_rw (ow_bb_t *ow, byte_t byte)
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
drv_status_en ow_bb_search (ow_bb_t *ow, uint8_t *romid)
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

   if (ow_bb_reset (ow) != DRV_READY)
      return DRV_NODEV;
   ow_bb_tx (ow, 0xF0);    /* Issue search command */

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
 * \param  buf    pointer to buffer for ioctl
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en ow_bb_ioctl (ow_bb_t *ow, ioctl_cmd_t cmd, ioctl_buf_t buf)
{
   switch (cmd) {
      case CTRL_GET_STATUS:      /*!< Probe function */
         if (buf)
            *(drv_status_en*)buf = ow->status;
         return DRV_READY;
      case CTRL_DEINIT:          /*!< De-init */
         ow_bb_deinit (ow);
         return DRV_READY;
      case CTRL_INIT:            /*!< Init */
         if (buf)
            *(drv_status_en*)buf = ow_bb_init (ow);
         else
            ow_bb_init (ow);
         return DRV_READY;
      case CTRL_SEARCH:         /*!< Search */

      default:                  /*!< Unsupported command, error */
         return DRV_ERROR;

   }
}

#undef  _delay
#undef  _waiting
#undef  _ow_dir_output
#undef  _ow_dir_input
#undef  _clear_u64
#undef  _read_bit_u64
#undef  _write_bit_u64
#undef  _usec
#undef _ow_is_timings
