
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

Consumer Electronics Control (CEC) is a single-wire bidirectional bus within an HDMI system, facilitating communication among interconnected products. HDMI-CEC establishes a protocol enabling high-level control functions between audiovisual devices linked via an HDMI network, facilitating communication and control among them. Communication can occur in either Direct messaging mode or Broadcast mode. 

## HDMI-CEC RDK HAL Functionality

The HAL layers within RDK serve as a bridge between the underlying low-level SoC drivers and the higher-level RDK layers that utilize the functionality offered by these HAL functions. Caller will manage the discovery of logical addresses in sink devices, while HAL needs to facilitate sending and receiving the CEC commands on the network. Specifically concerning the CEC Module, the HAL layers facilitate the following functionalities on sink devices:

1. Provision to set, get, and remove the logical address
2. Provision to get the Physical address
3. Provision to Tx and Rx the CEC data 

## Test scenarios

The HAL CEC layer enables the transmission and reception of CEC frames on the CEC bus. However, it does not manage or validate any particular CEC opcode commands to confirm the supported HAL CEC opcodes for transmission or reception.

Managing the opcodes is the responsibility of the caller. The existing test cases will validate responses from connected devices for a subset of opcodes as part of the testing procedure.

|S.No.|Test Functionality|Description|
|-----|------------------|-----------|
| 1 |[Logical address](#logical-address-discovery)|Facilitating the Discovery of logical addresses, Setting, getting, and removing the logical address of the device (for sink devices) |
| 2| [Physical Address](#physical-address)| Retrieving the physical address |
| 3| [CEC Synchronous Transmission](#cec-synchronous-transmission)| Transmitting CEC frames and reporting on their acknowledgement|
| 4| [CEC Receive functionality](#cec-receive-functionality)| Receiving CEC Information from other devices and passing it to the layer above through registered callback function |
| 5| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Managing CEC during Hotplug and HotUnplug events |

-----------

## Logical Address Discovery

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 1 |[Logical address](#logical-address-discovery)|Get the logical address of the `DUT` without actually adding the Logical Address and the API should return 0x0F as the default logical address.|HdmiCecGetLogicalAddress|Y|NA
|a| |Setup all valid logical addresses b/w 0x00 to 0x0F for the `DUT` and retrieve each to ensure proper functionality, using HAL APIs.|HdmiCecAddLogicalAddress, HdmiCecGetLogicalAddress, HdmiCecRemoveLogicalAddress| Y | NA |
|b| | Invoke the HAL API to delete the `DUT` logical address and verify that it is removed successfully.  |HdmiCecAddLogicalAddress, HdmiCecRemoveLogicalAddress, HdmiCecGetLogicalAddress| Y | NA|
|c| | After deleting the `DUT` logical address, try to send a broadcast CEC Command (as per 1.4b HDMI CEC spec) and confirm transmission is successful.|HdmiCecAddLogicalAddress, HdmiCecRemoveLogicalAddress, HdmiCecTx | Y|NA||

### Emulator Requirements

- Boot with control configuration with various configurations having a predefined set of nodes:
  - configuration to support the discovery of logical addresses. The caller provides the logical address, and HAL checks the availability of this address and feedback the same to the caller. 
  - Verify for the valid logical address and return the appropriate error code based on the logical address availability.  

## Physical Address

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 2| [Physical Address](#physical-address)| Verify the valid physical address allocated through the HAL function.|HdmiCecGetPhysicalAddress| Y  |NA||
| | | Verify the physical addresses allocated by connecting two sink devices through an HDMI switch.| HdmiCecGetPhysicalAddress| NA | Y  | Enable the television connected to `DUT` to declare its physical address first before `DUT`.|

### Emulator Requirements - Physical Address

- Boot control configuration to setup the CEC network nodes
- Scenario to have two sink devices on the network

### Control Plane Requirements - Physical Address

- The control plane will allow removing or adding a node to the network.
  - allowing add sink node before the `DUT` switched ON.    

## CEC Synchronous Transmission

|S.No.|Test Functionality|Description|HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|--------|--|--|--------------------------|
| 3| [CEC Transmission](#cec-transmission)| Verify the correct transmission of the supported CEC commands (as per 1.4b HDMI CEC spec) to the connected device and ensure it is acknowledged properly.| HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress | NA | Y  |Control plane to switch ON a CEC-supported device on the HDMI network so that it shall respond to the basic commands|
| | | Broadcast a supported CEC Command to all the devices connected to the network without any error |HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress| NA | Y  |Control plane to switch ON a CEC-supported device on the HDMI network to act on the broadcasted command|
| | | Transmit a CEC Command (as per 1.4b HDMI CEC spec) to put the connected device into standby mode and await the device's response. Monitoring the behaviour of the connected device accordingly.| HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress | NA | Y  | Control plane to monitor the behaviour of the connected devices.  |
| | | Transmit a HDMI CEC Command (as per 1.4b HDMI CEC spec) to get the CEC Version of a device that doesn't exist.| HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress | Y | NA  |  Control plane can unplug or switch off a previously existing CEC device |

### Emulator Requirements - CEC Transmission
- Boot configuration
  - Min case scenario multiple network nodes
  - Max case scenario multiple cec nodes

### Control Plane Requirements - CEC Transmission
- The control plane will allow adding a device that can respond to the CEC Frames sent by `DUT`

## CEC Receive functionality

|S.No.|Test Functionality|Description| HAL APIs|L2|L3|Control plane requirements|
|-----|------------------|-----------|---------|--|--|--------------------------|
| 5| [CEC Receive functionality](#cec-receive-functionality)| Transmit a CEC Command that expects a response (Eg. GetCECVersion) to a connected device and see the response is received correctly. Set the Rx Callback before sending the data. Validate the received CEC Version.|HdmiCecAddLogicalAddress, HdmiCecSetRxCallback, HdmiCecTx, HdmiCecRemoveLogicalAddress| NA |Y| Control plane to switch ON a CEC device that can respond to the Transmitted CEC Command|
| | | Transmit a CEC command from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (`DUT` TV here)| HdmiCecAddLogicalAddress, HdmiCecSetRxCallback, HdmiCecRemoveLogicalAddress | NA | Y  | Control panel to control the third-party devices to Transmit the required commands to  `DUT`|
| | | Transmit an OSD CEC command from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (`DUT` TV here). Make the OSD String to max length| HdmiCecAddLogicalAddress, HdmiCecSetRxCallback, HdmiCecRemoveLogicalAddress | NA | Y  | Control panel to control the third-party devices to Transmit the required commands to  `DUT`|
| | | Transmit an OSD CEC command from the connected devices continuously for 30 seconds changing the patterns in the payload and considering the Acknowledgement and responses are received correctly from the host device (`DUT` TV here). Make the OSD String to max length| HdmiCecAddLogicalAddress, HdmiCecSetRxCallback, HdmiCecRemoveLogicalAddress | NA | Y  | Control panel to control the third-party devices to Transmit the required commands to  `DUT`. Also, Control plane to detect the OSD Display on the Sink device to validate|
| | | Set the Logical address to 0 on `DUT` and make sure that it doesn't receive the messages sent to devices with different logical address.| HdmiCecAddLogicalAddress, HdmiCecSetRxCallback, HdmiCecRemoveLogicalAddress | NA | Y  | Control Plane to initiate a command to send CEC frames from CEC adaptor with a different logical address other than zero|

### Emulator Requirements - CEC Receive functionality
1. Emulate the Tx and Rx HAL functionalities with the required responses.

### Control Plane Requirements - CEC Transmission
- The control plane will allow adding a device that can respond to the CEC Frames sent by `DUT`
- The control plane to initiate CEC Transmissions from the connected devices as expected by the `DUT`

## CEC HotPlug Functionality

|S.No.|Test Functionality|Description| HAL APIs |L2|L3|Control plane requirements|
|-----|------------------|-----------|----------|--|--|--------------------------|
| 6| [CEC HotPlug Functionality](#cec-hotplug-functionality)| Generate a Hotplug event by disconnecting the device connected to the HDMI port of the Sink Platform. Validating whether the CEC Transmission (use Polling command) works when the HDMI port is disconnected should result in ACK not being received while the TX still works as expected. | HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress | NA | Y  | Control Panel to control the Hotplug activities |
| | | Check the behaviour when a device has been remove from the network which is not directly connected to the TV device.  Send a CEC Tx command with acknowledgement using HAL Interface and check the behaviour. The Tx command should succeed, but the message should not be Acknowledged.| HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress | NA | Y  | Control Panel to control the external devices connected.|

### Emulator Requirements - CEC HotPlug Functionality

### Control Plane Requirements - CEC HotPlug Functionality
1. Control plane to initiate the HotPlug activity by commanding an IP power switch to the OFF State to which the Node device is connected.

-----------
-----------

## Hardware Verification Testing Requirements

|S.No.|Test Functionality|Description| HAL APIs |L2|L3|Control plane requirements|
|-----|------------------|-----------|----------|--|--|--------------------------|
| 7| Introduce fault in the CEC Bus | Absorve the behaviour when the CEC line is pulled high during the CEC Transmission using a CEC Adaptor that provision to keep the CEC line pulled high| HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress | NA | Y  |CEC Adaptor used shall have a provision to introduce the fault. The control plane should be able to command to pull the CEC line high, else it should follow a manual process|
| 8| Overloading the CEC bus. | Overload the CEC bus with too many messages  (by connecting more devices in the network) and observe the behaviour| HdmiCecAddLogicalAddress, HdmiCecTx, HdmiCecRemoveLogicalAddress | NA | Y  |Control plane to initiate the CEC Transmission through all the connected devices continuously with a command that expects the response as well to overload the CEC Network.  |

### Emulator Requirements
1. Emulator to support the HDMI_CEC_IO_SENT_FAILED during the above scenarios
   
### Control Plane Requirements
1. Control Plane to control the external devices to pull the CEC line high.
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
