#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "cJSON.h"
#include "json-validator.h"

/**
 * Free resources allocated in a ValidationResult
 */
void validation_result_free(ValidationResult *result) {
    if (result && result->error_message) {
        free(result->error_message);
        result->error_message = NULL;
    }
}

/**
 * Read entire file into memory
 */
char* read_file(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *content = (char *)malloc(size + 1);
    if (!content) {
        fclose(fp);
        return NULL;
    }

    fread(content, 1, size, fp);
    content[size] = '\0';
    fclose(fp);

    return content;
}

/**
 * Validate that required fields exist in the JSON object
 */
int validate_required_fields(cJSON *json, const char **required_fields, int num_fields, char **error_msg) {
    if (!cJSON_IsObject(json)) {
        *error_msg = strdup("Root must be a JSON object");
        return 0;
    }

    for (int i = 0; i < num_fields; i++) {
        cJSON *field = cJSON_GetObjectItemCaseSensitive(json, required_fields[i]);
        if (!field) {
            int len = snprintf(NULL, 0, "Missing required field: '%s'", required_fields[i]);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "Missing required field: '%s'", required_fields[i]);
            return 0;
        }
    }

    return 1;
}

/**
 * Validate FRU Records structure
 */
int validate_fru_records(cJSON *fru_records, char **error_msg) {
    if (!cJSON_IsArray(fru_records)) {
        *error_msg = strdup("fruRecords must be an array");
        return 0;
    }

    cJSON *record = NULL;
    int record_idx = 0;
    cJSON_ArrayForEach(record, fru_records) {
        if (!cJSON_IsObject(record)) {
            int len = snprintf(NULL, 0, "fruRecords[%d] must be an object", record_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "fruRecords[%d] must be an object", record_idx);
            return 0;
        }

        // Check for required fields per schema: vendorIANA, required, fields
        cJSON *vendor_iana = cJSON_GetObjectItemCaseSensitive(record, "vendorIANA");
        if (!vendor_iana || !cJSON_IsNumber(vendor_iana)) {
            int len = snprintf(NULL, 0, "fruRecords[%d] missing or invalid 'vendorIANA' field", record_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "fruRecords[%d] missing or invalid 'vendorIANA' field", record_idx);
            return 0;
        }

        cJSON *required_field = cJSON_GetObjectItemCaseSensitive(record, "required");
        if (!required_field || !cJSON_IsBool(required_field)) {
            int len = snprintf(NULL, 0, "fruRecords[%d] missing or invalid 'required' field", record_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "fruRecords[%d] missing or invalid 'required' field", record_idx);
            return 0;
        }

        cJSON *fields = cJSON_GetObjectItemCaseSensitive(record, "fields");
        if (!fields || !cJSON_IsArray(fields)) {
            int len = snprintf(NULL, 0, "fruRecords[%d] missing or invalid 'fields' array", record_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "fruRecords[%d] missing or invalid 'fields' array", record_idx);
            return 0;
        }

        record_idx++;
    }

    return 1;
}

/**
 * Validate Channels structure
 */
int validate_channels(cJSON *channels, char **error_msg) {
    if (!cJSON_IsArray(channels)) {
        *error_msg = strdup("channels must be an array");
        return 0;
    }

    cJSON *channel = NULL;
    int channel_idx = 0;
    cJSON_ArrayForEach(channel, channels) {
        if (!cJSON_IsObject(channel)) {
            int len = snprintf(NULL, 0, "channels[%d] must be an object", channel_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "channels[%d] must be an object", channel_idx);
            return 0;
        }

        cJSON *name = cJSON_GetObjectItemCaseSensitive(channel, "name");
        if (!name || !cJSON_IsString(name)) {
            int len = snprintf(NULL, 0, "channels[%d] missing or invalid 'name' field", channel_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "channels[%d] missing or invalid 'name' field", channel_idx);
            return 0;
        }

        cJSON *type = cJSON_GetObjectItemCaseSensitive(channel, "type");
        if (!type || !cJSON_IsString(type)) {
            int len = snprintf(NULL, 0, "channels[%d] missing or invalid 'type' field", channel_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "channels[%d] missing or invalid 'type' field", channel_idx);
            return 0;
        }

        channel_idx++;
    }

    return 1;
}

/**
 * Validate Logical Entities structure
 */
