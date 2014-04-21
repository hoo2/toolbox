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

void crypt_test (void)
{
   uint8_t hash [32];
   aes_t key;
   uint8_t pass[11] = "a-password";
   uint8_t text [16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 ,1, 2, 3, 4, 5, 6 };

   sha2 ((uint8_t*)pass,  9, hash, SHA2_256);
   aes_key_init (&key, hash, AES_256);

   aes_encrypt (&key, text, text);
   aes_decrypt (&key, text, text);

   aes_key_deinit (&key);
}

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

int main(void)
{
   acs_test ();
   crypt_test ();
   dsp_test ();
   std_test ();

   while(1)
      ;
}
