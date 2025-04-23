#include "motor.h"
#include "pca9685.h"

void MotorInit(void) {
  PCA9685_Init(0x40);
  PCA9685_SetPWMFreq(100);
}

void RunMotor(char motor, char dir, int speed) {
  if (speed > 100) {
    speed = 100;
  } else if (speed < 0) {
    speed = 0;
  }

  if (motor == MOTOR_A) {
    PCA9685_SetPwmDutyCycle(PWMA, speed);
    if (dir == FORWARD) {
      PCA9685_SetLevel(IN1A, 0);
      PCA9685_SetLevel(IN2A, 1);
    } else {
      PCA9685_SetLevel(IN1A, 1);
      PCA9685_SetLevel(IN2A, 0);
    }
  } else {
    PCA9685_SetPwmDutyCycle(PWMB, speed);
    if (dir == FORWARD) {
      PCA9685_SetLevel(IN1B, 0);
      PCA9685_SetLevel(IN2B, 1);
    } else {
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
