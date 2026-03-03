#ifndef JSON_VALIDATOR_H
#define JSON_VALIDATOR_H

/**
 * @file json-validator.h
 * @brief JSON configuration validator for PICMG IOT.1 schema
 * 
 * This module provides validation functions for JSON configuration files
 * against the PICMG IOT.1 configuration schema requirements.
 */

typedef struct {
    int valid;              // 1 if validation passed, 0 if failed
    char *error_message;    // Error message if validation failed (caller must free)
    char *file_path;        // Path to the validated file
} ValidationResult;

/**
 * Validate a JSON configuration file against PICMG IOT.1 schema requirements
 * 
 * @param json_file Path to the JSON configuration file
 * @return ValidationResult structure with validation status and error details
 * 
 * The caller is responsible for freeing result.error_message if it is not NULL.
 */
ValidationResult validate_config_json(const char *json_file);

/**
 * Validate an already-parsed cJSON object against PICMG IOT.1 schema requirements
 * 
 * @param json Parsed cJSON object to validate
 * @return ValidationResult structure with validation status and error details
 * 
 * The caller is responsible for freeing result.error_message if it is not NULL.
 */
ValidationResult validate_config_json_object(void *json);

/**
 * Free resources allocated in a ValidationResult
 * 
 * @param result Pointer to ValidationResult to free
 */
void validation_result_free(ValidationResult *result);

#endif // JSON_VALIDATOR_H
