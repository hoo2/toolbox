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
#include <algo/psa.h>

/*
 * =========== Private Defines ===========
 */

#ifndef _deg2rad
#define _deg2rad(_d_)   ((M_PI/180.0)*(_d_))    /*!< degrees to rad converter */
#endif
#ifndef _rad2deg
#define _rad2deg(_r_)   ((180.0/M_PI)*(_r_))    /*!< rad to degrees converter */
#endif

#define  PSA_EARTH_MEAN_RADIUS      (6371.01)      /*!< [km] */
#define  PSA_ASTRONOMICAL_UNIT      (149597890)    /*!< [km] */

/*
 * =========== Private Functions ===========
 */
static void _ecliptic_coordinates (double jd, double *elon, double *eobl);
static void _celestial_coordinates (double elon, double eobl, double *ra, double * dec);
static void _local_coordinates (psa_t *psa, double ra, double dec, double *zen, double *az);


/*!
 * \brief
 *    Calculate ecliptic coordinates ( ecliptic longitude and obliquity ) in radians
 *    but without limiting the angle to be less than 2*Pi
 * \note
 *    the result may be greater than 2*Pi
 * \param   jd    Julian day from noon 1 January 2000 Universal Time
 * \param   elon  Pointer to ecliptic longitude to return
 * \param   eobl  Pointer to ecliptic obliquity to return
 * \return  None
 */
static void _ecliptic_coordinates (double jd, double *elon, double *eobl)
{
   double w             = 2.1429 - 0.0010394594*jd;
   double mean_lon      = 4.8950630 + 0.017202791698*jd;  /* Radians */
   double mean_anomaly  = 6.2400600 + 0.0172019699*jd;

   *elon = (mean_lon - 0.0001134
            + 0.03341607*sin (mean_anomaly)
            + 0.00034894*sin( 2*mean_anomaly )
            - 0.0000203*sin(w));
   *eobl = 0.4090928 - 6.2140e-9*jd + 0.0000396*cos(w);
}

/*!
 * \brief
 *    Calculate celestial coordinates ( right ascension and declination ) in radians
 * but without limiting the angle to be less than 2*Pi
 * \note
 *    the result may be greater than 2*Pi
 * \param
 * \return  None
 */
static void _celestial_coordinates (double elon, double eobl, double *ra, double * dec)
{
   double dx, dy;
   double sin_elon = sin(elon);

   dx = cos(elon);
   dy = cos(eobl) * sin_elon;

   *ra = atan2 (dy, dx);
   *ra += (*ra < 0.0) ? M_2PI : 0.0;
   *dec = asin (sin (eobl) * sin_elon);
}

/*
 * \brief
 *    Calculate local coordinates ( azimuth and zenith angle ) in rad
 * \param   psa      Pointer to psa_t structure to use for calculations
 * \param   ra       Celestial's coordinate right ascension [rad]
 * \param   dec      Celestial's coordinate declination [rad]
 * \param   zen      Pointer to Sun angle from zenith variable for data return [rad]
 * \param   az       Pointer to Sun azimuth for data return [rad]
 *                   The angle is: Zero azimuth is pointing North and positive
 *                                 to eastern hemisphere (clockwise).
 */
static void _local_coordinates (psa_t *psa, double ra, double dec, double *zen, double *az)
{
   double greenwich_mean_st;
   double local_mean_st;
   double hour_angle;
   double lat_rad;
   double parallax;
   /* Helper variables */
   double cos_lat;
   double sin_lat;
   double cos_ha;

   greenwich_mean_st = 6.6974243242 + 0.0657098283*psa->t.jd + psa->t.dec_hour;
   local_mean_st = _deg2rad (greenwich_mean_st*15 + psa->loc.longitude);
   hour_angle = local_mean_st - ra;
   lat_rad = _deg2rad (psa->loc.latitude);
   cos_lat = cos (lat_rad);
   sin_lat = sin (lat_rad);
   cos_ha = cos (hour_angle);

   *zen = acos (cos_lat*cos_ha*cos(dec) + sin(dec)*sin_lat);
   *az = atan2 (-sin (hour_angle), tan (dec)*cos_lat - sin_lat*cos_ha);
   *az += (*az < 0.0) ? M_2PI : 0.0;

   // Parallax Correction
   parallax = (PSA_EARTH_MEAN_RADIUS/PSA_ASTRONOMICAL_UNIT)*sin (*zen);
   *zen += parallax;
}



