/**
 * @file mctp_internal.h
 * @brief Internal libmctp structure definitions
 * 
 * This header provides definitions for internal libmctp structures that are
 * normally opaque. These definitions are copied from libmctp/core.c to allow
 * direct access to the EID field for MCTP control message handling.
 * 
 * WARNING: These structures are internal to libmctp and may change in future
 * versions. Use with caution.
 * 
 * @author Doug Sandy
 * @date February 2026
 */
#ifndef MCTP_INTERNAL_H
#define MCTP_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <libmctp.h>

/* Forward declarations */
struct mctp_binding;
struct mctp_pktbuf;

/* Internal libmctp structures - copied from libmctp/core.c v0.11 */

enum mctp_bus_state {
	mctp_bus_state_constructed = 0,
	mctp_bus_state_tx_enabled,
	mctp_bus_state_tx_disabled,
};

struct mctp_bus {
	mctp_eid_t eid;
	struct mctp_binding *binding;
	enum mctp_bus_state state;

	struct mctp_pktbuf *tx_queue_head;
	struct mctp_pktbuf *tx_queue_tail;

	/* todo: routing */
};

struct mctp_msg_ctx {
	uint8_t		src;
	uint8_t		dest;
	uint8_t		tag;
	uint8_t		last_seq;
	void		*buf;
	size_t		buf_size;
	size_t		buf_alloc_size;
	size_t		fragment_size;
};

struct mctp {
	int			n_busses;
	struct mctp_bus		*busses;

	/* Message RX callback */
	mctp_rx_fn		message_rx;
	void			*message_rx_data;

	/* Packet capture callback */
	mctp_capture_fn		capture;
	void			*capture_data;

	/* Message reassembly.
	 * @todo: flexible context count
	 */
	struct mctp_msg_ctx	msg_ctxs[16];

	enum {
		ROUTE_ENDPOINT,
		ROUTE_BRIDGE,
	}			route_policy;
	size_t max_message_size;
};

#endif /* MCTP_INTERNAL_H */
