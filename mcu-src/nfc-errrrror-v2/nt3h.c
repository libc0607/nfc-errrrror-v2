#include "nt3h.h"

void _nt3h_i2c_read_session_reg_nocheck(uint8_t reg_addr, uint8_t* pdat)
{
  I2CStart();
  I2CSend(NTAG_I2C_ADDR | 0x0);
  I2CSend(NT3H_SESSION_BLOCK_ADDR);
  I2CSend(reg_addr);
  I2CStop();
  I2CStart();
  I2CSend(NTAG_I2C_ADDR | 0x1);
  *pdat = I2CRead();
  I2CAck();
  I2CStop();
}

void nt3h1k_i2c_write_eeprom_block_nocheck(uint8_t block_addr, uint8_t* pdat)
{
  uint8_t i;

  if (block_addr < NT3H_EEPROM_LOWER_BOUND || block_addr > NT3H_EEPROM_UPPER_BOUND) 
    return; 

  // check rf_locked
  i = 0x20;
  while (i & 0x20) {
	_nt3h_i2c_read_session_reg_nocheck(NT3H_SESSION_NS_REG, &i);
  }
  
  // check wr_busy
  i = 0x02;
  while (i & 0x02) {
	_nt3h_i2c_read_session_reg_nocheck(NT3H_SESSION_NS_REG, &i);
  }

  I2CStart();
  I2CSend(NTAG_I2C_ADDR | 0x0);
  I2CSend(block_addr);
  for (i=0; i<16; i++) {
    I2CSend(pdat[i]);
  }
  I2CStop();
  
  delay(10);
  
}

