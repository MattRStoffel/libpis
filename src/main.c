/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 92312711
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: src/main.c
 *
 * Description:
 *   Main control program for the line-following robot.
 *   - Initializes GPIO and I2C interfaces for sensor and actuator communication
 *   - Spawns threads to read color (TCS34725) and ultrasonic distance sensors
 *   - Implements PID-based line following and obstacle avoidance routines
 *   - Manages signal handling for clean shutdown on user interrupt
 **************************************************************/

#include "gpio.h"
#include "motor.h"
#include "pid.h"
#include "tcsoup.h"

#include <math.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
#define MAX_SPEED 50
#define LEFT_MOTORS MOTOR_A
#define RIGHT_MOTORS MOTOR_B

// Sensors
#define LINE_SENS_1 20
#define LINE_SENS_2 21
#define LINE_SENS_3 5
#define LINE_SENS_4 26
#define LINE_SENS_5 6
#define ECHO_FRONT 23
#define TRIG_FRONT 24
#define ECHO_SIDE 14
#define TRIG_SIDE 15
#define NUM_SENS 5
#define BUTTON_PIN 27
#define BUTTON_PWR_PIN 22

#define COLOR_SENS_LED 16
#define RED_SENSITIVITY 100
#define GREEN_SENSITIVITY 20

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

const struct timespec triggertime = {0, TRIGGER_TIME};

double getDistance(int trig, int echo) {
  struct timespec start;
  struct timespec end;

  init_gpio(trig, GPIO_OUTPUT);
  init_gpio(echo, GPIO_INPUT);

  set_gpio(trig, 1);
  if (nanosleep(&triggertime, NULL) != 0) {
    return -1;
  }
  set_gpio(trig, 0);

  while (get_gpio(echo) != 1) {
    if (get_gpio(echo) < 0) {
      return -1;
    }
    if (nanosleep(&triggertime, NULL) != 0) {
      return -1;
    }
  }
  if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
    return -1;
  }

  while (get_gpio(echo) != 0) {
    if (get_gpio(echo) < 0) {
      return -1;
    }
    if (nanosleep(&triggertime, NULL) != 0) {
      return -1;
    }
  }
  if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
    return -1;
  }

  // reduce mults : 340 / 2 / 10000000 = 17 / 1000000
  return (double)(end.tv_nsec - start.tv_nsec) * 17.0 / 1000000.0;
}

void* sonic_front(void* arg) {
  (void)arg;
  int trig = TRIG_FRONT;
  int echo = ECHO_FRONT;
  while (running) {
    double distance = getDistance(trig, echo);
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
    distance_side = distance;
  }
  return NULL;
}

jmp_buf main_loop_jump_buf;

void cancel_handler(int sig) {
  signal(sig, SIG_IGN);
  running = 0;
  longjmp(main_loop_jump_buf, 1);
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

#define WINDOW_SIZE 5

double turn_90_degrees() {
  double readings[WINDOW_SIZE] = {9999, 9999, 9999, 9999, 9999};
  int index = 0;
  double min = 9999;
  double prev_avg = 9999;

  while (running) {
    steer_car(MAX_SPEED);

    readings[index] = distance_side;
    index = (index + 1) % WINDOW_SIZE;

    // Compute rolling average
    double sum = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
      sum += readings[i];
    }
    double avg = sum / WINDOW_SIZE;

    double derivative = avg - prev_avg;

    if (min < 9999 && derivative > 1.0) { // threshold > 0 to avoid false triggers
      return avg;
    }

    if (avg < 40 && avg > 0 && avg < min) {
      min = avg;
    }

    prev_avg = avg;
  }

  return -1;
}

void obstacle_avoidance() {
  const double preferred_distance = 20;
  if (distance_front <= 0 || distance_front >= preferred_distance) {
    return; // No obstacle detected
  }

  double distance = turn_90_degrees();
  if (distance < 0) {
    return;
  }

  PID_Controller pid;
  pid_init(&pid, KP, KI, KD, MAX_SPEED);

  bool sensors[NUM_SENS];

  while (running) {
    read_sensors(sensors);

    int x = 0;
    // Exit if we detect a line after timeout
    for (int i = 0; i < NUM_SENS; i++) {
      x += sensors[i];
      if (x > 2) {
        steer_car(MAX_SPEED);
        printf("OH SHOOT IM GONNA TURN");
        usleep(2e5);
        printf("OH SHOOT IM DONE TURNIN");
        return;
      }
    }

    double error = distance - distance_side;
    float pid_output = pid_update(&pid, error);

    // Adjust motor speeds based on PID
    int left_speed = MAX_SPEED;
    int right_speed = MAX_SPEED;
    int adjustment = pid_output;

    if (pid_output < 0) {
      right_speed += adjustment;
    } else {
      left_speed -= adjustment;
    }

    run_motor(RIGHT_MOTORS, FORWARD, left_speed);
    run_motor(LEFT_MOTORS, FORWARD, right_speed);
  }
}

void wait_for_button() {
  volatile int button_pressed = 0;

  while (true) {
    button_pressed = get_gpio(BUTTON_PIN);
    if (button_pressed) {
      break;
    }
  }

  button_pressed = 0;
}

void do_a_barrle_roll() {
  printf("bruh\n");
  run_motor(RIGHT_MOTORS, FORWARD, MAX_SPEED);
  run_motor(LEFT_MOTORS, BACKWARD, MAX_SPEED);
  usleep(2e6);
}

int main() {
  setup_gpio();

  init_gpio(LINE_SENS_1, GPIO_INPUT);
  init_gpio(LINE_SENS_2, GPIO_INPUT);
  init_gpio(LINE_SENS_3, GPIO_INPUT);
  init_gpio(LINE_SENS_4, GPIO_INPUT);
  init_gpio(LINE_SENS_5, GPIO_INPUT);
  init_gpio(COLOR_SENS_LED, GPIO_OUTPUT);
  init_gpio(BUTTON_PWR_PIN, GPIO_OUTPUT);
  init_gpio(BUTTON_PIN, GPIO_INPUT);

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
  set_gpio(BUTTON_PWR_PIN, 1);

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

  if (setjmp(main_loop_jump_buf) == 0) {
    wait_for_button();

    while (running) {
      read_sensors(sensors);
      error = calculate_error(sensors);
      if (error != 999) {
        known_error = error;
      }
      obstacle_avoidance();

      pid_out = pid_update(&pid, known_error);
      steer_car(pid_out);
      if (color.r - RED_SENSITIVITY > (color.g + color.b) / 2) {
        running = false;
      }

      if (color.g - GREEN_SENSITIVITY > (color.r + color.b) / 2) {
        printf("OH FUCK ITS GREEN AS SHIT\n");
        do_a_barrle_roll();
      }
    }
  } else {
    fprintf(stderr, "signal handler triggered, exiting\n");
  }

  pthread_join(color_thread, NULL);
  pthread_join(sonic_thread_front, NULL);
  pthread_join(sonic_thread_side, NULL);

  set_gpio(COLOR_SENS_LED, 0);
  set_gpio(BUTTON_PWR_PIN, 0);

  tcsoup_deinit(tc);
  stop_motor();
  deinit_gpio();

  return EXIT_SUCCESS;
}
