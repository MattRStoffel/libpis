#ifndef DIRECTREGISTER_H
#define DIRECTREGISTER_H

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// Each function select register controls 10 pins
#define OFFSET(x) (x / 10)
// Each pin has 3 bits to control its settings
#define SHIFT(x) ((x % 10) * 3)

#define GPIO_OUTPUT 0b001
#define GPIO_INPUT 0b000

int setup_io();
int initGPIO(int pin, int mode);
int setGPIO(int pin, int value);
int getGPIO(int pin);
int clean_up_io();
#endif
