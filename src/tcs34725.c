/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Arjun Bhagat, Joseph Behroz, Varun Narravula, Matt Stoffel
 * Student IDs:: 917129686, 923625817, 923287037, 923127111
 * Github-Name:: water-sucks
 * Project:: Line Following Bot
 *
 * File:: src/tcs34725.c
 *
 * Description:
 *   Driver library for the TCS34725 RGB color sensor via I2C.
 *   - Open and close the sensor interface on a given I2C bus path.
 *   - Read and write 8-bit registers and 16-bit word data.
 *   - Configure enable, integration time, wait time, and interrupt thresholds.
 *   - Fetch raw clear, red, green, and blue channel values.
 **************************************************************/

#include "tcs34725.h"
#include "i2c.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct tcs34725 {
  struct i2c_device* dev;
};

struct tcs34725* tcs34725_init(char* i2c_bus_path) {
  struct tcs34725* tcs = malloc(sizeof(struct tcs34725));
  if (tcs == NULL) {
    return NULL;
  }

  tcs->dev = i2c_open(i2c_bus_path, TCS34725_I2C_ADDR);
  if (tcs->dev == NULL) {
    free(tcs);
    return NULL;
  }

  return tcs;
}

void tcs34725_deinit(struct tcs34725* tcs) {
  i2c_close(tcs->dev);
  free(tcs);
}

int i2c_write_byte(i2c_device* dev, uint8_t reg, uint8_t data) {
  uint8_t write_contents[2] = {0x80 | reg, data};
  return write(dev->fd, write_contents, 2) < 2 ? -1 : 0;
}

int i2c_read_byte(i2c_device* dev, uint8_t reg, uint8_t* data) {
  i2c_write_byte(dev, reg, 0);
  if (read(dev->fd, data, 1) != 1) {
    return -1;
  }
  return 0;
}

int i2c_read_word(struct i2c_device* dev, uint8_t start_reg, uint8_t* data) {
  i2c_write_byte(dev, start_reg | 0b00100000, 0);

  if (read(dev->fd, data, 2) != 2) {
    return -1;
  }

  return 0;
}

int tcs34725_enable_write(struct tcs34725* tcs, uint8_t fields) {
  return i2c_write_byte(tcs->dev, TCS_ENABLE, fields);
}

int tcs34725_enable_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_ENABLE, out);
}

int tcs34725_atime_write(struct tcs34725* tcs, uint8_t fields) {
  return i2c_write_byte(tcs->dev, TCS_ATIME, fields);
}

int tcs34725_atime_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_ATIME, out);
}

int tcs34725_wtime_write(struct tcs34725* tcs, uint8_t fields) {
  return i2c_write_byte(tcs->dev, TCS_WTIME, fields);
}

int tcs34725_wtime_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_WTIME, out);
}

int tcs34725_ailtl_write(struct tcs34725* tcs, uint8_t lthresh) {
  return i2c_write_byte(tcs->dev, TCS_AILTL, lthresh);
}

int tcs34725_ailth_write(struct tcs34725* tcs, uint8_t hthresh) {
  return i2c_write_byte(tcs->dev, TCS_AILTH, hthresh);
}

int tcs34725_ailt_write(struct tcs34725* tcs, uint16_t thresh) {
  uint8_t lo = (uint8_t)(thresh & 0x00FF);
  uint8_t hi = (uint8_t)((thresh & 0xFF00) >> 8);

  int x = tcs34725_ailtl_write(tcs, lo);
  if (x < 0) {
    return x;
  }
  x = tcs34725_ailth_write(tcs, hi);
  if (x < 0) {
    return x;
  }
  return x;
}

int tcs34725_ailtl_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_AILTL, out);
}

int tcs34725_ailth_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_AILTH, out);
}

int tcs34725_ailt_read(struct tcs34725* tcs, uint16_t* out) {
  uint8_t lo = 0;
  uint8_t hi = 0;

  int x = tcs34725_ailtl_read(tcs, &lo);
  if (x < 0) {
    return x;
  }
  x = tcs34725_ailth_read(tcs, &hi);
  if (x < 0) {
    return x;
  }

  *out = ((uint16_t)hi << 8) | lo;
  return x;
}

int tcs34725_aihtl_write(struct tcs34725* tcs, uint8_t lthresh) {
  return i2c_write_byte(tcs->dev, TCS_AIHTL, lthresh);
}

int tcs34725_aihth_write(struct tcs34725* tcs, uint8_t hthresh) {
  return i2c_write_byte(tcs->dev, TCS_AIHTH, hthresh);
}

