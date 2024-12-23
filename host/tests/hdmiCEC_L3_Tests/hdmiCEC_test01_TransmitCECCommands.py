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

# Importing helper classes and modules for HDMI-CEC testing and logging
from hdmiCECHelperClass import hdmiCECHelperClass
from raft.framework.core.logModule import logModule

class hdmiCEC_test01_TransmitCECCommands(hdmiCECHelperClass):
    """
    A class for testing HDMI-CEC functionality by transmitting and verifying CEC commands.
    """
    def __init__(self, log:logModule=None):
        """
        Initializes the test class with test-specific configurations.

        Args:
            log (logModule): Logging module instance to record test execution.
        """
        # Class variables
        self.testName  = "test01_TransmitCECCommands"
        self.qcID = '1'
        self.broadcastAddress = 'f'

        # Initialize the parent class
        super().__init__(self.testName, self.qcID, log)

    def testVerifyTxResults(self, txResult:str, rxResult:bool):
        """
        Verifies the tx and rx results.

        Args:
            txResult(str): tx result
            rxResult(bool): rx result
        Returns:
            bool: Final result of the test.
        """

        return rxResult

    def testFunction(self):
        """
        Main test function for transmitting HDMI-CEC commands and validating responses.

        Steps:
        1. Initialize HDMI-CEC module.
        2. Add a logical address to the test device.
        3. Get the logical address of the device.
        4. List all connected CEC devices.
        5. Iterate through devices and send commands to appropriate logical addresses.
        6. Validate transmission and record results.
        7. Clean up by removing logical addresses and terminating the HDMI-CEC module.

        Returns:
            bool: Final result of the test execution.
        """

        # Initialize the hdmiCEC module
        self.testhdmiCEC.initialise()

        # Add the logical Address.
        self.testhdmiCEC.addLogicalAddress(self.testLogicalAddress)

        # Get the logical Address.
        deviceLogicalAddress = self.testhdmiCEC.getLogicalAddress()

        # List all connected CEC devices
        self.cecDevices = self.hdmiCECController.listDevices()

        # Final test result
        finalResult = True

        for device in self.cecDevices:
            logicalAddress = device["logical address"]

            # Skip sending messages to TV
            if logicalAddress == deviceLogicalAddress:
                continue

            for command in self.cecCommands:
                cec = command.get("command")
                payload = command.get("payload")
                type = command.get("type")

                # Determine the destination logical address
                destinationLogicalAddress = logicalAddress
                if type == "Broadcast":
                    destinationLogicalAddress = self.broadcastAddress

                self.log.stepStart(f'HdmiCecTx Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cec} Payload: {payload}')

                # Transmit the CEC command
                txResults = self.testhdmiCEC.cecTransmitCmd(destinationLogicalAddress, cec, payload)

                self.log.stepResult(txResults, f'HdmiCecTx Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cec} Payload: {payload}')

                finalResult &= txResults

                time.sleep(2)

                self.log.stepStart(f'HdmiCecTx Receive Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cec} Payload: {payload}')
                # Validate the transmission
                rxResult = self.hdmiCECController.checkMessageReceived(deviceLogicalAddress, destinationLogicalAddress, cec, payload=payload)

                self.log.stepResult(rxResult, f'HdmiCecTx Receive Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cec} Payload: {payload}')

                finalResult &= rxResult

        # Remove the Logical Address
        self.testhdmiCEC.removeLogicalAddress()

        # Terminate the hdmiCEC Module
        self.testhdmiCEC.terminate()

        return finalResult

if __name__ == '__main__':
    # Configure the summary log file
    summerLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summery"
    summeryLog = logModule(summerLogName, level=logModule.INFO)

    # Create an instance of the test class and execute the test
    test = hdmiCEC_test01_TransmitCECCommands(summeryLog)
    test.run(False)
