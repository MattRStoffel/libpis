/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 92312711
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: src/motor.c
 *
 * Description:
 *   PCA9685-based motor control implementation for the line-following robot.
 *   - Initialize the PCA9685 PWM controller and set operating frequency
 *   - Run individual motors with specified speed and direction
 *   - Stop both motors by setting PWM duty cycles to zero
 **************************************************************/

#include "motor.h"
#include "pca9685.h"

#include <stdio.h>

pca9685_device* dev = NULL;

int motor_init(void) {
  dev = pca9685_init(0x40);
  if (!dev) {
    return -1;
  }

  pca9685_set_pwm_freq(dev, 100);

  return 0;
}

void run_motor(char motor, char dir, int speed) {
  if (speed > 100) {
    speed = 100;
  } else if (speed < 0) {
    speed = 0;
  }

  if (motor == MOTOR_A) {
    pca9685_set_pwm_duty_cycle(dev, PWMA, speed);
    if (dir == FORWARD) {
      pca9685_set_level(dev, IN1A, 0);
      pca9685_set_level(dev, IN2A, 1);
    } else {
      pca9685_set_level(dev, IN1A, 1);
      pca9685_set_level(dev, IN2A, 0);
    }
  } else {
    pca9685_set_pwm_duty_cycle(dev, PWMB, speed);
    if (dir == FORWARD) {
      pca9685_set_level(dev, IN1B, 0);
      pca9685_set_level(dev, IN2B, 1);
    } else {
      pca9685_set_level(dev, IN1B, 1);
      pca9685_set_level(dev, IN2B, 0);
    }
  }
}

void stop_motor(void) {
  pca9685_set_pwm_duty_cycle(dev, PWMA, 0);
  pca9685_set_pwm_duty_cycle(dev, PWMB, 0);
}
