/*
 * \file tle5009.c
 * \brief
 *    A target independent TLE-5009 sensor driver
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
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
#include <drv/tle5009.h>

/*!
 * \brief
 *    Wrap angle in [0, 2pi) domain
 */
static float _wrap_0_2pi (float th)
{
   float _2pi = 2*M_PI;

   while (th<0 || th>_2pi) {
      if (th < 0)       th += _2pi;
      if (th >= _2pi)   th -= _2pi;
   }
   return th;
}

/*
 * ============ Public TLE5009 API ============
 */

/*
 * Link and Glue functions
 */

/*
 * Set functions
 */

/*
 * User Functions
 */

/*!
 * \brief
 *    tle5009 calibration routine based on exact method.
 *
 * We seek amplitude and phace of x and y measurments. We use
 * dot product for that.
 *
 * \param   tle5009  Pointer to tle5009 to use
 * \param   in       Pointer to tle5009 calibration data object
 * \return           The status of operation (not the driver's state)
 */
drv_status_en  tle5009_calib (tle5009_t *tle5009, tle5009_calib_input_t *in)
{
   _Complex double X, Y;
   float normal = in->size;
   int i;

   // We calculate one frequency of the frequency domain by dot product reference vector and measurments
   X = (vdot(in->cwcos, in->ref_cos, in->size) + vdot(in->ccwcos, in->ref_cos, in->size))/normal;
   X += 1j * (vdot(in->cwcos, in->ref_sin, in->size) + vdot (in->ccwcos, in->ref_sin, in->size))/normal;
   Y = (vdot(in->cwsin, in->ref_cos, in->size) + vdot(in->ccwsin, in->ref_cos, in->size))/normal;
   Y += 1j * (vdot (in->cwsin, in->ref_sin, in->size) + vdot(in->ccwsin, in->ref_sin, in->size))/normal;

   // Calculate mean offsets
   tle5009->calib.O_x = tle5009->calib.O_y = 0;
   for (i=0 ; i<in->size ; ++i) {
      tle5009->calib.O_x += in->cwcos[i] + in->ccwcos[i];
      tle5009->calib.O_y += in->cwsin[i] + in->ccwsin[i];
   }
   tle5009->calib.O_x /= 2*in->size;
   tle5009->calib.O_y /= 2*in->size;
   // Calculate Mean Amplitudes
   tle5009->calib.A_x = vnorm (&X, 1);
   tle5009->calib.A_y = vnorm (&Y, 1);
   // Calculate Orthogonality errors
   tle5009->calib.Phi_x = -atan2 (cimag(X), creal(X));
   tle5009->calib.Phi_y = M_PI_2 - atan2 (cimag(Y), creal(Y));

   return DRV_READY;
}

/*!
 * \brief
 *    Returns the current angle measured by tle5009
 *
 * \param   tle5009  Pointer to tle5009 to use
 * \param   cos_diff The X diff amplitude
 * \param   sin_diff The Y diff amplitude
 * \return           The angle in radians [0, 2pi)
 */
float tle5009_angle (tle5009_t *tle5009, float cos_diff, float sin_diff)
{
   float x, y, _phi;

   // Offset and gain correction
   x = (cos_diff - tle5009->calib.O_x )/tle5009->calib.A_x;
   y = (sin_diff - tle5009->calib.O_y )/tle5009->calib.A_y;

   // Non-orthogonality correction
   _phi = -tle5009->calib.Phi_x + tle5009->calib.Phi_y;
   y = (y - x * sin (_phi))/cos (_phi);

   return _wrap_0_2pi (atan2 (y, x) - tle5009->calib.Phi_x);
}

