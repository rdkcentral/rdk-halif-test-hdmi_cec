# HDMI CEC Test Document

## Version History

| Date(DD/MM/YY) | Comment | Version |
| -------------- | ------- | ------- |
| 13/02/24 | First Release | 1.0.0 |

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Overview](#overview)
- [HDMI-CEC RDK HAL Functionality](#hdmi-cec-rdk-hal-functionality)

## Acronyms, Terms and Abbreviations

- `CEC` - Consumer Electronics Control
- `HAL` - Hardware Abstraction layer
- `HDMI` - High Definition Multimedia Interface
- `L2` - Level 2 Testing ()
- `L3` - Level 3 Testing ()
- `Sink device` - Any piece of equipment or technology that provides an input signal or data to another device or system.
- `Source Device` - Any piece of equipment or technology that receives an input signal or data from another device or system.

## Scope

This document defines the requirements for testing the HDMI CEC device from a level 2/3 based on being a source device.

## Overview

Consumer Electronics Control (CEC) is a single-wire bidirectional bus within an HDMI system, facilitating communication among interconnected products. HDMI-CEC establishes a protocol enabling high-level control functions between audiovisual devices linked via an HDMI network, facilitating communication and control among them. Communication can occur in either Direct messaging mode or Broadcast mode. 

## HDMI-CEC RDK HAL Functionality

The HAL layers within RDK serve as a bridge between the underlying low-level SoC drivers and the higher-level RDK layers that utilize the functionality offered by these HAL functions. Specifically concerning the CEC Module, the HAL layers facilitate the following functionalities:

- Get Logical address
- Get Physical address
- Syncronous transmission, and communicating via hotplug connectivity

## Test scenarios

The HAL CEC layer facilitates the transmission and reception of CEC information on the CEC bus. It does not handle any specific opcode commands, nor does it validate supported HAL CEC opcodes for sending or receiving.

It is the responsibility of the caller to manage the opcodes. The current test cases will verify responses from connected devices for a subset of opcodes as part of the testing process.

|S.No.|Test Functionality|Description|
|-----|------------------|-----------|
| 1 |[Logical address](#logical-address-discovery)|Facilitating the Discovery of logical addresses getting, and removing the logical address of the device (for sink devices) |
| 2| [Physical Address](#physical-address)| Retrieving the physical address |
| 3| [CEC Synchronous Transmission](#cec-synchronous-transmission)| Transmitting CEC frames and acknowledging them |
| 4| [CEC Receive functionality](#cec-receive-functionality)| Receiving CEC information from other devices and communicating it to the above layers through registered callback functions |
| 5| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Managing CEC during Hotplug and HotUnplug events |

-----------

## Logical Address Discovery

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 1 |[Logical address](#logical-address-discovery)|Get the logical address of the `DUT`. This will add the logical address, as per source functionality. |HdmiCecGetLogicalAddress|Y|NA

### Emulator Requirements

- Boot with control configuration with various configurations having a predefined set of nodes:
  - configuration to support the discovery of logical addresses. The caller provides the logical address, and HAL checks the availability of this address and feedback the same to the caller. 
  - Verify for the valid logical address and return the appropriate error code based on the logical address availability.  

### Control Plane Requirements

- The control plane will allow removing or adding a node to the network.
  - allowing adding/removing node sink node logical address 0
  - allowing adding/removing source node
  - Support the CEC commands from the external devices on L3 Test Cases. 

### Physical Address

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 1| [Physical Address](#physical-address)| Verify the valid physical address allocated through the HAL function.|HdmiCecGetPhysicalAddress| Y  |NA||
| | | Verify the physical addresses allocated by connecting a source and sink device through an HDMI switch.|NA | NA | Y  | Enable the television connected to my Tele first so that it can declare its physical address first.  |

#### Emulator Requirements - Physical Address

- Boot control configuration to setup the cec network nodes
  - A source and sink devices are connected to the network

#### Control Plane Requirements - Physical Address

- Control plane will allow removing or adding a node to the network.
  - allowing adding/removing node sink node
  - allowing adding/removing node source node

### CEC Synchronous Transmission

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 3| [CEC Transmission](#cec-transmission)| Verify the correct transmission of the supported CEC commands (as per 1.4b HDMI CEC spec) to the connected device and ensure it is acknowledged properly.| HdmiCecTx | NA | Y  ||
| | | Broadcast a supported CEC Command to all the devices connected to the network without any error |HdmiCecTx| NA | Y  ||
| | | Transmit a CEC Command (as per 1.4b HDMI CEC spec) to put the connected device into standby mode and await the device's response. Monitor the behaviour of the connected device accordingly.| HdmiCecTx | NA | Y  | Control panel to monitor the behavior of the connected devices.  |
| | | Transmit a CEC Command (as per 1.4b HDMI CEC spec) to get the CEC Version for a logical address that doesn't exist.| HdmiCecTx | Y | NA  |   |

#### Emulator Requirements - CEC Transmission
- Boot configuration
  - Min case scenario multiple network nodes
  - Max case scenario multiple network nodes

#### Control Plane Requirements - CEC Transmission
- The control plane will allow putting nodes into standby mode, this will cause a CEC message on the network

### CEC Receive functionality

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
| 4| [CEC Receive functionality](#cec-receive-functionality)| Transmit a CEC Command that expects a response (Eg. GetCECVersion) to a connected device and see the response is received correctly. | NA |Y| |
| | | Transmit all the CEC Broadcast Command (as per 1.4b HDMI CEC spec) which expects a response from all the connected devices like `<GetPhysicalAddress>`  | NA | Y  ||
| | | Transmit Different CEC commands from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (my TV here)| NA | Y  | Control panel to control the third-party devices to Transmit the required commands to my Device (Device under test) |

#### Emulator Requirements - CEC Receive functionality

#### Control Plane Requirements - CEC Receive functionality

- The control panel to handle and facilitate communicate with third party devices to receive and send commands.

### CEC HotPlug Functionality

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
| 5| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Generate an Hotplug event by disconnecting the device connected to the HDMI port of the source Platform. Check whether the CEC Transmission works when HDMI port is disconnected. | NA | Y  | Control Panel to control the Hotplug activities |
| | | Check the behaviour when a device has been remove from the network which is not directly connected to the TV device.  Send a CEC Tx command with acknowledgement using HAL Interface and check the behaviour. The Tx command should fail in this state.| HdmiCecTx | NA | Y  | Control Panel to control the external devices connected.|
| | | Check the behaviour when a device has been remove from the network which is not directly connected to the TV device.  Send a CEC Tx command with acknowledgement using HAL Interface and check the behaviour. The Tx command should fail in this state. | NA | Y  | Control Panel to control the external devices connected.|

#### Emulator Requirements - CEC HotPlug Functionality

#### Control Plane Requirements - CEC HotPlug Functionality

- The control panel to handle and facilitate communicate with third party devices to receive and send commands.

-----------
-----------

## Hardware Verification Testing Requirements

|S.No.|Test Functionality|Description|L2|L3|Control plane requirements|
|-----|------------------|-----------|--|--|--------------------------|
| 6| Introduce fault in the CEC Bus | Absorve the behaviour when the CEC line is pulled high during the CEC Transmission.| NA | Y  |Fault Introduction|
| 7| Overloading the CEC bus. | Overload the CEC bus with too many messages  (by connecting more devices in the network) and observe the behaviour| NA | Y  |Fault Introduction|

### Emulator Requirements

### Control Plane Requirements

-----------
-----------

## Boot configuration

### Module Configuration Requirements

The module must be configured during the boot sequence in the case of emulation, as if it was a real hardware device with or without multiple connected HDMI nodes.

### Test Configuration for source Devices

The following information shall be helpful for further running the Automation Rack Test for this specific module and further the configuration will help for the Design and Development of Virtual Device.

Configurations:

```yaml
  Device:
    Type: Source / Sink
    Platform_Manufacturer: sony/samsung etc.
    Platform_Model:  xyz
    Port: 3, 4
    HDMI Node: 1-3 etc.
    CEC_Ports: 1, x, y
```