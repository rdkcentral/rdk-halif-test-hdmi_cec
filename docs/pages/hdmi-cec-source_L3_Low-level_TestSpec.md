# HDMICEC Source L3 Low Level Test Specification and Procedure Documentation

## Table of Contents

- [Overview](#overview)
- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [References](#references)
- [Level 3 Test cases High Level Overview](#level-3-test-cases-high-level-overview)
- [Level 3 Python Test Cases High Level Overview](#level-3-python-test-cases-high-level-overview)

## Overview

This document describes the L3 Low Level Test Specification and Procedure Documentation for the Device Settings Video Port module.

### Acronyms, Terms and Abbreviations

- `HAL` \- Hardware Abstraction Layer, may include some common components
- `UT`  \- Unit Test(s)
- `OEM` \- Original Equipment Manufacture
- `SoC` \- System on a Chip
- `HDMI`\- High-Definition Multimedia Interface
- `HDCP`\- High-bandwidth Digital Content Protection
- `HDR` \- High Dynamic Range
- `HLG` \- Hybrid Log-Gamma
- `SDR` \- Standard Dynamic Range
- `Y`   \- yes supported
- `NA`  \- Not Supported

### References

- The interface of the test is available here: [Hdmicec HAL header](https://github.com/rdkcentral/rdk-halif-hdmi_cec/blob/main/include/hdmi_cec_driver.h)

- The Hdmicec Hal Spec document: [Hdmicec HAL Spec](https://github.com/rdkcentral/rdk-halif-hdmi_cec/blob/main/docs/pages/hdmi-cec_halSpec.md)

## Level 3 Test Cases High Level Overview

|#|Test-case|Description|HAL APIs|
|-|---------|-----------|--------|
|1|Validate logical address|Connect port, and validate that the connected logical address is 4, 8, or 11.| `HdmiCecOpen()`,`HdmiCecGetLogicalAddress()` |
|2|Connect too many devices|Connect five other source devices to sink device using raft, before connecting the testing device. should return HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE as there is not a valid logical address.| `HdmiCecOpen()` |
|3|Rapid connection|Rapidly connect and disconnect HDMI connection in 100ms connection and 100ms disconnection cycle.| `HdmiCecOpen()` |
|4|Connect Physical Address | Enable a sink device connected to the DUT first to get the valid physical address allocated through the HAL function. The physical address should be 1.0.0.0 | `HdmiCecGetPhysicalAddress()` |
|5|Connect Physical Address with switch| EVerify the physical addresses allocated by connecting a source and sink device through an HDMI switch. The physical address should 1.1.0.0 | `HdmiCecGetPhysicalAddress()` |
|6|Reconnect physical address| Connect a sink device to the source device, get the physical address. Disconnect the sink device and attempt to get the physical address again | `HdmiCecGetPhysicalAddress()` |
|7|Transmit to invalid address|Transmit a CEC Command (as per 1.4b HDMI CEC spec) to get the CEC Version for a logical address that doesn't exist after the connected device is disconnected. Result should return HDMI_CEC_IO_SENT_BUT_NOT_ACKD.| `HdmiCecTx()` |
|8|Transmit after disconnection|Transmit a CEC Command (as per 1.4b HDMI CEC spec) to get the CEC Version for supported CEC commands (as per 1.4b HDMI CEC spec) after the connected device is disconnected. Result should return HDMI_CEC_IO_SENT_BUT_NOT_ACKD.| `HdmiCecTx()` |
|9|Transmit while connected|Verify the correct transmission of the supported CEC commands (as per 1.4b HDMI CEC spec) to the connected device and ensure it is acknowledged properly. The result should return HDMI_CEC_IO_SENT_AND_ACKD.| `HdmiCecTx()` |
|10|Transmit to all|Broadcast a supported CEC Command to all the devices connected to the network without any error. The result should return HDMI_CEC_IO_SENT_AND_ACKD| `HdmiCecTx()` |
|11|Transmit to standby.|Transmit a CEC Command (as per 1.4b HDMI CEC spec) to put the connected device into standby mode and await the device's response. Monitoring the behavior of the connected device accordingly. The result should return HDMI_CEC_IO_SENT_AND_ACKD.| `HdmiCecTx()` |
|12|Send command and receive response|Transmit a CEC Command that expects a response (Eg. GetCECVersion) to a connected device and see the response is received correctly. Set the Rx Callback before sending the data. Validate the received CEC Version.| `HdmiCecSetRxCallback()` `HdmiCecTx()` |
|13|Receive command and send response|Transmit a CEC command from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (DUT TV here)| `HdmiCecSetRxCallback()`|
|14|Transmit OSD Command|Transmit an OSD CEC command from the connected devices and consider the Acknowledgement and responses are received correctly from the host device (DUT TV here). Make the OSD String to max length| `HdmiCecSetRxCallback()`|
|15|Transmit OSD Command continuously|Transmit an OSD CEC command from the connected devices continuously for 30 seconds changing the patterns in the payload and considering the Acknowledgement and responses are received correctly from the host device (DUT TV here). Make the OSD String to max length| `HdmiCecSetRxCallback()`|
|16|Transmit logical 0|Set the Logical address to 0 on DUT and make sure that it doesn't receive the messages sent to devices with different logical address.| `HdmiCecSetRxCallback()`|
|17|Hotplug event|Generate a Hotplug event by disconnecting the device connected to the HDMI port of the Source Platform. Validating whether the CEC Transmission (use Polling command) works when the HDMI port is disconnected should result in ACK not being received while the TX still works as expected.| `HdmiCecTx()`|
|18|Hotplug event not-acked|Check the behaviour when a device has been remove from the network which is not directly connected to the TV device. Send a CEC Tx command with acknowledgment using HAL Interface and check the behaviour. The Tx command should succeed, but the message should not be Acknowledged.| `HdmiCecTx()`|
|19|Hardware bus failure|Observe the behaviour when the CEC line is pulled high during the CEC Transmission using a CEC Adaptor that provision to keep the CEC line pulled high |`HdmiCecTx()`|
|20|Hardware bus overload|Overload the CEC bus with too many messages (by connecting more devices in the network) and observe the behaviour. |`HdmiCecTx()`|

## Level 3 Python Test Cases High Level Overview
