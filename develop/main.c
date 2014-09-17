#include <toolbox.h>
#include <string.h>
#include <STM32F4_Discovery.h>
#include <stm32f4xx_systick.h>


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
   uint8_t text [16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,
                        10, 11, 12, 13, 14, 15, 16 };

   memset ((void*)hash, 0, 64);
   md5 ((uint8_t*)pass,  10, hash);
   sha1 ((uint8_t*)pass, 10, hash);
   sha224 ((uint8_t*)pass,  10, hash);
   sha256 ((uint8_t*)pass,  10, hash);
   sha384 ((uint8_t*)pass,  10, hash);
   sha512 ((uint8_t*)pass,  10, hash);

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

   //aes128_key_init (&key, hash);
   //aes192_key_init (&key, hash);
   //aes256_key_init (&key, hash);
   //aes_encrypt (&key, text, text);
   //aes_decrypt (&key, text, text);
   //aes_key_deinit (&key);
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

unsigned char buffer[512];
unsigned char play[512] = "These aren\'t the droids you\'re looking for!";
spi_bb_t spi;

void drv_test (void)
{
   float r1 = 1532, r2 = 167;
   temp_t t;
   //ee_t ee;
   char str[] = "These aren\'t the droids you\'re looking for!";
   drv_status_en res1, res2;

   /*t = sen_pt100 (r1);
   t = sen_pt100 (r2);
   t = sen_pt1000 (r1);
   t = sen_pt1000 (r2);
   t = sen_kty8x_121 (r1);
   t = sen_kty8x_122 (r1);
   t = sen_kty11_6 (r1);
   t = sen_ntc3997k (r1);
   t = sen_jtype (0.0125, 14);

   ee_deinit(&ee); // Clear data
   i2c_link_scl(&ee.i2c, EE_SCL);
   i2c_link_sda(&ee.i2c, EE_SDA);
   i2c_link_sdadir(&ee.i2c, EE_SDA_Dir);

   ee_set_hwaddress (&ee, 0xA0);      // 0xA0 + 000x
   ee_set_size (&ee, EE_128);
   ee_set_pagesize(&ee, 64);
   ee_set_speed(&ee, 50000);
   ee_set_timeout(&ee, 0x100);    // ~= 25 msec

   ee_init(&ee);
   ee_writebuffer(&ee, 0x100, (uint8_t *)str, strlen(str));
   ee_readbuffer(&ee, 0x100, (uint8_t *)str, strlen(str));
   */
   spi.status = DRV_NOINIT;
   sd_link_cd(0, SD_nCD);
   sd_link_cs(0, SD_nCS);
   sd_link_pw(0, SD_nEN);
   sd_link_spi (0, (void*)&spi);
   sd_link_spi_rw (0, (spi_rw_t)spi_rw);
   sd_link_spi_ioctl (0, (spi_ioctl_t)spi_ioctl);

   spi_link_miso (&spi, SD_MISO);
   spi_link_mosi (&spi, SD_MOSI);
   spi_link_sclk (&spi, SD_CLK);
   //spi_link_ss (&spi, SD_nCS);       // We use sd_cs instead

   spi_set_cpha (&spi, SPI_CPHA_1ST_EDGE);
   spi_set_cpol (&spi, SPI_CPOL_IDLE_LOW);
   spi_set_nss (&spi, SPI_NSS_SOFT);   // We let ss pin to sd_spi driver
   spi_set_freq (&spi, 100000);

   service_add (sd_service, SD_CRON_SET);
   //sd_init (0);
   res1 = sd_ioctl (0, CTRL_INIT, (ioctl_buf_t*)&res2);
   sd_read (0, 10, buffer, 1);
   sd_write (0, 10, play, 1);
   sd_read (0, 10, play, 1);
   sd_write (0, 10, buffer, 1);
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

ui_return_t task_a (void) {return EXIT_RETURN;}
ui_return_t task_b (void) {return EXIT_RETURN;}
ui_return_t task_c (void) {return EXIT_RETURN;}
ui_return_t task_d (void) {return EXIT_RETURN;}
ui_return_t info_entrance (void) {return EXIT_RETURN;}
ui_return_t info_a (void) {return EXIT_RETURN;}
ui_return_t info_b (void) {return EXIT_RETURN;}
ui_return_t info_c (void) {return EXIT_RETURN;}
ui_return_t info_d (void) {return EXIT_RETURN;}
ui_return_t info_e (void) {return EXIT_RETURN;}

/*
void task_a (void) {return ;}
void task_b (void) {return ;}
void task_c (void) {return ;}
void task_d (void) {return ;}
void info_entrance (void) {return ;}
void info_a (void) {return ;}
void info_b (void) {return ;}
void info_c (void) {return ;}
void info_d (void) {return ;}
void info_e (void) {return ;}
*/
const menud_item_t  main_menu [];
const menud_item_t  info_menu [];

const menud_item_t  main_menu [] =
{
   {{"MENU",           "MENU"},            UID_EMPTY,            UI_MM_EN},
   {{"Task a",         "Aufgabe a"},       UID_TASK(task_a),     UI_MM_EN},
   {{"Task b",         "Aufgabe b"},       UID_TASK(task_b),     UI_MM_EN},
   {{"View Info",      "Info anzeigen"},   UID_MENU(info_menu),  UI_MM_EN},
   {{"Task c",         "Aufgabe c"},       UID_TASK(task_c),     UI_MM_EN},
   {{"Task d",         "Aufgabe d"},       UID_TASK(task_d),     UI_MM_EN},
   {{"BACK",           "ZURUCK"},          UID_BACK,             UI_MM_EN},
   {{0,0}, UID_EMPTY,                                            UI_MM_EN}
};
const menud_item_t  info_menu [] =
{
   {{"Informations", "Information"},   UID_TASK(info_entrance),    UI_MM_EN},
   {{"Info a",       "Info a"},        UID_TASK(info_a),           UI_MM_EN},
   {{"Info b",       "Info b"},        UID_TASK(info_b),           UI_MM_EN},
   {{"Info c",       "Info c"},        UID_TASK(info_c),           UI_MM_EN},
   {{"Info d",       "Info d"},        UID_TASK(info_d),           UI_MM_EN},
   {{"Info e",       "Info e"},        UID_TASK(info_e),           UI_MM_EN},
   {{"BACK",         "ZURUCK"},        UID_BACK,                   UI_MM_EN},
   {{0,0}, UID_EMPTY,                                              UI_MM_EN}
};

/*
const menu_item_t  main_menu [];
const menu_item_t  info_menu [];

const menu_item_t  main_menu [] =
{
   {{"MENU",           "MENU"},            UI_EMPTY,            UI_MM_EN},
   {{"Task a",         "Aufgabe a"},       UI_TASK(task_a),     UI_MM_EN},
   {{"Task b",         "Aufgabe b"},       UI_TASK(task_b),     UI_MM_EN},
   {{"View Info",      "Info anzeigen"},   UI_MENU(info_menu),  UI_MM_EN},
   {{"Task c",         "Aufgabe c"},       UI_TASK(task_c),     UI_MM_EN},
   {{"Task d",         "Aufgabe d"},       UI_TASK(task_d),     UI_MM_EN},
   {{"BACK",           "ZURUCK"},          UI_BACK,             UI_MM_EN},
   {{0,0}, UI_EMPTY,                                            UI_MM_EN}
};
const menu_item_t  info_menu [] =
{
   {{"Informations", "Information"},   UI_TASK(info_entrance),    UI_MM_EN},
   {{"Info a",       "Info a"},        UI_TASK(info_a),           UI_MM_EN},
   {{"Info b",       "Info b"},        UI_TASK(info_b),           UI_MM_EN},
   {{"Info c",       "Info c"},        UI_TASK(info_c),           UI_MM_EN},
   {{"Info d",       "Info d"},        UI_TASK(info_d),           UI_MM_EN},
   {{"Info e",       "Info e"},        UI_TASK(info_e),           UI_MM_EN},
   {{"BACK",         "ZURUCK"},        UI_BACK,                   UI_MM_EN},
   {{0,0}, UI_EMPTY,                                              UI_MM_EN}
};
*/
const combobox_item_t      cb [] =
{
  {{"ITEMS",   "ANTIKEIMENA"}, 0},
  {{"item 1",  "antik 1"},     1},
  {{"item 2",  "antik 2"},     2},
  {{"item 3",  "antik 3"},     3},
  {{"item 4",  "antik 4"},     4},
  {{"item 5",  "antik 5"},     5},
  {{0,0},0}
};
tuid_t   tuid;
tui_t   tui;

void ui_test (void)
{
   int key, id = 2;
   float v;
   time_t t=120;
   char s[5] = "0000";

   /*
   tuid_link_framebuffer (&tuid, (uint8_t*)frame_buffer);
   tuid_set_fb_lines (&tuid, TUIFB_LINES);
   tuid_set_fb_columns (&tuid, TUIFB_COLUMNS);
   tuid_set_key_up (&tuid, 1);
   tuid_set_key_down (&tuid, 2);
   tuid_set_key_left (&tuid, 3);
   tuid_set_key_right (&tuid, 4);
   tuid_set_key_enter (&tuid, 4);
   tuid_set_key_enter_l (&tuid, 84);
   tuid_set_key_esc (&tuid, 5);

   tui_menud_init (&tuid);
*/
   /*
   tui_link_framebuffer (&tui, (uint8_t*)frame_buffer);
   tui_link_get_key (&tui, btn_getkey);
   tui_set_fb_lines (&tui, TUIFB_LINES);
   tui_set_fb_columns (&tui, TUIFB_COLUMNS);
   tui_set_key_up (&tui, 1);
   tui_set_key_down (&tui, 2);
   tui_set_key_left (&tui, 3);
   tui_set_key_right (&tui, 4);
   tui_set_key_enter (&tui, 4);
   tui_set_key_enter_l (&tui, 84);
   tui_set_key_esc (&tui, 5);

   tui_menu_init (&tui);

   while (1) {
      while (1) {
         key = btn_getkey(0);
         if (tui_menud (&tuid, key, (menud_item_t*)main_menu, LANG_EN) == EXIT_RETURN)
            break;
      }
      while (1) {
         key = btn_getkey(0);
         if (tui_comboboxd (&tuid, key, (combobox_item_t*)cb, &id, LANG_EN) == EXIT_RETURN)
            break;
      }
      while (1) {
         key = btn_getkey(0);
         if (tui_valueboxd(&tuid, key, "Value", "m/s", 300000000, 0, 1, 1, &v) == EXIT_RETURN)
            break;
      }
      while (1) {
         key = btn_getkey(0);
         if ( tui_timeboxd(&tuid, key, "Set time", UI_TIME_HH|UI_TIME_MM|UI_TIME_SS, 3600*24, 1, 60, &t) == EXIT_RETURN)
            break;
      }
      while (1) {
         key = btn_getkey(0);
         if (tui_textboxd (&tuid, key, "Text please", s, 4) == EXIT_RETURN)
            break;
      }
      //tui_menu (&tui, (menu_item_t*)main_menu, LANG_EN);
      //id = tui_combobox (&tui, (combobox_item_t*)cb, id, LANG_EN);
      //v = tui_valuebox (&tui, "Value please", "Km/h", 300, 15, 5, 0, 50);
      //t = tui_timebox (&tui, "Time please", UI_TIME_HH|UI_TIME_MM|UI_TIME_SS, 3600, 60, 60, t);
      //tui_textbox (&tui, "Text please", s, strlen (s));
   }*/
}

void target_init (void)
{
   Driver_Init ();
   SysTick_Init (1000);
   //System_Init (1000);

   jf_link_setfreq (JF_setfreq);
   jf_link_value ((jiffy_t*)&JF_TIM_VALUE);

   jf_init (1000);
}

int main(void)
{
   uint8_t cd;
   time_t t;

   target_init ();
   //acs_test ();
   //crypt_test ();
   //dsp_test ();
   //std_test ();
   drv_test ();
   //ui_test ();

   while(1)
   {
      t = time (0);
      while ( t+1 > time (0) );
   }
}
