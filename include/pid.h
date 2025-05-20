/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 92312711
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: include/pid.h
 *
 * Description:: Header for PID functionality.
 *
 **************************************************************/

#ifndef PID_H
#define PID_H

typedef struct {
  float kp, ki, kd;
  float prev_error;
  float clamp_val;
} PID_Controller;

/*
 * Initializes the given PID_Controller struct to the provided values.
 * var pid: PID_Controller struct being initialized.
 * var kp: TODO: fill these in idk how pid works -Joseph
 * var ki:
 * var kd:
 * var clamp_val:
 */
void pid_init(PID_Controller*, float, float, float, float);
float pid_update(PID_Controller*, float);
float clamp(float, float, float);

#endif
