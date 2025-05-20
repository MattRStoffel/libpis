/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 92312711
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: src/pca9685.c
 *
 * Description:
 *   Low-level driver for PCA9685 PWM over I2C.
 *   - Open and configure PCA9685 via Linux I2C-dev.
 *   - Send and receive MODE1, PRESCALE, and LED register bytes.
 *   - Enable auto-increment and set PWM frequency with correction.
 *   - Set PWM on/off counts or full on/off per channel.
 **************************************************************/

#include "pca9685.h"
#include "i2c.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct pca9685_device {
  i2c_device* i2c;
};

int pca9685_write_byte(pca9685_device* dev, uint8_t reg, uint8_t value) {
  char wbuf[2] = {reg, value};
  i2c_write(dev->i2c, wbuf, 2);
  return 0;
}

int pca9685_read_byte(pca9685_device* dev, uint8_t reg) {
  char rbuf[2] = {0};
  i2c_read(dev->i2c, reg, rbuf, 1);
  return rbuf[0];
}

static void pca9685_set_pwm(pca9685_device* dev, uint8_t channel, uint16_t on, uint16_t off) {
  pca9685_write_byte(dev, LED0_ON_L + 4 * channel, on & 0xFF);
  pca9685_write_byte(dev, LED0_ON_H + 4 * channel, on >> 8);
  pca9685_write_byte(dev, LED0_OFF_L + 4 * channel, off & 0xFF);
  pca9685_write_byte(dev, LED0_OFF_H + 4 * channel, off >> 8);
}

pca9685_device* pca9685_init(uint8_t addr) {
  pca9685_device* dev = malloc(sizeof(pca9685_device));
  if (dev == NULL) {
    return NULL;
  }

  i2c_device* i2c = i2c_open("/dev/i2c-1", addr);
  if (!i2c) {
    free(dev);
    return NULL;
  }

  char wbuf[2] = {MODE1, 0x00};
  i2c_write(i2c, wbuf, 2);

  dev->i2c = i2c;

  return dev;
}

void pca9685_set_pwm_freq(pca9685_device* dev, uint16_t freq) {
  freq *= 0.9; // Correct for overshoot in the frequency setting (see issue #11).
  double prescaleval = 25000000.0;
  prescaleval /= 4096.0;
  prescaleval /= freq;
  prescaleval -= 1;

  uint8_t prescale = floor(prescaleval + 0.5);

  uint8_t oldmode = pca9685_read_byte(dev, MODE1);
  uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep

  // Go to sleep
  pca9685_write_byte(dev, MODE1, newmode);
  // Set the pre-scaler
  pca9685_write_byte(dev, PRESCALE, prescale);
  pca9685_write_byte(dev, MODE1, oldmode);
  usleep(5000);
  // This sets the `MODE1` register to turn on auto increment.
  pca9685_write_byte(dev, MODE1, oldmode | 0x80);
}

void pca9685_set_pwm_duty_cycle(pca9685_device* dev, uint8_t channel, uint16_t pulse) {
  pca9685_set_pwm(dev, channel, 0, pulse * (4096 / 100) - 1);
}

void pca9685_set_level(pca9685_device* dev, uint16_t channel, uint16_t value) {
  if (value == 1)
    pca9685_set_pwm(dev, channel, 0, 4095);
  else
    pca9685_set_pwm(dev, channel, 0, 0);
}
