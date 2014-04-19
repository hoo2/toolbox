/*!
 * \file moving_av.c
 * \brief
 *    A moving average filter implementation.
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

#include <dsp/moving_av.h>


/*!
 * \brief
 *    Moving Average filter de-initialization.
 *
 * \param  filter  Which filter to free
 * \retval none
*/
void moving_av_deinit (moving_av_t* filter)
{
   if ( filter->pb )
   {
      free (filter->pb);
      filter->pb = 0;   // NULL out the pointer
      filter->size = filter->c = 0;
   }
}

/*!
 * \brief
 *    Moving Average filter initialization. On un-success
 *    buffer allocation set zero to filter->size also.
 *
 * \param  filter  Which filter to use
 * \param  points  The average filter points (size)
 * \retval On success return the number of points, else 0.
 */
int moving_av_init (moving_av_t* filter, uint8_t points)
{
   if ( (filter->pb = (float*)calloc (points, sizeof(float))) != NULL )
   {
      filter->size = points;
      return filter->size;
   }
   else
      return (filter->size = 0);
}


/*!
 * \brief
 *    Moving Average filter. Output = moving_av (Input)
 *
 * \param  filter  Which filter to use
 * \param  value   The input value.
 *
 * \retval What came out from filter.
 */
float moving_av (moving_av_t* filter, float value)
{
   float res;
   uint8_t  i;

   filter->pb[filter->c] = value;
   if ( ++(filter->c) >= filter->size) //Buffer overflow checking
      filter->c = 0;
   
   //Calculate the filtered value   
   for (res=0, i=0 ; i<filter->size ; ++i)
      res += filter->pb[i];

   if (filter->size)
      return res/filter->size;
   else
      return 0;
}
