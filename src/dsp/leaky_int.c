/*!
 * \file leaky_int.c
 * \brief
 *    A leaky integrator filter implementation.
 *
 * This file is part of toolbox
 *
 * Copyright (C) 2014 Houtouridis Christos (http://www.houtouridis.net)
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

#include <dsp/leaky_int.h>

/*!
 * \brief
 *    DeInitialize the double precision leaky integrator
 *    filter pointed by li
 *
 * \param   li,   which filter to de-initialize
 */
__Os__ void iir_li_d_deinit (iir_li_d_t* li) {
   li->l = li->out = 0;
}

/*!
 * \brief
 *    DeInitialize the single precision leaky integrator
 *    filter pointed by li
 *
 * \param   li,   which filter to de-initialize
 */
__Os__ void iir_li_f_deinit (iir_li_f_t* li) {
   li->l = li->out = 0;
}

/*!
 * \brief
 *    Initialize double precision the leaky integrator
 *    filter pointed by li
 *
 * \param   li,   which filter to initialize
 * \param   l,    the lambda factor of the filter
 */
__Os__ void iir_li_d_init (iir_li_d_t* li, double l) {
   li->out = 0;
   li->l = l;
}

/*!
 * \brief
 *    Initialize the single precision leaky integrator
 *    filter pointed by li
 *
 * \param   li,   which filter to initialize
 * \param   l,    the lambda factor of the filter
 */
__Os__ void iir_li_f_init (iir_li_f_t* li, float l) {
   li->out = 0;
   li->l = l;
}

/*!
 * \brief
 *    The double precision leaky integrator function
 *
 * \param   li,      which filter to use
 * \param   value,   the input value
 *
 * \return           The filtered value.
 */
__O3__ double iir_li_d (iir_li_d_t* li, double value) {
   if (isnan (value))
      return (li->out = 0);
   return (li->out = li->out*li->l + (1-li->l)*value);
}

/*!
 * \brief
 *    The single precision leaky integrator function
 *
 * \param   li,      which filter to use
 * \param   value,   the input value
 *
 * \return           The filtered value.
 */
__O3__ float iir_li_f (iir_li_f_t* li, float value) {
   if (isnan (value))
      return (li->out = 0);
   return (li->out = li->out*li->l + (1-li->l)*value);
}
