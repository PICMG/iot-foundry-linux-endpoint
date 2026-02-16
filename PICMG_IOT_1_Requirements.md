# PICMG IOT.1 R1.00 - Requirements and Specification Tables

**Source:** IIoT Firmware Specification (August 24, 2021)
**Last Updated:** Reorganized with interleaved tables

---

### Table 2: Parameters for Trapezoidal Motion Profile

| Parameter Name | Description | Typical Units |
|---|---|---|
| Aprofile | The acceleration of the motor in the acceleration phase of the trapezoidal motion profile. For s-curve motion, this parameter is the average acceleration reached in the acceleration phase. | rad/sec2 |
| Vprofile | The velocity of the motor in the constant-velocity region of the motion profile | rad/sec |
| Pfinal | The final position of the motor after completion of the motion. | rad |

**REQ 4.10**:  IIoT Endpoint **shall** implement an MCTP endpoint accessible through the point-topoint communications link.

**REQ 4.20**:  IIoT Endpoint **shall** comply with requirements applicable for simple MCTP Endpoint defined in [DMTF-DSP0236].

**REQ 4.30**:  IIoT Endpoint **shall** accept 0xFF (MCTP Base specification version) Message Type Number in the Get MCTP Version Support request, and response according to the [DMTF-DSP0236].

**REQ 4.40**:  IIoT Endpoint **shall** accept 0x00 (MCTP Control protocol version) Message Type Number in the Get MCTP Version Support request, and response according to the [DMTF-DSP0236]. 

**REQ 4.50**:  IIoT Endpoint **shall** accept 0x01 (PLDM over MCTP binding specification version) Message Type Number in the Get MCTP Version Support request, and response with a single Version Number Entry 0xF1F0F000.

**REQ 4.60**:  IIoT Endpoint **shall** report MCTP Control (0x00 message type in the Get Message Type Support command response.

**REQ 4.70**:  IIoT Endpoint **shall** report PLDM (0x01) message type in the Get Message Type Support command response.

**REQ 4.80**:  IIoT Endpoint **may** support other MCTP message types.

**REQ 4.90**:  IIoT Endpoint **shall** not generate any unsolicited messages unless the generation of each message type was previously enabled.

**REQ 4.100**: IIoT Node **shall** announce its presence on the bus by sending the “Discovery Notify” command request whenever it is powered up or undergoes a reset unless the underlying MCTP binding has its own means of signaling the device appearance to the bus owner. 

**REQ 4.110**: The MCTP message transfer unit **shall** be 64 bytes (not including header, length, and footer fields). 

**REQ 4.120**:  IIoT Endpoint **shall** implement a PLDM terminus over the associated MCTP endpoint.

**REQ 4.130**:  IIoT Endpoint **shall** expose Device PDR repository.

**REQ 4.140**:  IIoT Endpoint **shall** comply with requirements defined in [DMTF-DSP0240].

**REQ 4.150**:  IIoT Endpoint **shall** use [DMTF-DSP0241] for binding PLDM messages over MCTP.

**REQ 4.160**:  IIoT Endpoint **shall** use 64 byte transmission unit size for all PLDM messages.

**REQ 4.170**: IIoT Endpoint **shall** report PLDM type 000000b (PLDM Messaging Control and Discovery) as supported in the GetPLDMTypes command response.

**REQ 4.180**: IIoT Endpoint **shall** report PLDM 000010b (PLDM for Platform Monitoring and Control) as supported in the GetPLDMTypes command response.

**REQ 4.190**: IIoT Endpoint **shall** report PLDM type 000100b (PLDM for FRU Data) as supported in the GetPLDMTypes command response.

**REQ 4.200**: IIoT Endpoint **shall** implement PLDM for Platform Monitoring and Control commands marked as Mandatory by [DMTF-DSP0248].

**REQ 4.210**: IIoT Endpoint **shall** implement PLDM for FRU Data Specification commands marked as Mandatory by [DMTF-DSP0257].

**REQ 4.220**:  IIoT Endpoints **shall** support reporting of events. 

**REQ 4.230**: IIoT Endpoint **should** support asynchronous event message generation as defined in [DMTF-DSP0248].

**REQ 4.240**:  IIoT Endpoint **shall** contain exactly one Terminus Locator PDR.

**REQ 4.250**: The Terminus Locator PDR **shall** have a container id of 0x0000 (overall system), indicating it resides at the top level of the Sensor/Effecter Endpoint

**REQ 4.260**:  IIoT Endpoint **shall** contain exactly one Terminus Locator PDR.

**REQ 4.270**:  IIoT Endpoint **shall** not expose any Numeric Sensor Initialization PDRs.

**REQ 4.280**:  IIoT Endpoint **shall** not expose any State Sensor Initialization PDRs.

**REQ 4.290**:  IIoT Endpoint **shall** not expose any Numeric Effecter Initialization PDRs.

**REQ 4.300**:  IIoT Endpoint **shall** not expose any State Effecter Initialization PDRs.

**REQ 4.310**:  IIoT Endpoint Device PDR repository **shall** be statically configured. 

**REQ 4.320**: IIoT Endpoints **may** simplify their communications by assuming that only one client request will be received at a time. 

**REQ 4.330**:  The PLDM Event Generator **shall** comply with requirements found in [DMTF-DSP0248].

**REQ 4.340**:  An IIoT Node **shall** implement an MCTP Bridge.

**REQ 4.350**: The MCTP Bridge **shall** comply with requirements applicable for MCTP Bus Owner/Bridge defined in [DMTF-DSP0236].

**REQ 4.360**:  The MCTP bridge **shall** maintain static routing entries for all implemented MCTP Endpoints.

**REQ 4.370**: Each IIoT Endpoint on an IIoT Node **shall** comply with the MCTP and PLDM requirements for an IIoT Endpoint as described in sections 4.1.1 through 4.1.2.

**REQ 4.380**:  All IIoT Endpoints on a Physical Node **shall** implement Get PLDM Terminus UID command. 

**REQ 4.390**:  An IIoT Endpoint **shall** only expose PDRs for the Logical Node associated with the Endpoint.

**REQ 4.400**: An IIoT Endpoint **shall** only accept commands for the resources associated with the PLDM terminus.

**REQ 4.410**: All IIoT Endpoints on an IIoT Node **shall** report the same Entity Type for the container entity of the associated Logical Nodes. 

**REQ 4.420**:  The IIoT Endpoint **shall** not maintain state through a power-cycle. 

### Table 3: Terminus Locator PDR
| Offset | Length | Definition |
|---|---|---|
|0|4|Record Handle|
|4|1|PDR Header Version. For all records defined in this specification avalue of 01h shall be used.|
|5|1|PDR Type. For the Terminus Locator PDR, the value 01h is used.|
|6|2|recordChangeNumber. Consult [DMTF-0248] for more information.|
|8|2|dataLength – the total number of PDR bytes following this field.|
|10|2|PLDM Terminus Handle. For this specification, the value 0001h is used.|
|12|1|Validity. For this specification, the value 01h (valid) is used.|
|13|1|TID. For this specification, the value 01h (unassigned) is used.|
|14|2|Container ID. For this specification, the value 0001h is used.|
|16|1|Terminus Locator Type.<br>00h = UID<br>01h = MCTP_EID <br> Other Terminus Locator Types are not used for this specification.|
|17|2|Terminus Locator Value Size. Depends on Terminus Locator Type. <br>Use 0011h (17) for UID Terminus Locator Type. <br>Use 0001h (1) for MCTP_EID Terminus Locator Type.|
|Terminus Locator Value for Terminus Locator Type = UID|
|18|1|Terminus Instance.|
|19|16|Device UID.|
|Terminus Locator Value for Terminus Locator Type = MCTP_EID |
|35|1|EID. For this specification, the value 01h (unassigned) is used.|

### Table 4: Entity Association PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the Entity Association PDR, the value 15h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | Container ID. For this specification, the value 0001h is used. |
| 12 | 1 | Association Type. For this specification, the value 01h (logicalContainment) is used. |
| | | **Container Entity Identification Information** |
| 13 | 2 | Container Entity Type. Defines type of the entity that the Endpoint terminus. |
| 15 | 2 | Container Entity Instance Number. For this specification, the value 0001h is used. |
| 17 | 2 | Container Entity Container ID. For this specification, the value 0000h (SYSTEM) is used. |
| | | **Contained Entity Identification Information** |
| 19 | 1 | Contained Entity Count. For this specification, the value 0001h is used. |
| 20 | 2 | Contained Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 22 | 2 | Contained Entity Instance Number. For this specification, the value 0001h is used. |
| 24 | 2 | Contained Entity Container ID. For this specification, the value 0001h is used. |

### Table 5: OEM EntityID PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the OEM Entity PDR, the value 15h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | OEM Entity ID Handle. For this specification, the value 6000h is used. |
| 14 | 4 | Vendor IANA. For this specification, the value 12634 is used. |
| 18 | 2 | Vendor Entity ID.<br>0001h = Simple Sensor/Effecter<br>0002h = PID control<br>0003h = Profiled Motion control<br>Other values are not used for this specification. |
| 20 | 1 | String Count. For this specification, the value 01h is used. |
| 21 | 3 | Entity ID Language. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 24 | Variable | Entity ID Name. A null-terminated Unicode string in UTF-16BE<br>format.<br>“Simple” for Vendor Entity ID = 0001h.<br>“PID” for Vendor Entity ID = 0002h.<br>“Profiled” for Vendor Entity ID = 0003h. |

### Table 6: FRU Record Set PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the Entity Association PDR, the value 20h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | FRU Record Set Identifier. For this specification, the value 0001h is used. |
| 14 | 2 | Entity Type. For this specification, the value must match the Container Entity Type value in the Entity Association PDR. See Table 4. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |

**REQ 4.430**:  Each PLDM terminus **shall** provide Device PDR repository.

**REQ 4.440**:  Table 3. The Device PDR repository **shall** contain a Terminus Locator PDR as depicted in

**REQ 4.450**:  Table 4. The Device PDR repository **shall** contain an Entity Association PDR as depicted in

**REQ 4.480**: Each Sensor-related and Effecter-related PDR in the Device PDR Repository **shall** have PLDMTerminusHandle field equal to 1.

**REQ 4.490**: Each Sensor-related or Effecter-related PDR **shall** have entityType field equal to Logical|6000h.

**REQ 4.500**:  equal to 1. Each Sensor-related or Effecter-related PDR **shall** have entityInstanceNumber field

**REQ 4.510**:  Each Sensor-related or Effecter-related PDR **shall** have containerID field equal to 1. 

**REQ 4.540**:  in Table 9. The Device PDR repository **shall** contain a State Sensor PDR for Trigger as depicted

### Table 9: Trigger State Sensor PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. Offset Length |
| 5 | 1 | PDR Type. For the State Sensor PDR, the value 04h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Sensor ID. For this specification, the value 0002h is used. |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 1 | Sensor Init. For this specification, the value 00h (noInit) is used. |
| 21 | 1 | Sensor Auxiliary Names PDR. |
| 22 | 1 | Composite Sensor Count. For this specification, the value 01h is used. Sensor Possible States |
| 23 | 2 | State Set ID. This is a unique value in the OEM state set range such that it references a Trigger OEM State Set PDR. |
| 25 | 1 | Possible State Size. For this specification, the value 01h is used. |
| 26 | 1 | Possible States. For this specification, the value 03h is used. in Table 10. The Device PDR repository shall contain a State Effecter PDR for Trigger as depicted |

**REQ 4.550**:  in Table 10. The Device PDR repository **shall** contain a State Effecter PDR for Trigger as depicted

### Table 10: Trigger State Effecter PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the State Effecter PDR, the value 0Bh is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Effecter ID. For this specification, the value 0002h is used. Offset Length |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 2 | Effecter Semantic ID. For this specification, the value 0000h (unspecified) is used. |
| 22 | 1 | Effecter Init. For this specification, the value 00h (noInit) is used. |
| 23 | 1 | Effecter Description PDR. |
| 24 | 1 | Composite Effecter Count. For this specification, the value 01h is used. Effecter Possible States |
| 25 | 2 | State Set ID. This is a unique value in the OEM state set range such that it references a Trigger OEM State Set PDR. |
| 27 | 1 | Possible State Size. For this specification, the value 01h is used. |

**REQ 4.570**:  Sensor readings **shall** be linearized by the IIoT Node firmware.

**REQ 4.580**:  Effecter settings **shall** be linearized by the IIoT Node firmware. NOTE: it is expected that the firmware build process will combine response curves from the sensor/effecter and the input/output stages of the IIoT Node to perform the linearization. The IIoT Node is not required to use all the sample points present in the configuration tables, however, if fewer points are used for linearization, the accuracy reported in the Sensor/Effecter PDR **should** be updated to reflect the overall measurement/control accuracy. 

**REQ 4.590**: The simple Sensor/Effecter Endpoint **shall** include a OEM Entity ID for a PICMG Simple Sensor/Effecter OEM Entity.

**REQ 4.600**: The simple Sensor/Effecter Endpoint **shall** include at least one of the following PDRs: Numeric Sensor PDR, State Sensor PDR, Numeric Effecter PDR, State Effecter PDR.

**REQ 4.610**: Sensor/Effecter PDRs **shall** be associated with the PICMG Simple Sensor/Effecter OEM Entity. NOTE: Other PDRs **may** be present within the simple Sensor/Effecter Endpoint so long as they do not prevent meeting the stated requirements.  

**REQ 4.620**: The PICMG Simple Sensor/Effecter OEM Entity ID PDR **shall** have a entity id of 1, indicating a simple Sensor/Effecter Endpoint. 

**REQ 4.630**:  The following Sensor IDs **shall** be reserved for specific Sensors (if present):

### Table 12: Sensor IDs

| ID | Sensor Description|
|---|---|
| 1 | Global Interlock state Sensor|
| 2 | Trigger state Sensor |

**REQ 4.640**:  The following Effecter IDs **shall** be reserved for specific Effecters (if present):

### Table 13: Effecter IDs

|ID|Effecter Description|
|---|---|
|1|Global Interlock state Effecter|
|2|Trigger state Effecter|

**REQ 4.650**:  Effecters that can control physical devices **should** be disabled during start-up. NOTE: The intent is that outputs are placed in a “safe” state until configured for operation. NOTE: The “safe” state for a given Effecter **may** depend on its operating context but will, in general, be a condition in which 1) no additional energy is added to the Effecter; and 2) the Effecter is allowed to passively dissipate any kinetic and/or thermal energy it already has, without active intervention by the Endpoint Controller. 

