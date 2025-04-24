#ifndef PID_H
#define PID_H

typedef struct {
  float kp, ki, kd;
  float prev_error;
  float integral;
  float clamp_val;
} PID_Controller;

void pid_init(PID_Controller*, float, float, float, float);
float pid_update(PID_Controller*, float);
float clamp(float, float, float);

#endif
