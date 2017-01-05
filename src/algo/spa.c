/*
 * \file
 *    spa.c
 * \brief
 *    Based on sun position algorithm from Measurement & Instrumentation Team
 *    Solar Radiation Research Laboratory National Renewable Energy Laboratory
 *    1617 Cole Blvd,  Golden,  CO 80401
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
 *
 */
#include <algo/spa.h>

/*
 * ========== Private defines ==============
 */
#ifdef M_PI
#define PI           M_PI
#else
#define PI           (3.1415926535897932384626433832795028841971)
#endif

#define SUN_RADIUS   (0.26667)

#include <algo/spa_per_terms.h>     /*!< Define Terms as private data */

/*
 * Private data types
 */
typedef struct {
   double jd;          /*!< Julian day */
   double jc;          /*!< Julian century */
   double jde;         /*!< Julian ephemeris day */
   double jce;         /*!< Julian ephemeris century */
   double jme;         /*!< Julian ephemeris millennium */

   double l;           /*!< earth heliocentric longitude [degrees] */
   double b;           /*!< earth heliocentric latitude [degrees] */
   double r;           /*!< earth radius vector [Astronomical Units,  AU] */

   double theta;       /*!< geocentric longitude [degrees] */
   double beta;        /*!< geocentric latitude [degrees] */

   double x0;          /*!< mean elongation (moon-sun) [degrees] */
   double x1;          /*!< mean anomaly (sun) [degrees] */
   double x2;          /*!< mean anomaly (moon) [degrees] */
   double x3;          /*!< argument latitude (moon) [degrees] */
   double x4;          /*!< ascending longitude (moon) [degrees] */

   double del_psi;     /*!< nutation longitude [degrees] */
   double del_epsilon; /*!< nutation obliquity [degrees] */
   double epsilon0;    /*!< ecliptic mean obliquity [arc seconds] */
   double epsilon;     /*!< ecliptic true obliquity  [degrees] */

   double del_tau;     /*!< aberration correction [degrees] */
   double lamda;       /*!< apparent sun longitude [degrees] */
   double nu0;         /*!< Greenwich mean sidereal time [degrees] */
   double nu;          /*!< Greenwich sidereal time [degrees] */

   double alpha;       /*!< geocentric sun right ascension [degrees] */
   double delta;       /*!< geocentric sun declination [degrees] */

   double h;           /*!< observer hour angle [degrees] */
   double xi;          /*!< sun equatorial horizontal parallax [degrees] */
   double delta_alpha; /*!< sun right ascension parallax [degrees] */
   double delta_prime; /*!< topocentric sun declination [degrees] */
   double alpha_prime; /*!< topocentric sun right ascension [degrees] */
   double h_prime;     /*!< topocentric local hour angle [degrees] */

   double e0;          /*!< topocentric elevation angle (uncorrected) [degrees] */
   double del_e;       /*!< atmospheric refraction correction [degrees] */
   double e;           /*!< topocentric elevation angle (corrected) [degrees] */

   double eot;         /*!< equation of time [minutes] */
   double srha;        /*!< sunrise hour angle [degrees] */
   double ssha;        /*!< sunset hour angle [degrees] */
   double sta;         /*!< sun transit altitude [degrees] */
} _spa_data_t;

static _spa_data_t _spa_data;


/*
 * ============= Sun position Macros ==============
 */
#define  _int(_f_)      ((int)(_f_))         /*! Cast to integer */
#ifndef _rad2deg
#define _rad2deg(_r_)   ((180.0/PI)*(_r_))   /*!< rad to degrees converter */
#endif
#ifndef _deg2rad
#define _deg2rad(_d_)   ((PI/180.0)*(_d_))   /*!< degrees to rad converter */
#endif

/*! Calculate the 3rd order polynomial */
#define  _pol_3rd(_a_, _b_, _c_, _d_, _x_)   \
   ((((_a_)*(_x_) + (_b_))*(_x_) + (_c_))*(_x_) + (_d_))

#define _julian_century(_jd_)                (((_jd_) - 2451545.0) / 36525.0)
#define _julian_millennium(_jc_)             ((_jc_)/10.0)
#define _julian_eph_day(_jd_, _dt_)          ((_jd_)+((_dt_)/86400.0))
//#define  _julian_eph_century(_jde_)          (((_jde_) - 2451545.0) / 36525.0);

/*! Convert heliocentric longitude to geocentric [deg] */
#define _hel2geo_longitude(_l_)              (_wrap ((_l_)+180, 0, 360))

