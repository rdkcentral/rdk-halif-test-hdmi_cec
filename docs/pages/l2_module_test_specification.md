# HDMI `CEC`  Level 2 - Module Testing
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

- `HAL` \- Hardware Abstraction Layer, may include some common components
- `HDMI`   - High-Definition Multimedia Interface
- `CEC`    - Consumer Electronics Control
- `RDK`  \- Reference Design Kit for All Devices
- `RDK-V`  \- Reference Design Kit for Video Devices
- `UT`  \- Unit Test(s)
- `OEM`  \- Original Equipment Manufacture (Sky is also an OEM)
- `SoC`  \- System on a Chip

### Definitions

- `Broadcom` \- `SoC` manufacturer <https://www.broadcom.com/>
- `Amlogic` \- `SoC` manufacturer <https://en.wikipedia.org/wiki/Amlogic>
- `Soc Vendor` \- Definition to encompass multiple vendors
- `Unit Tests` \- C Function tests that run on the target hardware
- `Common Testing Framework` \- Off the shelf 3rd Party Testing Framework, or framework that does not require infrastructure to control it. That's not to say it cannot be controlled via infrastructure if required. Examples of which are.
  - `GTest` \- Google Test Suit <https://google.github.io/googletest>
  - `CUnit` \- C Testing Suit <http://cunit.sourceforge.net/>
  - `Unity` \- C Embedded Testing Suit <http://www.throwtheswitch.org/unity>
  - `ut-core` \- Common Testing Framework <https://github.com/comcast-sky/rdk-components-ut-core>, which wraps a open-source framework that can be expanded to the requirements for future comcast-sky framework.

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
|Description| This function will request the version from the connected devices and check if the opcode is received within the expected time interval. In oder to be deterministic opcode should be fixed.|
|Test Group|02 (Module)|
|Test Case ID|001|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :

 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the `CEC`  version after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getVendorID_sink () |
|Description| This function will request the vendor ID from the connected devices and check if the opcode is received within the expected time interval. In oder to be deterministic opcode should be fixed.|
|Test Group|02 (Module)|
|Test Case ID|002|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the vendor id after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getPowerStatus_sink () |
|Description| This function will request the power status from the connected devices and check if the opcode is received within the expected time interval. In oder to be deterministic opcode should be fixed.|
|Test Group|02 (Module)|
|Test Case ID|003|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getPowerStatusAndToggle_sink () |
|Description| This function will request the HDMI CEC power status and toggle the current power state of the connected device.|
|Test Group|02 (Module)|
|Test Case ID|004|

**Pre-Conditions :**
-  There should be any one of the `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |05|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |06|call HdmiCecTx(handle, buf, len, &ret) - Based on the power status from the other device toggle the power state by passing IMAGE_VIEW_ON when other device is in standby or passing STANDBY when the other device is is power on | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after toggling the power state and check expected power state is returned | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |09|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_sendMsgHdmiDisconnected_sink () |
|Description| This function will request the vendor ID  when `HDMI` is in disconnected state and will confirm that response is not received within the expected time interval. |
|Test Group|02 (Module)|
|Test Case ID|005|

**Pre-Conditions :**
-   All of the device HDMI cable should be disconnected.

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the vendor id after correct module initialization and ensure receive callback is not triggered. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|07|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected_sink () |
|Description| This function will request the power status  when `HDMI` is in disconnected state and will confirm that response is not received within the expected time interval. |
|Test Group|02 (Module)|
|Test Case ID|006|

**Pre-Conditions :**
-   All of the device HDMI cable should be disconnected.

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after correct module initialization and ensure ensure receive callback is not triggered. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|07|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getCecVersion_source ()|
|Description| This function will request the version from the connected devices and check if the opcode is received within the expected time interval. In oder to be deterministic opcode should be fixed.|
|Test Group|02 (Module)|
|Test Case ID|007|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the `CEC`  version after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getVendorID_source ()|
|Description| This function will request the vendor ID from the connected devices and check if the opcode is received within the expected time interval. In oder to be deterministic opcode should be fixed.|
|Test Group|02 (Module)|
|Test Case ID|008|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the vendor id after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getPowerStatus_source ()|
|Description| This function will request the power status from the connected devices and check if the opcode is received within the expected time interval. In oder to be deterministic opcode should be fixed.|
|Test Group|02 (Module)|
|Test Case ID|009|

**Pre-Conditions :**
-  There should be a `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_getPowerStatusAndToggle_source () |
|Description| This function will request the HDMI CEC power status and toggle the current power state of the connected device.|
|Test Group|02 (Module)|
|Test Case ID|010|

**Pre-Conditions :**
-  There should be any one of the `HDMI` `CEC` enabled device connected via HDMI

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
 |Variation / Step|Description|Test Data|Expected Result|Notes|
 |:--:|---------|----------|--------------|-----|
 |01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 |02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 |04|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |07|call HdmiCecTx(handle, buf, len, &ret) - Based on the power status from the other device toggle the power state by passing IMAGE_VIEW_ON when other device is in standby or passing STANDBY when the other device is is power on | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |08|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after toggling the power state and check expected power state is returned | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 |09|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 |10|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |


|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_sendMsgHdmiDisconnected_source ()|
|Description| This function will request the vendor ID  when `HDMI` is in disconnected state and will confirm that response is not received within the expected time interval. |
|Test Group|02 (Module)|
|Test Case ID|011|

**Pre-Conditions :**
-   All of the device HDMI cable should be disconnected.

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the vendor id after correct module initialization and ensure receive callback is not triggered. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

|Title|Details|
|--|--|
|Function Name|test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected_source ()|
|Description| This function will request the power status  when `HDMI` is in disconnected state and will confirm that response is not received within the expected time interval. |
|Test Group|02 (Module)|
|Test Case ID|012|

**Pre-Conditions :**
-   All of the device HDMI cable should be disconnected.

**Dependencies :** N/A

**User Interaction :** N/A

#### Test Procedure :
|Variation / Step|Description|Test Data|Expected Result|Notes|
|:--:|---------|----------|--------------|-----|
|01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
|02|call HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set RX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
|04|call HdmiCecTx(handle, buf, len, &ret) - send the `CEC`  message to get the power status after correct module initialization and ensure ensure receive callback is not triggered. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
|05|call HdmiCecSetRxCallback(handle, NULL, 0) - unregister RX call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
|06|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |

#TODO write a function to query the get power status if the device is in standby send image view on command and see device is waking up and if device in power on state send standby message and ensure device is going to sleep state.