**REQ 4.660**:  PID Endpoint **shall** support the following configuration parameters:

### Table 14: PID Endpoint Configuration Parameters

|Configuration Parameter Name |Description |Type|
|---|---|---|
|GlobalInterlockEffecter | A structure that defines the binding between the global interlock Effecter and specific device pin(s). For more information on I/O bindings, consult section 5 of this specification.|I/O Binding|
|GlobalInterlockSensor|A structure that defines the binding between the global interlock Sensor and specific device pin(s). For more information on I/O bindings, consult section 5 of this specification.|I/O Binding|
|TriggerEffecter|A structure that defines the binding between the I/O Binding trigger Effecter and specific device pin(s). For more information on I/O bindings, consult section 5 of this specification. | I/O Binding |
|TriggerSensor|A structure that defines the binding between the trigger Sensor and specific device pin(s). For more information on I/O bindings, consult section 5 of this specification.|I/O Binding|

**REQ 4.670**: The Simple Sensor/Effecter model **shall** support additional I/O bindings for each Sensor/Effecter supported by the IIoT Node.

**REQ 4.680**: Names for Sensor I/O bindings **shall** begin with “Sensor“ followed by a base-10 numeric identifier.

**REQ 4.690**: Names for Effecter I/O bindings **shall** begin with “Effecter“ followed by a base-10 numeric identifier. 

**REQ 4.700**: The PID controller Endpoint **shall** include an OEM Entity ID PDR for a PICMG PID Controller OEM Entity.

**REQ 4.710**:  The PID controller endpoint **shall** include PDRs as described in section 4.3.1.

**REQ 4.720**:  The PID controller endpoint **shall** include PDRs as described in section 4.3.2.

**REQ 4.730**: The PID controller Endpoint **shall** include an OEM State Set PDR for the PICMG OEM PID Controller State Set as described in Table 15. 

### Table 15: PID Controller OEM State Set PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the OEM State Set PDR, the value 08h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | OEM State Set ID Handle. A unique value in the OEM state set range. |
| 14 | 4 | Vendor IANA. For this specification, the value 12634 is used. |
| 18 | 2 | OEM State Set ID. For this specification, the value 0002h is used. |
| 20 | 1 | Unspecified Value Hint. For this specification, the value 01h (treatAsError) is used. |
| 21 | 2 | State Count. For this specification, the value 04h is used. OEM State Value Record |
| 23 | 1 | Min State Value. For this specification, the value 01h is used. |
| 24 | 1 | Max State Value. For this specification, the value 01h is used. |
| 25 | 1 | String Count. For this specification, the value 01h is used. |
| 26 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 29 | 10 | State Name. A null-terminated Unicode string “Idle” in UTF-16BE format. OEM State Value Record |
| 39 | 1 | Min State Value. For this specification, the value 02h is used. |
| 40 | 1 | Max State Value. For this specification, the value 02h is used. |
| 41 | 1 | String Count. For this specification, the value 01h is used. |
| 42 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 45 | 30 | State Name. A null-terminated Unicode string “ConditionStop” in UTF-16BE format. OEM State Value Record |
| 75 | 1 | Min State Value. For this specification, the value 03h is used. Offset Length |
| 76 | 1 | Max State Value. For this specification, the value 03h is used. |
| 77 | 1 | String Count. For this specification, the value 01h is used. |
| 78 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 81 | 20 | State Name. A null-terminated Unicode string “ErrorStop” in UTF-16BE format. OEM State Value Record |
| 101 | 1 | Min State Value. For this specification, the value 04h is used. |
| 102 | 1 | Max State Value. For this specification, the value 04h is used. |
| 103 | 1 | String Count. For this specification, the value 01h is used. |
| 104 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 107 | 16 | State Name. A null-terminated Unicode string “Running” in UTF-16BE format. PID Controller Command State Set as described in Table 16. |

**REQ 4.740**: The PID controller Endpoint **shall** include an OEM Sate Set PDR for the PICMG OEM PID Controller Command State Set as described in Table 16.

### Table 16: PID Controller Command OEM State Set PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the OEM State Set PDR, the value 08h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | OEM State Set ID Handle. A unique value in the OEM state set range. |
| 14 | 4 | Vendor IANA. For this specification, the value 12634 is used. |
| 18 | 2 | OEM State Set ID. For this specification, the value 0003h is used. |
| 20 | 1 | Unspecified Value Hint. For this specification, the value 01h (treatAsError) is used. |
| 21 | 2 | State Count. For this specification, the value 02h is used. OEM State Value Record Offset Length |
| 23 | 1 | Min State Value. For this specification, the value 01h is used. |
| 24 | 1 | Max State Value. For this specification, the value 01h is used. |
| 25 | 1 | String Count. For this specification, the value 01h is used. |
| 26 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 29 | 12 | State Name. A null-terminated Unicode string “Start” in UTF-16BE format. OEM State Value Record |
| 41 | 1 | Min State Value. For this specification, the value 02h is used. |
| 42 | 1 | Max State Value. For this specification, the value 02h is used. |
| 43 | 1 | String Count. For this specification, the value 01h is used. |
| 44 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 47 | 10 | State Name. A null-terminated Unicode string “Stop” in UTF-16BE format. PID Controller State as described in Table 17. |

**REQ 4.750**: The PID controller Endpoint **shall** include a State Sensor PDR for the PICMG OEM PID Controller State as described in Table 17.

### Table 17: PID Controller State Sensor PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the State Sensor PDR, the value 04h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Sensor ID. For this specification, the value 0003h is used. |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 1 | Sensor Init. For this specification, the value 00h (noInit) is used. Offset Length |
| 21 | 1 | Sensor Auxiliary Names PDR. |
| 22 | 1 | Composite Sensor Count. For this specification, the value 01h is used. Sensor Possible States |
| 23 | 2 | State Set ID. A value that references a PID Controller Operational State Set OEM State Set PDR. |
| 25 | 1 | Possible State Size. For this specification, the value 01h is used. |
| 26 | 1 | Possible States. For this specification, the value 0Fh is used. PID Controller Command Effecter as described in Table 18. |

**REQ 4.760**: The PID controller Endpoint **shall** include a State Effecter PDR for the PICMG OEM PID Controller Command Effecter as described in Table 18.

### Table 18: PID Controller Command Effecter PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the State Effecter PDR, the value 0Bh is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Effecter ID. For this specification, the value 0003h is used. |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 2 | Effecter Semantic ID. For this specification, the value 0000h (unspecified) is used. |
| 22 | 1 | Effecter Init. For this specification, the value 00h (noInit) is used. |
| 23 | 1 | Effecter Description PDR. |
| 24 | 1 | Composite Effecter Count. For this specification, the value 01h is used. Effecter Possible States Offset Length |
| 25 | 2 | State Set ID. A value that references a PID Controller Command OEM State Set PDR. |
| 27 | 1 | Possible State Size. For this specification, the value 01h is used. |
| 28 | 1 | Possible States. For this specification, the value 03h is used. NOTE: Other PDRs may be present within the PID Controller Endpoint so long as they do not prevent meeting the stated requirements. |

**REQ 4.770**: The PICMG PID Controller OEM Entity ID PDR **shall** have an entity id of 2, indicating a PID Controller Endpoint. 

**REQ 4.780**: The PICMG OEM PID Controller operational state **shall** implement state transitions as show in the following table: 

### Table 19: PID Controller State Transitions

|State|Event|Next State|
|---|---|---|
|Idle|Command: Start|Running|
|Running|Command: Stop|Idle|
||Error Sensor over critical threshold|ConditionStop|
||Feedback Sensor over critical threshold|ConditionStop|
||Trigger Sensor is active|ConditionStop|
||Error Sensor over fatal threshold|ErrorStop|
||Feedback Sensor over fatal threshold|ErrorStop|
||Global interlock Sensor is active|ErrorStop|
|ComnditionStop|Command: Stop|Idle|
||Error Sensor over fatal threshold|ErrorStop|
||Feedback Sensor over fatal threshold|ErrorStop|
||Global interlock Sensor is active|ErrorStop|
|ErrorStop|Command: Stop|Idle|

**REQ 4.790**: Events that could cause transitions to ErrorStop **shall** be given precedence over all other conditions.

**REQ 4.800**: Events that could cause transitions to ConditionStop **shall** be given precedence over all other conditions except those that could cause a transition to ErrorStop.

**REQ 4.810**: In all states other than Running, the PID controller **shall** be placed into a “safe” state; see notes on “safe” states in section 4.3.4.5. NOTE: In order to transition back to idle from either ConditionStop or ErrorStop, all error conditions must be cleared. 

**REQ 4.820**:  The PID Controller **shall** implement a Global Interlock State Sensor. 

**REQ 4.830**:  The PID Controller **shall** implement a Trigger State Sensor.

**REQ 4.840**:  The PID Controller **shall** implement a PID Control Error numeric Sensor.

**REQ 4.850**: The base units of the PID Control Error numeric Sensor **shall** be the same as the Setpoint Effecter units. NOTE: This Sensor **should** return the real-time difference between the setpoint and the actual controlled value.

**REQ 4.860**:  The PID Controller **shall** implement a OperationalState virtual state Sensor.

**REQ 4.870**: The OperationalState Sensor **shall** return the state of the PID controller state machine as represented by a PID Controller Operational State Set value.

**REQ 4.880**:  The following Sensor IDs **shall** be reserved for specific Sensors (if present):

### Table 20: PID Controller Sensor IDs

|ID|Sensor Description|
|---|---|
|1|Global Interlock state Sensor|
|2|Trigger state Sensor|
|3|PID OperationalState Sensor|
|4|PID Control Error numeric Sensor|
|5|Feedback numeric Sensor|

**REQ 4.890**:  Sensor PDRs **shall** be associated with the PICMG PID Controller Entity.

**REQ 4.900**:  The PID Controller **shall** implement a Global Interlock State Effecter.

**REQ 4.910**:  The PID Controller **shall** implement a Trigger State Effecter.

**REQ 4.920**:  The PID Controller **shall** implement a PID Command Effecter.

**REQ 4.930**: The PID Command Effecter **shall** implement a PICMG OEM PID Controller Command State Set.

**REQ 4.940**:  The PID Controller **shall** implement a Proportional Gain numeric Effecter.

**REQ 4.950**: The base units of the Proportional Gain numeric Effecter **shall** be ((Control Output Units)/(Setpoint Units)). NOTE: It is recommended that the resolution and offset Effecter fields be used to provide for fractional precision.

**REQ 4.960**:  The PID Controller **shall** implement an Integral Gain numeric Effecter.

**REQ 4.970**: The base units of the Integral Gain numeric Effecter **shall** be ((Control Output Units)/(Setpoint Units))*seconds.  NOTE: If this Effecter is implemented with an integer data type, it is recommended that the resolution and offset Effecter fields be used to provide for fractional precision. NOTE: The resolution field for the Effecter **may** include a scaling ratio to convert from seconds to sample periods, simplifying the computations by the controller.

**REQ 4.980**:  The PID Controller **shall** implement a Differential Gain numeric Effecter.

**REQ 4.990**: The base units of the Differential Gain numeric Effecter **shall** be ((Control Output Units)/(Setpoint Units))/seconds. NOTE: If this Effecter is implemented with an integer data type, it is recommended that the resolution and offset Effecter fields be used to provide for fractional precision. NOTE: The resolution field for the Effecter **may** include a scaling ratio to convert from seconds to sample periods, simplifying the computations by the controller.

**REQ 4.1000**: The PID Controller **shall** implement a Setpoint numeric Effecter.

**REQ 4.1010**: The following Effecter IDs **shall** be reserved for specific Effecters (if present)

### Table 21: PID Controller Effecter IDs

|ID|Effecter Description|
|---|---|
|1|Global Interlock state Effecter|
|2|Trigger state Effecter|
|3|Command Effecter|
|4|Setpoint numeric Effecter|
|5|Proportional gain numeric Effecter|
|6|Integral gain numeric Effecter|
|7|Differential gain numeric Effecter|

