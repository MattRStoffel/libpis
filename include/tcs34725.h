/**************************************************************
 * Class:: CSC-615-01 Spring 2025
 * Name:: Dammit, Bobby!
 * Student ID::
 * Github-Name:: water-sucks
 * Project:: Assignment 5 - RGB Sensor
 *
 * File:: include/tcsoup/tcs34725.h
 *
 * Description:: Create a library for the TCS34725 RGB sensor.
 *
 **************************************************************/

#ifndef _TCS34725_H_
#define _TCS34725_H_

#include <stdint.h>

/// @brief I2C address of the TCS34725 device.
#define TCS34725_I2C_ADDR 0x29

/// @brief Register: Enable (controls power and ADC).
#define TCS_ENABLE 0x00

/// @brief Register: Integration time.
#define TCS_ATIME 0x01
/// @brief Register: Wait time.
#define TCS_WTIME 0x03

/// @brief Register: ALS interrupt low threshold low byte.
#define TCS_AILTL 0x04
/// @brief Register: ALS interrupt low threshold high byte.
#define TCS_AILTH 0x05
/// @brief Register: ALS interrupt high threshold low byte.
#define TCS_AIHTL 0x06
/// @brief Register: ALS interrupt high threshold high byte.
#define TCS_AIHTH 0x07

/// @brief Register: Interrupt persistence filter.
#define TCS_PERS 0x0C
/// @brief Register: Configuration.
#define TCS_CONFIG 0x0D
/// @brief Register: Control (gain settings).
#define TCS_CTRL 0x0F
/// @brief Register: Device ID.
#define TCS_ID 0x12
/// @brief Register: Status (interrupt and ADC validity).
#define TCS_STATUS 0x13

/// @brief Register: Clear data low byte.
#define TCS_CDATAL 0x14
/// @brief Register: Clear data high byte.
#define TCS_CDATAH 0x15
/// @brief Register: Red data low byte.
#define TCS_RDATAL 0x16
/// @brief Register: Red data high byte.
#define TCS_RDATAH 0x17
/// @brief Register: Green data low byte.
#define TCS_GDATAL 0x18
/// @brief Register: Green data high byte.
#define TCS_GDATAH 0x19
/// @brief Register: Blue data low byte.
#define TCS_BDATAL 0x1A
/// @brief Register: Blue data high byte.
#define TCS_BDATAH 0x1B

// ENABLE register bit masks

/// @brief Enable ALS interrupts.
#define TCS_ENABLE_AIEN 1 << 4
/// @brief Enable wait timer.
#define TCS_ENABLE_WEN 1 << 3
/// @brief Enable RGBC ADC.
#define TCS_ENABLE_AEN 1 << 1
/// @brief Power ON.
#define TCS_ENABLE_PON 1 << 0

// PERS register values

/// @brief Interrupt on every cycle.
#define TCS_PERS_EVERY 0
/// @brief Interrupt after 1 cycle.
#define TCS_PERS_1 1
/// @brief Interrupt after 2 cycles.
#define TCS_PERS_2 2
/// @brief Interrupt after 3 cycles.
#define TCS_PERS_3 3
/// @brief Interrupt after 5 cycles.
#define TCS_PERS_5 4
/// @brief Interrupt after 10 cycles.
#define TCS_PERS_10 5
/// @brief Interrupt after 15 cycles.
#define TCS_PERS_15 6
/// @brief Interrupt after 20 cycles.
#define TCS_PERS_20 7
/// @brief Interrupt after 25 cycles.
#define TCS_PERS_25 8
/// @brief Interrupt after 30 cycles.
#define TCS_PERS_30 9
/// @brief Interrupt after 35 cycles.
#define TCS_PERS_35 10
/// @brief Interrupt after 40 cycles.
#define TCS_PERS_40 11
/// @brief Interrupt after 45 cycles.
#define TCS_PERS_45 12
/// @brief Interrupt after 50 cycles.
#define TCS_PERS_50 13
/// @brief Interrupt after 55 cycles.
#define TCS_PERS_55 14
/// @brief Interrupt after 60 cycles.
#define TCS_PERS_60 15

// CTRL register gain settings

/// @brief 1x gain.
#define TCS_CTRL_AGAIN_1 0
/// @brief 4x gain.
#define TCS_CTRL_AGAIN_4 1
/// @brief 16x gain.
#define TCS_CTRL_AGAIN_16 2
/// @brief 60x gain.
#define TCS_CTRL_AGAIN_60 3

