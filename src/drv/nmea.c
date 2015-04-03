/*
 * \file nmea.c
 * \ingroup Drivers
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

/*!
 * \name Static data types
 */
//!@{
/*
 * NMEA Sentences
 *
 * $GPGGA - Global Positioning System Fix Data
 * $GPGLL - Geographic position, latitude / longitude
 * $GPRMC - Recommended minimum specific GPS/Transit data
 * $GPVTG - Track made good and ground speed
 * $GPZDA - Date & Time
 */
static const nmea_msgig_t  nmea_msgid[] = {
   {NMEA_GGA, "GGA"},
   {NMEA_GLL, "GLL"},
   {NMEA_GSA, "GSA"},
   {NMEA_GSV, "GSV"},
   {NMEA_RMC, "RMC"},
   {NMEA_VTG, "VTG"},
   {NMEA_ZDA, "ZDA"},
   {NMEA_NULL,0}
};
/*!
 * GGA message grammar
 */
static const parse_obj_en _GGA[] = {
    _msgid, _utc, _lat, _lat_s, _long, _long_s, _fix_t, _sats, _disc, _elev, _null
};
/*!
 * GLL message grammar
 */
static const parse_obj_en _GLL[] = {
   _msgid, _lat, _lat_s, _long, _long_s, _utc, _valid_t, _disc, _null
};
/*!
 * GSA message grammar
 */
static const parse_obj_en _GSA[] = {
   _msgid, _null
};
/*!
 * GSV message grammar
 */
static const parse_obj_en _GSV[] = {
   _msgid, _disc, _disc, _sats, _null
};
/*!
 * RMC message grammar
 */
static const parse_obj_en _RMC[] = {
   _msgid, _utc, _valid_t, _lat, _lat_s, _long, _long_s, _speed_knt, _course_t, _date, _mag_var, _mag_var_s, _null
};
/*!
 * VTG message grammar
 */
static const parse_obj_en _VTG[] = {
   _msgid, _course_t, _disc, _course_m, _disc, _speed_knt, _disc, _speed_kmh, _null
};
/*!
 * ZDA message grammar
 */
static const parse_obj_en _ZDA[] = {
   _msgid, _utc, _day, _month, _year, _zone_h, _zone_m, _null
};
//!@}


/*
 * ============ Static API ============
 */

//! \name Tools
//!@{
static int _checksum (char* str);
static float _nmea2dec (float c);
static float _dec2nmea (float c);
static int _match (char *sen, char *word, int n);
static nmea_msgid_en _msgid_type (char *str);
static char * _msgid_str (nmea_msgid_en id);
//!@}

//! \name Extract tools
//!@{
static int _read_sen_type (char *str, nmea_msgid_en *id);
static int _read_utc (char *str, utc_time_t *t);
static int _read_day (char *str, int *day);
static int _read_month (char *str, int *month);
static int _read_year (char *str, int *year);
static int _read_zone_h (char *str, int *zone_h);
static int _read_zone_m (char *str, int *zone_m);
static int _read_date (char *str, utc_date_t *date);
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
static int _read_sats (char *str, int *sats);
static int _read_fix (char *str, nmea_fix_en *fix);
static int _read_valid (char *str, nmea_valid_en *valid);
//!@}

//! \name Middle level
//!@{
static int _get_sen (nmea_t *nmea);
static int _get_token (char *str, char *token);
static int _checksum_chk (char *str);
static int _read_until (nmea_t *nmea, nmea_msgid_en id, int tries);
static int _tokenise (nmea_t *nmea, const parse_obj_en *format, nmea_common_t *obj);
//!@}



/*
 * ============== Tools ==============
 */
//! \name Tools
//!@{

/*!
 * \brief
 *    Return the checksum of the string
 */
static int _checksum (char* str) {
   int c = 0;

   while (*str)
      c ^= *str++;
   return c;
}

/*!
 * \brief
 *    Converts decimal angle to NMEA's format angle
 */
static float _nmea2dec (float c)
{
   int d;

   d = (int)c/100;
   c -= d*100;
   return d + (c/60);
}

/*!
 * \brief
 *    Converts NMEA's format angle to decimal angle
 */
static float _dec2nmea (float c)
{
   int d = (int)c;
   return d*100 + ((c-d) * 60);
}

/*!
 * \brief
 *    Search if there is a string pointed by \a word
 *    into string pointed by \a sen
 * \param   sen   Pointer to string to search into
 * \param   word  Pointer to string to search
 * \param   n     Size of word string
 * \return        The result
 *    \arg  0     Not found
 *    \arg  1     Found
 */
