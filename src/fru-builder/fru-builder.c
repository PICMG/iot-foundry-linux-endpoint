/**
 * @file fru-builder.c
 * @brief Runtime library for building PLDM FRU Record Table data
 *
 * Implementation of functions to dynamically generate PLDM FRU records
 * into a buffer at runtime, following DSP0257 (PLDM for FRU Data).
 *
 * @author Doug Sandy
 * @date February 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#include "fru-builder.h"
#include <string.h>

/* Internal helper: Write little-endian uint16 */
static void write_le16(uint8_t* buf, uint16_t value)
{
    buf[0] = value & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
}

/* Internal helper: Write little-endian uint32 */
static void write_le32(uint8_t* buf, uint32_t value)
{
    buf[0] = value & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
    buf[2] = (value >> 16) & 0xFF;
    buf[3] = (value >> 24) & 0xFF;
}

void fru_builder_init(fru_builder_t* builder, uint8_t* buffer, 
                      size_t buffer_size, uint16_t start_record_set_id)
{
    if (!builder) return;
    
    builder->buffer = buffer;
    builder->buffer_size = buffer_size;
    builder->offset = 0;
    builder->next_record_set_id = start_record_set_id;
    builder->record_count = 0;
}

size_t fru_builder_get_offset(const fru_builder_t* builder)
{
    return builder ? builder->offset : 0;
}

uint32_t fru_builder_get_record_count(const fru_builder_t* builder)
{
    return builder ? builder->record_count : 0;
}

/**
 * @brief Internal helper to write FRU record header
 *
 * Writes the FRU Record Set Identifier, Record Type, Number of Fields, 
 * and Encoding Type.
 *
 * @return Number of bytes written (5), or 0 on error
 */
static size_t write_fru_record_header(fru_builder_t* builder, 
                                      uint8_t record_type,
                                      uint8_t num_fields,
                                      uint8_t encoding_type)
{
    if (!builder || !builder->buffer) return 0;
    
    /* Check buffer space: 2 (RSI) + 1 (type) + 1 (num) + 1 (enc) = 5 bytes */
    if (builder->offset + 5 > builder->buffer_size) return 0;
    
    uint8_t* ptr = builder->buffer + builder->offset;
    
    /* FRU Record Set Identifier (2 bytes, little-endian) */
    write_le16(ptr, builder->next_record_set_id);
    ptr += 2;
    
    /* FRU Record Type (1 byte) */
    *ptr++ = record_type;
    
    /* Number of FRU Fields (1 byte) */
    *ptr++ = num_fields;
    
    /* Encoding Type (1 byte) */
    *ptr++ = encoding_type;
    
    builder->offset += 5;
    builder->next_record_set_id++;
    builder->record_count++;
    
    return 5;
}

/**
 * @brief Internal helper to write a single FRU field
 *
 * Writes Type + Length + Value for one field.
 *
 * @return Number of bytes written, or 0 on error
 */
static size_t write_fru_field(fru_builder_t* builder, const fru_field_t* field)
{
    if (!builder || !builder->buffer || !field) return 0;
    if (!field->data && field->length > 0) return 0;
    
    /* Check buffer space: 1 (type) + 1 (length) + length (data) */
    size_t needed = 2 + field->length;
    if (builder->offset + needed > builder->buffer_size) return 0;
    
    uint8_t* ptr = builder->buffer + builder->offset;
    
    /* Field Type (1 byte) */
    *ptr++ = field->type;
    
    /* Field Length (1 byte) */
    *ptr++ = field->length;
    
    /* Field Value (0-255 bytes) */
    if (field->length > 0) {
        memcpy(ptr, field->data, field->length);
        ptr += field->length;
    }
    
    builder->offset += needed;
    return needed;
}

size_t fru_build_general_record(fru_builder_t* builder, uint8_t encoding_type,
                                const fru_field_t* fields, uint8_t num_fields)
{
    if (!builder || !fields || num_fields == 0) return 0;
    
    size_t start_offset = builder->offset;
    
    /* Write record header */
    if (write_fru_record_header(builder, FRU_RECORD_TYPE_GENERAL, 
                                num_fields, encoding_type) == 0) {
        return 0;
    }
    
    /* Write each field */
    for (uint8_t i = 0; i < num_fields; i++) {
        if (write_fru_field(builder, &fields[i]) == 0) {
            /* Rollback on error */
            builder->offset = start_offset;
            builder->next_record_set_id--;
            builder->record_count--;
            return 0;
        }
    }
    
    return builder->offset - start_offset;
}

size_t fru_build_oem_record(fru_builder_t* builder, uint8_t encoding_type,
                            uint32_t vendor_iana, const fru_field_t* oem_fields,
                            uint8_t num_oem_fields)
{
    if (!builder) return 0;
    
    size_t start_offset = builder->offset;
    
    /* Total fields = 1 (IANA) + num_oem_fields */
    uint8_t total_fields = 1 + num_oem_fields;
    
    /* Write record header */
    if (write_fru_record_header(builder, FRU_RECORD_TYPE_OEM, 
                                total_fields, encoding_type) == 0) {
        return 0;
    }
    
    /* Write Vendor IANA field (mandatory first field) */
    uint8_t iana_bytes[4];
    write_le32(iana_bytes, vendor_iana);
    
    fru_field_t iana_field;
    iana_field.type = FRU_FIELD_OEM_VENDOR_IANA;
    iana_field.length = 4;
    iana_field.data = iana_bytes;
    
    if (write_fru_field(builder, &iana_field) == 0) {
        /* Rollback */
        builder->offset = start_offset;
        builder->next_record_set_id--;
        builder->record_count--;
        return 0;
    }
    
    /* Write OEM-defined fields */
    for (uint8_t i = 0; i < num_oem_fields; i++) {
        if (write_fru_field(builder, &oem_fields[i]) == 0) {
            /* Rollback */
            builder->offset = start_offset;
            builder->next_record_set_id--;
            builder->record_count--;
            return 0;
        }
    }
    
    return builder->offset - start_offset;
}

void fru_field_string(fru_field_t* field, uint8_t type, 
                      const char* str, uint8_t length)
{
    if (!field) return;
    
    field->type = type;
    field->length = length;
    field->data = (const uint8_t*)str;
}

void fru_field_uint32(fru_field_t* field, uint8_t type, 
                      const uint8_t* value_le_bytes)
{
    if (!field) return;
    
    field->type = type;
    field->length = 4;
    field->data = value_le_bytes;
}

void fru_field_timestamp(fru_field_t* field, const uint8_t* timestamp_bytes)
{
    if (!field) return;
    
    field->type = FRU_FIELD_MANUFACTURE_DATE;
    field->length = 13;
    field->data = timestamp_bytes;
}

void fru_field_binary(fru_field_t* field, uint8_t type,
                      const uint8_t* data, uint8_t length)
{
    if (!field) return;
    
    field->type = type;
    field->length = length;
    field->data = data;
}
