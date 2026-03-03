/**
 * PDR Builder Library - Runtime PDR Generation
 * 
 * Provides functions to generate PLDM PDRs into buffers at runtime.
 * All functions write PDRs with 10-byte headers matching config.c format.
 */

#ifndef PDR_BUILDER_H
#define PDR_BUILDER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// PDR Types (DSP0248)
#define PDR_TYPE_TERMINUS_LOCATOR       0x01
#define PDR_TYPE_NUMERIC_SENSOR         0x02
#define PDR_TYPE_STATE_SENSOR           0x04
#define PDR_TYPE_OEM_STATE_SET          0x08
#define PDR_TYPE_NUMERIC_EFFECTER       0x09
#define PDR_TYPE_STATE_EFFECTER         0x0B
#define PDR_TYPE_ENTITY_ASSOCIATION     0x0F
#define PDR_TYPE_OEM_ENTITY_ID          0x11
#define PDR_TYPE_FRU_RECORD_SET         0x14

// PDR Header size (config.c format)
#define PDR_HEADER_SIZE 10

// Helper structure for building PDRs
typedef struct {
    uint8_t *buffer;
    size_t max_size;
    size_t offset;
    uint32_t record_handle;
} pdr_builder_t;

/**
 * Initialize PDR builder with output buffer
 * @param builder Pointer to builder instance
 * @param buffer Output buffer for PDR data
 * @param max_size Maximum size of output buffer
 * @param start_handle Starting record handle
 */
void pdr_builder_init(pdr_builder_t *builder, uint8_t *buffer, size_t max_size, uint32_t start_handle);

/**
 * Get current offset (total bytes written)
 */
size_t pdr_builder_get_offset(const pdr_builder_t *builder);

/**
 * Get number of records written
 */
uint32_t pdr_builder_get_record_count(const pdr_builder_t *builder);

/**
 * Generate Terminus Locator PDR (Type 0x01)
 * @param builder PDR builder context
 * @param tid Terminus ID (EID)
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_terminus_locator(pdr_builder_t *builder, uint8_t tid);

/**
 * Generate FRU Record Set PDR (Type 0x14)
 * @param builder PDR builder context
 * @param fru_rsi FRU Record Set Identifier
 * @param entity_type Entity Type (e.g., 0x0050 for generic device)
 * @param entity_instance Entity Instance Number
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_fru_record_set(pdr_builder_t *builder, 
                                 uint16_t fru_rsi,
                                 uint16_t entity_type,
                                 uint16_t entity_instance);

/**
 * Generate Entity Association PDR (Type 0x0F)
 * @param builder PDR builder context
 * @param container_type Container entity type
 * @param container_instance Container entity instance
 * @param contained_type Contained entity type
 * @param contained_instance Contained entity instance
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_entity_association(pdr_builder_t *builder,
                                     uint16_t container_type,
                                     uint16_t container_instance,
                                     uint16_t contained_type,
                                     uint16_t contained_instance);

/**
 * Generate OEM Entity ID PDR (Type 0x11)
 * @param builder PDR builder context
 * @param entity_instance Entity instance number
 * @param vendor_iana IANA Enterprise ID (e.g., 12634 for PICMG)
 * @param vendor_entity_id Vendor-specific entity ID
 * @param name Entity name (UTF-8 string, will be converted to UTF-16LE)
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_oem_entity_id(pdr_builder_t *builder,
                                uint16_t oem_entity_id_handle,
                                uint32_t vendor_iana,
                                uint16_t vendor_entity_id,
                                const char *name);

/**
 * Generate OEM State Set PDR (Type 0x08)
 * @param builder PDR builder context
 * @param state_set_id OEM State Set ID
 * @param vendor_iana IANA Enterprise ID
 * @param num_states Number of state value records
 * @param state_values Array of state values (min, max pairs)
 * @param state_names Array of state name strings
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_oem_state_set(pdr_builder_t *builder,
                                uint16_t state_set_id,
                                uint32_t vendor_iana,
                                uint8_t num_states,
                                const uint8_t *state_values,  // min,max pairs
                                const char **state_names);

/**
 * Generate State Sensor PDR (Type 0x04)
 * @param builder PDR builder context
 * @param sensor_id Sensor ID
 * @param entity_type Entity Type
 * @param entity_instance Entity Instance
 * @param state_set State Set ID (OEM bit set if vendor-specific)
 * @param possible_states Bitmask of possible states
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_state_sensor(pdr_builder_t *builder,
                               uint16_t sensor_id,
                               uint16_t entity_type,
                               uint16_t entity_instance,
                               uint16_t state_set,
                               uint8_t possible_states);

/**
 * Generate State Effecter PDR (Type 0x0B)
 * @param builder PDR builder context
 * @param effecter_id Effecter ID
 * @param entity_type Entity Type
 * @param entity_instance Entity Instance
 * @param state_set State Set ID (OEM bit set if vendor-specific)
 * @param possible_states Bitmask of possible states
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_state_effecter(pdr_builder_t *builder,
                                 uint16_t effecter_id,
                                 uint16_t entity_type,
                                 uint16_t entity_instance,
                                 uint16_t state_set,
                                 uint8_t possible_states);

/**
 * Generate Numeric Sensor PDR (Type 0x02) - Simplified version
 * @param builder PDR builder context
 * @param sensor_id Sensor ID
 * @param entity_type Entity Type
 * @param entity_instance Entity Instance
 * @param base_unit Physical base unit
 * @param unit_modifier Unit modifier
 * @param rate_unit Rate unit
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_numeric_sensor(pdr_builder_t *builder,
                                 uint16_t sensor_id,
                                 uint16_t entity_type,
                                 uint16_t entity_instance,
                                 uint8_t base_unit,
                                 int8_t unit_modifier,
                                 uint8_t rate_unit);

/**
 * Generate Numeric Effecter PDR (Type 0x09) - Simplified version
 * @param builder PDR builder context
 * @param effecter_id Effecter ID
 * @param entity_type Entity Type
 * @param entity_instance Entity Instance
 * @param base_unit Physical base unit
 * @param unit_modifier Unit modifier
 * @param rate_unit Rate unit
 * @return Number of bytes written, or 0 on error
 */
size_t pdr_build_numeric_effecter(pdr_builder_t *builder,
                                   uint16_t effecter_id,
                                   uint16_t entity_type,
                                   uint16_t entity_instance,
                                   uint8_t base_unit,
                                   int8_t unit_modifier,
                                   uint8_t rate_unit);

#endif /* PDR_BUILDER_H */
