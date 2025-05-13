#include "button.h"
#include "gpio.h"

#include <stdio.h>

void waitForButton() {
  if (BUTTON_GPIO == -1) {
    printf("ERROR: BUTTON GPIO PIN NOT SET IN \"button.h\"\nNOT WAITING FOR BUTTON\n");
    return;
  }
  volatile int b_val = -1;
  b_val = get_gpio(BUTTON_GPIO);
  while (1) {
    if (b_val != 0) {
      return;
    }
    b_val = get_gpio(BUTTON_GPIO);
  }
}
