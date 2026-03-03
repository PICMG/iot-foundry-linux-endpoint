# PLDM FRU Data Specification (DSP0257 v2.0.0) - Tables and Requirements

## Document Information
- **Document**: DSP0257 - Platform Level Data Model (PLDM) for FRU Data Specification
- **Version**: 2.0.0
- **Date**: 2025-05-19
- **Status**: Published
- **Supersedes**: 1.0.1

## Table 1 — PLDM FRU Record Data Format

| Size | Type | Field |
|------|------|-------|
| 2 bytes | uint16 | FRU Record Set Identifier |
| 1 byte | uint8 | FRU Record Type |
| 1 byte | uint8 | Number of FRU fields |
| 1 byte | uint8 | Encoding Type for FRU fields<br>0 = Unspecified<br>1 = strASCII<br>2 = strUTF-8<br>3 = strUTF-16<br>4 = strUTF-16LE<br>5 = strUTF-16BE<br>6–255 = reserved |
| 1 byte | uint8 | FRU Field Type #1 |
| 1 byte | uint8 | FRU Field Length #1 |
| Up to 255 bytes | Determined by FRU Field Type (see Table 4) | FRU Field #1 Value |
| 1 byte | uint8 | FRU Field #2 Type |
| 1 byte | uint8 | FRU Field #2 Length |
| Up to 255 bytes | Determined by FRU Field Type / Length (see Table 4) | FRU Field #2 Value |
| ... | ... | ... |
| 1 byte | uint8 | FRU Field #n Type |
| 1 byte | uint8 | FRU Field #n Length |
| Up to 255 bytes | Determined by FRU Field Type / Length (see Table 4) | FRU Field #n Value |

## Table 2 — PLDM FRU Record Data Table Format

| Field |
|-------|
| FRU Record Data #1 (See Table 1) |
| FRU Record Data #2 |
| FRU Record Data #3 |
| ... |
| FRU Record Data #n |

## Table 3 — FRU Record Type Definitions

| Record Type | Description |
|-------------|-------------|
| 0 | Reserved |
| 1 | General FRU Record |
| 2–253 | Reserved |
| 254 | OEM FRU Record |
| 255 | Reserved |

## Table 4 — General FRU Record Field Type Definitions

**Note**: Support for General FRU record field types 2–6 (Model, Part Number, Serial Number, Manufacturer, and Manufacture Date) are **mandatory**; support for all other General FRU record field types is optional.

| Field Type Number | Field Type Description | Field Format | Length |
|-------------------|------------------------|--------------|--------|
| 0 | Reserved | N/A | N/A |
| 1 | Chassis Type | string | 1–255 bytes |
| 2 | Model | string | 1–255 bytes |
| 3 | Part Number | string | 1–255 bytes |
| 4 | Serial Number | string | 1–255 bytes |
| 5 | Manufacturer | string | 1–255 bytes |
| 6 | Manufacture Date | timestamp104 | 13 bytes |
| 7 | Vendor | string | 1–255 bytes |
| 8 | Name | string | 1–255 bytes |
| 9 | SKU | string | 1–255 bytes |
| 10 | Version | string | 1–255 bytes |
| 11 | Asset Tag | string | 1–255 bytes |
| 12 | Description | string | 1–255 bytes |
| 13 | Engineering Change Level | string | 1–255 bytes |
| 14 | Other Information | string | 1–255 bytes |
| 15 | Vendor IANA | uint32 | 4 bytes |
| 16 | Spare Part Number | string | 1–255 bytes |
| 17–255 | Reserved | N/A | N/A |

## Table 5 — OEM FRU Record Field Type Definitions

**Note**: When the record type is set to OEM = 254, then that record shall contain one field of field type 1 that contains the Vendor IANA. Other field types 2–254 are defined by the OEM.

