#include "pdr_utils.h"
#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

size_t pdr_repo_bytes(void)
{
    return (size_t)PDR_TOTAL_SIZE + ((size_t)PDR_NUMBER_OF_RECORDS * PDR_HEADER_SIZE);
}

bool pdr_read_record_at(size_t offset, uint32_t *handle, size_t *record_size)
{
    size_t repo = pdr_repo_bytes();
    if (offset + PDR_HEADER_SIZE > repo) return false;

    const uint8_t *base = (const uint8_t *)__pdr_data;
    uint32_t h = (uint32_t)base[offset] | ((uint32_t)base[offset+1] << 8) | ((uint32_t)base[offset+2] << 16) | ((uint32_t)base[offset+3] << 24);
    uint16_t len = (uint16_t)base[offset + 8] | ((uint16_t)base[offset + 9] << 8);
    size_t recsz = (size_t)len + PDR_HEADER_SIZE;
    if (offset + recsz > repo) return false;
    if (handle) *handle = h;
    if (record_size) *record_size = recsz;
    return true;
}
