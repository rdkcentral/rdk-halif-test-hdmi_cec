#!/usr/bin/env python3
#** *****************************************************************************
# *
# * If not stated otherwise in this file or this component's LICENSE file the
# * following copyright and licenses apply:
# *
# * Copyright 2024 RDK Management
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *
# http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
# *
#* ******************************************************************************

import os
import sys
import time

# Append the current and parent directory paths to sys.path for module imports
dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(dir_path))
sys.path.append(os.path.join(dir_path, "../"))

from hdmiCECHelperClass import hdmiCECHelperClass
from raft.framework.core.logModule import logModule
from raft.framework.plugins.ut_raft.configRead import ConfigRead

class hdmiCEC_test02_ReceiveCECCommands(hdmiCECHelperClass):
    """
    A class for testing HDMI-CEC functionality by sending CEC commands
    and verifying the received callback data.
    """
    def __init__(self, log:logModule=None):
        """
        Initializes the test class with test-specific configurations.

        Args:
            log (logModule): Logging module instance to record test execution.
        """
        # Class variables
        self.testName  = "test02_ReceiveCECCommands"
        self.qcID = '2'
        self.broadcastAddress = 'f'

        # Initialize the parent class
        super().__init__(self.testName, self.qcID, log)

    def testVerifyReceivedData(self, callbackData:dict, initiatorLogicalAddress:int, destinationLogicalAddress:int, opCode:str, payload:list):
        """
        Verifies the received callback data.

        Args:
            callbackData(dict): callback data received
            initiatorLogicalAddress(int): Initiator logical address
            destinationLogicalAddress(int): Destination logical address
            opCode (str): opcode sent
            payload (str): Payload Sent
        Returns:
            bool: Final result of the test.
        """
        result = False
        for received in callbackData["Received"]:
            # Check if initiator, destination, and opcode match the expected values
            if (received["Initiator"] == initiatorLogicalAddress and
                received["Destination"] == destinationLogicalAddress and
                received["Opcode"] == opCode):
                result = True
                # Verify payload if provided
                if payload:
                    for rec, sent in zip(received["Data"][2:], payload):
                        if rec != sent:
                            result = False
                            break
        return result

    def testFunction(self):
        """
        Main test function to send CEC commands and validate received callback data.

        Steps:
        1. Initialize HDMI-CEC module.
        2. Add and retrieve logical address of the device.
        3. Send commands to other devices and validate responses using callback data.
        4. Validate response commands if specified.
        5. Clean up by removing logical addresses and terminating the module.

        Returns:
            bool: Final result of the test execution.
        """

        # Initialize the hdmiCEC module
        self.testhdmiCEC.initialise()

        # Add the logical Address.
        self.testhdmiCEC.addLogicalAddress(self.testLogicalAddress)

        # Get the logical Address.
        deviceLogicalAddress = self.testhdmiCEC.getLogicalAddress()

        # Get Physical Address
        devicePhysicalAddress = self.testhdmiCEC.getPhysicalAddress()

        # Ensure the logical address is retrieved successfully
        if deviceLogicalAddress is None:
            self.log.error("Failed to get the device logical address")
            return False

        # List all connected CEC devices
        self.cecDevices = self.hdmiCECController.listDevices()

        finalResult = True

        for device in self.cecDevices:
            logicalAddress = device["logical address"]

            # Skip sending messages to TV
            if logicalAddress == deviceLogicalAddress:
                continue

            cecAdapterLogicalAddress = logicalAddress

            for command in self.cecCommands:
                result = False
                cecOpcode = command.get("command")
                payload = command.get("payload")
                response = command.get("response")
                type = command.get("type")

                # Determine the destination logical address
                destinationLogicalAddress = deviceLogicalAddress
                if type == "Broadcast":
                    destinationLogicalAddress = self.broadcastAddress

                self.log.stepStart(f'Send Test: Source: {cecAdapterLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')

                self.hdmiCECController.sendMessage(cecAdapterLogicalAddress, destinationLogicalAddress, cecOpcode, payload)

                # Read the callback details and verify the received data
                callbackData = self.testhdmiCEC.readCallbackDetails()
                result = self.testVerifyReceivedData(callbackData, cecAdapterLogicalAddress, destinationLogicalAddress, cecOpcode, payload)

                finalResult &= result

                self.log.stepResult(result, f'Send Test: Source: {cecAdapterLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')

                # If a response is expected, validate the response
                if response:
                    destinationLogicalAddress = cecAdapterLogicalAddress
                    if response.get("type") == "Broadcast":
                        destinationLogicalAddress = self.broadcastAddress

                    cecOpcode = response.get("command")
                    payload = response.get("payload")
                    if response.get("update_payload"):
                        payload[0] = devicePhysicalAddress[0]
                        payload[1] = devicePhysicalAddress[1]

                    self.log.stepStart(f'Response Test: Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')
                    result = self.hdmiCECController.checkMessageReceived(deviceLogicalAddress, destinationLogicalAddress, cecOpcode, payload=payload)
                    self.log.stepResult(result, f'Response Test: Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')

                    finalResult &= result

        # Remove the Logical Address
        self.testhdmiCEC.removeLogicalAddress()

        # Terminate the hdmiCEC module
        self.testhdmiCEC.terminate()

        return finalResult

if __name__ == '__main__':
    # Configure the summary log file
    summerLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summery"
    summeryLog = logModule(summerLogName, level=logModule.INFO)

    # Create an instance of the test class and execute the test
    test = hdmiCEC_test02_ReceiveCECCommands(summeryLog)
    test.run(False)
