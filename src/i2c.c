#include "i2c.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct i2c_device {
  int fd;
  uint16_t addr;
};

i2c_device* i2c_open(char* dev_path, uint8_t addr) {
  struct i2c_device* dev = malloc(sizeof(i2c_device));
  if (dev == NULL) {
    return NULL;
  }

  dev->fd = open(dev_path, O_RDWR);
  dev->addr = addr;

  if (dev->fd < 0) {
    printf("i2c open failed\n");
    free(dev);
    return NULL;
  }

  if (i2c_set_target_addr(dev, addr)) {
    i2c_close(dev);
    return NULL;
  }

  return dev;
}

void i2c_close(i2c_device* dev) {
  close(dev->fd);
  free(dev);
}

int i2c_set_target_addr(i2c_device* dev, uint8_t addr) {
  if (ioctl(dev->fd, I2C_SLAVE, addr) < 0) {
    return -1;
  }

  dev->addr = addr;
  return 0;
}

ssize_t i2c_write(i2c_device* dev, const char* buf, uint32_t len) {
  write(dev->fd, buf, len);
  return 0;
}

ssize_t i2c_read(i2c_device* dev, uint8_t reg, char* buf, uint32_t len) {
  uint8_t temp[1] = {reg};
  write(dev->fd, temp, 1);
  read(dev->fd, buf, len);
  return 0;
}
