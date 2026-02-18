/**
 * @file config_loader.c
 * @brief Load JSON configuration and generate PDR/FRU data at runtime
 *
 * This module loads a JSON configuration file and uses the PDR and FRU
 * builder libraries to generate PLDM data structures dynamically.
 *
 * @author Doug Sandy
 * @date February 2026
 * SPDX-License-Identifier: Apache-2.0
 */

#include "config_loader.h"
#include "pdr-builder.h"
#include "fru-builder.h"
#include "json-validator/json-validator.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Maximum sizes for generated data */
#define MAX_PDR_DATA_SIZE (16 * 1024)  /* 16KB for PDR data */
#define MAX_FRU_DATA_SIZE (4 * 1024)   /* 4KB for FRU data */

/* Forward declarations */
static int build_fru_from_json_with_overrides(const char* json_filename, 
                                                runtime_config_t* config,
                                                const fru_overrides_t* overrides);
static char* read_text_file(const char* filename);
static uint16_t json_get_u16(cJSON* obj, const char* name, uint16_t fallback);
static uint32_t json_get_u32(cJSON* obj, const char* name, uint32_t fallback);
static int8_t json_get_i8(cJSON* obj, const char* name, int8_t fallback);
static uint8_t json_get_u8(cJSON* obj, const char* name, uint8_t fallback);

static char* read_text_file(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }

    long size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        return NULL;
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return NULL;
    }

    char* content = (char*)malloc((size_t)size + 1);
    if (!content) {
        fclose(fp);
        return NULL;
    }

    size_t read_bytes = fread(content, 1, (size_t)size, fp);
    content[read_bytes] = '\0';
    fclose(fp);
    return content;
}

static uint16_t json_get_u16(cJSON* obj, const char* name, uint16_t fallback)
{
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, name);
    if (!item || !cJSON_IsNumber(item)) {
        return fallback;
    }
    if (item->valuedouble < 0) {
        return fallback;
    }
    if (item->valuedouble > 65535.0) {
        return 65535u;
    }
    return (uint16_t)item->valuedouble;
}

static uint32_t json_get_u32(cJSON* obj, const char* name, uint32_t fallback)
{
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, name);
    if (!item || !cJSON_IsNumber(item)) {
        return fallback;
    }
    if (item->valuedouble < 0) {
        return fallback;
    }
    if (item->valuedouble > 4294967295.0) {
        return 4294967295u;
    }
    return (uint32_t)item->valuedouble;
}

static int8_t json_get_i8(cJSON* obj, const char* name, int8_t fallback)
{
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, name);
    if (!item || !cJSON_IsNumber(item)) {
        return fallback;
    }
    if (item->valuedouble < -128.0) {
        return -128;
    }
    if (item->valuedouble > 127.0) {
        return 127;
    }
    return (int8_t)item->valuedouble;
}

static uint8_t json_get_u8(cJSON* obj, const char* name, uint8_t fallback)
{
    cJSON* item = cJSON_GetObjectItemCaseSensitive(obj, name);
    if (!item || !cJSON_IsNumber(item)) {
        return fallback;
    }
    if (item->valuedouble < 0) {
        return fallback;
    }
    if (item->valuedouble > 255.0) {
        return 255u;
    }
    return (uint8_t)item->valuedouble;
}

/**
 * @brief Build basic PDR repository from JSON (placeholder)
 *
 * TODO: Implement full JSON parsing. This is a minimal implementation
 * that generates essential PDRs to get started.
 */
