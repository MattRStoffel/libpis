#ifndef _GPIO_H_
#define _GPIO_H_

// Each function select register controls 10 pins
#define OFFSET(x) (x / 10)
// Each pin has 3 bits to control its settings
#define SHIFT(x) ((x % 10) * 3)

#define GPIO_OUTPUT 0b001
#define GPIO_INPUT 0b000

int setup_gpio();
int init_gpio(int pin, int mode);
int set_gpio(int pin, int value);
int get_gpio(int pin);
int clean_up_io();

#endif
