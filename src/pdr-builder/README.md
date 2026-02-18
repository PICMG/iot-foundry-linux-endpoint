# PDR Builder Library

Runtime library for building PLDM Platform Descriptor Records (PDRs) into buffers.

## Files

- **pdr-builder.h** - API header with function declarations
- **pdr-builder.c** - Implementation

## Integration

This library is automatically built as part of the main project. It's included in:
- **CMakeLists.txt** - Added to `PLATFORM_SOURCES` and include paths
- **Makefile** - Pattern-matched via wildcard (if used)

## Usage in Your Code

```c
#include "pdr-builder.h"

uint8_t pdr_buffer[2048];
pdr_builder_t builder;

pdr_builder_init(&builder, pdr_buffer, sizeof(pdr_buffer), 1);

// Build PDRs
pdr_build_terminus_locator(&builder, 0x01);
pdr_build_fru_record_set(&builder, 0x0001, 0x0050, 0x0001);
pdr_build_state_sensor(&builder, 1, 0x6001, 1, 0x0060, 0x03);

// Use the buffer
size_t total_size = pdr_builder_get_offset(&builder);
uint32_t num_pdrs = pdr_builder_get_record_count(&builder);
```

## Supported PDR Types

1. **Terminus Locator** (0x01)
2. **Numeric Sensor** (0x02)
3. **State Sensor** (0x04)
4. **OEM State Set** (0x08)
5. **Numeric Effecter** (0x09)
6. **State Effecter** (0x0B)
7. **Entity Association** (0x0F)
8. **OEM Entity ID** (0x11)
9. **FRU Record Set** (0x14)

## Documentation

See `PDR_BUILDER_README.md` in this directory for complete API documentation and examples.

## Dependencies

None - pure C library with standard headers only.
