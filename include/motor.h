/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Dammit, Bobby!
 * Student IDs:: Joseph - 923625817
 * Github-Name:: water-sucks
 * Project:: Assignment 5 - RGB Sensor
 *
 * File:: include/motor.h
 *
 * Description:: Functionality for motor control.
 *
 **************************************************************/

#ifndef _MOTOR_H_
#define _MOTOR_H_

// Motor B on the hat
#define PWMB PCA_CHANNEL_5
#define IN1B PCA_CHANNEL_4
#define IN2B PCA_CHANNEL_3
// Motor A
#define PWMA 0
#define IN1A 1
#define IN2A 2

#define FORWARD 'F'
#define BACKWARD 'B'
#define MOTOR_A 'A'
#define MOTOR_B 'B'

/*
 * Initialize the motors. The motors are connected to a PCA9685 hat
 * for the Raspberry Pi, and communicate with each other via I2C.
 * This function assumes you have a stock hat which has the I2C
 * address 0x40. This initializes a PCA device struct pointer "dev".
 * Afterwards, the PWM frequency of the motors are set to 100.
 * Returns 0 on success.
 */
int motor_init(void);

/*
 * Powers the given motor in given direction at given speed.
 * var motor: Which motor to power.
 * var dir: Which direction to turn
 * var speed: What percentage of the max power is being provided
 */
void run_motor(char motor, char dir, int speed);

/*
 * Stops both motors.
 */
void stop_motor(void);

#endif
