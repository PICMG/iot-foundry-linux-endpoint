/**
 * PDR Builder Library - Runtime PDR Generation Implementation
 */

#include "pdr-builder.h"
#include <string.h>
#include <stdio.h>

// Helper to write multi-byte values in little-endian
static void write_le16(uint8_t *buf, uint16_t val) {
    buf[0] = val & 0xFF;
    buf[1] = (val >> 8) & 0xFF;
}

static void write_le32(uint8_t *buf, uint32_t val) {
    buf[0] = val & 0xFF;
    buf[1] = (val >> 8) & 0xFF;
    buf[2] = (val >> 16) & 0xFF;
    buf[3] = (val >> 24) & 0xFF;
}

static size_t write_utf16be_string(uint8_t *buf, const char *str, size_t max_len) {
    size_t len = strlen(str);
    size_t offset = 0;

    for (size_t i = 0; i < len && offset + 2 < max_len; i++) {
        buf[offset++] = 0x00;
        buf[offset++] = (uint8_t)str[i];  // UTF-16 BE
    }
    if (offset + 2 <= max_len) {
        buf[offset++] = 0x00;
        buf[offset++] = 0x00;  // Null terminator
    }
    return offset;
}

// Write PLDM PDR header (10 bytes)
static size_t write_pdr_header(uint8_t *buf, uint32_t handle, uint8_t type, uint16_t length) {
    size_t offset = 0;
    write_le32(&buf[offset], handle);
    offset += 4;
    buf[offset++] = 0x01;  // Version
    buf[offset++] = type;  // PDR Type
    write_le16(&buf[offset], 0x0000);  // Record Change Number
    offset += 2;
    write_le16(&buf[offset], length); // Record Length (payload bytes)
    offset += 2;
    return offset;
}

void pdr_builder_init(pdr_builder_t *builder, uint8_t *buffer, size_t max_size, uint32_t start_handle) {
    builder->buffer = buffer;
    builder->max_size = max_size;
    builder->offset = 0;
    builder->record_handle = start_handle;
}

size_t pdr_builder_get_offset(const pdr_builder_t *builder) {
    return builder->offset;
}

uint32_t pdr_builder_get_record_count(const pdr_builder_t *builder) {
    return builder->record_handle - 1;
}

