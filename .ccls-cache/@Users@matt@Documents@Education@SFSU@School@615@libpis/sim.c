#include "sim.h"
#include <math.h>

int track_point_count = 0;

void add_point(Point track[], double x, double y) {
  if (track_point_count < MAX_TRACK_POINTS) {
    track[track_point_count].x = x;
    track[track_point_count].y = y;
    track_point_count++;
  }
}

void generate_track(Point track[]) {
  track_point_count = 0;

  // 1. Extended S-Curve
  int s_curve_points = 60;
  for (int i = 0; i < s_curve_points; ++i) {
    double x = i * 0.2;
    double y = sin(i * 0.2) * 1.0;
    add_point(track, x, y);
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
    add_point(track, x, y);
  }

  x_last = track[track_point_count - 1].x;
  y_last = track[track_point_count - 1].y;

  // 3. Slalom
  int slalom_points = 30;
  for (int i = 0; i < slalom_points; ++i) {
    double x = x_last + i * 0.3;
    double y = y_last + sin(i * 0.6) * 0.6;
    add_point(track, x, y);
  }

  x_last = track[track_point_count - 1].x;
  y_last = track[track_point_count - 1].y;

  // 4. Series of right-angle turns (zigzag corners)
  int corner_sets = 5;
  double step = 1.0;
  for (int i = 0; i < corner_sets; ++i) {
    // Move right
    add_point(track, x_last + step, y_last);
    x_last += step;

    // Move up
    add_point(track, x_last, y_last + step);
    y_last += step;

    // Move left
    add_point(track, x_last - step, y_last);
    x_last -= step;

    // Move up again
    add_point(track, x_last, y_last + step);
    y_last += step;
  }

  // 5. Final straightaway to test speed
  int final_straight = 20;
  for (int i = 1; i <= final_straight; ++i) {
    add_point(track, x_last + i * 0.3, y_last);
  }
}

double get_distance_from_line(Car car, Point track[]) {
  if (track_point_count < 2) {
    return 0.0;
  }

  double min_distance_sq = -1;
  double signed_distance = 0.0;

  for (int i = 0; i < track_point_count - 1; ++i) {
    Point p1 = track[i];
    Point p2 = track[i + 1];

    double segment_dx = p2.x - p1.x;
    double segment_dy = p2.y - p1.y;

    double car_dx = car.position.x - p1.x;
    double car_dy = car.position.y - p1.y;

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

    double distance_sq = pow(car.position.x - closest_point.x, 2) +
                         pow(car.position.y - closest_point.y, 2);

    double cross_product = (p2.x - p1.x) * (car.position.y - p1.y) -
                           (p2.y - p1.y) * (car.position.x - p1.x);

    if (min_distance_sq < 0 || distance_sq < min_distance_sq) {
      min_distance_sq = distance_sq;
      signed_distance =
          (cross_product >= 0) ? sqrt(distance_sq) : -sqrt(distance_sq);
    }
  }

  return signed_distance;
}

void update_car_position(Car *car, double left_track_speed,
                         double right_track_speed, double dt) {
  double track_width = 0.5;
  double linear_velocity = (left_track_speed + right_track_speed) / 2.0;

  double angular_velocity =
      (right_track_speed - left_track_speed) / track_width;

  car->orientation += angular_velocity * dt;

  double dx = linear_velocity * cos(car->orientation) * dt;
  double dy = linear_velocity * sin(car->orientation) * dt;

  car->position.x += dx;
  car->position.y += dy;
}