**REQ 4.1020**: PID Controller Endpoint **shall** support the following configuration IOBinding objects:

### Table 22: PID Controller IOBindings

|IOBinding Name|Description|I/O Binding Type|“virtual” Field Value|“required” Field Value|Include in PDR|
|---|---|---|---|---|---|
|GlobalInterlockEffecter|Global interlock Effecter|State Effecter|false|true|true|
|GlobalInterlockSensor|Global interlock Sensor|State Sensor|false|true|true|
|TriggerEffecter|Trigger Effecter|State Effecter|false|true|true|
|TriggerSensor|Trigger Effecter|State Sensor|false|true|true|
|Feedback|The PID feedback Sensor|Numeric Effecter|false|true|false|
|OutputEffecter|the PID output Effecter|Numeric Effecter|false|true|false|
|Command|The command Effecter|State Effecter|true|true|true|
|OperationalState|The operational state Sensor|State Sensor|true|true|true|
|ProportionalGain|The proportional gain Effecter|Numeric Effecter|true|true|true|
|IntegralGain|The integral gain Effecter|Numeric Effecter|true|true|true|
|Setpoint|The PID setpoint Effecter|Numeric Effecter|true|true|true|
|Error|The position error virtual Sensor|Numeric Sensor|true|false|true|

**REQ 4.1030**: PID Endpoint **shall** support the following configuration requirements:

### Table 23: PID Controller Configurations

|Configuration Parameter Name|Description|Type|
|---|---|---|
|SampleRate|The sample rate of the PID controller expressed in Hertz. The sample rate controls the rate at which the feedback signal is sampled and the output signal is updated.|Type|

**REQ 4.1040**: The Profiled Motion controller Endpoint **shall** include an OEM Entity ID PDR for a PICMG Profiled Motion Controller OEM Entity.

**REQ 4.1050**: The Profiled Motion controller Endpoint **shall** include PDRs as described in section 4.3.1.

**REQ 4.1060**: The Profiled Motion controller Endpoint **shall** include PDRs as described in section 4.3.2.

**REQ 4.1070**: The Profiled Motion controller Endpoint **shall** include an OEM State Set PDR for the PICMG OEM Profiled Motion Controller State Set as described in Table 24.

### Table 24: Profiled Motion Controller OEM State Set PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the OEM State Set PDR, the value 08h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. Offset Length |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | OEM State Set ID Handle. A unique value in the OEM state set range. |
| 14 | 4 | Vendor IANA. For this specification, the value 12634 is used. |
| 18 | 2 | OEM State Set ID. For this specification, the value 0004h is used. |
| 20 | 1 | Unspecified Value Hint. For this specification, the value 01h (treatAsError) is used. |
| 21 | 2 | State Count. For this specification, the value 07h is used. OEM State Value Record |
| 23 | 1 | Min State Value. For this specification, the value 01h is used. |
| 24 | 1 | Max State Value. For this specification, the value 01h is used. |
| 25 | 1 | String Count. For this specification, the value 01h is used. |
| 26 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 29 | 10 | State Name. A null-terminated Unicode string “Idle” in UTF-16BE format. OEM State Value Record |
| 39 | 1 | Min State Value. For this specification, the value 02h is used. |
| 40 | 1 | Max State Value. For this specification, the value 02h is used. |
| 41 | 1 | String Count. For this specification, the value 01h is used. |
| 42 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 45 | 30 | State Name. A null-terminated Unicode string “ConditionStop” in UTF-16BE format. OEM State Value Record |
| 75 | 1 | Min State Value. For this specification, the value 03h is used. |
| 76 | 1 | Max State Value. For this specification, the value 03h is used. |
| 77 | 1 | String Count. For this specification, the value 01h is used. |
| 78 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 81 | 20 | State Name. A null-terminated Unicode string “ErrorStop” in UTF-16BE format. OEM State Value Record |
| 101 | 1 | Min State Value. For this specification, the value 04h is used. Offset Length |
| 102 | 1 | Max State Value. For this specification, the value 04h is used. |
| 103 | 1 | String Count. For this specification, the value 01h is used. |
| 104 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 107 | 16 | State Name. A null-terminated Unicode string “RunningV” in UTF-16BE format. OEM State Value Record |
| 123 | 1 | Min State Value. For this specification, the value 05h is used. |
| 124 | 1 | Max State Value. For this specification, the value 05h is used. |
| 125 | 1 | String Count. For this specification, the value 01h is used. |
| 126 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 129 | 18 | State Name. A null-terminated Unicode string “RunningP” in UTF-16BE format. OEM State Value Record |
| 147 | 1 | Min State Value. For this specification, the value 06h is used. |
| 148 | 1 | Max State Value. For this specification, the value 06h is used. |
| 149 | 1 | String Count. For this specification, the value 01h is used. |
| 150 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 159 | 16 | State Name. A null-terminated Unicode string “Waiting” in UTF-16BE format. OEM State Value Record |
| 169 | 1 | Min State Value. For this specification, the value 07h is used. |
| 170 | 1 | Max State Value. For this specification, the value 07h is used. |
| 171 | 1 | String Count. For this specification, the value 01h is used. |
| 172 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 175 | 10 | State Name. A null-terminated Unicode string “Done” in UTF-16BE format. |

**REQ 4.1080**: The Profiled Motion controller Endpoint **shall** include an OEM Sate Set PDR for the PICMG OEM Profiled Motion Controller Command State Set as described in Table 25. 

### Table 25: Profiled Motion Controller Command OEM State Set PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the OEM State Set PDR, the value 08h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | OEM State Set ID Handle. A unique value in the OEM state set range. |
| 14 | 4 | Vendor IANA. For this specification, the value 12634 is used. |
| 18 | 2 | OEM State Set ID. For this specification, the value 0005h is used. |
| 20 | 1 | Unspecified Value Hint. For this specification, the value 01h (treatAsError) is used. |
| 21 | 2 | State Count. For this specification, the value 03h is used. OEM State Value Record |
| 23 | 1 | Min State Value. For this specification, the value 01h is used. |
| 24 | 1 | Max State Value. For this specification, the value 01h is used. |
| 25 | 1 | String Count. For this specification, the value 01h is used. |
| 26 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 29 | 12 | State Name. A null-terminated Unicode string “Start” in UTF-16BE format. OEM State Value Record |
| 41 | 1 | Min State Value. For this specification, the value 02h is used. |
| 42 | 1 | Max State Value. For this specification, the value 02h is used. |
| 43 | 1 | String Count. For this specification, the value 01h is used. |
| 44 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 47 | 10 | State Name. A null-terminated Unicode string “Stop” in UTF-16BE format. OEM State Value Record |
| 57 | 1 | Min State Value. For this specification, the value 03h is used. |
| 58 | 1 | Max State Value. For this specification, the value 03h is used. Offset Length |
| 59 | 1 | String Count. For this specification, the value 01h is used. |
| 60 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 63 | 12 | State Name. A null-terminated Unicode string “Wait” in UTF-16BE format. |

**REQ 4.1090**: The Profiled Motion controller Endpoint **shall** include a State Sensor PDR for the PICMG OEM Profiled Motion Controller State Sensor as described in Table 26.

### Table 26: Profiled Motion Controller State Sensor PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the State Sensor PDR, the value 04h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Sensor ID. For this specification, the value 0003h is used. |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 1 | Sensor Init. For this specification, the value 00h (noInit) is used. |
| 21 | 1 | Sensor Auxiliary Names PDR. |
| 22 | 1 | Composite Sensor Count. For this specification, the value 01h is used. Sensor Possible States |
| 23 | 2 | State Set ID. A value that references a Profiled Motion Controller OEM State Set PDR. |
| 25 | 1 | Possible State Size. For this specification, the value 01h is used. |
| 26 | 1 | Possible States. For this specification, the value 7Fh is used. Profiled Motion Controller Command Effecter as described in Table 27. |

**REQ 4.1100**: The PID controller Endpoint **shall** include a State Effecter PDR for the PICMG OEM Profiled Motion Controller Command Effecter as described in Table 27. 

### Table 27: Profiled Motion Controller Command Effecter PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the State Effecter PDR, the value 0Bh is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Effecter ID. For this specification, the value 0003h is used. |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 2 | Effecter Semantic ID. For this specification, the value 0000h (unspecified) is used. |
| 22 | 1 | Effecter Init. For this specification, the value 00h (noInit) is used. |
| 23 | 1 | Effecter Description PDR. |
| 24 | 1 | Composite Effecter Count. For this specification, the value 01h is used. Effecter Possible States |
| 25 | 2 | State Set ID. A value that references a Profiled Motion Controller Command OEM State Set PDR. |
| 27 | 1 | Possible State Size. For this specification, the value 01h is used. |
| 28 | 1 | Possible States. For this specification, the value 07h is used. NOTE: Other PDRs may be present within the Profiled Motion Controller Endpoint so long as they do not prevent meeting the stated requirements. |

**REQ 4.1110**: The PICMG Profiled Motion Controller OEM Entity ID PDR **shall** have an entity id of 3, indicating a Profiled Motion Controller Endpoint. 

**REQ 4.1120**: Start commands **shall** not be accepted if Vprofile or Aprofile Effecters are disabled.

**REQ 4.1130**: When a new motion is initiated by a Start or Wait command, the current values of Pfinal, Vprofile, and Aprofile **shall** be latched for use for the motion. NOTE: Further changes **may** be made to the Effecter, but will not change the motion until a Start or Wait command is sent.

**REQ 4.1140**: If the Pfinal Effecter is enabled at the time a Start or Wait command is initiated from the Idle state, the motion profile generated will be for a profiled position move.

**REQ 4.1150**: When the PICMG OEM Motion Controller is operating in position control mode, the output command **shall** control the velocity of the motor to follow the velocity, acceleration and position from the profile generator.

**REQ 4.1160**: If a Stop command is received by the controller when in the RunningP state, the controller **shall** slew to zero velocity as specified the value of Aprofile set when the motion was begun.

**REQ 4.1170**: If the Pfinal Effecter is disabled at the time a Start or Wait command is initiated from the Idle state, the motion profile generated will be for a profiled velocity move (no final position).

**REQ 4.1180**: If a Start command is received by the controller when in the RunningV state, the controller **shall** slew to the new velocity as specified by most recently latched values of Vprofile and Aprofile.

**REQ 4.1190**: If a Stop command is received by the controller when in the Running state for a profiled velocity move, the controller **shall** slew to zero velocity as specified the value of Aprofile set when the motion was begun.

**REQ 4.1200**: The PICMG OEM Profiled Motion Controller operational state **shall** implement state transitions as shown in the following table:

### Table 28: Profiled Motion Controller State Transitions

|State|Event|Next State|
|---|---|---|
|Idle| Command: Start & Pfinal Effecter enabled|RunningP|
||Command: Start & Pfinal Effecter disabled|RunningV|
||Command: Wait|Waiting|
|RunningP|Command: Stop|Idle|
||Final Position Reached (profiled position motion only)|Done|
||Command: Start (profiled velocity move only)|Running|
||PositiveLimit Sensor (if it is enabled) is Pressed/On and motion is in the positive direction|ConditionStop|
||NegativeLimit Sensor Condition (if it is enabled) is Pressed/On and motion is in the negative direction|Stop|
||Perror or Verror Sensor over criticalthreshold|ConditionStop|
||Position or Velocity Sensor over critical threshold|ConditionStop|
||Trigger Sensor is active|ConditionStop|
||Perror or Verror Sensor over fatal threshold|ErrorStop|
||Position or Velocity Sensor over critical threshold|ErrorStop|
||Feedback Sensor over fatal threshold|ErrorStop|
||Global interlock Sensor is active|ErrorStop|
|RunningV|Command: Stop|Idle|
||Command: Start|RunningV, latching in new Vprofile and Aprofile|
||PositiveLimit Sensor (if it is enabled) is Pressed/On and motion is in the positive direction|ConditionStop|
||NegativeLimit Sensor (if it is enabled) is Pressed/On and motion is in the negative direction|ConditionStop|
|Waiting|Perror or Verror Sensor over critical threshold|ConditionStop|
||Position or Velocity Sensor over critical threshold|ConditionStop|
||Trigger Sensor is active|ConditionStop|
||Perror or Verror Sensor over fatal threshold|ErrorStop|
||Position or Velocity Sensor over critical threshold|ErrorStop|
||Feedback Sensor over fatal threshold|ErrorStop|
||Global interlock Sensor is active|ErrorStop|
||Trigger Sensor is positive edge|ConditionStop|
||Trigger Sensor|negative edge & Pfinal was enabled at transition to Waiting state|RunningP|
||Trigger Sensor negative edge & Pfinal was disabled at transition to Waiting state| RunningV |
||Global interlock Sensor is active|ErrorStop|
||Command: Stop|Idle|
|Done|Command: Stop|Idle|
|ConditionStop|Error Sensor over fatal threshold|ErrorStop|
||Feedback Sensor over fatal threshold|ErrorStop|
||Global interlock Sensor is active|ErrorStop|
||Command: Stop|Idle|
|ErrorStop|Command: Stop|Idle|

