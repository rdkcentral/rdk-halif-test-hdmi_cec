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

dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(dir_path))
sys.path.append(os.path.join(dir_path, "../"))

from hdmiCECHelperClass import hdmiCECHelperClass
from raft.framework.core.logModule import logModule
from raft.framework.plugins.ut_raft.configRead import ConfigRead

class hdmiCEC_test02_ReceiveCECCommands(hdmiCECHelperClass):
    """
    Test class to enable, disable, and verify the status of audio ports on a device.

    This class uses the `dsAudioClass` to interact with the device's audio ports,
    downloading necessary test assets, playing audio streams, enabling and disabling
    audio ports, and performing verification of audio output.
    """
    def __init__(self, log:logModule=None):
        """
        Initializes the test class with test name, setup configuration, and sessions for the device.

        Args:
            None
        """
        # Class variables
        self.testName  = "test02_ReceiveCECCommands"
        self.qcID = '2'
        self.tvLogicalAddress = '0'
        self.broadcastAddress = 'f'

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
            if (received["Initiator"] == initiatorLogicalAddress and
                received["Destination"] == destinationLogicalAddress and
                received["Opcode"] == opCode):
                result = True
                if payload:
                    for rec, sent in zip(received["Data"][2:], payload):
                        if rec != sent:
                            result = False
                            break
        return result

    def testFunction(self):
        """
        The main test function that Transmits the Stanby Command and checks the ACK and validates it.

        This function:
        - Send a standby command to a device that is connected on the CEC network and get the ack.
        - User to confirm whether the targetted device had recieved this command or not.

        Returns:
            bool: Final result of the test.
        """

        # Initialize the hdmiCEC module
        self.testhdmiCEC.initialise()

        # Add the logical Address.
        self.testhdmiCEC.addLogicalAddress(self.tvLogicalAddress)

        # Get the logical Address.
        deviceLogicalAddress = self.testhdmiCEC.getLogicalAddress()

        if deviceLogicalAddress is None:
            self.log.error("Failed to get the device logical address")
            return False

        self.cecAdaptor = self.hdmiCECController.adaptorDetails

        if self.cecAdaptor is None:
            return False

        cecAdapterLogicalAddress = self.cecAdaptor["logical address"]

        finalResult = True
        for command in self.cecCommands:
            result = False
            cecOpcode = command.get("command")
            payload = command.get("payload")
            response = command.get("response")
            type = command.get("type")

            destinationLogicalAddress = deviceLogicalAddress
            if type == "Broadcast":
                destinationLogicalAddress = self.broadcastAddress

            self.log.stepStart(f'Send Test: {cecAdapterLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')

            self.hdmiCECController.sendMessage(cecAdapterLogicalAddress, destinationLogicalAddress, cecOpcode, payload)

            callbackData = self.testhdmiCEC.readCallbackDetails()

            result = self.testVerifyReceivedData(callbackData, cecAdapterLogicalAddress, destinationLogicalAddress, cecOpcode, payload)

            finalResult &= result

            self.log.stepResult(result, f'Send Test: {cecAdapterLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')

            if response:
                destinationLogicalAddress = cecAdapterLogicalAddress
                if response.get("type") == "Broadcast":
                    destinationLogicalAddress = self.broadcastAddress

                cecOpcode = response.get("command")
                payload = response.get("payload")

                self.log.stepStart(f'Response Test: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')
                result = self.hdmiCECController.checkTransmitStatus(deviceLogicalAddress, destinationLogicalAddress, cecOpcode, payload)
                self.log.stepResult(result, f'Response Test: {cecAdapterLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cecOpcode} Payload: {payload}')

                finalResult &= result

        # Remove the Logical Address
        self.testhdmiCEC.removeLogicalAddress()

        # Terminate dsAudio Module
        self.testhdmiCEC.terminate()

        return finalResult

if __name__ == '__main__':
    summerLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summery"
    summeryLog = logModule(summerLogName, level=logModule.INFO)
    test = hdmiCEC_test02_ReceiveCECCommands(summeryLog)
    test.run(False)