static int _match (char *sen, char *word, int n)
{
   while (*sen) {
      if ( !strncmp (sen, word, n) )
         return 1;
      ++sen;
   }
   return 0;
}

/*!
 * \brief
 *    Converts Message id string into message id type
 */
static nmea_msgid_en _msgid_type (char *str)
{
   for (int i=0 ; nmea_msgid[i].id_type ; ++i)
      if ( !strcmp (str, nmea_msgid[i].id_str) )
         return nmea_msgid[i].id_type;
   return NMEA_NULL;
}

/*!
 * \brief
 *    Converts Message id type into message id string
 */
static char * _msgid_str (nmea_msgid_en id)
{
   for (int i=0 ; nmea_msgid[i].id_type ; ++i)
      if ( id == nmea_msgid[i].id_type )
         return nmea_msgid[i].id_str;
   return NULL;
}
//!@}

/*
 * ============== Extract tools ==============
 */
//! \name Extract tools
//!@{

/*!
 * \brief
 *    Read sentence type
 * Try to find id string \a str into sentence
 * \param   str   Pointer to token
 * \param   id    Pointer to id to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_sen_type (char *str, nmea_msgid_en *id) {
   for (int i=0 ; nmea_msgid[i].id_type; ++i) {
      if ( _match (str, nmea_msgid[i].id_str, 3) ) {
         *id = nmea_msgid[i].id_type;
         return 1;
      }
   }
   *id = NMEA_NULL;
   return 0;
}

/*!
 * \brief
 *    Read utc time
 * Try to extract utc time from token \a str
 * \param   str   Pointer to token
 * \param   t     Pointer to utc_time_t type to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_utc (char *str, utc_time_t *t) {
   int utc_d, utc_f;

   if (sscanf (str, "%d.%d", &utc_d, &utc_f)  != 2)
      return 0;

   t->hour = utc_d / 10000;
   utc_d -= t->hour * 10000;
   t->min = utc_d / 100;
   t->sec = utc_d - t->min*100;
   t->sec += (float)utc_f / 1000;
   return 1;
}

/*!
 * \brief
 *    Read utc date
 * Try to extract utc date from token \a str
 * \param   str   Pointer to token
 * \param   date  Pointer to utc_date_t type to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_date (char *str, utc_date_t *date) {
   int d;
   if (sscanf (str, "%d", &d) != 1)
      return 0;
   date->day = d/10000;
   d -= date->day * 10000;
   date->month = d / 100;
   date->year = d - date->month*100 + 2000;
   return 1;
}

/*!
 * \brief
 *    Read day of month
 * Try to extract day from token \a str
 * \param   str   Pointer to token
 * \param   day   Pointer to day variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_day (char *str, int *day) {
   return (sscanf (str, "%d", day) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read month
 * Try to extract month from token \a str
 * \param   str   Pointer to token
 * \param   month Pointer to month variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_month (char *str, int *month) {
   return (sscanf (str, "%d", month) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read year
 * Try to extract year from token \a str
 * \param   str   Pointer to token
 * \param   year  Pointer to year variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_year (char *str, int *year) {
   return (sscanf (str, "%d", year) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read local hour time zone offset
 * Try to extract hour time zone from token \a str
 * \param   str   Pointer to token
 * \param   zone_h Pointer to time zone variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_zone_h (char *str, int *zone_h) {
   return (sscanf (str, "%d", zone_h) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read local minute time zone offset
 * Try to extract minute time zone from token \a str
 * \param   str   Pointer to token
 * \param   zone_m Pointer to time zone variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_zone_m (char *str, int *zone_m) {
   return (sscanf (str, "%d", zone_m) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read latitude
 * Try to extract latitude from token \a str and convert it to
 * decimal format
 * \param   str   Pointer to token
 * \param   lat   Pointer to latitude variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_lat (char *str, float *lat) {
   float l;
   if (sscanf (str, "%f", &l) != 1)
      return 0;
   *lat = _nmea2dec (l);
   return 1;
}

/*!
 * \brief
 *    Read latitude's orientation (South or north)
 * Try to extract latitude sign from token \a str
 * \param   str   Pointer to token
 * \param   lat_s Pointer to latitude sign variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_lat_s (char *str, nmea_lat_sign_en *lat_s) {
   char sign;
   *lat_s = NMEA_N;
   if (sscanf (str, "%c", &sign) != 1)
      return 0;
   if (sign == 'S' || sign == 's')
      *lat_s = NMEA_S;
   return 1;
}

/*!
 * \brief
 *    Read longitude
 * Try to extract longitude from token \a str and convert it to
 * decimal format
 * \param   str   Pointer to token
 * \param   lon   Pointer to longitude variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_long (char *str, float *lon) {
   float l;
   if (sscanf (str, "%f", &l) != 1)
      return 0;
   *lon = _nmea2dec (l);
   return 1;
}

/*!
 * \brief
 *    Read longitude orientation (South or north)
 * Try to extract longitude sign from token \a str
 * \param   str   Pointer to token
 * \param   lon_s Pointer to longirude sign variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_long_s (char *str, nmea_long_sign_en *lon_s) {
   char sign;
   *lon_s = NMEA_E;
   if (sscanf (str, "%c", &sign) != 1)
      return 0;
   if (sign == 'W' || sign == 'w')
      *lon_s = NMEA_W;
   return 1;
}

/*!
 * \brief
 *    Read elevation
 * Try to extract elevation from token \a str
 * \param   str   Pointer to token
 * \param   elev  Pointer to elevation variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_elev (char *str, float *elev) {
   return (sscanf (str, "%f", elev) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read speed
 * Try to extract speed from token \a str
 * \param   str   Pointer to token
 * \param   speed Pointer to speed variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_speed (char *str, float *speed) {
   return (sscanf (str, "%f", speed) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read speed units
 * Try to extract speed units from token \a str
 * \param   str   Pointer to token
 * \param   sp_unts Pointer to speed units variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_sp_unts (char *str, nmea_speed_units_en *sp_unts) {
   char unt;
   *sp_unts = NMEA_KNOTS;
   if (sscanf (str, "%c", &unt) != 1)
      return 0;
   if (unt == 'K' || unt == 'k')
      *sp_unts = NMEA_KMH;
   return 1;
}

/*!
 * \brief
 *    Read course
 * Try to extract course from token \a str
 * \param   str   Pointer to token
 * \param   course Pointer to course variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_course (char *str, float *course) {
   return (sscanf (str, "%f", course) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read course type
 * Try to extract course type from token \a str
 * \param   str   Pointer to token
 * \param   course_type  Pointer to course type variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_course_type (char *str, nmea_course_en *course_type) {
   char tpe;
   *course_type = NMEA_COURSE_MAG;
   if (sscanf (str, "%c", &tpe) != 1)
      return 0;
   if (tpe == 'T' || tpe == 't')
      *course_type = NMEA_COURSE_TRUE;
   return 1;
}

/*!
 * \brief
 *    Read Magnetic variation
 * Try to extract magnetic variation from token \a str
 * \param   str   Pointer to token
 * \param   mag_var Pointer to magnetic variation variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_mag_var (char *str, float *mag_var) {
   return (sscanf (str, "%f", mag_var) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read Magnetic variation orientation
 * Try to extract magnetic variation sign from token \a str
 * \param   str   Pointer to token
 * \param   mag_var_s Pointer to magnetic variation variable sign to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_mag_var_s (char *str, nmea_long_sign_en *mag_var_s) {
   char tpe;
   *mag_var_s = NMEA_E;
   if (sscanf (str, "%c", &tpe) != 1)
      return 0;
   if (tpe == 'W' || tpe == 'w')
      *mag_var_s = NMEA_W;
   return 1;
}

/*!
 * \brief
 *    Read satellites
 * Try to extract satellites from token \a str
 * \param   str   Pointer to token
 * \param   sats  Pointer to satellites variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_sats (char *str, int *sats) {
   return (sscanf (str, "%d", sats) == 1) ? 1:0;
}

/*!
 * \brief
 *    Read fix
 * Try to extract fix from token \a str
 * \param   str   Pointer to token
 * \param   fix   Pointer to fix variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_fix (char *str, nmea_fix_en *fix) {
   int d;
   if (sscanf (str, "%d", &d) != 1)
      return 0;
   switch (d) {
      default:
      case 0: *fix = NMEA_NOT_FIX;  return 1;
      case 1: *fix = NMEA_FIX;      return 1;
      case 2: *fix = NMEA_DFIX;     return 1;
   }
}

/*!
 * \brief
 *    Read valid
 * Try to extract valid from token \a str
 * \param   str   Pointer to token
 * \param   valid Pointer to valid variable to return
 * \return        The status of the operation
 *    \arg  0     Not recognised id = NULL
 *    \arg  1     Recognised
 */
