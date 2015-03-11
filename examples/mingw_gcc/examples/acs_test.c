/*
 * acs_test.c
 *
 *  Created on: 9 Ìבס 2015
 *      Author: hoo2
 */

#include <toolbox.h>

void acs_test (void)
{
   int i;
   pid_c_t pid;
   tne_t tne;

   pid_init(&pid, 1, 0.25, 0.15, 0.1, 0.01);
   tne_init(&tne, 1, 0.1, 0.05, 0.1);
   tne_sat (&tne, 10, -30);

   for (i=0 ; i<20 ; ++i)
   {
      pid_out(&pid, 0.1);
      tne_out(&tne, 0.06);
   }
}