**REQ 4.1210**: Events that could cause transitions to ErrorStop **shall** be given precedence over all other conditions.

**REQ 4.1220**: Events that could cause transitions to ConditionStop **shall** be given precedence over all other conditions except those that could cause a transition to ErrorStop.

**REQ 4.1230**: In all states other than Running, the Profiled Motion controller **shall** control the output and brake as directed by the associated configuration parameters. NOTE: The motion controller **may** transition back to Idle from either ConditionStop or ErrorStop, even if error conditions exist. Some error conditions (for instance, Perror), **may** require sending a new motion command to recover. Allowing transitions back to Idle prevents a deadlock state where an error condition exists, but it cannot be cleared. 

**REQ 4.1240**: The Profiled Motion Controller **shall** implement a Global Interlock State Sensor.

**REQ 4.1250**: The Profiled Motion Controller **shall** implement a Trigger State Sensor.

**REQ 4.1260**: The Profiled Motion Controller **shall** implement a Perror (Position Error) numeric Sensor when configured for PI-V operation.

**REQ 4.1270**: The base units of the Perror numeric Sensor **shall** be the same as the Pfinal Effecter units. NOTE: This Sensor **should** return the real-time difference between the position setpoint from the profile generator and the actual motor position. 

**REQ 4.1280**: The Profiled Motion Controller **shall** implement a Verror (Velocity Error) numeric Sensor when configured for PI-V operation.

**REQ 4.1290**: The base units of the Verror numeric Sensor **shall** be the same as the Vprofile Effecter units. NOTE: This Sensor **should** return the real-time difference between the velocity setpoint from the profile generator and the actual or estimated motor velocity value.

**REQ 4.1300**: The Profiled Motion Controller **shall** implement a MotionState virtual state Sensor.

**REQ 4.1310**: The MotionState Sensor **shall** return the state of the Profiled Motion controller state machine as represented by a PICMG OEM Profiled Motion Controller Motion State Set value.

**REQ 4.1320**: The units of the PositiveLimit Sensor (if present) **shall** be implemented with State Set ID 67 (Switch State).

**REQ 4.1330**: The units of the NegativeLimit Sensor (if present) **shall** be implemented with State Set ID 67 (Switch State).

**REQ 4.1340**: The units of the Velocity Sensor (if present) **shall** be the same as the Vprofile Effecter. NOTE: The velocity Sensor is intended to return a real-time reading of the actual (or estimated) motor velocity.

**REQ 4.1350**: The units of the Position Sensor (if present) **shall** be the same as the Pfinal Effecter. NOTE: The position Sensor is intended to return a real-time reading of the actual motor position.

**REQ 4.1360**: The following Sensor IDs **shall** be reserved for specific Sensors (if present)

### Table 29: Profiled Motion Controller Sensor IDs

|ID|Sensor Description|
|---|---|
|1|Global Interlock state Sensor|
|2|Trigger state Sensor|
|3|MotionState state Sensor|
|4|Verror numeric Sensor|
|5|Perror numeric Sensor|
|6|Velocity numeric Sensor|
|7|Position numeric Sensor|
|8|PositiveLimit state Sensor|
|9|NegativeLimit state Sensor|

**REQ 4.1370**: The Profiled Motion Controller **shall** implement a Global Interlock State Effecter.

**REQ 4.1380**: The Profiled Motion Controller **shall** implement a Trigger State Effecter. 

**REQ 4.1390**: The Profiled Motion Controller **shall** implement a Command virtual state Effecter.

**REQ 4.1400**: The Command Effecter **shall** accept state transitions commands as represented by a PICMG OEM Profiled Motion Controller Command State Set value.

**REQ 4.1410**: The Profiled Motion Controller **shall** implement a Pfinal virtual numeric Effecter.

**REQ 4.1420**: The value of Pfinal upon exit from the IDLE state **shall** be used by the profile generator as the target position for a profiled move.

**REQ 4.1430**: The PLDM resolution parameter **should** be used for the Pfinal Effecter to convert to/from the device’s raw units.

**REQ 4.1440**: The Profiled Motion Controller **shall** implement a Vprofile virtual numeric Effecter. NOTE: It is recommended that the resolution and offset Effecter fields be used to provide for fractional precision.

**REQ 4.1450**: The value of the Vprofile Effecter upon exit from the IDLE state **shall** be used by the profile generator as the target velocity for the profiled move. NOTE: For position moves, where the distance traveled is very small relative to the acceleration, Vprofile **may** not actually be reached.

**REQ 4.1460**: The units of Vprofile **shall** be the (units of Pfinal) / second.

**REQ 4.1470**: The PLDM resolution parameter **should** be used for the Vprofile Effecter to convert to/from the device’s raw units.

**REQ 4.1480**: The Profiled Motion Controller **shall** implement an Aprofile virtual numeric Effecter. NOTE: If this Effecter is implemented with an integer data type, it is recommended that the resolution and offset Effecter fields be used to provide for fractional precision.

**REQ 4.1490**: The value of the Aprofile Effecter upon exit from the IDLE state **shall** be used by the profile generator as the average acceleration during the acceleration phase of the profiled move and the average deceleration during the deceleration phase of the profiled move. NOTE: Interpreting Aprofile as an average value means that trapezoidal and s-curve motions with the same parameters will spend an equal amount of time in the acceleration and deceleration phases of motion as each other.

**REQ 4.1500**: The sign of the Aprofile value **shall** be ignored. NOTE: Only the magnitude is required. The sign of the velocity will be used to interpret the direction of acceleration/deceleration.

**REQ 4.1510**: The units of Aprofile **shall** be the (units of Pfinal) / second2.

**REQ 4.1520**: The PLDM resolution parameter **should** be used for the Aprofile Effecter to convert to/from the device’s raw units.

**REQ 4.1530**: The value of the AccelerationGain Effecter (if implemented) **shall** be used by the PIV controller as the acceleration feedforward gain. NOTE: This parameter allows for tuning of the PIV response when loading at the output changes.

**REQ 4.1540**: The sign of the AccelerationGain Effecter value (if implemented) **shall** be ignored. NOTE: Only the magnitude is required. 

**REQ 4.1550**: The units of AcclerationGain Effecter (if implemented) **shall** be the (PIV output units)/((units of Pfinal) / second2).

**REQ 4.1560**: The PLDM resolution parameter **should** be used for the AccelerationGain Effecter (if implemented) to convert to/from the device’s raw units.

**REQ 4.1570**: The following Effecter IDs **shall** be reserved for specific Effecters (if present):

### Table 30: Profiled Motion Controller Effecter IDs

|ID|Effecter Description|
|---|---|
|1|Global Interlock state Effecter|
|2|Trigger state Effecter|
|3|Command Effecter|
|4|Pfinal numeric Effecter|
|5|Vprofile numeric Effecter|
|6|Aprofile numeric Effecter|
|7|AccelerationGain numeric Effecter|

**REQ 4.1580**: Profiled Motion Controller Endpoint **shall** support the following configuration IOBinding objects:

### Table 31: Profiled Motion Controller IOBindings

|IOBinding Name|Description|I/O Binding Type|“virtual” Field Value|“required” Field Value|Include in PDR|
|---|---|---|---|---|---|
|GlobalInterlockEffecter|Global interlock Effecter|State Effecter|false|true|true|
|GlobalInterlockSensor|Global interlock Sensor|State Sensor|false|true|true|
|TriggerEffecter|Trigger Effecter|State Effecter|false|true|true|
|TriggerSensor|Trigger Effecter |State Sensor|false|true|true|
|PositionSensor|The positional feedback Sensor|Numeric Effecter|false|false (open loop), true (closed loop)|true| 
|VelocitySensor|The virtual velocity feedback Sensor|Numeric Effecter|true|false|true|
|PositiveLimit|the positive limit Sensor |State Sensor|false|false|true|
|NegativeLimit|the negative limit Sensor|State Sensor|false |false|true|
|OutputEffecter|the PIV output Effecter|Numeric Effecter|false|true|false|
|OutputEnable|An effecter that enables the output Effecter|State Effecter|true|false|false|
|BrakeEffecter|the brake Effecter|State Effecter|false|false|false|
|Command|The command Effecter|State Effecter|true|true|true|
|MotionState |The motion state Sensor|State Sensor|true|true|true|
|AccelerationGain|The acceleration gain Effecter|Numeric Effecter|true|false|true|
|Pfinal|The final position virtual Effecter|Numeric Effecter|true|true|true|
|Vprofile|The profile velocity virtual Effecter|Numeric Effecter|true|true|true|
|Aprofile|The profile acceleration virtual Effecter|Numeric Effecter|true|true|true|
|Verror|The velocity error virtual Sensor|Numeric Sensor|true|false (open loop), true (closed loop)|true|
|Perror|The position error virtual Sensor|Numeric Sensor|true|false (open loop), true (closed loop)|true|

**REQ 4.1590**: Profiled Motion Controller Endpoint **shall** support the following configuration requirements:

### Table 32: Profiled Motion Controller Configurations

|Configuration Parameter Name|Description|Type|
|---|---|---|
|SampleRate|The sample rate of the Profiled Motion Controller expressed in Hertz. The sample rate controls the rate at which the profile is generated, feedback signals are sampled and the output signal is updated.|Numeric|
|OutputInIdle|This parameter defines the behavior of the output while in the Idle state. Valid options are:<br>0: COAST – the output will be disabled<br>1: HOLD - the output will hold the current position<br>2: BRAKE – the brake will be engaged|Enumeration:|
|OutputInConditionStop|This parameter defines the behavior of the output while in the ConditionStop state. Valid options are:<br>0: COAST – the output will be disabled<br>1: HOLD - the output will hold the current position<br>2: BRAKE – the brake will be engaged|Enumeration|
|OutputInErrorStop|This parameter defines the behavior of the output while in the ErrorStop state. Valid options are:<br>0: COAST – the output will be disabled<br>1: HOLD - the output will hold the current position<br>2: BRAKE – the brake will be engaged|Enumeration|
|DoneTimeConstant|The amount of time between when the profile generator reaches the final position and the controller state machine transitions to DONE. This parameter can be used to unsure that the motion has “settled” prior to signaling the completion of a move.|Numeric|
|MotionProfile|This parameter defines the shape of the profiled move. Valid options are:<br>0: Trapezoid<br>1: S-Curve|Enumeration|

**REQ 4.1600**: When configured for closed-loop operation, the Profiled Motion Controller Endpoint **shall** support the following additional configuration parameters:

### Table 33: Profiled Motion Controller Closed-Loop Configurations

|Configuration Parameter Name|Description|Type|
|---|---|---|
|PositionLoopLimit|A unitless constant that expresses the maximum/minimum value that will pass through the limiter in the position loop of the PIV controller (in device units).|Numeric|
|PositionProportionalGain|A unitless constant that expresses the proportional gain in the position loop of the PIV controller.|Numeric|
|VelocityProportionalGain A unitless constant that expresses the proportional gain in the velocity loop of the PIV controller.|Numeric|
|VelocityIntegralGain|A unitless constant that expresses the integral gain in the velocity loop of the PIV controller.|Numeric|
|VelocityDifferentialGain|A unitless constant that expresses the differential gain in the velocity loop of the PIV controller|Numeric|
|VelocityFFGain|A unitless constant that expresses the feed forward gain for velocity.|Numeric|
|OutputGain|A unitless constant that scales the output of the PIV controller|Numeric|

**REQ 4.1610**: Other parameters **may** be present for device configuration. 4.4 Firmware Update Requirements As part of periodic maintenance, it **may** be necessary to update Endpoint firmware. For the typical IIoT installation, this update would not be performed with the equipment in the operational state, however, it **may** be convenient for the operator to leave the Endpoint in-place within the factory equipment while performing the updates. The exact mechanism of firmware update is not specified by this document.

**REQ 4.1620**: The Endpoint **shall** support the minimum set of Firmware Update commands described in [DMTF-DSP0267].

**REQ 4.1630**: Firmware updates **should** only be possible when the Interlock signal is externally asserted. NOTE: This ensures that the device is in a “safe” state prior to firmware updates. 

### Section 5

### Table 35: ControllerCapabilities object structure

|Keyword|Description|Nullable| Data Type|
|---|---|---|---|
|device|The targeted hardware device for this build. This field can be used to direct the build process for the correct hardware platform.|No|String|
|pins|An array of unique String names for I/O pins on the device.|No|Array of String|
|channels|An array of channels supported by the device|No|Array of Channel objects|
|maxFruBytes|The maximum number of bytes of FRU data the device can support.|No|Unsigned Integer|
|fruRecords|An array of FRU record objects that are associated with the device’s physical entity|No|Array of fruRecord objects|
|logicalEntities|An array of all possible logical entity objects supported by the device. The device does not need to support all logical entities at the same time.|No|Array of LogicalEntity objects|

**REQ 5.10**:  The controller capabilities file **shall** consist of a single ControllerCapabilities object as defined defined in Table 35.

**REQ 5.20**:  The controller capabilities file **shall** include at least one LogicalEntity object within the logicalEntities array.

**REQ 5.30**:  Other fields **may** be present in the ControllerCapabilities object. 

### Table 36: Channel object structure

