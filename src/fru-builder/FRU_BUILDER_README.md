# FRU Builder Library

A runtime C library for generating PLDM FRU (Field Replaceable Unit) Record Table data into buffers, following the DSP0257 specification (PLDM for FRU Data).

## Overview

This library provides functions to dynamically build FRU records at runtime, useful for firmware that needs to generate FRU data based on device configuration rather than using static arrays.

## Files

- **`fru-builder.h`** - Header file with API declarations
- **`fru-builder.c`** - Implementation (~250 lines)
- **`fru-builder-example.c`** - Example demonstrating usage

## FRU Record Format

Per DSP0257, each FRU record contains:

```
┌─────────────────────────────────────────────┐
│ FRU Record Set Identifier (2 bytes, uint16) │
├─────────────────────────────────────────────┤
│ FRU Record Type (1 byte)                    │
│   0x01 = General FRU Record                 │
│   0xFE = OEM FRU Record                     │
├─────────────────────────────────────────────┤
│ Number of FRU Fields (1 byte)               │
├─────────────────────────────────────────────┤
│ Encoding Type (1 byte)                      │
│   0x00 = Unspecified                        │
│   0x01 = ASCII, 0x02 = UTF-8                │
│   0x04 = UTF-16LE, etc.                     │
├─────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────┐ │
│ │ Field 1: Type (1 byte)                  │ │
│ │          Length (1 byte, 0-255)         │ │
│ │          Value (0-255 bytes)            │ │
│ └─────────────────────────────────────────┘ │
│ ┌─────────────────────────────────────────┐ │
│ │ Field 2: Type, Length, Value            │ │
│ └─────────────────────────────────────────┘ │
│ ...                                         │
│ ┌─────────────────────────────────────────┐ │
│ │ Field N: Type, Length, Value            │ │
│ └─────────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

## API Usage

### 1. Initialize Builder

```c
#include "fru-builder.h"

uint8_t fru_buffer[1024];
fru_builder_t builder;

fru_builder_init(&builder, fru_buffer, sizeof(fru_buffer), 1);
```

### 2. Build General FRU Records

General FRU records contain standard fields like Model, Serial Number, Manufacturer, etc.

**Standard Field Types (DSP0257 Table 4):**
- `FRU_FIELD_CHASSIS_TYPE` (1) - Chassis type string
- `FRU_FIELD_MODEL` (2) - Model number (**mandatory**)
- `FRU_FIELD_PART_NUMBER` (3) - Part number (**mandatory**)
- `FRU_FIELD_SERIAL_NUMBER` (4) - Serial number (**mandatory**)
- `FRU_FIELD_MANUFACTURER` (5) - Manufacturer name (**mandatory**)
- `FRU_FIELD_MANUFACTURE_DATE` (6) - Timestamp104 (13 bytes, **mandatory**)
- `FRU_FIELD_VENDOR` (7) - Vendor name
- `FRU_FIELD_NAME` (8) - Device name
- `FRU_FIELD_SKU` (9) - SKU string
- `FRU_FIELD_VERSION` (10) - Version string
- `FRU_FIELD_ASSET_TAG` (11) - Asset tag
- `FRU_FIELD_DESCRIPTION` (12) - Description
- `FRU_FIELD_ECL` (13) - Engineering Change Level
- `FRU_FIELD_OTHER_INFO` (14) - Other information
- `FRU_FIELD_VENDOR_IANA` (15) - Vendor IANA (4 bytes, uint32)
- `FRU_FIELD_SPARE_PART_NUM` (16) - Spare part number

**Example:**

```c
const char* model = "IoT-Endpoint-v1";
const char* part_num = "12345-67890";
const char* serial = "SN20260216001";
const char* mfg = "PICMG";

fru_field_t fields[4];
fru_field_string(&fields[0], FRU_FIELD_MODEL, model, strlen(model));
fru_field_string(&fields[1], FRU_FIELD_PART_NUMBER, part_num, strlen(part_num));
fru_field_string(&fields[2], FRU_FIELD_SERIAL_NUMBER, serial, strlen(serial));
fru_field_string(&fields[3], FRU_FIELD_MANUFACTURER, mfg, strlen(mfg));

size_t bytes = fru_build_general_record(&builder, FRU_ENCODING_ASCII, 
                                        fields, 4);
