# JSON Validator

This directory contains the JSON configuration validator that validates PICMG IOT.1 configuration files against the schema requirements.

## Files

- **json-validator.h** - Public API header
- **json-validator.c** - Implementation of JSON validation functions
- **picmg-iot1-config-schema.json** - PICMG IOT.1 configuration schema (reference)

## Integration

The JSON validator is integrated into the main endpoint build and is used automatically when loading configuration files via the `-c` command line option.

## Validation Process

When a JSON configuration file is loaded:

1. The file is validated against PICMG IOT.1 schema requirements
2. Required fields are checked:
   - `device` - Device information
   - `pins` - Pin definitions
   - `channels` - Channel configurations
   - `maxFruBytes` - Maximum FRU data size
   - `fruRecords` - FRU record definitions
   - `logicalEntities` - Logical entity definitions
   - `oemStateSets` - OEM state set definitions

3. Structure validation:
   - Channels must be an array with valid name and type fields
   - FRU records must have vendorIANA, required, and fields
   - Logical entities must have entityVendorIANA, vendorEntityID, and name

4. If validation fails, the endpoint exits with a clear error message

## API Usage

### In C Code

```c
#include "json-validator/json-validator.h"

// Validate a JSON file
ValidationResult result = validate_config_json("config.json");

if (!result.valid) {
    fprintf(stderr, "Validation failed: %s\n", result.error_message);
    validation_result_free(&result);
    return -1;
}

printf("Validation passed!\n");
validation_result_free(&result);
```

### Validation Result Structure

```c
typedef struct {
    int valid;              // 1 if validation passed, 0 if failed
    char *error_message;    // Error message if failed (must be freed)
    char *file_path;        // Path to validated file
} ValidationResult;
```

## Example Output

### Valid JSON
```
Validating configuration file: example-simple-endpoint.json
JSON validation passed
```

### Invalid JSON
```
Validating configuration file: test-invalid.json
JSON validation failed: Missing required field: 'channels'
```

## Dependencies

- **cJSON** - JSON parsing library (automatically downloaded by CMake)

## Building

The validator is automatically built as part of the main endpoint build:

```bash
cd build
cmake ..
make
```

## Testing

Test the validator by running the endpoint with various JSON files:

```bash
# Valid configuration
./build/endpoint -p -c example-simple-endpoint.json

# Invalid configuration (missing fields)
./build/endpoint -p -c test-invalid.json
```

## Schema Compliance

The validator checks compliance with the PICMG IOT.1 configuration schema as defined in the specification. The schema file `picmg-iot1-config-schema.json` is included for reference.

### Validated Requirements

- ✅ All required top-level fields present
- ✅ Channel structure (name, type)
- ✅ FRU record structure (vendorIANA, required, fields)
- ✅ Logical entity structure (entityVendorIANA, vendorEntityID, name)
- ✅ Array types validated
- ✅ Field types validated (string, number, boolean, array)

### Future Enhancements

- Full schema validation (min/max values, enumerations)
- Pin definition validation
- OEM state set validation
- Device information validation
- Cross-reference validation (e.g., channel bindings to pins)

## Integration Points

The validator is used by:
- **src/config_loader.c** - Validates JSON before generating PDR/FRU data
- **src/main.c** - Indirectly via config_loader during startup

## Error Handling

All validation errors include descriptive messages indicating:
- What field is missing or invalid
- The index of the problematic array element (if applicable)
- The type of validation failure

This helps developers quickly identify and fix configuration issues.