|Keyword|Description|Nullable |Data Type|
|---|---|---|---|
|name|A unique name that identifies the channel from all other possible channels on the device.|No|String|
|type|The type of physical interface this channel is associated. More on electrical interfaces can be found in section 5.2.1.1|No|String|
|description|An optional description of the channel that may be helpful to the Sensor/Effecter integrator. If not used, this field may be null. It is recommended that this field be used to provide guidance around IOBinding input/output curves.|Yes|String|
|maxValueAtPin|The maximum quantity that can be measured or output at the device pin. The units of this field varies by electrical interface type.<br>- analog interface types: A voltage in Volts<br>- pwm interface types: a duty cycle between 0 and 100<br>- rate interface types: A frequency in Hertz<br>- all other interface types – not used, set to 0|No|Numeric|
|minValueAtPin|The minimum quantity that can be measured or output at the device pin. The units of this field varies by electrical interface type.<br>- analog interface types: A voltage in Volts<br>- pwm interface types: a duty cycle between 0 and 100<br>- rate interface types: A frequency in Hertz<br>- all other interface types – not used, set to 0|No|Numeric|
|precision|The number of precision bits associated with this channel. For instance, a 10-bit analog to digital converter would have a value of 10. Negative values indicate that the internal representation of the channel is signed. A 10 bit analog to digital converter that has a signed output would have a value of -10. For digital_in/out, count, quadrature, and step_dir_enable interface types this field should be set to 0.|No|Integer|
|accuracy|An error component that scales linearly with the magnitude of the reading, expressed as a percentage (1.0 = 1%). |No|Numeric|
|pins|An array of PinUse objects associated with this channel. In many cases pins may be shared between multiple channels; however, channels that share pins cannot both be configured for use since pins may only serve one function at a time.|No|Array of PinUse objects|

**REQ 5.40**:  Channel objects **shall** consist of the JSON structure defined in Table 36. 

**REQ 5.50**:  Channel type strings **shall** consist of one of the electrical interface type names found Table 37. 

### Table 37: Channel Electrical Interface Types

|Electrical Interface Type Name|Description|
|---|---|
|digital_in|A digital input interface corresponding to a single device digital input pin.|
|analog_in|An analog input interface corresponding to a single device analog input pin.|
|count_in|A digital input interface associated with a single digital input pin. The controller counts positive edge transitions on this channel and keeps a running total of the transitions.|
|rate_in|A digital input interface associated with a single digital input pin. The controller counts positive edge transitions on the channel over time and estimates the frequency of the edge transitions from this information. The sampled representation of this electrical type will be an estimate of the edge transitions per second (with possible scaling). |
|quadrature_in|A digital input interface consisting of two pins: “A”, and “B”, that constitute a quadrature input signal. The controller keeps track of the absolute “position” based on transitions of the input signals relative to each other. Note that the quadrature input interface does not support an index (“Z”) input. Implementations require a index pulse can use a separate digital input channel for this purpose. The sampled representation of this electrical interface type will be the sum of pulses in the positive direction minus the sum of pulses in the negative direction (with possible scaling).|
|analog_out|An analog output interface consisting of a single analog output pin pin that produces a voltage that is proportional to the controller’s internal representation of the value.|
|digital_out|A digital output interface associated with a single digital output pin.
|pwm_out||A digital output interface intended to be used for pulse-widthmodulation. The quantity produced at the output pin of the controller is a pulse with a varying duty cycle that is proportional to the controller’s internal representation of the value. This interface type is associated with a single pin.|
|rate_out|A digital output interface that generates a pulse train with processorcontrolled output rate. This channel is associated with a single pin.|
|step_dir_out|A digital output interface intended to control stepper motors. The interface consists of three pins. The first pin will send pulses to the motor to make it move one step. The second pin controls the direction of the motor. The third pin enables the motor driver. This interface type is always expected to be used in conjunction with a controller (PID, Profiled Motion Controller) logical entity. Control of stepper motors manually can be accomplished by combining rate_out and digital_out electrical interfaces.|

### Table 38: PinUse object structure

|Keyword |Description|Nullable |Data Type|
|---|---|---|---|
|name|The name of the pin – the pin name should match one found under the “pins” keyword of the controller capabilities.|No|String|
|function|A description or name of the pin use when this channel is selected. This information may be useful when connecting physical devices to the controller.|No|String|

**REQ 5.60**:  PinUse objects **shall** consist of the structure found in Table 38.

**REQ 5.70**:  PinUse objects **shall** be used to ensure that channels that use the same pins cannot be configured for use at the same time.

**REQ 5.80**:  The value for the name field in the PinUse object **shall** match one of the strings found under the pins keyword of its associated ControllerCapabilities object. 

### Table 39: FruRecord object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| Type | vendorIANA The vendor IANA for the vendor that defined the fru record type. For IANA will be 12634, for general FRU records, the value should be 412 (DMTF) | No | Integer |
| required | true if this entity must always be present in the device’s configuration. This must be set to true for FruRecords in the ControllerConfiguration | No | Boolean |
| description | A text description of the fru record that may be useful to the sensor/effecter integerator. | Yes | String |
| fields | An array of FruField objects that define each field in the FruRecord. | No | Array of |

**REQ 5.90**:  FruRecord objects **shall** comply with the structure defined in Table 39.

**REQ 5.100**: All FruRecord objects within the ControllerCapabilities object **shall** have the required field set to true. 

### Table 40:  FruField object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| type | This metadata defines the numeric field type for this field. Field types are specified in DSP0257 | No | Integer |
| required | true if this field must always be present in the device’s configuration. | No | Boolean |
| description | A text description of the fru field that may Yes be useful to the sensor/effecter integrator. | Yes | String |
| format | The data format for the field. Valid options include a subset of those defined in DSP0240 (“uint8”, “sint8”, “uint16”, “sint16”, “uint32”, “sint32”, “uint64”, “sint64”, “string”, “bool8”, “real32”, “real64”, “timestamp104”), and “bytes” which is represented by a jsonArray of unsigned 8-bit integers. | No | String |
| length | The required length of byte-formatted fields. This field must be specified for fields with the format of “bytes”. For other types of fields it will be ignored and may be null. | Yes | Numeric or null |
| value | The value of the field. | Yes | Dependent upon the format field of this object2. |

**REQ 5.110**:  All FruField objects **shall** comply with the structure defined in Table 40.

**REQ 5.120**: All FruRecord objects within the ControllerCapabilities object **shall** be present in the final device configuration if their required field is true.

**REQ 5.130**: FruField fields that are non-null in within the ControllerCapabilities object **shall** not be modified in the resulting device configuration.

**REQ 5.140**:  FruField objects that have a “bytes” format **shall** have a non-null length value.

**REQ 5.150**:  FruField objects that do not have a “bytes” format **shall** have a null length value.

**REQ 5.160**: If specified, the number of bytes in the FruField objects value **shall** match the number specified by length. 

### Table 41:  LogicalEntity object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| entityVendorIANA | The vendor IANA for the vendor that defined the entity type. For PICMG, the vendor IANA will be 12634 | No | Integer |
| vendorEntityID | The vendor entity id associated with this entity. | No | Integer |
| name | A unique name that identifies this possible instance of the entity. For instance, “pid1” for the first possible instance of a pid controller instance. | No | String |
| required | true if this entity must always be present in the device’s configuration. | No | Boolean |
| description | A text description of the logical entity that Yes may be useful to the Sensor/Effecter integerator. | Yes | String |
| ioBindings | An array of IOBinding objects that define how Sensors/Effecters for this LogicalEntity are bound to device channels. | No | Array of IOBinding objects |
| parameters | An array of Parameter objects that define configuration parameter options for the LogicalEntity | No | Array of Parameter objects |

**REQ 5.170**:  LogicalEntity objects **shall** consist of the structure found in Table 41.

**REQ 5.180**: The value of the vendorIANA and vendorID fields within the LogicalEntity object **shall** match their corresponding fields in the desired OEM Entity ID PDR.

**REQ 5.190**: The value for the name field in the LogicalEntity object **shall** be unique across all LogicalEntity objects. 

**REQ** If the value of the required field within the LogicalEntity object is true, the LogicalEntity shall be required be present in any resulting configuration of the device. 

### Table 42:  IOBinding object structure for State Sensors

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| bindingType | A string that identifies the binding type as a state Sensor binding. | No | “stateSensor” |
| name | A unique name that identifies this IOBinding from all other IOBinding objects within the LogicalEntity. For required I/O bindings, the name should match those found in sections of this document. | No | String |
| includeInPdr | If value of this field is true, a corresponding PDR record should be generated in the firmware build process. If the value of this field is false, a PDR record will not be generated – this is useful in cases where binding of a function is required, but no direct interaction with the Sensor is allowed at runtime. | No | Boolean |
| required | true if this I/O binding is required. False of the binding Sensor is optional for the specific LogicalEntity. | No | Boolean |
| description | A text description of the function of the I/O binding that may be useful to the Sensor/Effecter integrator. | Yes | String |
| isVirtual | The value of this field should be set to true if the I/O binding is virtual. Virtual Sensors will not be bound to physical I/O and will not be checked against I/O constraints when configured. | No | Boolean |
| sensorID | The sensor ID for the sensor bound to this channel. This field may only be null if the sensor is not included in the PDR. | Yes | Integer |
| allowedInterfaceTypes | An array of allowed electrical interface types that this binding can be configured to work with. See section 5.2.1.1 for possible values. For virtual Sensors this array should be blank. | No | Array of String |
| boundChannel | The actual channel the Sensor is configured to use. The value of this field should be null for LogicalEntity objects in the ControllerCapabilities. When the device is configured, the value of this field will be changed to match a channel from the channels. IOBindings for virtual Sensors should always leave this value null. | Yes | String |
| stateSetVendorIANA | The vendor id associated with the state set for this Sensor. If this field is nonnull in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| stateSet | The state set id for the state set used by this Sensor. If this field is non-null in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| usedStates | A bitfield that defines which states in the state set are used. Bit 0, corresponds to the first state in the set, bit-1, the second state and so forth. A bit value of 1 means the state is used, otherwise the state is not used. | No |  |
| Bitfield | stateWhenHigh The state that will be returned when the binary state of the Sensor’s input pin (or virtual pin) is High. The value of this field should correspond to one of the states in the configured state set. If this field is non-null in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| stateWhenLow | The state that will be returned when the binary state of the Sensor’s input pin (or virtual pin) is Low. The value of this field should correspond to one of the states in the configured state set. If this field is non-null in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| possibleStates | A 32-bit bitfield where each bit specifies whether or not a specific state in the state set is used. A value of 1 in the bit specifies that the state is used, a value of 0 specifies that it is not. As an example, a value of 3 for this field means that states 0 and 1 are both used, all others are not. | No | Integer |

**REQ 5.200**:  IOBinding objects for state Sensors **shall** consist of the structure found in Table 42.

**REQ 5.210**: The value of the bindingType field for IOBinding objects for state Sensors **shall** be “stateSensor”.

**REQ 5.220**: The stateWhenHigh field for IOBinding objects for virtual state sensors **shall** be ignored by the configuration process. NOTE: This field **should** be left null for virtual state sensors

**REQ 5.230**: The stateWhenLow field for IOBinding objects for virtual state sensors **shall** be ignored by the configuration process. NOTE: This field **should** be left null for virtual state sensors 

### Table 43:  IOBinding object structure for State Effecters

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| bindingType | A string that identifies the binding type as a state Effecter binding. | No | “stateEffecter” |
| name | A unique name that identifies this IOBinding from all other IOBinding objects within the LogicalEntity. For required I/O bindings, the name should match those found in sections 4.3.5.6 and 4.3.6.8 of this document. | No | String |
| includeInPdr | If value of this field is true, a corresponding No PDR record should be generated in the firmware build process. If the value of this field is false, a PDR record will not be generated – this is useful in cases where binding of a function is required, but no direct interaction with the Effecter is allowed at runtime. | No | Boolean |
| required | true if this I/O binding is required. False of the binding is optional for the specific LogicalEntity. | No | Boolean |
| description | A text description of the function of the I/O binding that may be useful to the Sensor/Effecter integrator. | Yes | String |
| isVirtual | The value of this field should be set to true if the I/O binding is virtual. Virtual Effecters will not be bound to physical I/O and will not be checked against I/O constraints when configured. | No | Boolean |
| effecterID | The effecter ID for the effecter bound to this channel. This field may only be null if the effecter is not included in the PDR. | Yes | Integer allowedInterfaceTypes An array of allowed physical interface types that this channel can be configured to work with. See section 5.2.1.1 for possible values. For virtual Effecters this array should be blank. |
| No | Array of String boundChannel The actual channel the Effecter is configured Yes to use. The value of this field should be null for LogicalEntities in the ControllerCapabilities. When the device is configured, the value of this field will be changed to match a channel from the channels array. IOBindings for virtual Effecters should always leave this value null. | Yes | String |
| stateSetVendorIANA | The vendor id associated with the state set for this Effecter. If this field is non-null in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| stateSet | The state set id for the state set used by this Effecter. If this field is non-null in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| usedStates | A bitfield that defines which states in the state set are used. Bit 0, corresponds to the first state in the set, bit-1, the second state and so forth. A bit value of 1 means the state is used, otherwise the state is not used. | No |  |
| Bitfield | stateWhenHigh The state that, when set, will result in a binary High state of the Effecter’s output pin (or virtual pin). The value of this field should correspond to one of the states in the configured state set. If this field is non-null in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| stateWhenLow | The state that, when set, will result in a binary Low state of the Effecter’s output pin (or virtual pin). The value of this field should correspond to one of the states in the configured state set. If this field is non-null in the ControllerCapabilities object, it should not be overridden during configuration. | Yes | Integer |
| possibleStates | A 32-bit bitfield where each bit specifies whether or not a specific state in the state set is used. A value of 1 in the bit specifies that the state is used, a value of 0 specifies that it is not. As an example, a value of 3 for this field means that states 0 and 1 are both used, all others are not. | No | Integer |
| defaultState | The default state for the effecter when it is enabled. | Yes | Integer |