/*! Convert heliocentric latitude to geocentric [deg] */
#define _hel2geo_latitude(_l_)               (-(_l_))

#define _mean_elongation_moon_sun(_jce_)     \
   _pol_3rd (1.0/189474.0, -0.0019142, 445267.11148, 297.85036, (_jce_))

#define _mean_anomaly_sun(_jce_)      \
   _pol_3rd (-1.0/300000.0, -0.0001603, 35999.05034, 357.52772, (_jce_))

#define _mean_anomaly_moon(_jce_)     \
   _pol_3rd (1.0/56250.0, 0.0086972, 477198.867398, 134.96298, (_jce_))

#define _argument_latitude_moon(_jce_)   \
   _pol_3rd (1.0/327270.0, -0.0036825, 483202.017538, 93.27191, (_jce_))

#define _ascending_longitude_moon(_jce_) \
   _pol_3rd (1.0/450000.0, 0.0020708, -1934.136261, 125.04452, (_jce_))

/*! Return the corrected obliquity */
#define _obliquity_correction(_de_, _e0_)    ((_de_) + (_e0_)/3600.0)


#define _aberration_correction(_r_)             (-20.4898 / (3600.0*(_r_)))
#define _apparent_sun_lon(_th_, _dpsi_, _dt_)   ((_th_) + (_dpsi_) + (_dt_))

#define _greenwich_mean_sidereal_time(_jd_, _jc_)        \
    _wrap (280.46061837                                  \
              + 360.98564736629 * ((_jd_) - 2451545.0)   \
              + (_jc_)*(_jc_)*(0.000387933               \
              - (_jc_)/38710000.0),                      \
           0, 360)


#define _greenwich_sidereal_time(_nu0_, _dpsi_, _e_)     \
    ((_nu0_) + (_dpsi_)*cos (_deg2rad (_e_)))


#define _hour_angle(_nu_, _lon_, _alpha_deg_)   _wrap ((_nu_)+(_lon_)-(_alpha_deg_), 0, 360)
#define _horizontal_parallax(_r_)               (8.794 / (3600.0 * (_r_)))


#define _top_right_ascension(_adeg_, _da_)      ((_adeg_) + (_da_))
#define _top_hour_angle(_h_, _da_)              ((_h_) - (_da_))

#define _top_elevation_angle_correction(_e0_, _de_)   ((_e0_) + (_de_))
#define _top_zenith_angle(_e_)                        (90.0 - (_e_))

#define _top_azimuth_angle(_az_astro_)          _wrap((_az_astro_) + 180.0, 0, 360)

/*
 * ================== Sun RTS Macros ==================
 */
/*! Convert fractional day to local hour */
#define  _dayfrac_to_local_hr(_df_, _tz_)    \
   (24.0 * _wrap (((_df_) + (_tz_)/24.0), 0, 1))

#define _rts_sun_mean_lon(_jme_)                                     \
   _wrap (280.4664567 + _jme_*(360007.6982779 + _jme_*(0.03032028 +  \
          _jme_*(1/49931.0 + _jme_*(-1/15300.0 + _jme_*(-1/2000000.0))))), 0, 360)

#define _rts_eot(_m_, _alpha_, _dpsi_, _epsilon_)                    \
      _wrap_min (4.0*(_m_ - 0.0057183 - _alpha_ + _dpsi_*cos(_deg2rad(_epsilon_))))

#define _rts_approx_sun_transit_time(_alpha_zero_, _lon_, _nu_)      \
   ((_alpha_zero_ - _lon_ - _nu_) / 360.0)

#define _rts_sun_rise_and_set(pm_rts, ph_rts, pdprime, lat, ph_prime, h0_prime, sun)      \
      (pm_rts[sun] + (ph_rts[sun] - h0_prime)                                             \
   / (360.0*cos(_deg2rad(pdprime[sun]))*cos(_deg2rad(lat))*sin(_deg2rad(ph_prime[sun]))))

/*
 * ================= private API =================
 */

/*
 * Sun position
 */
