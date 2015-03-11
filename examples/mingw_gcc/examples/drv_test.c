/*
 * drv_test.c
 *
 *  Created on: 9 Ìבס 2015
 *      Author: hoo2
 */

#include <toolbox.h>
#include <string.h>
#include <stdlib.h>

/*!
 * Target API
 */
#define MAX_RESISTOR_VALUE		(2700)

res_t read_input (void) {
	static int i=0;

	if (!i)
		srand ((unsigned int)time(NULL));
	return rand () % (MAX_RESISTOR_VALUE+1);
}

uint8_t I2C_SDA_PIN (uint8_t hi) {
	return hi;
}
void I2C_SCL_PIN (uint8_t hi) {
	return;
}
void I2C_SDA_DIR (uint8_t out) {
	return;
}

/*
 * ======== Test ===========
 */
void sensor_test (void)
{
   temp_t t;

   t = sen_pt100 (read_input ());
   t = sen_pt1000 (read_input ());
   t = sen_kty8x_121 (read_input ());
   t = sen_kty8x_122 (read_input ());
   t = sen_kty11_6 (read_input ());
   t = sen_ntc3997k (read_input ());
   t = sen_jtype (0.0125, 14);
}

void ee_test (void)
{
   ee_t   ee;
   i2c_bb_t i2c;
   char str[] = "These aren\'t the droids you\'re looking for!";

   // Connect and initialise eeprom
   i2c_link_sda (&i2c, I2C_SDA_PIN);
   i2c_link_scl (&i2c, I2C_SCL_PIN);
   i2c_link_sdadir (&i2c, I2C_SDA_DIR);
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

void drv_test (void)
{
	sensor_test ();
	ee_test ();
}