static int build_pdrs_from_json(const char* json_filename, runtime_config_t* config)
{
    int rc = -1;
    char* json_content = NULL;
    cJSON* root = NULL;

    /* Allocate buffer for PDR data */
    config->pdr_data = (uint8_t*)malloc(MAX_PDR_DATA_SIZE);
    if (!config->pdr_data) {
        snprintf(config->error_msg, sizeof(config->error_msg), 
                 "Failed to allocate PDR data buffer");
        return -1;
    }

    /* Initialize PDR builder */
    pdr_builder_t builder;
    pdr_builder_init(&builder, config->pdr_data, MAX_PDR_DATA_SIZE, 1);

    /* Build essential system PDRs */
    
    /* 1. Terminus Locator PDR (Type 0x01) */
    if (pdr_build_terminus_locator(&builder, 0x01) == 0) {
        snprintf(config->error_msg, sizeof(config->error_msg),
                 "Failed to build Terminus Locator PDR");
        free(config->pdr_data);
        config->pdr_data = NULL;
        return -1;
    }

    /* 2. FRU Record Set PDR (Type 0x14) */
    pdr_build_fru_record_set(&builder, 0x0001, 0x0050, 0x0001);

    /* 3. Entity Association PDR (Type 0x0F) */
        printf("[DEBUG] pdr_build_entity_association: container_type=0x%04X, container_instance=0x%04X, contained_type=0x%04X, contained_instance=0x%04X\n",
            0x0050, 0x0001, 0x6000, 0x0001);
        pdr_build_entity_association(&builder, 0x0050, 0x0001, 0x6000, 0x0001);

    json_content = read_text_file(json_filename);
    if (!json_content) {
        snprintf(config->error_msg, sizeof(config->error_msg),
                 "Failed to read JSON configuration: %s", json_filename);
        goto cleanup;
    }

    root = cJSON_Parse(json_content);
    if (!root) {
        snprintf(config->error_msg, sizeof(config->error_msg),
                 "Failed to parse JSON configuration: %s", json_filename);
        goto cleanup;
    }

    cJSON* logical_entities = cJSON_GetObjectItemCaseSensitive(root, "logicalEntities");
    if (cJSON_IsArray(logical_entities)) {
        cJSON* entity = NULL;
        cJSON_ArrayForEach(entity, logical_entities) {
            uint32_t vendor_iana = json_get_u32(entity, "entityVendorIANA", 0);
            uint16_t vendor_entity_id = json_get_u16(entity, "vendorEntityID", 0);
            const char* entity_name = cJSON_GetStringValue(
                cJSON_GetObjectItemCaseSensitive(entity, "name"));

            if (vendor_iana != 0 && vendor_entity_id != 0 && entity_name) {
                if (pdr_build_oem_entity_id(&builder, 0x6000, vendor_iana,
                                            vendor_entity_id, entity_name) == 0) {
                    snprintf(config->error_msg, sizeof(config->error_msg),
                             "Failed to build OEM Entity ID PDR");
                    goto cleanup;
                }
            }

            cJSON* io_bindings = cJSON_GetObjectItemCaseSensitive(entity, "ioBindings");
            if (!cJSON_IsArray(io_bindings)) {
                continue;
            }

            cJSON* binding = NULL;
            cJSON_ArrayForEach(binding, io_bindings) {
                cJSON* include_in_pdr = cJSON_GetObjectItemCaseSensitive(binding, "includeInPdr");
                if (!cJSON_IsBool(include_in_pdr) || !cJSON_IsTrue(include_in_pdr)) {
                    continue;
                }

                const char* binding_type = cJSON_GetStringValue(
                    cJSON_GetObjectItemCaseSensitive(binding, "bindingType"));
                if (!binding_type) {
                    continue;
                }

                if (strcmp(binding_type, "stateSensor") == 0) {
                    uint16_t sensor_id = json_get_u16(binding, "sensorID", 0);
                    uint16_t state_set = json_get_u16(binding, "stateSet", 0);
                    uint8_t possible_states = json_get_u8(binding, "possibleStates", 0);

                    if (sensor_id == 0) {
                        continue;
                    }
                    if (pdr_build_state_sensor(&builder, sensor_id, 0x6000, 0x0001,
                                               state_set, possible_states) == 0) {
                        snprintf(config->error_msg, sizeof(config->error_msg),
                                 "Failed to build State Sensor PDR");
                        goto cleanup;
                    }
                } else if (strcmp(binding_type, "stateEffecter") == 0) {
                    uint16_t effecter_id = json_get_u16(binding, "effecterID", 0);
                    uint16_t state_set = json_get_u16(binding, "stateSet", 0);
                    uint8_t possible_states = json_get_u8(binding, "possibleStates", 0);

                    if (effecter_id == 0) {
                        continue;
                    }
                    if (pdr_build_state_effecter(&builder, effecter_id, 0x6000, 0x0001,
                                                 state_set, possible_states) == 0) {
                        snprintf(config->error_msg, sizeof(config->error_msg),
                                 "Failed to build State Effecter PDR");
                        goto cleanup;
                    }
                } else if (strcmp(binding_type, "numericSensor") == 0) {
                    uint16_t sensor_id = json_get_u16(binding, "sensorID", 0);
                    uint8_t base_unit = json_get_u8(binding, "physicalBaseUnit", 0);
                    int8_t unit_modifier = json_get_i8(binding, "physicalUnitModifier", 0);
                    uint8_t rate_unit = json_get_u8(binding, "physicalRateUnit", 0);

                    if (sensor_id == 0) {
                        continue;
                    }
                    if (pdr_build_numeric_sensor(&builder, sensor_id, 0x6000, 0x0001,
                                                 base_unit, unit_modifier, rate_unit) == 0) {
                        snprintf(config->error_msg, sizeof(config->error_msg),
                                 "Failed to build Numeric Sensor PDR");
                        goto cleanup;
                    }
                } else if (strcmp(binding_type, "numericEffecter") == 0) {
                    uint16_t effecter_id = json_get_u16(binding, "effecterID", 0);
                    uint8_t base_unit = json_get_u8(binding, "physicalBaseUnit", 0);
                    int8_t unit_modifier = json_get_i8(binding, "physicalUnitModifier", 0);
                    uint8_t rate_unit = json_get_u8(binding, "physicalRateUnit", 0);

                    if (effecter_id == 0) {
                        continue;
                    }
                    if (pdr_build_numeric_effecter(&builder, effecter_id, 0x6000, 0x0001,
                                                   base_unit, unit_modifier, rate_unit) == 0) {
                        snprintf(config->error_msg, sizeof(config->error_msg),
                                 "Failed to build Numeric Effecter PDR");
                        goto cleanup;
                    }
                }
            }
        }
    }

    cJSON* oem_state_sets = cJSON_GetObjectItemCaseSensitive(root, "oemStateSets");
    if (cJSON_IsArray(oem_state_sets)) {
        cJSON* state_set = NULL;
        cJSON_ArrayForEach(state_set, oem_state_sets) {
            uint32_t vendor_iana = json_get_u32(state_set, "vendorIANA", 0);
            uint16_t state_set_id = json_get_u16(state_set, "stateSetID", 0);
            cJSON* records = cJSON_GetObjectItemCaseSensitive(state_set, "oemStateValueRecords");

            if (vendor_iana == 0 || state_set_id == 0 || !cJSON_IsArray(records)) {
                continue;
            }

            int record_count = cJSON_GetArraySize(records);
            if (record_count <= 0) {
                continue;
            }

            uint8_t* values = (uint8_t*)calloc((size_t)record_count * 2, sizeof(uint8_t));
            const char** names = (const char**)calloc((size_t)record_count, sizeof(char*));
            if (!values || !names) {
                free(values);
                free(names);
                snprintf(config->error_msg, sizeof(config->error_msg),
                         "Failed to allocate OEM state set data");
                goto cleanup;
            }

            int idx = 0;
            cJSON* record = NULL;
            cJSON_ArrayForEach(record, records) {
                values[idx * 2] = json_get_u8(record, "minStateValue", 0);
                values[idx * 2 + 1] = json_get_u8(record, "maxStateValue", 0);

                const char* state_name = NULL;
                cJSON* state_names = cJSON_GetObjectItemCaseSensitive(record, "stateName");
                if (cJSON_IsArray(state_names)) {
                    cJSON* name_item = cJSON_GetArrayItem(state_names, 0);
                    if (cJSON_IsString(name_item)) {
                        state_name = name_item->valuestring;
                    }
                }
                if (!state_name) {
                    state_name = "";
                }
                names[idx] = state_name;
                idx++;
                if (idx >= record_count) {
                    break;
                }
            }

            if (pdr_build_oem_state_set(&builder, state_set_id, vendor_iana,
                                        (uint8_t)record_count, values, names) == 0) {
                free(values);
                free(names);
                snprintf(config->error_msg, sizeof(config->error_msg),
                         "Failed to build OEM State Set PDR");
                goto cleanup;
            }

            free(values);
            free(names);
        }
    }

    /* Get final sizes */
    config->pdr_data_size = pdr_builder_get_offset(&builder);
    config->pdr_record_count = pdr_builder_get_record_count(&builder);

    /* Shrink buffer to actual size used */
    config->pdr_data = (uint8_t*)realloc(config->pdr_data, config->pdr_data_size);

    printf("Generated %u PDR records (%zu bytes) from %s\n",
           config->pdr_record_count, config->pdr_data_size, json_filename);

    rc = 0;

cleanup:
    if (root) {
        cJSON_Delete(root);
    }
    free(json_content);
    if (rc != 0) {
        free(config->pdr_data);
        config->pdr_data = NULL;
    }
    return rc;
}