static int _read_valid (char *str, nmea_valid_en *valid) {
   char v;
   *valid = NMEA_VALID;
   if (sscanf (str, "%c", &v) != 1)
      return 0;
   if (v == 'V' || v == 'v')
      *valid = NMEA_NOT_VALID;
   return 1;
}
//!@}


/*
 * ============== Middle level ==============
 */
//! \name Middle level
//!@{

/*!
 * \brief
 *    Read a sentence from input stream into nmea's buffer
 * \param   nmea  Pointer to linked data to use
 * \return        The size of string in buffer
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

/*!
 * \brief
 *    Get the next token from string
 * \param   str   Pointer to string to search into
 * \param   token Pointer to token to return
 * \return        The size of token
 */
static int _get_token (char *str, char *token)
{
   char *s = str;
   while (1) {
      if (NMEA_IS_DELIMITER (*s)) {
         ++s;
         *token = 0;
         return s - str;   // eat next delimiter
      }
      else
         *token++ = *s++;
   }
   return 0;
}

/*!
 * \brief
 *    Check the checksum of a sentence
 * \param   str   Pointer to hole sentence (from '$' to "\r\n")
 * \return        The result
 *    \arg  0     Checksum don't match
 *    \arg  1     Checksum match
 */
static int _checksum_chk (char *str)
{
   char *s, *d;   // Star and dollar pointers
   int sc, cc;    // string and calculated checksum

   d = strchr (str, '$');  // find where is '$'
   s = strchr (d, '*');    // find where is '*'

   sscanf (s, "*%2X\r\n", &sc);
   for (cc=0,++d ; d<s ; ++d)
      cc ^= *d;

   return (cc == sc) ? 1:0;
}

