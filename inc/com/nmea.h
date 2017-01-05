/*
 * \file nmea.h
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
#ifndef __nmea_h__
#define __nmea_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <toolbox_defs.h>
#include <tbx_types.h>
#include <tbx_ioctl.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/*
 * ============ User options ===========
 */
//! Parser maximum number of extract tries before return to caller
#define NMEA_WAIT_MAX_TRIES         (20)

/*
 * ============ parser's data types =============
 */
#define NMEA_IS_DELIMITER(_c)    (_c == ',' || _c == '*' || _c == '\r')
#define NMEA_TOKEN_SIZE          (12)


/*
 * ============ Data types ============
 */

/*!
 * \name NMEA helper enumerators
 */
//!@{
/*!
 * Parser's Recognised tokens
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
   _crs_type,  /*!< Course type */
   _mag_var,   /*<! magnetic variation in degrees */
   _mag_var_s, /*<! magnetic variation sign E or W */
   _msgid,     /*!< '$' character */
   _disc,      /*!< discard field */
   _crc,       /*!< Checksum of message */
}parse_obj_en;
typedef enum { NMEA_S=-1, NMEA_N=1 }nmea_lat_sign_en;                //!< South or North
typedef enum { NMEA_W=-1, NMEA_E=1 }nmea_long_sign_en;               //!< West or East
typedef enum { NMEA_KNOTS=0, NMEA_KMH }nmea_speed_units_en;          //!< Knots or Km/h
typedef enum { NMEA_COURSE_TRUE=0, NMEA_COURSE_MAG }nmea_course_en;  //!< True course or magnetic
typedef enum { NMEA_NOT_FIX=0, NMEA_FIX, NMEA_DFIX }nmea_fix_en;     //!< Position fix or not
typedef enum { NMEA_VALID=0, NMEA_NOT_VALID }nmea_valid_en;          //!< Message valid or not
//!@}

typedef float  latitude_t;       //!< Latitude type
typedef float  longitude_t;      //!< Longitude type

/*!
 * UTC time type
 */
typedef struct {
   int   hour;    //!< hours
   int   min;     //!< minutes
   float sec;     //!< seconds
}utc_time_t;

/*!
 * UTC date type
 */
typedef struct {
   int day;       //!< day of month
   int month;     //!< month
   int year;      //!< year, ex: 2011
}utc_date_t;

/*!
 * Enumerator for the implemented sentences
 */
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

/*!
 * Message ID pairs
 */
typedef struct {
   nmea_msgid_en  id_type;    //!< Message id type
   char           *id_str;    //!< Identification string
}nmea_msgig_t;


/*!
 * NMEA's common object type. This type is used by
 * NMEA parser for data extraction.
 */
typedef struct {
   nmea_fix_en    fix;        //!< Position fix
   nmea_valid_en  valid;      //!< Message valid
   int            sats;       //!< Satellites in view

   utc_time_t     time;       //!< UTC time
   utc_date_t     date;       //!< UTC date
   int            day;        //!< Day of month
   int            month;      //!< Month
   int            year;       //!< Year
   int            zone_h;     //!< Local hour time zone (hour offset)
   int            zone_m;     //!< Local minutes time zone (minute offset)

   latitude_t     latitude;   //!< Latitude
   latitude_t     longitude;  //!< Longitude
   float          elevation;  //!< Elevation

   float          course_t;   //!< True course
   float          course_m;   //!< Magnetic course
   float          speed_knt;  //!< Speed in knots
   float          speed_kmh;  //!< speed in Km/h
   float          mag_var;    //!< Magnetic variation
}nmea_common_t;


/*!
 * GGA message type
 */
typedef struct {
   nmea_fix_en    fix;        //!< Position fix
   utc_time_t     time;       //!< UTC time
   latitude_t     latitude;   //!< latitude
   latitude_t     longitude;  //!< longitude
   float          elevation;  //!< Elevation
   int            sats;       //!< Satellites in view
}nmea_gga_t;

/*!
 * GLL message type
 */
