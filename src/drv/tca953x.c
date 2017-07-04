/*!
 * \file tca953x.c
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
#include <drv/tca953x.h>

/*
 * ------------ Static API ------------------
 */
static drv_status_en _send_control (tca953x_t *tca, uint8_t rd, uint8_t ackp);
static drv_status_en _send_regadd (tca953x_t *tca, uint32_t add);

static drv_status_en _read_regs (tca953x_t *tca, uint8_t reg_add, uint8_t *data, int n);
static drv_status_en _write_regs (tca953x_t *tca, uint8_t reg_add, uint8_t *data, int n);


/*!
 * \brief
 *    Send control byte and select to use or not ACK polling
 *
 * \param  tcaPointer indicate the tca data stuct to use
 * \param  rd    Read flag, 1 to read, 0 to write.
 * \param  ackp  Use Ack polling
 *
 * \return
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _send_control (tca953x_t *tca, uint8_t rd, uint8_t ackp)
{
   uint8_t  ack;
   uint32_t to = tca->conf.timeout;

   // Cast rd to 0/1
   rd = (rd) ? TCA953x_READ : TCA953x_WRITE;

   // Control byte (read/write) with ACK polling or not
   do {
      tca->io.i2c_ioctl (tca->io.i2c, CTRL_START, (void*)0);
      ack = tca->io.i2c_tx (tca->io.i2c, (TCA953x_ADDRESS_MASK | tca->conf.addr) | rd, I2C_SEQ_BYTE_ACK);
      --to;
   }while (!ack && ackp && to);

   return (drv_status_en) (ack)? DRV_READY : DRV_ERROR;
}


/*!
 * \brief
 *    Send the register address to the BUS.
 *
 * \param  tca  Pointer indicate the tca data stuct to use
 * \param  add  The register address to send
 * \return
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _send_regadd (tca953x_t *tca, uint32_t add)
{
   if (!tca->io.i2c_tx (tca->io.i2c, (byte_t)add, I2C_SEQ_BYTE_ACK))
      return DRV_ERROR;
   return DRV_READY;
}

/*!
 * \brief
 *    Read data from registers
 * \param  tca       Pointer indicate the tca data stuct to use
 * \param  reg_add   Register address to read
 * \param  data      Pointer to buffer to store the data
 * \param  n         Number of registers to read
 * \return
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _read_regs (tca953x_t *tca, uint8_t reg_add, uint8_t *data, int n)
{
   uint8_t  ack;

   // Control byte (write)
   if (_send_control (tca, TCA953x_WRITE, 1) == DRV_ERROR) {
      tca->io.i2c_ioctl (tca->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }
   if (_send_regadd (tca, reg_add) == DRV_ERROR) {
      tca->io.i2c_ioctl (tca->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }
   if (_send_control (tca, TCA953x_READ, 0) == DRV_ERROR) {
      tca->io.i2c_ioctl (tca->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }

   // Seq read bytes with ACK except last one
   do {
      ack = (n>1) ? 1:0;
      *data++ = tca->io.i2c_rx (tca->io.i2c, ack, I2C_SEQ_BYTE_ACK);
      --n;
   } while (n);

   tca->io.i2c_ioctl (tca->io.i2c, CTRL_STOP, (void*)0);
   return DRV_READY;
}

/*!
 * \brief
 *    Send data to registers
 * \param  tca       Pointer indicate the tca data stuct to use
 * \param  reg_add   Register address to write
 * \param  data      Pointer to buffer to read the data
 * \param  n         Number of registers to write
 * \return
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
static drv_status_en _write_regs (tca953x_t *tca, uint8_t reg_add, uint8_t *data, int n)
{
   // Control byte (write)
   if (_send_control (tca, TCA953x_WRITE, 1) == DRV_ERROR) {
      tca->io.i2c_ioctl (tca->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }
   if (_send_regadd (tca, reg_add) == DRV_ERROR) {
      tca->io.i2c_ioctl (tca->io.i2c, CTRL_STOP, (void*)0);
      return DRV_ERROR;
   }

   // Try to write the data
   for (int i=0 ; i<n ; ++i, ++data)
      if (!tca->io.i2c_tx (tca->io.i2c, *data, I2C_SEQ_BYTE_ACK))
         return DRV_ERROR;

   tca->io.i2c_ioctl (tca->io.i2c, CTRL_STOP, (void*)0);
   return DRV_READY;
}




/*
 * ============ Public TCA953x API ============
 */