static double _wrap (double x, double a, double b);
static double _julian_day (spa_time_t st);
static double _periodic_term_sum(const double terms[][TERM_COUNT], int count, double jme);
static double _earth_values (double term_sum[], int count, double jme);
static double _earth_hel_longitude (double jme);
static double _earth_hel_latitude (double jme);
static double _earth_radius_vector (double jme);
static double _xy_term_sum (int i, double x[TERM_X_COUNT]);
static void   _nutation (double jce, double x[TERM_X_COUNT], double *del_psi, double *del_epsilon);
static double _ecliptic_mean_obliquity(double jme);
static double _geo_right_ascension(double lamda, double epsilon, double beta);
static double _geo_declination(double beta, double epsilon, double lamda);
static void   _delta_alpha_prime(spa_location_t sl, double xi, double h, double delta, double *delta_alpha, double *delta_prime);
static double _top_elevation_angle (double latitude, double delta_prime, double h_prime);
static double _refraction_correction(spa_atmos_t sa, double e0);
static double _top_azimuth_angle_astro(double h_prime, double latitude, double delta_prime);
static double _incidence_angle (spa_output_t so, spa_location_t sl);
static void   _geo_sun_ra_and_decl (double delta_t);

/*
 * Sun Rise transit and set
 */
static double _wrap_min (double m);
static double _rts_hour_angle_at_rise_set (double latitude, double delta_zero, double h0_prime);
static void   _rts_approx_rise_and_set (double *m_rts, double h0);
static double _rts_alpha_delta_prime (double *ad, double n);
static double _rts_sun_altitude(double latitude, double delta_prime, double h_prime);
static void   _eot_and_sun_rts(spa_t *spa, _spa_data_t *_spa_data, spa_output_t *spa_out);


/*
 * \brief
 *    Wrap a periodic result to a interval of [a, b]
 * \param   x  The number to wrap/limit
 * \param   a  The interval first number
 * \param   b  The interval second number
 * \return  The wrapped number
 */
static double _wrap (double x, double a, double b)
{
   double   b_a = b-a;              /* take interval */
   int      p = _int(x)/_int(b_a);  /* find periods */

   x -= p*b_a;
   x += (x<a) ? b_a : 0;
   x -= (x>b) ? b_a : 0;
   return x;
}


/*!
 * \brief
 *    Calculate Fractional Julian day
 * \param   st    SPA time
 * \return  Fractional Julian day
 */
static double _julian_day (spa_time_t st)
{
   double day_dec, julday, a;

   day_dec = st.day + (st.hour - st.timezone + (st.min + (st.sec + st.delta_ut1)/60.0)/60.0)/24.0;

   if (st.mon < 3) {
      st.mon += 12;
      st.year--;
   }

   julday = _int(365.25*(st.year+4716.0)) + _int(30.6001*(st.mon+1)) + day_dec - 1524.5;

   if (julday > 2299160.0) {
      a = _int(st.year/100);
      julday += (2 - a + _int(a/4));
   }

   return julday;
}

/*!
 * \brief
 *    Calculate the earth periodic terms summation
 */
static double _periodic_term_sum(const double terms[][TERM_COUNT], int count, double jme)
{
   int i;
   double s=0;

   for (i=0, s=0; i < count; i++)
      s += terms[i][TERM_A]*cos(terms[i][TERM_B]+terms[i][TERM_C]*jme);
   return s;
}

/*!
 * return rad
 */
static double _earth_values (double term_sum[], int count, double jme)
{
   int i;
   double s=0;

   for (i=0, s=0; i < count; i++)
      s += term_sum[i]*pow(jme, i);

   return (s /= 1.0e8);
}

/*!
 * \brief
 *    Calculate Earth's Heliocentric longitude
 * \return [deg]
 */
static double _earth_hel_longitude (double jme)
{
   double sum[L_COUNT];
   int i;

   for (i = 0; i < L_COUNT; i++)
      sum[i] = _periodic_term_sum (L_TERMS[i], l_subcount[i], jme);

   return _wrap (_rad2deg (_earth_values (sum, L_COUNT, jme)), 0, 360);
}

/*!
 * \brief
 *    Calculate Earth's heliocentric latitude
 * \return [deg]
 */
static double _earth_hel_latitude (double jme)
{
   double sum[B_COUNT];
   int i;

   for (i = 0; i < B_COUNT; i++)
      sum[i] = _periodic_term_sum (B_TERMS[i], b_subcount[i], jme);

   return _rad2deg (_earth_values (sum, B_COUNT, jme));
}

/*!
 * \brief
 *    Calculate earth's radius vector
 * \return  [rad]
 */
static double _earth_radius_vector (double jme)
{
   double sum[R_COUNT];
   int i;

   for (i = 0; i < R_COUNT; i++)
      sum[i] = _periodic_term_sum (R_TERMS[i], r_subcount[i], jme);

   return _earth_values (sum, R_COUNT, jme);
}

