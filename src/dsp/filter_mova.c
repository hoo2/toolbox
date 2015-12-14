/*!
 * \file filter_mova.c
 * \brief
 *    A recursive moving average filter implementation.
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

#include <dsp/filter_mova.h>

/*
 * =================== Public API =====================
 */

/*
 * Link and Glue functions
 */


/*
 * Set functions
 */

/*!
 * \brief
 *    Set the size of data/points
 *
 * \param   f     Which filter to use
 * \param   size  The size in size_t
 * \return        none
*/
void filter_mova_set_item_size (filter_mova_t *f, uint32_t size) {
   f->it_size = size;
}

/*!
 * \brief
 *    Set the normalised cut-off frequency of the filter.
 *    The range is 0 to 0.5, where 0.5 is half of the sampling frequency
 *
 * \param   f     Which filter to use
 * \param   fc    The normalised cut-off frequency of the filter
 * \return        none
*/
void filter_mova_set_fc (filter_mova_t *f, double fc) {
   f->N = FILTER_MOVA_SAMPLES (fc);
}


/*
 * User Functions
 */

/*!
 * \brief
 *    Moving Average filter de-initialisation.
 *
 * \param  f      Which filter to free
 * \return none
*/
void filter_mova_deinit (filter_mova_t* f) {
   if ( f->bf )
      free ((void*)f->bf);
   memset ((void*)f, 0, sizeof (filter_mova_t));
}

/*!
 * \brief
 *    Moving Average filter initialisation.
 *
 * \param  f      Which filter to use
 * \param  fc     The normalised cutoff frequency [0fs - 0.5fs]
 * \return        None
 */
uint32_t filter_mova_init (filter_mova_t* f)
{
   // Check sample points for cutoff frequency
   if (f->N == 0)
      return 0;

   // Try to allocate memory
   if ( (f->bf = (void*)calloc (f->N, f->it_size)) != NULL ) {
      // Clear accumulator and cursor
      memset ((void*)f->last, 0, FILTER_MOVA_LAST_SIZE);
      f->c = 0;
      return f->N;
   }
   else
      return 0;
}



/*!
 * \brief
 *    Recursive moving average algorithm
 */
#define  _filter_body(_rtype, _type)  {   \
   _type dep;                             \
                                          \
   dep = ((_type*)f->bf)[f->c];     /* Save departed point */        \
   ((_type*)f->bf)[f->c] = in;      /* Get new value */              \
   if ( ++(f->c) >= f->N)           /* Buffer overflow checking */   \
      f->c = 0;                           \
   /* Recursive calculation */            \
   return *(_rtype*)(f->last) += (_rtype)(in - dep)/f->N;            \
}

#define  _double     double
#define  _float      float
#define  _int        int
#define  _complex_d  complex_d_t
#define  _complex_f  complex_f_t
#define  _complex_i  complex_i_t

/*!
 * \brief
 *    Double precision recursive Moving Average filter.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
double filter_mova_d (filter_mova_t* f, double in) {
   _filter_body(_double, _double);
}

/*!
 * \brief
 *    Single precision recursive Moving Average filter.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
float filter_mova_f (filter_mova_t* f, float in) {
   _filter_body(_float, _float);
}

/*!
 * \brief
 *    Integer recursive Moving Average filter returning
 *    single precision float.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
float filter_mova_i (filter_mova_t* f, int in) {
   _filter_body(_float, _int);
}

/*!
 * \brief
 *    Double precision complex recursive Moving Average filter.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
complex_d_t filter_mova_cd (filter_mova_t* f, complex_d_t in) {
   _filter_body(_complex_d, _complex_d);
}

/*!
 * \brief
 *    Single precision complex recursive Moving Average filter.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
complex_f_t filter_mova_cf (filter_mova_t* f, complex_f_t in) {
   _filter_body(_complex_f, _complex_f);
}

/*!
 * \brief
 *    Integer complex recursive Moving Average filter, returning
 *    single precision complex.
 *    Output = Moving_Average (Input)
 *
 * \param  f      Which filter to use
 * \param  in     The input value.
 *
 * \return        Filtered value
 */
complex_f_t filter_mova_ci (filter_mova_t* f, complex_i_t in) {
   _filter_body(_complex_f, _complex_i);
}

#undef   _filter_body
#undef  _double
#undef  _float
#undef  _int
#undef  _complex_d
#undef  _complex_f
#undef  _complex_i


