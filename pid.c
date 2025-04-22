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
  pid->integral += error;
  float derivative = error - pid->prev_error;
  float max_d = 10;
  if (fabs(derivative) > max_d)
    derivative = max_d * (derivative / fabs(derivative));
  pid->prev_error = error;

  float output =
      pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
  return clamp(output, pid->min_output, pid->max_output);
}
