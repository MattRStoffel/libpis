#include "gpio.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

volatile unsigned* gpio;

int setup_gpio() {
  int fd;
  void* map;

  if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC)) < 0) {
    return -1;
  }

  map = mmap(NULL, 4096, 2, 1, fd, 0x20200000);
  close(fd);

  if (map == MAP_FAILED) {
    return -1;
  }

  gpio = (volatile unsigned*)map;

  return 0;
}

int init_gpio(int pin, int mode) {
  // Clearing any previous configs
  *(gpio + OFFSET(pin)) &= ~(0b111 << SHIFT(pin));
  // Set function select register to output
  *(gpio + OFFSET(pin)) |= (mode << SHIFT(pin));
  return (*(gpio + OFFSET(pin)) & (0b111 << SHIFT(pin))) ^ (mode << SHIFT(pin));
}

int set_gpio(int pin, int value) {
  // Make sure the pin is set to output
  if ((*(gpio + OFFSET(pin)) & (0b111 << SHIFT(pin))) ^ (GPIO_OUTPUT << SHIFT(pin))) {
    return -1;
  }

  if (value) {
    // Set the pin high
    *(gpio + 7) = 1 << pin;
  } else {
    // Set the pin low
    *(gpio + 10) = 1 << pin;
  }
  return 0;
}

void turnOnGPIO(int pin) {
  *(gpio + 7) = 1 << pin;
}

void turnOffGPIO(int pin) {
  *(gpio + 10) = 1 << pin;
}

int get_gpio(int pin) {
  // Make sure the pin is set to input
  if ((*(gpio + OFFSET(pin)) & (0b111 << SHIFT(pin))) ^ (GPIO_INPUT << SHIFT(pin))) {
    return -1;
  }
  return (*(gpio + 13) & (1 << pin)) >> pin;
}

int deinit_gpio() {
  return munmap(&gpio, 4096);
}
