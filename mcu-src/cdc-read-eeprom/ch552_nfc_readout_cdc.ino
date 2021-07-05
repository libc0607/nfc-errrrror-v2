/*
 * CH552 reading NT3H1101 Demo
 * 
 * Hardware design at https://oshwhub.com/libc0607/ch552-nfc-v1_copy
 * Github https://github.com/libc0607/nfc-errrrror-v2
 * 
 * Install https://github.com/DeqingSun/ch55xduino , 
 * Config: "CH552 Board", "Default CDC", "16MHz, 3.3V"
 * 
 * Github @libc0607 
 */

#ifdef USER_USB_RAM
#error "This example needs to be compiled with USB CDC"
#endif
#if (F_CPU != 16000000)
#error "This example is designed running at 16MHz clock (3.3V)"
#endif

#include "nt3h.h"

// ========= Pin Def ===========
#define PIN_SDA   14  // OC with pull-up
#define PIN_SCL   17  // OC with pull-up
#define PIN_FD    16  // OC with pull-up
#define PIN_LED   15  // Push-Pull， PWM1, HIGH=led_on

void usb_print_16bytes_dump(uint8_t index, uint8_t* dat) 
{
  uint8_t i=0;
  
  USBSerial_print_c('[');
  USBSerial_print_ub(index/16, 16);
  USBSerial_print_ub(index%16, 16);
  USBSerial_print_c('h');
  USBSerial_print_c(']');
  USBSerial_print_c(':');
  USBSerial_print_c(' ');
  
  for (i=0; i<16; i++) {
    USBSerial_print_ub(dat[i]/16, 16);
    USBSerial_print_ub(dat[i]%16, 16);
    USBSerial_print_c(' ');
  }
  USBSerial_println_c(';');
}

const uint8_t test_write_start_block[16] = {
  0x03, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const uint8_t test_write_fill_block[16] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {
  uint8_t i, ack;
  uint8_t buf[16];
  
  delay(10);
  pinMode(PIN_SDA, INPUT_PULLUP);
  pinMode(PIN_SCL, INPUT_PULLUP);   
  pinMode(PIN_FD, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_SDA, HIGH);
  digitalWrite(PIN_SCL, HIGH);
  scl_pin = PIN_SCL;
  sda_pin = PIN_SDA;

  while (!USBSerial());
  USBSerial_println_s("[INF] ");
  delay(2000);
  USBSerial_println_s("[INF] USB serial connected, startup.");
  
  USBSerial_println_s("[DBG] Dump eeprom start:");
  for (i=NT3H_EEPROM_LOWER_BOUND; i<NT3H_EEPROM_UPPER_BOUND; i++) {
    ack = nt3h1k_i2c_read_eeprom_block(i, buf);
    usb_print_16bytes_dump(i, buf);
  }
  USBSerial_println_s("[DBG] Dump eeprom end;");
  
  delay(2000);
  
  USBSerial_println_s("[DBG] wipe eeprom start:");
  nt3h1k_i2c_write_eeprom_block(NT3H_EEPROM_LOWER_BOUND, test_write_start_block);
  for (i=NT3H_EEPROM_LOWER_BOUND+1; i<NT3H_EEPROM_UPPER_BOUND; i++) {
    ack = nt3h1k_i2c_write_eeprom_block(i, test_write_fill_block);
  }
  USBSerial_println_s("[DBG] wipe eeprom end;");

  delay(2000);
  
  USBSerial_println_s("[DBG] Dump eeprom start:");
  for (i=NT3H_EEPROM_LOWER_BOUND; i<NT3H_EEPROM_UPPER_BOUND; i++) {
    ack = nt3h1k_i2c_read_eeprom_block(i, buf);
    usb_print_16bytes_dump(i, buf);
  }
  USBSerial_println_s("[DBG] Dump eeprom end;");
  
}

void loop() {
  

}
