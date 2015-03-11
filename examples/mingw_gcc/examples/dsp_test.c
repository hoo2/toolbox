/*
 * dsp_test.c
 *
 *  Created on: 9 Ìבס 2015
 *      Author: hoo2
 */

#include <toolbox.h>

void dsp_test (void)
{
   leaky_int_t li;
   moving_av_t ma;
   float out;

   leaky_int_init (&li, 0.85);
   out = leaky_int (&li, 1);
   out = leaky_int (&li, 1);

   moving_av_init(&ma, 4);
   out = moving_av(&ma, 1);
   out = moving_av(&ma, 1);
   out = moving_av(&ma, 1);

   out+=1;  // Discard warning
}

