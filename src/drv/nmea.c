/*
 * \file nmea.c
 * \brief
 *    A target independent NMEA 0183 parser
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
#include <drv/nmea.h>

/*
 * ============ Static data types ============
 */

/*
 * ========== NMEA Sentences ==========
 *
 * $GPGGA - Global Positioning System Fix Data
 * $GPGLL - Geographic position, latitude / longitude
 * $GPRMC - Recommended minimum specific GPS/Transit data
 * $GPVTG - Track made good and ground speed
 * $GPZDA - Date & Time
 */
static const char * _txt_sen_types[] = {
   "GGA",
   "GLL",
   "GSA"
   "GSV"
   "RMC",
   "VTG",
   "ZDA",
   ""
};

static const parse_obj_en _GGA[] = {
    _utc, _lat, _lat_s, _long, _long_s, _fix_t, _sats, _disc, _elev, _disc, _disc, _disc
};
static const parse_obj_en _GLL[] = {
   _lat, _lat_s, _long, _long_s, _aster, _crc
};
static const parse_obj_en _RMC[] = {
   _utc, _valid_t, _lat, _lat_s, _long, _long_s, _speed, _course, _date, _mag_var, _mag_var_s
};
static const parse_obj_en _VTG[] = {
   _course, _course_t, _course, _course_t, _speed, _sp_unts, _speed, _sp_unts
};
static const parse_obj_en _ZDA[] = {
   _utc, _day, _month, _year, _zone_h, _zone_m
};

/*
 * ============ Static API ============
 */
// tools
static int _checksum (char* str);
static int _match (char *sen, char *word, int n);

// Middle level
static int _get_sen (nmea_t *nmea);
static int _get_token (char *str, char *token);
static int _checksum_chk (char *str);

// Extract tools
static int _read_sen_type (char *str, NMEA_sentence_en *s);
static int _read_utc (char *str, utc_time_t *t);
static int _read_day (char *str, int *day);
static int _read_month (char *str, int *month);
static int _read_year (char *str, int *year);
static int _read_zone_h (char *str, int *zone_h);
static int _read_zone_m (char *str, int *zone_m);
static int _read_data (char *str, utc_date_t *date);
static int _read_lat (char *str, float *lat);
static int _read_lat_s (char *str, nmea_lat_sign_en *lat_s);
static int _read_long (char *str, float *lon);
static int _read_long_s (char *str, nmea_long_sign_en *lon_s);
static int _read_elev (char *str, float *elev);
static int _read_speed (char *str, float *speed);
static int _read_sp_unts (char *str, nmea_speed_units_en *sp_unts);
static int _read_course (char *str, float *course);
static int _read_course_type (char *str, nmea_course_en *course_type);
static int _read_mag_var (char *str, float *mag_var);
static int _read_mag_var_s (char *str, nmea_long_sign_en *mag_var_s);
static int _read_sats (char *str, float *sats);
static int _read_fix (char *str, nmea_fix_en *fix);
static int _read_valid (char *str, nmea_valid_en *valid);

/*
 * ============== tools ==============
 */
static int _checksum (char* str) {
   int c = 0;

   while (*str)
      c ^= *str++;
   return c;
}
static int _match (char *sen, char *word, int n)
{
   while (*sen) {
      if ( !strncmp (sen, word, n) )
         return 1;
      ++sen;
   }
   return 0;
}


/*
 * ============== Middle level ==============
 */
static int _get_sen (nmea_t *nmea)
{
   int i=0, st=0;
   char ch;

   do {
      ch = nmea->io.in ();
      if (ch == '$') { st = 1; i=0; }
      if (st)        nmea->buf[i++] = ch;
   }while (ch != '\n' && ch != 0 && i<nmea->buf_size);
   nmea->buf[i] = 0;
   return i;
}

static int _get_token (char *str, char *token)
{
   char *s = str;
   while (1) {
      if (NMEA_IS_DELIMITER (*s)) {
         ++s;
         *token = 0;
         return s - str - 1;
      }
      else
         *token++ = *s++;
   }
   return 0;
}

static int _checksum_chk (char *str)
{
   //char s[120];
   int cs;

   //sscanf (str, "$%s*%2X", s, &cs);
   return 1;//(cs == _checksum (s)) ? 1:0;
}



/*
 * ============== Extract tools ==============
 */