/*!
 * \brief
 *    Read from input stream until a checksum valid sentence
 *    with id \a id appears.
 * \param   nmea  Pointer to linked nmea data to use
 * \param   id    The desired message id sentence
 * \param   tries The maximum number of sentence to read from input
 *                before give up
 * \return        The status of the operation
 *    \arg  0     Fail
 *    \arg  1     Success
 */
static int _read_until (nmea_t *nmea, nmea_msgid_en id, int tries)
{
   nmea_msgid_en s;
   uint8_t to = (tries==0) ? 0:1;  // time out check

   // Read sentences until we find id
   do {
      _get_sen (nmea);
      if ( !_checksum_chk ((char *)nmea->buf) )
         return 0;
      _read_sen_type ((char *)nmea->buf, &s);
   } while (s != id && (!to || --tries));
   return (to && !tries) ? 0 : 1;
}

/*!
 * \brief
 *    Extract nmea data from sentence into buffer using grammar in \a format
 * \param   nmea  Pointer to linked data to use
 * \param   format Pointer to grammar array to use
 * \param   obj   Pointer to data object to return
 * \return        Number or read tokens
 */
static int _tokenise (nmea_t *nmea, const parse_obj_en *format, nmea_common_t *obj)
{
   char token[NMEA_TOKEN_SIZE];
   int cur, tk, sign=1;

   for (cur=0, tk=0 ; format[tk] != _null ; ++tk) {
      cur += _get_token ((char*)nmea->buf+cur, token);
      switch (format[tk]) {
         case _fix_t:      _read_fix (token, &obj->fix);          break;
         case _valid_t:    _read_valid (token, &obj->valid);      break;
         case _sats:       _read_sats (token, &obj->sats);        break;
         case _utc:        _read_utc (token, &obj->time);         break;
         case _date:       _read_date (token, &obj->date);        break;
         case _day:        _read_day (token, &obj->day);          break;
         case _month:      _read_month (token, &obj->month);      break;
         case _year:       _read_year (token, &obj->year);        break;
         case _zone_h:     _read_zone_h (token, &obj->zone_h);    break;
         case _zone_m:     _read_zone_m (token, &obj->zone_m);    break;
         case _lat:        _read_lat (token, &obj->latitude);     break;
         case _lat_s:      _read_lat_s (token, (nmea_lat_sign_en*)&sign);
                           obj->latitude *= sign;
                           break;
         case _long:       _read_long (token, &obj->longitude);   break;
         case _long_s:     _read_long_s (token, (nmea_long_sign_en*)&sign);
                           obj->longitude *= sign;
                           break;
         case _elev:       _read_elev (token, &obj->elevation);   break;
         case _speed_knt:  _read_speed (token, &obj->speed_knt);  break;
         case _speed_kmh:  _read_speed (token, &obj->speed_kmh);  break;
         case _sp_unts:    break;
         case _course_t:   _read_course (token, &obj->course_t);  break;
         case _course_m:   _read_course (token, &obj->course_m);  break;
         case _crs_type:   break;
         case _mag_var:    _read_mag_var (token, &obj->mag_var);  break;
         case _mag_var_s:  _read_mag_var_s (token, (nmea_long_sign_en*)&sign);
                           obj->mag_var *= sign;
                           break;

         case _msgid:      break;
         case _disc:       break;
         default:          break;
      }
   }
   return tk;
}