// Device part IDs

/// @brief Part ID for TCS34721.
#define TCS_PART_34721 0x44
/// @brief Part ID for TCS34725.
#define TCS_PART_34725 0x44
/// @brief Part ID for TCS34723.
#define TCS_PART_34723 0x45
/// @brief Part ID for TCS34727.
#define TCS_PART_34727 0x45

// STATUS register bits

/// @brief ALS interrupt occurred.
#define TCS_STATUS_AINT (1 << 4)
/// @brief RGBC data valid.
#define TCS_STATUS_AVALID (1 << 0)

struct tcs34725;

/// @brief Initializes the TCS34725 device over the specified I2C bus.
/// @param i2c_bus_path Path to the I2C device (e.g., "/dev/i2c-1").
/// @return Pointer to a tcs34725 handle, or NULL on failure.
struct tcs34725* tcs34725_init(char* i2c_bus_path);

/// @brief Releases resources for the TCS34725 device.
void tcs34725_deinit(struct tcs34725* tcs);

/// @brief Writes to the ENABLE register.
/// @param fields Bitmask of TCS_ENABLE_* values.
/// @return 0 on success, -1 on failure.
int tcs34725_enable_write(struct tcs34725* tcs, uint8_t fields);

/// @brief Reads the ENABLE register.
/// @param out Pointer to store register value.
/// @return 0 on success, -1 on failure.
int tcs34725_enable_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Sets the integration time.
/// @param fields Raw ATIME value (0–255).
int tcs34725_atime_write(struct tcs34725* tcs, uint8_t fields);
int tcs34725_atime_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Sets the wait time between cycles.
/// @param fields Raw WTIME value (0–255).
int tcs34725_wtime_write(struct tcs34725* tcs, uint8_t fields);
int tcs34725_wtime_read(struct tcs34725* tcs, uint8_t* out);

// Interrupt threshold functions

int tcs34725_ailtl_write(struct tcs34725* tcs, uint8_t lthresh);
int tcs34725_ailth_write(struct tcs34725* tcs, uint8_t hthresh);
int tcs34725_ailt_write(struct tcs34725* tcs, uint16_t thresh);
int tcs34725_ailtl_read(struct tcs34725* tcs, uint8_t* out);
int tcs34725_ailth_read(struct tcs34725* tcs, uint8_t* out);
int tcs34725_ailt_read(struct tcs34725* tcs, uint16_t* out);

int tcs34725_aihtl_write(struct tcs34725* tcs, uint8_t lthresh);
int tcs34725_aihth_write(struct tcs34725* tcs, uint8_t hthresh);
int tcs34725_aiht_write(struct tcs34725* tcs, uint16_t thresh);
int tcs34725_aihtl_read(struct tcs34725* tcs, uint8_t* out);
int tcs34725_aihth_read(struct tcs34725* tcs, uint8_t* out);
int tcs34725_aiht_read(struct tcs34725* tcs, uint16_t* out);

/// @brief Writes both low and high threshold values at once.
int tcs34725_ailtht_write(struct tcs34725* tcs, uint16_t lthresh, uint16_t hthresh);

/// @brief Reads both low and high threshold values at once.
int tcs34725_ailtht_read(struct tcs34725* tcs, uint16_t* lout, uint16_t* hout);

int tcs34725_aihtl_write(struct tcs34725* tcs, uint8_t fields);
int tcs34725_aihth_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Sets interrupt persistence behavior.
int tcs34725_pers_write(struct tcs34725* tcs, uint8_t fields);
int tcs34725_pers_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Enables or disables wait long mode.
int tcs34725_config_write(struct tcs34725* tcs, uint8_t wlong_bool);
int tcs34725_config_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Sets the RGBC gain control.
int tcs34725_ctrl_write(struct tcs34725* tcs, uint8_t fields);
int tcs34725_ctrl_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Reads the device ID register.
int tcs34725_id_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Reads the status register.
int tcs34725_status_read(struct tcs34725* tcs, uint8_t* out);

/// @brief Reads clear light data.
int tcs34725_cdata_read(struct tcs34725* tcs, uint16_t* out);
/// @brief Reads red light data.
int tcs34725_rdata_read(struct tcs34725* tcs, uint16_t* out);
/// @brief Reads green light data.
int tcs34725_gdata_read(struct tcs34725* tcs, uint16_t* out);
/// @brief Reads blue light data.
int tcs34725_bdata_read(struct tcs34725* tcs, uint16_t* out);

#endif
