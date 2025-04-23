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

int main() {
  PID_Controller pid;
  pid_init(&pid, 23.0, 0.02, 25.4, -100.0, 100.0);

  setup_gpio();

  init_gpio(LINE_SENS_1, GPIO_INPUT);
  init_gpio(LINE_SENS_2, GPIO_INPUT);
  init_gpio(LINE_SENS_3, GPIO_INPUT);

  MotorInit();

  signal(SIGINT, cancel_handler);
  signal(SIGTERM, cancel_handler);

  // 2. Run simulation
  while (running) {
    bool sensors[3];

    read_sensors(sensors);
    double known_error = 0;

    if (sensors[0]) {
      known_error = 1;
    } else if (sensors[2]) {
      known_error = -1;
    } else if (sensors[1]) {
      known_error = 0;
    }

    float diff = pid_update(&pid, known_error);

    if (diff < 0.5) {
      RunMotor(MOTOR_A, FORWARD, 100 * diff);
      RunMotor(MOTOR_B, FORWARD, 200 * diff);
    } else {
      RunMotor(MOTOR_A, FORWARD, 200 * diff);
      RunMotor(MOTOR_B, FORWARD, 100 * diff);
    }
  }

  StopMotor();
  return 0;
}
