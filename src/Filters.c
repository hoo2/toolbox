/*

	Filters.c - Is the filters API.
   includes:
      Averege Low pass filter

   Copyright (C) 2011 Houtouridis Christos (http://houtouridis.blogspot.com/)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
   Date:       7/2012
   Version:    0.1
 */

#include "Filters.h"


/*!
 * \brief
 *    Moving Average filter de-initialization.
 *
 * \param  buffer  Which buffer to free
 *
 * \retval If success the number of points, else 0
*/
int MovingAv_DeInit (MovingAv_t* buffer)
{
   uint8_t  ret = buffer->size;
   if ( buffer->pb )
   {
      free(buffer->pb);
      buffer->pb = 0;   // NULL the pointer for later KnockKnock
      buffer->size = buffer->c = 0;
      return ret;
   }
   else
      return 0;
}

/*!
 * \brief
 *    Moving Average filter initialization. On un-success
 *    buffer allocation set zero to buffer->size also.
 *
 * \param  buffer  Which buffer to use
 * \param  points  The average filter points (size)
 *
 * \retval If succes the number of points, else 0
 */
int MovingAv_Init (MovingAv_t* buffer, uint8_t points)
{
   if ( (buffer->pb = (float*)calloc (points, sizeof(float))) != NULL )
   {
      buffer->size = points;
      return buffer->size;
   }
   else
      return (buffer->size = 0);
}


/*!
 * \brief
 *    Moving Average filter.
 *
 * \param  buffer  Which buffer to use
 * \param  value   The input value.
 *
 * \retval What came out from buffer.
 */
float MovingAv (MovingAv_t* buffer, float value)
{
   float res;
   uint8_t  i;

   buffer->pb[buffer->c] = value;
   if ( ++(buffer->c) >= buffer->size) //Buffer overflow checking
      buffer->c = 0;
   
   //Calculate the filtered value   
   for (res=0, i=0 ; i<buffer->size ; ++i)
      res += buffer->pb[i];

   if (buffer->size)
      return res/buffer->size;
   else
      return 0;
}

/*!
 * \brief
 *    DeInitialize the leaky integrator filter pointed by li
 *
 * \param   li,   which filter to de-initialize
 */
void LeakyInt_DeInit (LeakyInt_t* li) {
   li->lambda = li->out = 0;
}

/*!
 * \brief
 *    Initialize the leaky integrator filter pointed by li
 *
 * \param   li,   which filter to initialize
 * \param   l,    the lambda factor of the filter
 */
void LeakyInt_Init (LeakyInt_t* li, float l) {
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
float LeakyInt (LeakyInt_t* li, float value) {
   return (li->out = li->out*li->lambda + (1-li->lambda)*value);
}
