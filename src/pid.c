/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 92312711
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: src/pid.c
 *
 * Description:
 *   Proportional-Integral-Derivative (PID) controller module.
 *   - Clamp a value within specified bounds.
 *   - Initialize PID gains and reset internal state.
 *   - Compute output from error, integral, and derivative terms.
 *   - Apply output clamping to limit controller commands.
 **************************************************************/

#include "pid.h"
#include <math.h>

float clamp(float value, float min_val, float max_val) {
  return fmax(min_val, fmin(max_val, value));
}

void pid_init(PID_Controller* pid, float kp, float ki, float kd, float clamp_val) {
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->prev_error = 0;
  pid->integral = 0;
  pid->clamp_val = clamp_val;
}

float pid_update(PID_Controller* pid, float error) {

  pid->integral += error;
  float derivative = error - pid->prev_error;
  pid->prev_error = error;

  float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

  return clamp(output, -pid->clamp_val, pid->clamp_val);
}