/*!
 * \return  [deg]
 */
static double _xy_term_sum (int i, double x[TERM_X_COUNT])
{
   int j;
   double sum=0;

   for (j = 0; j < TERM_Y_COUNT; j++)
      sum += x[j]*Y_TERMS[i][j];

   return sum;
}

/*!
 * \Calculate Nutation longitude and obliquity
 * \return  [rad]
 */
static void _nutation (double jce, double x[TERM_X_COUNT], double *del_psi, double *del_epsilon)
{
   int i;
   double xy_term_sum, sum_psi=0, sum_epsilon=0;

   for (i = 0; i < Y_COUNT; i++) {
      xy_term_sum  = _deg2rad (_xy_term_sum (i, x));
      sum_psi     += (PE_TERMS[i][TERM_PSI_A] + jce*PE_TERMS[i][TERM_PSI_B])*sin(xy_term_sum);
      sum_epsilon += (PE_TERMS[i][TERM_EPS_C] + jce*PE_TERMS[i][TERM_EPS_D])*cos(xy_term_sum);
   }

   *del_psi     = sum_psi     / 36000000.0;
   *del_epsilon = sum_epsilon / 36000000.0;
}

/*!
 * \brief
 *    Calculate the eliptic mean obliquity
 */
static double _ecliptic_mean_obliquity(double jme)
{
   double u = jme/10.0;

   return 84381.448 + u*(-4680.93 + u*(-1.55 + u*(1999.25 + u*(-51.38 + u*(-249.67 +
                    u*(  -39.05 + u*( 7.12 + u*(  27.87 + u*(  5.79 + u*2.45)))))))));
}

/*!
 * \brief
 *    Calculate geocentric right ascension
 * \return  [deg]
 */
static double _geo_right_ascension(double lamda, double epsilon, double beta)
{
   double lamda_rad   = _deg2rad (lamda);
   double epsilon_rad = _deg2rad (epsilon);

   return _wrap (_rad2deg (
                    atan2( sin (lamda_rad)*cos (epsilon_rad) - tan (_deg2rad (beta))*sin (epsilon_rad),
                           cos (lamda_rad))),
                 0, 360);
}

/*!
 * \brief
 *    Calculate geocentric declination
 * \return  [deg]
 */
static double _geo_declination(double beta, double epsilon, double lamda)
{
   double beta_rad    = _deg2rad (beta);
   double epsilon_rad = _deg2rad (epsilon);

   return _rad2deg (asin (sin(beta_rad)*cos(epsilon_rad) +
                          cos(beta_rad)*sin(epsilon_rad)*sin(_deg2rad(lamda))));
}

/*
 * \brief
 *    Calculate topocentric declination and right ascension parallax at once
 * \return  [dec]
 */
static void _delta_alpha_prime(spa_location_t sl, double xi, double h, double delta, double *delta_alpha, double *delta_prime)
{
   double delta_alpha_rad;
   double lat_rad   = _deg2rad (sl.latitude);
   double xi_rad    = _deg2rad (xi);
   double h_rad     = _deg2rad (h);
   double delta_rad = _deg2rad (delta);
   double u = atan(0.99664719 * tan(lat_rad));
   double y = 0.99664719 * sin(u) + sl.elevation*sin(lat_rad)/6378140.0;
   double x =              cos(u) + sl.elevation*cos(lat_rad)/6378140.0;

   delta_alpha_rad =      atan2(                - x*sin(xi_rad) *sin(h_rad),
                                 cos(delta_rad) - x*sin(xi_rad) *cos(h_rad));

   *delta_prime = _rad2deg (atan2((sin(delta_rad) - y*sin(xi_rad))*cos(delta_alpha_rad),
                                   cos(delta_rad) - x*sin(xi_rad) *cos(h_rad)));

   *delta_alpha = _rad2deg (delta_alpha_rad);
}


static double _top_elevation_angle (double latitude, double delta_prime, double h_prime)
{
   double lat_rad         = _deg2rad (latitude);
   double delta_prime_rad = _deg2rad (delta_prime);

   return _rad2deg (asin(sin(lat_rad)*sin(delta_prime_rad) +
                         cos(lat_rad)*cos(delta_prime_rad) * cos(_deg2rad(h_prime))));
}

