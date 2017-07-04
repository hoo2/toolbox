/*!
 * \file mcp4728.h
 * \brief
 *    A target independent MCP4728 I2C DAC driver
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2017 Houtouridis Christos (http://www.houtouridis.net)
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
#ifndef __mcp4728_h__
#define __mcp4728_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <com/i2c_bb.h>

/*
 * ================   User defines  ==================
 */
#define MCP4728_READ_ADDRESS_TRIES     (4)
#define MCP4728_WRITE_ADDRESS_TRIES    (4)

/*
 * ================   General Defines   ====================
 */
// Bit fields
#define MCP4728_WRITE                  (0x0)
#define MCP4728_READ                   (0x1)

#define MCP4728_UDAC_UPDATE            (0x00)
#define MCP4728_UDAC_NO_UPDATE         (0x01)

#define MCP4728_ADDRESS_MASK           (0xC0)      /*!< [ 1   1   0   0  A2  A1  A0 R/nW] */


// Commands Masks
#define MCP4728_GEN_RESET              (0x06)
#define MCP4728_GEN_WAKE_UP            (0x09)
#define MCP4728_GEN_SOFT_UPDATE        (0x08)
#define MCP4728_GEN_READ_ADD           (0x0C)
#define MCP4728_GEN_RA_VALID_PATTERN   (0x10)      /*!< [A2   A1   A0    1   A2   A1   A0    0] */
#define MCP4728_GEN_RA_VALID_MASK      (0x11)      /*!< we check the bit0 and bit4 --> 0x11 */
#define MCP4728_GEN_RA_EEPROM_MASK     (0xE0)
#define MCP4728_GEN_RA_DACREG_MASK     (0x0E)

#define MCP4728_FAST_WRITE             (0x00)      /*!< [ 0    0  PD1  PD0  D11  D10   D9   D8] ... */
#define MCP4728_MULTI_WRITE            (0x40)      /*!< [ 0    1    0    0    0 DAC1 DAC0 UDAC] */
#define MCP4728_SEQ_WRITE              (0x50)      /*!< [ 0    1    0    1    0 DAC1 DAC0 UDAC] */
#define MCP4728_SINGLE_WRITE           (0x58)      /*!< [ 0    1    0    1    1 DAC1 DAC0 UDAC] */
#define MCP4728_ADD_WRITE              (0x60)      /*!< [ 0    1    1   A2   A1   A0    *    *] */
#define MCP4728_VREF_WRITE             (0x80)      /*!< [ 1    0    0    x   VA   VB   VC   VD] */
#define MCP4728_PWR_WRITE              (0xA0)      /*!< [ 1    0    1    x PDA1 PDA0 PDB1 PDB0] ... */
#define MCP4728_GAIN_WRITE             (0xC0)      /*!< [ 1    1    0    x   GA   GB   GC   GD] */

// IOCTL commands
#define MCP_CTRL_WAKEUP                (0x0100)
#define MCP_CTRL_SOFT_UPDATE           (0x0101)
#define MCP_CTRL_READ_ADD              (0x0102)
#define MCP_CTRL_FAST_WRITE            (0x0103)
#define MCP_CTRL_MULTI_WRITE           (0x0104)
#define MCP_CTRL_SEQ_WRITE             (0x0105)
#define MCP_CTRL_ADD_WRITE             (0x0106)
#define MCP_CTRL_VREF_WRITE            (0x0107)
#define MCP_CTRL_PWR_WRITE             (0x0108)
#define MCP_CTRL_GAIN_WRITE            (0x0109)

/*
 * ============ Data types ============
 */

typedef enum {
   MCP4728_CH_A   = 0x00,
   MCP4728_CH_B   = 0x01,
   MCP4728_CH_C   = 0x02,
   MCP4728_CH_D   = 0x03,
   MCP4728_CH_ALL = 0x04
}mcp4728_channel_en;


typedef enum {
   MCP4728_VREF_Ext = 0,     //!< Vref = VDD
   MCP4728_VREF_Int          //!< Vref = 2.048 [Volt]
}mcp4728_vref_en;

typedef enum {
   MCP4728_PD_Normal = 0x00,
   MCP4728_PD_1k     = 0x01,
   MCP4728_PD_100k   = 0x02,
   MCP4728_PD_500k   = 0x03
}mcp4728_pwr_en;

typedef enum {
   MCP4728_GAIN_x1 = 0,     //!< Vref = VDD
   MCP4728_GAIN_x2          //!< Vref = 2.048 [Volt]
}mcp4728_gain_en;


typedef struct {
   void*             i2c;        /*!< void I2C type structure - NULL for hardware I2C */
   drv_i2c_rx_ft     i2c_rx;     /*!< I2C read function */
   drv_i2c_tx_ft     i2c_tx;     /*!< I2C write function */
   drv_i2c_ioctl_ft  i2c_ioctl;  /*!< I2C ioctl function */
   drv_pinout_ft     ldac;
   drv_pinin_ft      bsy;
}mcp4728_io_t;

typedef struct {
   uint8_t           cur_addr;   /*!< I2C hardware address */
   uint8_t           usr_add;
   mcp4728_vref_en   vref[4];    /*!<  */
   mcp4728_pwr_en    pwr[4];     /*!<  */
   mcp4728_gain_en   gain[4];    /*!<  */
   uint32_t          timeout;    /*!<  */
}mcp4728_conf_t;

/*!
 * \brief
 *    mcp4728 data struct
 */
typedef struct {
   int16_t              vout[4];
   mcp4728_io_t         io;
   mcp4728_conf_t       conf;
   drv_status_en        status;
}mcp4728_t;


/*
 * ============ Public MCP4728 API ============
 */

/*
 * Link and Glue functions
 */
void mcp4728_link_i2c (mcp4728_t *mcp, void* i2c);
void mcp4728_link_i2c_rx (mcp4728_t *mcp, drv_i2c_rx_ft fun);
void mcp4728_link_i2c_tx (mcp4728_t *mcp, drv_i2c_tx_ft fun);
void mcp4728_link_i2c_ioctl (mcp4728_t *mcp, drv_i2c_ioctl_ft fun);
void mcp4728_link_ldac (mcp4728_t *mcp, drv_pinout_ft fun);
void mcp4728_link_bsy (mcp4728_t *mcp, drv_pinin_ft fun);

/*
 * Set functions
 */
void mcp4728_set_address (mcp4728_t *mcp, uint8_t add);
void mcp4728_set_vref (mcp4728_t *mcp, mcp4728_channel_en ch, mcp4728_vref_en vref);
void mcp4728_set_pwr (mcp4728_t *mcp, mcp4728_channel_en ch, mcp4728_pwr_en vref);
void mcp4728_set_gain (mcp4728_t *mcp, mcp4728_channel_en ch, mcp4728_gain_en vref);
void mcp4728_set_timeout (mcp4728_t *mcp, uint32_t to);

/*
 * User Functions
 */
void mcp4728_deinit (mcp4728_t *mcp);
drv_status_en mcp4728_init (mcp4728_t *mcp);

drv_status_en mcp4728_ch_write (mcp4728_t *mcp, mcp4728_channel_en ch, int16_t *vout);
drv_status_en mcp4728_ch_save (mcp4728_t *mcp, mcp4728_channel_en ch, int16_t *vout);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef __mcp4728_h__ */
