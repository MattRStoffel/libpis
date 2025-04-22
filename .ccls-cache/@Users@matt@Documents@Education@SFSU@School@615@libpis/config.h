#ifndef CONFIG_H
#define CONFIG_H

// PID Gains
#define KP_DEFAULT 1.0
#define KI_DEFAULT 0.1
#define KD_DEFAULT 0.01

// Limits for motor speeds (output of PID)
#define MIN_OUTPUT -100
#define MAX_OUTPUT 100

// Simulation parameters
#define MAX_ITERATIONS 100
#define INITIAL_POSITION_X 0.0
#define INITIAL_POSITION_Y 0.0
#define TARGET_DISTANCE 1.0 // Distance between waypoints

// Track parameters
#define TRACK_LENGTH 5
#define TRACK_WIDTH 1.0 // Width of the track, used for detecting deviation

#endif