int tcs34725_aiht_write(struct tcs34725* tcs, uint16_t thresh) {
  uint8_t lo = (uint8_t)(thresh & 0x00FF);
  uint8_t hi = (uint8_t)((thresh & 0xFF00) >> 8);

  int x = tcs34725_aihtl_write(tcs, lo);
  if (x < 0) {
    return x;
  }

  x = tcs34725_aihth_write(tcs, hi);
  if (x < 0) {
    return x;
  }
  return x;
}

int tcs34725_aihtl_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_AIHTL, out);
}

int tcs34725_aihth_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_AIHTH, out);
}

int tcs34725_aiht_read(struct tcs34725* tcs, uint16_t* out) {
  uint8_t lo = 0;
  uint8_t hi = 0;

  int x = tcs34725_aihtl_read(tcs, &lo);
  if (x < 0) {
    return x;
  }
  x = tcs34725_aihth_read(tcs, &hi);
  if (x < 0) {
    return x;
  }

  *out = ((uint16_t)hi << 8) | lo;
  return x;
}

int tcs34725_ailtht_write(struct tcs34725* tcs, uint16_t lthresh, uint16_t hthresh) {
  uint8_t llo = (uint8_t)(lthresh & 0x00FF);
  uint8_t lhi = (uint8_t)((lthresh & 0xFF00) >> 8);
  uint8_t hlo = (uint8_t)(hthresh & 0x00FF);
  uint8_t hhi = (uint8_t)((hthresh & 0xFF00) >> 8);

  int x = tcs34725_ailtl_write(tcs, llo);
  if (x < 0) {
    return x;
  }
  x = tcs34725_ailth_write(tcs, lhi);
  if (x < 0) {
    return x;
  }
  x = tcs34725_aihtl_write(tcs, hlo);
  if (x < 0) {
    return x;
  }
  x = tcs34725_aihth_write(tcs, hhi);
  if (x < 0) {
    return x;
  }
  return x;
}

int tcs34725_ailtht_read(struct tcs34725* tcs, uint16_t* lout, uint16_t* hout) {
  uint8_t llo = 0;
  uint8_t lhi = 0;
  uint8_t hlo = 0;
  uint8_t hhi = 0;

  int x = tcs34725_ailtl_read(tcs, &llo);
  if (x < 0) {
    return x;
  }
  x = tcs34725_ailth_read(tcs, &lhi);
  if (x < 0) {
    return x;
  }
  x = tcs34725_aihtl_read(tcs, &hlo);
  if (x < 0) {
    return x;
  }
  x = tcs34725_aihth_read(tcs, &hhi);
  if (x < 0) {
    return x;
  }

  *lout = ((uint16_t)lhi << 8) | llo;
  *hout = ((uint16_t)hhi << 8) | hlo;
  return x;
}

int tcs34725_pers_write(struct tcs34725* tcs, uint8_t fields) {
  return i2c_write_byte(tcs->dev, TCS_PERS, fields);
}

int tcs34725_pers_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_PERS, out);
}

int tcs34725_config_write(struct tcs34725* tcs, uint8_t wlong_bool) {
  return i2c_write_byte(tcs->dev, TCS_CONFIG, wlong_bool);
}

int tcs34725_config_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_CONFIG, out);
}

int tcs34725_ctrl_write(struct tcs34725* tcs, uint8_t fields) {
  return i2c_write_byte(tcs->dev, TCS_CTRL, fields);
}

int tcs34725_ctrl_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_CTRL, out);
}

int tcs34725_id_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_ID, out);
}

int tcs34725_status_read(struct tcs34725* tcs, uint8_t* out) {
  return i2c_read_byte(tcs->dev, TCS_STATUS, out);
}

int tcs34725_cdata_read(struct tcs34725* tcs, uint16_t* out) {
  uint8_t values[2];
  if (i2c_read_word(tcs->dev, TCS_CDATAL, values)) {
    return -1;
  };

  *out = ((uint16_t)values[1] << 8) | values[0];
  return 0;
}

int tcs34725_rdata_read(struct tcs34725* tcs, uint16_t* out) {
  uint8_t values[2];
  if (i2c_read_word(tcs->dev, TCS_RDATAL, values)) {
    return -1;
  };

  *out = ((uint16_t)values[1] << 8) | values[0];
  return 0;
}

int tcs34725_gdata_read(struct tcs34725* tcs, uint16_t* out) {
  uint8_t values[2];
  if (i2c_read_word(tcs->dev, TCS_GDATAL, values)) {
    return -1;
  };

  *out = ((uint16_t)values[1] << 8) | values[0];
  return 0;
}

int tcs34725_bdata_read(struct tcs34725* tcs, uint16_t* out) {
  uint8_t values[2];
  if (i2c_read_word(tcs->dev, TCS_BDATAL, values)) {
    return -1;
  };

  *out = ((uint16_t)values[1] << 8) | values[0];
  return 0;
}