size_t pdr_build_terminus_locator(pdr_builder_t *builder, uint8_t tid) {
    const uint16_t pdr_length = 9;
    const size_t total_size = PDR_HEADER_SIZE + pdr_length;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;  // Not enough space
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    size_t offset = 0;

    // Write header
    offset += write_pdr_header(buf, builder->record_handle++, PDR_TYPE_TERMINUS_LOCATOR, pdr_length);

    // Write PDR payload directly into buf
    write_le16(&buf[offset], 0x0001);           // PLDMTerminusHandle
    offset += 2;
    buf[offset++] = 0x01;                       // Validity: Valid
    buf[offset++] = tid;                        // TID
    write_le16(&buf[offset], 0x0000);           // ContainerID (overall system)
    offset += 2;
    buf[offset++] = 0x01;                       // Terminus Locator Type: MCTP_EID
    buf[offset++] = 0x01;                       // Terminus Locator Value Length
    buf[offset++] = tid;                        // Terminus Locator Value (EID)

    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_fru_record_set(pdr_builder_t *builder, 
                                uint16_t fru_rsi,
                                uint16_t entity_type,
                                uint16_t entity_instance) {
    const uint16_t pdr_length = 10;
    const size_t total_size = PDR_HEADER_SIZE + pdr_length;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    size_t offset = 0;

    // Write header
    offset += write_pdr_header(buf, builder->record_handle++, PDR_TYPE_FRU_RECORD_SET, pdr_length);

    // Write PDR payload directly into buf
    write_le16(&buf[offset], 0x0001);           // PLDMTerminusHandle
    offset += 2;
    write_le16(&buf[offset], fru_rsi);          // FRU Record Set ID
    offset += 2;
    write_le16(&buf[offset], entity_type);      // Entity Type
    offset += 2;
    write_le16(&buf[offset], entity_instance);  // Entity Instance
    offset += 2;
    write_le16(&buf[offset], 0x0000);           // Container ID
    offset += 2;

    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_entity_association(pdr_builder_t *builder,
                                    uint16_t container_type,
                                    uint16_t container_instance,
                                    uint16_t contained_type,
                                    uint16_t contained_instance) {
    const uint16_t pdr_length = 16;
    const size_t total_size = PDR_HEADER_SIZE + pdr_length;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    size_t offset = 0;

    // Write header
    offset += write_pdr_header(buf, builder->record_handle++, PDR_TYPE_ENTITY_ASSOCIATION, pdr_length);

    // Write PDR payload directly into buf
    write_le16(&buf[offset], 0x0001);            // Container ID
    offset += 2;
    buf[offset++] = 0x01;                        // Association Type: Logical Containment
    write_le16(&buf[offset], container_type);    // Container Entity Type
    offset += 2;
    write_le16(&buf[offset], container_instance);// Container Entity Instance
    offset += 2;
    write_le16(&buf[offset], 0x0000);            // Container Entity Container ID
    offset += 2;
    buf[offset++] = 0x01;                        // Number of children
    write_le16(&buf[offset], contained_type);    // Contained Entity Type
    offset += 2;
    write_le16(&buf[offset], contained_instance);// Contained Entity Instance
    offset += 2;
    write_le16(&buf[offset], 0x0001);            // Contained Entity Container ID
    offset += 2;

    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_oem_entity_id(pdr_builder_t *builder,
                               uint16_t oem_entity_id_handle,
                               uint32_t vendor_iana,
                               uint16_t vendor_entity_id,
                               const char *name) {
    uint8_t temp_buf[512];
    size_t payload_offset = 0;
    
    // PLDMTerminusHandle
    write_le16(&temp_buf[payload_offset], 0x0001);
    payload_offset += 2;
    // OEM Entity ID Handle (PICMG uses 0x6000 for logical class)
    write_le16(&temp_buf[payload_offset], oem_entity_id_handle);
    payload_offset += 2;
    // Vendor IANA (4 bytes, little-endian)
    write_le32(&temp_buf[payload_offset], vendor_iana);
    payload_offset += 4;
    // Vendor Entity ID
    write_le16(&temp_buf[payload_offset], vendor_entity_id);
    payload_offset += 2;
    // String Count
    temp_buf[payload_offset++] = 0x01;
    // Language tag "en" (null-terminated)
    temp_buf[payload_offset++] = 0x65;
    temp_buf[payload_offset++] = 0x6E;
    temp_buf[payload_offset++] = 0x00;
    // Entity ID Name (UTF-16BE, null-terminated)
    payload_offset += write_utf16be_string(&temp_buf[payload_offset], name, sizeof(temp_buf) - payload_offset);

    uint16_t pdr_length = (uint16_t)payload_offset;
    const size_t total_size = PDR_HEADER_SIZE + payload_offset;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    
    // Write header
    write_pdr_header(buf, builder->record_handle++, PDR_TYPE_OEM_ENTITY_ID, pdr_length);
    
    // Copy payload
    memcpy(buf + PDR_HEADER_SIZE, temp_buf, payload_offset);
    
    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_oem_state_set(pdr_builder_t *builder,
                               uint16_t state_set_id,
                               uint32_t vendor_iana,
                               uint8_t num_states,
                               const uint8_t *state_values,
                               const char **state_names) {
    uint8_t temp_buf[2048];
    size_t payload_offset = 0;

    // PLDMTerminusHandle
    write_le16(&temp_buf[payload_offset], 0x0001);
    payload_offset += 2;
    // OEM State Set ID Handle
    write_le16(&temp_buf[payload_offset], state_set_id);
    payload_offset += 2;
    // Vendor IANA (4 bytes)
    write_le32(&temp_buf[payload_offset], vendor_iana);
    payload_offset += 4;
    // OEM State Set ID
    write_le16(&temp_buf[payload_offset], state_set_id);
    payload_offset += 2;
    // Unspecified Value Hint (treatAsError)
    temp_buf[payload_offset++] = 0x01;
    // State Count (2 bytes)
    write_le16(&temp_buf[payload_offset], num_states);
    payload_offset += 2;

    for (uint8_t i = 0; i < num_states; i++) {
        temp_buf[payload_offset++] = state_values[i * 2];      // Min state value
        temp_buf[payload_offset++] = state_values[i * 2 + 1];  // Max state value
        temp_buf[payload_offset++] = 0x01;                     // String count
        temp_buf[payload_offset++] = 0x65;                     // 'e'
        temp_buf[payload_offset++] = 0x6E;                     // 'n'
        temp_buf[payload_offset++] = 0x00;                     // null terminator
        payload_offset += write_utf16be_string(&temp_buf[payload_offset], state_names[i], sizeof(temp_buf) - payload_offset);
    }

    uint16_t pdr_length = (uint16_t)payload_offset;
    const size_t total_size = PDR_HEADER_SIZE + payload_offset;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    
    // Write header
    write_pdr_header(buf, builder->record_handle++, PDR_TYPE_OEM_STATE_SET, pdr_length);
    
    // Copy payload
    memcpy(buf + PDR_HEADER_SIZE, temp_buf, payload_offset);
    
    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_state_sensor(pdr_builder_t *builder,
                              uint16_t sensor_id,
                              uint16_t entity_type,
                              uint16_t entity_instance,
                              uint16_t state_set,
                              uint8_t possible_states) {
    const uint16_t pdr_length = 17;
    const size_t total_size = PDR_HEADER_SIZE + pdr_length;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    size_t offset = 0;

    // Write header
    offset += write_pdr_header(buf, builder->record_handle++, PDR_TYPE_STATE_SENSOR, pdr_length);

    // Write PDR payload directly into buf
    write_le16(&buf[offset], 0x0001);       // PLDMTerminusHandle
    offset += 2;
    write_le16(&buf[offset], sensor_id);    // Sensor ID
    offset += 2;
    write_le16(&buf[offset], entity_type);  // Entity Type
    offset += 2;
    write_le16(&buf[offset], entity_instance); // Entity Instance
    offset += 2;
    write_le16(&buf[offset], 0x0001);       // Container ID
    offset += 2;
    buf[offset++] = 0x00;                   // Sensor Init (noInit)
    buf[offset++] = 0x00;                   // Sensor Auxiliary Names PDR
    buf[offset++] = 0x01;                   // Composite Sensor Count
    write_le16(&buf[offset], state_set);    // State Set ID
    offset += 2;
    buf[offset++] = 0x01;                   // Possible States Size
    buf[offset++] = possible_states;        // Possible States

    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_state_effecter(pdr_builder_t *builder,
                                uint16_t effecter_id,
                                uint16_t entity_type,
                                uint16_t entity_instance,
                                uint16_t state_set,
                                uint8_t possible_states) {
    const uint16_t pdr_length = 19;
    const size_t total_size = PDR_HEADER_SIZE + pdr_length;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    size_t offset = 0;
    
    // Write header
    offset += write_pdr_header(buf, builder->record_handle++, PDR_TYPE_STATE_EFFECTER, pdr_length);
    
    // Write PDR payload
    write_le16(&buf[offset], 0x0001);       // PLDMTerminusHandle
    offset += 2;
    write_le16(&buf[offset], effecter_id);  // Effecter ID
    offset += 2;
    write_le16(&buf[offset], entity_type);  // Entity Type
    offset += 2;
    write_le16(&buf[offset], entity_instance); // Entity Instance
    offset += 2;
    write_le16(&buf[offset], 0x0001);       // Container ID
    offset += 2;
    write_le16(&buf[offset], 0x0000);       // Effecter Semantic ID
    offset += 2;
    buf[offset++] = 0x00;                   // Effecter Init (noInit)
    buf[offset++] = 0x00;                   // Effecter Description PDR
    buf[offset++] = 0x01;                   // Composite Effecter Count
    write_le16(&buf[offset], state_set);    // State Set ID
    offset += 2;
    buf[offset++] = 0x01;                   // Possible States Size
    buf[offset++] = possible_states;        // Possible States
    
    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_numeric_sensor(pdr_builder_t *builder,
                                uint16_t sensor_id,
                                uint16_t entity_type,
                                uint16_t entity_instance,
                                uint8_t base_unit,
                                int8_t unit_modifier,
                                uint8_t rate_unit) {
    const uint16_t pdr_length = 95;
    const size_t total_size = PDR_HEADER_SIZE + pdr_length;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    size_t offset = 0;
    
    // Write header
    offset += write_pdr_header(buf, builder->record_handle++, PDR_TYPE_NUMERIC_SENSOR, pdr_length);
    
    memset(buf + PDR_HEADER_SIZE, 0, pdr_length);
    // Required header-adjacent fields in order
    write_le16(&buf[offset], 0x0001);       // PLDMTerminusHandle
    offset += 2;
    write_le16(&buf[offset], sensor_id);    // Sensor ID
    offset += 2;
    write_le16(&buf[offset], entity_type);  // Entity Type
    offset += 2;
    write_le16(&buf[offset], entity_instance); // Entity Instance
    offset += 2;
    write_le16(&buf[offset], 0x0001);       // Container ID
    offset += 2;
    buf[offset++] = 0x00;                   // Sensor Init
    buf[offset++] = 0x00;                   // Sensor Auxiliary Names PDR
    buf[offset++] = base_unit;              // Base unit
    buf[offset++] = (uint8_t)unit_modifier; // Unit modifier
    buf[offset++] = rate_unit;              // Rate unit
    buf[offset++] = 0x00;                   // Base OEM Unit Handle
    buf[offset++] = 0x00;                   // Aux Unit
    buf[offset++] = 0x00;                   // Aux Unit Modifier
    buf[offset++] = 0x00;                   // Aux Rate Unit
    buf[offset++] = 0x00;                   // Relationship (dividedBy)
    buf[offset++] = 0x00;                   // Aux OEM Unit Handle
    buf[offset++] = 0x00;                   // Is Linear
    buf[offset++] = 0x00;                   // Sensor Data Size (uint8)
    
    builder->offset += total_size;
    return total_size;
}

size_t pdr_build_numeric_effecter(pdr_builder_t *builder,
                                  uint16_t effecter_id,
                                  uint16_t entity_type,
                                  uint16_t entity_instance,
                                  uint8_t base_unit,
                                  int8_t unit_modifier,
                                  uint8_t rate_unit) {
    const uint16_t pdr_length = 74;
    const size_t total_size = PDR_HEADER_SIZE + pdr_length;
    
    if (builder->offset + total_size > builder->max_size) {
        return 0;
    }
    
    uint8_t *buf = &builder->buffer[builder->offset];
    size_t offset = 0;
    
    // Write header
    offset += write_pdr_header(buf, builder->record_handle++, PDR_TYPE_NUMERIC_EFFECTER, pdr_length);
    
    memset(buf + PDR_HEADER_SIZE, 0, pdr_length);
    write_le16(&buf[offset], 0x0001);       // PLDMTerminusHandle
    offset += 2;
    write_le16(&buf[offset], effecter_id);  // Effecter ID
    offset += 2;
    write_le16(&buf[offset], entity_type);  // Entity Type
    offset += 2;
    write_le16(&buf[offset], entity_instance); // Entity Instance
    offset += 2;
    write_le16(&buf[offset], 0x0001);       // Container ID
    offset += 2;
    write_le16(&buf[offset], 0x0000);       // Effecter Semantic ID
    offset += 2;
    buf[offset++] = 0x00;                   // Effecter Init
    buf[offset++] = 0x00;                   // Effecter Auxiliary Names PDR
    buf[offset++] = base_unit;              // Base unit
    buf[offset++] = (uint8_t)unit_modifier; // Unit modifier
    buf[offset++] = rate_unit;              // Rate unit
    buf[offset++] = 0x00;                   // Base OEM Unit Handle
    buf[offset++] = 0x00;                   // Aux Unit
    buf[offset++] = 0x00;                   // Aux Unit Modifier
    buf[offset++] = 0x00;                   // Aux Rate Unit
    buf[offset++] = 0x00;                   // Aux OEM Unit Handle
    buf[offset++] = 0x00;                   // Is Linear
    buf[offset++] = 0x00;                   // Effecter Data Size (uint8)
    
    builder->offset += total_size;
    return total_size;
}
