/**
 * @file main.c
 * @brief Linux Endpoint for IoT-Foundry project
 * This is a simple MCTP endpoint implementation for Linux.
 * It uses MCTP over UART for communication and handles basic
 * MCTP control messages such as Get Endpoint ID.
 * @author Doug Sandy
 * @date February 2026
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <libmctp.h>
#include <libmctp-serial.h>
#include "config.h"
#include "platform.h"
#include "mctp_control.h"
#include "process_pldm.h"
#include "msgqueue.h"
#include "platform.h"

/* MIN macro */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Global serial device configuration */
config_t serial_device = {
	.baud = B115200,
	.hwflow = 0,
	.path = "",
	.fd = -1
};

/* Duplicate of libmctp control header structures/macros.
 * These are intentionally local copies for development/testing when
 * the upstream libmctp headers are not available on the include path.
 */
struct mctp_ctrl_msg_hdr {
	uint8_t ic_msg_type;
	uint8_t rq_dgram_inst;
	uint8_t command_code;
} __attribute__((packed));

#define MCTP_CTRL_HDR_MSG_TYPE	       0
#define MCTP_CTRL_HDR_FLAG_REQUEST     (1 << 7)
#define MCTP_CTRL_HDR_FLAG_DGRAM       (1 << 6)
#define MCTP_CTRL_HDR_INSTANCE_ID_MASK 0x1F

#define ECHO_Q_DEPTH 4
#define RX_BUF_SZ 256

struct echo_msg {
	uint8_t remote_eid;
	uint8_t destination_eid;
	bool tag_owner;
	uint8_t msg_tag;
	size_t len;
	uint8_t data[RX_BUF_SZ];
};

// message queue for received echo messages
static struct echo_msg echo_buffer[ECHO_Q_DEPTH];
static struct msgqueue echo_q;

/**
 * @brief Calculate CRC16-CCITT checksum for MCTP serial framing
 * Uses the same lookup table approach as the test script
 * Polynomial: 0x1021, Initial CRC: 0xFFFF
 * @param buf Pointer to data buffer
 * @param len Length of data in bytes
 * @return Calculated CRC16 value
 */
static uint16_t mctp_serial_crc16(const uint8_t *buf, size_t len)
{
	static const uint16_t fcstab[256] = {
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48, 0x9dc1,
		0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 0x1081, 0x0108, 0x3393, 0x221a,
		0x56a5, 0x472c, 0x75b7, 0x643e, 0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64,
		0xf9ff, 0xe876, 0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 0x3183, 0x200a,
		0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 0xbdcb, 0xac42, 0x9ed9, 0x8f50,
		0xfbef, 0xea66, 0xd8fd, 0xc974, 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9,
		0x2732, 0x36bb, 0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44,
		0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 0x6306, 0x728f, 0x4014, 0x519d,
		0x2522, 0x34ab, 0x0630, 0x17b9, 0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3,
		0x8a78, 0x9bf1, 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 0x8408, 0x9581,
		0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb,
		0x4e64, 0x5fed, 0x6d76, 0x7cff, 0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324,
		0xf1bf, 0xe036, 0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 0x2942, 0x38cb,
		0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 0xb58b, 0xa402, 0x9699, 0x8710,
		0xf3af, 0xe226, 0xd0bd, 0xc134, 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e,
		0x5cf5, 0x4d7c, 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 0xd68d, 0xc704,
		0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 0x5ac5, 0x4b4c, 0x79d7, 0x685e,
		0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3,
		0x8238, 0x93b1, 0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 0x7bc7, 0x6a4e,
		0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78,
	};
	
	uint16_t crc = 0xffff;
	size_t i;
	for (i = 0; i < len; i++) {
		crc = (crc >> 8) ^ fcstab[(crc ^ buf[i]) & 0xff];
	}
	return crc;
}

/**
 * @brief Calculate CRC16-CCITT checksum for MCTP serial framing
 * Replaces libmctp's serial TX function to add proper FCS calculation
 * @param b Pointer to the binding structure
 * @param pkt Pointer to the packet buffer to transmit
 * @return 0 on success, -1 on error
 */
static int custom_serial_tx_with_fcs(struct mctp_binding *b,
		struct mctp_pktbuf *pkt)
{
	struct mctp_binding_serial *serial = (struct mctp_binding_serial *)b;
	uint8_t txbuf[256];
	size_t txlen = 0;
	
	/* Get the unescaped MCTP packet data */
	uint8_t *mctp_data = (uint8_t *)mctp_pktbuf_hdr(pkt);
	size_t mctp_len = mctp_pktbuf_size(pkt);
	
	if (mctp_len > 250)
		return -1;
	
	/* Build frame: [flag][revision][length][data...][fcs_hi][fcs_lo][flag]
	 * But data needs to be escaped, so we'll build it step by step
	 */
	txbuf[txlen++] = 0x7E;  /* Start flag */
	txbuf[txlen++] = 0x01;  /* Revision */
	txbuf[txlen++] = mctp_len;  /* Length (unescaped packet size) */
	
