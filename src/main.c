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
 * @brief MCTP receive message handler
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
		memcpy(emsg.data, msg, emsg.len);
	}

	msgqueue_put(&echo_q, &emsg);
}

/**
 * @brief Main function
 * Initializes MCTP over UART and enters the main processing loop.
 */
int main(void)
{
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
	mctp_register_bus(mctp_ctx, binding, 0x00);
	
	// set the default rx message handler
	mctp_set_rx_all(mctp_ctx, rx_message, NULL);

	// Initialize the PLDM processing module
	init_pldm();

	printf("MCTP endpoint ready on %s\n", serial_device.path);

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
		if (msgqueue_get(&echo_q, &em) == 0) {		
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

	// Cleanup (unreachable in current infinite loop)
	mctp_serial_destroy(serial);
	msgqueue_destroy(&echo_q);
	return 0;
}
