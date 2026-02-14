/**
 * @file platform.h
 * @brief Platform API for serial I/O
 */
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <time.h>

/* MIN macro for platform code */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Zephyr compatibility: uptime in milliseconds */
uint32_t k_uptime_get_32(void);

/**
 * @brief Initialize platform hardware.
 */
void platform_init(void);

/**
 * @brief Query whether data is available to read from the serial interface.
 * @return Non-zero when data is available to read.
 */
uint8_t platform_serial_has_data(void);

/**
 * @brief Read a byte from the serial interface.
 * @return The byte read from the serial interface.
 */
uint8_t platform_serial_read_byte(void);

/**
 * @brief Write a byte to the serial interface.
 * @param b The byte to write.
 */
void platform_serial_write_byte(uint8_t b);

/**
 * @brief Query whether the serial interface can accept writes.
 * @return Non-zero when writes are currently allowed.
 */
uint8_t platform_serial_can_write(void);

#endif /* PLATFORM_H */
