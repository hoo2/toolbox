/*
 * \file
 *    psa.c
 * \brief
 *    Based on PSA algorithm by Manuel Blanco-Muriel, Diego C. Alarcon-Padilla,
 *    Thedoro Lopez-Moratalla and Martin Lara-Coira published on
 *    Solar Energy Vol. 70, No. 5, pp. 431-441, 2001
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
 *
 * This header file contains the declaration of a type which includes all the
 * input and output data,  and the function that performs the calculation.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation,  either version 3
 * of the License,  or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not,  see <http://www.gnu.org/licenses/>.
 */
#ifndef __psa_h__
#define __psa_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <std/stime.h>
#include <tbx_types.h>
#include <math/math.h>
#include <math.h>

/*!
 * \brief
 *    PSA idea of time
 */
typedef struct {
   double dec_hour;
   /*!<
    * Fractional hour
    * \note
    *    This is fractional in order help Julian day calculation
    */
   double jd;     /*!< Julian day from noon 1 January 2000 Universal Time */
}psa_time_t;

/*!
 * \brief
 *    PSA idea of observer's location
 */
typedef struct {
   double   longitude;  /*!< Positive towards east [deg] */
   double   latitude;   /*!< Positive towards north [deg] */
}psa_location_t;

/*!
 * \brief
 *    PSA algorithm output values
 */
typedef struct {
   double   zenith;     /*!< Sun angle from zenith [rad] */
   double   azimuth;    /*!<
                         * Sun azimuth. Zero azimuth is pointing North
                         * and positive to eastern hemisphere (clockwise). [rad]
                         */
}psa_output_t;

/*!
 * \brief
 *    PSA algorithm data, based on user input
 */
typedef struct {
   psa_time_t     t;    /*!< Julian "Universal" time */
   psa_location_t loc;  /*!< Observer's location */
}psa_t;

/*
 * =============== Public API ==============
 */

/*
 * ============= Set functions ============
 */
drv_status_en psa_set_time (psa_t *psa, time_t utc);
drv_status_en psa_set_location (psa_t *psa, double lon, double lat);

/*
 * ============ User Functions ===========
 */
psa_output_t psa_calculate (psa_t *psa);

#ifdef __cplucpluc
}
#endif

#endif /* #ifndef __psa_h__ */
