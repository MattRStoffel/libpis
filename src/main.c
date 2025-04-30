#include "gpio.h"
#include "motor.h"
#include "pid.h"
#include "tcsoup.h"

#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static volatile sig_atomic_t running = 1;
static volatile color_t color;

// PID
#define KP 10.0
#define KI 0.0
#define KD 30.0

// Error
#define ERR_WEIGHT_STEP 2

// Motors
#define MAX_SPEED 80
#define LEFT_MOTORS MOTOR_A
#define RIGHT_MOTORS MOTOR_B

// Sensors
#define LINE_SENS_1 20
#define LINE_SENS_2 21
#define LINE_SENS_3 5
#define LINE_SENS_4 26
#define LINE_SENS_5 6
#define NUM_SENS 5

#define COLOR_SENS_LED 16
#define RED_SENSITIVITY 80

void* read_color(void* arg) {
  struct tcsoup* tc = (struct tcsoup*)arg;
  while (running) {
    tcsoup_read(tc, (color_t*)&color);
  }
  return NULL;
}

void cancel_handler(int sig) {
  signal(sig, SIG_IGN);
  running = 0;
}

void read_sensors(bool sensors[NUM_SENS]) {
  sensors[0] = get_gpio(LINE_SENS_1);
  sensors[1] = get_gpio(LINE_SENS_2);
  sensors[2] = get_gpio(LINE_SENS_3);
  sensors[3] = get_gpio(LINE_SENS_4);
  sensors[4] = get_gpio(LINE_SENS_5);
  // print the sensor values
  printf("\rSensors: %d %d %d %d %d", sensors[0], sensors[1], sensors[2], sensors[3], sensors[4]);
}

float calculate_error(const bool sensors[NUM_SENS]) {
  const float weights[NUM_SENS] = {ERR_WEIGHT_STEP * 2, ERR_WEIGHT_STEP, 0.0, -ERR_WEIGHT_STEP,
                                   -ERR_WEIGHT_STEP * 2};
  float sum = 0;
  int active_sensors = 0;

  for (int i = 0; i < NUM_SENS; ++i) {
    if (sensors[i]) {
      sum += weights[i];
      active_sensors++;
    }
  }

  if (active_sensors == 0)
    return 999;

  return sum / active_sensors;
}

void steer_car(float pid_output) {
  int left_speed = MAX_SPEED;
  int right_speed = MAX_SPEED;
  char left_dir = FORWARD;
  char right_dir = FORWARD;
  int adjusted_speed = pid_output * MAX_SPEED;

  if (pid_output > 0) {
    right_speed -= adjusted_speed;
    if (pid_output > 10) {
      right_speed = MAX_SPEED;
      right_dir = BACKWARD;
    }
  } else if (pid_output < 0) {
    left_speed += adjusted_speed;
    if (pid_output < -10) {
      left_speed = MAX_SPEED;
      left_dir = BACKWARD;
    }
  }

  run_motor(LEFT_MOTORS, left_dir, left_speed);
  run_motor(RIGHT_MOTORS, right_dir, right_speed);
}

int main() {
  setup_gpio();

  init_gpio(LINE_SENS_1, GPIO_INPUT);
  init_gpio(LINE_SENS_2, GPIO_INPUT);
  init_gpio(LINE_SENS_3, GPIO_INPUT);
  init_gpio(LINE_SENS_4, GPIO_INPUT);
  init_gpio(LINE_SENS_5, GPIO_INPUT);
  init_gpio(COLOR_SENS_LED, GPIO_OUTPUT);

  if (motor_init()) {
    fprintf(stderr, "error: failed to init motors\n");
    return EXIT_FAILURE;
  }

  struct tcsoup* tc = tcsoup_init();
  if (!tc) {
    fprintf(stderr, "error: failed to init color sensor\n");
    return EXIT_FAILURE;
  }

  if (tcsoup_set_integration_time(tc, 100) != 0) {
    tcsoup_deinit(tc);
    fprintf(stderr, "error: failed to set integration time for color sensor\n");
    return -1;
  }

  if (tcsoup_set_gain(tc, 1) != 0) {
    tcsoup_deinit(tc);
    fprintf(stderr, "error: failed to set gain for color sensor\n");
    return -1;
  }

  set_gpio(COLOR_SENS_LED, 1);

  signal(SIGINT, cancel_handler);
  signal(SIGTERM, cancel_handler);

  PID_Controller pid;
  pid_init(&pid, KP, KI, KD, MAX_SPEED);

  float known_error;
  float error;
  float pid_out;
  bool sensors[NUM_SENS];

  pthread_t color_thread;
  pthread_create(&color_thread, NULL, read_color, tc);

  while (running) {
    read_sensors(sensors);
    error = calculate_error(sensors);
    if (error != 999) {
      known_error = error;
    }
    pid_out = pid_update(&pid, known_error);
    steer_car(pid_out);
    printf("R::%d, G::%d, B::%d", color.r, color.g, color.b);
    if (color.r - RED_SENSITIVITY > (color.g + color.b) / 2) {
      running = false;
    }
  }

  pthread_join(color_thread, NULL);
  set_gpio(COLOR_SENS_LED, 0);

  tcsoup_deinit(tc);
  stop_motor();
  deinit_gpio();

  return EXIT_SUCCESS;
}
