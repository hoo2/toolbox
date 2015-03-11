/*
 * std_test.c
 *
 *  Created on: 9 Ìבס 2015
 *      Author: hoo2
 */

#include <toolbox.h>

void std_test (void)
{
   char date[50];
   time_t bd = 310291200;
   struct tm *t = sgmtime (&bd);

   bd = smktime (t);
   sprintf (date, "%04d-%02d-%02d %02d:%02d:%02d\"",
      t->tm_year,
      t->tm_mon,
      t->tm_mday,
      t->tm_hour,
      t->tm_min,
      t->tm_sec);
}
