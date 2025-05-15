/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Dammit, Bobby!
 * Student IDs:: Joseph - 923625817
 * Github-Name:: water-sucks
 * Project:: Assignment 5 - RGB Sensor
 *
 * File:: include/pid.h
 *
 * Description:: Header file for PID functionality.
 *
 **************************************************************/


#ifndef PID_H
#define PID_H

typedef struct {
  float kp, ki, kd;
  float prev_error;
  float integral;
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
