# HDMI CEC Level 2 - Module Testing
## History

| Version | Date(YY-MM-DD) |Comments |
| -------| ----- | ----- |
| 1.0.0 | 23/04/28 | Initial Document |

## Table of Contents

- [Overview](#overview)
  - [Acronyms](#acronyms)
  - [Definitions](#definitions)
  - [References](#references)
- [Level 2 Test Suite](#level-2-test-suite)
  - [Functions to check the `CEC` transmit and receive messages](#functions-to-check-the-CEC-transmit-and-receive-messages)
 
## Overview

This document describes the level 2 testing suite for the `HDMI` `CEC` module.

### Acronyms, Terms and Abbreviations

- `HAL`   \- Hardware Abstraction Layer, may include some common components
- `HDMI`   - High-Definition Multimedia Interface
- `CEC`    - Consumer Electronics Control
- `RDK`   \- Reference Design Kit for All Devices
- `RDK-V` \- Reference Design Kit for Video Devices
- `UT`    \- Unit Test(s)
- `TV`    \- Television

### Definitions

- `Unit Tests` \- C Function tests that run on the target hardware

### References

- `Feedback Loops` \- <https://www.softwaretestingnews.co.uk/4-methods-to-improve-your-feedback-loops-and-supercharge-your-testing-process/>
- `Doxygen` \- SourceCode documentation tool - <https://www.doxygen.nl/index.html>
- `Black Box Testing` \- <https://en.wikipedia.org/wiki/Black-box_testing>

## Level 2 Test Suite

The following functions test module functionality.

### Functions to check the `CEC` transmit and receive messages

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getCecVersion_sink() |
|Description| This function will request the version from the connected devices and check if the valid opcode is received within the expected time interval. In order to be deterministic opcode should be fixed|
|Test Group|02 (Module)|
|Test Case ID|001|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled source device connected via HDMI port (Tuner, playback device)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :

 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|Call `HdmiCecTx()` - send the `CEC`  message to get the `CEC`  version after correct module initialization and ensure response is received within expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getVendorID_sink () |
|Description| This function will request the vendor ID from the connected devices and check if the valid opcode is received within the expected time interval. In order to be deterministic opcode should be fixed|
|Test Group|02 (Module)|
|Test Case ID|002|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled source device connected via HDMI port (Tuner, playback device)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|Call `HdmiCecTx()` - send the `CEC`  message to get the vendor id after correct module initialization and ensure response is received within expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getPowerStatus_sink () |
|Description| This function will request the power status from the connected devices and check if the valid opcode is received within the expected time interval. In order to be deterministic opcode should be fixed|
|Test Group|02 (Module)|
|Test Case ID|003|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled source device connected via HDMI port (Tuner, playback device)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|Call `HdmiCecTx()` - send the `CEC`  message to get the power status after correct module initialization and ensure response is received within expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_TogglePowerState_sink () |
|Description| This function will toggle the current power state of the connected device|
|Test Group|02 (Module)|
|Test Case ID|004|

**Pre-Conditions :**
-  There should be any one of the `HDMI` `CEC` enabled source device connected via HDMI (Tuner, playback device)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|Call `HdmiCecTx()` - Send standby `CEC` message to the connected device. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|Call `HdmiCecTx()` - Query power state and ensure connected device is in power off state | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|Call `HdmiCecTx()` - Request physical address from the receiver device and ensure physical address is received | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |09|Call `HdmiCecTx()` - Broadcast set stream path to receiver using its physical address | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |10|Call `HdmiCecTx()` - Query power state and ensure connected device is in power on state | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |11|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |12|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_validateHdmiCecConnection_sink () |
|Description| This function will request the vendor ID  when `HDMI` is in disconnected state and will confirm that response is not received within the expected time interval|
|Test Group|02 (Module)|
|Test Case ID|005|

**Pre-Conditions :**
-   All of the device HDMI cable should be disconnected

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|Call `HdmiCecTx()` - ping all `CEC` devices and ensure none of the pings are successful| handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|07|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_back_to_back_send_sink () |
|Description| This function will ensure back to back CEC message send is working as expected for sink devices by sending power status query and vendor id query back to back loop|
|Test Group|02 (Module)|
|Test Case ID|006|

**Pre-Conditions :**
-  There should be more than one `HDMI` `CEC` enabled source device connected via HDMI port and running this back to back send test parallel (Tuner, playback device)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|Call `HdmiCecTx()` - send the `CEC`  message to get the vendor id query in a loop | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|Call `HdmiCecTx()` - send the `CEC`  message to get the power status query in a loop | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |09|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getCecVersion_source ()|
|Description| This function will request the version from the connected devices and check if the valid opcode is received within the expected time interval. In order to be deterministic opcode should be fixed|
|Test Group|02 (Module)|
|Test Case ID|007|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled sink device connected via HDMI (`TV`)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|Call `HdmiCecTx()` - send the `CEC`  message to get the `CEC`  version after correct module initialization and ensure response is received within expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|07|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getVendorID_source ()|
|Description| This function will request the vendor ID from the connected devices and check if the valid opcode is received within the expected time interval. In order to be deterministic opcode should be fixed|
|Test Group|02 (Module)|
|Test Case ID|008|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled sink device connected via HDMI (`TV`)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|Call `HdmiCecTx()` - send the `CEC`  message to get the vendor id after correct module initialization and ensure response is received within expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|07|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getPowerStatus_source ()|
|Description| This function will request the power status from the connected devices and check if the valid opcode is received within the expected time interval. In order to be deterministic opcode should be fixed|
|Test Group|02 (Module)|
|Test Case ID|009|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled sink device connected via HDMI (`TV`)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|Call `HdmiCecTx()` - send the `CEC`  message to get the power status after correct module initialization and ensure response is received within expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|07|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_TogglePowerState_source () |
|Description| This function will toggle the current power state of the connected device|
|Test Group|02 (Module)|
|Test Case ID|010|

**Pre-Conditions :**
-  There should be any one of the `HDMI` `CEC` enabled sink device connected via HDMI (`TV`)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|Call `HdmiCecTx()` - Send standby `CEC` message to the connected device. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|Call `HdmiCecTx()` - Query power state and ensure connected device is in power off state | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|Call `HdmiCecTx()` - Send image view on command to receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|Call `HdmiCecGetPhysicalAddress()` - Get physical address of the device | handle, physicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |09|Call `HdmiCecTx()` - Broadcast active source with device's own physical address | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |10|Call `HdmiCecTx()` - Query power state and ensure connected device is in power on state | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |11|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |12|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_validateHdmiCecConnection_source ()|
|Description| This function will request the vendor ID  when `HDMI` is in disconnected state and will confirm that response is not received within the expected time interval|
|Test Group|02 (Module)|
|Test Case ID|011|

**Pre-Conditions :**
-   All of the device HDMI cable should be disconnected

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|Call `HdmiCecTx()` - ping all `CEC` devices and ensure none of the pings are successful| handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|07|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_back_to_back_send_source () |
|Description| This function will ensure back to back CEC message send is working as expected for source devices by sending power status query and vendor id query back to back loop|
|Test Group|02 (Module)|
|Test Case ID|012|

**Pre-Conditions :**
-  There should be more than one `HDMI` `CEC` enabled source device connected via HDMI port and running this back to back send test parallel (TV,Tuner, playback device)

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|Call `HdmiCecTx()` - ping all `CEC` devices to get the logical address of receiver | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|Call `HdmiCecTx()` - send the `CEC`  message to get the vendor id query in a loop | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|Call `HdmiCecTx()` - send the `CEC`  message to get the power status query in a loop | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

