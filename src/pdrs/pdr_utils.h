/* PDR utility helpers
 * Provides helpers to compute repository byte length and parse records
 */
#ifndef PDR_UTILS_H
#define PDR_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Size of the PDR record header emitted by the builder */
#define PDR_HEADER_SIZE 10u

/* Return the total number of bytes present in __pdr_data[] on device
 * (payload bytes + per-record headers). */
size_t pdr_repo_bytes(void);

/* Read a record at `offset` within the repo. On success returns true and
 * writes the record handle and total record size (header+payload) into
 * `handle` and `record_size` respectively. */
bool pdr_read_record_at(size_t offset, uint32_t *handle, size_t *record_size);

#endif /* PDR_UTILS_H */