/*
 * Link and Glue functions
 */
__INLINE void tca953x_link_i2c (tca953x_t *tca, void* i2c){
   tca->io.i2c = i2c;
}
__INLINE void tca953x_link_i2c_rx (tca953x_t *tca, drv_i2c_rx_ft fun) {
   tca->io.i2c_rx = fun;
}
__INLINE void tca953x_link_i2c_tx (tca953x_t *tca, drv_i2c_tx_ft fun) {
   tca->io.i2c_tx = fun;
}
__INLINE void tca953x_link_i2c_ioctl (tca953x_t *tca, drv_i2c_ioctl_ft fun) {
   tca->io.i2c_ioctl = fun;
}


/*
 * Set functions
 */
void tca953x_set_hwaddress (tca953x_t *tca, address_t add) {
   tca->conf.addr = add;
}

void tca953x_set_timeout (tca953x_t *tca, uint32_t to) {
   tca->conf.timeout = to;
}

/*
 * User Functions
 */
/*!
 * \brief
 *    De-Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  tca       Pointer indicate the tca data stuct to use
 */
void tca953x_deinit (tca953x_t *tca)
{
   memset ((void*)tca, 0, sizeof (tca953x_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initializes peripherals used by the I2C EEPROM driver.
 *
 * \param  tca       Pointer indicate the tca data stuct to use
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en tca953x_init (tca953x_t *tca)
{
   #define _bad_link(_link)   (!tca->io._link) ? 1:0

   if (_bad_link (i2c))       return tca->status = DRV_ERROR;
   if (_bad_link (i2c_rx))    return tca->status = DRV_ERROR;
   if (_bad_link (i2c_tx))    return tca->status = DRV_ERROR;
   if (_bad_link (i2c_ioctl)) return tca->status = DRV_ERROR;

   if (tca->status == DRV_BUSY || tca->status == DRV_NODEV)
      return tca->status = DRV_ERROR;

   tca->status = DRV_BUSY;

   tca->reg.pol [0] = tca->reg.pol [1] = 0;        // Non-inverted
   tca->reg.conf [0] = tca->reg.conf [1] = 0xFF;   // Inputs

   return tca->status = DRV_READY;
   #undef _bad_link
}

/*!
 * \brief
 *    Sets Port's pin direction
 * \param   tca      Pointer indicate the tca data stuct to use
 * \param   port     Port to set
 *    \arg  TCA953x_PORT_0
 *    \arg  TCA953x_PORT_1
 * \param   pin      Pin to set
 *    \arg  TCA953x_PIN_0
 *    \arg  TCA953x_PIN_1
 *    \arg  TCA953x_PIN_2
 *    \arg  TCA953x_PIN_3
 *    \arg  TCA953x_PIN_4
 *    \arg  TCA953x_PIN_5
 *    \arg  TCA953x_PIN_6
 *    \arg  TCA953x_PIN_7
 *    \arg  TCA953x_PIN_ALL
 * \param   dir      The direction of the pins
 *    \arg  TCA953x_CONF_INPUT
 *    \arg  TCA953x_CONF_OUTPUT
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en tca953x_direction (tca953x_t *tca, tca953x_port_en port, tca953x_pin_en pin, uint8_t dir)
{
   uint8_t  reg_add = (port == TCA953x_PORT_0) ? TCA953x_CONF_PORT_0 : TCA953x_CONF_PORT_1;

   if (dir == TCA953x_CONF_INPUT)
      tca->reg.conf[port] |= pin;
   else
      tca->reg.conf[port] &= ~pin;

   return _write_regs (tca, reg_add, &tca->reg.conf[port], 1);
}

/*!
 * \brief
 *    Read Port's data
 * \param   tca      Pointer indicate the tca data stuct to use
 * \param   port     Port to read
 *    \arg  TCA953x_PORT_0
 *    \arg  TCA953x_PORT_1
 * \param   in       Pointer to byte to return
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en tca953x_port_read (tca953x_t *tca, tca953x_port_en port, uint8_t *in)
{
   uint8_t  reg_add = (port == TCA953x_PORT_0) ? TCA953x_INPUT_PORT_0 : TCA953x_INPUT_PORT_1;

   return _read_regs (tca, reg_add, in, 1);
}

/*!
 * \brief
 *    Write Port's data
 * \param   tca      Pointer indicate the tca data stuct to use
 * \param   port     Port to write
 *    \arg  TCA953x_PORT_0
 *    \arg  TCA953x_PORT_1
 * \param   out      byte to write
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en tca953x_port_write (tca953x_t *tca, tca953x_port_en port, uint8_t out)
{
   uint8_t  reg_add = (port == TCA953x_PORT_0) ? TCA953x_OUTPUT_PORT_0 : TCA953x_OUTPUT_PORT_1;

   return _write_regs (tca, reg_add, &out, 1);
}

/*!
 * \brief
 *    Read Port's pin data
 * \param   tca      Pointer indicate the tca data stuct to use
 * \param   port     Port to read
 *    \arg  TCA953x_PORT_0
 *    \arg  TCA953x_PORT_1
 * \param   pin      Pin to read, must be one or combination of the followings
 *    \arg     TCA953x_PIN_0
 *    \arg     TCA953x_PIN_1
 *    \arg     TCA953x_PIN_2
 *    \arg     TCA953x_PIN_3
 *    \arg     TCA953x_PIN_4
 *    \arg     TCA953x_PIN_5
 *    \arg     TCA953x_PIN_6
 *    \arg     TCA953x_PIN_7
 * \param   in       Pointer to byte to return
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en tca953x_pin_read (tca953x_t *tca, tca953x_port_en port, tca953x_pin_en pin, uint8_t *in)
{
   uint8_t  reg_add = (port == TCA953x_PORT_0) ? TCA953x_INPUT_PORT_0 : TCA953x_INPUT_PORT_1;

   if (_read_regs (tca, reg_add, in, 1) != DRV_READY)
      return DRV_ERROR;

   *in &= pin;
   return DRV_READY;
}

/*!
 * \brief
 *    Write Port's pin data
 * \param   tca      Pointer indicate the tca data stuct to use
 * \param   port     Port to write
 *    \arg  TCA953x_PORT_0
 *    \arg  TCA953x_PORT_1
 * \param   out      byte to write
 * \param   pin      Pin to write, must be one or combination of the followings
 *    \arg     TCA953x_PIN_0
 *    \arg     TCA953x_PIN_1
 *    \arg     TCA953x_PIN_2
 *    \arg     TCA953x_PIN_3
 *    \arg     TCA953x_PIN_4
 *    \arg     TCA953x_PIN_5
 *    \arg     TCA953x_PIN_6
 *    \arg     TCA953x_PIN_7
 * \return The status of the operation
 *    \arg DRV_READY
 *    \arg DRV_ERROR
 */
drv_status_en tca953x_pin_write (tca953x_t *tca, tca953x_port_en port, tca953x_pin_en pin, uint8_t out)
{
   uint8_t  reg_add = (port == TCA953x_PORT_0) ? TCA953x_OUTPUT_PORT_0 : TCA953x_OUTPUT_PORT_1;
   uint8_t  r;

   if (_read_regs (tca, reg_add, &r, 1) != DRV_READY)
      return DRV_ERROR;

   r &= ~pin;
   r |= (out != 0) ? pin : 0;

   return _write_regs (tca, reg_add, &r, 1);
}
