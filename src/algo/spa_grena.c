/*
 * \file
 *    spa_grena.c
 * \brief
 *    R Grena (2008), An algorithm for the computation of the solar position,
 *    Solar Energy (82), pp 462-470.
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
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
#include <algo/spa_grena.h>

/*
 * ============ Static Functions ==========
 */
inline static double _rad2deg (double rad) {
   return fabs (fmod ((180 * rad / M_PI), 360.0));
}

// Convert degree angle to radians
inline static double _deg2rad (double dec) {
   return fabs (fmod ((M_PI * dec / 180), 2*M_PI));
}

/*
 * ============= Link Functions ===========
 */

/*
 * ============= Set functions ============
 */
/*!
 * \brief
 *    Calculates and sets the fractional date and time in the form of
 *    Julian day. The time scale njd, used in the computation, is the
 *    Julian Day shifted to make it start at noon, 1st January 2003.
 * \param   spa   Pointer to linked data struct
 * \param   utc   UTC time in UNIX format
 * \param   dt    Difference between UT and terrestrial time
 */
void spa_set_time (spa_t *spa, time_t utc, double dt)
{
   struct tm *t;
   double year, month, ftime;

   // broke down utc time
   t = gmtime(&utc);
   ftime = t->tm_hour + t->tm_min / 60.0 + t->tm_sec / 3600.0;

   // round year and month
   month = (double)t->tm_mon + 1;
   year  = (double)t->tm_year + 1900;
   if (month <= 2) {
      year -= 1.0;
      month += 12.0;
   }

   // Normalised Julian day, Julian day and terrestrial time diff
   spa->njd = (double)trunc (365.25 * (year - 2000))
            + (double)trunc (30.6001 * (month + 1))
            + (double)t->tm_mday + ftime/24.0 - 1158.5;

   spa->jd  = spa->njd + 2452640;   // Add Julian day shift
   spa->delta_t = dt;
}

/*!
 * \brief
 *    Set latitude
 * \param   spa   Pointer to linked data struct
 * \param   lat   Latitude in Degrees.
 */
inline void spa_set_latitude (spa_t *spa, double lat) {
   spa->latitude = _deg2rad (lat);
}

/*!
 * \brief
 *    Set longitude
 * \param   spa   Pointer to linked data struct
 * \param   lon   Longitude in Degrees.
 */
inline void spa_set_longitude (spa_t *spa, double lon) {
   spa->longitude = _deg2rad (lon);
}

/*!
 * \brief
 *    Set Pressure
 * \param   spa   Pointer to linked data struct
 * \param   p     Pressure in Atm.
 */
inline void spa_set_pressure (spa_t *spa, double p) {
   spa->p = p;
}

/*!
 * \brief
 *    Set Temperature
 * \param   spa   Pointer to linked data struct
 * \param   p     Pressure in °C.
 */
inline void spa_set_temperature (spa_t *spa, double T) {
   spa->T = T;
}

/*
 * ============ User functions ===========
 */

/*!
 * \brief
 *    Initialise the spa data
 * \param   spa   Pointer to linked data struct
 */
void spa_init (spa_t *spa)
{
   if (spa->T == 0)  spa->T = 20;
   if (spa->p == 0)  spa->p = 1;
}

/*!
 * \brief
 *    Sun position algorithm based on Roberto Grena's paper
 *
 * \param   spa      Pointer to linked data struct
 * \param   elev     Pointer to elevation variable for the results in rad [-pi, pi]
 * \param   azimuth  Pointer to azimuth variable for the results in rad [0, 2pi]
 */