static double _refraction_correction(spa_atmos_t sa, double e0)
{
   double del_e = 0;

   if (e0 >= -1*(SUN_RADIUS + sa.refract))
      del_e = (sa.pressure / 1010.0) * (283.0 / (273.0 + sa.temperature)) *
              1.02 / (60.0 * tan(_deg2rad(e0 + 10.3/(e0 + 5.11))));
   return del_e;
}


static double _top_azimuth_angle_astro(double h_prime, double latitude, double delta_prime)
{
   double h_prime_rad = _deg2rad(h_prime);
   double lat_rad     = _deg2rad(latitude);

   return _wrap (_rad2deg (atan2(sin(h_prime_rad),
                                 cos(h_prime_rad)*sin(lat_rad) - tan(_deg2rad(delta_prime))*cos(lat_rad))), 0, 360);
}


static double _incidence_angle (spa_output_t so, spa_location_t sl)
{
   double zenith_rad = _deg2rad (so.zenith);
   double slope_rad  = _deg2rad (sl.slope);

   return _rad2deg(acos(cos(zenith_rad)*cos(slope_rad)  +
                        sin(slope_rad )*sin(zenith_rad) *
                           cos(_deg2rad(so.azimuth_astro - sl.azm_rotation))));
}

/*!
 * \brief
 *    Calculate required SPA parameters to get the right ascension (alpha) and declination (delta)
 * \note
 *    Julian day must be already calculated and in structure
 */
static void _geo_sun_ra_and_decl (double delta_t)
{
    double x[TERM_X_COUNT];

    _spa_data.jde = _julian_eph_day (_spa_data.jd, delta_t);
    _spa_data.jc  = _julian_century (_spa_data.jd);
    _spa_data.jce = _julian_century (_spa_data.jde);
    _spa_data.jme = _julian_millennium (_spa_data.jce);

    _spa_data.l = _earth_hel_longitude (_spa_data.jme);
    _spa_data.b = _earth_hel_latitude (_spa_data.jme);
    _spa_data.r = _earth_radius_vector (_spa_data.jme);

    _spa_data.theta = _hel2geo_longitude (_spa_data.l);
    _spa_data.beta = _hel2geo_latitude(_spa_data.b);

    x[TERM_X0] = _spa_data.x0 = _mean_elongation_moon_sun (_spa_data.jce);
    x[TERM_X1] = _spa_data.x1 = _mean_anomaly_sun (_spa_data.jce);
    x[TERM_X2] = _spa_data.x2 = _mean_anomaly_moon (_spa_data.jce);
    x[TERM_X3] = _spa_data.x3 = _argument_latitude_moon (_spa_data.jce);
    x[TERM_X4] = _spa_data.x4 = _ascending_longitude_moon (_spa_data.jce);

    _nutation (_spa_data.jce, x, &_spa_data.del_psi, &_spa_data.del_epsilon);

    _spa_data.epsilon0 = _ecliptic_mean_obliquity (_spa_data.jme);
    _spa_data.epsilon  = _obliquity_correction (_spa_data.del_epsilon, _spa_data.epsilon0);

    _spa_data.del_tau   = _aberration_correction(_spa_data.r);
    _spa_data.lamda     = _apparent_sun_lon (_spa_data.theta, _spa_data.del_psi, _spa_data.del_tau);
    _spa_data.nu0       = _greenwich_mean_sidereal_time (_spa_data.jd, _spa_data.jc);
    _spa_data.nu        = _greenwich_sidereal_time (_spa_data.nu0, _spa_data.del_psi, _spa_data.epsilon);

    _spa_data.alpha = _geo_right_ascension (_spa_data.lamda, _spa_data.epsilon, _spa_data.beta);
    _spa_data.delta = _geo_declination (_spa_data.beta, _spa_data.epsilon, _spa_data.lamda);
}


/*
 * ========= Sun Rise transit and set private api ===========
 */

/*!
 * Wrap minutes
 */
static double _wrap_min (double m)
{
   if      (m < -20.0) m += 1440.0;
   else if (m >  20.0) m -= 1440.0;
   return m;
}

static double _rts_hour_angle_at_rise_set (double latitude, double delta_zero, double h0_prime)
{
   double h0             = -99999;
   double latitude_rad   = _deg2rad (latitude);
   double delta_zero_rad = _deg2rad (delta_zero);
   double argument       = (sin(_deg2rad(h0_prime)) - sin(latitude_rad)*sin(delta_zero_rad)) /
                                                     (cos(latitude_rad)*cos(delta_zero_rad));

   if (fabs(argument) <= 1) h0 = _wrap (_rad2deg(acos(argument)), 0, 180);

   return h0;
}

