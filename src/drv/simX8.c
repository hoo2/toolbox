/*
 * \file simX8.c
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
#include <drv/simX8.h>

static char mask_str[46] = "PMTK314,";

/*
 * Static API
 */
static _set_nmea_output (nmea_t *nmea, nmea_output_en cmd, uint_fast8_t on)
{
   static uint32_t mask = 0;
   int i;

   // Init mask with modules default output
   if (!mask)  mask = NMEA_OUTPUT_DEFAULT_MASK;

   if (on)  mask |= 0x1 << cmd;
   else     mask &= ~ (0x1 << cmd);

   for (i=0 ; i<SIMX8_NUMBER_OF_SENTENCES ; ++i) {
      sprintf (mask_str, "%s%d,", mask_str, ());
   }
}

/*
 * ============ Public NMEA API ============
 */

/*
 * \name Link and Glue functions
 */
//!@{
inline void simX8_link_nmea (simX8_t *sim, nmea_t *nmea) {
   sim->nmea = nmea;
}
//!@}

/*
 * \name Set functions
 */
//!@{
inline void simX8_set (simX8_t *sim, int disc) {}
//!@}


/*
 * \name User functions
 */
//!@{

void simX8_deinit (simX8_t *sim) {}
drv_status_en simX8_init (simX8_t *sim, int disc) { return DRV_READY; }

inline drv_status_en simX8_read_gga (simX8_t *sim, nmea_gga_t *gga, int tries) {
   return nmea_read_gga (sim->nmea, gga, tries);
}
inline drv_status_en simX8_read_gll (simX8_t *sim, nmea_gll_t *gll, int tries) {
   return nmea_read_gll (sim->nmea, gll, tries);
}
inline drv_status_en simX8_read_gsa (simX8_t *sim, nmea_gsa_t *gsa, int tries) {
   return nmea_read_gsa (sim->nmea, gsa, tries);
}
inline drv_status_en simX8_read_gsv (simX8_t *sim, nmea_gsv_t *gsv, int tries) {
   return nmea_read_gsv (sim->nmea, gsv, tries);
}
inline drv_status_en simX8_read_rmc (simX8_t *sim, nmea_rmc_t *rmc, int tries) {
   return nmea_read_rmc (sim->nmea, rmc, tries);
}
inline drv_status_en simX8_read_vtg (simX8_t *sim, nmea_vtg_t *vtg, int tries) {
   return nmea_read_vtg (sim->nmea, vtg, tries);
}
inline drv_status_en simX8_read_zda (simX8_t *sim, nmea_zda_t *zda, int tries) {
   return nmea_read_zda (sim->nmea, zda, tries);
}

/*!
 * \brief
 *    Send command to simX8
 * \param   sim   Pointer to linked simX8 data struct to use
 * \param   cmd   The command to simX8
 *    \arg CTRL_DEINIT
 *    \arg CTRL_INIT
 *    \arg PMTK_CMD_HOT_START
 *    \arg PMTK_CMD_WARM_START
 *    \arg PMTK_CMD_COLD_START
 *    \arg PMTK_CMD_FULL_COLD_START
 *    \arg PMTK_CMD_STANDBY_MODE
 *    \arg PMTK_API_SET_NMEA_OUTPUT
 *    \arg PMTK_SET_NMEA_BAUDRATE
 *    \arg PMTK_NMEA_OUTPUT_GLL

 *    \arg PMTK_DT_SBAS_ENABLED
 * \param   buf   pointer to buffer for ioctl
 */
drv_status_en simX8_ctrl (simX8_t *sim, ioctl_cmd_t cmd, ...)
{
   __VALIST ap;
   drv_status_en res;
   char str[20];

   va_start(ap, (ioctl_cmd_t)frm);
   switch (cmd) {
      case CTRL_DEINIT:
         break;
      case CTRL_INIT:
         break;

      case PMTK_CMD_HOT_START:
         res = nmea_write (sim-nmea, "PMTK101");
         if (va_arg(ap, drv_status_en*)) *va_arg(ap, drv_status_en*) = res;
         return DRV_READY;

      case PMTK_CMD_WARM_START:
         res = nmea_write (sim-nmea, "PMTK102");
         if (buf) *(drv_status_en*)buf = res;
         return DRV_READY;

      case PMTK_CMD_COLD_START:
         res = nmea_write (sim-nmea, "PMTK103");
         if (buf) *(drv_status_en*)buf = res;
         return DRV_READY;

      case PMTK_CMD_FULL_COLD_START:
         res = nmea_write (sim-nmea, "PMTK104");
         if (buf) *(drv_status_en*)buf = res;
         return DRV_READY;

      case PMTK_CMD_STANDBY_MODE:
         if (*buf == 0)
            res = nmea_write (sim-nmea, "PMTK161,0");
         else
            res = nmea_write (sim-nmea, "PMTK161,1");
         return DRV_READY;

      case PMTK_SET_NMEA_BAUDRATE:
         if (*buf) {
            sprintf (str, "PMTK251,%d", *buf);
            nmea_write (sim-nmea, str);
         }
         else
            nmea_write (sim-nmea, "PMTK251,0"); // default setting
         return DRV_READY;

      case PMTK_API_SET_NMEA_OUTPUT:
         break;

      case PMTK_DT_SBAS_ENABLED:
         if (*buf == 0)
            res = nmea_write (sim-nmea, "PMTK313,0");
         else
            res = nmea_write (sim-nmea, "PMTK313,1");
         return DRV_READY;

      case PMTK_DT_UTC:
         break;
      case PMTK_DT_POS:
         break;
   }
   return DRV_READY;
}

//!@}
