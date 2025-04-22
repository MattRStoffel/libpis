#ifndef SIM_H
#define SIM_H
#include <stdio.h>
#define MAX_TRACK_POINTS 10000

typedef struct {
  double x;
  double y;
} Point;

typedef struct {
  Point position;
  double orientation;
  FILE *path_file;
  float variation;
} Car;

void sim_init();
void sim_deinit();
void sim_read_sensors(int8_t[3]);
void sim_drive_motors(double, double);
#endif
