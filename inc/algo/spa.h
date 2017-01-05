/*
 * \file
 *    spa.h
 * \brief
 *    Based on sun position algorithm from Measurement & Instrumentation Team
 *    Solar Radiation Research Laboratory National Renewable Energy Laboratory
 *    1617 Cole Blvd, Golden, CO 80401
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2015 Houtouridis Christos (http://www.houtouridis.net)
 *
 * This header file contains the declaration of a type which includes all the
 * input and output data, and the function that performs the calculation.
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

#ifndef __spa_h__
#define __spa_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <std/stime.h>
#include <tbx_types.h>
#include <math.h>

/*
 * ===================== Public Data types ================
 */
/*!
 * \brief
 *    SPA idea of time
 */
typedef struct {
   int   year;    /*!< Real year number,  valid range:   -2000 to 6000 */
   int   mon;     /*!< Real month number, valid range:   1 to 12 */
   int   day;     /*!< Real day number,   valid range:   1 to 31 */
   int   hour;    /*!< Observer's hour UTC      valid range:   0 to 24 */
   int   min;     /*!< Observer's minutes UTC   valid range:   0 to 59 */
   int   sec;     /*!< Observer's sec UTC       valid range:   0 to 59 */

   double delta_ut1;    /*!<
                         * Fractional second difference between UTC and UT which is used
                         * to adjust UTC for earth's irregular rotation rate and is derived
                         * from observation only and is reported in this bulletin:
                         * http://maia.usno.navy.mil/ser7/ser7.dat,
                         * where delta_ut1 = DUT1
                         * valid range: -1 to 1 second (exclusive)
                         */
    double delta_t;      /*!<
                          * Difference between earth rotation time and terrestrial time
                          * It is derived from observation only and is reported in this
                          * bulletin: http://maia.usno.navy.mil/ser7/ser7.dat,
                          * where delta_t = 32.184 + (TAI-UTC) - DUT1
                          * valid range: -8000 to 8000 seconds
                          */
    double timezone;     /*!<
                          * Observer time zone (negative west of Greenwich)
                          * valid range: -18   to   18 hours
                          */
} spa_time_t;


/*!
 * \brief
 *    Observer's location data
 */
typedef struct {
   double longitude;    /*!<
                         * Observer longitude (negative west of Greenwich)
                         * valid range: -180  to  180 degrees
                         */
   double latitude;     /*!<
                         * Observer latitude (negative south of equator)
                         * valid range: -90   to   90 degrees
                         */
   double elevation;    /*!<
                         * Observer elevation [meters]
                         * valid range: -6500000 or higher meters
                         */

   double slope;        /*!<
                         * Surface slope (measured from the horizontal plane)
                         * valid range: -360 to 360 degrees
                         */
   double azm_rotation; /*!<
                         * Surface azimuth rotation (measured from south to projection of
                         * surface normal on horizontal plane, negative east)
                         * valid range: -360 to 360 degrees
                         */
} spa_location_t;

/*!
 * \brief
 *    Observers atmospheric conditions
 */
typedef struct {
   double pressure;     /*!<
                         * Annual average local pressure [millibars]
                         * valid range:    0 to 5000 millibars
                         */
   double temperature;  /*!<
                         * Annual average local temperature [degrees Celsius]
                         * valid range: -273 to 6000 degrees Celsius
                         */
   double refract;      /*!<
                         * Atmospheric refraction at sunrise and sunset (0.5667 deg is typical)
                         * valid range: -5   to   5 degrees
                         */
} spa_atmos_t;


/*!
 * \brief
 *    Enumerator to select which function to perform
 */
typedef enum {
    SPA_ZA,           /*!< calculate zenith and azimuth */
    SPA_ZA_INC,       /*!< calculate zenith, azimuth, and incidence */
    SPA_ZA_RTS,       /*!< calculate zenith, azimuth, and sun rise/transit/set values */
    SPA_ALL,          /*!< calculate all SPA output values */
}spa_func_en;


/*!
 * \brief
 *    SPA input structure to pass the data to algorithm
 */
typedef struct {
   spa_time_t        utc;     /*!< UTC time of the observer */
   spa_location_t    loc;     /*!< Observer's location */
   spa_atmos_t       atmos;   /*!< Observer's air condition data */
   spa_func_en       fun;     /*!< Switch to choose functions for desired output (from enumeration) */
}spa_t;

/*!
 * \brief
 *    SPA return object
 */
typedef struct {
   double zenith;       //topocentric zenith angle [degrees]
   double azimuth_astro;//topocentric azimuth angle (westward from south) [for astronomers]
   double azimuth;      //topocentric azimuth angle (eastward from north) [for navigators and solar radiation]
   double incidence;    //surface incidence angle [degrees]

   double suntransit;   //local sun transit time (or solar noon) [fractional hour]
   double sunrise;      //local sunrise time (+/- 30 seconds) [fractional hour]
   double sunset;       //local sunset time (+/- 30 seconds) [fractional hour]
} spa_output_t;

/*
 * ================== PUBLIC API ===================
 */

/*
 * ============= Set functions ============
 */

drv_status_en spa_set_time (spa_t *spa, time_t utc, double dut1, double dt, double tz);
drv_status_en spa_set_location (spa_t *spa, double lon, double lat, double el, double sl, double azr);
drv_status_en spa_set_atmos (spa_t *spa, double pr, double temp, double at_ref);

/*
 * ============ User Functions ===========
 */
spa_output_t spa_calculate (spa_t *spa, spa_func_en fun);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef __spa_h__ */
