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
static volatile double distance_front;
static volatile double distance_side;
const struct timespec triger_sleep_time = {0, 10000};

// PID
#define KP 20.0
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
#define ECHO_FRONT 24
#define TRIG_FRONT 23
#define ECHO_SIDE 8
#define TRIG_SIDE 25
#define NUM_SENS 5

#define COLOR_SENS_LED 16
#define RED_SENSITIVITY 80

#define TRIGGER_TIME 10000

// t_max = 2 * d_max / v_sound
#define MAX_DISTANCE 300
#define SOUND_SPEED 340

// found by testing
#define CHIPBUFFER 99999999

#define TIMEOUT 10000000 / SOUND_SPEED* MAX_DISTANCE * 2 + CHIPBUFFER

void* read_color(void* arg) {
  struct tcsoup* tc = (struct tcsoup*)arg;
  while (running) {
    tcsoup_read(tc, (color_t*)&color);
  }
  return NULL;
}

double getDistance(int trig, int echo) {
  struct timespec now, start, timeout, triggertime;

  int started = 0;

  triggertime.tv_sec = 0;
  triggertime.tv_nsec = TRIGGER_TIME;
  turnOnGPIO(trig);
  if (nanosleep(&triggertime, NULL) != 0) {
    return -1;
  }
  turnOffGPIO(trig);

  if (clock_gettime(CLOCK_MONOTONIC, &timeout) != 0) {
    return -1;
  }
  timeout.tv_nsec += TIMEOUT;

  volatile int signal = get_gpio(echo);
  while (clock_gettime(CLOCK_MONOTONIC, &now) == 0) {

    if (!started && signal == 1) {
      if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        return -1;
      }
      started = 1;
    } else if (started && signal == 0) {
      if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        return -1;
      }
      break;
    }

    if (now.tv_sec > timeout.tv_sec) {
      return -2;
    }
  }

  // reduce mults : 340 / 2 / 10000000 = 17 / 1000000
  return (double)(now.tv_nsec - start.tv_nsec) * 17.0 / 1000000.0;
}

void* sonic_front(void* arg) {
  (void)arg;
  int trig = TRIG_FRONT;
  int echo = ECHO_FRONT;
  while (running) {
    double distance = getDistance(trig, echo);
    printf("			%.2f cm", distance);
    distance_front = distance;
  }
  return NULL;
}

void* sonic_side(void* arg) {
  (void)arg;
  int trig = TRIG_SIDE;
  int echo = ECHO_SIDE;
  while (running) {
    double distance = getDistance(trig, echo);
    printf("%.2f cm", distance);
    distance_side = distance;
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

// avoid obstcle, we will calculate delta theta to find our distance from the object relative to
// the speed of the car if delta thea is greater than the threshhold we are too close and need to
// turn The ratio of our current speed and dT will give us the distance to the object we just need
// to maintain that direction using that ratio

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

  pthread_t sonic_thread_front;
  pthread_create(&sonic_thread_front, NULL, sonic_front, NULL);

  pthread_t sonic_thread_side;
  pthread_create(&sonic_thread_side, NULL, sonic_side, NULL);

  while (running) {
    read_sensors(sensors);
    error = calculate_error(sensors);
    if (error != 999) {
      known_error = error;
    }

    pid_out = pid_update(&pid, known_error);
    steer_car(pid_out);
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
