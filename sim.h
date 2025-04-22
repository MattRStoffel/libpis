#ifndef SIM_H
#define SIM_H

#define MAX_TRACK_POINTS 10000

typedef struct {
  double x;
  double y;
} Point;

typedef struct {
  Point position;
  double orientation;
} Car;

void generate_track(Point[]);
double get_distance_from_line(Car, Point[]);
void update_car_position(Car *, double, double, double);
void add_point(Point[], double, double);

#endif
