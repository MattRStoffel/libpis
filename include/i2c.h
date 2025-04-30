#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <sys/types.h>

typedef struct i2c_device i2c_device;

i2c_device* i2c_open(char* dev_path, uint8_t addr);
void i2c_close(i2c_device* dev);

int i2c_set_target_addr(i2c_device* dev, uint8_t addr);

ssize_t i2c_write(i2c_device* dev, const char* buf, uint32_t len);
ssize_t i2c_read(i2c_device* dev, uint8_t reg, char* buf, uint32_t len);

#endif
