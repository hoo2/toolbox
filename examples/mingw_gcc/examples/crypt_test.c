/*
 * crypt_test.c
 *
 *  Created on: 9 Ìבס 2015
 *      Author: hoo2
 */

#include <toolbox.h>

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

