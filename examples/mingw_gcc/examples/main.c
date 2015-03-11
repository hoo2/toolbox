#include <toolbox.h>

extern void acs_test (void);
extern void crypt_test (void);
extern void dsp_test (void);
extern void drv_test (void);}
extern void std_test (void);

int main(void)
{
   acs_test ();
   //crypt_test ();
   //dsp_test ();
   //std_test ();
   //drv_test ();

   while(1)
      ;
}
