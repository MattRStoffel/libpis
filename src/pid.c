#include "pid.h"
#include <math.h>

float clamp(float value, float min_val, float max_val) {
  return fmax(min_val, fmin(max_val, value));
}

void pid_init(PID_Controller *pid, float kp, float ki, float kd,
              float min_output, float max_output) {
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;
  pid->prev_error = 0;
  pid->integral = 0;
  pid->min_output = min_output;
  pid->max_output = max_output;
}

float pid_update(PID_Controller *pid, float error) {
  float max_d = 50.0f;
  float max_i = 50.0f;

  pid->integral += error;
  pid->integral = clamp(pid->integral, -max_i, max_i);
  float derivative = error - pid->prev_error;
  derivative = clamp(derivative, -1 * max_d, max_d);
  pid->prev_error = error;

  float output =
      pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

  return clamp(output, pid->min_output, pid->max_output);
}