**REQ 5.240**:  IOBinding objects for state Effecters **shall** consist of the structure found in Table 43. 

**REQ 5.250**: The value of the bindingType field for IOBinding objects for state Effecters **shall** be “stateEffecter”.

**REQ 5.260**: The stateWhenHigh field for IOBinding objects for virtual state effecters **shall** be ignored by the configuration process. NOTE: This field **should** be left null for virtual state effecters

**REQ 5.270**: The stateWhenLow field for IOBinding objects for virtual state effecters **shall** be ignored by the configuration process. NOTE: This field **should** be left null for virtual state effecters 5.2.4.3 Numeric Sensor IOBinding

### Table 44:  IOBinding object structure for Numeric Sensors

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| bindingType | A string that identifies the binding type as a numeric Sensor binding. | No | “numericSensor” |
| name | A unique name that identifies this IOBinding from all other IOBinding objects within the LogicalEntity. For required I/O bindings, the name should match those found in section sections 4.3.5.6 and 4.3.6.8 of this document. | No | String |
| includeInPdr | If value of this field is true, a corresponding PDR record should be generated in the firmware build process. If the value of this field is false, a PDR record will not be generated – this is useful in cases where binding of a function is required, but no direct interaction with the Sensor is allowed at runtime. | No | Boolean |
| required | True if this I/O binding is required. False if the binding Sensor is optional for the specific LogicalEntity. | No | Boolean |
| description | A text description of the function of the I/O binding that may be useful to the Sensor/Effecter integrator. | Yes | String |
| isVirtual | The value of this field should be set to true if the I/O binding is virtual. Virtual Sensors will not be bound to physical I/O and will not be checked against I/O constraints when configured. | No | Boolean |
| sensorID | The sensor ID for the effecter bound to this channel. This field may only be null if the effecter is not included in the PDR. | Yes | Integer |
| allowedInterfaceTypes | An array of allowed electrical interface types that this binding can be configured to work with. See section 5.2.1.1for possible values. For virtual Sensors this array should be blank. | No | Array of String |
| sensor | A SensorDefinition object that defines the Sensor characteristics for the Sensor that is bound to this I/O function. It is intended that this field be null in the ControllerCapabilities structure. In the process of configuration it will be populated with a SensorDefintion. Virtual Sensors should leave this field null. | Yes | SensorDefinition object (null expected) |
| inputCurve | An array of DataPoint objects that specify the input curve of the sampler input (see section 3.3.1). DataPoint objects are defined in section 5.2.4.5. <br>The input value for each point on the curve represent the input to the controller’s input circuitry. The output values represent the quantity present at the controller’s input pin. Physical meaning for the values at the input pin vary by interface type. They are: <br>- analog interface types: A voltage in Volts <br>- rate interface types: A frequency in Hertz <br>- count/quadrature interface types: a count of pulses | Yes | Array of at least two DataPoint objects |
| boundChannel | The actual channel the Sensor is configured to use. The value of this field should be null for LogicalEntities in the ControllerCapabilities. When the device is configured, the value of this field will be changed to match a channel from the channels array. IOBindings for virtual Sensors should always leave this value null. | Yes | String |
| inputGearingRatio | The gearing ratio expressed in terms of physical units to measured units expressed as a real number. The measured quantity will be the physical quantity divided by this amount. | Yes | Real |
| physicalBaseUnit | The base units for the physical quantity measured by this I/O binding (or the virtual base units for virtual Sensors). The values for this field are defined identically to the baseUnit field in the PLDM specification. | Yes | Integer |
| physicalUnitModifier | The unit modifier for the physical quantity measured by this I/O binding (or the virtual unit modifier for virtual Sensors). The value for this field is defined identically to the baseUnitModifier field in the PLDM specification. | Yes | Integer |
| physicalRateUnit | The base unit modifier for the physical quantity measured by this I/O binding (or the virtual base unit modifier for virtual Sensors). The value for this field is defined identically to the baseUnitModifier field in the PLDM specification. | Yes | Integer |
| physicalAuxUnit | The aux unit for the physical quantity measured by this I/O binding (or the virtual aux unit for virtual Sensors). The value for this field is defined identically to the auxUnit field in the PLDM specification. | Yes | Integer |
| rel | The relationship between the base unit and the auxiliary unit, as follows: <br>value = { “dividedBy”, “multipliedBy”} <br>“dividedBy” implies a "/" or "per" relationship, such as "per foot" <br>“multipliedBy” implies a "*" operation, such as "foot*lbs (foot-lbs)" <br>This field definition is derived from an identically named field in PLDM Numeric Sensor / Effecter PDRs as defined by DSP0248. | Yes | String |
| physicalAuxUnitModifier | The aux unit modifier for the physical quantity measured by this I/O binding (or the virtual aux unit modifier for virtual Sensors). The value for this field is defined identically to the auxUnitModifier field in the PLDM specification. | Yes | Integer |
| physicalAuxRateUnit | The aux rate unit for the physical quantity measured by this I/O binding (or the virtual base unit modifier for virtual Sensors). The value for this field is defined identically to the auxRateUnit field in the PLDM specification. | Yes | Integer |
| normalMax | The normalMax threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |
| normalMin | The normalMin threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |
| upperThresholdWarning | The upper warning threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |
| upperThresholdCritical | The upper critical threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |
| upperThresholdFatal | The upper fatal threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |
| lowerThresholdWarning | The lower warning threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |
| lowerThresholdCritical | The lower critical threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |
| lowerThresholdFatal | The lower fatal threshold value as defined in PLDM specification. If not used or not defined, this value may be null. When specified, this should be expressed in terms of the physical units. | Yes | Real |

**REQ 5.280**:  IOBinding objects for numeric Sensors **shall** consist of the structure found in Table 44. 

**REQ 5.290**: The value of the bindingType field for IOBinding objects for numeric Sensors **shall** be “numericSensor”.

**REQ 5.300**: The value of the sensor field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Sensors.

**REQ 5.310**: The value of the sensor field **should** be null for IOBindings in the ControllerCapabilites object. NOTE: This field will be populated during configuration.

**REQ 5.320**: The value of the outputCurve field in the ControllerCapabilites object **shall** be null for virtual Sensors.

**REQ 5.330**: The value of the boundChannel field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Sensors.

**REQ 5.340**: The value of the inputGearingRatio field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Sensors.

**REQ 5.350**: The value of the physicalBaseUnit field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Sensors.

**REQ 5.360**: The value of the physicalUnitModifier field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Sensors.

**REQ 5.370**: The value of the physicalRateUnit field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Sensors.

**REQ 5.380**: The value of the rel field in IOBinding object within the ControllerCapabilities object **shall** be null for virtual sensors.

**REQ 5.390**: The value of the physicalAuxUnit field in IOBindings in the ControllerCapabilities object **shall** be null for virtual Sensors.

**REQ 5.400**: The value of the physicalAuxUnitModifier field in IOBindings in the ControllerCapabilities object **shall** be null for virtual Sensors.

**REQ 5.410**: The value of the physicalAuxRateUnit field in IOBindings in the ControllerCapabilities object **shall** be null for virtual Sensors. 

### Table 45:  IOBinding object structure for Numeric Effecters

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| bindingType | A string that identifies the binding type as a numeric Effecter binding. | No | “numericEffecter” |
| name | A unique name that identifies this IOBinding from all other IOBinding objects within the LogicalEntity. For required I/O bindings, the name should match those found in section sections 4.3.5.6 and 4.3.6.8 of this document. | No | String |
| includeInPdr | If value of this field is true, a corresponding PDR record should be generated in the firmware build process. If the value of this field is false, a PDR record will not be generated – this is useful in cases where binding of a function is required, but no direct interaction with the Effecter is allowed at runtime. | No | Boolean |
| required | true if this I/O binding is required. False if the binding Effecter is optional for the specific LogicalEntity. | No | Boolean |
| description | A text description of the function of the I/O binding that may be useful to the Sensor/Effecter integrator. | Yes | String |
| isVirtual | The value of this field should be set to true if the I/O binding is virtual. Virtual Effecters will not be bound to physical I/O and will not be checked against I/O constraints when configured. | No | Boolean |
| effecterID | The effecter ID for the effecter bound to this channel. This field may only be null if the effecter is not included in the PDR. | Yes | Integer |
| allowedInterfaceTypes | An array of allowed electrical interface types that this channel can be configured to work with. See section 5.2.1.1 for possible values. For virtual Effecters this array should be blank. | No | Array of String |
| effecter | An effecterDefinition object that defines the Effecter characteristics for the Effecter that is bound to this I/O function. It is intended that this field be null in the ControllerCapabilities structure. In the process of configuration it will be populated with an effecterDefinition. Virtual Effecters should leave this field null. | Yes | effecterDefinition object (null expected) |
| outputCurve | An array of DataPoint objects that specify the output curve of the sampler output (see section 3.3.1). DataPoint objects are defined in section 5.2.4.5. <br>The input value for each point on the curve represent the output from the controller’s pin. The output values of this curve represent the quantity present at the exit of any external output circuitry. Physical meaning for the values at the output pin vary by interface type. They are: <br>- analog interface types: A voltage in Volts <br>- pwm interface types: a duty cycle between 0 and 100 <br>- rate interface types: A frequency in Hertz <br>- step_dir_enable – the number of steps to move. | Yes | Array of at least two DataPoint objects |
| boundChannel | The actual channel the Effecter is configured to use. The value of this field should be null for LogicalEntity object in the ControllerCapabilities. When the device is configured, the value of this field will be changed to match a channel from the channels array. IOBindings for virtual Effecters should always leave this value null. | Yes | String |
| outputGearingRatio | The gearing ratio expressed in terms of physical units to device output units expressed as a real number. The output quantity will be the physical quantity divided by this amount. | Yes | Real |
| physicalBaseUnit | The base units for the physical quantity controlled by this I/O binding (or the virtual base units for virtual Effecters). The values for this field are defined identically to the baseUnit field in the PLDM specification. | Yes | Integer |
| physicalUnitModifier | The unit modifier for the physical quantity controlled by this I/O binding (or the virtual unit modifier for virtual Effecters). The value for this field is defined identically to the baseUnitModifier field in the PLDM specification. | Yes | Integer |
| physicalRateUnit | The base unit modifier for the physical quantity controlled by this I/O binding (or the virtual base unit modifier for virtual Effecters). The value for this field is defined identically to the baseUnitModifier field in the PLDM specification. | Yes | Integer |
| physicalAuxUnit | The aux unit for the physical quantity controlled by this I/O binding (or the virtual aux unit for virtual Effecters). The value for this field is defined identically to the auxUnit field in the PLDM specification. | Yes | Integer |
| rel | The relationship between the base unit and the auxiliary unit, as follows: <br>value = { “dividedBy”, “multipliedBy”} <br>“dividedBy” implies a "/" or "per" relationship, such as "per foot" <br>“multipliedBy” implies a "*" operation, such as "foot*lbs (footlbs)" <br>This field definition is derived from an identically named field in PLDM Numeric Sensor / Effecter PDRs as defined by DSP0248. | No | String |
| physicalAuxUnitModifier | The aux unit modifier for the physical quantity controlled by this I/O binding (or the virtual aux unit modifier for virtual Effecters). The value for this field is defined identically to the auxUnitModifier field in the PLDM specification. | Yes | Integer |
| physicalAuxRateUnit | The aux rate unit for the physical quantity controlled by this I/O binding (or the virtual base unit modifier for virtual Sensors). The value for this field is defined identically to the auxRateUnit field in the PLDM specification. | Yes | Integer |
| physicalDefaultValue | The default value for the physical quantity controlled by this I/O binding when it is enabled. This value is specified in real-world units. | Yes | Real |

**REQ 5.420**:  45. IOBinding objects for numeric Effecters **shall** consist of the structure found in Table

**REQ 5.430**: The value of the bindingType field for IOBinding objects for numeric Effecters **shall** be “numericEffecter”.

**REQ 5.440**: The value of the effecter field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Effecters.

**REQ 5.450**: The value of the effecter field **should** be null for IOBindings in the ControllerCapabilites object. NOTE: This field will be populated during configuration.

**REQ 5.460**: The value of the outputCurve field in the ControllerCapabilites object **shall** be null for virtual Effecters.

**REQ 5.470**: The value of the boundChannel field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Effecters.

**REQ 5.480**: The value of the inputGearingRatio field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Effecters.