/*

// functions below are not used 
// comment to reduce rom usage  

uint8_t nt3h1k_i2c_write_eeprom_block(uint8_t block_addr, uint8_t* pdat)
{
  uint8_t ack = 1, i = 0;

  // block[0x38] has 8 Bytes user memory and 3 bytes "Dynamic lock bytes"
  // we just don't touch block 0x38
  if (block_addr < NT3H_EEPROM_LOWER_BOUND || block_addr > NT3H_EEPROM_UPPER_BOUND) 
    return 1; // err?

  // wait until RF finish r/w 
  while (1 == _nt3h_get_ns_rf_locked()) {
    delayMicroseconds(100);
  }
  
  // check eeprom is not busy
  while (1 == _nt3h_get_eeprom_wr_busy()) {
    delayMicroseconds(100);
  }
  
  I2CStart();
  ack = I2CSend(NTAG_I2C_ADDR | 0x0);
  if (ack != 0) 
    return ack;
  ack = I2CSend(block_addr);
  if (ack != 0) 
    return ack;
  for (i=0; i<16; i++) {
    ack = I2CSend(pdat[i]);
    if (ack != 0) 
      return ack;
  }
  I2CStop();

  // wait for writing complete, datasheet gives 4.5ms
  while (1 == _nt3h_get_eeprom_wr_busy()) {
    delayMicroseconds(100);
  }
  
  // unlock i2c
  return _nt3h_set_ns_i2c_locked(0);
}


uint8_t _nt3h_i2c_read_session_reg(uint8_t reg_addr, uint8_t* pdat)
{
  uint8_t ack = 1, i = 0;

  I2CStart();
  ack = I2CSend(NTAG_I2C_ADDR | 0x0);
  if (ack != 0) 
    return ack;
  ack = I2CSend(NT3H_SESSION_BLOCK_ADDR);
  if (ack != 0) 
    return ack; 
  ack = I2CSend(reg_addr);
  if (ack != 0) 
    return ack;
  I2CStop();
  I2CStart();
  ack = I2CSend(NTAG_I2C_ADDR | 0x1);
  if (ack != 0) 
    return ack;
  *pdat = I2CRead();
  I2CAck();
  I2CStop();
  return 0;
}

uint8_t nt3h1k_i2c_read_eeprom_block(uint8_t block_addr, uint8_t* pdat)
{
  uint8_t ack = 1, i = 0;

  // block[0x38] has 8 Bytes user memory and 3 bytes "Dynamic lock bytes"
  // we just don't touch block 0x38
  if (block_addr < NT3H_EEPROM_LOWER_BOUND || block_addr > NT3H_EEPROM_UPPER_BOUND) 
    return 1; // err?
    
  I2CStart();
  ack = I2CSend(NTAG_I2C_ADDR | 0x0);
  if (ack != 0) 
    return ack;
  ack = I2CSend(block_addr);
  if (ack != 0) 
    return ack;
  I2CStop();
  I2CStart();
  ack = I2CSend(NTAG_I2C_ADDR | 0x1);
  if (ack != 0) 
    return ack;
  for (i=0; i<16; i++) {
    pdat[i] = I2CRead();
    I2CAck();
  }
  I2CStop();
  // unlock i2c
  return _nt3h_set_ns_i2c_locked(0);
}

uint8_t _nt3h_get_eeprom_wr_err()
{
  uint8_t tmp;
  _nt3h_i2c_read_session_reg(NT3H_SESSION_NS_REG, &tmp);
  return (tmp & 0x04)? 1: 0;
}

uint8_t _nt3h_get_ns_rf_locked()
{
  uint8_t tmp;
  _nt3h_i2c_read_session_reg(NT3H_SESSION_NS_REG, &tmp);
  return (tmp & 0x20)? 1: 0;
}

uint8_t _nt3h_get_eeprom_wr_busy()
{
  uint8_t tmp;
  _nt3h_i2c_read_session_reg(NT3H_SESSION_NS_REG, &tmp);
  return (tmp & 0x02)? 1: 0;
}

uint8_t _nt3h_i2c_write_session_reg(uint8_t reg_addr, uint8_t reg_mask, uint8_t dat)
{
	uint8_t ack = 1, i = 0;

  I2CStart();
  ack = I2CSend(NTAG_I2C_ADDR | 0x0);
  if (ack != 0) 
    return ack; 
  ack = I2CSend(NT3H_SESSION_BLOCK_ADDR);
  if (ack != 0) 
    return ack; 
  ack = I2CSend(reg_addr);
  if (ack != 0) 
    return ack;
  ack = I2CSend(reg_mask);
  if (ack != 0) 
    return ack;  
  ack = I2CSend(dat);
  if (ack != 0) 
    return ack;  
  I2CStop();
  return 0;
}

// I2C timeout = n*9.43us, n:0001h~FFFFh
uint8_t nt3h_set_session_wdt_counter(uint16_t n)
{
  uint8_t ret;
  ret = _nt3h_i2c_write_session_reg(NT3H_SESSION_WDT_LS, 0xFF, (uint8_t)(n&0x00FF) ); 
  ret |= _nt3h_i2c_write_session_reg(NT3H_SESSION_WDT_MS, 0xFF, (uint8_t)((n&0xFF00)>>8) );
  return ret;
}

uint8_t _nt3h_set_ns_i2c_locked(uint8_t i2c_locked)
{
  return _nt3h_i2c_write_session_reg(NT3H_SESSION_NS_REG, 0x40, i2c_locked? 0x40: 0x00);
}

uint8_t nt3h_get_rf_field_presence()
{
  uint8_t tmp;
  _nt3h_i2c_read_session_reg(NT3H_SESSION_NS_REG, &tmp);
  return (tmp & 0x01)? 1: 0;
}

uint8_t _nt3h_get_ns_i2c_locked()
{
  uint8_t tmp;
  _nt3h_i2c_read_session_reg(NT3H_SESSION_NS_REG, &tmp);
  return (tmp & 0x40)? 1: 0;
}


uint8_t _nt3h_reset_eeprom_wr_err()
{
  uint8_t tmp;
  _nt3h_i2c_read_session_reg(NT3H_SESSION_NS_REG, &tmp);
  return _nt3h_i2c_write_session_reg(NT3H_SESSION_NS_REG, 0x04, tmp&(~0x04) );
}

uint8_t nt3h1k_soft_reset()
{
  // NC_REG.I2C_RST_ON_OFF should be set
  I2CStart();
  delayMicroseconds(10);
  I2CStart();
  return 0;
}

uint8_t nt3h1k_init_00()
{
  // we use ntag as a "mailbox" containing wi-fi status
  // just use all dafault values in configuration regs

  return 0;
}
*/
