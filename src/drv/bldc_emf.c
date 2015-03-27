/*!
 * \file bldc_emf.c
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

#include <drv/bldc_emf.h>

void _set_output_off (bldc_t *bldc)
{
   bldc->io.uh (0);
   bldc->io.ul (0);
   bldc->io.vh (0);
   bldc->io.vl (0);
   bldc->io.wh (0);
   bldc->io.wl (0);
}

void _set_output (bldc_t *bldc, bldc_br_state_en state, float speed)
{
   switch (state) {
      default:
      case BLDC_ST0:
         bldc->io.uh (0); bldc->io.vl (0); bldc->io.wh (0); bldc->io.wl (0);
         bldc->io.ul (speed);    bldc->io.vh (speed);
         break;
      case BLDC_ST1:
         bldc->io.uh (0); bldc->io.vh (0); bldc->io.vl (0); bldc->io.wl (0);
         bldc->io.ul (speed);                            bldc->io.wh (speed);
         break;
      case BLDC_ST2:
         bldc->io.uh (0); bldc->io.ul (0); bldc->io.vh (0); bldc->io.wl (0);
                                 bldc->io.vl (speed);    bldc->io.wh (speed);
         break;
      case BLDC_ST3:
         bldc->io.ul (0); bldc->io.vh (0); bldc->io.wh (0); bldc->io.wl (0);
         bldc->io.uh (speed);    bldc->io.vl (speed);
         break;
      case BLDC_ST4:
         bldc->io.ul (0); bldc->io.vh (0); bldc->io.vl (0); bldc->io.wh (0);
         bldc->io.uh (speed);                            bldc->io.wl (speed);
         break;
      case BLDC_ST5:
         bldc->io.uh (0); bldc->io.ul (0); bldc->io.vl (0); bldc->io.wh (0);
                                 bldc->io.vh (speed);    bldc->io.wl (speed);
         break;
   }
}

float _i_suppressor (bldc_t *bldc, float dc, float i)
{
   float i_th = bldc->set.I_br - bldc->set.I_th_diff;

   if (i > i_th)
      return (dc - dc * (i - i_th)/bldc->set.I_th_diff);
   else
      return dc;
}

/*
 *  ============= PUBLIC BLDC API =============
 */

/*
 * Link and Glue functions
 */
inline void bldc_link_uh (bldc_t *bldc, drv_out_f_ft uh) {
   bldc->io.uh = uh;
}
inline void bldc_link_ul (bldc_t *bldc, drv_out_f_ft ul) {
   bldc->io.ul = ul;
}
inline void bldc_link_vh (bldc_t *bldc, drv_out_f_ft vh) {
   bldc->io.vh = vh;
}
inline void bldc_link_vl (bldc_t *bldc, drv_out_f_ft vl) {
   bldc->io.vl = vl;
}
inline void bldc_link_wh (bldc_t *bldc, drv_out_f_ft wh) {
   bldc->io.wh = wh;
}
inline void bldc_link_wl (bldc_t *bldc, drv_out_f_ft wl) {
   bldc->io.wl = wl;
}
inline void bldc_link_u_emf (bldc_t *bldc, drv_ain_i_ft u_emf) {
   bldc->io.u_emf = u_emf;
}
inline void bldc_link_v_emf (bldc_t *bldc, drv_ain_i_ft v_emf) {
   bldc->io.v_emf = v_emf;
}
void bldc_link_w_emf (bldc_t *bldc, drv_ain_i_ft w_emf) {
   bldc->io.w_emf = w_emf;
}
void bldc_link_i_br (bldc_t *bldc, drv_ain_f_ft i_br) {
   bldc->io.I_br = i_br;
}
void bldc_link_pid (bldc_t *bldc, pid_c_t *pid) {
   bldc->io.pid = pid;
}

/*
 * Set functions
 */
void bldc_set_poles (bldc_t *bldc, int poles) {
   bldc->set.poles = poles;
}

void bldc_set_rpm (bldc_t *bldc, bldc_rpm_t rpm) {
   bldc->set.Freq_r = rpm / 60;
}

void bldc_set_i_br (bldc_t *bldc, float i_br) {
   bldc->set.I_br = i_br;
}

void bldc_set_i_th_diff (bldc_t *bldc, float i) {
   bldc->set.I_th_diff = i;
}

void bldc_set_dir (bldc_t *bldc, bldc_dir_en dir) {
   bldc->set.dir = dir;
}

void bldc_set_startup_speed (bldc_t *bldc, float sp) {
   bldc->set.startup_speed = sp;
}

void bldc_set_startup_time (bldc_t *bldc, clock_t t) {
   bldc->set.startup_time = t;
}

/*
 * User Functions
 */

void bldc_startup (bldc_t *bldc, float sp)
{
   uint32_t startup_timing [30] = {
      500000, 450000, 400000, 300000, 200000, 100000,
       90000,  80000,  70000,  60000,  55000,  52000,
       51000,  50000,  49500,  49000,  48500,  48000,
       47500,  47000,  46500,  46000,  45000,  45000,
       45000,  45000,  45000,  45000,  45000,  45000
   };
   bldc_br_state_en state;
   int i;

   // charge low site capacitors
   bldc->io.ul (100);   bldc->io.vl (100);   bldc->io.wl (100);
   jf_delay_ms (100);
   bldc->io.ul (0);     bldc->io.vl (0);     bldc->io.wl (0);

   i=0;
   state = BLDC_ST0;
   do {
      if ( !jf_check_usec (startup_timing[i]) ) {
         ++i;
         ++state;
         if (state >BLDC_ST5)
            state = BLDC_ST0;
      }
      _set_output (bldc, state, sp);
   } while (i<30);

}

void bldc_stop (bldc_t *bldc) {
   _set_output_off (bldc);
}


drv_status_en bldc_init (bldc_t *bldc)
{
   #define _bldc_assert(_x)   if (!(_x)) return bldc->status = DRV_ERROR;

   drv_status_en st = jf_probe ();

   if (st == DRV_NODEV || st == DRV_NOINIT)
      return bldc->status = DRV_ERROR;

   _bldc_assert (bldc->io.uh);
   _bldc_assert (bldc->io.ul);
   _bldc_assert (bldc->io.vh);
   _bldc_assert (bldc->io.vl);
   _bldc_assert (bldc->io.wh);
   _bldc_assert (bldc->io.wl);
   _bldc_assert (bldc->io.u_emf);
   _bldc_assert (bldc->io.v_emf);
   _bldc_assert (bldc->io.w_emf);
   //_bldc_assert (bldc->io.pid);
   //_bldc_assert (bldc->io.I_br);        // Optional feature

   _bldc_assert ( bldc->set.poles >= BLDC_STATES );

   /*
    * We are here, so all its OK.
    */
   return bldc->status = DRV_READY;
   #undef _bldc_assert
}

drv_status_en bldc_control (bldc_t *bldc, uint8_t wait);

drv_status_en bldc_ioctl (bldc_t *bldc, ioctl_cmd_t cmd, ioctl_buf_t buf);