static void _rts_approx_rise_and_set (double *m_rts, double h0)
{
   double h0_dfrac = h0/360.0;

   m_rts[SUN_RISE]    = _wrap ((m_rts[SUN_TRANSIT] - h0_dfrac), 0, 1);
   m_rts[SUN_SET]     = _wrap ((m_rts[SUN_TRANSIT] + h0_dfrac), 0, 1);
   m_rts[SUN_TRANSIT] = _wrap ((m_rts[SUN_TRANSIT]), 0, 1);
}

static double _rts_alpha_delta_prime (double *ad, double n)
{
   double a = ad[JD_ZERO] - ad[JD_MINUS];
   double b = ad[JD_PLUS] - ad[JD_ZERO];

   if (fabs(a) >= 2.0) a = _wrap (a, 0, 1);
   if (fabs(b) >= 2.0) b = _wrap (b, 0, 1);

   return ad[JD_ZERO] + n * (a + b + (b-a)*n)/2.0;
}

static double _rts_sun_altitude(double latitude, double delta_prime, double h_prime)
{
   double latitude_rad    = _deg2rad(latitude);
   double delta_prime_rad = _deg2rad(delta_prime);

   return _rad2deg (asin (sin(latitude_rad)*sin(delta_prime_rad) +
                          cos(latitude_rad)*cos(delta_prime_rad)*cos(_deg2rad(h_prime))));
}


/*!
 * \brief
 *    Calculate Equation of time sun rise, transit and set
 */
static void _eot_and_sun_rts(spa_t *spa, _spa_data_t *_spa_data, spa_output_t *spa_out)
{
   _spa_data_t sun_rts = *_spa_data;
   spa_time_t  utc_rts = spa->utc;
   double nu, m, h0, n;
   double alpha[JD_COUNT], delta[JD_COUNT];
   double m_rts[SUN_COUNT], nu_rts[SUN_COUNT], h_rts[SUN_COUNT];
   double alpha_prime[SUN_COUNT], delta_prime[SUN_COUNT], h_prime[SUN_COUNT];
   double h0_prime = -1*(SUN_RADIUS + spa->atmos.refract);
   int i;

   m           = _rts_sun_mean_lon (_spa_data->jme);
   sun_rts.eot = _rts_eot (m, sun_rts.alpha, sun_rts.del_psi, sun_rts.epsilon);

   // Clear hour and keep Date only
   utc_rts.hour = utc_rts.min = utc_rts.sec = 0;
   utc_rts.delta_ut1 = utc_rts.timezone = 0.0;

   sun_rts.jd = _julian_day (utc_rts);

   _geo_sun_ra_and_decl (utc_rts.delta_t);
   nu = sun_rts.nu;

   utc_rts.delta_t = 0;
   sun_rts.jd--;
   for (i = 0; i < JD_COUNT; i++) {
      _geo_sun_ra_and_decl (utc_rts.delta_t);
     alpha[i] = sun_rts.alpha;
     delta[i] = sun_rts.delta;
     sun_rts.jd++;
   }

   m_rts[SUN_TRANSIT] = _rts_approx_sun_transit_time (alpha[JD_ZERO], spa->loc.longitude, nu);
   h0 = _rts_hour_angle_at_rise_set (spa->loc.latitude, delta[JD_ZERO], h0_prime);

   if (h0 >= 0) {

      _rts_approx_rise_and_set (m_rts, h0);

     for (i=0; i<SUN_COUNT; ++i) {

         nu_rts[i]      = nu + 360.985647*m_rts[i];

         n              = m_rts[i] + spa->utc.delta_t/86400.0;
         alpha_prime[i] = _rts_alpha_delta_prime (alpha, n);
         delta_prime[i] = _rts_alpha_delta_prime (delta, n);

         h_prime[i]     = _wrap ((nu_rts[i] + spa->loc.longitude - alpha_prime[i]), -180, 180);

         h_rts[i]       = _rts_sun_altitude (spa->loc.latitude, delta_prime[i], h_prime[i]);
     }

     _spa_data->srha = h_prime[SUN_RISE];
     _spa_data->ssha = h_prime[SUN_SET];
     _spa_data->sta  = h_rts[SUN_TRANSIT];

     spa_out->suntransit =
        _dayfrac_to_local_hr ((m_rts[SUN_TRANSIT] - h_prime[SUN_TRANSIT] / 360.0), spa->utc.timezone);

     spa_out->sunrise =
        _dayfrac_to_local_hr (
           _rts_sun_rise_and_set(m_rts, h_rts, delta_prime, spa->loc.latitude, h_prime, h0_prime, SUN_RISE),
           spa->utc.timezone);

     spa_out->sunset  =
        _dayfrac_to_local_hr (
           _rts_sun_rise_and_set (m_rts, h_rts, delta_prime, spa->loc.latitude, h_prime, h0_prime, SUN_SET),
           spa->utc.timezone);

   } else {
      _spa_data->srha = _spa_data->ssha = _spa_data->sta = -99999;
      spa_out->suntransit= spa_out->sunrise= spa_out->sunset= -99999;
   }

}

