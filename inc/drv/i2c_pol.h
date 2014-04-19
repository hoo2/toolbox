/*
 * \file jiffies.h
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
 * All Rights Reserved.
 *
 * NOTICE:  All information contained herein is, and remains
 * the property of Houtouridis Christos. The intellectual
 * and technical concepts contained herein are proprietary to
 * Houtouridis Christos and are protected by copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Houtouridis Christos.
 *
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       06/2013
 * Version:    0.1
 *
 */
#ifndef __i2c_pol_h__
#define __i2c_pol_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/jiffies.h>
#include <stdint.h>

// Callback types
typedef uint8_t (*i2c_pin_ft) (uint8_t);
typedef void (*i2c_pindir_ft) (uint8_t);

typedef volatile struct
{
   i2c_pin_ft     sda,scl;
   i2c_pindir_ft  sda_dir;
   int            clk_delay;
}i2c_pol_t;


void i2c_setsda(i2c_pol_t *i2c, i2c_pin_ft sda);
void i2c_setscl (i2c_pol_t *i2c, i2c_pin_ft scl);
void i2c_setsdadir (i2c_pol_t *i2c, i2c_pindir_ft pd);
void i2c_setspeed (i2c_pol_t *i2c, int speed);


void i2c_deinit (i2c_pol_t *i2c);
void i2c_init (i2c_pol_t *i2c, int speed);
void i2c_start (i2c_pol_t *i2c);
void i2c_stop (i2c_pol_t *i2c);
uint8_t i2c_tx(i2c_pol_t *i2c, uint8_t byte);
uint8_t i2c_rx(i2c_pol_t *i2c, uint8_t ack);


#ifdef __cplusplus
}
#endif

#endif   //#ifndef __i2c_pol_h__
