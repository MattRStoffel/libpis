#include "DirectRegister.h"

volatile unsigned *gpio;

// grab the memory of the GPIO pins
int setup_io() {
  int fd;
  void *map;

  if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC)) < 0) {
    return -1;
  }

  map = mmap(NULL, 4096, 2, 1, fd, 0x20200000);
  close(fd);

  if (map == MAP_FAILED) {
    return -1;
  }

  gpio = (volatile unsigned *)map;

  return 0;
}

int initGPIO(int pin, int mode) {
  // clearing any previous configs
  *(gpio + OFFSET(pin)) &= ~(0b111 << SHIFT(pin));
  // set function select register to output
  *(gpio + OFFSET(pin)) |= (mode << SHIFT(pin));
  return (*(gpio + OFFSET(pin)) & (0b111 << SHIFT(pin))) ^ (mode << SHIFT(pin));
}

int setGPIO(int pin, int value) {
  // make sure the pin is set to output
  if ((*(gpio + OFFSET(pin)) & (0b111 << SHIFT(pin))) ^
      (GPIO_OUTPUT << SHIFT(pin))) {
    return -1;
  }
  if (value) {
    // set the pin high
    *(gpio + 7) = 1 << pin;
  } else {
    // set the pin low
    *(gpio + 10) = 1 << pin;
  }
  return 0;
}

int getGPIO(int pin) {
  // make sure the pin is set to input
  if ((*(gpio + OFFSET(pin)) & (0b111 << SHIFT(pin))) ^
      (GPIO_INPUT << SHIFT(pin))) {
    return -1;
  }
  return (*(gpio + 13) & (1 << pin)) >> pin;
}

int clean_up_io() { return munmap(&gpio, 4096); }