/**
 * @brief Build FRU data from JSON (placeholder)
 *
 * TODO: Implement full JSON parsing. This is a minimal implementation
 * that generates essential FRU records to get started.
 */
static int build_fru_from_json(const char* json_filename, runtime_config_t* config)
{
    return build_fru_from_json_with_overrides(json_filename, config, NULL);
}

/**
 * @brief Build FRU data from JSON with field overrides
 *
 * Generates FRU records from JSON configuration, applying command-line
 * overrides when provided.
 */
static int build_fru_from_json_with_overrides(const char* json_filename, 
                                                runtime_config_t* config,
                                                const fru_overrides_t* overrides)
{
    /* Allocate buffer for FRU data */
    config->fru_data = (uint8_t*)malloc(MAX_FRU_DATA_SIZE);
    if (!config->fru_data) {
        snprintf(config->error_msg, sizeof(config->error_msg),
                 "Failed to allocate FRU data buffer");
        return -1;
    }

    /* Initialize FRU builder */
    fru_builder_t builder;
    fru_builder_init(&builder, config->fru_data, MAX_FRU_DATA_SIZE, 1);

    /* Build a basic General FRU Record */
    /* TODO: Extract these values from JSON */
    const char* model = "IoT-Endpoint";
    const char* part_num = "IOT-EP-001";
    const char* serial = "SN202602160001";
    const char* manufacturer = "PICMG";

    /* Apply global overrides if provided */
    if (overrides && overrides->global_manufacturer) {
        manufacturer = overrides->global_manufacturer;
    }

    /* Apply record-specific overrides for first record (index 0) */
    if (overrides && overrides->override_count > 0) {
        const fru_record_override_t* ovr = &overrides->overrides[0];
        
        /* Check if this override applies to first record */
        if (ovr->record_index == -1 || ovr->record_index == 0 || 
            (ovr->record_set_id == 0) || (ovr->record_set_id == 1)) {
            
            if (ovr->model) model = ovr->model;
            if (ovr->part_number) part_num = ovr->part_number;
            if (ovr->serial_number) serial = ovr->serial_number;
            if (ovr->manufacturer) manufacturer = ovr->manufacturer;
        }
    }

    /* Build FRU field array */
    fru_field_t fields[32];  /* Increased to support more fields */
    int field_count = 0;

    /* Add standard fields */
    fru_field_string(&fields[field_count++], FRU_FIELD_MODEL, model, strlen(model));
    fru_field_string(&fields[field_count++], FRU_FIELD_PART_NUMBER, part_num, strlen(part_num));
    fru_field_string(&fields[field_count++], FRU_FIELD_SERIAL_NUMBER, serial, strlen(serial));
    fru_field_string(&fields[field_count++], FRU_FIELD_MANUFACTURER, manufacturer, strlen(manufacturer));

    /* Apply additional overrides if provided */
    if (overrides && overrides->override_count > 0) {
        const fru_record_override_t* ovr = &overrides->overrides[0];
        
        if (ovr->record_index == -1 || ovr->record_index == 0 || 
            (ovr->record_set_id == 0) || (ovr->record_set_id == 1)) {
            
            /* Add optional standard fields if overridden */
            if (ovr->name) {
                fru_field_string(&fields[field_count++], FRU_FIELD_NAME, 
                               ovr->name, strlen(ovr->name));
            }
            if (ovr->version) {
                fru_field_string(&fields[field_count++], FRU_FIELD_VERSION, 
                               ovr->version, strlen(ovr->version));
            }
            if (ovr->sku) {
                fru_field_string(&fields[field_count++], FRU_FIELD_SKU, 
                               ovr->sku, strlen(ovr->sku));
            }
            if (ovr->asset_tag) {
                fru_field_string(&fields[field_count++], FRU_FIELD_ASSET_TAG, 
                               ovr->asset_tag, strlen(ovr->asset_tag));
            }
            if (ovr->description) {
                fru_field_string(&fields[field_count++], FRU_FIELD_DESCRIPTION, 
                               ovr->description, strlen(ovr->description));
            }
            if (ovr->engineering_change) {
                /* Use OEM field for engineering change (not in standard fields) */
                fru_field_string(&fields[field_count++], 0xFE /* OEM field */, 
                               ovr->engineering_change, strlen(ovr->engineering_change));
            }
            if (ovr->fru_file_id) {
                /* Use OEM field for FRU file ID (not in standard fields) */
                fru_field_string(&fields[field_count++], 0xFD /* OEM field */, 
                               ovr->fru_file_id, strlen(ovr->fru_file_id));
            }
            if (ovr->other) {
                /* Use OEM field for other information (not in standard fields) */
                fru_field_string(&fields[field_count++], 0xFC /* OEM field */, 
                               ovr->other, strlen(ovr->other));
            }
            if (ovr->mfg_date) {
                /* TODO: Parse date string and convert to timestamp */
                fru_field_string(&fields[field_count++], 0xFB /* Custom field */, 
                               ovr->mfg_date, strlen(ovr->mfg_date));
            }

            /* Add custom fields */
            for (int i = 0; i < ovr->custom_field_count && field_count < 32; i++) {
                const char* type = ovr->custom_fields[i].type;
                const char* value = ovr->custom_fields[i].value;
                
                if (strcmp(type, "string") == 0) {
                    fru_field_string(&fields[field_count++], 0xFA /* OEM field */, 
                                   value, strlen(value));
                } else if (strcmp(type, "uint32") == 0) {
                    uint32_t val = (uint32_t)atoi(value);
                    uint8_t val_bytes[4];
                    /* Convert to little-endian */
                    val_bytes[0] = val & 0xFF;
                    val_bytes[1] = (val >> 8) & 0xFF;
                    val_bytes[2] = (val >> 16) & 0xFF;
                    val_bytes[3] = (val >> 24) & 0xFF;
                    fru_field_binary(&fields[field_count++], 0xF9, val_bytes, 4);
                }
                /* TODO: Support timestamp types */
            }
        }
    }

    if (fru_build_general_record(&builder, FRU_ENCODING_ASCII, fields, field_count) == 0) {
        snprintf(config->error_msg, sizeof(config->error_msg),
                 "Failed to build General FRU record");
        free(config->fru_data);
        config->fru_data = NULL;
        return -1;
    }

    /* Build OEM FRU record if OEM fields are provided */
    if (overrides && overrides->override_count > 0) {
        const fru_record_override_t* ovr = &overrides->overrides[0];
        
        if (ovr->oem_field_count > 0) {
            uint32_t oem_iana = ovr->oem_iana ? ovr->oem_iana : 12634; /* Default PICMG IANA */
            fru_field_t oem_fields[16];
            int oem_count = 0;

            for (int i = 0; i < ovr->oem_field_count && oem_count < 16; i++) {
                fru_field_string(&oem_fields[oem_count++], 0xFE,
                               ovr->oem_fields[i].value, 
                               strlen(ovr->oem_fields[i].value));
            }

            fru_build_oem_record(&builder, oem_iana, FRU_ENCODING_ASCII, 
                               oem_fields, oem_count);
        }
    }

    /* Get final sizes */
    config->fru_data_size = fru_builder_get_offset(&builder);
    config->fru_record_count = fru_builder_get_record_count(&builder);

    /* Shrink buffer to actual size used */
    config->fru_data = (uint8_t*)realloc(config->fru_data, config->fru_data_size);

    printf("Generated %u FRU records (%zu bytes) from %s\n",
           config->fru_record_count, config->fru_data_size, json_filename);

    return 0;
}

