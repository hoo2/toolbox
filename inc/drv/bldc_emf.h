/*!
 * \file bldc_emf.h
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
#ifndef __bldc_emf_h__
#define __bldc_emf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_ioctl.h>
#include <tbx_iotypes.h>
#include <acs/pid.h>
#include <sys/jiffies.h>


/*
 * ==================  USER defines   =======================
 */
#define BLDC_JF_FREQ                      (1000000)   //1MHz - time base 1usec
#define BLDC_JF_ARVALUE                   (0x8FFF)
#define BLDC_MIN_STEP_PER_COMMUTE         (500)
/*!<
 * Indicates the minimum check points per zc period
 */


/*
 * ================== General defines =======================
 */
#define BLDC_STATES                    (6)         // 3 phace system
#define BLDC_MIN_RPM                   (60)        // 1 Hz
#define BLDC_MAX_RPM(_poles)           ( (60*BLDC_JF_FREQ) / (BLDC_MIN_STEP_PER_COMMUTE * (_poles)) )
/*!<
 *             60 * JF_CLK
 *   RPM <= -------------------
 *           MIN_STEP * POLES
 *
 * MIN_STEP = BLDC_DT_PER_COMMUTE
 */

#define BLDC_RPM2USEC(_rpm, _poles)    ( (60 * 1000000) / ((_rpm)*(_poles)) )

typedef int bldc_rpm_t;


/*!
 * 3 Phase Brushless DC motor output bridge driving state
 */
typedef enum {
   BLDC_ST0 = 0,     /*!< UL(PWM), VH(ON), W(ZC) */
   BLDC_ST1,         /*!< WH(PWM), UL(ON), V(ZC) */
   BLDC_ST2,         /*!< VL(PWM), WH(ON), U(ZC) */
   BLDC_ST3,         /*!< UH(PWM), VL(ON), W(ZC) */
   BLDC_ST4,         /*!< WL(PWM), UH(ON), V(ZC) */
   BLDC_ST5          /*!< VH(PWM), WL(ON), U(ZC) */
}bldc_state_en;

/*!
 * BLDC motor direction enumerator
 */
typedef enum {
   BLDC_REV = -1,       /*!< Reverse */
   BLDC_FWD = 1         /*!< Forward */
}bldc_dir_en;

/*!
 * I/O link function pointers
 */
typedef struct {
   drv_out_f_ft   uh, ul;     /*!< Output U high and low site functions */
   drv_out_f_ft   vh, vl;     /*!< Output V high and low site functions */
   drv_out_f_ft   wh, wl;     /*!< Output W high and low site functions */
   drv_ain_i_ft   u_emf;      /*!< analog zc function, return integer ADC value */
   drv_ain_i_ft   v_emf;      /*!< analog zc function, return integer ADC value */
   drv_ain_i_ft   w_emf;      /*!< analog zc function, return integer ADC value */
   drv_ain_f_ft   I_br;       /*!< analog I bridge function, return current value as float */
   pid_c_t        *pid;       /*!< closed loop PID controller */
}bldc_link_t;

/*!
 * BLDC motor data struct
 */
typedef struct {
   int   Freq_r;              /*!< rotation frequency rpm/60 */
   int   Freq_zc;             /*!< zero cross frequency */
   float I_br;                /*<! Total bridge current */
   float V_br;                /*<! Bridge voltage */
}bldc_in_data_t;

/*!
 * User settings for the control loop
 */
typedef struct {
   int            Freq_r;     /*!< Desired rotation frequency */
   float          I_br;       /*!< Desired Maximum bridge current */
   bldc_dir_en   dir;        /*!< Desired direction of rotation */
   int            poles;      /*!< Number of Motor poles */
}bldc_set_t;

/*!
 * Event struct
 */
typedef struct {
   uint8_t  zc       :1;      /*!< zero cross event */
   uint8_t  commute  :1;      /*!< Commute event */
}bldc_event_t;

/*!
 * 3 phase BLDC motor type
 */
typedef volatile struct {
   bldc_link_t       io;      /*!< link to application low level driver */
   bldc_in_data_t    in;      /*!< Input data */
   bldc_set_t        set;     /*!< Settings */
   bldc_event_t      event;   /*!< Events */
   bldc_state_en     state;   /*!< Current output driving state */
   drv_status_en     status;  /*!< Driver's status */
}bldc_t;


/*
 *  ============= PUBLIC BLDC API =============
 */

/*
 * Link and Glue functions
 */
void bldc_link_uh (bldc_t *bldc, drv_out_f_ft uh);
void bldc_link_ul (bldc_t *bldc, drv_out_f_ft ul);
void bldc_link_vh (bldc_t *bldc, drv_out_f_ft vh);
void bldc_link_vl (bldc_t *bldc, drv_out_f_ft vl);
void bldc_link_wh (bldc_t *bldc, drv_out_f_ft wh);
void bldc_link_wl (bldc_t *bldc, drv_out_f_ft wl);
void bldc_link_u_emf (bldc_t *bldc, drv_ain_i_ft u_emf);
void bldc_link_v_emf (bldc_t *bldc, drv_ain_i_ft v_emf);
void bldc_link_w_emf (bldc_t *bldc, drv_ain_i_ft w_emf);
void bldc_link_i_br (bldc_t *bldc, drv_ain_f_ft i_br);
void bldc_link_pid (bldc_t *bldc, pid_c_t *pid);

/*
 * Set functions
 */
drv_status_en bldc_set_poles (bldc_t *bldc, int poles);
drv_status_en bldc_set_rpm (bldc_t *bldc, bldc_rpm_t rpm);
drv_status_en bldc_set_i_br (bldc_t *bldc, float i_br);
drv_status_en bldc_set_dir (bldc_t *bldc, bldc_dir_en dir);

/*
 * User Functions
 */
void bldc_startup (bldc_t *bldc);
void bldc_roll (bldc_t *bldc);
void bldc_stop (bldc_t *bldc);

drv_status_en bldc_init (bldc_t *bldc);
drv_status_en bldc_control (bldc_t *bldc, uint8_t wait);

drv_status_en bldc_ioctl (bldc_t *bldc, ioctl_cmd_t cmd, ioctl_buf_t buf);

#ifdef __cplusplus
}
#endif

#endif   //#ifndef __bldc_emf_h__