**REQ 5.490**: The value of the physicalBaseUnit field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Effecters.

**REQ 5.500**: The value of the physicalUnitModifier field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Effecters. 

**REQ 5.510**: The value of the physicalRateUnit field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Effecters.

**REQ 5.520**: The value of the rel field in IOBinding objects within the ControllerCapabilities object **shall** be null for virtual effecters.

**REQ 5.530**: The value of the physicalAuxUnit field in IOBindings in the ControllerCapabilites ty object **shall** be null for virtual Effecters.

**REQ 5.540**: The value of the physicalAuxUnitModifier field in IOBindings in the ControllerCapabilites object **shall** be null for virtual Effecters.

**REQ 5.550**: The value of the physicalAuxRateUnit field in IOBindings in the ControllerCapabilites object **shall** null for virtual Effecters. 

### Table 46:  DataPoint object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| in | The input value | No | Real |
| out | The resulting output value | No | Real |

**REQ 5.560**:  DataPoint objects **shall** consist of the structure found in table Table 46. 5.2.4.6 Common IOBinding requirements The following requirements apply to all IOBinding types:

**REQ 5.570**: The value for the name field in the IOBinding object **shall** be unique across all IOBinding objects within the LogicalEntity.

**REQ 5.580**: If the value of the includeInPdr field in the IOBinding object is true, the configuration tools **shall** create a PDR record associated with the IOBinding.

**REQ 5.590**: If the value of the includeInPdr field in the IOBinding object is false, the configuration tools **shall** not create a PDR record associated with the IOBinding.

**REQ 5.600**: If the value of the required field within the IOBinding object is true, the IOBinding **shall** be required to be fully configured in any resulting configuration of the LogicalEntity with which it is associated.

**REQ 5.610**: If the value of the isVirtual field within the IOBinding object is true, the boundChannel field of the IOBinding **shall** be null.

**REQ 5.620**: If the value of the isVirtual field within the IOBinding object is true, the boundChannel field of the IOBinding **shall** remain null after configuration. 

**REQ 5.630**:  sets. The stateSetVendorIANA field **shall** be set to 412 (DMTF) for PLDM standard state

**REQ 5.640**:  state sets. The stateSetVendorIANA field **shall** be set to 12634 (PICMG) for all PICMG OEM

**REQ 5.650**: For interoperability, the only stateSetVendorIANA values that **shall** be used in IOBindings in LogicalEntity objects in the ControllerCapabilities object **shall** correspond to DMTF, PICMG and null. NOTE: Other OEM state sets **may** be specified in the configuration process, this process is beyond the scope of this specification. One possibility is that the configuration tool has an extensible library of OEM state sets that can be drawn from. It is the responsibility of the build process to make sure all required OEM entity definitions are included in the device’s PDR repository.

**REQ 5.660**: IOBinding fields that are non-null in LogicalEntity objects within the ControllerCapabilities object **shall** not be modified in the resulting device configuration. 

### Table 47:  Parameter object structure for Numeric parameters

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| parameterType | A string that identifies the parameter type as a numeric parameter. The value of this string should be “integer” for signed or unsigned integers or “real” for real-valued parameters. | No | String (“integer” or “real”) |
| name | A unique name that identifies this Parameter from all other Parameter objects within the LogicalEntity. | No | String |
| description | A text description of the parameter that may be useful to the sensor/effecter integrator. | Yes | String |
| value | The value of the parameter. If null, the parameter must be set during the configuration process. If non-null, the value cannot be altered by the configuration process. by the parameter type | Yes | Numeric of the same type specified |
| defaultValue | Provided as a hint to the configuration process. The default value of the parameter if no other value is specified. If null, the configuration process will not assume a default value. | Yes | Numeric of the same type specified by the parameter type |
| maxValue | The maximum value allowed for this parameter. If null, the configuration process should not check for a maximum value. | Yes | Numeric of the same type specified by the parameter type |
| minValue | The minimum value allowed for this parameter. If null, the configuration process should not check for a minimum value. | Yes | Numeric of the same type specified by the parameter type |

**REQ 5.670**:  47. Parameter objects for numeric parameters **shall** consist of the structure found in Table

**REQ 5.680**: If the value of the maxValue field in a numeric Parameter object is non-null the configuration process **shall** not allow setting numbers greater than the value of maxValue.

**REQ 5.690**: If the value of the minValue field in a numeric Parameter object is non-null the configuration process **should** not allow setting numbers smaller than the value of minValue. 5.2.5.2 Enumerated Parameter Object Enumerated parameters are used to specify a choice from a list of values. Their format is as follows:

### Table 48:  Parameter object structure for enumerated parameters

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| parameterType | A string that identifies the Parameter type as an enumerated parameter. | No | “enum” |
| name | A unique name that identifies this Parameter from all other Parameter objects within the LogicalEntity. | No | String |
| description | A text description of the parameter that may be useful to the sensor/effecter integrator. | Yes | String |
| value | The value of the parameter. If null, the parameter must be set during the configuration process. If non-null, the value cannot be altered by the configuration process. | Yes | String |
| defaultValue | Provided as a hint to the configuration process. The default value of the parameter if no other value is specified. If null, the configuration process will not assume a default value. | Yes | String |
| choices | An array of strings where each element specifies a possible configuration choice. | Yes | Array of String |

**REQ 5.700**:  Table 48. Parameter objects for enumerated parameters **shall** consist of the structure found in

**REQ 5.710**: If the value of the numeric Parameter object is non-null within the ControllerCapabilities, it **shall** not be altered by the configuration process.

**REQ 5.720**: For an enumerated Parameter, the configuration process **shall** only allow selection of parameter values from the array of choices within the Parameter object.

**REQ 5.730**: If non-null, the value of an enumerated Parameter defaultValue field **shall** belong to the array of choices. 

### Table 49:  SensorDefinition JSON object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| name | A human-readable name that identifies the sensor model. This should also be the file name (minus extension) of the sensor JSON file. | No | String |
| manufacturer | The manufacturer of the sensor. | No | String |
| partNumber | The manufacturer’s part number for the sensor. | No | String |
| description | A text description of the sensor that may be helpful in understanding its capabilities. | No | String |
| supportedInterfaces | An array of electrical interface types that this effecter can work with. The interface must match one of those found in section 5.2.1.1 | No | Array of String (cannot be empty) |
| maxSampleRate | The maximum allowed sample rate of this sensor, expressed in Hz. If null, there is no maximum sample rate. | Yes | Number |
| baseUnit | A numeric descriptor for the base units of the sensor. The values for this field are defined in DSP0248. This field applies to the physical quantity that is measured by the sensor (as opposed to the output of the sensor). | No | Integer |
| unitModifier | A numeric descriptor for the base unit modifier for the sensor. The values for this field are defined in DSP0248. This field applies to the physical quantity that is measured by the sensor (as opposed to the output of the sensor). | No | Integer |
| rateUnit | A numeric descriptor for the rate units of the sensor. The values for this field are defined in DSP0248. This field applies to the physical quantity that is measured by the sensor (as opposed to the output of the sensor). | No | Integer |
| auxUnit | A numeric descriptor for the aux units of the sensor. The values for this field are defined in DSP0248. This field applies to the physical quantity that is measured by the sensor (as opposed to the output of the sensor). | No | Integer |
| auxUnitModifier | A numeric descriptor for the aux unit modifier for the sensor. The values for this field are defined in DSP0248. This field applies to the physical quantity that is measured by the sensor (as opposed to the output of the sensor). | No | Integer |
| rel | The relationship between the base unit and the auxiliary unit, as follows: <br>value = { “dividedBy”, “multipliedBy”} <br>“dividedBy” implies a "/" or "per" relationship, such as "per foot" <br>“multipliedBy” implies a "*" operation, such as "foot*lbs (foot-lbs)" <br>This field definition is derived from an identically named field in PLDM Numeric Sensor / Effecter PDRs as defined by DSP0248. | No | String |
| auxRateUnit | A numeric descriptor for the aux rate unit modifier for the sensor. The values for this field are defined in DSP0248. This field applies to the physical quantity that is measured by the sensor (as opposed to the output of the sensor). | No | Integer |
| plusAccuracy | The maximum amount the sensor may overreport the actual reading. This value should include all sources of inaccuracy including any loss of accuracy from the response curve inaccuracy. | No | Real |
| minusAccuracy | The maximum amount the sensor may underreport the actual reading. This value should include all sources of inaccuracy including any loss of accuracy from the response curve inaccuracy. | No | Real |
| outputUnits | The output (electrical) units of the sensor. This may be useful to the sensor/effecter integrator when defining the input circuitry and input curve for the controller. | No | String |
| responseCurve | An array of DataPoint objects that specify the response curve of the sensor. DataPoint objects are described in section 5.2.4.5 | No | Array of at least two DataPoint objects |

**REQ 5.740**:  SensorDefinition objects **shall** consist of the structure found in Table 49.

**REQ 5.750**:  Sensor JSON files **shall** contain one SensorDefinition object. 

### Table 50:  EffecterDefinition JSON object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| name | A human-readable name that identifies the effecter model. This should also be the file name (minus extension) of the effecter JSON file. | No | String |
| manufacturer | The manufacturer of the effecter. | No | String |
| partNumber | The manufacturer’s part number for the effecter. | No | String |
| description | A text description of the sensor that may be helpful in understanding its capabilities. | No | String |
| supportedInterfaces | An array of electrical interface types that No this sensor can work with. The interface must match one of those found in section 5.2.1.1 Array of String (cannot be empty) maxSampleRate The maximum allowed output sample rate for this effecter, expressed in Hz. If null, there is no maximum sample rate. | Yes | Number |
| baseUnit | A numeric descriptor for the base units of the effecter. The values for this field are defined in DSP0248. This field applies to the physical quantity that is output by the effecter (as opposed to the control input of the effecter). | No | Integer |
| unitModifier | A numeric descriptor for the base unit modifier for the effecter. The values for this field are defined in DSP0248. This field applies to the physical quantity that is output by the effecter (as opposed to the control input of the effecter). | No | Integer |
| rateUnit | A numeric descriptor for the rate units of the effecter. The values for this field are defined in DSP0248. This field applies to the physical quantity that is controlled by the effecter (as opposed to the control input of the effecter). | No | Integer |
| auxUnit | A numeric descriptor for the aux units of the effecter. The values for this field are defined in DSP0248. This field applies to the physical quantity that is controlled by the effecter (as opposed to the control input of the effecter). | No | Integer |
| auxUnitModifier | A numeric descriptor for the aux unit modifier for the effecter. The values for this field are defined in DSP0248. This field applies to the physical quantity that is controlled by the effecter (as opposed to the control input of the effecter). | No | Integer |
| rel | The relationship between the base unit and the auxiliary unit, as follows: <br>value = { “dividedBy”, “multipliedBy”} <br>“dividedBy” implies a "/" or "per" relationship, such as "per foot" <br>“multipliedBy” implies a "*" operation, such as "foot*lbs (foot-lbs)" <br>This field definition is derived from an identically named field in PLDM Numeric Sensor / Effecter PDRs as defined by DSP0248. | No | String |
| auxRateUnit | A numeric descriptor for the aux rate unit modifier for the effecter. The values for this field are defined in DSP0248. This field applies to the physical quantity that is controlled by the effecter (as opposed to the control input of the effecter). | No | Integer |
| plusAccuracy | The maximum amount the effecter output may overshoot the requested value (under normal conditions). This value should include all sources of inaccuracy including any loss of accuracy from the response curve inaccuracy. | No | Real |
| minusAccuracy | The maximum amount the sensor may undershoot the requested value (under normal conditions). This value should include all sources of inaccuracy including any loss of accuracy from the response curve inaccuracy. | No | Real |
| inputUnits | The input (electrical) units of the effecter. This may be useful to the sensor/effecter integrator when defining the output circuitry and output curve for the controller. | No | String |
| responseCurve | An array of DataPoint objects that specify the response curve of the effecter. DataPoint objects are described in section 5.2.4.5. | No | Array of at least two DataPoint objects |
| ratedMax | The maximum rated output value of the effecter. If null, there is no rated maximum value | Yes | Real |
| nominalValue | The nominal rated value of the effecter. If null, there is no nominal value. | Yes | Real |

**REQ 5.760**:  EffecterDefinition objects **shall** consist of the structure found in Table 50.

**REQ 5.770**:  Effecter JSON files **shall** contain one EffecterDefinition object. 

### Table 51:  ConfigFile object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| capabilities | The original (unmodified) ControllerCapabilities object from the controller device manufacturer | No | ControllerCapabilities object |
| configuration | The configuration for the device. | No | Configuration object |
| oemStateSets | An array of state set definitions for the oem state sets used by this configuration. | No | Array of stateSet objects |

**REQ 5.780**:  The config file **shall** consist of a single ConfigFile object.

**REQ 5.790**:  The ConfigFile object **shall** comply with the structure in Table 51.

**REQ 5.800**: The structure of the Configuration object within the ConfigFile **shall** be the same as that of the ControllerCapabilities object defined in Section 5.2.

**REQ 5.810**: The value of the capabilities field in the config file **shall** match the ControllerCapabilities object associated with the associated hardware device without modification. 