/*!
 * \brief
 *    Stream out a string
 * \param   nmea  Pointer to linked data to use
 * \param   str   Pointer to string to stream
 */
void _stream (nmea_t *nmea, char *str)
{
   while (*str)
      nmea->io.out (*str++);
}
//!@{


/*
 * =============== Public NMEA API =================
 */

/*
 * ========= Link and Glue functions ==============
 */
/*
 * \name Link and Glue functions
 */
//!@{
/*!
 * Link buffer to nmea data
 */
void nmea_link_buffer (nmea_t *nmea, byte_t *b) {
   nmea->buf = b;
}
/*!
 * Link input function to nmea data
 */
void nmea_link_in (nmea_t *nmea, nmea_in_ft in) {
   nmea->io.in = in;
}
/*!
 * Link output function to nmea data
 */
void nmea_link_out (nmea_t *nmea, nmea_out_ft out) {
   nmea->io.out = out;
}
//!@}

/*
 * ============== Set functions ================
 */
/*
 * \name Set functions
 */
//!@{
/*!
 * Set buffer size
 */
void nmea_set_buffer_size (nmea_t *nmea, int s) {
   nmea->buf_size = s;
}
//!@}

/*
 * ============= User Functions ==============
 */

/*
 * \name User Functions
 */
//!@{
/*!
 * \brief
 *    De-Initializes nmea parser used by the driver.
 *
 * \param  nmea   Pointer to linked nmea data stuct to use
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
 * \param  nmea   Pointer to linked nmea data stuct to use
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

/*!
 * \brief
 *    Read and extract GGA data from input stream
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   gga   Pointer to gga data for the results
 *                The gga variable is written only when we have position fix
 * \param   tries The maximum number of sentences to read before give up, 0 for unlimited
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GGA sentence in stream
 *    \arg  DRV_READY   Success
 */