// Returns: 57 bytes written
```

### 3. Build OEM FRU Records

OEM records must start with a Vendor IANA field, followed by OEM-defined fields.

**OEM Field Types:**
- `FRU_FIELD_OEM_VENDOR_IANA` (1) - **Mandatory first field** (4 bytes)
- `FRU_FIELD_OEM_DEFINED_MIN` (2) through (254) - OEM-specific fields

**Example:**

```c
const uint32_t picmg_iana = 12634;  // PICMG IANA number
const char* oem_data = "CustomData";
const uint8_t binary_data[4] = {0x00, 0x02, 0x04, 0x08};

fru_field_t oem_fields[2];
fru_field_string(&oem_fields[0], FRU_FIELD_OEM_DEFINED_MIN, 
                 oem_data, strlen(oem_data));
fru_field_binary(&oem_fields[1], FRU_FIELD_OEM_DEFINED_MIN + 1,
                 binary_data, 4);

size_t bytes = fru_build_oem_record(&builder, FRU_ENCODING_UTF8,
                                    picmg_iana, oem_fields, 2);
// Returns: 21 bytes written (includes auto-added IANA field)
```

### 4. Helper Functions

**String Fields:**
```c
fru_field_t field;
fru_field_string(&field, FRU_FIELD_NAME, "DeviceName", 10);
```

**Binary Fields:**
```c
uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
fru_field_t field;
fru_field_binary(&field, FRU_FIELD_OTHER_INFO, data, 8);
```

**Timestamp Fields (Manufacture Date):**
```c
uint8_t timestamp[13] = { /* 13-byte timestamp104 data */ };
fru_field_t field;
fru_field_timestamp(&field, timestamp);
```

**uint32 Fields (e.g., Vendor IANA):**
```c
uint8_t iana_bytes[4] = {0x5A, 0x31, 0x00, 0x00};  // 12634 in LE
fru_field_t field;
fru_field_uint32(&field, FRU_FIELD_VENDOR_IANA, iana_bytes);
```

### 5. Get Results

```c
size_t total_bytes = fru_builder_get_offset(&builder);
uint32_t num_records = fru_builder_get_record_count(&builder);

// Use fru_buffer with total_bytes length
```

## Example Output

Running `fru-builder-example`:

```
Generating FRU Record Table...

General FRU Record 1: 30 bytes
  Fields: Name="Microsam", Manufacture Date (13 bytes)

OEM FRU Record 2: 21 bytes
  Vendor IANA: 12634 (PICMG)
  OEM Fields: String="10", Binary=[4 bytes]

General FRU Record 3 (with standard fields): 57 bytes
  Model: IoT-Endpoint-v1
  Part Number: 12345-67890
  Serial Number: SN20260216001
  Manufacturer: PICMG

Total bytes: 108
Total records: 3

First 64 bytes (hex):
0x01, 0x00, 0x01, 0x02, 0x02, 0x08, 0x08, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x61, 0x6d, 0x06,
0x0d, 0x0d, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x02, 0x00,
0xfe, 0x03, 0x02, 0x01, 0x04, 0x5a, 0x31, 0x00, 0x00, 0x02, 0x02, 0x31, 0x30, 0x03, 0x04, 0x00,
0x02, 0x04, 0x08, 0x03, 0x00, 0x01, 0x04, 0x01, 0x02, 0x0f, 0x49, 0x6f, 0x54, 0x2d, 0x45, 0x6e,
```

## Decoding First Record

```
Byte Offset  Value   Meaning
-----------  -----   -------
0-1          0x0001  Record Set Identifier = 1
2            0x01    Record Type = General FRU
3            0x02    Number of Fields = 2
4            0x02    Encoding Type = UTF-8

5            0x08    Field 1 Type = Name (8)
6            0x08    Field 1 Length = 8 bytes
7-14         "Microsam"

15           0x06    Field 2 Type = Manufacture Date (6)
16           0x0d    Field 2 Length = 13 bytes
17-29        [timestamp data]

Total: 30 bytes
```

## Integration with Your Firmware

### Replace Static FRU Data

In your `init_pldm()` or equivalent initialization function:

```c
#include "fru-builder.h"

extern uint8_t __fru_data[];  // Remove or keep as fallback
extern size_t __fru_size;

