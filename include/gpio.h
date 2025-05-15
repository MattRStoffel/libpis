/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Dammit, Bobby!
 * Student IDs:: Joseph - 923625817
 * Github-Name:: water-sucks
 * Project:: Assignment 5 - RGB Sensor
 *
 * File:: include/gpio.h
 *
 * Description:: Functionality for communication with GPIO pins.
 *
 **************************************************************/

#ifndef _GPIO_H_
#define _GPIO_H_

// Each function select register controls 10 pins
#define OFFSET(x) (x / 10)
// Each pin has 3 bits to control its settings
#define SHIFT(x) ((x % 10) * 3)

#define GPIO_OUTPUT 0b001
#define GPIO_INPUT 0b000

/*
 * Sets up the memory map needed to read/write from
 * the GPIO pins on the pi.
 * Map is stored in pointer "gpio".
 * Returns 0 on success.
 */
int setup_gpio();

/*
 * Initializes inputted GPIO pin to provided mode.
 * Returns 0 on success.
 * var pin: GPIO pin number being initialized.
 * var mode: Which mode to set the pin to (input or output)
 */
int init_gpio(int pin, int mode);

/*
 * Set GPIO pin value to provided value.
 * Returns 0 on success.
 * var pin: GPIO pin number to set value for
 * var value: Value to set pin to
 */
int set_gpio(int pin, int value);

/*
 * Retrieve value of provided GPIO pin.
 * Returns value of pin.
 * var pin: Pin to read
 * WARNING: This function is commonly inlined by the
 * compiler unless output is stored to a volatile int.
 */
int get_gpio(int pin);

/*
 * Deinitializes the GPIO memmap.
 * Returns 0 on success.
 */
int deinit_gpio();

/*
 * Set provided GPIO pin to the "high" state.
 * WARNING: This function is commonly inlined by the
 * compiler.
 */
void turnOnGPIO(int pin);

/*
 * Set provided GPIO pin to the "low" state.
 * WARNING: This function is commonly inlined by the
 * compiler.
 */
void turnOffGPIO(int pin);

#endif
