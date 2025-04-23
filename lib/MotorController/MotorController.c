/****
 *
 * TODO: HEADER
 *
 *****/

#include "MotorController.h"
#include "../Config/Debug.h"

/**
 * Initialize motor. According to the PCA circuit,
 * the memory to output a I2C signal to the PWM chip
 * starts at 0x40 and has a frequency range of 40-1000hz.
 * I choose 100hz so it easily translates to percentages.
 */
void MotorInit(void) {
  PCA9685_Init(0x40);
  PCA9685_SetPWMFreq(100);
}

/**
 * Turn on the motor.
 *
 * @param dir: Direction of motor, defined in header File
 * @param speed: Percent of power to run motor at.
 *
 */
void RunMotor(char motor, char dir, int speed) {
  if (speed > 100) {
    speed = 100;
  } else if (speed < 0) {
    speed = 0;
  }
  DEBUG("Motor being ran = %c\r\n", motor);
  DEBUG("Motor speed = %d\r\n", speed);

  if (motor == MOTOR_A) {
    PCA9685_SetPwmDutyCycle(PWMA, speed);
    if (dir == FORWARD) {
      DEBUG("Motor moving forward\r\n");
      PCA9685_SetLevel(IN1A, 0);
      PCA9685_SetLevel(IN2A, 1);
    } else {
      DEBUG("Motor moving backward\r\n");
      PCA9685_SetLevel(IN1A, 1);
      PCA9685_SetLevel(IN2A, 0);
    }
  } else {
    PCA9685_SetPwmDutyCycle(PWMB, speed);
    if (dir == FORWARD) {
      DEBUG("Motor moving forward\r\n");
      PCA9685_SetLevel(IN1B, 0);
      PCA9685_SetLevel(IN2B, 1);
    } else {
      DEBUG("Motor moving backward\r\n");
      PCA9685_SetLevel(IN1B, 1);
      PCA9685_SetLevel(IN2B, 0);
    }
  }
}

/**
 * Turns off motor.
 */
void StopMotor(void) {
  PCA9685_SetPwmDutyCycle(PWMA, 0);
  PCA9685_SetPwmDutyCycle(PWMB, 0);
}
