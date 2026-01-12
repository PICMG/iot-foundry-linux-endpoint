/**
 * @file main.c
 * @brief Application entry for the AVR-based MCTP demo.
 *
 * Initializes platform and MCTP subsystems, then runs the main polling
 * loop which processes incoming MCTP packets.
 *
 * @author Douglas Sandy
 *
 * MIT License
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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include "config.h"

#include "core/mctp.h"
#include "core/platform.h"

#ifdef PLDM_SUPPORT
#include "core/pldm_version.h"
#endif

// our configuration structure
config_t serial_device = {
    .baud = 115200,
    .hwflow = 0,
    .path = "",
    .fd = -1
};

/*
 * @brief Handle signals (e.g., SIGINT, SIGTERM) by setting the interrupted flag.
 *
 * @param signum  Signal number received.
 * @return void
 */
static volatile int interrupted = 0;
void signalHandler(int signum) {
    printf("\nCaught signal %d, cleaning up...\n", signum);
    interrupted = 1;
}

/**
 * @brief Maps a string like "115200" to a BaudRate enum value.
 * @param str The baud rate string (e.g., "9600", "115200").
 * @return returns the baud rate for the string, or a default if not found.
 */
int baudRateFromString(char * str) {
    static const struct {
        const char *name;
        int val;
    } baudMap[] = {
        {"4800", B4800}, {"9600", B9600}, {"19200", B19200},
        {"38400", B38400}, {"57600", B57600}, {"115200", B115200},
        {"230400", B230400}, {NULL, 0}
    };

    if (!str) return B115200;
    for (const typeof(baudMap[0]) *e = baudMap; e->name != NULL; ++e) {
        if (strcmp(str, e->name) == 0) return e->val;
    }
    printf("Warning: Unrecognized baud rate '%s', using default 115200.\n", str);
    return B115200; /* default */
}

/**
 * @brief Print command-line usage for the program.
 *
 * @param progName  Program name (typically argv[0]) used in usage and examples.
 * @return void     Prints usage to stdout and returns.
 */
void printUsage(const char* progName) {
    printf("Usage: %s --tty <tty-path> [options]\n\n", progName);

    printf("Required:\n");
    printf("  --tty  <tty-path>       Path to serial device (e.g. /dev/ttyS0, /dev/ttyUSB0).\n\n");

    printf("Optional:\n");
    printf("  --baud <baud-string>    Baud rate string (e.g. 9600, 115200). If omitted, default 115200 is used\n");
    printf("  --hwflow <TRUE|FALSE>   Hardware flow control. TRUE to enable RTS/CTS, FALSE (default) to disable.\n");
    printf("  --help                  Show this help message and exit.\n\n");

    printf("Examples:\n");
    printf("  %s --tty /dev/ttyUSB0 --baud 115200 --hwflow TRUE \n", progName);
    printf("Notes:\n");
    printf("  - The code is blocking and will run until iterrupted with SIGINT.\n");
    printf("\n");
}

/**
 * @brief Convert a string to lowercase.
 *
 * @param s  Input string to convert (modified copy).
 * @return void - lowercase conversion is done in place.
 */
 void toLower(char *s) {
    while (*s) {
        *s = tolower((unsigned char)*s);
        s++;
    }
}

/*
 * @brief Parse a boolean-like string into a boolean value.
 *
 * Accepts (case-insensitive): true/1/yes and false/0/no.
 *
 * @param s  Input string (e.g. "TRUE", "false", "1", "0").
 * @return int  true/false on recognized values, 0 otherwise.
 */
static int parseBool(char* s) {
    if (s == NULL) return 0; /* default to false when missing */
    toLower(s);
    if (strcmp(s, "true") == 0 || strcmp(s, "1") == 0 || strcmp(s, "yes") == 0) return 1;
    if (strcmp(s, "false") == 0 || strcmp(s, "0") == 0 || strcmp(s, "no") == 0) return 0;
    printf("Warning: Unrecognized boolean value '%s'. Using FALSE.\n", s);
    return 0;
}

/**
 * @brief Parse and validate command-line arguments.
 *
 * Uses getopt_long to accept:
 *   --tty  <tty-path>     (optional)
 *   --baud <baud-string>  (optional)
 *   --hwflow <TRUE|FALSE> (optional)
 *   --help                (prints usage and returns 0)
 *
 * On parse/validation error this function prints usage (via printUsage)
 * and returns 0.
 *
 * @param argc  Argument count.
 * @param argv  Argument vector.
 * @return int  1 on success, zero on error.
 */
int parseArgs(int argc, char** argv) {
    static struct option longOpts[] = {
        {"tty",     optional_argument, NULL, 't'},
        {"baud",    optional_argument, NULL, 'b'},
        {"hwflow",  optional_argument, NULL, 'f'},
        {"help",    no_argument,       NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int opt;
    int longIndex = 0;
    while ((opt = getopt_long(argc, argv, "t:b:f:h", longOpts, &longIndex)) != -1) {
        switch (opt) {
        case 't':
            {
                char *val = optarg;
                if (!val && optind < argc && argv[optind][0] != '-') {
                    val = argv[optind++];
                }
                if (val) {
                    strncpy(serial_device.path, val, SERIAL_PATH_MAX - 1);
                    serial_device.path[SERIAL_PATH_MAX - 1] = '\0';
                } else {
                    serial_device.path[0] = '\0';
                }
            }
            break;
        case 'b': {
            {
                char *val = optarg;
                if (!val && optind < argc && argv[optind][0] != '-') {
                    val = argv[optind++];
                }
                if (val) {
                    int b = baudRateFromString(val);
                    serial_device.baud = b;
                }
            }
            break;
        }
        case 'f': {
            {
                char *val = optarg;
                if (!val && optind < argc && argv[optind][0] != '-') {
                    val = argv[optind++];
                }
                if (val) {
                    int pb = parseBool(val);
                    serial_device.hwflow = pb;
                }
            }
            break;
        }
        case 'h':
        default:
            printUsage(argv[0]);
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Program entry point.
 *
 * This function initializes the MCTP subsystem and platform hardware,
 * then enters the main loop which repeatedly updates the MCTP framer
 * and processes any available packets. Control and PLDM packets are
 * dispatched to their respective handlers; other packets are ignored.
 *
 * @return int Returns 0 on normal termination (never reached in typical
 *             embedded runtime where main runs indefinitely).
 */
int main(int argc, char *argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // get command line options
    if (!parseArgs(argc, argv)) return EXIT_FAILURE;

    if (serial_device.fd > -1) {
        printf("Using serial device: %s at baud %d, hwflow %s\n",
               serial_device.path,
               serial_device.baud,
               serial_device.hwflow ? "TRUE" : "FALSE");
    } else {
        printf("Using simulated pty device:\n");
    }

    /* initialize the mctp subsystem (and platform)*/
    mctp_init();

    while (!interrupted) {
        /* update the mctp framer state */
        mctp_update();

        /* process_packet */
        if (mctp_is_packet_available()) {
            if (mctp_is_control_packet()) {
                mctp_process_control_message();
            }
#ifdef PLDM_SUPPORT
            else if (mctp_is_pldm_packet()) {
                pldm_process_packet();
            }
#endif
            else {
                // non-control packet - drop packet
                mctp_ignore_packet();
            }
        }

        /* other application tasks can be added here */
    }

    // close the file descriptor if open
    if (serial_device.fd != -1) {
        close(serial_device.fd);
        serial_device.fd = -1;
    }

    return 0;
}
