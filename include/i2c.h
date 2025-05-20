/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 92312711
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: include/i2c.h
 *
 * Description:: Functionality for I2C communication protocol.
 *
 **************************************************************/

#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>
#include <sys/types.h>

typedef struct i2c_device {
  int fd;
  uint16_t addr;
} i2c_device;

/*
 * Opens the I2C device at target address.
 * Returns NULL on faliure.
 * Returns an initialized i2c_device struct on success.
 * var dev_path: Path to pi I2C device file
 * var addr: Target I2C device address
 */
i2c_device* i2c_open(char* dev_path, uint8_t addr);

/*
 * Closes the provided i2c_device device.
 * var dev: I2C device
 */
void i2c_close(i2c_device* dev);

/*
 * Sets the target address of the provided i2c_device to the provided address.
 * Returns 0 on success.
 * var dev: I2C device
 * var addr: New address for the provided I2C device to target
 */
int i2c_set_target_addr(i2c_device* dev, uint8_t addr);

/*
 * Write len bytes from provided buffer into provided I2C device.
 * Returns number of bytes written.
 * var dev: I2C device
 * var buf: Buffer containing data to write from
 * var len: Amount of bytes to write from buffer into the I2C device
 */
ssize_t i2c_write(i2c_device* dev, const char* buf, uint32_t len);

/*
 * Write len bytes from the provided register of the I2C device to the
 * provided buffer.
 * Returns number of bytes read.
 * var dev: I2C device
 * var reg: I2C device register to read from
 * var buf: Buffer to copy data into
 * var len: Amount of bytes to read
 */
ssize_t i2c_read(i2c_device* dev, uint8_t reg, char* buf, uint32_t len);

#endif
