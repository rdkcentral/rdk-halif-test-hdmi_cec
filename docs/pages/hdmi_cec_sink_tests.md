
# HDMI CEC High Level Test Specification Document

## Version History

| Date(DD/MM/YY) | Comment | Version |
| -------------- | ------- | ------- |
| 13/02/24 | First Release | 1.0.0 |

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

The HAL layers within RDK serve as a bridge between the underlying low-level SoC drivers and the higher-level RDK layers that utilize the functionality offered by these HAL functions. Specifically concerning the CEC Module, the HAL layers facilitate the following functionalities:

## Test scenarios

The HAL CEC layer facilitates the transmission and reception of CEC information on the CEC bus. It does not handle any specific opcode commands, nor does it validate supported HAL CEC opcodes for sending or receiving.

It is the responsibility of the caller to manage the opcodes. The current test cases will verify responses from connected devices for a subset of opcodes as part of the testing process.

|S.No.|Test Functionality|Description|
|-----|------------------|-----------|
| 1 |[Logical address](#logical-address-discovery)|Discovery of logical addresses, Setting, getting, and removing the logical address of the device (for sink devices) |
| 2| [Physical Address](#physical-address)| Retrieving the physical address |
| 3| [CEC Transmission](#cec-transmission)| Transmitting CEC frames and acknowledging them |
| 4| [CEC Async Transmission](#cec-async-transmission) | Asynchronously transmitting CEC frames without requiring acknowledgment |
| 5| [CEC Receive functionality](#cec-receive-functionality)| Receiving CEC information from other devices and communicating it to the above layers through registered callback functions |
| 6| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Managing CEC during Hotplug and HotUnplug events |

-----------

## Logical Address Discovery

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 1 |[Logical address](#logical-address-discovery)|Set up a legitimate logical address for the `DUT` using HAL APIs, then retrieve it to ensure proper functionality.|HdmiCecAddLogicalAddress, HdmiCecGetLogicalAddress|Y|NA|
|a| | If the logical address is configured to any value except 0 (Allocated TV address), 0x14 (Specific Use), or 0x15 (Unregistered), it should result in a failure for a TV `DUT`|HdmiCecAddLogicalAddress| Y |NA |
|b| | Invoke the HAL API to delete the `DUT` logical address and verify that it is removed successfully.  |HdmiCecAddLogicalAddress, HdmiCecRemoveLogicalAddress, HdmiCecGetLogicalAddress| Y | NA|
|c| | The attempt to obtain the `DUT` logical address should fail if more than two TV devices are connected.| HdmiCecAddLogicalAddress |NA | Y | Enable two televisions that can acquire the logical address 0 and 14 before attempting `DUT` Tele to acquire the logical address. |
|d| | After deleting the `DUT` logical address, try to send a broadcast command. Should fail to send during HAL Transmission,  call.|HdmiCecAddLogicalAddress, HdmiCecRemoveLogicalAddress, HdmiCecTx | Y|NA||

### Emulator Requirements

- Boot with control configuration with various configurations having a predefined set of nodes:
  - configuration to support two sinks setup, consuming 0 & 14 as a logical address.
  - configuration to support a sink with 0 allocated
  - configuration to support a sink setup with 14 allocated

### Control Plane Requirements

- The control plane will allow removing or adding a node to the network.
  - allowing add node sink node logical address 14
  - allowing add node sink node logical address 0

## Physical Address

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 2| [Physical Address](#physical-address)| Verify the valid physical address allocated through the HAL function.|HdmiCecGetPhysicalAddress| Y  |NA||
| | | Verify the physical addresses allocated by connecting two sink devices through an HDMI switch.|NA | NA | Y  | Enable the television connected to `DUT` Tele first so that it can declare its physical address first.  |

### Emulator Requirements - Physical Address

- Boot control configuration to setup the CEC network nodes
  - Two sink devices are connected to the network

### Control Plane Requirements - Physical Address

- The control plane will allow removing or adding a node to the network.
  - allowing add node sink node

## CEC Synchronous Transmission

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
| 3| [CEC Transmission](#cec-transmission)| Verify the correct transmission of all the supported CEC commands (as per 1.4b HDMI CEC spec) to the connected device and ensure it is acknowledged properly.  | NA | Y  ||
| | | Broadcast all the supported CEC Commands ((as per 1.4b HDMI CEC spec)) to all the devices connected to the network and receive the response.| NA | Y  ||
| | | Transmit all the  CEC Command (as per 1.4b HDMI CEC spec) to put the connected device into standby mode and await the device's response. Monitor the behaviour of the connected device accordingly.   | NA | Y  | Control panel to monitor the behavior of the connected devices.  |

### Emulator Requirements - CEC Transmission

- Boot configuration
  - Min case scenario multiple network nodes
  - Max case scenario multiple cec nodes

### Control Plane Requirements - CEC Transmission

- The control plane will allow putting nodes into standby mode, this will cause a CEC message on the network

## CEC Async Transmission

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
 4| [CEC Async Transmission](#cec-async-transmission) | Verify the correct transmission of all the CEC commands (as per 1.4b HDMI CEC spec) using the Async Transmission support of the CEC HAL interface.   | NA | Y  ||
| | | Verify the correct behaviour of the connected device when the CEC standby command is sent to it.| NA | Y  | Control panel to monitor the behaviour of the connected devices.  |
||| Broadcast all the CEC Commands ((as per 1.4b HDMI CEC spec)) to all the devices connected in the network and check the behaviour and response. | NA | Y  | Control panel to monitor the behaviour of the connected devices.  |

### Emulator Requirements - CEC Async Transmission


### Control Plane Requirements - CEC Async Transmission


## CEC Receive functionality

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
| 5| [CEC Receive functionality](#cec-receive-functionality)| Transmit a CEC Command that expects a response (Eg. GetCECVersion) to a connected device and see the response is received correctly. | NA |Y| |
| | | Transmit all the CEC Broadcast Command (as per 1.4b HDMI CEC spec) which expects a response from all the connected devices like `<GetPhysicalAddress>`  | NA | Y  ||
| | | Transmit Different CEC commands from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (`DUT` TV here)| NA | Y  | Control panel to control the third-party devices to Transmit the required commands to  `DUT`|

### Emulator Requirements - CEC Receive functionality

### Control Plane Requirements - CEC Receive functionality

## CEC HotPlug Functionality

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
| 6| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Generate a Hotplug event by disconnecting the device connected to the HDMI port of the Sink Platform. Check whether the CEC Transmission works when the HDMI port is disconnected. | NA | Y  | Control Panel to control the Hotplug activities |
| | | Verify  the behaviour when a device has been removed from the network which is not directly connected to the TV device.  Send a CEC Async Tx command using HAL Interface and check the behaviour. The Tx Command should not fail.   | NA | Y  | Control Panel to control the devices connected on the CEC Network.|
| | | Check the behaviour when a device has been remove from the network which is not directly connected to the TV device.  Send a CEC Tx command with acknowledgement using HAL Interface and check the behaviour. The Tx command should fail in this state. | NA | Y  | Control Panel to control the external devices connected.|

### Emulator Requirements - CEC HotPlug Functionality

### Control Plane Requirements - CEC HotPlug Functionality

-----------
-----------

## Hardware Verification Testing Requirements

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
| 7| Introduce fault in the CEC Bus | Absorve the behaviour when the CEC line is pulled high during the CEC Transmission.| NA | Y  |Fault Introduction|
| 8| Overloading the CEC bus. | Overload the CEC bus with too many messages  (by connecting more devices in the network) and observe the behaviour| NA | Y  |Fault Introduction|

### Emulator Requirements

### Control Plane Requirements

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
