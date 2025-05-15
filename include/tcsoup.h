/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Dammit, Bobby!
 * Student IDs:: Joseph - 923625817
 * Github-Name:: water-sucks
 * Project:: Assignment 5 - RGB Sensor
 *
 * File:: include/tcsoup.h
 *
 * Description:: Create a library for the TCS34725 RGB sensor.
 *
 **************************************************************/

#ifndef _TCSOUP_H_
#define _TCSOUP_H_

#include <stdint.h>

/// @brief Forward declaration of the opaque tcsoup context structure.
struct tcsoup;

/// @brief Represents a simple RGB color with 8-bit components.
typedef struct color_t {
  /// Red component (0–255).
  uint8_t r;
  /// Green component (0–255).
  uint8_t g;
  /// Blue component (0–255).
  uint8_t b;
} color_t;

/// @brief Initializes a new tcsoup context and powers on the TCS34725 sensor.
/// @return Pointer to a new tcsoup context, or NULL on failure.
struct tcsoup* tcsoup_init();

/// @brief Deinitializes a tcsoup context and powers down the sensor.
/// Frees any allocated resources.
/// @param tc Pointer to the tcsoup context to destroy.
void tcsoup_deinit(struct tcsoup* tc);

/// @brief Assigns a GPIO pin for controlling the onboard LED.
/// This function does not configure the pin direction—just stores the pin for internal use.
/// @param cs Pointer to the tcsoup context.
/// @param pin GPIO pin number to use.
/// @return 0 on success, -1 on failure.
int tcsoup_set_led_pin(struct tcsoup* cs, int pin);

/// @brief Sets the integration time for the sensor's ADC conversions.
/// Affects the sensitivity and update rate of color readings.
/// @param tc Pointer to the tcsoup context.
/// @param time Integration time in microseconds. Must match supported values.
/// @return 0 on success, -1 on failure.
int tcsoup_set_integration_time(struct tcsoup* tc, uint32_t time);

/// @brief Sets the gain level for color sensing amplification.
/// Useful for adapting to low- or high-light environments.
/// @param tc Pointer to the tcsoup context.
/// @param gain_level One of the valid gain values (typically 1×, 4×, 16×, or 60×).
/// @return 0 on success, -1 on failure.
int tcsoup_set_gain(struct tcsoup* tc, uint8_t gain_level);

/// @brief Reads the current color sample from the sensor and stores it as an RGB triplet.
/// @param tc Pointer to the tcsoup context.
/// @param color Pointer to a color_t struct to populate with the result.
/// @return 0 on success, -1 on failure.
int tcsoup_read(struct tcsoup* tc, color_t* color);

/// @brief Approximates the current color reading as a named color (e.g., "red", "blue").
/// @param tc Pointer to the tcsoup context.
/// @param color_name Pointer to a string pointer to be set to the name of the color. Memory is
/// owned by the caller.
/// @param confidence Pointer to a float to receive the confidence value (range 0.0–1.0).
/// @return 0 on success, -1 on failure.
int tcsoup_approx(struct tcsoup* tc, char** color_name, float* confidence);

#endif