void spa_calculation (spa_t *spa, double *elev, double *azimuth)
{
   double t, t2;        // Global time and helper time variable
   double Lh;           // Heliocentric longitude
   double Dy;           // Geocentric longitude correction
   double epsilon;      // Earth axis inclination
   double y;            // Geocentric longitude
   double ar;           // Right ascension, in radians
   double D;            // Declination
   double h;            // Hour angle
   double Da;           // Parallax correction to right ascension
   //double at;           // Topocentric right ascension
   double Dt;           //Topocentric declination
   //double ht;           // Topocentric hour angle
   double e0;           // Solar elevation without refraction
   double De;           // Atmospheric refraction correction

   // Helper variables to speed up computations
   double s, sin_y;           // Angle and sine of Geocentric longitude y
   double cos_phy, sin_phy;   // Cosine and sine of latitude
   double cos_h, sin_h;       // Cosine and sine of hour angle
   double sin_Dt;             // Sin of Dt
   double cos_Dt;             // Cos of Dt
   double cht;                // Approximate cosine of ht
   double sht;                // Approximate sine of ht

   // Time calculation
   t = spa->njd + spa->delta_t / 86400;
   t2 = t/1000.0;

   /*
    *  Heliocentric longitude
    */
   // linear
   s = 1.72019e-2 * t - 0.0563;
   Lh = 1.740940 + 1.7202768683e-2 * t + 3.34118e-2 * sin(s) + 3.488e-4 * sin(2*s);
   // Add moon perturbation
   Lh += 3.13e-5 * sin(0.2127730*t - 0.585);
   // Add harmonic correction
   Lh += 1.26e-5 * sin(4.243e-3 * t + 1.46)
      + 2.35e-5 * sin(1.0727e-2 * t + 0.72)
      + 2.76e-5 * sin(1.5799e-2 * t + 2.35)
      + 2.75e-5 * sin(2.1551e-2 * t - 1.98)
      + 1.26e-5 * sin(3.1490e-2 * t - 0.80);

   // Add polynomial correction
   Lh += ((( -2.30796e-7 * t2 + 3.7976e-6) * t2 - 2.0458e-5) * t2 + 3.976e-5) * t2*t2;
   // to obtain obtain Heliocentric longitude in the range [0,2pi] uncomment:
   // Lh = fmod(Lh, 2*M_PI);

   // Correction to longitude due to nutation
   Dy = 8.33e-5 * sin (9.252e-4 * t - 1.173);
   // Earth axis inclination
   epsilon = -6.21e-9 * t + 0.409086 + 4.46e-5 * sin (9.252e-4 * t + 0.397);

   /*
    *  Geocentric global solar coordinates:
    */
   // Geocentric solar longitude
   y = Lh + M_PI + Dy - 9.932e-5;
   //helper calculation
   sin_y = sin (y);
   // Geocentric Right ascencion
   ar = atan2 (sin_y * cos (epsilon), cos (y));

   // Declination
   D = asin (sin (epsilon) * sin_y);

   // local hour angle of the sun
   h = 6.30038809903*spa->njd + 4.8824623 + 0.9174*Dy + spa->longitude - ar;
   // to obtain the local hour angle in the range [0,2pi] uncomment:
   //h = fmod(h,2*M_PI);

   // helper calculations
   cos_phy = cos(spa->latitude);
   sin_phy = sin(spa->latitude);
   cos_h = cos(h);
   sin_h = sin(h);

   // parallax correction to right ascension
   Da = -4.26e-5 * cos_phy * sin_h;
   // Topocentric right ascension
   //at = ar + Da;
   // Topocentric declination
   Dt = D - 4.26e-5 * (sin_phy - D * cos_phy);
   // Topocentric hour angle
   //ht = h - Da;

   // helper calculations
   sin_Dt = sin(Dt);
   cos_Dt = cos(Dt);
   cht = cos_h + Da * sin_h;
   sht = sin_h - Da * cos_h;

   // solar elevation angle, without refraction correction
   e0 =  asin(sin_phy * sin_Dt + cos_phy * cos_Dt * cht);

   // refraction correction: it is calculated only
   // if Elevation_no_refract > elev_min
   if (e0 > SPA_ELEVATION_REFRACTION_TH)
      De = 0.084217 * spa->p / (  (273 + spa->T) * tan (e0 + 0.0031376/(e0 + 0.089186)) );
   else
      De = 0;

   // local coordinates of the sun
   *elev = e0 - De;
   *azimuth = M_PI + atan2 (sht, cht*sin_phy - sin_Dt*cos_phy);
   /*
    * xxx:
    * The original equation returns azimuth [-pi, pi] with zero azimuth
    * towards south, and positive in the western hemisphere.
    * Shift azimuth pointing North, by adding pi to it. It is also shifted
    * in [0, 2pi).
    */
}