drv_status_en nmea_read_gga (nmea_t *nmea, nmea_gga_t *gga, int tries)
{
   nmea_common_t obj;

   // Read sentences until we find GGA
   if (_read_until (nmea, NMEA_GGA, tries) == 0)
      return DRV_ERROR;

   // tokenise
   _tokenise (nmea, _GGA, &obj);

   if (obj.fix != NMEA_NOT_FIX) {
      gga->fix = obj.fix;
      gga->sats = obj.sats;
      gga->time = obj.time;
      gga->latitude = obj.latitude;
      gga->longitude = obj.longitude;
      gga->elevation = obj.elevation;
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Read and extract GLL data from input stream
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   gll   Pointer to gll data for the results
 *                The gll variable is written only when we have position fix
 * \param   tries The maximum number of sentences to read before give up, 0 for unlimited
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GLL sentence in stream
 *    \arg  DRV_READY   Success
 */
drv_status_en nmea_read_gll (nmea_t *nmea, nmea_gll_t *gll, int tries)
{
   nmea_common_t obj;

   // Read sentences until we find GLL
   if (_read_until (nmea, NMEA_GLL, tries) == 0)
      return DRV_ERROR;

   // tokenise
   _tokenise (nmea, _GLL, &obj);

   if (obj.fix != NMEA_NOT_FIX) {
      gll->valid = obj.valid;
      gll->time = obj.time;
      gll->latitude = obj.latitude;
      gll->longitude = obj.longitude;
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Read and extract GSA data from input stream
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   gsa   Pointer to gsa data for the results
 * \param   tries The maximum number of sentences to read before give up, 0 for unlimited
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GSA sentence in stream
 *    \arg  DRV_READY   Success
 * \note    Not implemented yet
 */
drv_status_en nmea_read_gsa (nmea_t *nmea, nmea_gsa_t *gsa, int tries)
{
   nmea_common_t obj;

   // Read sentences until we find GSA
   if (_read_until (nmea, NMEA_GSA, tries) == 0)
      return DRV_ERROR;

   // tokenise
   _tokenise (nmea, _GSA, &obj);

   gsa->crap = 0;
   return DRV_READY;
}

/*!
 * \brief
 *    Read and extract GSV data from input stream
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   gsv   Pointer to gsv data for the results
 * \param   tries The maximum number of sentences to read before give up, 0 for unlimited
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GSV sentence in stream
 *    \arg  DRV_READY   Success
 * \note    Not implemented yet
 */
drv_status_en nmea_read_gsv (nmea_t *nmea, nmea_gsv_t *gsv, int tries)
{
   nmea_common_t obj;

   // Read sentences until we find GSV
   if (_read_until (nmea, NMEA_GSV, tries) == 0)
      return DRV_ERROR;

   // tokenise
   _tokenise (nmea, _GSV, &obj);

   gsv->sats = obj.sats;
   return DRV_READY;
}

/*!
 * \brief
 *    Read and extract RMC data from input stream
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   rmc   Pointer to rmc data for the results
 *                The rmc variable is written only when we have position fix
 * \param   tries The maximum number of sentences to read before give up, 0 for unlimited
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid RMC sentence in stream
 *    \arg  DRV_READY   Success
 */
drv_status_en nmea_read_rmc (nmea_t *nmea, nmea_rmc_t *rmc, int tries)
{
   nmea_common_t obj;

   // Read sentences until we find RMC
   if (_read_until (nmea, NMEA_RMC, tries) == 0)
      return DRV_ERROR;

   // tokenise
   _tokenise (nmea, _RMC, &obj);

   if (obj.fix != NMEA_NOT_FIX) {
      rmc->valid = obj.valid;
      rmc->date = obj.date;
      rmc->time = obj.time;
      rmc->latitude = obj.latitude;
      rmc->longitude = obj.longitude;
      rmc->speed_knt = obj.speed_knt;
      rmc->course_t = obj.course_t;
      rmc->mag_var = obj.mag_var;
   }
   return DRV_READY;
}

/*!
 * \brief
 *    Read and extract VTG data from input stream
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   vtg   Pointer to vtg data for the results
 * \param   tries The maximum number of sentences to read before give up, 0 for unlimited
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid VTG sentence in stream
 *    \arg  DRV_READY   Success
 */
drv_status_en nmea_read_vtg (nmea_t *nmea, nmea_vtg_t *vtg, int tries)
{
   nmea_common_t obj;

   // Read sentences until we find RMC
   if (_read_until (nmea, NMEA_VTG, tries) == 0)
      return DRV_ERROR;

   // tokenise
   _tokenise (nmea, _VTG, &obj);

   vtg->course_m = obj.course_m;
   vtg->course_t = obj.course_t;
   vtg->speed_knt = obj.speed_knt;
   vtg->speed_kmh = obj.speed_kmh;

   return DRV_READY;
}

/*!
 * \brief
 *    Read and extract ZDA data from input stream
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   zda   Pointer to zda data for the results
 * \param   tries The maximum number of sentences to read before give up, 0 for unlimited
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid ZDA sentence in stream
 *    \arg  DRV_READY   Success
 */
drv_status_en nmea_read_zda (nmea_t *nmea, nmea_zda_t *zda, int tries)
{
   nmea_common_t obj;

   // Read sentences until we find RMC
   if (_read_until (nmea, NMEA_ZDA, tries) == 0)
      return DRV_ERROR;

   // tokenise
   _tokenise (nmea, _ZDA, &obj);

   zda->time = obj.time;
   zda->day = obj.day;
   zda->month = obj.month;
   zda->year = obj.year;
   zda->zone_h = obj.zone_h;
   zda->zone_m = obj.zone_m;
   return DRV_READY;
}

/*!
 * \brief
 *    Write a message to output stream. The message is wrapped
 *    with '$' and "*<checksum>\r\n"
 * \param   nmea  Pointer to linked nmea data struct to use
 * \param   msg   Pointer to message to send
 * \return        The status of the operation
 *    \arg  DRV_READY  Success
 */
drv_status_en nmea_write (nmea_t *nmea, char *msg)
{
   char cs[6];

   // Create checksum
   sprintf (cs, "*%02X\r\n", _checksum (msg));

   // send message
   nmea->io.out ('$');
   _stream (nmea, msg);
   _stream (nmea, cs);

   return DRV_READY;
}
//!@}