/*
 * =============== Public API ==============
 */

/*
 * ============= Set functions ============
 */

/*!
 * \brief
 *    Set the Universal time for calculations
 * \param   psa      Pointer to psa_t structure to use for calculations
 * \param   utc      the time to use in UNIX time format
 * \return  The status of the operation
 *    \arg  DRV_ERROR   Validation failure
 *    \arg  DRV_READY   Success
 */
drv_status_en psa_set_time (psa_t *psa, time_t utc)
{
   struct tm   *t;   /* lib time */
   int   y,m;        /* year month helpers */
   long int aux1, aux2; /* Julian day helpers */

   /*
    * Get PSA time from UNIX time and parameters
    */
   t = sgmtime (&utc);
   y = _TM_YEAR_2_YEAR (t->tm_year);
   m  = _TM_MON_2_MON (t->tm_mon);

   /*
    * Calculate fractional time of the day in UT decimal hours
    */
   psa->t.dec_hour = t->tm_hour + t->tm_min/60.0 + t->tm_sec/3600.0;

   /*
    * Calculate Julian day from noon 1 January 2000 Universal Time
    */
   aux1 = (m - 14)/12;
   aux2 = (1461*(y + 4800 + aux1))/4
          + (367*(m - 2 - 12*aux1))/12
          - (3*((y + 4900 + aux1)/100))/4
          + t->tm_mday - 32075;
   psa->t.jd = (double)(aux2) - 0.5 + psa->t.dec_hour/24.0;    /*!< this last division is floating point */
   psa->t.jd -= 2451545.0;    /*!< shift to start at noon 1 January 2000 Universal Time JD 2451545.0 */

   return DRV_READY;
}

/*
 * \brief
 *    Set location for PSA algorithm
 * \param   psa      Pointer to psa_t structure to use for calculations
 * \param   lon      Observer longitude (negative west of Greenwich)
 *                   valid range: -180  to  180 degrees
 * \param   lat      Observer latitude (negative south of equator)
 *                   valid range: -90   to   90 degrees
 * \return  The status of the operation
 *    \arg  DRV_ERROR   Validation failure
 *    \arg  DRV_READY   Success
 */
drv_status_en psa_set_location (psa_t *psa, double lon, double lat)
{
   /*
    * Validate inputs
    */
   if ((lon < -180) || (lon > 180))    return DRV_ERROR;
   if ((lat < -90 ) || (lat > 90 ))    return DRV_ERROR;

   psa->loc.longitude = lon;
   psa->loc.latitude = lat;

   return DRV_READY;
}

/*
 * ============ User Functions ===========
 */


/*!
 * \brief
 *    Calculate Sun position using input data stored in psa
 * \param   spa      Pointer to SPA structure to use for calculations
 * \return  The results in \ref  psa_output_t structure
 */
psa_output_t psa_calculate (psa_t *psa)
{
   psa_output_t   out;

   // Helper variables
   double ecliptic_longitude;
   double ecliptic_obliquity;
   double right_ascension;
   double declination;

   /* Calculate ecliptic coordinates */
   _ecliptic_coordinates (psa->t.jd, &ecliptic_longitude, &ecliptic_obliquity);
   /* Calculate celestial coordinates */
   _celestial_coordinates (ecliptic_longitude, ecliptic_obliquity, &right_ascension, &declination);
   /* Calculate local coordinates */
   _local_coordinates (psa, right_ascension, declination, &out.zenith, &out.azimuth);
   return out;
}


