/*!
 * \file bldc3_emf.c
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

#include <drv/bldc3_emf.h>

inline void _set_output_off (bldc3_t *bldc3)
{
   bldc3->io.uh (0);
   bldc3->io.ul (0);
   bldc3->io.vh (0);
   bldc3->io.vl (0);
   bldc3->io.wh (0);
   bldc3->io.wl (0);
}

inline void _set_output (bldc3_t *bldc3, bldc3_state_en state)
{
   switch (state) {
      default:
      case BLDC3_ST0:
         bldc3->io.uh (0); bldc3->io.vl (0); bldc3->io.wh (0); bldc3->io.wl (0);
         bldc3->io.ul (100);     bldc3->io.vh (100);
         break;
      case BLDC3_ST1:
         bldc3->io.uh (0); bldc3->io.vh (0); bldc3->io.vl (0); bldc3->io.wl (0);
         bldc3->io.ul (100);                             bldc3->io.wh (100);
         break;
      case BLDC3_ST2:
         bldc3->io.uh (0); bldc3->io.ul (0); bldc3->io.vh (0); bldc3->io.wl (0);
                                 bldc3->io.vl (100);     bldc3->io.wh (100);
         break;
      case BLDC3_ST3:
         bldc3->io.ul (0); bldc3->io.vh (0); bldc3->io.wh (0); bldc3->io.wl (0);
         bldc3->io.uh (100);     bldc3->io.vl (100);
         break;
      case BLDC3_ST4:
         bldc3->io.ul (0); bldc3->io.vh (0); bldc3->io.vl (0); bldc3->io.wh (0);
         bldc3->io.uh (100);                             bldc3->io.wl (100);
         break;
      case BLDC3_ST5:
         bldc3->io.uh (0); bldc3->io.ul (0); bldc3->io.vl (0); bldc3->io.wh (0);
                                 bldc3->io.vh (100);     bldc3->io.wl (100);
         break;
   }

}

/*
 *  ============= PUBLIC BLDC3 API =============
 */

/*
 * Link and Glue functions
 */
inline void bldc3_link_uh (bldc3_t *bldc3, drv_out_f_ft uh) {
   bldc3->io.uh = uh;
}
inline void bldc3_link_ul (bldc3_t *bldc3, drv_out_f_ft ul) {
   bldc3->io.ul = ul;
}
inline void bldc3_link_vh (bldc3_t *bldc3, drv_out_f_ft vh) {
   bldc3->io.vh = vh;
}
inline void bldc3_link_vl (bldc3_t *bldc3, drv_out_f_ft vl) {
   bldc3->io.vl = vl;
}
inline void bldc3_link_wh (bldc3_t *bldc3, drv_out_f_ft wh) {
   bldc3->io.wh = wh;
}
inline void bldc3_link_wl (bldc3_t *bldc3, drv_out_f_ft wl) {
   bldc3->io.wl = wl;
}
inline void bldc3_link_u_emf (bldc3_t *bldc3, drv_ain_i_ft u_emf) {
   bldc3->io.u_emf = u_emf;
}
inline void bldc3_link_v_emf (bldc3_t *bldc3, drv_ain_i_ft v_emf) {
   bldc3->io.v_emf = v_emf;
}
void bldc3_link_w_emf (bldc3_t *bldc3, drv_ain_i_ft w_emf) {
   bldc3->io.w_emf = w_emf;
}
void bldc3_link_i_br (bldc3_t *bldc3, drv_ain_f_ft i_br) {
   bldc3->io.I_br = i_br;
}
void bldc3_link_pid (bldc3_t *bldc3, pid_c_t *pid) {
   bldc3->io.pid = pid;
}

/*
 * Set functions
 */
drv_status_en bldc3_set_poles (bldc3_t *bldc3, int poles) {
   if ( poles<BLDC3_STATES )
      return DRV_ERROR;
   bldc3->set.poles = poles;
   return bldc3->status = DRV_READY;
}
drv_status_en bldc3_set_rpm (bldc3_t *bldc3, bldc3_rpm_t rpm) {
   if ( rpm < BLDC3_MIN_RPM || rpm > BLDC3_MAX_RPM(bldc3->set.poles))
      return DRV_ERROR;
   bldc3->set.Freq_r = rpm / 60;
   return bldc3->status = DRV_READY;
}

drv_status_en bldc3_set_i_br (bldc3_t *bldc3, float i_br) {
   if (i_br < 0)
      return DRV_ERROR;
   bldc3->set.I_br = i_br;

   return bldc3->status = DRV_READY;
}

drv_status_en bldc3_set_dir (bldc3_t *bldc3, bldc3_dir_en dir) {
   if (! (dir==BLDC3_FWD || dir == BLDC3_REV) )
      return DRV_ERROR;
   bldc3->set.dir = dir;
   return bldc3->status = DRV_READY;
}

/*
 * User Functions
 */
void bldc3_startup (bldc3_t *bldc3)
{
   int i, dl = BLDC3_RPM2USEC (120, bldc3->set.poles);

   // Clear all outputs
   _set_output_off (bldc3);
   jf_delay_ms (2000);

   // charge low site capacitors
   bldc3->io.ul (100);   bldc3->io.vl (100);   bldc3->io.wl (100);
   jf_delay_ms (100);
   bldc3->io.ul (0);     bldc3->io.vl (0);     bldc3->io.wl (0);

   // find rotor
   for (i=0 ; i<BLDC3_STATES ; ++i) {
      _set_output (bldc3, i);
      jf_delay_us (dl);
   }

   _set_output_off (bldc3);

}

void bldc3_stop (bldc3_t *bldc3);

drv_status_en bldc3_init (bldc3_t *bldc3)
{
   #define _bldc3_assert(_x)  if (!(_x)) return bldc3->status = DRV_ERROR;

   drv_status_en st = jf_probe ();

   if (st == DRV_NODEV || st == DRV_NOINIT) {
      if ( jf_init (BLDC3_JF_FREQ, BLDC3_JF_ARVALUE) )
         return bldc3->status = DRV_ERROR;
   }
   else  // if  (st == DRV_NODEV || st == DRV_NOINIT)
      return bldc3->status = DRV_ERROR;

   _bldc3_assert (bldc3->io.uh);
   _bldc3_assert (bldc3->io.ul);
   _bldc3_assert (bldc3->io.vh);
   _bldc3_assert (bldc3->io.vl);
   _bldc3_assert (bldc3->io.wh);
   _bldc3_assert (bldc3->io.wl);
   _bldc3_assert (bldc3->io.u_emf);
   _bldc3_assert (bldc3->io.v_emf);
   _bldc3_assert (bldc3->io.w_emf);
   //_bldc3_assert (bldc3->io.pid);
   //_bldc3_assert (bldc3->io.I_br);      // Optional feature

   _bldc3_assert ( bldc3->set.poles >= BLDC3_STATES );
      return bldc3->status = DRV_ERROR;

   /*
    * We are here, so all its OK.
    */
   return bldc3->status = DRV_READY;
   #undef _bldc3_assert
}

drv_status_en bldc3_control (bldc3_t *bldc3, uint8_t wait);

drv_status_en bldc3_ioctl (bldc3_t *bldc3, ioctl_cmd_t cmd, ioctl_buf_t buf);


