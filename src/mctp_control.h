/**
 * @file mctp_control.h
 * @brief MCTP Control Message Handling Header
 * 
 * This header file defines the structures and function prototypes
 * for handling MCTP control messages in a Zephyr-based MCTP endpoint.
 * It includes definitions for control message types, completion codes,
 * and function declarations for processing control messages.
 * 
 * @author Doug Sandy
 * @date January 2026
 */
#ifndef MCTP_CONTROL_H
#define MCTP_CONTROL_H

#include <stdlib.h>
#ifdef MCTP_POSIX_UNIT_TEST
#include <stdint.h>
#include <stdbool.h>
#include "libmctp.h"
#include "mctp_internal.h"
/* Minimal logging macros for POSIX builds */
#include <stdio.h>
#define LOG_MODULE_REGISTER(name, level)
#define LOG_INF(...) printf(__VA_ARGS__); printf("\n")
#define LOG_DBG(...) printf(__VA_ARGS__); printf("\n")
#define LOG_WRN(...) printf(__VA_ARGS__); printf("\n")
#define LOG_ERR(...) printf(__VA_ARGS__); printf("\n")
#else
#include <libmctp.h>
#include <mctp_internal.h>
#endif

#define MCTP_CTRL_HDR_MSG_TYPE      0
#define MCTP_PLDM_HDR_MSG_TYPE      1

/* EID management macros - access libmctp internal bus structure */
#define GET_EID_FROM_MCTP(mctp) ((mctp)->busses[0].eid)
#define SET_EID_IN_MCTP(mctp, new_eid) ((mctp)->busses[0].eid = (new_eid))

/* control message codes */
#define CONTROL_MSG_SET_ENDPOINT_ID 0x01
#define CONTROL_MSG_GET_ENDPOINT_ID 0x02
#define CONTROL_MSG_GET_MCTP_VERSION_SUPPORT 0x04
#define CONTROL_MSG_GET_MESSAGE_TYPE_SUPPORT 0x05

/* Control message completion codes */
#define CONTROL_COMPLETE_SUCCESS 0x00
#define CONTROL_COMPLETE_ERROR 0x01
#define CONTROL_COMPLETE_INVALID_DATA 0x02
#define CONTROL_COMPLETE_INVALID_LENGTH 0x03
#define CONTROL_COMPLETE_NOT_READY 0x04
#define CONTROL_COMPLETE_UNSUPPORTED_CMD 0x05
#define CONTROL_COMPLETE_COMMAND_SPECIFIC_START 0x80
#define CONTROL_COMPLETE_COMMAND_SPECIFIC_END 0xFF

// structure for MCTP version entry
struct mctp_version_entry {
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t update_version;
    uint8_t alpha_version;
} __attribute__((packed));

// initialize the versions map
void initialize_versions_map(void);

// send a control message using MCTP
int send_control_message(struct mctp *mctp, uint8_t eid, const void *msg, size_t msg_len);

// send control completion response
void send_completion_response(struct mctp *mctp, uint8_t remote_eid, const void *msg, size_t len, uint8_t completion_code);

// add a supported version for a message type
int mctp_versions_map_add(uint8_t msg_type, const struct mctp_version_entry *ver);

// get the current endpoint ID from the MCTP instance.
uint8_t get_current_endpoint_id(struct mctp *mctp);

#endif /* MCTP_CONTROL_H */
