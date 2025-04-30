#ifndef _PCA9685_H_
#define _PCA9685_H_

#include <stdint.h>

#define SUBADR1 0x02
#define SUBADR2 0x03
#define SUBADR3 0x04
#define MODE1 0x00
#define PRESCALE 0xFE
#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09
#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

#define PCA_CHANNEL_0 0
#define PCA_CHANNEL_1 1
#define PCA_CHANNEL_2 2
#define PCA_CHANNEL_3 3
#define PCA_CHANNEL_4 4
#define PCA_CHANNEL_5 5
#define PCA_CHANNEL_6 6
#define PCA_CHANNEL_7 7
#define PCA_CHANNEL_8 8
#define PCA_CHANNEL_9 9
#define PCA_CHANNEL_10 10
#define PCA_CHANNEL_11 11
#define PCA_CHANNEL_12 12
#define PCA_CHANNEL_13 13
#define PCA_CHANNEL_14 14
#define PCA_CHANNEL_15 15

typedef struct pca9685_device pca9685_device;

pca9685_device* pca9685_init(uint8_t addr);
void pca9685_set_pwm_freq(pca9685_device* dev, uint16_t freq);
void pca9685_set_pwm_duty_cycle(pca9685_device* dev, uint8_t channel, uint16_t pulse);
void pca9685_set_level(pca9685_device* dev, uint16_t channel, uint16_t value);

#endif
