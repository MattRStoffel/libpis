#include "pid.h"
#include "sim.h"
#include <math.h>
#include <stdio.h>

int main() {
  FILE *track_file = fopen("track.csv", "w");
  FILE *path_file = fopen("car_path.csv", "w");

  Car car;
  Point track[MAX_TRACK_POINTS];
  generate_track(track);

  // save the track
  for (int i = 1; i < MAX_TRACK_POINTS && (track[i].x != 0 && track[i].y != 0);
       ++i) {
    fprintf(track_file, "%.5f,%.5f\n", track[i].x, track[i].y);
  }
  fclose(track_file);

  // Put car on the track
  car.position.x = track[0].x;
  car.position.y = track[0].y;
  car.orientation = atan2(track[1].y - track[0].y, track[1].x - track[0].x);

  double time_step = 0.01;

  PID_Controller pid;
  pid_init(&pid, 1, 0.0, 50, -100.0, 100.0);

  for (int i = 0; i < 9000; ++i) {
    double distance_error = get_distance_from_line(car, track);

    float diff = pid_update(&pid, distance_error * -1);

    double base_speed = 1.0;
    double left_track_speed = base_speed - diff;
    double right_track_speed = base_speed + diff;

    update_car_position(&car, left_track_speed, right_track_speed, time_step);
    fprintf(path_file, "%.5f,%.5f\n", car.position.x, car.position.y);
  }
  fclose(path_file);

  return 0;
}