| Field Type Number | Field Type Description | Field Format |
|-------------------|------------------------|--------------|
| 0 | Reserved | N/A |
| 1 | Vendor IANA | uint32 |
| 2–254 | OEM specific field types | OEM specific |
| 255 | Reserved | N/A |

## Table 6 — FRU Record Table Carriage

| Byte | Type | Field |
|------|------|-------|
| Variable | — | **FRU Record Data (one or more)**<br>See Table 1 for the PLDM representation of PLDM FRU Record Data. |
| Variable | uint8[] | **Pad**<br>0 to 3 number of pad bytes. The value stored in each pad byte is 0x00.<br><br>**Transmitter algorithm**:<br>Let L be the total number of bytes in the FRU Record Data excluding the pad and the integrity checksum.<br>`if (L modulo 4 == 0) then NumPadBytes = 0; else NumPadBytes = 4 − (L modulo 4);`<br><br>**Receiver algorithm**:<br>1. The receiver first processes all FRU Record Data at the beginning of the message; the remaining bytes of the message contain the pad bytes and four bytes of data integrity checksum.<br>2. Then the size of the Pad field is the number of remaining bytes of the message minus 4. |
| 4 bytes | uint32 | **FRUDataStructureIntegrityChecksum**<br>Integrity checksum on the FRU data including the pad bytes (if any). It is calculated starting at the first byte of the PLDM representation of FRU data.<br><br>For this specification, the CRC-32 algorithm with the polynomial x³² + x²⁶ + x²³ + x²² + x¹⁶ + x¹² + x¹¹ + x¹⁰ + x⁸ + x⁷ + x⁵ + x⁴ + x² + x + 1 (the same as the one used by IEEE 802.3) shall be used for the integrity checksum computation. The CRC computation involves processing a byte at a time with the least significant bit first. |

---

## Key Requirements

### FRU Record Table Requirements

1. **String Encoding**: All strings within a FRU Record Table shall be preceded by a length variable where a length of zero indicates that the field is not used in this specific FRU Field. String lengths shall be in bytes. Strings are not null terminated and are limited to a 255-byte size.

2. **Record Set Contiguity**: FRU Record Set Identifiers and their associated record types shall appear contiguous within the FRU Record Table.

3. **OEM Record Structure**: When the record type is set to OEM = 254, then that record shall contain one field of field type 1 that contains the Vendor IANA. Other field types 2–254 are defined by the OEM.

### FRU Data Item Requirements

4. **ItemName Structure**:
   - Item Area is the most significant 16 bits of an ItemName (analogous to FRU Record Type)
   - Item Index is the least significant 16 bits of an ItemName (analogous to FRU Record Field Type)
   - Within a given FRU Item Area, each FRU Item Index shall be a number from 1 to N, assigned sequentially

5. **ItemName Mappings**:
   - FRU Item Area = 1 is equivalent to FRU Record Type = 1 (General FRU Record). Each FRU Item Index is equivalent to the corresponding FRU Field Type defined in Table 4.
   - FRU Item Area = 254 is equivalent to FRU Record Type = 254 (OEM FRU Record). Item Index = 1 is assigned "Vendor IANA".
   - All other FRU Item Areas are reserved for future definition.


### Checksum Requirements

11. **CRC-32 Algorithm**: The CRC-32 algorithm with the polynomial x³² + x²⁶ + x²³ + x²² + x¹⁶ + x¹² + x¹¹ + x¹⁰ + x⁸ + x⁷ + x⁵ + x⁴ + x² + x + 1 (the same as the one used by IEEE 802.3) shall be used for the integrity checksum computation. The CRC computation involves processing a byte at a time with the least significant bit first.

12. **Padding Algorithm**: 
    - Transmitter: `if (L modulo 4 == 0) then NumPadBytes = 0; else NumPadBytes = 4 − (L modulo 4);` where L is the total number of bytes in the FRU Record Data excluding the pad and the integrity checksum
    - Pad byte values shall be 0x00

