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

static char _mask_str[47];

static const uint32_t _nmea_out_freq_def[] = {
   0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0
};

static uint32_t _nmea_out_freq[] = {
   0, // SIMX8_GLL [0]
   1, // SIMX8_RMC [1]
   0, // SIMX8_VTG [2]
   1, // SIMX8_GGA [3]
   1, // SIMX8_GSA [4]
   1, // SIMX8_GSV [5]
   0, // SIMX8_RES [6]
   0, // SIMX8_RES [7]
   0, // SIMX8_RES [8]
   0, // SIMX8_RES [9]
   0, // SIMX8_RES [10]
   0, // SIMX8_RES [11]
   0, // SIMX8_RES [12]
   0, // SIMX8_RES [13]
   0, // SIMX8_RES [14]
   0, // SIMX8_RES [15]
   0, // SIMX8_RES [16]
   0, // SIMX8_ZDA [17]
};

/*
 * Static API
 */
#define _chk_update(_p, _x)      do{ if (_p) _p = _x; }while (0)

static void _set_nmea_output (nmea_t *nmea, nmea_output_en cmd, uint32_t freq)
{
   uint32_t i;
   char str[3];

   if (cmd == SIMX8_DEF) {
      for (i=0 ; i<SIMX8_NUMBER_OF_SENTENCES ; ++i) {
         _nmea_out_freq[i] = _nmea_out_freq_def[i];
      }
      nmea_write (nmea, NMEA_OUTPUT_DEFAULT);
   }
   else {
      _nmea_out_freq[cmd] = freq;

      strcpy (_mask_str, NMEA_OUTPUT_INIT_MASK_STR);
      for (i=0 ; i<SIMX8_NUMBER_OF_SENTENCES ; ++i) {
         sprintf (str, "%u,", (unsigned int)_nmea_out_freq[i]);
         strcat (_mask_str, str);
      }
      _mask_str[strlen (_mask_str) - 1] = 0;   // eat last ','
      nmea_write (nmea, _mask_str);
   }

}

/*
 * ============ Public NMEA API ============
 */

/*
 * Link and Glue functions
 */
inline void simX8_link_nmea (simX8_t *sim, nmea_t *nmea) {
   sim->nmea = nmea;
}


/*
 * Set functions
 */
inline void simX8_set (simX8_t *sim, int disc) {}



/*
 * User functions
 */

/*!
 * \brief
 *    De-Initializes simX8 driver.
 *
 * \param  sim    Pointer to linked simX8_t data stuct to use
 */
void simX8_deinit (simX8_t *sim) {
   memset ((void*)sim, 0, sizeof (simX8_t));
   /*!<
    * This leaves the status = DRV_NOINIT
    */
}

/*!
 * \brief
 *    Initialises simX8.
 *
 * \param  sim    Pointer to linked simX8_t data stuct to use
 * \return        The status of the operation
 */
drv_status_en simX8_init (simX8_t *sim)
{
   if (!sim->nmea)   return sim->status = DRV_ERROR;

   if (sim->status == DRV_BUSY || sim->status == DRV_NODEV)
      return sim->status = DRV_ERROR;

   return sim->status = DRV_READY;
}

/*!
 * \brief
 *    Read and extract GGA data from NMEA
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   gga   Pointer to gga data for the results
 *                The gga variable is written only when we have position fix
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GGA sentence in stream
 *    \arg  DRV_BUSY    No GPS fix
 *    \arg  DRV_READY   Success, GPS fix
 */
inline drv_status_en simX8_read_gga (simX8_t *sim, nmea_gga_t *gga) {
   return nmea_read_gga (sim->nmea, gga);
}

/*!
 * \brief
 *    Read and extract GLL data from NMEA
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   gll   Pointer to gll data for the results
 *                The gll variable is written only when we have position fix
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GLL sentence in stream
 *    \arg  DRV_BUSY    No GPS fix
 *    \arg  DRV_READY   Success, GPS fix
 */
