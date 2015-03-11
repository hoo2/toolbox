#include <toolbox.h>
#include <string.h>
#include <RN_DPEC_v0.3.h>
#include <stm32f10x_systick.h>

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
   uint8_t hash [64];
   //aes_t key;
   //des_t dkey;
   des3_t d3key;
   uint8_t pass[11] = "a-password";
   uint8_t text [16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 ,1, 2, 3, 4, 5, 6 };

   memset ((void*)hash, 0, 64);
   md5 ((uint8_t*)pass,  10, hash);
   sha1 ((uint8_t*)pass, 10, hash);
   sha224 ((uint8_t*)pass,  10, hash);
   sha256 ((uint8_t*)pass,  10, hash);
   sha384 ((uint8_t*)pass,  10, hash);
   sha512 ((uint8_t*)pass,  10, hash);

   //aes128_key_init (&key, hash);
   //aes192_key_init (&key, hash);
   //aes256_key_init (&key, hash);
   //aes_encrypt (&key, text, text);
   //aes_decrypt (&key, text, text);
   //aes_key_deinit (&key);

   des_key_check_parity(hash);
   des_key_set_parity (hash);
   des_key_check_parity(hash);
   //des_setkey_enc (&dkey, hash);
   //des_crypt_ecb (&dkey, text, text);
   //des_setkey_dec (&dkey, hash);
   //des_crypt_ecb (&dkey, text, text);

   des3_set2key_enc(&d3key, hash);
   des3_crypt_ecb(&d3key, text, text);
   des3_set2key_dec(&d3key, hash);
   des3_crypt_ecb(&d3key, text, text);

   des3_set3key_enc(&d3key, hash);
   des3_crypt_ecb(&d3key, text, text);
   des3_set3key_dec(&d3key, hash);
   des3_crypt_ecb(&d3key, text, text);
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

void drv_test (void)
{
   float r1 = 1532, r2 = 167;
   temp_t t;
   ee_t     ee;
   i2c_bb_t i2c;
   char str[] = "These aren\'t the droids you\'re looking for!";

   t = sen_pt100 (r1);
   t = sen_pt100 (r2);
   t = sen_pt1000 (r1);
   t = sen_pt1000 (r2);
   t =  sen_kty8x_121 (r1);
   t =  sen_kty8x_122 (r1);
   t =  sen_kty11_6 (r1);
   t = sen_ntc3997k (r1);
   t = sen_jtype (0.0125, 14);

   // Connect and initialise eeprom
   i2c_link_sda (&i2c, EE_SDA);
   i2c_link_scl (&i2c, EE_SCL);
   i2c_link_sdadir (&i2c, EE_SDA_Dir);
   i2c_set_speed(&i2c, 50000);
   i2c_init (&i2c);

   ee_link_i2c (&ee, (void*)&i2c);
   ee_link_i2c_rx (&ee, (ee_i2c_rx_ft)i2c_rx);
   ee_link_i2c_tx (&ee, (ee_i2c_tx_ft)i2c_tx);
   ee_link_i2c_ioctl (&ee, (ee_i2c_ioctl_ft)i2c_ioctl);
   ee_set_hwaddress (&ee, 0xA0);      // 0xA0 + 000x
   ee_set_size (&ee, EE_128);
   ee_set_page_size(&ee, 64);
   ee_set_timeout(&ee, 0x100);    // ~= 25 msec
   ee_init (&ee);

   ee_write(&ee, 0x100, (byte_t *)str, strlen(str));
   ee_read(&ee, 0x100, (byte_t *)str, strlen(str));

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

void target_init (void)
{
   Driver_Init ();
   SysTick_Init(1000);

   jf_link_setfreq(JF_setfreq);
   jf_link_value((jiffy_t*)&JF_TIM_VALUE);

   jf_init(1000);
}

int main(void)
{
   //target_init ();
   //acs_test ();
   crypt_test ();
   //dsp_test ();
   //std_test ();
   //drv_test ();

   while(1)
      ;
}