int init_fru_table() {
    uint8_t fru_buffer[2048];
    fru_builder_t builder;
    
    fru_builder_init(&builder, fru_buffer, sizeof(fru_buffer), 1);
    
    // Build records based on your device config
    const char* model = get_device_model();
    const char* serial = get_device_serial();
    // ... etc.
    
    fru_field_t fields[5];
    int field_count = 0;
    
    fru_field_string(&fields[field_count++], FRU_FIELD_MODEL, 
                     model, strlen(model));
    fru_field_string(&fields[field_count++], FRU_FIELD_SERIAL_NUMBER,
                     serial, strlen(serial));
    // Add more fields...
    
    fru_build_general_record(&builder, FRU_ENCODING_ASCII, 
                             fields, field_count);
    
    // Copy to __fru_data or register with PLDM
    size_t fru_size = fru_builder_get_offset(&builder);
    memcpy(__fru_data, fru_buffer, fru_size);
    __fru_size = fru_size;
    
    return 0;
}
```

### Dynamic FRU Based on JSON Config

```c
#include "fru-builder.h"
#include <cjson/cJSON.h>

int build_fru_from_json(const char* json_config, 
                        uint8_t* fru_buffer, size_t buffer_size) {
    cJSON* root = cJSON_Parse(json_config);
    if (!root) return -1;
    
    fru_builder_t builder;
    fru_builder_init(&builder, fru_buffer, buffer_size, 1);
    
    // Extract fields from JSON
    cJSON* device = cJSON_GetObjectItem(root, "device");
    const char* model = cJSON_GetStringValue(
        cJSON_GetObjectItem(device, "model"));
    const char* serial = cJSON_GetStringValue(
        cJSON_GetObjectItem(device, "serialNumber"));
    // ... etc.
    
    // Build FRU record
    fru_field_t fields[10];
    int count = 0;
    
    if (model) {
        fru_field_string(&fields[count++], FRU_FIELD_MODEL,
                         model, strlen(model));
    }
    if (serial) {
        fru_field_string(&fields[count++], FRU_FIELD_SERIAL_NUMBER,
                         serial, strlen(serial));
    }
    
    fru_build_general_record(&builder, FRU_ENCODING_UTF8, fields, count);
    
    cJSON_Delete(root);
    return fru_builder_get_offset(&builder);
}
```

## Building

### Compile the Library

```bash
cd tools
gcc -c fru-builder.c -o build/fru-builder.o -Wall
```

### Build and Run Example

```bash
gcc fru-builder-example.c fru-builder.c -o build/fru-builder-example -Wall
./build/fru-builder-example
```

### Link with Your Project

```bash
gcc your-program.c fru-builder.c -o your-program
```

Or in CMakeLists.txt:
```cmake
add_library(fru-builder fru-builder.c)
target_link_libraries(your-target fru-builder)
```

## Key Features

✓ **No dependencies** - Pure C, no external libraries  
✓ **Buffer-based** - Writes directly to your buffer  
✓ **Safe** - Checks buffer overflow, returns 0 on error with rollback  
✓ **Flexible** - Supports both General and OEM records  
✓ **Compliant** - Follows DSP0257 specification exactly  

## Limitations

- Maximum field length: 255 bytes (per spec)
- Maximum fields per record: 255 (per spec)
- No automatic endianness conversion for user data (you must provide little-endian)
- Strings are NOT null-terminated in FRU records (length-prefixed)

## Comparison: Static vs. Runtime

| Aspect | Static Arrays (config.c) | FRU Builder (runtime) |
|--------|-------------------------|----------------------|
| **Generation** | Build-time | Runtime |
| **Flexibility** | Fixed at compile | Dynamic based on config |
| **Dependencies** | None | None |
| **Memory** | ROM/Flash | RAM (can copy to static) |
| **Use Case** | Fixed configuration | Variable/JSON-driven config |
| **Integration** | Direct array reference | Function calls |

## References

- **DSP0257 v2.0.0** - PLDM for FRU Data Specification
- **DSP0240** - PLDM Base Specification
- **DSP0248** - PLDM Platform Monitoring and Control (FRU Record Set PDR)

## License

Apache-2.0

---

**Note:** This library generates the FRU Record Table data only. The FRU Record Set PDR (PDR Type 0x14) that references this FRU data must be generated separately using the `pdr-builder` library.
