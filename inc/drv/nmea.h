/*
 * \file nmea.h
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
#ifndef __nmea_h__
#define __nmea_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <tbx_ioctl.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/*
 * ============ User options ===========
 */
#define NMEA_WAIT_MAX_TRIES         (20)

/*
 * ============ parser's data types =============
 */
#define NMEA_IS_DELIMITER(_c)    (_c == ',' || _c == '*' || _c == '\r')
#define NMEA_TOKEN_SIZE          (12)


/*!
 * Parser's Recognised types
 */
typedef enum {
   _null=0,
   _fix_t,     /*!< Fix position type */
   _valid_t,   /*!< Validy type */
   _sats,      /*!< satellites in view */
   _utc,       /*!< UTC time */
   _date,      /*!< Date in ddmmyy format */
   _day,       /*!< Day */
   _month,     /*!< Month */
   _year,      /*!< Year */
   _zone_h,    /*!< Local hour offset from UTC*/
   _zone_m,    /*!< Local minute offset from UTC */
   _lat,       /*!< Latitude in iso llll.ll = 100 x latitude */
   _lat_s,     /*!< Latitude sign N or S */
   _long,      /*!< Longitude in iso yyyyy.yy = 100 x longitude */
   _long_s,    /*!< Longitude sign E or W */
   _elev,      /*!< Elevation */
   _speed_knt, /*!< speed over ground in knots */
   _speed_kmh, /*!< speed over ground in km/h */
   _sp_unts,   /*!< speed units knots of km/h */
   _course_t,  /*<! True Course */
   _course_m,  /*!< Magnetic Course */
   _crs_type,
   _mag_var,   /*<! magnetic variation in degrees */
   _mag_var_s, /*<! magnetic variation sign E or W */
   _msgid,     /*!< '$' character */
   _disc,      /*!< discard field */
   _crc,       /*!< Checksum of message */

}parse_obj_en;

typedef struct {
   parse_obj_en   obj;
   char *         format;
}parse_pair_t;


/*
 * ============ Data types ============
 */
typedef float  latitude_t;
typedef float  longitude_t;

typedef struct {
   int   hour;
   int   min;
   float sec;
}utc_time_t;

typedef struct {
   int day;
   int month;
   int year;
}utc_date_t;

typedef enum {
   NMEA_NULL=0,
   NMEA_GGA,   /*!< Global Positioning System Fix Data */
   NMEA_GLL,   /*!< Geographic position, latitude / longitude */
   NMEA_GSA,   /*!< GPS DOP and active satellites  */
   NMEA_GSV,   /*!< GPS Satellites in view */
   NMEA_RMC,   /*!< Recommended minimum specific GPS/Transit data */
   NMEA_VTG,   /*!< Track made good and ground speed */
   NMEA_ZDA    /*!< Date & Time */
}nmea_msgid_en;

typedef struct {
   nmea_msgid_en  id_type;
   char           *id_str;
}nmea_msgig_t;

typedef enum { NMEA_S=-1, NMEA_N=1 }nmea_lat_sign_en;
typedef enum { NMEA_W=-1, NMEA_E=1 }nmea_long_sign_en;
typedef enum { NMEA_KNOTS=0, NMEA_KMH }nmea_speed_units_en;
typedef enum { NMEA_COURSE_TRUE=0, NMEA_COURSE_MAG }nmea_course_en;
typedef enum { NMEA_NOT_FIX=0, NMEA_FIX, NMEA_DFIX }nmea_fix_en;
typedef enum { NMEA_VALID=0, NMEA_NOT_VALID }nmea_valid_en;

typedef struct {
   nmea_fix_en    fix;
   nmea_valid_en  valid;
   int            sats;

   utc_time_t     time;
   utc_date_t     date;
   int            day;
   int            month;
   int            year;
   int            zone_h;
   int            zone_m;

   latitude_t     latitude;
   latitude_t     longitude;
   float          elevation;

   float          course_t;
   float          course_m;
   float          speed_knt;
   float          speed_kmh;
   float          mag_var;
}nmea_common_t;



typedef struct {
   nmea_fix_en    fix;
   utc_time_t     time;
   latitude_t     latitude;
   latitude_t     longitude;
   float          elevation;
   int            sats;
}nmea_gga_t;

typedef struct {
   nmea_valid_en  valid;
   latitude_t     latitude;
   longitude_t    longitude;
   utc_time_t     time;
}nmea_gll_t;

typedef struct {
   int            crap;
}nmea_gsa_t;

typedef struct {
   int            sats;
}nmea_gsv_t;

typedef struct {
   nmea_valid_en  valid;
   utc_time_t     time;
   utc_date_t     date;
   latitude_t     latitude;
   longitude_t    longitude;
   float          speed_knt;
   float          course_t;
   float          mag_var;
}nmea_rmc_t;

typedef struct {
   float          course_t;
   float          course_m;
   float          speed_knt;
   float          speed_kmh;
}nmea_vtg_t;

typedef struct {
   utc_time_t  time;
   int         day;
   int         month;
   int         year;
   int         zone_h;
   int         zone_m;
}nmea_zda_t;

typedef byte_t (*nmea_in_ft) (void);
typedef int    (*nmea_out_ft) (byte_t);

typedef struct {
   nmea_in_ft      in;
   nmea_out_ft     out;
}nmea_io_t;

typedef struct {
   byte_t         *buf;
   int            buf_size;
   nmea_io_t      io;
   drv_status_en  status;
}nmea_t;



/*
 * ============ Public GPS API ============
 */

/*
 * Link and Glue functions
 */
void nmea_link_buffer (nmea_t *nmea, byte_t *b);
void nmea_link_in (nmea_t *nmea, nmea_in_ft in);
void nmea_link_out (nmea_t *nmea, nmea_out_ft out);

/*
 * Set functions
 */
void nmea_set_buffer_size (nmea_t *nmea, int s);

/*
 * User Functions
 */
void nmea_deinit (nmea_t *nmea);
drv_status_en nmea_init (nmea_t *nmea);

drv_status_en nmea_read_gga (nmea_t *nmea, nmea_gga_t *gga, int tries);
drv_status_en nmea_read_gll (nmea_t *nmea, nmea_gll_t *gll, int tries);
drv_status_en nmea_read_gsa (nmea_t *nmea, nmea_gsa_t *gsa, int tries);
drv_status_en nmea_read_gsv (nmea_t *nmea, nmea_gsv_t *gsv, int tries);
drv_status_en nmea_read_rmc (nmea_t *nmea, nmea_rmc_t *rmc, int tries);
drv_status_en nmea_read_vtg (nmea_t *nmea, nmea_vtg_t *vtg, int tries);
drv_status_en nmea_read_zda (nmea_t *nmea, nmea_zda_t *zda, int tries);

drv_status_en nmea_write( nmea_t *nmea, char *msg);




#ifdef __cplusplus
}
#endif

#endif   // #ifndef __nmea_h__
