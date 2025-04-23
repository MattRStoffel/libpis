#include "DirectRegister.h"
#include "lib/MotorController/MotorController.h"
#include "pid.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_SENS_1 5
#define LINE_SENS_2 6
#define LINE_SENS_3 26

void read_sensors(int8_t sensors[3]) {
  sensors[0] = getGPIO(LINE_SENS_1);
  sensors[0] = getGPIO(LINE_SENS_2);
  sensors[0] = getGPIO(LINE_SENS_3);
}

int main() {
  // 1. PID and GPIO setup
  PID_Controller pid;
  pid_init(&pid, 23.0, 0.00, 25.4, -100.0, 100.0);
  setup_io();
  initGPIO(LINE_SENS_1, GPIO_INPUT);
  initGPIO(LINE_SENS_2, GPIO_INPUT);
  initGPIO(LINE_SENS_3, GPIO_INPUT);
  MotorInit();

  // 2. Run car
  double total_error = 0;
  for (int i = 0; i < 19000; ++i) {
    int8_t sensors[3];
    read_sensors(sensors);
    double known_error = 0;
    if (sensors[0])
      known_error = 1;
    else if (sensors[2])
      known_error = -1;
    else if (sensors[1])
      known_error = 0;

    total_error += fabs(known_error);
    float diff = pid_update(&pid, known_error);
  }

  return 0;
}
