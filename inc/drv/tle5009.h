/*
 * \file tle5009.h
 * \brief
 *    A target independent TLE-5009 sensor driver
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
#ifndef __tle5009_h__
#define __tle5009_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <dsp/vectors.h>
#include <math/math.h>
#include <complex.h>

/*
 * ============ User options ===========
 */
#define  TLE_COSINE_FUN       qcos     /*!< not used yet */
#define  TLE_SIN_FUN          qsin     /*!< not used yet */

/*
 * ============ Data types ============
 */

/*!
 * \brief
 *    tle5009 calibration data struct
 */
typedef struct {
   float    A_x;           /*!< Mean X(cos) amplitude */
   float    A_y;           /*!< Mean Y(sin) amplitude */
   float    O_x;           /*!< Mean X(cos) offset */
   float    O_y;           /*!< Mean Y(sin) offset */
   float    Phi_x;         /*!< X(cos) orthogonality error (phace drift) */
   float    Phi_y;         /*!< Y(sin) orthogonality error (phace drift) */
}tle5009_calib_data_t;

/*!
 * \brief
 *    tle5009 calibration routine object type for
 *    parameters passing
 */
typedef struct {
   float *cwcos;     /*!< Clock-wise cos measurments */
   float *cwsin;     /*!< Clock-wise sin measurments */
   float *ccwcos;    /*!< Counter clock-wise cos measurments */
   float *ccwsin;    /*!< Counter clock-wise sin measurments */
   float *ref_cos;   /*!< Cosine reference vector */
   float *ref_sin;   /*!< Sine reference vector */
   int   size;       /*<! Vector sizes */
}tle5009_calib_input_t;

/*!
 * \brief
 *    tle5009 data struct
 */
typedef struct {
   float                angle;
   tle5009_calib_data_t calib;
   drv_status_en        status;
}tle5009_t;


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
drv_status_en  tle5009_calib (tle5009_t *tle5009, tle5009_calib_input_t *in);
float tle5009_angle (tle5009_t *tle5009, float cos_diff, float sin_diff);

#ifdef __cplusplus
 }
#endif

#endif //#ifndef __tle5009_h__
