
# HDMI CEC High-Level Test Specification Document

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Overview](#overview)
- [HDMI-CEC RDK HAL Functionality](#hdmi-cec-rdk-hal-functionality)

## Acronyms, Terms and Abbreviations

- `CEC`  - Consumer Electronics Control
- `HAL`  - Hardware Abstraction layer
- `HDMI` - High Definition Multimedia Interface
- `DUT`  - Device Under Test  
- `L2`   - Level 2 Testing ()
- `L3`   - Level 3 Testing ()

## Scope

This document defines the requirements for testing the HDMI CEC device from a level 2/3 based on being a sink device.

## Overview

Consumer Electronics Control (CEC) is a single-wire bidirectional bus within an HDMI system, facilitating communication among interconnected products. HDMI-CEC establishes a protocol enabling high-level control functions between audiovisual devices linked via an HDMI network, facilitating communication and control among them. Communication can occur in either Direct messaging mode or Broadcast mode. Below is a diagram illustrating a basic network setup:

![CEC_Network](https://github.com/rdkcentral/rdk-halif-test-hdmi_cec/assets/71806084/133a9c82-0cf8-4184-97c6-021b9e10ec4a)

## HDMI-CEC RDK HAL Functionality

The HAL layers within RDK serve as a bridge between the underlying low-level SoC drivers and the higher-level RDK layers that utilize the functionality offered by these HAL functions. Caller will manage the discovery of logical addresses in sink devices, while HAL needs to facilitate sending and receiving the CEC commands on the network. Specifically concerning the CEC Module, the HAL layers facilitate the following functionalities:

## Test scenarios

The HAL CEC layer facilitates the transmission and reception of CEC information on the CEC bus. It does not handle any specific opcode commands, nor does it validate supported HAL CEC opcodes for sending or receiving.

It is the responsibility of the caller to manage the opcodes. The current test cases will verify responses from connected devices for a subset of opcodes as part of the testing process.

|S.No.|Test Functionality|Description|
|-----|------------------|-----------|
| 1 |[Logical address](#logical-address-discovery)|Facilitating the Discovery of logical addresses, Setting, getting, and removing the logical address of the device (for sink devices) |
| 2| [Physical Address](#physical-address)| Retrieving the physical address |
| 3| [CEC Transmission](#cec-transmission)| Transmitting CEC frames and acknowledging them |
| 4| [CEC Async Transmission](#cec-async-transmission) | Asynchronously transmitting CEC frames without requiring acknowledgment |
| 5| [CEC Receive functionality](#cec-receive-functionality)| Receiving CEC information from other devices and communicating it to the above layers through registered callback functions |
| 6| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Managing CEC during Hotplug and HotUnplug events |

-----------

## Logical Address Discovery

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 1 |[Logical address](#logical-address-discovery)|Get the logical address of the `DUT` without actually adding the Logical Address and the API should return 0x0F as the default logical address.|HdmiCecGetLogicalAddress|Y|NA
|a| |Set up a legitimate logical address (0x00 to 0x0F) for the `DUT` using HAL APIs, then retrieve it to ensure proper functionality.|HdmiCecAddLogicalAddress, HdmiCecGetLogicalAddress| Y | NA |
|b| | Invoke the HAL API to delete the `DUT` logical address and verify that it is removed successfully.  |HdmiCecAddLogicalAddress, HdmiCecRemoveLogicalAddress, HdmiCecGetLogicalAddress| Y | NA|
|c| | After deleting the `DUT` logical address, try to send a broadcast command. Should fail to send during HAL Transmission,  call.|HdmiCecAddLogicalAddress, HdmiCecRemoveLogicalAddress, HdmiCecTx | Y|NA||

### Emulator Requirements

- Boot with control configuration with various configurations having a predefined set of nodes:
  - configuration to support the discovery of logical addresses. The caller provides the logical address, and HAL checks the availability of this address and feedback the same to the caller. 
  - Verify for the valid logical address and return the appropriate error code based on the logical address availability.  

### Control Plane Requirements

- The control plane will allow removing or adding a node to the network.
  - allowing add node sink node logical address 14
  - allowing add node sink node logical address 0
  - Support the CEC commands from the external devices on L3 Test Cases. 

## Physical Address

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 2| [Physical Address](#physical-address)| Verify the valid physical address allocated through the HAL function.|HdmiCecGetPhysicalAddress| Y  |NA||
| | | Verify the physical addresses allocated by connecting two sink devices through an HDMI switch.| HdmiCecGetPhysicalAddress| NA | Y  | Enable the television connected to `DUT` Tele first so that it can declare its physical address first.  |

### Emulator Requirements - Physical Address

- Boot control configuration to setup the CEC network nodes
- Scenario to have two sink devices on the network

### Control Plane Requirements - Physical Address

- The control plane will allow removing or adding a node to the network.
  - allowing add node sink node

## CEC Synchronous Transmission

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 3| [CEC Transmission](#cec-transmission)| Verify the correct transmission of the supported CEC commands (as per 1.4b HDMI CEC spec) to the connected device and ensure it is acknowledged properly.| HdmiCecTx | NA | Y  ||
| | | Broadcast a supported CEC Command to all the devices connected to the network without any error |HdmiCecTx| NA | Y  ||
| | | Transmit a CEC Command (as per 1.4b HDMI CEC spec) to put the connected device into standby mode and await the device's response. Monitor the behaviour of the connected device accordingly.| HdmiCecTx | NA | Y  | Control panel to monitor the behavior of the connected devices.  |
| | | Transmit a CEC Command (as per 1.4b HDMI CEC spec) to get the CEC Version for a logical address that doesn't exist.| HdmiCecTx | Y | NA  |   |

### Emulator Requirements - CEC Transmission
- Boot configuration
  - Min case scenario multiple network nodes
  - Max case scenario multiple cec nodes

### Control Plane Requirements - CEC Transmission
- The control plane will allow putting nodes into standby mode, this will cause a CEC message on the network

## CEC Receive functionality

|S.No.|Test Functionality|Description| HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|---------|--|--|--------------------------|
| 5| [CEC Receive functionality](#cec-receive-functionality)| Transmit a CEC Command that expects a response (Eg. GetCECVersion) to a connected device and see the response is received correctly. Set the Rx Callback before sending the data. Validate the received CEC Version.|HdmiCecSetRxCallback HdmiCecTx | NA |Y| |
| | | Transmit all the CEC Broadcast Command (as per 1.4b HDMI CEC spec) which expects a response from all the connected devices like `<GetPhysicalAddress>` and validate the data received.|HdmiCecSetRxCallback HdmiCecTx | NA | Y  ||
| | | Transmit a CEC command from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (`DUT` TV here)| HdmiCecSetRxCallback | NA | Y  | Control panel to control the third-party devices to Transmit the required commands to  `DUT`|
| | | Transmit an OSD CEC command from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (`DUT` TV here). Make the OSD String to max length| HdmiCecSetRxCallback | NA | Y  | Control panel to control the third-party devices to Transmit the required commands to  `DUT`|
| | | Transmit an OSD CEC command from the connected devices continuously and consider the Acknowledgement and responses are received correctly from the host device (`DUT` TV here). Make the OSD String to max length| HdmiCecSetRxCallback | NA | Y  | Control panel to control the third-party devices to Transmit the required commands to  `DUT`|

### Emulator Requirements - CEC Receive functionality
1. Emulate the Tx and Rx HAL functionalities with the required responses.

### Control Plane Requirements - CEC Receive functionality
1. Control the devices to switch ON and respond to the CEC commands.

## CEC HotPlug Functionality

|S.No.|Test Functionality|Description| HAL APIs |L2|L3|Control plane requirements|
|-----|------------------|-----------|----------|--|--|--------------------------|
| 6| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Generate a Hotplug event by disconnecting the device connected to the HDMI port of the Sink Platform. Validating whether the CEC Transmission works when the HDMI port is disconnected should result in ACK not being received while the TX still works as expected. | HdmiCecTx | NA | Y  | Control Panel to control the Hotplug activities |
| | | Check the behaviour when a device has been remove from the network which is not directly connected to the TV device.  Send a CEC Tx command with acknowledgement using HAL Interface and check the behaviour. The Tx command should fail in this state.| HdmiCecTx | NA | Y  | Control Panel to control the external devices connected.|

### Emulator Requirements - CEC HotPlug Functionality

### Control Plane Requirements - CEC HotPlug Functionality
1. Controlling the connected devices to create the HotPlug functionality.

-----------
-----------

## Hardware Verification Testing Requirements

|S.No.|Test Functionality|Description| HAL APIs |L2|L3|Control plane requirements|
|-----|------------------|-----------|----------|--|--|--------------------------|
| 7| Introduce fault in the CEC Bus | Absorve the behaviour when the CEC line is pulled high during the CEC Transmission.| HdmiCecTx | NA | Y  |Fault Introduction|
| 8| Overloading the CEC bus. | Overload the CEC bus with too many messages  (by connecting more devices in the network) and observe the behaviour| HdmiCecTx | NA | Y  |Fault Introduction|

### Emulator Requirements
1. Emulator to support the HDMI_CEC_IO_SENT_FAILED during the above scenarios
   
### Control Plane Requirements
1. Control Plane to control the external devices to pull either the CEC line high.
2. Control Plan to initiate multiple CEC commands from the different devices connected to the network.
-----------
-----------

## Boot configuartion

### Module Configuration Requirements

The module must be configured during the boot sequence in the case of emulation as if it were a real hardware device with or without multiple connected HDMI nodes.

### Test Configuration for Sink Devices

The following information shall be helpful for further running the Automation Rack Test for this specific module and further configuration will help for the Design and Development of Virtual Device.

Configurations:

```yaml
  Device:
    Type: Source / Sink
    Platform_Manufacturer: sony/Samsung etc.
    Platform_Model:  xyz
    Port: 3, 4
    HDMI Node: 1-3 etc.
    CEC_Ports: 1, x, y
```
