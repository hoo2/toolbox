/*!
 * \file i2c_bb.h
 * \brief
 *    A target independent i2c using bit-banging driver with ACK/NACK support.
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
#ifndef __i2c_bb_h__
#define __i2c_bb_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_types.h>
//#include <toolbox_defs.h>
#include <sys/jiffies.h>
#include <string.h>
#include <stdint.h>

/* ================   General Defines   ====================*/
#define I2C_FREQ_DEF        (100000)           // 100KHz

typedef volatile struct
{
   drv_pinio_ft   sda;
   drv_pinout_ft  scl;
   drv_pindir_ft  sda_dir;
   int            clk_delay;
   drv_status_en  status;
}i2c_bb_t;



/*
 *  ============= PUBLIC ALCD API =============
 */

/*
 * Link and Glue functions
 */
void i2c_link_sda(i2c_bb_t *i2c, drv_pinio_ft sda); /*!< link driver's SDA function*/
void i2c_link_scl (i2c_bb_t *i2c, drv_pinout_ft scl); /*!< link driver's SCL function*/
void i2c_link_sdadir (i2c_bb_t *i2c, drv_pindir_ft pd); /*!< link driver's SDA_dir function*/

/*
 * Set functions
 */
void i2c_set_speed (i2c_bb_t *i2c, uint32_t freq); /*!< set i2c speed */

/*
 * User Functions
 */
void i2c_deinit (i2c_bb_t *i2c);         /*!< for compatibility */
drv_status_en i2c_init (i2c_bb_t *i2c);  /*!< for compatibility */

void i2c_start(i2c_bb_t *i2c);
void i2c_stop (i2c_bb_t *i2c);
byte_t i2c_rx (i2c_bb_t *i2c, uint8_t ack);
int    i2c_tx (i2c_bb_t *i2c, byte_t byte);


drv_status_en i2c_ioctl (i2c_bb_t *i2c, ioctl_cmd_t cmd, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __i2c_bb_h__
