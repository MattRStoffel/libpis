/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 923127111
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: src/tcsoup.c
 *
 * Description:
 *   High-level library for the TCS34725 RGB color sensor.
 *   - Initialize sensor and configure integration time and gain.
 *   - Read raw clear, red, green, and blue channel values.
 *   - Normalize and gamma-correct raw readings.
 *   - Approximate to nearest named color with confidence score.
 **************************************************************/

#include "tcsoup.h"
#include "gpio.h"
#include "tcs34725.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct tcsoup {
  struct tcs34725* tcs;
};

typedef struct named_color {
  char name[20];
  uint8_t r, g, b;
} named_color;

#define CHECK_WRITE_ONE(f, tcs, expected)                                                          \
  {                                                                                                \
    uint8_t output = -1;                                                                           \
    f(tcs, &output);                                                                               \
    if (output != expected) {                                                                      \
      return -1;                                                                                   \
    }                                                                                              \
    return 0;                                                                                      \
  }

uint8_t check_write_one(int (*read_f)(struct tcs34725*, uint8_t*), struct tcs34725* tcs,
                        uint8_t expected) {
  uint8_t output = -1;
  (*read_f)(tcs, &output);
  if (output != expected) {
    printf("failed %d :: %d\n", expected, output);
    return -1;
  };
  return 0;
}

uint8_t check_write_two(int (*read_f)(struct tcs34725*, uint16_t*), struct tcs34725* tcs,
                        uint16_t expected) {
  uint16_t output = -1;
  (*read_f)(tcs, &output);
  if (output != expected) {
    return -1;
  };
  return 0;
}

struct tcsoup* tcsoup_init() {
  struct tcsoup* tc = malloc(sizeof(struct tcsoup));
  if (tc == NULL) {
    return NULL;
  }

  struct tcs34725* tcs = tcs34725_init("/dev/i2c-1");

  if (!tcs) {
    free(tc);
    return NULL;
  }

  tc->tcs = tcs;

  if ((tcs34725_enable_write(tcs,
                             TCS_ENABLE_AEN | TCS_ENABLE_PON | TCS_ENABLE_WEN | TCS_ENABLE_AIEN) ||
       tcs34725_atime_write(tcs, 0xFF) | tcs34725_wtime_write(tcs, 0xFF) ||
       tcs34725_ailt_write(tcs, 0) | tcs34725_aiht_write(tcs, 0xFFFF) ||
       tcs34725_pers_write(tcs, TCS_PERS_60) | tcs34725_config_write(tcs, 0) ||
       tcs34725_ctrl_write(tcs, TCS_CTRL_AGAIN_1)) != 0) {
    tcs34725_deinit(tcs);
    free(tc);
    return NULL;
  }

  return tc;
}

void tcsoup_deinit(struct tcsoup* cs) {
  tcs34725_deinit(cs->tcs);
  free(cs);
}

int tcsoup_set_integration_time(struct tcsoup* cs, uint32_t time) {
  if (!cs || !cs->tcs) {
    return -1;
  }
  return tcs34725_atime_write(cs->tcs, (uint8_t)time);
}

int tcsoup_set_gain(struct tcsoup* cs, uint8_t gain_level) {
  if (!cs || !cs->tcs || gain_level > 3) {
    return -1;
  }

  return tcs34725_ctrl_write(cs->tcs, gain_level);
}

int tcsoup_read(struct tcsoup* cs, color_t* color) {
  if (!cs || !cs->tcs) {
    return -1;
  }

  uint16_t r = 0;
  uint16_t g = 0;
  uint16_t b = 0;
  uint16_t c = 0;

  if (tcs34725_rdata_read(cs->tcs, &r) < 0) {
    return -1;
  }

  if (tcs34725_gdata_read(cs->tcs, &g) < 0) {
    return -1;
  }

  if (tcs34725_bdata_read(cs->tcs, &b) < 0) {
    return -1;
  }

  if (tcs34725_cdata_read(cs->tcs, &c) < 0) {
    return -1;
  }

  if (c == 0) {
    return -1;
  }

  // `c` is a sum of all channels, and is used to
  // normalize the RGB values.
  uint8_t r_value = (uint8_t)((r * 255) / c);
  uint8_t g_value = (uint8_t)((g * 255) / c);
  uint8_t b_value = (uint8_t)((b * 255) / c);

  color->r = r_value;
  color->g = g_value;
  color->b = b_value;

  return 0;
}

named_color known_colors[] = {
    {"White", 255, 255, 255},      {"Pink", 255, 0, 255},   {"Red", 255, 0, 0},
    {"Dark Pink", 128, 0, 128},    {"Dark Red", 128, 0, 0},

    {"Light Blue", 0, 255, 255},   {"Blue", 0, 0, 255},     {"Dark Cyan", 0, 128, 128},
    {"Dark Blue", 0, 0, 128},

    {"Light Green", 0, 255, 0},    {"Yellow", 255, 255, 0}, {"Dark Yellow", 128, 128, 0},
    {"Green", 0, 128, 0},

    {"Light Gray", 192, 192, 192}, {"Gray", 128, 128, 128}, {"Black", 0, 0, 0},
};

int tcsoup_approx(struct tcsoup* tc, char** color_name, float* confidence) {
  if (!tc || !tc->tcs || !color_name || !confidence) {
    return -1;
  }

  color_t raw_color;
  if (tcsoup_read(tc, &raw_color) != 0) {
    return -1;
  }

  uint8_t r = raw_color.r;
  uint8_t g = raw_color.g;
  uint8_t b = raw_color.b;

  // Apply gamma correction (inline)
  float gamma = 1 / 2.2f;
  float r_f = powf(r / 255.0f, gamma) * 255.0f;
  float g_f = powf(g / 255.0f, gamma) * 255.0f;
  float b_f = powf(b / 255.0f, gamma) * 255.0f;

  float min_distance = 1e9f;
  int closest_index = -1;

  for (size_t i = 0; i < sizeof(known_colors) / sizeof(known_colors[0]); i++) {
    float dr = known_colors[i].r - r_f;
    float dg = known_colors[i].g - g_f;
    float db = known_colors[i].b - b_f;
    float dist = sqrtf(dr * dr + dg * dg + db * db);

    if (dist < min_distance) {
      min_distance = dist;
      closest_index = i;
    }
  }

  // Compute confidence
  float max_dist = sqrtf(3 * 255.0f * 255.0f); // ≈ 441.67
  *confidence = 1.0f - (min_distance / max_dist);

  if (*confidence < 0) {
    *confidence = 0;
  }

  *color_name = known_colors[closest_index].name;
  return 0;
}
