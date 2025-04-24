#include "gpio.h"
#include "motor.h"
#include "pid.h"

#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static volatile sig_atomic_t running = 1;

// PID
#define KP 3.0
#define KI 0.001
#define KD 20.0

// Error
#define ERR_WEIGHT_STEP 2

// Motors
#define MAX_SPEED 60
#define LEFT_MOTORS MOTOR_A
#define RIGHT_MOTORS MOTOR_B

// Sensors
#define LINE_SENS_1 5
#define LINE_SENS_2 6
#define LINE_SENS_3 26
#define NUM_SENS 3

void cancel_handler(int sig) {
  signal(sig, SIG_IGN);
  running = 0;
}

void read_sensors(bool sensors[NUM_SENS]) {
  sensors[0] = get_gpio(LINE_SENS_1);
  sensors[1] = get_gpio(LINE_SENS_2);
  sensors[2] = get_gpio(LINE_SENS_3);
}

float calculate_error(const bool sensors[NUM_SENS]) {
  const float weights[NUM_SENS] = {ERR_WEIGHT_STEP, 0.0, -ERR_WEIGHT_STEP};
  float sum = 0;
  int active_sensors = 0;

  for (int i = 0; i < NUM_SENS; ++i) {
    if (sensors[i]) {
      sum += weights[i];
      active_sensors++;
    }
  }

  if (active_sensors == 0)
    return 0.0;

  return sum / active_sensors;
}

void steer_car(float pid_output) {
  int left_speed = MAX_SPEED;
  int right_speed = MAX_SPEED;

  if (pid_output > 0) {
    // Turning right: reduce right motor speed
    right_speed = MAX_SPEED - pid_output;
  } else if (pid_output < 0) {
    // Turning left: reduce left motor speed
    left_speed = MAX_SPEED + pid_output; // since pid_output is negative
  }

  RunMotor(LEFT_MOTORS, FORWARD, left_speed);
  RunMotor(RIGHT_MOTORS, FORWARD, right_speed);
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
  pid_init(&pid, KP, KI, KD, MAX_SPEED);

  while (running) {
    bool sensors[NUM_SENS];
    read_sensors(sensors);
    double known_error = calculate_error(sensors);
    float pid_out = pid_update(&pid, known_error * MAX_SPEED);
    steer_car(pid_out);
  }

  StopMotor();
  return 0;
}