inline drv_status_en simX8_read_gll (simX8_t *sim, nmea_gll_t *gll) {
   return nmea_read_gll (sim->nmea, gll);
}

/*!
 * \brief
 *    Read and extract GSA data from NMEA
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   gsa   Pointer to gsa data for the results
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GSA sentence in stream
 *    \arg  DRV_BUSY    No GPS fix
 *    \arg  DRV_READY   Success, GPS fix
 * \note    Not implemented yet
 */
inline drv_status_en simX8_read_gsa (simX8_t *sim, nmea_gsa_t *gsa) {
   return nmea_read_gsa (sim->nmea, gsa);
}

/*!
 * \brief
 *    Read and extract GSV data from NMEA
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   gsv   Pointer to gsv data for the results
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid GSV sentence in stream
 *    \arg  DRV_BUSY    No GPS fix
 *    \arg  DRV_READY   Success, GPS fix
 * \note    Not implemented yet
 */
inline drv_status_en simX8_read_gsv (simX8_t *sim, nmea_gsv_t *gsv) {
   return nmea_read_gsv (sim->nmea, gsv);
}

/*!
 * \brief
 *    Read and extract RMC data from NMEA
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   rmc   Pointer to rmc data for the results
 *                The rmc variable is written only when we have position fix
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid RMC sentence in stream
 *    \arg  DRV_BUSY    No GPS fix
 *    \arg  DRV_READY   Success, GPS fix
 */
inline drv_status_en simX8_read_rmc (simX8_t *sim, nmea_rmc_t *rmc) {
   return nmea_read_rmc (sim->nmea, rmc);
}

/*!
 * \brief
 *    Read and extract VTG data from NMEA
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   vtg   Pointer to vtg data for the results
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid VTG sentence in stream
 *    \arg  DRV_BUSY    No GPS fix
 *    \arg  DRV_READY   Success, GPS fix
 */
inline drv_status_en simX8_read_vtg (simX8_t *sim, nmea_vtg_t *vtg) {
   return nmea_read_vtg (sim->nmea, vtg);
}

/*!
 * \brief
 *    Read and extract ZDA data from NMEA
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   zda   Pointer to zda data for the results
 * \return        The status of the operation
 *    \arg  DRV_ERROR   No valid ZDA sentence in stream
 *    \arg  DRV_BUSY    No GPS fix
 *    \arg  DRV_READY   Success, GPS fix
 */
inline drv_status_en simX8_read_zda (simX8_t *sim, nmea_zda_t *zda) {
   return nmea_read_zda (sim->nmea, zda);
}

