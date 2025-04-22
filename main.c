#include "pid.h"
#include "sim.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  // 1. Reset sim and PID
  sim_init();
  PID_Controller pid;
  pid_init(&pid, 23.0, 0.00, 25.4, -100.0, 100.0);

  // 2. Run simulation
  double total_error = 0;
  for (int i = 0; i < 19000; ++i) {
    int8_t sensors[3];
    sim_read_sensors(sensors);
    double known_error = 0;
    if (sensors[0])
      known_error = 1;
    else if (sensors[2])
      known_error = -1;
    else if (sensors[1])
      known_error = 0;

    total_error += fabs(known_error);
    float diff = pid_update(&pid, known_error);
    sim_drive_motors(diff, 0.7);
  }

  sim_deinit();
  return 0;
}