int config_load_from_json(const char* json_filename, runtime_config_t* config)
{
    if (!json_filename || !config) {
        return -1;
    }

    /* Initialize config structure */
    memset(config, 0, sizeof(runtime_config_t));

    /* Validate JSON file against schema */
    printf("Validating configuration file: %s\n", json_filename);
    ValidationResult validation = validate_config_json(json_filename);
    
    if (!validation.valid) {
        snprintf(config->error_msg, sizeof(config->error_msg),
                 "JSON validation failed: %s", 
                 validation.error_message ? validation.error_message : "Unknown error");
        validation_result_free(&validation);
        return -1;
    }
    
    printf("JSON validation passed\n");
    validation_result_free(&validation);

    /* Build PDR data */
    if (build_pdrs_from_json(json_filename, config) != 0) {
        return -1;
    }

    /* Build FRU data */
    if (build_fru_from_json(json_filename, config) != 0) {
        /* Clean up PDR data if FRU generation failed */
        if (config->pdr_data) {
            free(config->pdr_data);
            config->pdr_data = NULL;
        }
        return -1;
    }

    config->loaded = true;
    printf("Configuration loaded successfully\n");

    return 0;
}

void config_free(runtime_config_t* config)
{
    if (!config) return;

    if (config->pdr_data) {
        free(config->pdr_data);
        config->pdr_data = NULL;
    }

    if (config->fru_data) {
        free(config->fru_data);
        config->fru_data = NULL;
    }

    config->pdr_data_size = 0;
    config->pdr_record_count = 0;
    config->fru_data_size = 0;
    config->fru_record_count = 0;
    config->loaded = false;
}

