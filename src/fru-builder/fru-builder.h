/**
 * @file fru-builder.h
 * @brief Runtime library for building PLDM FRU Record Table data
 *
 * This header provides functions to dynamically generate PLDM FRU records
 * into a buffer at runtime. The format follows DSP0257 (PLDM for FRU Data).
 *
 * FRU Record Table Format (per DSP0257):
 * - FRU Record Set Identifier (2 bytes, uint16)
 * - FRU Record Type (1 byte)
 * - Number of FRU Fields (1 byte)
 * - Encoding Type (1 byte)
 * - FRU Fields: Type (1 byte) + Length (1 byte) + Value (0-255 bytes)
 *
 * @author Doug Sandy
 * @date February 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FRU_BUILDER_H
#define FRU_BUILDER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FRU Record Types (DSP0257 Table 3) */
#define FRU_RECORD_TYPE_GENERAL   1
#define FRU_RECORD_TYPE_OEM       254

/* FRU Encoding Types (DSP0257 Table 1) */
#define FRU_ENCODING_UNSPECIFIED  0
#define FRU_ENCODING_ASCII        1
#define FRU_ENCODING_UTF8         2
#define FRU_ENCODING_UTF16        3
#define FRU_ENCODING_UTF16LE      4
#define FRU_ENCODING_UTF16BE      5

/* General FRU Field Types (DSP0257 Table 4) */
#define FRU_FIELD_CHASSIS_TYPE    1
#define FRU_FIELD_MODEL           2
#define FRU_FIELD_PART_NUMBER     3
#define FRU_FIELD_SERIAL_NUMBER   4
#define FRU_FIELD_MANUFACTURER    5
#define FRU_FIELD_MANUFACTURE_DATE 6  /* timestamp104, 13 bytes */
#define FRU_FIELD_VENDOR          7
#define FRU_FIELD_NAME            8
#define FRU_FIELD_SKU             9
#define FRU_FIELD_VERSION         10
#define FRU_FIELD_ASSET_TAG       11
#define FRU_FIELD_DESCRIPTION     12
#define FRU_FIELD_ECL             13  /* Engineering Change Level */
#define FRU_FIELD_OTHER_INFO      14
#define FRU_FIELD_VENDOR_IANA     15  /* uint32, 4 bytes */
#define FRU_FIELD_SPARE_PART_NUM  16

/* OEM FRU Field Types (DSP0257 Table 5) */
#define FRU_FIELD_OEM_VENDOR_IANA 1   /* Mandatory first field for OEM records */
#define FRU_FIELD_OEM_DEFINED_MIN 2
#define FRU_FIELD_OEM_DEFINED_MAX 254

/**
 * @brief FRU field descriptor for building records
 */
typedef struct {
    uint8_t type;        /* Field type from FRU_FIELD_* defines */
    uint8_t length;      /* Field length in bytes (0-255) */
    const uint8_t* data; /* Pointer to field data */
} fru_field_t;

/**
 * @brief FRU builder context
 */
typedef struct {
    uint8_t* buffer;           /* Buffer to write FRU data into */
    size_t buffer_size;        /* Total size of buffer */
    size_t offset;             /* Current write offset */
    uint16_t next_record_set_id; /* Next Record Set Identifier */
    uint32_t record_count;     /* Number of records written */
} fru_builder_t;

/**
 * @brief Initialize FRU builder
 *
 * @param builder Pointer to FRU builder context
 * @param buffer Buffer to write FRU records into
 * @param buffer_size Size of buffer in bytes
 * @param start_record_set_id Starting Record Set Identifier (typically 1)
 */
void fru_builder_init(fru_builder_t* builder, uint8_t* buffer, 
                      size_t buffer_size, uint16_t start_record_set_id);

/**
 * @brief Get current buffer offset (total bytes written)
 *
 * @param builder Pointer to FRU builder context
 * @return Number of bytes written to buffer
 */
size_t fru_builder_get_offset(const fru_builder_t* builder);

/**
 * @brief Get number of records written
 *
 * @param builder Pointer to FRU builder context
 * @return Number of FRU records written
 */
uint32_t fru_builder_get_record_count(const fru_builder_t* builder);

/**
 * @brief Build a General FRU Record
 *
 * Builds a complete FRU record with the specified fields. The encoding type
 * applies to all string fields in the record.
 *
 * Mandatory fields (per DSP0257): Model, Part Number, Serial Number, 
 * Manufacturer, Manufacture Date
 *
 * @param builder Pointer to FRU builder context
 * @param encoding_type Encoding type (FRU_ENCODING_*)
 * @param fields Array of field descriptors
 * @param num_fields Number of fields in array
 * @return Number of bytes written, or 0 on error
 */
size_t fru_build_general_record(fru_builder_t* builder, uint8_t encoding_type,
                                const fru_field_t* fields, uint8_t num_fields);

/**
 * @brief Build an OEM FRU Record
 *
 * Builds an OEM FRU record. The first field MUST be the Vendor IANA
 * (FRU_FIELD_OEM_VENDOR_IANA, 4 bytes). Remaining fields are OEM-defined.
 *
 * @param builder Pointer to FRU builder context
 * @param encoding_type Encoding type (FRU_ENCODING_*)
 * @param vendor_iana 4-byte IANA enterprise number (e.g., 12634 for PICMG)
 * @param oem_fields Array of OEM-defined field descriptors (types 2-254)
 * @param num_oem_fields Number of OEM fields (not counting IANA field)
 * @return Number of bytes written, or 0 on error
 */
size_t fru_build_oem_record(fru_builder_t* builder, uint8_t encoding_type,
                            uint32_t vendor_iana, const fru_field_t* oem_fields,
                            uint8_t num_oem_fields);

/**
 * @brief Helper: Create a string field descriptor
 *
 * Creates a field descriptor for a string field. The string should NOT be
 * null-terminated (or if it is, the null will be included in the length).
 *
 * @param field Pointer to field descriptor to fill
 * @param type Field type
 * @param str Pointer to string data
 * @param length String length in bytes (max 255)
 */
void fru_field_string(fru_field_t* field, uint8_t type, 
                      const char* str, uint8_t length);

/**
 * @brief Helper: Create a uint32 field descriptor (e.g., Vendor IANA)
 *
 * Creates a field descriptor for a 4-byte uint32 value.
 *
 * @param field Pointer to field descriptor to fill
 * @param type Field type
 * @param value_le_bytes Pointer to 4-byte value in little-endian format
 */
void fru_field_uint32(fru_field_t* field, uint8_t type, 
                      const uint8_t* value_le_bytes);

/**
 * @brief Helper: Create a timestamp104 field descriptor (Manufacture Date)
 *
 * Creates a field descriptor for a 13-byte timestamp104 value.
 * Format per DSP0240: 13 bytes representing date/time
 *
 * @param field Pointer to field descriptor to fill
 * @param timestamp_bytes Pointer to 13-byte timestamp104 data
 */
void fru_field_timestamp(fru_field_t* field, const uint8_t* timestamp_bytes);

/**
 * @brief Helper: Create a binary data field descriptor
 *
 * Creates a field descriptor for arbitrary binary data.
 *
 * @param field Pointer to field descriptor to fill
 * @param type Field type
 * @param data Pointer to binary data
 * @param length Data length in bytes (max 255)
 */
void fru_field_binary(fru_field_t* field, uint8_t type,
                      const uint8_t* data, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif /* FRU_BUILDER_H */