static int _read_sen_type (char *str, NMEA_sentence_en *s)
{
   int i;

   if (_match (str, (char *)_txt_sen_types[*s], 3)) {
      *s = i;
      return 1;
   }
   else
      return 0;
}
static int _read_utc (char *str, utc_time_t *t) {
   return (sscanf (str, "%2d%2d%2f", &t->hour, &t->min, &t->sec)  == 3) ? 1:0;
}
static int _read_day (char *str, int *day) {
   return (sscanf (str, "%d", day) == 1) ? 1:0;
}
static int _read_month (char *str, int *month) {
   return (sscanf (str, "%d", month) == 1) ? 1:0;
}
static int _read_year (char *str, int *year) {
   return (sscanf (str, "%d", year) == 1) ? 1:0;
}
static int _read_zone_h (char *str, int *zone_h) {
   return (sscanf (str, "%d", zone_h) == 1) ? 1:0;
}
static int _read_zone_m (char *str, int *zone_m) {
   return (sscanf (str, "%d", zone_m) == 1) ? 1:0;
}
static int _read_data (char *str, utc_date_t *date) {
   return (sscanf (str, "$%2d%2d%2d", &date->day, &date->month, &date->year) == 3) ? 1:0;
}
static int _read_lat (char *str, float *lat) {
   if (sscanf (str, "%f", lat) != 1)
      return 0;
   *lat /= 100;
   return 1;
}
static int _read_lat_s (char *str, nmea_lat_sign_en *lat_s) {
   char sign;
   *lat_s = NMEA_N;
   if (sscanf (str, "%c", &sign) != 1)
      return 0;
   if (sign == 'S' || sign == 's')
      *lat_s = NMEA_S;
   return 1;
}
static int _read_long (char *str, float *lon) {
   if (sscanf (str, "%f", lon) != 1)
      return 0;
   *lon /= 100;
   return 1;
}
static int _read_long_s (char *str, nmea_long_sign_en *lon_s) {
   char sign;
   *lon_s = NMEA_E;
   if (sscanf (str, "%c", &sign) != 1)
      return 0;
   if (sign == 'W' || sign == 'w')
      *lon_s = NMEA_W;
   return 1;
}
static int _read_elev (char *str, float *elev) {
   return (sscanf (str, "%f", elev) == 1) ? 1:0;
}
static int _read_speed (char *str, float *speed) {
   return (sscanf (str, "%f", speed) == 1) ? 1:0;
}
static int _read_sp_unts (char *str, nmea_speed_units_en *sp_unts) {
   char unt;
   *sp_unts = NMEA_KNOTS;
   if (sscanf (str, "%c", &unt) != 1)
      return 0;
   if (unt == 'K' || unt == 'k')
      *sp_unts = NMEA_KMH;
   return 1;
}
static int _read_course (char *str, float *course) {
   return (sscanf (str, "%f", course) == 1) ? 1:0;
}
static int _read_course_type (char *str, nmea_course_en *course_type) {
   char tpe;
   *course_type = NMEA_COURSE_MAG;
   if (sscanf (str, "%c", &tpe) != 1)
      return 0;
   if (tpe == 'T' || tpe == 't')
      *course_type = NMEA_COURSE_TRUE;
   return 1;
}
static int _read_mag_var (char *str, float *mag_var) {
   return (sscanf (str, "%f", mag_var) == 1) ? 1:0;
}
static int _read_mag_var_s (char *str, nmea_long_sign_en *mag_var_s) {
   char tpe;
   *mag_var_s = NMEA_E;
   if (sscanf (str, "%c", &tpe) != 1)
      return 0;
   if (tpe == 'W' || tpe == 'w')
      *mag_var_s = NMEA_W;
   return 1;
}
static int _read_sats (char *str, float *sats) {
   return (sscanf (str, "%f", sats) == 1) ? 1:0;
}
static int _read_fix (char *str, nmea_fix_en *fix) {
   int f;
   if (sscanf (str, "%d", &f) != 1)
      return 0;
   switch (f) {
      default:
      case 0: *fix = NMEA_NOT_FIX;  return 1;
      case 1: *fix = NMEA_FIX;      return 1;
      case 2: *fix = NMEA_DFIX;     return 1;
   }
}
static int _read_valid (char *str, nmea_valid_en *valid) {
   char v;
   *valid = NMEA_VALID;
   if (sscanf (str, "%c", &v) != 1)
      return 0;
   if (v == 'V' || v == 'v')
      *valid = NMEA_NOT_VALID;
   return 1;
}



/*
 * ============ Public GPS API ============
 */

/*
 * Link and Glue functions
 */
void nmea_link_buffer (nmea_t *nmea, byte_t *b) {
   nmea->buf = b;
}
void nmea_link_in (nmea_t *nmea, nmea_in_ft in) {
   nmea->io.in = in;
}
void nmea_link_out (nmea_t *nmea, nmea_out_ft out) {
   nmea->io.out = out;
}

/*
 * Set functions
 */
void nmea_set_buffer_size (nmea_t *nmea, int s) {
   nmea->buf_size = s;
}

/*
 * User Functions
 */

/*!
 * \brief
 *    De-Initializes nmea parser used by the driver.
 *
 * \param  nmea   Pointer indicate the nmea data stuct to use
 */
void nmea_deinit (nmea_t *nmea)
{
   memset ((void*)nmea, 0, sizeof (nmea_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initializes nmea.
 *
 * \param  nmea   Pointer indicate the nmea data stuct to use
 * \return        The status of the operation
 */
drv_status_en nmea_init (nmea_t *nmea)
{
   #define _bad_link(_link)   (!nmea->io._link) ? 1:0

   if (_bad_link (in))     return nmea->status = DRV_ERROR;
   if (_bad_link (out))    return nmea->status = DRV_ERROR;

   if (nmea->status == DRV_BUSY || nmea->status == DRV_NODEV)
      return nmea->status = DRV_ERROR;

   return nmea->status = DRV_READY;
   #undef _bad_link
}

drv_status_en nmea_read_gga (nmea_t *nmea, nmea_gga_t *gga)
{
   NMEA_sentence_en s;
   char token[NMEA_TOKEN_SIZE];
   int cur=0;

   // Read sentences until we find GGA
   do {
      _get_sen (nmea);
      if ( !_checksum_chk ((char *)nmea->buf) )
         return DRV_ERROR;
      _read_sen_type ((char *)nmea->buf, &s);
   } while (s != NMEA_GGA);

   // tokenise
   do {
      cur += _get_token (nmea->buf+cur, token);
   }while (*token);

   return DRV_READY;
}

drv_status_en nmea_read_gll (nmea_t *nmea, nmea_gll_t *gll);
drv_status_en nmea_read_gsa (nmea_t *nmea, nmea_gsa_t *gsa);
drv_status_en nmea_read_gsv (nmea_t *nmea, nmea_gsv_t *gsv);
drv_status_en nmea_read_rmc (nmea_t *nmea, nmea_rmc_t *rmc);
drv_status_en nmea_read_vtg (nmea_t *nmea, nmea_vtg_t *vtg);
drv_status_en nmea_read_zda (nmea_t *nmea, nmea_zda_t *zda);
