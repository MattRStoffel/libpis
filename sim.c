#include "sim.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

int track_point_count = 0;
Car car;
Point track[MAX_TRACK_POINTS];

//-----------------------------------------------------------------------------
// Track
//-----------------------------------------------------------------------------

void sim_add_point(Point track[], double x, double y) {
  if (track_point_count < MAX_TRACK_POINTS) {
    track[track_point_count].x = x;
    track[track_point_count].y = y;
    track_point_count++;
  }
}

void sim_generate_track(Point track[]) {
  track_point_count = 0;

  // 1. Extended S-Curve
  int s_curve_points = 60;
  for (int i = 0; i < s_curve_points; ++i) {
    double x = i * 0.2;
    double y = sin(i * 0.2) * 1.0;
    sim_add_point(track, x, y);
  }

  double x_last = track[track_point_count - 1].x;
  double y_last = track[track_point_count - 1].y;

  // 2. Wide arc to the right (gentle turn)
  double radius = 5.0;
  int arc_points = 40;
  for (int i = 0; i <= arc_points; ++i) {
    double angle = M_PI * i / arc_points / 2; // Quarter circle
    double x = x_last + radius * sin(angle);
    double y = y_last + radius * (1 - cos(angle));
    sim_add_point(track, x, y);
  }

  x_last = track[track_point_count - 1].x;
  y_last = track[track_point_count - 1].y;

  // 3. Slalom
  int slalom_points = 30;
  for (int i = 0; i < slalom_points; ++i) {
    double x = x_last + i * 0.3;
    double y = y_last + sin(i * 0.6) * 0.6;
    sim_add_point(track, x, y);
  }

  x_last = track[track_point_count - 1].x;
  y_last = track[track_point_count - 1].y;

  // 4. Series of right-angle turns (zigzag corners)
  int corner_sets = 3;
  double step = 4.0;
  for (int i = 0; i < corner_sets; ++i) {
    // Move right
    sim_add_point(track, x_last + step, y_last);
    x_last += step;

    // Move up
    sim_add_point(track, x_last, y_last + step);
    y_last += step;

    // Move left
    sim_add_point(track, x_last - step, y_last);
    x_last -= step;

    // Move up again
    sim_add_point(track, x_last, y_last + step);
    y_last += step;
  }

  // 5. Final straightaway to test speed
  int final_straight = 70;
  for (int i = 1; i <= final_straight; ++i) {
    sim_add_point(track, x_last + i * 0.3, y_last);
  }
}

//-----------------------------------------------------------------------------
// Track / Car
//-----------------------------------------------------------------------------

double sim_get_distance_from_line(double pos_x, double pos_y, Point track[]) {
  if (track_point_count < 2) {
    return 0.0;
  }

  double min_distance_sq = -1;
  double signed_distance = 0.0;

  // Loop through all lines to find closest point
  for (int i = 0; i < track_point_count - 1; ++i) {
    Point p1 = track[i];
    Point p2 = track[i + 1];

    double segment_dx = p2.x - p1.x;
    double segment_dy = p2.y - p1.y;

    double car_dx = pos_x - p1.x;
    double car_dy = pos_y - p1.y;

    double dot_product = car_dx * segment_dx + car_dy * segment_dy;
    double segment_length_sq =
        segment_dx * segment_dx + segment_dy * segment_dy;

    double t = 0.0;
    if (segment_length_sq > 0) {
      t = dot_product / segment_length_sq;
    }

    if (t < 0.0)
      t = 0.0;
    if (t > 1.0)
      t = 1.0;

    Point closest_point;
    closest_point.x = p1.x + t * segment_dx;
    closest_point.y = p1.y + t * segment_dy;

    double distance_sq =
        pow(pos_x - closest_point.x, 2) + pow(pos_y - closest_point.y, 2);

    double cross_product =
        (p2.x - p1.x) * (pos_y - p1.y) - (p2.y - p1.y) * (pos_x - p1.x);

    if (min_distance_sq < 0 || distance_sq < min_distance_sq) {
      min_distance_sq = distance_sq;
      signed_distance =
          (cross_product >= 0) ? sqrt(distance_sq) : -sqrt(distance_sq);
    }
  }

  return signed_distance;
}

//-----------------------------------------------------------------------------
// Car
//-----------------------------------------------------------------------------

void sim_update_car_position(Car *car, double left, double right, double dt) {
  double track_width = 1;

  // Simulated imperfections
  // Randomness between -5% and +5%
  double motor_variation_left =
      0.95 + ((rand() % 1001) / 10000.0); // [0.95, 1.05]
  double motor_variation_right = 0.95 + ((rand() % 1001) / 10000.0);

  // Apply variation
  left *= motor_variation_left;
  right *= motor_variation_right;

  // Simulate wheel slippage (10% chance of 20% velocity loss)
  if (rand() % 10 == 0) {
    left *= 0.8;
    right *= 0.8;
  }

  double linear_velocity = (left + right) / 2.0;

  double angular_velocity = (right - left) / track_width;

  car->orientation += angular_velocity * dt;

  double dx = linear_velocity * cos(car->orientation) * dt;
  double dy = linear_velocity * sin(car->orientation) * dt;

  car->position.x += dx;
  car->position.y += dy;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void sim_init() {
  FILE *track_file = fopen("track.csv", "w");
  FILE *path_file = fopen("car_path.csv", "w");

  sim_generate_track(track);

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
  car.path_file = path_file;
}

void sim_deinit() { fclose(car.path_file); }

void sim_read_sensors(int8_t sensors[3]) {
  float sensor_offset = 1.0f; // Lateral distance from car center
  float line_half_width = 0.5f;

  // Get perpendicular vector to orientation (left-right axis of car)
  float dx = -sin(car.orientation);
  float dy = cos(car.orientation);

  // Calculate sensor positions
  float center_x = car.position.x;
  float center_y = car.position.y;

  float left_x = center_x + sensor_offset * dx;
  float left_y = center_y + sensor_offset * dy;

  float right_x = center_x - sensor_offset * dx;
  float right_y = center_y - sensor_offset * dy;

  // Get distances to the track
  double dist_left = sim_get_distance_from_line(left_x, left_y, track);
  double dist_center = sim_get_distance_from_line(center_x, center_y, track);
  double dist_right = sim_get_distance_from_line(right_x, right_y, track);

  // Determine if each sensor is on the line
  sensors[0] = (dist_left > -line_half_width && dist_left < line_half_width);
  sensors[1] =
      (dist_center > -line_half_width && dist_center < line_half_width);
  sensors[2] = (dist_right > -line_half_width && dist_right < line_half_width);
}
void sim_drive_motors(double diff, double base_speed) {
  double time_step = 0.01;
  double left_track_speed = base_speed - diff;
  double right_track_speed = base_speed + diff;

  sim_update_car_position(&car, left_track_speed, right_track_speed, time_step);
  fprintf(car.path_file, "%.5f,%.5f\n", car.position.x, car.position.y);
}
