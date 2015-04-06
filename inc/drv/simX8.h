/*
 * \file simX8.h
 * \ingroup Drivers
 * \brief
 *    A sim28 sim68R sim68V driver using NMEA parser
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
#ifndef __simX8_h__
#define __simX8_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <tbx_types.h>
#include <tbx_ioctl.h>
#include <drv/nmea.h>
#include <string.h>
#include <stdarg.h>

/*
 * ======= General defines =========
 */

#define  PMTK_CMD_HOT_START            (0x100)
#define  PMTK_CMD_WARM_START           (0x101)
#define  PMTK_CMD_COLD_START           (0x102)
#define  PMTK_CMD_FULL_COLD_START      (0x103)
#define  PMTK_CMD_STANDBY_MODE         (0x104)
#define  PMTK_API_SET_NMEA_OUTPUT      (0x105)
#define  PMTK_DT_SBAS_ENABLED          (0x106)
#define  PMTK_SET_NMEA_BAUDRATE        (0x107)
#define  PMTK_NMEA_OUTPUT_GLL          (0x108)
#define  PMTK_DT_UTC                   (0x110)
#define  PMTK_DT_POS                   (0x111)

#define  NMEA_OUTPUT_INIT_MASK_STR     "PMTK314,"
#define  NMEA_OUTPUT_DEFAULT           "PMTK314,-1"

#define  SIMX8_NUMBER_OF_SENTENCES     (19)
/*
 * ======= Data types ============
 */
typedef enum {
   SIMX8_GLL = 0,
   SIMX8_RMC,
   SIMX8_VTG,
   SIMX8_GGA,
   SIMX8_GSA,
   SIMX8_GSV,
   SIMX8_ZDA = 17,
   SIMX8_RES = 18,
   SIMX8_DEF = 19
}nmea_output_en;

typedef struct {
   nmea_t         *nmea;
   drv_status_en  status;
}simX8_t;

/*
 * ============ Public NMEA API ============
 */

/*
 * \name Link and Glue functions
 */
//!@{
void simX8_link_nmea (simX8_t *sim, nmea_t *nmea);
//!@}

/*
 * \name Set functions
 */
//!@{
void simX8_set (simX8_t *sim, int disc);
//!@}

/*
 * \name User functions
 */
//!@{
void simX8_deinit (simX8_t *sim);
drv_status_en simX8_init (simX8_t *sim);

drv_status_en simX8_read_gga (simX8_t *sim, nmea_gga_t *gga, int tries);
drv_status_en simX8_read_gll (simX8_t *sim, nmea_gll_t *gll, int tries);
drv_status_en simX8_read_gsa (simX8_t *sim, nmea_gsa_t *gsa, int tries);
drv_status_en simX8_read_gsv (simX8_t *sim, nmea_gsv_t *gsv, int tries);
drv_status_en simX8_read_rmc (simX8_t *sim, nmea_rmc_t *rmc, int tries);
drv_status_en simX8_read_vtg (simX8_t *sim, nmea_vtg_t *vtg, int tries);
drv_status_en simX8_read_zda (simX8_t *sim, nmea_zda_t *zda, int tries);

drv_status_en simX8_ctl (simX8_t *sim, ioctl_cmd_t cmd, ...);

//!@}

#ifdef __cplusplus
}
#endif

#endif // #ifndef __simX8_h__
