#include "gpio.h"
#include "motor.h"
#include "pid.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LINE_SENS_1 5
#define LINE_SENS_2 6
#define LINE_SENS_3 26

void read_sensors(int8_t sensors[3]) {
  sensors[0] = get_gpio(LINE_SENS_1);
  sensors[0] = get_gpio(LINE_SENS_2);
  sensors[0] = get_gpio(LINE_SENS_3);
}

int main() {
  PID_Controller pid;
  pid_init(&pid, 23.0, 0.00, 25.4, -100.0, 100.0);

  setup_gpio();

  init_gpio(LINE_SENS_1, GPIO_INPUT);
  init_gpio(LINE_SENS_2, GPIO_INPUT);
  init_gpio(LINE_SENS_3, GPIO_INPUT);

  MotorInit();

  // 2. Run simulation
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

    float diff = pid_update(&pid, known_error);

    if (diff < 0) {
      RunMotor(MOTOR_A, FORWARD, 100 + diff);
      RunMotor(MOTOR_B, FORWARD, 0 - diff);
    } else {
      RunMotor(MOTOR_A, FORWARD, 0 + diff);
      RunMotor(MOTOR_B, FORWARD, 100 - diff);
    }
  }

  StopMotor();
  return 0;
}
