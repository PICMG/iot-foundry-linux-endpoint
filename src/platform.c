/**
 * @file platform.c
 * @brief Platform API shim layer for Linux microcontrollers/microprocessors.
 * 
 * Provides implementations of platform-specific functions for serial I/O.  Initialization
 * is performed based on command-line settings.
 *
 * @author Douglas Sandy
 *
 * MIT No Attribution
 *
 * Copyright (c) 2026 Douglas Sandy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _XOPEN_SOURCE
    #define _XOPEN_SOURCE 700
#endif
#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#endif
#include "core/platform.h"
#include "config.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pty.h>
#include <termios.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

/* Global/static serial device instance for platform serial I/O */
extern config_t serial_device;

/**
 * @brief Initialize platform hardware.
 *
 * This function is called once by mctp_init to initialize
 * platform-specific hardware (serial interfaces, timers, etc.).
 */
void platform_init(void) {
    // special case: if path is empty, create a ptys pair for testing
    printf("Initializing platform serial interface...\n");
    printf("  Device path: %s\n", serial_device.path[0] == '\0' ? "(pty)" : serial_device.path);
    printf("  Baud rate: %d\n", serial_device.baud);
    printf("  Hardware flow control: %s\n", serial_device.hwflow ? "ENABLED" : "DISABLED");
    if (serial_device.path[0] == '\0') {
        // open a pty device and get its name
        int master_fd = posix_openpt(O_RDWR | O_NOCTTY);
        if (master_fd == -1) {
            perror("posix_openpt");
            return;
        }
        if (grantpt(master_fd) == -1 || unlockpt(master_fd) == -1) {
            perror("grantpt/unlockpt");
            close(master_fd);
            return;
        }
        char* slave_name = ptsname(master_fd);
        if (slave_name == NULL) {
            perror("ptsname");
            close(master_fd);
            return;
        }
        // copy the slave name to the config
        strncpy(serial_device.path, slave_name, SERIAL_PATH_MAX - 1);
        serial_device.path[SERIAL_PATH_MAX - 1] = '\0';
        serial_device.fd = master_fd;
        printf("  Created pty device: %s\n", serial_device.path);
        fflush(stdout);
    } else {
        serial_device.fd = open(serial_device.path, O_RDWR | O_NOCTTY | O_NDELAY);
        if (serial_device.fd == -1) {
            perror("open");
            return;
        }

        // Configure serial port
        struct termios tty;
        memset(&tty, 0, sizeof tty);

        if (tcgetattr(serial_device.fd, &tty) != 0) {
            perror("tcgetattr");
            close(serial_device.fd);
            serial_device.fd = -1;
            return;
        }

        // Set baud rate
        cfsetospeed(&tty, serial_device.baud);
        cfsetispeed(&tty, serial_device.baud);

        // 8N1 mode
        tty.c_cflag &= ~PARENB;        // No parity
        tty.c_cflag &= ~CSTOPB;        // 1 stop bit
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;            // 8 bits
        tty.c_cflag &= ~CRTSCTS;       // No hardware flow control
        if (serial_device.hwflow) {
            tty.c_cflag |= CRTSCTS;    // Enable hardware flow control if requested
        }
        tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines

        // Raw mode
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        tty.c_oflag &= ~OPOST;

        // Apply settings
        if (tcsetattr(serial_device.fd, TCSANOW, &tty) != 0) {
            perror("tcsetattr");
            close(serial_device.fd);
            serial_device.fd = -1;
            return;
        }
    }
}

/**
 * @brief Query whether data is available to read from the serial interface.
 *
 * @return uint8_t Returns non-zero when data is available to read.
 */
uint8_t platform_serial_has_data(void) {
    if (serial_device.fd == -1) return 0;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(serial_device.fd, &rfds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0; /* non-blocking */

    int ret = select(serial_device.fd + 1, &rfds, NULL, NULL, &tv);
    if (ret > 0) {
        return FD_ISSET(serial_device.fd, &rfds) ? 1 : 0;
    }
    /* on error or no data available, return 0 */
    return 0;
}

/**
 * @brief Read a byte from the serial interface. May block if no data is available.
 *
 * @return uint8_t The byte read from the serial interface.
 */
uint8_t platform_serial_read_byte(void) {
    uint8_t byte;
    ssize_t result = read(serial_device.fd, &byte, 1);
    if (result == 1) {
        return byte;
    } else {
        /* on error or no data, return 0 */
        return 0;
    }
}

/**
 * @brief Write a byte to the serial interface. May block if the interface is not ready.
 *
 * @param b The byte to write.
 */
void platform_serial_write_byte(uint8_t b) {
    ssize_t result = write(serial_device.fd, &b, 1);
    if (result != 1) {
        /* handle error */
        perror("write");
    }   
}

/**
 * @brief Query whether the serial interface can accept writes.
 *
 * @return uint8_t Returns non-zero when writes are currently allowed.
 */
uint8_t platform_serial_can_write(void) {
    if (serial_device.fd == -1) return 0;

    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(serial_device.fd, &wfds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0; /* non-blocking */

    int ret = select(serial_device.fd + 1, NULL, &wfds, NULL, &tv);
    if (ret > 0) {
        return FD_ISSET(serial_device.fd, &wfds) ? 1 : 0;
    }
    /* on error or not writable, return 0 */
    return 0;
}
