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

void MotorInit(void);
void RunMotor(char motor, char dir, int speed);
void StopMotor(void);

#endif
