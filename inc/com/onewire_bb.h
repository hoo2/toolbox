/*!
 * \file onewire_bb.h
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
#ifndef __onewire_bb_h__
#define __onewire_bb_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_types.h>
#include <sys/jiffies.h>
#include <string.h>


/* ================   General Defines   ====================*/

typedef  jtime_t    _100nsec_t;     /*!< Use jiffy for timing */

/*!
 * 1-Wire timing structure, to hold waveform timings
 */
typedef struct {
   _100nsec_t  t_brt;   /*!< Bus recovery timeout before reset */
   _100nsec_t  t_rstl;  /*!< Reset low time */
   _100nsec_t  t_rsth;  /*!< Reset high time */
   _100nsec_t  t_msp;   /*!< Reset Master sample pulse */
   _100nsec_t  t_slot;  /*!< Time slot */
   _100nsec_t  t_w1l;   /*!< Write 1 low time */
   _100nsec_t  t_w0l;   /*!< Write 0 low time */
   _100nsec_t  t_rec;   /*!< Recovery time */
   _100nsec_t  t_msr;   /*!< Master sample read */
   uint8_t     state;   /*!< Have timing state flag */
}ow_timings_t;

/*!
 * 1-Wire driver function callback pointers
 * \note
 *    The input and output pin functions can be the same function
 *    depend on the implementation.
 */
typedef struct {
   drv_pinin_ft   in;      /*!< Pointer to 1-Wire bus pin input function */
   drv_pinout_ft  out;     /*!< Pointer to 1-Wire bus pin output function */
   drv_pindir_ft  dir;     /*!< Pointer to 1-Wire bus pin direction function */
   uint8_t        dir_state;  /*!< Flag to hold 1-Wire bus pin direction state */
}ow_io_t;

/*!
 * 1-Wire Data type
 */
typedef struct {
   ow_io_t        io;         /*!< Callback pointers and direction state */
   ow_timings_t   timings;    /*!< Timings */
   drv_status_en  status;     /*!< Toolbox driver status */
}ow_bb_t;



/*
 * ============ Helper Macros =============
 */

/*!
 * 1-Wire speed
 */
#define  OW_BB_T_STANDARD         (0)
#define  OW_BB_T_OVERDRIVE        (1)
/*!
 * Check if the timing is a valid \ref ow_uart_timing_en value
 */
#define  _ow_bb_is_timings(_t_)     ( ((_t_) == OW_BB_T_STANDARD) || ((_t_) == OW_BB_T_OVERDRIVE) )

#define  _ow_have_timings  (0x5A)   /*!< Have timings magic number */
#define  _usec             (10)     /*!< 10 * _100nsec = 1usec */

/*
 *                   Timing Diagram
 * --------------------------------------------------
 * Based on Application Note 126 (www.maxim-ic.com)
 *
 *           ---------                ----       -------------
 * Reset       |      \              /    \     /
 *             |       --------------      -----
 *              <  G ><      H      ><   I   > <  J  >
 *                                            ^
 *                                            |
 *                                       Master Sample
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
 *           ---         - - - - - - - - - - - - -
 * Read         \      /   X  X  X  X  X  X  X  X
 *               ------  - - - - - - - - - - - - -
 *              <  A  ><   E   > <      F     >
 *                              ^
 *                              |
 *                          Master Sample
 *
 * A = t_w1l            H = t_rstl
 * B = t_slot - A       I = t_msp
 * C = t_w0l            J = t_rsth - I
 * D = t_rec
 * E = t_msr - A
 * F = t_slot - A - E
 * G = timeout
 */

/*
 *
 */
#define  _ow_timings_standard(_ow)           \
   do {                                      \
      _ow->timings.t_brt = 1*_usec;          \
      _ow->timings.t_rstl = 480*_usec;       \
      _ow->timings.t_rsth = 480*_usec;       \
      _ow->timings.t_msp = 70*_usec;         \
      _ow->timings.t_slot = 70*_usec;        \
      _ow->timings.t_w1l = 5*_usec;          \
      _ow->timings.t_w0l = 60*_usec;         \
      _ow->timings.t_rec = 10*_usec;         \
      _ow->timings.t_msr = 14*_usec;         \
      _ow->timings.state = _ow_have_timings; \
   } while (0)


