# HDMI CEC Test Document
 
## Version History
 
| Date(DD/MM/YY) | Comment       | Version |
| -------------- | ------------- | ------- |
| 13/02/24       | First Release | 1.0.0   |
 
## Table of Contents
 
- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Description](#description)
- [Component Runtime Execution Requirements](#component-runtime-execution-requirements)
  - [Initialization and Startup]
 
## Acronyms, Terms and Abbreviations
 
- `CEC`   - Consumer Electronics Control
- `HAL`   - Hardware Abstraction layer
- `HDMI`  - High Definition Multimedia Interface
 
 
## Overview
 
Consumer Electronics Control (CEC) is a single-wire bidirectional bus within an HDMI system, facilitating communication among interconnected products. HDMI-CEC establishes a protocol enabling high-level control functions between audiovisual devices linked via an HDMI network, facilitating communication and control among them. Communication can occur in either Direct messaging mode or Broadcast mode. Below is a diagram illustrating a basic network setup:

 

 
## HDMI-CEC RDK HAL Functionality
The HAL layers within RDK serve as a bridge between the underlying low-level SoC drivers and the higher-level RDK layers that utilize the functionality offered by these HAL functions. Specifically concerning the CEC Module, the HAL layers facilitate the following functionalities:
 
1. Discovery of logical addresses.
2. Setting, getting, and removing the logical address of the device (for sink devices).
3. Retrieving the physical address.
4. Transmitting CEC frames and acknowledging them.
5. Asynchronously transmitting CEC frames without requiring acknowledgment.
6. Receiving CEC information from other devices and communicating it to the above layers through registered callback functions.
7. Managing CEC during Hotplug and HotUnplug events.

## Test scenarios that shall be evaluated.

Note: The HAL CEC layer facilitates the transmission and reception of CEC information on the CEC bus. It does not handle any specific opcode commands, nor does it validate supported HAL CEC opcodes for sending or receiving. It is the responsibility of the caller to manage the opcodes. The current test cases will verify responses from connected devices for a subset of opcodes as part of the testing process.

| S.No. | Test Functionality             | Description                                                                                                                                                                                                                                             | L2 | L3 | L4 | Control plane requirements                                                                                                                 | HW Test |
|-------|--------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----|----|----|-------------------------------------------------------------------------------------------------------------------------------------------|---------|
| 1     | Logical address Discovery      | Establish my logical address (through HAL APIs) as valid, then retrieve it to verify proper functionality.                                                                                                                                              | Y  | NA | NA |                                                                                                                                           | No      |
|       |                                | If the logical address is set to anything other than 0 (Allocated TV address) or 14 (Wild card address), it should result in a failure of a TV device                                                                                                | Y  | NA | NA |                                                                                                                                           | No      |
|       |                                | Invoke the HAL API to delete my logical address and verify that it is removed successfully.                                                                                                                                                             | Y  | NA | NA |                                                                                                                                           | No      |
|       |                                | The attempt to obtain my logical address should fail if more than two TV devices are connected.                                                                                                                                                         | NA | Y  | NA | Enable two televisions that can acquire the logical address 0 and 14 before attempting my Tele under test to acquire the logical address. | No      |
|       |                                | After deleting my logical address, try to send a broadcast command. Should fail to send during HAL Transmission call.                                                                                                                                    | Y  | NA | NA |                                                                                                                                           | No      |
| 2     | Physical Address               | Verify the valid physical address allocated through the HAL function.                                                                                                                                                                                   | Y  | NA | NA |                                                                                                                                           | No      |
|       |                                | Verify the physical addresses allocated by connecting two sink devices through an HDMI switch.                                                                                                                                                          | NA | Y  | NA | Enable the television connected to my Tele first so that it can declare its physical address first.                                       | No      |
| 3     | CEC Transmission               | Verify the correct transmission of all the supported CEC commands (as per 1.4b HDMI CEC spec) to the connected device and ensure it is acknowledged properly.                                                                                           | NA | Y  | NA |                                                                                                                                           | No      |
|       |                                | Broadcast all the supported CEC Commands ((as per 1.4b HDMI CEC spec)) to all the devices connected on the network and receive the response.                                                                                                             | NA | Y  | NA |                                                                                                                                           | No      |
|       |                                | Transmit all the  CEC Command (as per 1.4b HDMI CEC spec) to put the connected device into standby mode and await the device's response. Monitor the behavior of the connected device accordingly.                                                      | NA | Y  | NA | Control panel to monitor the behaviour of the connected devices.                                                                          | No      |
| 4     | CEC Async Transmission         | Verify the correct transmission of all the CEC commands (as per 1.4b HDMI CEC spec) using the Async Transmission support of the CEC HAL interface.                                                                                                      | NA | Y  | NA |                                                                                                                                           | No      |
|       |                                | Verify the correct behavior of the connected device when the CEC standby command is sent to it.                                                                                                                                                         | NA | Y  | NA | Control panel to monitor the behaviour of the connected devices.                                                                          | No      |
|       |                                | Broadcast all the CEC Commands ((as per 1.4b HDMI CEC spec)) to all the devices connected in the network and check the behaviour and response.                                                                                                          | NA | Y  | NA | Control panel to monitor the behaviour of the connected devices.                                                                          | No      |
| 5     | CEC Receive functionality.     | Transmit a CEC Command that expects a response (Eg. GetCECVersion) to a connected device and see the response is received correctly.                                                                                                                    | NA | Y  | NA |                                                                                                                                           | No      |
|       |                                | Transmit all the CEC Broadcast Command (as per 1.4b HDMI CEC spec) which expects a response from all the connected devices like <GetPhysicalAddress>                                                                                                     | NA | Y  | NA |                                                                                                                                           | No      |
|       |                                | Transmit Different CEC commands from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (my TV here)                                                                                       | NA | Y  | NA | Control panel to control the third-party devices to Transmit the required commands to my Device (Device under test)                        | No      |
| 6     | CEC HotPlug Functionality.     | Generate an Hotplug event by disconnecting the device connected to the HDMI port of the Sink Platform. Check whether the CEC Transmission works when HDMI port is disconnected.                                                                         | NA | Y  | NA | Control Panel to control the Hotplug activities                                                                                           | No      |
|       |                                | Verify  the behaviour when a device has been removed from the network which is not directly connected to the TV device.  Send a CEC Async Tx command using HAL Interface and check the behaviour. The Tx Command should not fail.                       | NA | Y  | NA | Control Panel to control the devices connected on the CEC Network.                                                                        | No      |
|       |                                | Check the behaviour when a device has been remove from the network which is not directly connected to the TV device.  Send a CEC Tx command with acknowledgement using HAL Interface and check the behaviour. The Tx command should fail in this state. | NA | Y  | NA | Control Panel to control the external devices connected.                                                                                  | No      |
| 7     | Introduce fault in the CEC Bus | Absorve the behaviour when the CEC line is pulled high during the CEC Transmission.                                                                                                                                                                      | NA | Y  | NA |                                                                                                                                           | Yes     |
| 8     | Overloading the CEC bus.       | Overload the CEC bus with too many messages  (by connecting more devices in the network) and observe the behaviour                                                                                                                                      | NA | Y  | NA |                                                                                                                                           | Yes     |



## Test Configuration for Sink Devices.

The following information shall be helpful for further running the Automation Rack Test for this specific module and further the configuration will help for the Design and Development of Virtual Device.

Configurations:

1. DeviceType : 1 - Source Device, 2 - Sink Device, 
2. Platform Model: ["< Make >" ,  "< Model of the platform >"]
3. PortNo: 3 or 4
4. HDMI Node : [1, 2, 3].
5. Ports Supporting CEC: [ 1,  3 ]

