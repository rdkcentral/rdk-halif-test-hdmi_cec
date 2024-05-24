# HDMI CEC DRIVER L2 Low Level Test Specification and Procedure Documentation
 
## Table of Contents
            
- [HDMI CEC DRIVER L2 Low Level Test Specification and Procedure Documentation](#hdmi-cec-driver-l2-low-level-test-specification-and-procedure-documentation)

  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
    - [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
    - [Definitions](#definitions)
    - [References](#references)
  - [Level 2 Test Procedure](#level-2-test-procedure)
 
## Overview

This document describes the level 2 testing suite for the HDMI CEC DRIVER module.
 
### Acronyms, Terms and Abbreviations

- `HAL`  \- Hardware Abstraction Layer, may include some common components
- `UT`   \- Unit Test(s)
- `HDMI` \- High-Definition Multimedia Interface
- `CEC`  \- Consumer Electronics Contro  
- `DUT`  \- Device Under Test
- `API`  \- Application Program Interface
### Definitions

- `ut-core` \- Common Testing Framework <https://github.com/rdkcentral/ut-core>, which wraps a open-source framework that can be expanded to the requirements for future framework.
 
### References
- `High Level Test Specification` - [hdmi_cec_sink_tests.md](hdmi_cec_sink_tests.md)
 
## Level 2 Test Procedure

The following functions are intended to test the HDMI CEC HAL module's operation on sink devices according to the L2 Test specification.
 
### Test 1

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_GetDefaultLogicalAddress`|
|Description|Get the logical address of the `DUT` without actually adding the Logical Address and the API should return 0x0F as the default logical address.|
|Test Group|Module : 02|
|Test Case ID|001|
|Priority|High|

**Pre-Conditions :**
None

**Dependencies :** 
None

**User Interaction :** 
If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Call the pre-requisite API HdmiCecOpen() | handle = 0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Call the API HdmiCecGetLogicalAddress() | handle = valid handle, logicalAddress = 0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Check the logical address |  | logicalAddress = 0x0F| Should be successful |
| 04 | Call the post-requisite API HdmiCecClose() | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TD
    style A fill:#bff,stroke:#333,stroke-width:2px
    style B fill:#bbf,stroke:#333,stroke-width:2px
    style C fill:#bbf,stroke:#333,stroke-width:2px
    style D fill:#bff,stroke:#333,stroke-width:2px
	  style E fill:#f00,stroke:#333,stroke-width:2px

    A[Open HDMI CEC] --> B{Get Logical Address \n without adding one}
    B -->|HDMI_CEC_IO_SUCCESS| C[Check Logical Address == 0xf]
    C -->|Test successful| D[Close HDMI CEC]
    B -->|Not HDMI_CEC_IO_SUCCESS| E[Test Fail]
    C -->|NO| E
    E --> D
```


### Test 2

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_AddAndGetLogicalAddress`|
|Description|Setup all valid logical addresses b/w 0x00 to 0x0F for the `DUT` using HAL APIs, then retrieve it to ensure proper functionality.|
|Test Group|Module : 02|
|Test Case ID|002|
|Priority|High|

**Pre-Conditions :**
None

**Dependencies :** 
None

**User Interaction :** 
If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
01 | Open HDMI CEC Driver | Open the HDMI CEC driver using `HdmiCecOpen` API | `handle = valid pointer` | `HDMI_CEC_IO_SUCCESS` | Should be successful
02 | Manage Logical Addresses | For each logical address from 0x00 to 0x0F, add, validate, and remove the address sequentially | `handle = valid handle`, `logicalAddress = 0x00 to 0x0F` | `HDMI_CEC_IO_SUCCESS` | Each step should be successful
02a | Add Logical Address | Add the logical address using `HdmiCecAddLogicalAddress` API | `handle = valid handle`, `logicalAddress = current address` | `HDMI_CEC_IO_SUCCESS` | Should be successful
02b | Validate Added Logical Address | Get the previously added logical address using `HdmiCecGetLogicalAddress` API and validate it | `handle = valid handle` | `HDMI_CEC_IO_SUCCESS`, `logical address = current address` | Should match the added logical address
02c | Remove Logical Address | Remove the logical address using `HdmiCecRemoveLogicalAddress` API | `handle = valid handle`, `logicalAddress = current address` | `HDMI_CEC_IO_SUCCESS` | Should be successful
03 | Close HDMI CEC Driver | Close the HDMI CEC driver using `HdmiCecClose` API | `handle = valid handle` | `HDMI_CEC_IO_SUCCESS` | Should be successful


```mermaid
graph TD
    style A fill:#bff,stroke:#333,stroke-width:2px
    style B fill:#bbf,stroke:#333,stroke-width:2px
    style C fill:#f9f,stroke:#333,stroke-width:2px
    style D fill:#bbf,stroke:#333,stroke-width:2px
    style E fill:#bbf,stroke:#333,stroke-width:2px
    style F fill:#bbf,stroke:#333,stroke-width:2px
    style G fill:#f9f,stroke:#333,stroke-width:2px
    style H fill:#bff,stroke:#333,stroke-width:2px
    style I fill:#f00,stroke:#333,stroke-width:2px

    A[Open HDMI CEC Driver] --> B{HdmiCecOpen: \n handle = valid pointer}
    B -->|HDMI_CEC_IO_SUCCESS| C[Manage Logical Addresses]
    C -->|Each step successful| D[Add Logical Address]
    D -->|HDMI_CEC_IO_SUCCESS| E[Validate Added Logical Address]    
    E -->|Matched| F[Remove Logical Address]    
    F -->|HDMI_CEC_IO_SUCCESS| G[All Addresses processed?]
    G -->|No| C
    D <-->|NOT SUCCESS| I[Test Fail]
    E <-->|NOT SUCCESS| I[Test Fail]
    F <-->|NOT SUCCESS| I[Test Fail]
    G -->|Yes| H[Close HDMI CEC Driver]
```


### Test 3

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_RemoveLogicalAddress`|
|Description|Invoke the HAL API to delete the `DUT` logical address and verify that it is removed successfully.|
|Test Group|Module : 02|
|Test Case ID|003|
|Priority|High|

**Pre-Conditions :**
None

**Dependencies :** 
None

**User Interaction :** 
If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open the HDMI CEC driver using HdmiCecOpen API | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Add a logical address using HdmiCecAddLogicalAddress API | handle = valid handle, logicalAddress = 0x00 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Get the logical address using HdmiCecGetLogicalAddress API | handle = valid handle, logicalAddress = valid buffer | HDMI_CEC_IO_SUCCESS , logicalAddress = 0x00 | Should be successful |
| 04 | Remove the logical address using HdmiCecRemoveLogicalAddress API | handle = valid handle, logicalAddress = 0x00 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 05 | Get the logical address using HdmiCecGetLogicalAddress API | handle = valid handle, logicalAddress = valid buffer | HDMI_CEC_IO_SUCCESS , logicalAddress = 0x0F | Should be successful |
| 06 | Close the HDMI CEC driver using HdmiCecClose API | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TD
    style A fill:#bff,stroke:#333,stroke-width:2px
    style B fill:#bbf,stroke:#333,stroke-width:2px
    style C fill:#bbf,stroke:#333,stroke-width:2px
    style D fill:#bbf,stroke:#333,stroke-width:2px
    style E fill:#bbf,stroke:#333,stroke-width:2px
    style F fill:#bbf,stroke:#333,stroke-width:2px
    style G fill:#bff,stroke:#333,stroke-width:2px
    style H fill:#bbf,stroke:#333,stroke-width:2px
	  style I fill:#f00,stroke:#333,stroke-width:2px
	  style J fill:#bbf,stroke:#333,stroke-width:2px

    A[Open HDMI CEC Driver] --> B{HdmiCecOpen:\n handle = valid handle}
    B -->|HDMI_CEC_IO_SUCCESS| C[Add Logical Address]
	  C -->|NOT SUCCESS| I[Test Fail]
	  I -->G
    C -->|HDMI_CEC_IO_SUCCESS| D[Get Logical Address]
	  D -->|NOT SUCCESS| J[Remove Logical Address] --> I
    D -->|HDMI_CEC_IO_SUCCESS| E[Remove Logical Address]
	  E -->|NOT SUCCESS| I
    E -->|HDMI_CEC_IO_SUCCESS| F[Get Logical Address]
	  F -->|NOT SUCCESS| I
    F -->|HDMI_CEC_IO_SUCCESS| H[Compare Logical Address 0x0F]
	  H -->|NOT SUCCESS| I
    H -->|HDMI_CEC_IO_SUCCESS| G[Close HDMI CEC Driver]
```



### Test 4

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_BroadcastHdmiCecCommand`|
|Description|After deleting the `DUT` logical address, try to send a broadcast CEC Command (as per 1.4b HDMI CEC spec) and confirm transmission is successful.|
|Test Group|Module : 02|
|Test Case ID|004|
|Priority|High|

**Pre-Conditions :**
None

**Dependencies :** 
None

**User Interaction :** 
If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open HDMI CEC using HdmiCecOpen | handle = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Add logical address using HdmiCecAddLogicalAddress | handle = valid handle, logicalAddresses = 0x0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Remove logical address using HdmiCecRemoveLogicalAddress | handle = valid handle, logicalAddresses = 0x0 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 04 | Broadast CEC message using HdmiCecTx | handle = valid handle, buf = {0x0F, 0x84, 0x00, 0x00}, len = sizeof(buf), result = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 05 | Check the result of transmission | result = valid buffer | HDMI_CEC_IO_SENT_BUT_NOT_ACKD| Should be successful |
| 06 | Close HDMI CEC using HdmiCecClose | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TD
    style A fill:#bff,stroke:#333,stroke-width:2px
    style B fill:#bbf,stroke:#333,stroke-width:2px
    style C fill:#bbf,stroke:#333,stroke-width:2px
    style D fill:#bbf,stroke:#333,stroke-width:2px
    style E fill:#bbf,stroke:#333,stroke-width:2px
    style F fill:#bbf,stroke:#333,stroke-width:2px
    style G fill:#bff,stroke:#333,stroke-width:2px
	  style I fill:#f00,stroke:#333,stroke-width:2px

    A[Open HDMI CEC Driver] --> B{HdmiCecOpen:\n handle = valid buffer}
    B -->|HDMI_CEC_IO_SUCCESS| C[Add Logical Address] 
	  C -->|NOT SUCCESS| I[Test Fail]--> G
    C -->|HDMI_CEC_IO_SUCCESS| D[Remove Logical Address]
	  D -->|NOT SUCCESS| I[Test Fail]
    D -->|HDMI_CEC_IO_SUCCESS| E[Broadcast CEC Message]
	  E -->|NOT SUCCESS| I[Test Fail]
    E -->|HDMI_CEC_IO_SUCCESS| F[Check Transmission Result]
	  F -->|NOT SUCCESS| I[Test Fail]
    F -->|HDMI_CEC_IO_SENT_BUT_NOT_ACKD| G[Close HDMI CEC Driver]
```



### Test 5

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_VerifyPhysicalAddress`|
|Description|Verify the valid physical address allocated through the HAL function.|
|Test Group|Module : 02|
|Test Case ID|005|
|Priority|High|

**Pre-Conditions :**
None

**Dependencies :**
None

**User Interaction :**
If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Call the pre-requisite API HdmiCecOpen() | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Call the API HdmiCecGetPhysicalAddress() | handle = valid handle, physicalAddress = valid address | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Check the return status of HdmiCecGetPhysicalAddress() | status = return status of HdmiCecGetPhysicalAddress() | HDMI_CEC_IO_SUCCESS | Should be successful |
| 04 | Verify that the physical address obtained is less than F.F.F.F | physicalAddress = obtained physical address | physicalAddress < 0xFFFF | Should be successful |
| 05 | Call the post-requisite API HdmiCecClose() | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TD
    style A fill:#bff,stroke:#333,stroke-width:2px
    style B fill:#bbf,stroke:#333,stroke-width:2px
    style C fill:#bbf,stroke:#333,stroke-width:2px
    style D fill:#bff,stroke:#333,stroke-width:2px
	  style E fill:#f00,stroke:#333,stroke-width:2px
    
    A[Open HDMI CEC Driver] --> B{HdmiCecGetPhysicalAddress}
    B -->|Not SUCCESS | E[Test Fail] --> D
    B -->|HDMI_CEC_IO_SUCCESS| C[Verify physical address < 0xFFFF]
	  C -->|Not SUCCESS | E
    C -->D[Close HDMI CEC Driver]
```


### Test 6

|Title|Details|
|--|--|
|Function Name|`test_l2_hdmi_cec_driver_TransmitCECCommand`|
|Description|DUT transmit a CEC Command (as per 1.4b HDMI CEC spec) to get the CEC version of device that doesn't exist.|
|Test Group|Module : 02|
|Test Case ID|006|
|Priority|High|

**Pre-Conditions :**
None

**Dependencies :** 
None

**User Interaction :** 
If user chose to run the test in interactive mode, then the test case has to be selected via console.

#### Test Procedure :

| Variation / Steps | Description | Test Data | Expected Result | Notes|
| -- | --------- | ---------- | -------------- | ----- |
| 01 | Open HDMI CEC driver using HdmiCecOpen | handle = valid buffer | HDMI_CEC_IO_SUCCESS | Should be successful |
| 02 | Add logical address using HdmiCecAddLogicalAddress | handle = valid handle, logicalAddresses = 0x4 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 03 | Transmit CEC command using HdmiCecTx for a non existing device | handle = valid handle, buf = {0x47, 0x9F}, len = sizeof(buf), result = valid buffer | HDMI_CEC_IO_SUCCESS, result = HDMI_CEC_IO_SENT_BUT_NOT_ACKD | Should be successful |
| 04 | Remove logical address using HdmiCecRemoveLogicalAddress | handle = valid handle, logicalAddresses = 0x4 | HDMI_CEC_IO_SUCCESS | Should be successful |
| 05 | Close HDMI CEC driver using HdmiCecClose | handle = valid handle | HDMI_CEC_IO_SUCCESS | Should be successful |


```mermaid
graph TD
    style A fill:#bff,stroke:#333,stroke-width:2px
    style B fill:#bbf,stroke:#333,stroke-width:2px
    style C fill:#bbf,stroke:#333,stroke-width:2px
    style D fill:#bbf,stroke:#333,stroke-width:2px
	  style D fill:#bbf,stroke:#333,stroke-width:2px
    style F fill:#bff,stroke:#333,stroke-width:2px
	  style I fill:#f00,stroke:#333,stroke-width:2px

    A[Open HDMI CEC Driver] --> B{HdmiCecAddLogicalAddress}
	  B --> |NOT SUCCESS| I[TEST FAIL] --> F
    B -->|HDMI_CEC_IO_SUCCESS| C[Transmit CEC Command for \n a device not in the network]
	  C -->|NOT SUCCESS| E[Remove Logical Address]-->I
    C -->|HDMI_CEC_IO_SUCCESS| D[Check Tx Ack]
	  D -->|NOT SUCCESS| E[Remove Logical Address]
	  D -->|HDMI_CEC_IO_SENT_BUT_NOT_ACKD| G[Remove Logical Address]
	  G -->F[Close HDMI CEC]
```