**REQ 5.820**:  The value of the pins field within the Configuration object in the config file **may** be empty. NOTE: The pins array does not change between capabilities and configuration. Vendor-specific build tools **should** use the pin list from the capabilities section of the config file (if required).

**REQ 5.830**:  The value of the channels field within the Configuration object in the config file **may** be empty. NOTE: The channels array does not change between capabilities and configuration. Vendorspecific build tools **should** use the channels list from the capabilities section of the config file (if required). 

**REQ 5.840**: All FruRecord objects within the capabilities ControllerCapabilities object **shall** be present in the Configuration object in the config file.

**REQ 5.850**: All FruRecord fields that are non-null in within the ControllerCapabilities object **shall** not be modified in the resulting Configuration object in the config file.

**REQ 5.860**: All FruRecord objects added to the Configuration object of the config file **shall** have their required field set to false. NOTE: This allows the configuration tool to differentiate between the device-required FRU records and those added through the configuration process.

**REQ 5.870**: The configuration process **shall** not add new fields to FruRecord objects in the Configuration object if the required field of the FruRecord is set to true NOTE: The intention is that required FruRecord objects can be completed, but not augmented. If new fields are required, they **may** be added to a new FruRecord object.

**REQ 5.880**: All FruField objects within the Configuration object in the config file **shall** have a non-null value for the value field.

**REQ 5.890**: The Configuration object in the config file **shall** have at least one LogicalEntity object within its logicalEntities array.

**REQ 5.900**: Only LogicalEntity objects found within the ControllerCapabilities object **shall** be present in the Configuration object in the config file. NOTE: The configuration process cannot add new LogicalEntity objects that the device **may** not support.

**REQ 5.910**: With the exception of IOBinding objects and Parameter objects, the fields within each LogicalEntity object within the Configuration object **shall** not be altered from their values found in the corresponding LogicalEntity found in the ControllerCapabilites object. 

**REQ 5.920**: Only LogicalEntity objects found within the ControllerCapabilities object **shall** be present in the Configuration object in the config file.

**REQ 5.930**: No IOBinding objects **shall** be added to LogicalEntity objects in the Configuration object beyond those found within the related LogicalEntity objects in the ControllerCapabilities object.

**REQ 5.940**: No IOBinding objects **shall** be removed from any LogicalEntity object in the Configuration object as compared to the related LogicalEntity object in the ControllerCapabilities object.

**REQ 5.950**: Non-null IOBinding values within the ControllerCapabilites object **shall** not be altered within the related IOBinding within the Configuration object.

**REQ 5.960**:  The boundChannel value for any IOBinding within the Configuration object **shall** be non-null. 

**REQ 5.970**: The boundChannel value for any IOBinding within the Configuration object **shall** have a valid channel name as found in the channels array of the ControllerCapabilities object.

**REQ 5.980**: The stateSetVendorIANA value for any state sensor or state effecter IOBinding within the Configuration object **shall** be non-null if the includeInPDR field is true.

**REQ 5.990**: The stateSet value for any state sensor or state effecter IOBinding within the Configuration object **shall** be non-null if the includeInPDR field is true.

**REQ 5.1000**: The stateWhenHigh value for any non-virtual state sensor or state effecter IOBinding within the Configuration object **shall** be non-null if the includeInPDR field is true.

**REQ 5.1010**: The stateWhenHigh value for any virtual state sensor or state effecter IOBinding within the Configuration object **shall** be null.

**REQ 5.1020**: The stateWhenHigh value for any state sensor or state effecter IOBinding within the Configuration object **shall** be null if the includeInPdr field is false.

**REQ 5.1030**: The stateWhenLow value for any non-virtual state sensor or state effecter IOBinding within the Configuration object **shall** be non-null if the includeInPdr field is true.

**REQ 5.1040**: The stateWhenLow value for any virtual state sensor or state effecter IOBinding within the Configuration object **shall** be null.

**REQ 5.1050**: The stateWhenLow value for any state sensor or state effecter IOBinding within the Configuration object **shall** be null if the includeInPdr field is false.

**REQ 5.1060**: The sensor value for any non-virtual numeric sensor IOBinding within the Configuration object **shall** be comprised of a valid SensorDefinition object.

**REQ 5.1070**: The sensor value for any virtual numeric sensor IOBinding within the Configuration object **shall** be null.

**REQ 5.1080**: The effecter value for any non-virtual numeric effecter IOBinding within the Configuration object **shall** be comprised of a valid EffecterDefinition object.

**REQ 5.1090**: The effecter value for any virtual numeric effecter IOBinding within the Configuration object **shall** be null. 

**REQ 5.1100**: The inputGearingRatio value for any non-virtual numeric sensor IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1110**: The inputGearingRatio value for any virtual numeric sensor IOBinding within the Configuration object **shall** be null.

**REQ 5.1120**: The outputGearingRatio value for any non-virtual numeric effecter IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1130**: The outputGearingRatio value for any virtual numeric effecter IOBinding within the Configuration object **shall** be null.

**REQ 5.1140**: The physicalBaseUnit value for any non-virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1150**: The physicalBaseUnit value for any virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be null.

**REQ 5.1160**: The physicalUnitModifier value for any non-virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1170**: The physicalUnitModifier value for any virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be null.

**REQ 5.1180**: The physicalRateUnit value for any non-virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1190**: The physicalRateUnit value for any virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be null.

**REQ 5.1200**: The value of the rel field in IOBindings within the Configuration object **shall** be null for virtual sensors.

**REQ 5.1210**: The value of the rel field in IOBindings within the Configuration object **shall** be null for virtual effecters.

**REQ 5.1220**: The physicalAuxUnit value for any non-virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1230**: The physicalAuxUnit value for any virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be null.

**REQ 5.1240**: The rel value for any non-virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be either “dividedBy” or “multipledBy”.

**REQ 5.1250**: The physicalAuxUnitModifier value for any non-virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1260**: The physicalAuxRateUnit value for any non-virtual numeric sensor/effecter IOBinding within the Configuration object **shall** be non-null.

**REQ 5.1262**: The defaultState value for any non-virtual state effecter IOBinding within the configuration object **shall** be non-null. NOTE: This requirement allows the firmware to determine which state the effecter **should** enter when enabled. Behavior of virtual sensors is known to the firmware already so the parameter is not required. 

**REQ 5.1264**: The normalMin value for any numeric sensor IOBinding that is included in the PDR **shall** be non-null.

**REQ 5.1266**: The normalMax value for any numeric sensor IOBinding that is included in the PDR **shall** be non-null.

**REQ 5.1268**: The values for any non-null threshold specified in a numeric sensor IOBinding **shall** be strictly decreasing from upperFatal to lowerFatal as described in the PLDM specification. 

**REQ 5.1270**: No Parameter objects **shall** be added to the Configuration object beyond those found within the parameters array in the ControllerCapabilities object.

**REQ 5.1280**: No Parameter objects **shall** be removed from the Configuration object as comparted to those found within the parameters array in the ControllerCapabilities object.

**REQ 5.1290**: The only field within Parameter objects within that **may** differ between the Configuration and the ControllerCapabilities objects is the value field.

**REQ 5.1300**: If the value of the value field of a Parameter object is non-null within the ControllerCapabilities object, the value within the associated Parameter object in the Configuration object **shall** match exactly.

**REQ 5.1310**: Parameter objects within the Configuration object **shall** have their value field set to a non-null value.

**REQ 5.1320**: Value fields within numeric Parameter objects within the Configuration object **shall** have their values set less than or equal to the value of their maxValue field if that field has a non-null value.

**REQ 5.1330**: Value fields within numeric Parameter objects within the Configuration object **shall** have their values set greater than or equal to the value of their minValue field if that field has a nonnull value.

**REQ 5.1340**: Value fields within enumerated Parameter objects within the Configuration object **shall** have their value set to one of the enumerated choices defined in the Parameter. 

### Table 52:  OemStateSet object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| name | The name of the state set. | No | String |
| vendorName | The name of the vendor that is responsible for this state set | No | String |
| vendorIANA | The IANA enterprise number for the vendor that defined the OEM state set. | No | Integer |
| stateSetID | The OEM state set ID that uniquely identifies this state set from all other state sets from the same vendor. | No | Integer |
| oemStateValueRecords | An array of one or more oemStateValueRecord objects that define each state in the set. | No | Array of oemStateSetValueRecord objects |

**REQ 5.1350**: OemStateSet objects **shall** consist of the structure found in Table 52.

**REQ 5.1360**: An OemStateSet object **shall** exist for each unique stateSetVendorIANA/stateSet value pair found in the IOBindings of the Configuration object where stateSetVendorIANA does not have a value of 412 (DMTF).

**REQ 5.1370**: The vendorIANA/stateSetId value pair within the OemStateSet object **shall** match a cooresponding stateSetVendorIANA/stateSet value pair found in the related IOBinding objects in the Configuration object.

**REQ 5.1380**: There **shall** be no duplicate OemStateSet objects within the Configuration object’s oemStateSets array. 

### Table 53:  OemStateSetValueRecord object structure

| Keyword | Description | Nullable | Data Type |
|---|---|---|---|
| minStateValue | The lowest state enumeration value that corresponds to this state. | No | Unsigned Integer |
| maxStateValue | The highest state enumeration value that corresponds to this state. | No | Unsigned Integer |
| languageTags | An array of one or more strings that hold a language tag for the state corresponding state stateName array. Language tags are defined in RFC4646. | No | Array of String |
| stateName | An array of state names for this state. Each element in the array represents the state name in the language corresponding to the same element in the languageTags array. | No | Array of String |

**REQ 5.1390**: OemStateSetValueRecord objects **shall** consist of the structure found in Table 53. 

---

## Additional Specification Tables

*Tables referenced but not associated with specific requirements:*

### Table 7: Global Interlock State Sensor PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the State Sensor PDR, the value 04h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Sensor ID. For this specification, the value 0001h is used. |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 1 | Sensor Init. For this specification, the value 00h (noInit) is used. |
| 21 | 1 | Sensor Auxiliary Names PDR. |
| 22 | 1 | Composite Sensor Count. For this specification, the value 01h is used. Sensor Possible States |
| 23 | 2 | State Set ID. For this specification, the value 96 (Lock State) is used. |
| 25 | 1 | Possible State Size. For this specification, the value 01h is used. |
| 26 | 1 | Possible States. For this specification, the value 03h is used. |

### Table 8: Global Interlock State Effecter PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | Record Handle |  |
| 4 | Offset Length |  |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the State Effecter PDR, the value 0Bh is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | Effecter ID. For this specification, the value 0001h is used. |
| 14 | 2 | Entity Type. For this specification, the value 6000h (OEM System Integrator defined) is used. |
| 16 | 2 | Entity Instance Number. For this specification, the value 0001h is used. |
| 18 | 2 | Container ID. For this specification, the value 0001h is used. |
| 20 | 2 | Effecter Semantic ID. For this specification, the value 0000h (unspecified) is used. |
| 22 | 1 | Effecter Init. For this specification, the value 00h (noInit) is used. |
| 23 | 1 | Effecter Description PDR. |
| 24 | 1 | Composite Effecter Count. For this specification, the value 01h is used. Effecter Possible States |
| 25 | 2 | State Set ID. For this specification, the value 96 (Lock State) is used. |
| 27 | 1 | Possible State Size. For this specification, the value 01h is used. |
| 28 | 1 | Possible States. For this specification, the value 03h is used. |

### Table 11: Trigger OEM State Set PDR

| Offset | Length | Definition |
|---|---|---|
| 0 | 4 | Record Handle |
| 4 | 1 | PDR Header Version. For all records defined in this specification a value of 01h shall be used. |
| 5 | 1 | PDR Type. For the OEM State Set PDR, the value 08h is used. |
| 6 | 2 | recordChangeNumber. Consult [DMTF-0248] for more information. |
| 8 | 2 | dataLength – the total number of PDR bytes following this field. |
| 10 | 2 | PLDM Terminus Handle. For this specification, the value 0001h is used. |
| 12 | 2 | OEM State Set ID Handle. A unique value in the OEM state set range. |
| 14 | 4 | Vendor IANA. For this specification, the value 12634 is used. |
| 18 | 2 | OEM State Set ID. For this specification, the value 0001h is used. |
| 20 | 1 | Unspecified Value Hint. For this specification, the value 01h (treatAsError) is used. Offset Length |
| 21 | State Count. For this specification, the value 02h is used. |  |
| 2 | OEM State Value Record |  |
| 23 | 1 | Min State Value. For this specification, the value 01h is used. |
| 24 | 1 | Max State Value. For this specification, the value 01h is used. |
| 25 | 1 | String Count. For this specification, the value 01h is used. |
| 26 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 29 | 38 | State Name. A null-terminated Unicode string “Trigger Activated” in UTF-16BE format. OEM State Value Record |
| 67 | 1 | Min State Value. For this specification, the value 02h is used. |
| 68 | 1 | Max State Value. For this specification, the value 02h is used. |
| 69 | 1 | String Count. For this specification, the value 01h is used. |
| 70 | 3 | State Language Tag. A null-terminated ISO646 ASCII string. For this specification, the value “en” is used. |
| 73 | 42 | State Name. A null-terminated Unicode string “Trigger Deactivated” in UTF16BE format. |