/*
 * ================== PUBLIC API ===================
 */

/*
 * ============= Set functions ============
 */

/*
 * \brief
 *    Set time for SPA algorithm
 * \param   spa      Pointer to SPA structure to use for calculations
 * \param   utc      Universal time in UNIX time format
 * \param   dut      Fractional second difference between UTC and UT which is used
 *                   to adjust UTC for earth's irregular rotation rate and is derived
 *                   from observation only and is reported in this bulletin:
 *                   http://maia.usno.navy.mil/ser7/ser7.dat,
 *                   where dut = DUT1
 *                   valid range: -1 to 1 second (exclusive)
 * \param   dt       Difference between earth rotation time and terrestrial time
 *                   It is derived from observation only and is reported in this
 *                   bulletin: http://maia.usno.navy.mil/ser7/ser7.dat,
 *                   where dt = 32.184 + (TAI-UTC) - DUT1
 *                   valid range: -8000 to 8000 seconds
 * \param   tz       Observer time zone (negative west of Greenwich)
 *                   valid range: -18   to   18 hours
 * \return  The status of the operation
 *    \arg  DRV_ERROR   Validation failure
 *    \arg  DRV_READY   Success
 */
drv_status_en spa_set_time (spa_t *spa, time_t utc, double dut1, double dt, double tz)
{
   struct tm   *t;   /* lib time */
   spa_time_t  st;   /* spa time */

   /*
    * Get time from UNIX time and parameters
    */
   t = sgmtime (&utc);
   st.year = _TM_YEAR_2_YEAR(t->tm_year);
   st.mon = _TM_MON_2_MON(t->tm_mon);
   st.day = t->tm_mday;
   st.hour = t->tm_hour;
   st.min = t->tm_min;
   st.sec = t->tm_sec;
   st.delta_ut1 = dut1;
   st.delta_t = dt;
   st.timezone = tz;

   /*
    * Validate inputs
    */
   if ((st.year < -2000) || (st.year > 6000))   return DRV_ERROR;
   if ((st.mon  < 1    ) || (st.mon  > 12  ))   return DRV_ERROR;
   if ((st.day  < 1    ) || (st.day  > 31  ))   return DRV_ERROR;
   if ((st.hour < 0    ) || (st.hour > 24  ))   return DRV_ERROR;
   if ((st.min  < 0    ) || (st.min  > 59  ))   return DRV_ERROR;
   if ((st.sec  < 0    ) || (st.sec  >=60  ))   return DRV_ERROR;

   if ((st.delta_ut1 <=-1  ) || (st.delta_ut1 >=1   ))   return DRV_ERROR;
   if ((st.delta_t   <-8000) || (st.delta_t   > 8000))   return DRV_ERROR;
   if ((st.timezone  <-18  ) || (st.timezone  > 18  ))   return DRV_ERROR;

   spa->utc = st;
   return DRV_READY;
}

/*
 * \brief
 *    Set location for SPA algorithm
 * \param   spa      Pointer to SPA structure to use for calculations
 * \param   lon      Observer longitude (negative west of Greenwich)
 *                   valid range: -180  to  180 degrees
 * \param   lat      Observer latitude (negative south of equator)
 *                   valid range: -90   to   90 degrees
 * \param   el       Observer elevation [meters]
 *                   valid range: -6500000 or higher meters
 * \param   sl       Surface slope (measured from the horizontal plane)
                     valid range: -360 to 360 degrees
 * \param   azr      Surface azimuth rotation (measured from south to projection of
 *                   surface normal on horizontal plane, negative east)
 *                   valid range: -360 to 360 degrees
 * \return  The status of the operation
 *    \arg  DRV_ERROR   Validation failure
 *    \arg  DRV_READY   Success
 */
