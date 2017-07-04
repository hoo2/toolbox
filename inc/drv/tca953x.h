/*!
 * \file tca953x.h
 * \brief
 *    A target independent TCA953x I2C expander driver
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
#ifndef __tca953x_h__
#define __tca953x_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <com/i2c_bb.h>


/*
 * ================   General Defines   ====================
 */
#define TCA953x_WRITE     (0x0)
#define TCA953x_READ      (0x1)

#define TCA953x_INPUT_PORT_0           (0x00)
#define TCA953x_INPUT_PORT_1           (0x01)
#define TCA953x_OUTPUT_PORT_0          (0x02)
#define TCA953x_OUTPUT_PORT_1          (0x03)
#define TCA953x_POLARITY_PORT_0        (0x04)
#define TCA953x_POLARITY_PORT_1        (0x05)
#define TCA953x_CONF_PORT_0            (0x06)
#define TCA953x_CONF_PORT_1            (0x07)

#define TCA953x_POL_TRUE               (0)
#define TCA953x_POL_INVERTED           (1)

#define TCA953x_CONF_INPUT             (1)
#define TCA953x_CONF_OUTPUT            (0)

#define TCA953x_ADDRESS_MASK           (0xE8)      //!< [ 1   1   1   0   1  A1  A2 R/nW]
/*
 * ============ Data types ============
 */
typedef enum {
   TCA953x_PORT_0 = 0,
   TCA953x_PORT_1
}tca953x_port_en;

typedef enum {
   TCA953x_PIN_NONE = 0x00,
   TCA953x_PIN_0 = 0x01,
   TCA953x_PIN_1 = 0x02,
   TCA953x_PIN_2 = 0x04,
   TCA953x_PIN_3 = 0x08,
   TCA953x_PIN_4 = 0x10,
   TCA953x_PIN_5 = 0x20,
   TCA953x_PIN_6 = 0x40,
   TCA953x_PIN_7 = 0x80,
   TCA953x_PIN_ALL = 0xFF
}tca953x_pin_en;

typedef struct {
   void*             i2c;        /*!< void I2C type structure - NULL for hardware I2C */
   drv_i2c_rx_ft     i2c_rx;     /*!< I2C read function */
   drv_i2c_tx_ft     i2c_tx;     /*!< I2C write function */
   drv_i2c_ioctl_ft  i2c_ioctl;  /*!< I2C ioctl function */
}tca953x_io_t;

typedef struct {
   address_t            addr;    /*!< I2C hardware address */
   uint32_t             timeout;
}tca953x_conf_t;

typedef struct {
   uint8_t     pol[2];     /*!<
                            * Polarity of port pins
                            * A '1' in a bit of a register inverts the pin polarity
                            */
   uint8_t     conf[2];    /*!<
                            * I/O configuration of port pins
                            * A '1' in a bit of a register indicates input pin
                            * A '0' in a bit of a register indicated output pin
                            */
}tca953x_reg_t;


/*!
 * \brief
 *    tle5009 data struct
 */
typedef struct {
   tca953x_io_t         io;
   tca953x_conf_t       conf;
   tca953x_reg_t        reg;
   drv_status_en        status;
}tca953x_t;


/*
 * ============ Public TCA953x API ============
 */

/*
 * Link and Glue functions
 */
void tca953x_link_i2c (tca953x_t *tca, void* i2c);
void tca953x_link_i2c_rx (tca953x_t *tca, drv_i2c_rx_ft fun);
void tca953x_link_i2c_tx (tca953x_t *tca, drv_i2c_tx_ft fun);
void tca953x_link_i2c_ioctl (tca953x_t *tca, drv_i2c_ioctl_ft fun);

/*
 * Set functions
 */
void tca953x_set_hwaddress (tca953x_t *tca, address_t add);
void tca953x_set_timeout (tca953x_t *tca, uint32_t to);

/*
 * User Functions
 */
void tca953x_deinit (tca953x_t *tca);
drv_status_en tca953x_init (tca953x_t *tca);

drv_status_en tca953x_direction (tca953x_t *tca, tca953x_port_en port, tca953x_pin_en pin, uint8_t dir);
drv_status_en tca953x_port_read (tca953x_t *tca, tca953x_port_en port, uint8_t *in);
drv_status_en tca953x_port_write (tca953x_t *tca, tca953x_port_en port, uint8_t out);
drv_status_en tca953x_pin_read (tca953x_t *tca, tca953x_port_en port, tca953x_pin_en pin, uint8_t *in);
drv_status_en tca953x_pin_write (tca953x_t *tca, tca953x_port_en port, tca953x_pin_en pin, uint8_t out);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __tca953x_h__ */
