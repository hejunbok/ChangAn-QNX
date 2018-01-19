#ifndef I2C_IO_H
#define I2C_IO_H

#include <stdint.h>

int i2c_open();

void i2c_close();

int i2c_write(const uint8_t* data, uint8_t len);

int i2c_read(const uint8_t* subaddr, uint8_t subaddr_len, uint8_t* data, uint8_t len);

int i2c_write_high(uint8_t subaddr, const uint8_t* data, uint8_t len);

int i2c_read_high(uint8_t subaddr, uint8_t* data, uint8_t len);

int i2c_read_back_high(uint8_t subaddr, uint8_t* data, uint8_t len);

#endif
