/**
 * @file config_loader.h
 * @brief Load JSON configuration and generate PDR/FRU data at runtime
 *
 * This module loads a JSON configuration file and uses the PDR and FRU
 * builder libraries to generate PLDM data structures dynamically.
 *
 * @author Doug Sandy
 * @date February 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief FRU field override for a single FRU record
 */
typedef struct {
    // Standard FRU fields (NULL = no override)
    const char* model;
    const char* serial_number;
    const char* part_number;
    const char* fru_file_id;
    const char* manufacturer;
    const char* name;
    const char* sku;
    const char* version;
    const char* asset_tag;
    const char* description;
    const char* engineering_change;
    const char* other;
    const char* mfg_date;          // ISO format (YYYY-MM-DD) or Unix timestamp
    
    // Vendor IANA (0 = no override)
    uint32_t vendor_iana;
    
    // OEM fields
    uint32_t oem_iana;             // 0 = use JSON default
    int oem_field_count;
    struct {
        char name[64];
        char value[256];
    } oem_fields[16];              // Up to 16 OEM override fields
    
    // Custom fields with explicit type
    int custom_field_count;
    struct {
        char type[16];             // "string", "binary", "uint32", "timestamp"
        char name[64];
        char value[256];
    } custom_fields[16];           // Up to 16 custom fields
    
    // Target record (by index or record set ID)
    int record_index;              // -1 = first record, 0+ = specific index
    uint16_t record_set_id;        // 0 = use index, non-zero = match RSI
} fru_record_override_t;

/**
 * @brief Container for all FRU overrides
 */
typedef struct {
    int override_count;
    fru_record_override_t overrides[8];  // Support up to 8 FRU record overrides
    
    // Global defaults (applied to all records if not overridden)
    const char* global_manufacturer;
    uint32_t global_vendor_iana;         // 0 = no override
} fru_overrides_t;

/**
 * @brief Runtime configuration data
 */
typedef struct {
    uint8_t* pdr_data;        /* Generated PDR data buffer */
    size_t pdr_data_size;     /* Size of PDR data in bytes */
    uint32_t pdr_record_count; /* Number of PDR records */
    
    uint8_t* fru_data;        /* Generated FRU data buffer */
    size_t fru_data_size;     /* Size of FRU data in bytes */
    uint32_t fru_record_count; /* Number of FRU records */
    
    bool loaded;              /* True if config was loaded successfully */
    char error_msg[256];      /* Error message if loading failed */
} runtime_config_t;

/**
 * @brief Load JSON configuration file and generate PDR/FRU data
 *
 * Reads the specified JSON file, parses it, and uses the PDR and FRU
 * builder libraries to generate runtime configuration data. The generated
 * data is allocated dynamically and must be freed by the caller.
 *
 * @param json_filename Path to JSON configuration file
 * @param config Pointer to runtime_config_t structure to populate
 * @return 0 on success, negative error code on failure
 */
int config_load_from_json(const char* json_filename, runtime_config_t* config);

/**
 * @brief Load JSON configuration with FRU field overrides
 *
 * Same as config_load_from_json() but allows command-line overrides of
 * FRU field values. Override values take precedence over JSON values.
 *
 * @param json_filename Path to JSON configuration file
 * @param config Pointer to runtime_config_t structure to populate
 * @param fru_overrides Pointer to FRU override structure (NULL = no overrides)
 * @return 0 on success, negative error code on failure
 */
int config_load_from_json_with_overrides(
    const char* json_filename,
    runtime_config_t* config,
    const fru_overrides_t* fru_overrides);

/**
 * @brief Initialize FRU overrides structure with defaults
 *
 * @param overrides Pointer to fru_overrides_t structure to initialize
 */
void fru_overrides_init(fru_overrides_t* overrides);

/**
 * @brief Free runtime configuration data
 *
 * Releases all dynamically allocated memory in the runtime configuration.
 *
 * @param config Pointer to runtime_config_t structure to free
 */
void config_free(runtime_config_t* config);

/**
 * @brief Get pointer to PDR data
 *
 * Returns a pointer to the generated PDR data buffer. This pointer remains
 * valid until config_free() is called.
 *
 * @param config Pointer to runtime_config_t structure
 * @return Pointer to PDR data, or NULL if not loaded
 */
const uint8_t* config_get_pdr_data(const runtime_config_t* config);

/**
 * @brief Get size of PDR data
 *
 * @param config Pointer to runtime_config_t structure
 * @return Size of PDR data in bytes, or 0 if not loaded
 */
size_t config_get_pdr_size(const runtime_config_t* config);

/**
 * @brief Get number of PDR records
 *
 * @param config Pointer to runtime_config_t structure
 * @return Number of PDR records, or 0 if not loaded
 */
uint32_t config_get_pdr_count(const runtime_config_t* config);

/**
 * @brief Get pointer to FRU data
 *
 * Returns a pointer to the generated FRU data buffer. This pointer remains
 * valid until config_free() is called.
 *
 * @param config Pointer to runtime_config_t structure
 * @return Pointer to FRU data, or NULL if not loaded
 */
const uint8_t* config_get_fru_data(const runtime_config_t* config);

/**
 * @brief Get size of FRU data
 *
 * @param config Pointer to runtime_config_t structure
 * @return Size of FRU data in bytes, or 0 if not loaded
 */
size_t config_get_fru_size(const runtime_config_t* config);

/**
 * @brief Get number of FRU records
 *
 * @param config Pointer to runtime_config_t structure
 * @return Number of FRU records, or 0 if not loaded
 */
uint32_t config_get_fru_count(const runtime_config_t* config);

#endif /* CONFIG_LOADER_H */
