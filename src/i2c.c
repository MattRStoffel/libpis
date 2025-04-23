#include "i2c.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

HARDWARE_I2C hardware_i2c;

void DEV_HARDWARE_I2C_begin(char* i2c_device) {
  // device
  if ((hardware_i2c.fd = open(i2c_device, O_RDWR)) < 0) { // 打开I2C
    perror("Failed to open i2c device.\n");
    printf("Failed to open i2c device\r\n");
    exit(1);
  }
}

void DEV_HARDWARE_I2C_end(void) {
  if (close(hardware_i2c.fd) != 0) {
    perror("Failed to close i2c device.\n");
  }
}

void DEV_HARDWARE_I2C_setSlaveAddress(uint8_t addr) {
  if (ioctl(hardware_i2c.fd, I2C_SLAVE, addr) < 0) {
    printf("Failed to access bus.\n");
    exit(1);
  }
}

uint8_t DEV_HARDWARE_I2C_write(const char* buf, uint32_t len) {
  write(hardware_i2c.fd, buf, len);
  return 0;
}

uint8_t DEV_HARDWARE_I2C_read(uint8_t reg, char* buf, uint32_t len) {
  uint8_t temp[1] = {reg};
  write(hardware_i2c.fd, temp, 1);
  read(hardware_i2c.fd, buf, len);
  return 0;
}