typedef struct {
   nmea_valid_en  valid;      //!< Message valid
   latitude_t     latitude;   //!< Latitude
   longitude_t    longitude;  //!< Longitude
   utc_time_t     time;       //!< UTC time
}nmea_gll_t;

/*!
 * GSA message type
 * \note Not supported yet
 * \fixme
 *    Add extract tool and the appropriate types in
 *    nmea_common_t struct
 */
typedef struct {
   int            crap;
}nmea_gsa_t;

/*!
 * GSA message type
 * \note Not fully supported yet
 * \fixme
 *    Add extract tool and the appropriate types in
 *    nmea_common_t struct
 */
typedef struct {
   int            sats;    //!< Satellites in view
}nmea_gsv_t;

/*!
 * RMC message type
 */
typedef struct {
   nmea_valid_en  valid;      //!< Message valid
   utc_time_t     time;       //!< UTC time
   utc_date_t     date;       //!< UTC date
   latitude_t     latitude;   //!< Latitude
   longitude_t    longitude;  //!< Longitude
   float          speed_knt;  //!< Speed in Knots
   float          course_t;   //!< True course
   float          mag_var;    //!< Magnetic variation
}nmea_rmc_t;

/*!
 * VTG message type
 */
typedef struct {
   float          course_t;   //!< True course
   float          course_m;   //!< Magnetic course
   float          speed_knt;  //!< Speed in Knots
   float          speed_kmh;  //!< Speed in Km/h
}nmea_vtg_t;

/*!
 * ZDA message type
 */
typedef struct {
   utc_time_t  time;          //!< UTC time
   int         day;           //!< Day of month
   int         month;         //!< Month
   int         year;          //!< Year, ex: 2011
   int         zone_h;        //!< Local hour time zone (hour offset)
   int         zone_m;        //!< Local minutes time zone (minute offset)
}nmea_zda_t;

/*!
 * \name interface types
 */
//!@{
typedef byte_t (*nmea_in_ft) (void);      //!< Input function pointer
typedef int    (*nmea_out_ft) (byte_t);   //!< Output function pointer

/*!
 * In/Out type
 */
typedef struct {
   nmea_in_ft      in;     //!< Module's input
   nmea_out_ft     out;    //!< Module's output
}nmea_io_t;
//!@}

/*!
 * NMEA public data type
 */
typedef struct {
   byte_t         *buf;       //!< Pointer to sentence buffer
   int            buf_size;   //!< Buffer size
   nmea_io_t      io;         //!< Module's input output
   drv_status_en  status;     //!< Driver's status
}nmea_t;



/*
 * ============ Public NMEA API ============
 */
void mnea_unused (void);

/*
 * \name Link and Glue functions
 */
//!@{
void nmea_link_buffer (nmea_t *nmea, byte_t *b);
void nmea_link_in (nmea_t *nmea, nmea_in_ft in);
void nmea_link_out (nmea_t *nmea, nmea_out_ft out);
//!@}

/*
 * \name Set functions
 */
//!@{
void nmea_set_buffer_size (nmea_t *nmea, int s);
//!@}

/*
 * \name User Functions
 */
//!@{
void nmea_deinit (nmea_t *nmea);
drv_status_en nmea_init (nmea_t *nmea);

drv_status_en nmea_read_gga (nmea_t *nmea, nmea_gga_t *gga);
drv_status_en nmea_read_gll (nmea_t *nmea, nmea_gll_t *gll);
drv_status_en nmea_read_gsa (nmea_t *nmea, nmea_gsa_t *gsa);
drv_status_en nmea_read_gsv (nmea_t *nmea, nmea_gsv_t *gsv);
drv_status_en nmea_read_rmc (nmea_t *nmea, nmea_rmc_t *rmc);
drv_status_en nmea_read_vtg (nmea_t *nmea, nmea_vtg_t *vtg);
drv_status_en nmea_read_zda (nmea_t *nmea, nmea_zda_t *zdas);

drv_status_en nmea_write( nmea_t *nmea, char *msg);
//!@}



#ifdef __cplusplus
}
#endif

#endif   // #ifndef __nmea_h__
