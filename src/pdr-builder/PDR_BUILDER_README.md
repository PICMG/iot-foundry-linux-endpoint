# PDR Builder Library

## Overview

Runtime library for generating PLDM PDRs into buffers. All generated PDRs include 10-byte headers matching the `config.c` format.

## Usage

```c
#include "pdr-builder.h"

// Allocate buffer
uint8_t pdr_buffer[4096];
pdr_builder_t builder;

// Initialize
pdr_builder_init(&builder, pdr_buffer, sizeof(pdr_buffer), 1);

// Generate PDRs
pdr_build_terminus_locator(&builder, 0x01);
pdr_build_fru_record_set(&builder, 0x0001, 0x0050, 0x0001);
pdr_build_state_sensor(&builder, 1, 0x6001, 0x0001, 0x0060, 0x03);

// Get results
size_t total_bytes = pdr_builder_get_offset(&builder);
uint32_t num_records = pdr_builder_get_record_count(&builder);
```

## API Functions

### Initialization
- `pdr_builder_init()` - Initialize builder with buffer
- `pdr_builder_get_offset()` - Get total bytes written
- `pdr_builder_get_record_count()` - Get number of records

### System PDRs
- `pdr_build_terminus_locator()` - Generate Terminus Locator (Type 0x01)
- `pdr_build_fru_record_set()` - Generate FRU Record Set (Type 0x14)
- `pdr_build_entity_association()` - Generate Entity Association (Type 0x0F)

### Entity PDRs
- `pdr_build_oem_entity_id()` - Generate OEM Entity ID (Type 0x11)

### State Set PDRs
- `pdr_build_oem_state_set()` - Generate OEM State Set (Type 0x08)

### Sensor/Effecter PDRs
- `pdr_build_state_sensor()` - Generate State Sensor (Type 0x04)
- `pdr_build_state_effecter()` - Generate State Effecter (Type 0x0B)
- `pdr_build_numeric_sensor()` - Generate Numeric Sensor (Type 0x02)
- `pdr_build_numeric_effecter()` - Generate Numeric Effecter (Type 0x09)

## Building

### Compile library
```bash
gcc -c pdr-builder.c -o pdr-builder.o -Wall
```

### Build example
```bash
gcc pdr-builder-example.c pdr-builder.c -o pdr-builder-example -Wall
./pdr-builder-example
```

### Integration with your code
```bash
gcc your-program.c pdr-builder.c -o your-program
```

## Output Format

All PDRs include a 10-byte header:
```
Offset  Size  Field
+0      4     Record Handle (LE)
+4      1     Version (0x01)
+5      1     PDR Type
+6      1     Record Format Version (0x01)
+7      1     Reserved (0x00)
+8      2     PDR Length (LE)
+10     N     PDR Payload
```

This matches the runtime PDR repository format used by `config_loader.c` and `platform_pdr.c`.

## Example: Integration with PLDM Stack

```c
#include "pdr-builder.h"

// Your PDR generation function
size_t generate_my_pdrs(uint8_t *buffer, size_t max_size) {
    pdr_builder_t builder;
    pdr_builder_init(&builder, buffer, max_size, 1);
    
    // System PDRs
    pdr_build_terminus_locator(&builder, MY_TID);
    pdr_build_fru_record_set(&builder, 1, 0x0050, 1);
    
    // Your device-specific PDRs
    pdr_build_oem_entity_id(&builder, 0x6002, 12634, 2, "pid_controller");
    pdr_build_state_effecter(&builder, 1, 0x6002, 1, 0x0180 | 3, 0x03);
    
    return pdr_builder_get_offset(&builder);
}

// Use in init_pldm()
int init_pldm() {
    uint8_t pdr_data[2048];
    size_t pdr_size = generate_my_pdrs(pdr_data, sizeof(pdr_data));
    
    pdr_repo = pldm_pdr_init();
    // Add records from pdr_data...
}
```

## Notes

- All functions return 0 on error (buffer overflow)
- All multi-byte values are little-endian
- Strings are automatically converted to UTF-16LE
- State sets with OEM bit: use 0x8000 | state_set_id
- Entity types with logical class: use 0x6000 | vendor_entity_id
