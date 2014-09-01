/*!
 * \file bldc3_emf.h
 * \brief
 *    A target independent brushless DC motor with back-emf.
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
#ifndef __bldc3_emf_h__
#define __bldc3_emf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_iotypes.h>
#include <acs/pid.h>

typedef int bldc3_rpm_t;

/*!
 * 3 Phase Brushless DC motor output bridge driving state
 */
typedef enum {
   BLDC3_ST0 = 0,    /*!< UL(PWM), VH(ON), W(ZC) */
   BLDC3_ST1,        /*!< WH(PWM), UL(ON), V(ZC) */
   BLDC3_ST2,        /*!< VL(PWM), WH(ON), U(ZC) */
   BLDC3_ST3,        /*!< UH(PWM), VL(ON), W(ZC) */
   BLDC3_ST4,        /*!< WL(PWM), UH(ON), V(ZC) */
   BLDC3_ST5         /*!< VH(PWM), WL(ON), U(ZC) */
}bldc3_state_en;

/*!
 * BLDC motor direction enumerator
 */
typedef enum {
   BLDC3_REV = -1,      /*!< Reverse */
   BLDC3_FWD = 1        /*!< Forward */
}bldc3_dir_en;

/*!
 * I/O link function pointers
 */
typedef struct {
   drv_ain_i_t    u_emf;      /*!< analog zc function, return integer ADC value */
   drv_ain_i_t    v_emf;      /*!< analog zc function, return integer ADC value */
   drv_ain_i_t    w_emf;      /*!< analog zc function, return integer ADC value */
   drv_ain_f_t    I_br;       /*!< analog I bridge function, return current value as float */
   pid_c_t        *pid;       /*!< closed loop PID controller */
}bldc3_link_t;

/*!
 * BLDC motor data struct
 */
typedef struct {
   int   Freq_r;              /*!< rotation frequency rpm/60 */
   int   Freq_zc;             /*!< zero cross frequency */
   float I_br;                /*<! Total bridge current */
   float V_br;                /*<! Bridge voltage */
}bldc3_in_data_t;

/*!
 * User set point for the control loop
 */
typedef struct {
   int            Freq_r;     /*!< Desired rotation frequency */
   float          I_br;       /*!< Desired Maximum bridge current */
   bldc3_dir_en   dir;        /*!< Desired direction of rotation */
}bldc3_sp_t;

/*!
 * Event struct
 */
typedef struct {
   uint8_t  zc       :1;      /*!< zero cross event */
   uint8_t  commute  :1;      /*!< Commute event */
}bldc3_event_t;

/*!
 * 3 phase BLDC motor type
 */
typedef volatile struct {
   bldc3_link_t      io;      /*!< link to application low level driver */
   bldc3_in_data_t   in;      /*!< Input data */
   bldc3_sp_t        sp;      /*!< Set point data */
   bldc3_event_t     event;   /*!< Events */
   bldc3_state_en    state;   /*!< Current output driving state */
}bldc3_t;



/*
 *  ============= PUBLIC BLDC3 API =============
 */

/*
 * Link and Glue functions
 */
void bldc3_link_u_emf (drv_ain_i_t u_emf);
void bldc3_link_v_emf (drv_ain_i_t v_emf);
void bldc3_link_w_emf (drv_ain_i_t w_emf);
void bldc3_link_i_br (drv_ain_f_t i_br);
void bldc3_link_pid (pid_c_t *pid);

/*
 * Set functions
 */
void bldc3_set_rpm (bldc3_rpm_t rpm);
void bldc3_set_dir (bldc3_dir_en dir);

/*
 * User Functions
 */
void bldc3_startup (void);
void bldc3_control (uint8_t wait);

drv_status_en bldc3_ioctl (ioctl_cmd_t cmd, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __bldc3_emf_h__
