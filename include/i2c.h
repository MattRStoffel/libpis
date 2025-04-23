#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

typedef struct I2CStruct {
  int fd;
  uint16_t addr;
} HARDWARE_I2C;

void DEV_HARDWARE_I2C_begin(char* i2c_device);
void DEV_HARDWARE_I2C_end(void);
void DEV_HARDWARE_I2C_setSlaveAddress(uint8_t addr);
uint8_t DEV_HARDWARE_I2C_write(const char* buf, uint32_t len);
uint8_t DEV_HARDWARE_I2C_read(uint8_t reg, char* buf, uint32_t len);
#endif