/*!
 * \brief
 *    Send command to simX8
 * \param   sim   Pointer to linked simX8_t data struct to use
 * \param   cmd   The command to simX8
 *    \arg CTRL_DEINIT                 Send De-Init request
 *       ex: simX8_ctrl (&sim_data, CTRL_DEINIT);
 *
 *    \arg CTRL_INIT                   Send Init request. The next argument takes back the result
 *       ex: simX8_ctrl (&sim_data, CTRL_INIT, (drv_status_en*)&result);
 *
 *    \arg PMTK_CMD_HOT_START          Send PMTK_CMD_HOT_START command, takes no other argument
 *       ex: simX8_ctrl (&sim_data, PMTK_CMD_HOT_START);
 *
 *    \arg PMTK_CMD_WARM_START         Send PMTK_CMD_WARM_START command, takes no other argument
 *       ex: simX8_ctrl (&sim_data, PMTK_CMD_WARM_START);
 *
 *    \arg PMTK_CMD_COLD_START         Send PMTK_CMD_COLD_START command, takes no other argument
 *       ex: simX8_ctrl (&sim_data, PMTK_CMD_COLD_START);
 *
 *    \arg PMTK_CMD_FULL_COLD_START    Send PMTK_CMD_FULL_COLD_START command, takes no other argument
 *       ex: simX8_ctrl (&sim_data, PMTK_CMD_FULL_COLD_START);
 *
 *    \arg PMTK_CMD_STANDBY_MODE       Send PMTK_CMD_FULL_COLD_START command. The next argument is 0(dis) or 1(en)
 *       ex: simX8_ctrl (&sim_data, PMTK_CMD_STANDBY_MODE, 1);
 *
 *    \arg PMTK_API_SET_NMEA_OUTPUT    Send a PMTK_API_SET_NMEA_OUTPUT command.  Next arguments are Sentence and frequency
 *        ex: simX8_ctrl (&sim_data, PMTK_API_SET_NMEA_OUTPUT, (uint32_t)SIMX8_ZDA, 1);
 *
 *    \arg PMTK_SET_NMEA_BAUDRATE      Send a PMTK_SET_NMEA_BAUDRATE command. Next argument is baudrate
 *       ex: simX8_ctrl (&sim_data, PMTK_SET_NMEA_BAUDRATE, 57600);
 *
 *    \arg PMTK_DT_SBAS_ENABLED        Send a PMTK_DT_SBAS_ENABLED command. The next argument is 0(dis) or 1(en)
 *       ex: simX8_ctrl (&sim_data, PMTK_DT_SBAS_ENABLED, 1);
 */
drv_status_en simX8_ctl (simX8_t *sim, ioctl_cmd_t cmd, ...)
{
   __VALIST ap;
   char str[20];
   drv_status_en res;
   uint32_t i,j;

   va_start(ap, (ioctl_cmd_t)cmd);  // start va list
   switch (cmd) {
      case CTRL_DEINIT:          // De-Init
         simX8_deinit (sim);
         break;
      case CTRL_INIT:            // Init
         res = simX8_init (sim);
         _chk_update (*va_arg(ap, drv_status_en*), res);
         break;

      case PMTK_CMD_HOT_START:   // Hot start, no other argument
         nmea_write (sim->nmea, "PMTK101");
         break;

      case PMTK_CMD_WARM_START:  // Warm start, no other argument
         nmea_write (sim->nmea, "PMTK102");
         break;

      case PMTK_CMD_COLD_START:  // Cold start, no other argument
         nmea_write (sim->nmea, "PMTK103");
         break;

      case PMTK_CMD_FULL_COLD_START:   // Full cold start, no other argument
         nmea_write (sim->nmea, "PMTK104");
         break;

      case PMTK_CMD_STANDBY_MODE:      // Standby mode, Next argument is 0 or 1
         if (va_arg(ap, uint32_t) == 0)
            res = nmea_write (sim->nmea, "PMTK161,0");
         else
            res = nmea_write (sim->nmea, "PMTK161,1");
         break;

      case PMTK_SET_NMEA_BAUDRATE:  // Buadrate selection
         i = va_arg(ap, uint32_t);  // Take desired baudrate
         if (i != 0) {
            sprintf (str, "PMTK251,%u", (unsigned int)i);
            nmea_write (sim->nmea, str);
         }
         else
            nmea_write (sim->nmea, "PMTK251,0"); // default setting
         break;

      case PMTK_API_SET_NMEA_OUTPUT:   // API set NMEA Output
         i = va_arg(ap, uint32_t);     // Take sentence
         j = va_arg(ap, uint32_t);     // Take frequency
         _set_nmea_output (sim->nmea, i, j);
         break;

      case PMTK_DT_SBAS_ENABLED:       // SBAS
         if (va_arg(ap, uint32_t) == 0)
            res = nmea_write (sim->nmea, "PMTK313,0");
         else
            res = nmea_write (sim->nmea, "PMTK313,1");
         break;

      case PMTK_DT_UTC:
         break;
      case PMTK_DT_POS:
         break;
   }
   va_end(ap);
   return DRV_READY;
}

