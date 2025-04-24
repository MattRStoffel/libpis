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
