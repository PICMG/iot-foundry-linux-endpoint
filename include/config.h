/*
 * serial.h
 * Small struct to represent a serial device: baud rate, device path, and file descriptor.
 */
#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum path length fallback */
#ifndef SERIAL_PATH_MAX
# ifdef PATH_MAX
#  define SERIAL_PATH_MAX PATH_MAX
# else
#  define SERIAL_PATH_MAX 1024
# endif
#endif

typedef struct {
    int baud;                 /* integer baud rate (e.g., 115200) */
    int hwflow;               /* hardware flow control enabled (1) or disabled (0) */
    char path[SERIAL_PATH_MAX];    /* null-terminated device path */
    int fd;                        /* POSIX file descriptor for the device, -1 if closed */
} config_t;

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
