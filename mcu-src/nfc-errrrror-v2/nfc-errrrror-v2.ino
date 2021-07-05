/*
   NFC ERRRROR V2

   A simple NFC tag with USB
   Hardware design at https://oshwhub.com/libc0607/ch552-nfc-v1_copy
   Codes at Github: https://github.com/libc0607/nfc-errrrror-v2
   The code of USB Mass Storage is from https://github.com/DeqingSun/ch55xduino

   To compile, install https://github.com/DeqingSun/ch55xduino first,
   Config: "CH552 Board", "USER CODE w/ 148B USB RAM", "16MHz, 3.3V"

   Note that I've found <Github: LieBtrau/arduino-ntag>
   but it is too heavy for ch552 so I wrote these dirty pieces.

   Github @libc0607
*/

#ifndef USER_USB_RAM
#error "This program needs to be compiled with a USER USB setting, choose <USER CODE w/ 148B USB RAM> in <Tools - USB Settings>"
#endif
#ifndef CH552
#error "This program is designed running on CH552"
#endif
#if (F_CPU != 16000000)
#error "This program is designed running at 16MHz clock (3.3V)"
#endif

#include "nt3h.h"
#include "USBMassStorage.h"
#include "ndef_simple_wifi.h"

#define PIN_SDA   14  // OC with pull-up
#define PIN_SCL   17  // OC with pull-up
#define PIN_FD    16  // OC with pull-up
#define PIN_LED   15  // Push-Pull, PWM1, HIGH=led_on

__xdata uint8_t	glb_wifi_ssid[MAX_WIFI_SSID_LEN];
__xdata uint8_t	glb_ssid_len;
__xdata uint8_t	glb_pwd[MAX_WIFI_PWD_LEN];
__xdata uint8_t	glb_pwd_len;
__xdata uint8_t	glb_auth;
__xdata uint8_t	glb_enc;
__xdata uint8_t	glb_needs_sync;
__xdata uint8_t	glb_msg[MAX_NDEF_MSG_LEN_ALIGNED]; 
__xdata uint8_t	glb_msg_len;
__xdata uint32_t timestamp_ms, old_timestamp_ms, led_cnt;
__code const uint8_t led_pwm_sequence_period = 100;	// ms
// 重构后全用全局变量 之前爆栈了md

// LED PWM Sequence, 0(off)~255(on), update every led_pwm_sequence_period while idle
__code const uint8_t led_pwm_sequence[] = {
	32,32,32,32, 32,32,0,0,
	32,32,32,32, 32,32,32,32,	
	32,32,32,32, 32,32,0,8,
	32,32,0,0,   32,32,32,32,
	32,32,32,32, 32,32,32,32,		
};
__code const uint8_t led_pwm_sequence_fd[] = {
	128,8,128,0,				
};

__code const uint8_t null_ndef_message_first_block[NT3H_BYTES_PER_BLOCK+4] = {
	0x03, 0x00, 0xFE, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

void setup() 
{
	USBInit();
	delay(10);

	// override I2CInit() here, we have no pull-up resistor
	// INPUT_PULLUP = standard 8051 io with pull-up (Pn_MOD_OC=1 & Pn_DIR_PU=1)
	pinMode(PIN_SDA, INPUT_PULLUP);
	pinMode(PIN_SCL, INPUT_PULLUP);
	pinMode(PIN_FD, INPUT_PULLUP);
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_SCL, HIGH);
	digitalWrite(PIN_SDA, HIGH);
	scl_pin = PIN_SCL;
	sda_pin = PIN_SDA;
	
	// startup blink
	analogWrite(PIN_LED, 32);
	delay(100);
	analogWrite(PIN_LED, 255);
	delay(100);
	analogWrite(PIN_LED, 32);
	delay(100);
	analogWrite(PIN_LED, 255);
	delay(100);
	analogWrite(PIN_LED, 32);

	glb_needs_sync = SYNC_TYPE_NONE;
	led_cnt = 0;
}

void loop() 
{
	uint8_t i;
	
	// 1. check if eeprom needs update 
	if (glb_needs_sync == SYNC_TYPE_WIFI) {

		// 1.a. Wi-Fi config
		// generate NDEF msg, save to glb_msg[]
		generate_simple_ndef_wifi_message();
		
		// Write glb_msg[] to ntag eeprom
		for (i = 0; i < (glb_msg_len / 16) + 1; i++) { 
			nt3h1k_i2c_write_eeprom_block_nocheck(i+NT3H_EEPROM_LOWER_BOUND, glb_msg + i*16);
			delay(40);
			analogWrite(PIN_LED, (i%2==0)? 32: 255);	// blink led while updating
		}
		analogWrite(PIN_LED, 255);	// update LED
		glb_needs_sync = SYNC_TYPE_NONE;
		
	} else if (glb_needs_sync == SYNC_TYPE_FORMAT) {

		// 1.b. format eeprom
		// We just Format the maximum possible used area (for Wi-Fi message) to 0x00
		nt3h1k_i2c_write_eeprom_block_nocheck(NT3H_EEPROM_LOWER_BOUND, null_ndef_message_first_block);
		delay(40);
		analogWrite(PIN_LED, 32);
		for (i = 1; i < MAX_FORMAT_BLOCKS; i++) { 
			nt3h1k_i2c_write_eeprom_block_nocheck(i+NT3H_EEPROM_LOWER_BOUND, null_ndef_message_first_block+4);
			delay(40);
			analogWrite(PIN_LED, (i%2==0)? 32: 255);	// blink led while updating
		}
		analogWrite(PIN_LED, 255); // update LED
		glb_needs_sync = SYNC_TYPE_NONE;	
	}

	
	// 2. update led
	// Note: call millis() will cause reset here and i don't know why
	// so just use delay() 
	delay(led_pwm_sequence_period);
	led_cnt++;
	
	if (digitalRead(PIN_FD) == LOW) {
		// 2.a. RF field detected
		analogWrite(PIN_LED, led_pwm_sequence_fd[led_cnt % sizeof(led_pwm_sequence_fd)]);
	} else {
		// 2.b. RF field not detected
		analogWrite(PIN_LED, led_pwm_sequence[led_cnt % sizeof(led_pwm_sequence)]);
	}
}

// 草了 就这点儿破代码 写了三天 重构了两次 一次爆栈一次跑飞
// md什么时候stm32才能降回六块五