int validate_logical_entities(cJSON *entities, char **error_msg) {
    if (!cJSON_IsArray(entities)) {
        *error_msg = strdup("logicalEntities must be an array");
        return 0;
    }

    if (entities->child == NULL) {
        *error_msg = strdup("logicalEntities must have at least one item");
        return 0;
    }

    cJSON *entity = NULL;
    int entity_idx = 0;
    cJSON_ArrayForEach(entity, entities) {
        if (!cJSON_IsObject(entity)) {
            int len = snprintf(NULL, 0, "logicalEntities[%d] must be an object", entity_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "logicalEntities[%d] must be an object", entity_idx);
            return 0;
        }

        cJSON *entity_vendor_iana = cJSON_GetObjectItemCaseSensitive(entity, "entityVendorIANA");
        if (!entity_vendor_iana || !cJSON_IsNumber(entity_vendor_iana)) {
            int len = snprintf(NULL, 0, "logicalEntities[%d] missing or invalid 'entityVendorIANA' field", entity_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "logicalEntities[%d] missing or invalid 'entityVendorIANA' field", entity_idx);
            return 0;
        }

        cJSON *vendor_entity_id = cJSON_GetObjectItemCaseSensitive(entity, "vendorEntityID");
        if (!vendor_entity_id || !cJSON_IsNumber(vendor_entity_id)) {
            int len = snprintf(NULL, 0, "logicalEntities[%d] missing or invalid 'vendorEntityID' field", entity_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "logicalEntities[%d] missing or invalid 'vendorEntityID' field", entity_idx);
            return 0;
        }

        cJSON *name = cJSON_GetObjectItemCaseSensitive(entity, "name");
        if (!name || !cJSON_IsString(name)) {
            int len = snprintf(NULL, 0, "logicalEntities[%d] missing or invalid 'name' field", entity_idx);
            *error_msg = (char *)malloc(len + 1);
            snprintf(*error_msg, len + 1, "logicalEntities[%d] missing or invalid 'name' field", entity_idx);
            return 0;
        }

        entity_idx++;
    }

    return 1;
}

/**
 * Validate an already-parsed cJSON object against PICMG IOT.1 schema requirements
 */
ValidationResult validate_config_json_object(void *json_ptr) {
    ValidationResult result = {0, NULL, NULL};
    cJSON *json = (cJSON *)json_ptr;

    if (!json) {
        result.error_message = strdup("NULL cJSON object provided");
        return result;
    }

    // Define required fields
    const char *required_fields[] = {
        "device", "pins", "channels", "maxFruBytes",
        "fruRecords", "logicalEntities", "oemStateSets"
    };
    int num_required = sizeof(required_fields) / sizeof(required_fields[0]);

    // Validate required fields
    if (!validate_required_fields(json, required_fields, num_required, &result.error_message)) {
        return result;
    }

    // Validate specific structures
    cJSON *channels = cJSON_GetObjectItemCaseSensitive(json, "channels");
    if (!validate_channels(channels, &result.error_message)) {
        return result;
    }

    cJSON *fru_records = cJSON_GetObjectItemCaseSensitive(json, "fruRecords");
    if (!validate_fru_records(fru_records, &result.error_message)) {
        return result;
    }

    cJSON *logical_entities = cJSON_GetObjectItemCaseSensitive(json, "logicalEntities");
    if (!validate_logical_entities(logical_entities, &result.error_message)) {
        return result;
    }

    result.valid = 1;
    return result;
}

/**
 * Main validation function
 */
ValidationResult validate_config_json(const char *json_file) {
    ValidationResult result = {0, NULL, (char *)json_file};

    char *json_content = read_file(json_file);
    if (!json_content) {
        result.error_message = strdup("Failed to read JSON file");
        return result;
    }

    cJSON *json = cJSON_Parse(json_content);
    free(json_content);

    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            int len = snprintf(NULL, 0, "JSON parse error: %s", error_ptr);
            result.error_message = (char *)malloc(len + 1);
            snprintf(result.error_message, len + 1, "JSON parse error: %s", error_ptr);
        } else {
            result.error_message = strdup("JSON parse error");
        }
        return result;
    }

    // Define required fields
    const char *required_fields[] = {
        "device", "pins", "channels", "maxFruBytes",
        "fruRecords", "logicalEntities", "oemStateSets"
    };
    int num_required = sizeof(required_fields) / sizeof(required_fields[0]);

    // Validate required fields
    if (!validate_required_fields(json, required_fields, num_required, &result.error_message)) {
        cJSON_Delete(json);
        return result;
    }

    // Validate specific structures
    cJSON *channels = cJSON_GetObjectItemCaseSensitive(json, "channels");
    if (!validate_channels(channels, &result.error_message)) {
        cJSON_Delete(json);
        return result;
    }

    cJSON *fru_records = cJSON_GetObjectItemCaseSensitive(json, "fruRecords");
    if (!validate_fru_records(fru_records, &result.error_message)) {
        cJSON_Delete(json);
        return result;
    }

    cJSON *logical_entities = cJSON_GetObjectItemCaseSensitive(json, "logicalEntities");
    if (!validate_logical_entities(logical_entities, &result.error_message)) {
        cJSON_Delete(json);
        return result;
    }

    cJSON_Delete(json);
    result.valid = 1;
    return result;
}

