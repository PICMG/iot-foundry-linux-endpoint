# FRU Builder Library

Runtime library for building PLDM FRU (Field Replaceable Unit) Record Table data into buffers.

## Files

- **fru-builder.h** - API header with function declarations
- **fru-builder.c** - Implementation

## Integration

This library is automatically built as part of the main project. It's included in:
- **CMakeLists.txt** - Added to `PLATFORM_SOURCES` and include paths
- **Makefile** - Pattern-matched via wildcard (if used)

## Usage in Your Code

```c
#include "fru-builder.h"

uint8_t fru_buffer[1024];
fru_builder_t builder;

fru_builder_init(&builder, fru_buffer, sizeof(fru_buffer), 1);

// Build General FRU Record
fru_field_t fields[4];
fru_field_string(&fields[0], FRU_FIELD_MODEL, "IoT-Device", 10);
fru_field_string(&fields[1], FRU_FIELD_PART_NUMBER, "12345", 5);
fru_field_string(&fields[2], FRU_FIELD_SERIAL_NUMBER, "SN001", 5);
fru_field_string(&fields[3], FRU_FIELD_MANUFACTURER, "PICMG", 5);

fru_build_general_record(&builder, FRU_ENCODING_ASCII, fields, 4);

// Build OEM FRU Record
const uint32_t picmg_iana = 12634;
fru_field_t oem_fields[1];
fru_field_string(&oem_fields[0], FRU_FIELD_OEM_DEFINED_MIN, "data", 4);

fru_build_oem_record(&builder, FRU_ENCODING_UTF8, picmg_iana, oem_fields, 1);

// Use the buffer
size_t total_size = fru_builder_get_offset(&builder);
uint32_t num_records = fru_builder_get_record_count(&builder);
```

## Supported Record Types

1. **General FRU Record** (0x01)
   - Standard fields: Model, Part Number, Serial Number, Manufacturer, etc.
   - 16 predefined field types per DSP0257

2. **OEM FRU Record** (0xFE)
   - Vendor IANA (mandatory first field, auto-added)
   - OEM-defined fields (types 2-254)

## Documentation

See `FRU_BUILDER_README.md` in this directory for complete API documentation and examples.

## Dependencies

None - pure C library with standard headers only.
