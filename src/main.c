#include "gpio.h"
#include "motor.h"
#include "pid.h"

#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static volatile sig_atomic_t running = 1;

void cancel_handler(int sig) {
  signal(sig, SIG_IGN);
  running = 0;
}

#define LINE_SENS_1 5
#define LINE_SENS_2 6
#define LINE_SENS_3 26

void read_sensors(bool sensors[3]) {
  sensors[0] = get_gpio(LINE_SENS_1);
  sensors[1] = get_gpio(LINE_SENS_2);
  sensors[2] = get_gpio(LINE_SENS_3);
}

float calculate_error(const bool sensors[3]) {
  const float weights[3] = {1.0, 0.0, -1.0};
  float sum = 0;
  int active_sensors = 0;

  for (int i = 0; i < 3; ++i) {
    if (sensors[i]) {
      sum += weights[i];
      active_sensors++;
    }
  }

  if (active_sensors == 0)
    return 0.0;

  return sum / active_sensors;
}

void set_motor_speed(float pid_output) {
  int max_speed = 100;
  int sharp_thresh = 70;
  int norm_thresh = 2;

  int left_speed = 0, right_speed = 0;
  char left_dir = FORWARD;
  char right_dir = FORWARD;

  // Turning right
  if (pid_output > norm_thresh) {
    left_speed = max_speed;
    if (pid_output > sharp_thresh) { // Big correction: reverse right motor
      right_dir = BACKWARD;
      right_speed = (int)(pid_output);
    } else {
      right_speed = max_speed - (int)(pid_output);
    }
  }
  // Turning left
  else if (pid_output < -norm_thresh) {
    right_speed = max_speed;
    if (pid_output < -sharp_thresh) { // Big correction: reverse left motor
      left_dir = BACKWARD;
      left_speed = (int)(-pid_output);
    } else {
      left_speed = max_speed - (int)(-pid_output);
    }
  }
  // No correction needed
  else {
    left_speed = max_speed;
    right_speed = max_speed;
  }

  RunMotor(MOTOR_A, left_dir, left_speed);
  RunMotor(MOTOR_B, right_dir, right_speed);
}

int main() {

  setup_gpio();
  init_gpio(LINE_SENS_1, GPIO_INPUT);
  init_gpio(LINE_SENS_2, GPIO_INPUT);
  init_gpio(LINE_SENS_3, GPIO_INPUT);

  MotorInit();

  signal(SIGINT, cancel_handler);
  signal(SIGTERM, cancel_handler);

  PID_Controller pid;
  pid_init(&pid, 50.0, 0.0, 290, -100.0, 100.0);

  while (running) {
    bool sensors[3];
    read_sensors(sensors);
    double known_error = calculate_error(sensors);
    float diff = pid_update(&pid, known_error);
    set_motor_speed(diff);
  }

  StopMotor();
  return 0;
}