const uint8_t* config_get_pdr_data(const runtime_config_t* config)
{
    return (config && config->loaded) ? config->pdr_data : NULL;
}

size_t config_get_pdr_size(const runtime_config_t* config)
{
    return (config && config->loaded) ? config->pdr_data_size : 0;
}

uint32_t config_get_pdr_count(const runtime_config_t* config)
{
    return (config && config->loaded) ? config->pdr_record_count : 0;
}

const uint8_t* config_get_fru_data(const runtime_config_t* config)
{
    return (config && config->loaded) ? config->fru_data : NULL;
}

size_t config_get_fru_size(const runtime_config_t* config)
{
    return (config && config->loaded) ? config->fru_data_size : 0;
}

uint32_t config_get_fru_count(const runtime_config_t* config)
{
    return (config && config->loaded) ? config->fru_record_count : 0;
}

void fru_overrides_init(fru_overrides_t* overrides)
{
    if (!overrides) return;
    
    memset(overrides, 0, sizeof(fru_overrides_t));
    
    /* Set default record indices to -1 (apply to first record) */
    for (int i = 0; i < 8; i++) {
        overrides->overrides[i].record_index = -1;
        overrides->overrides[i].record_set_id = 0;
    }
}

int config_load_from_json_with_overrides(
    const char* json_filename,
    runtime_config_t* config,
    const fru_overrides_t* fru_overrides)
{
    if (!json_filename || !config) {
        return -1;
    }

    /* Initialize config structure */
    memset(config, 0, sizeof(runtime_config_t));

    /* Validate JSON file against schema */
    printf("Validating configuration file: %s\n", json_filename);
    ValidationResult validation = validate_config_json(json_filename);
    
    if (!validation.valid) {
        snprintf(config->error_msg, sizeof(config->error_msg),
                 "JSON validation failed: %s", 
                 validation.error_message ? validation.error_message : "Unknown error");
        validation_result_free(&validation);
        return -1;
    }
    
    printf("JSON validation passed\n");
    validation_result_free(&validation);

    /* Build PDR data */
    if (build_pdrs_from_json(json_filename, config) != 0) {
        return -1;
    }

    /* Build FRU data with overrides */
    if (build_fru_from_json_with_overrides(json_filename, config, fru_overrides) != 0) {
        /* Clean up PDR data if FRU generation failed */
        if (config->pdr_data) {
            free(config->pdr_data);
            config->pdr_data = NULL;
        }
        return -1;
    }

    config->loaded = true;
    
    /* Print override info if provided */
    if (fru_overrides && fru_overrides->override_count > 0) {
        printf("Applied %d FRU field override(s)\n", fru_overrides->override_count);
    }
    
    printf("Configuration loaded successfully\n");

    return 0;
}