	/* Calculate FCS over: revision + length + unescaped_data */
	uint8_t fcs_input[256];
	size_t fcs_input_len = 0;
	fcs_input[fcs_input_len++] = 0x01;  /* revision */
	fcs_input[fcs_input_len++] = mctp_len;  /* length */
	memcpy(fcs_input + fcs_input_len, mctp_data, mctp_len);
	fcs_input_len += mctp_len;
	
	uint16_t fcs = mctp_serial_crc16(fcs_input, fcs_input_len);
	
	/* Escape and add MCTP data */
	size_t i;
	for (i = 0; i < mctp_len; i++) {
		uint8_t c = mctp_data[i];
		if (c == 0x7E || c == 0x7D) {
			txbuf[txlen++] = 0x7D;
			txbuf[txlen++] = c ^ 0x20;
		} else {
			txbuf[txlen++] = c;
		}
		if (txlen >= sizeof(txbuf) - 4)
			return -1;
	}
	
	/* Add FCS bytes (not escaped, per MCTP serial spec) */
	txbuf[txlen++] = (fcs >> 8) & 0xFF;
	txbuf[txlen++] = fcs & 0xFF;
	
	/* End flag */
	txbuf[txlen++] = 0x7E;
	
	/* Send the frame */
	ssize_t ret = 0;
	size_t sent = 0;
	int fd = mctp_serial_get_fd(serial);
	while (sent < txlen) {
		ret = write(fd, txbuf + sent, txlen - sent);
		if (ret < 0)
			return -1;
		sent += ret;
	}
	
	return 0;
}

/**
 * @brief Custom TX function that replaces the default serial binding TX
 */
/**

 * Handles incoming MCTP messages by enqueueing them for processing.
 * @param remote_eid The source endpoint ID of the message sender.
 * @param data Pointer to user data (not used).
 * @param msg Pointer to the full message including header.
 * @param len Length of the message in bytes.
 */
static void rx_message(uint8_t remote_eid, void *data, void *msg, size_t len)
{
	struct echo_msg emsg;

	emsg.remote_eid = remote_eid;
	emsg.tag_owner = 0; /* will be set appropriately when replying */
	emsg.msg_tag = 0;
	emsg.len = MIN(len, (size_t)RX_BUF_SZ);
	if (msg && emsg.len) {
        printf("Received message from EID 0x%02X, length %zu\n", remote_eid, emsg.len);
        for (size_t i = 0; i < emsg.len; i++) {
            printf("%02X ", ((uint8_t *)msg)[i]);
        }
        printf("\n");
		memcpy(emsg.data, msg, emsg.len);
	}

	msgqueue_put(&echo_q, &emsg);
}

/**
 * @brief Convert baud rate integer to termios constant
 */
static speed_t get_baud_constant(int baud)
{
	switch (baud) {
	case 9600:   return B9600;
	case 19200:  return B19200;
	case 38400:  return B38400;
	case 57600:  return B57600;
	case 115200: return B115200;
	case 230400: return B230400;
	case 460800: return B460800;
	case 500000: return B500000;
	case 576000: return B576000;
	case 921600: return B921600;
	case 1000000: return B1000000;
	case 1152000: return B1152000;
	case 1500000: return B1500000;
	case 2000000: return B2000000;
	default:
		fprintf(stderr, "Unsupported baud rate: %d, using 115200\n", baud);
		return B115200;
	}
}

/**
 * @brief Print usage information
 */
static void print_usage(const char *progname)
{
	printf("Usage: %s [OPTIONS]\n", progname);
	printf("MCTP endpoint for Linux\n\n");
	printf("Options:\n");
	printf("  -p, --pty              Create a pseudo-terminal (default)\n");
	printf("  -d, --device <path>    Use specified serial device\n");
	printf("  -b, --baud <rate>      Set baud rate (default: 115200)\n");
	printf("                         Supported: 9600, 19200, 38400, 57600,\n");
	printf("                                    115200, 230400, 460800, 921600\n");
	printf("  -f, --flow-control     Enable hardware flow control (RTS/CTS)\n");
	printf("  -h, --help             Show this help message\n");
	printf("\nExamples:\n");
	printf("  %s -p                      # Create pty for testing\n", progname);
	printf("  %s -d /dev/ttyUSB0         # Use serial device\n", progname);
	printf("  %s -d /dev/ttyUSB0 -b 9600 # Use 9600 baud\n", progname);
	printf("  %s -d /dev/ttyUSB0 -f      # Enable flow control\n", progname);
}

/**
 * @brief Main function
 * Initializes MCTP over UART and enters the main processing loop.
 */