drv_status_en spa_set_location (spa_t *spa, double lon,  double lat,  double el, double sl, double azr)
{
   /*
    * Validate inputs
    */
   if ((lon < -180) || (lon > 180))    return DRV_ERROR;
   if ((lat < -90 ) || (lat > 90 ))    return DRV_ERROR;
   if ((sl  < -360) || (sl  > 360))    return DRV_ERROR;
   if ((azr < -360) || (azr > 360))    return DRV_ERROR;

   if (el < -6500000)                  return DRV_ERROR;

   spa->loc.longitude = lon;
   spa->loc.latitude = lat;
   spa->loc.elevation = el;
   spa->loc.slope = sl;
   spa->loc.azm_rotation = azr;

   return DRV_READY;
}

/*
 * \brief
 *    Set atmospheric conditions for SPA algorithm
 * \param   spa      Pointer to SPA structure to use for calculations
 * \param   pr       Annual average local pressure [millibars]
 *                   valid range:    0 to 5000 millibars
 * \param   temp     Annual average local temperature [degrees Celsius]
 *                   valid range: -273 to 6000 degrees Celsius
 * \param   ar       Atmospheric refraction at sunrise and sunset (0.5667 deg is typical)
 *                   valid range: -5   to   5 degrees
 * \return  The status of the operation
 *    \arg  DRV_ERROR   Validation failure
 *    \arg  DRV_READY   Success
 */
drv_status_en spa_set_atmos (spa_t *spa, double pr, double temp, double ar)
{
   if ((pr   < 0  ) || (pr   > 5000))  return DRV_ERROR;
   if ((temp <-273) || (temp > 6000))  return DRV_ERROR;
   if ((ar   <-5  ) || (ar   > 5   ))  return DRV_ERROR;

   spa->atmos.pressure = pr;
   spa->atmos.temperature = temp;
   spa->atmos.refract = ar;

   return DRV_READY;
}


/*
 * ============ User Functions ===========
 */

/*!
 * \brief
 *    Calculate Sun position using input data stored in spa
 * \param   spa      Pointer to SPA structure to use for calculations
 * \param   fun      Select which output data to calculate
 *    \arg  SPA_ZA      calculate zenith and azimuth
 *    \arg  SPA_ZA_INC  calculate zenith, azimuth, and incidence
 *    \arg  SPA_ZA_RTS  calculate zenith, azimuth, and sun rise/transit/set values
 *    \arg  SPA_ALL     calculate all SPA output values
 * \return  The results in \ref  spa_output_t structure
 */
spa_output_t spa_calculate (spa_t *spa, spa_func_en fun)
{
   spa_output_t ret;

   _spa_data.jd = _julian_day (spa->utc);
   _geo_sun_ra_and_decl (spa->utc.delta_t);

   _spa_data.h  = _hour_angle (_spa_data.nu, spa->loc.longitude, _spa_data.alpha);
   _spa_data.xi = _horizontal_parallax (_spa_data.r);

   _delta_alpha_prime (spa->loc, _spa_data.xi, _spa_data.h, _spa_data.delta,
                       &_spa_data.delta_alpha, &_spa_data.delta_prime);

   _spa_data.alpha_prime = _top_right_ascension (_spa_data.alpha, _spa_data.delta_alpha);
   _spa_data.h_prime     = _top_hour_angle (_spa_data.h, _spa_data.delta_alpha);

   _spa_data.e0      = _top_elevation_angle (spa->loc.latitude, _spa_data.delta_prime, _spa_data.h_prime);
   _spa_data.del_e   = _refraction_correction (spa->atmos, _spa_data.e0);
   _spa_data.e       = _top_elevation_angle_correction (_spa_data.e0, _spa_data.del_e);

   ret.zenith        = _top_zenith_angle (_spa_data.e);
   ret.azimuth_astro = _top_azimuth_angle_astro (_spa_data.h_prime, spa->loc.latitude, _spa_data.delta_prime);
   ret.azimuth       = _top_azimuth_angle (ret.azimuth_astro);

   switch (fun) {
      default:
      case SPA_ZA:      break;
      case SPA_ZA_INC:
         ret.incidence  = _incidence_angle (ret, spa->loc);    break;
      case SPA_ZA_RTS:
         _eot_and_sun_rts (spa, &_spa_data, &ret);             break;
      case SPA_ALL:
         ret.incidence  = _incidence_angle (ret, spa->loc);
         _eot_and_sun_rts (spa, &_spa_data, &ret);             break;
   }
   return ret;
}