#define  _ow_timings_overdrive(_ow)          \
   do {                                      \
      _ow->timings.t_brt = 2.5*_usec;        \
      _ow->timings.t_rstl = 70*_usec;        \
      _ow->timings.t_rsth = 48*_usec;        \
      _ow->timings.t_msp = 8.5*_usec;        \
      _ow->timings.t_slot = 10*_usec;        \
      _ow->timings.t_w1l = 1.4*_usec;        \
      _ow->timings.t_w0l = 7.5*_usec;        \
      _ow->timings.t_rec = 2.5*_usec;        \
      _ow->timings.t_msr = 2.2*_usec;        \
      _ow->timings.state = _ow_have_timings; \
   } while (0)

#define  _ow_time_A(_ow_)     (_ow_->timings.t_w1l)                     /*!< std =  5 usec, over = 1.4 usec */
#define  _ow_time_B(_ow_)     (_ow_->timings.t_slot - _ow_time_A(_ow_)) /*!< std = 65 usec, over = 8.6 usec */
#define  _ow_time_C(_ow_)     (_ow_->timings.t_w0l)                     /*!< std = 60 usec, over = 7.5 usec */
#define  _ow_time_D(_ow_)     (_ow_->timings.t_rec)                     /*!< std = 10 usec, over = 2.5 usec */
#define  _ow_time_E(_ow_)     (_ow_->timings.t_msr - _ow_time_A(_ow_))  /*!< std =  9 usec, over = 0.8 usec */
#define  _ow_time_F(_ow_)     (_ow_->timings.t_slot - \
                               _ow_time_A(_ow_) -     \
                               _ow_time_E(_ow_))                        /*!< std = 56 usec, over = 7.8 usec */
#define  _ow_time_G(_ow_)     (_ow_->timings.t_brt)                     /*!< std =  1 usec, over = 2.5 usec */
#define  _ow_time_H(_ow_)     (_ow_->timings.t_rstl)                    /*!< std =480 usec, over =  70 usec */
#define  _ow_time_I(_ow_)     (_ow_->timings.t_msp)                     /*!< std = 70 usec, over = 8.5 usec */
#define  _ow_time_J(_ow_)     (_ow_->timings.t_rsth - _ow_time_I(_ow_)) /*!< std =410 usec, over =39.5 usec */


/*
 *  ============= PUBLIC ALCD API =============
 */

/*
 * Link and Glue functions
 */
void ow_bb_link_in (ow_bb_t *ow, drv_pinin_ft in);       /*!< link driver's input pin wire function */
void ow_bb_link_out (ow_bb_t *ow, drv_pinout_ft out);    /*!< link driver's output pin wire function */
void ow_bb_link_dir (ow_bb_t *ow, drv_pindir_ft dir);    /*!< link driver's pin wire direction function*/

/*
 * Set functions
 */
void ow_bb_set_timing (ow_bb_t *ow, uint32_t owt);       /*!< set 1-wire timing mode */

/*
 * User Functions
 */
void ow_bb_deinit (ow_bb_t *ow);             /*!< for compatibility */
drv_status_en ow_bb_init (ow_bb_t *ow);      /*!< for compatibility */

drv_status_en
         ow_bb_reset (ow_bb_t *ow);
uint8_t  ow_bb_rx (ow_bb_t *ow);
void     ow_bb_tx (ow_bb_t *ow, byte_t byte);
uint8_t  ow_bb_rw (ow_bb_t *ow, byte_t byte);
drv_status_en
         ow_bb_search (ow_bb_t *ow, uint8_t *romid);

drv_status_en ow_bb_ioctl (ow_bb_t *ow, ioctl_cmd_t cmd, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   /* #ifndef __onewire_bb_h__ */
