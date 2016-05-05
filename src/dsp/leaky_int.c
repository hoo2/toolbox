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
 *    DeInitialize the leaky integrator filter pointed by li
 *
 * \param   li,   which filter to de-initialize
 */
__Os__ void leaky_int_deinit (leaky_int_t* li) {
   li->lambda = li->out = 0;
}

/*!
 * \brief
 *    Initialize the leaky integrator filter pointed by li
 *
 * \param   li,   which filter to initialize
 * \param   l,    the lambda factor of the filter
 */
__Os__ void leaky_int_init (leaky_int_t* li, double l) {
   li->out = 0;
   li->lambda = l;
}

/*!
 * \brief
 *    The leaky integrator function
 *
 * \param   li,      which filter to use
 * \param   value,   the input value
 *
 * \return           The filtered value.
 */
__O3__ double leaky_int (leaky_int_t* li, double value) {
   if (isnan (value))
      return (li->out = 0);
   return (li->out = li->out*li->lambda + (1-li->lambda)*value);
}