int main(int argc, char *argv[])
{
	int opt;
	int use_pty = 1; // Default to pty
	
	static struct option long_options[] = {
		{"pty", no_argument, 0, 'p'},
		{"device", required_argument, 0, 'd'},
		{"baud", required_argument, 0, 'b'},
		{"flow-control", no_argument, 0, 'f'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
	};
	
	// Parse command-line options
	while ((opt = getopt_long(argc, argv, "pd:b:fh", long_options, NULL)) != -1) {
		switch (opt) {
		case 'p':
			use_pty = 1;
			serial_device.path[0] = '\0'; // Empty path signals pty creation
			break;
		case 'd':
			use_pty = 0;
			strncpy(serial_device.path, optarg, SERIAL_PATH_MAX - 1);
			serial_device.path[SERIAL_PATH_MAX - 1] = '\0';
			break;
		case 'b':
			serial_device.baud = get_baud_constant(atoi(optarg));
			break;
		case 'f':
			serial_device.hwflow = 1;
			break;
		case 'h':
			print_usage(argv[0]);
			return 0;
		default:
			print_usage(argv[0]);
			return 1;
		}
	}
	
	printf("mctp_endpoint: main() start\n");

	// Initialize message queue
	if (msgqueue_init(&echo_q, echo_buffer, sizeof(struct echo_msg), ECHO_Q_DEPTH) != 0) {
		fprintf(stderr, "Failed to initialize message queue\n");
		return 1;
	}

	mctp_set_alloc_ops(malloc, free, realloc);
	
	// initialize MCTP
	struct mctp *mctp_ctx = mctp_init();
	assert(mctp_ctx != NULL);

	// Initialize the versions map (versions of supported MCTP message types) 
	initialize_versions_map();

	// Initialize platform and open serial port
	platform_init();

	// Create and configure serial binding
	struct mctp_binding_serial *serial = mctp_serial_init();
	if (!serial) {
		fprintf(stderr, "Failed to initialize MCTP serial binding\n");
		return 1;
	}

	// Use the FD from platform initialization
	extern config_t serial_device;
	if (serial_device.fd < 0) {
		fprintf(stderr, "Serial device not initialized\n");
		return 1;
	}
	mctp_serial_open_fd(serial, serial_device.fd);

	// Register the serial binding with MCTP context (EID 0x00 = unconfigured)
	struct mctp_binding *binding = mctp_binding_serial_core(serial);
	
	/* Replace the TX function with our custom one that adds proper FCS */
	binding->tx = custom_serial_tx_with_fcs;
	
	mctp_register_bus(mctp_ctx, binding, 0x00);
	
	// set the default rx message handler
	mctp_set_rx_all(mctp_ctx, rx_message, NULL);

	// Initialize the PLDM processing module
	init_pldm();

	printf("MCTP endpoint ready on %s\n", serial_device.path);
	fflush(stdout);

	// Main event loop with select()
	int serial_fd = mctp_serial_get_fd(serial);
	struct echo_msg em;
	
	while (1) {
		fd_set readfds;
		struct timeval tv;
		
		FD_ZERO(&readfds);
		FD_SET(serial_fd, &readfds);
		
		// Small timeout to periodically check message queue
		tv.tv_sec = 0;
		tv.tv_usec = 10000; // 10ms
		
		int ret = select(serial_fd + 1, &readfds, NULL, NULL, &tv);
		
		if (ret < 0) {
			perror("select");
			break;
		} else if (ret > 0 && FD_ISSET(serial_fd, &readfds)) {
			// Data available on serial port - read and process
			mctp_serial_read(serial);
		}
		
		// Process any queued messages (non-blocking)
		if (msgqueue_tryget(&echo_q, &em) == 0) {		
			const struct mctp_ctrl_msg_hdr *hdr = (const struct mctp_ctrl_msg_hdr *)em.data;
			if ((em.len >= sizeof(struct mctp_ctrl_msg_hdr)) && 
				(hdr->ic_msg_type == MCTP_CTRL_HDR_MSG_TYPE)) {
				// this is a control message - process it
				int ret = send_control_message(mctp_ctx, em.remote_eid, em.data, em.len);
				if (ret) {
					printf("send_control_message failed: %d\n", ret);
				}
			} 

            // process pldm messages if libpldm is included
			else if ((em.len >= sizeof(struct mctp_ctrl_msg_hdr)) && 
				(hdr->ic_msg_type == MCTP_PLDM_HDR_MSG_TYPE)) {
				// this is a PLDM message - process it
				handle_pldm_message(mctp_ctx, em.remote_eid, em.data, em.len);
			}

            else {
				printf("unknown message type, dropping\n");
				continue;
			}			
		}
	}

	// Cleanup
	printf("\nShutting down...\n");
	
	// Close serial device
	if (serial_device.fd >= 0) {
		printf("Closing serial device fd=%d\n", serial_device.fd);
		close(serial_device.fd);
		serial_device.fd = -1;
	}
	
	mctp_serial_destroy(serial);
	msgqueue_destroy(&echo_q);
	
	printf("Shutdown complete\n");
	return 0;
}
