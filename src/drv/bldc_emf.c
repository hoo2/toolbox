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

inline void _set_output_off (bldc_t *bldc)
{
   bldc->io.uh (0);
   bldc->io.ul (0);
   bldc->io.vh (0);
   bldc->io.vl (0);
   bldc->io.wh (0);
   bldc->io.wl (0);
}

inline void _set_output (bldc_t *bldc, bldc_state_en state, float speed)
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

inline float _i_suppressor (bldc_t *bldc, float dc, float i)
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
drv_status_en bldc_set_poles (bldc_t *bldc, int poles) {
   if ( poles<BLDC_STATES )
      return DRV_ERROR;
   bldc->set.poles = poles;
   return bldc->status = DRV_READY;
}

drv_status_en bldc_set_rpm (bldc_t *bldc, bldc_rpm_t rpm) {
   if ( rpm < BLDC_MIN_RPM || rpm > BLDC_MAX_RPM(bldc->set.poles))
      return DRV_ERROR;
   bldc->set.Freq_r = rpm / 60;
   return bldc->status = DRV_READY;
}

drv_status_en bldc_set_i_br (bldc_t *bldc, float i_br) {
   if (i_br < 0)
      return DRV_ERROR;
   bldc->set.I_br = i_br;

   return bldc->status = DRV_READY;
}

drv_status_en bldc_set_dir (bldc_t *bldc, bldc_dir_en dir) {
   if (! (dir==BLDC_FWD || dir == BLDC_REV) )
      return DRV_ERROR;
   bldc->set.dir = dir;
   return bldc->status = DRV_READY;
}

/*
 * User Functions
 */

void bldc_startup (bldc_t *bldc)
{
   float dc = BLDC_STARTING_DC;
   int i;

   // Clear all outputs
   _set_output_off (bldc);
   jf_delay_ms (1000);

   // charge low site capacitors
   bldc->io.ul (100);   bldc->io.vl (100);   bldc->io.wl (100);
   jf_delay_ms (500);
   bldc->io.ul (0);     bldc->io.vl (0);     bldc->io.wl (0);


   // find rotor
   for (i=0 ; i<BLDC_STARTING_TIME*2 ; ++i) {
      _set_output (bldc, BLDC_ST5, _i_suppressor (bldc, dc, bldc->io.I_br()));
      jf_delay_us (500);
   }
   _set_output_off (bldc);
}

void bldc_roll (bldc_t *bldc)
{
   int i, dl = BLDC_RPM2USEC (600, bldc->set.poles);

   // charge low site capacitors
   bldc->io.ul (100);   bldc->io.vl (100);   bldc->io.wl (100);
   jf_delay_ms (100);
   bldc->io.ul (0);     bldc->io.vl (0);     bldc->io.wl (0);

   for (i=0 ; i<bldc->set.poles ; ++i) {
      _set_output (bldc, i%6, 2);
      jf_delay_us (dl);
   }
}

void bldc_stop (bldc_t *bldc);

drv_status_en bldc_init (bldc_t *bldc)
{
   #define _bldc_assert(_x)   if (!(_x)) return bldc->status = DRV_ERROR;

   drv_status_en st = jf_probe ();

   if (st == DRV_NODEV || st == DRV_NOINIT) {
      if ( jf_init (BLDC_JF_FREQ, BLDC_JF_ARVALUE) )
         return bldc->status = DRV_ERROR;
   }
   else  // if  (st == DRV_NODEV || st == DRV_NOINIT)
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


