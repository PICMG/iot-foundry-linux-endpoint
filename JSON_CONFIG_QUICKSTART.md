# JSON Configuration - Quick Start Guide

## Overview

The IoT Foundry Linux Endpoint requires a JSON configuration file to define PDR and FRU data. Configuration is loaded at runtime using the `-c` or `--config` command line option.

**Note:** JSON configuration is **mandatory**. The endpoint will not start without a valid JSON config file.

## Command Line Usage

### Basic Syntax
```bash
./endpoint [serial options] -c config.json
```

**Note:** The `-c` option is required. The endpoint will not start without it.

### With JSON Configuration (Required)
```bash
# Using pty with JSON config
./endpoint -p -c example-simple-endpoint.json

# Using serial device with JSON config
./endpoint -d /dev/ttyUSB0 -c my-device-config.json

# JSON config with other options
./endpoint -d /dev/ttyUSB0 -b 115200 -f -c device.json
```

### Error: No Config File
```bash
# Without -c option: ERROR
./endpoint -p
```
**Output:**
```
Error: JSON configuration file required.
Use -c option to specify configuration file.
Example: ./endpoint -p -c example-simple-endpoint.json
```

## What Happens

### Startup Process:
1. Checks for `-c` option (mandatory)
2. **Validates JSON file against PICMG IOT.1 schema**
3. Loads specified JSON file
4. Generates PDR data using pdr-builder library
5. Generates FRU data using fru-builder library
6. Uses runtime-generated data for all operations

**Console output:**
```
Validating configuration file: example-simple-endpoint.json
JSON validation passed
Generated 6 PDR records (167 bytes) from example-simple-endpoint.json
Generated 1 FRU records (54 bytes) from example-simple-endpoint.json
Configuration loaded successfully
Configuring PDRs from runtime config (6 records)
PDRs configured successfully (6 records, 167 bytes)
```

### Without `-c` Option:
**Console output:**
```
Error: JSON configuration file required.
Use -c option to specify configuration file.
```
Endpoint exits with error code 1.

### Invalid JSON Configuration:
**Console output:**
```
Validating configuration file: invalid-config.json
JSON validation failed: Missing required field: 'channels'
```
Endpoint exits with error code 1.

## Available Example Configurations

### 1. Simple Endpoint
```bash
./endpoint -p -c example-simple-endpoint.json
```
- Basic digital I/O
- Simple sensors and effecters
- 6 I/O bindings, 1 OEM state set

### 2. PID Controller
```bash
./endpoint -p -c example-pid-endpoint.json
```
- PID controller functionality
- Numeric sensors and effecters
- 11 bindings (mix physical/virtual), 3 OEM state sets

### 3. Profiled Motion Controller
```bash
./endpoint -p -c example-profiled-motion-endpoint.json
```
- Motion control with stepper motors
- Position, velocity, acceleration control
- 14 bindings, 3 OEM state sets

## Testing

### Test 1: Verify Option Exists
```bash
./endpoint -h
```
Look for: `-c, --config <file>    JSON configuration file (REQUIRED)`

### Test 2: Run with JSON (Required)
```bash
./endpoint -p -c example-simple-endpoint.json
```
Expected: Should show "Loading configuration from..." and "Configuring PDRs from runtime config"

### Test 3: Run without JSON (Should Fail)
```bash
./endpoint -p
```
Expected: Should show error "JSON configuration file required" and exit

### Test 4: Invalid JSON File
```bash
./endpoint -p -c nonexistent.json
```
Expected: Should show error "Failed to load JSON configuration" and exit

## How It Works

```
┌─────────────────────────────────────────────────┐
│ Command Line: endpoint -c config.json           │
└───────────────────┬─────────────────────────────┘
                    │
                    ↓
┌─────────────────────────────────────────────────┐
│ config_load_from_json(config.json)              │
│ ├─ Validates JSON against PICMG IOT.1 schema   │
│ ├─ Reads JSON file                              │
│ ├─ pdr_builder: generates PDR data              │
│ └─ fru_builder: generates FRU data              │
└───────────────────┬─────────────────────────────┘
                    │
                    ↓
┌─────────────────────────────────────────────────┐
│ runtime_config (global)                         │
│ ├─ pdr_data (dynamically allocated)             │
│ ├─ pdr_data_size                                │
│ ├─ fru_data (dynamically allocated)             │
│ ├─ fru_data_size                                │
│ └─ loaded = true                                │
└───────────────────┬─────────────────────────────┘
                    │
                    ↓
┌─────────────────────────────────────────────────┐
│ init_pldm()                                     │
│ ├─ get_pdr_data()                               │
│ │  └─ Returns runtime data if loaded            │
│ │     Otherwise returns static __pdr_data       │
│ └─ Populates PDR repository                     │
└─────────────────────────────────────────────────┘
```

## Key Features

- ✅ **Schema Validation** - Validates JSON against PICMG IOT.1 schema
- ✅ **Runtime Configuration** - No recompilation needed
- ✅ **Multiple Configs** - Different JSON files for different devices
- ✅ **Required Config** - Ensures consistent configuration management
- ✅ **Clear Error Messages** - Helpful feedback when config is missing
- ✅ **Memory Safe** - Dynamically allocated, properly freed

## Current Status

### Working Now
- **JSON schema validation** - Validates against PICMG IOT.1 schema
- JSON file loading and validation (required)
- PDR generation using builder library
- FRU generation using builder library
- Clear error messages for missing/invalid config

### Generates (Placeholder)
- Basic system PDRs (Terminus Locator, FRU Record Set, Entity Association)
- Example sensors and effecters
- Generic FRU records

### To Be Extended
Full JSON parsing to map:
- JSON channels → PDR sensors/effecters
- JSON device info → FRU fields
- JSON bindings → Entity relationships

## Troubleshooting

### Issue: "JSON configuration file required"
**Cause:** No `-c` option provided
**Solution:** Always use `-c` option with a valid JSON file path

### Issue: "JSON validation failed"
**Cause:** JSON file missing required fields or has invalid structure
**Solution:** 
- Check error message for specific field that's missing/invalid
- Ensure JSON has all required fields: device, pins, channels, maxFruBytes, fruRecords, logicalEntities, oemStateSets
- Verify structure matches PICMG IOT.1 schema
- See `src/json-validator/README.md` for schema requirements

### Issue: "Failed to load JSON configuration"
**Cause:** JSON file doesn't exist or can't be opened
**Solution:** Check file path is correct, file has read permissions

### Issue: No PDRs configured

## Next Steps

1. **Use as-is** - Infrastructure works, generates basic PDRs
2. **Extend JSON parsing** - Implement full channel-to-PDR mapping
3. **Create custom configs** - Write JSON files for your devices
4. **Test configurations** - Validate with PLDM tools

## Help

```bash
./endpoint -h
```

Shows full command line options including the new `-c` flag.

## Documentation

- **JSON_CONFIG_IMPLEMENTATION.md** - Detailed implementation guide
- **src/config_loader.h** - API documentation
- **tools/PDR_BUILDER_README.md** - PDR builder API
- **tools/FRU_BUILDER_README.md** - FRU builder API
