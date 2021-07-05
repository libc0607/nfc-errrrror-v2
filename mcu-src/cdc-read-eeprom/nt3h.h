#ifndef __NT3H_H__
#define __NT3H_H__

#include <SoftI2C.h>
#define NTAG_I2C_ADDR 0xAA

#define NT3H1K_CONF_BLOCK_ADDR 		0x3A
#define NT3H2K_CONF_BLOCK_ADDR 		0x7A
#define NT3H_SESSION_BLOCK_ADDR 	0xFE
#define NT3H_EEPROM_LOWER_BOUND   0x01
#define NT3H_EEPROM_UPPER_BOUND   0x37
#define NT3H_SESSION_NC_REG 		0x00
#define NT3H_SESSION_NS_REG 		0x06
#define NT3H_SESSION_WDT_LS			0x03
#define NT3H_SESSION_WDT_MS			0x04

uint8_t nt3h1k_i2c_write_eeprom_block (uint8_t block_addr, uint8_t* pdat);
uint8_t nt3h1k_i2c_read_eeprom_block (uint8_t block_addr, uint8_t* pdat);

//uint8_t nt3h1k_write_single_ndef_message(uint8_t msg_len, uint8_t* msg);
//uint8_t nt3h1k_read_single_ndef_message(uint8_t msg_len, uint8_t* msg);

uint8_t nt3h1k_soft_reset();
uint8_t nt3h_set_session_wdt_counter(uint16_t n);
uint8_t nt3h1k_init_00();

#endif
