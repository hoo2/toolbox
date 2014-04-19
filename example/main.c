#include <toolbox.h>

pid_t pid;
tne_t tne;
uint8_t hash [32];
aes_t key;

uint8_t text [16] =
{
   1, 2, 3, 4, 5, 6, 7, 8, 9, 0 ,1, 2, 3, 4, 5, 6
};

int main(void)
{
   int i;

   //pid_init(&pid, 1, 0.25, 0.15, 0.1, 0.01);
   //tne_init(&tne, 1, 0.1, 0.05, 0.1);
   //tne_sat (&tne, 10, -30);

   sha2 ((uint8_t*)"apassword",  9, hash, SHA2_256);
   aes_key_init (&key, hash, AES_256);

   aes_encrypt (&key, text, text);
   aes_decrypt (&key, text, text);

   aes_key_deinit (&key);

   for (i=0 ; i<20 ; ++i)
   {
      //pid_out(&pid, 0.1);
      //tne_out(&tne, 0.06);
   }

   while(1)
      ;
}
