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

class hdmiCEC_test01_TransmitCECCommands(hdmiCECHelperClass):
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
        self.testName  = "test01_TransmitCECCommands"
        self.qcID = '1'
        self.tvLogicalAddress = '0'
        self.broadcastAddress = 'f'

        super().__init__(self.testName, self.qcID, log)

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

        self.cecDevices = self.hdmiCECController.listDevices()

        finalResult = True
        for device in self.cecDevices:
            logicalAddress = device["logical address"]

            # To bypass sending the message to TV
            if logicalAddress == '0' or logicalAddress == 'f':
                continue

            for command in self.cecCommands:
                result = False
                cec = command.get("command")
                payload = command.get("payload")
                type = command.get("type")

                destinationLogicalAddress = logicalAddress
                if type == "Broadcast":
                    destinationLogicalAddress = self.broadcastAddress

                # Transmit Standby command to a specific destination address
                self.testhdmiCEC.cecTransmitCmd(destinationLogicalAddress, cec, payload)

                self.log.stepStart(f'HdmiCecTx Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cec} Payload: {payload}')

                result = self.hdmiCECController.checkTransmitStatus(deviceLogicalAddress, destinationLogicalAddress, cec, payload)

                self.log.stepResult(result, f'HdmiCecTx Source: {deviceLogicalAddress} Destination: {destinationLogicalAddress} CEC OPCode: {cec} Payload: {payload}')

                finalResult &= result

        # Remove the Logical Address
        self.testhdmiCEC.removeLogicalAddress()

        # Terminate dsAudio Module
        self.testhdmiCEC.terminate()

        return finalResult

if __name__ == '__main__':
    summerLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summery"
    summeryLog = logModule(summerLogName, level=logModule.INFO)
    test = hdmiCEC_test01_TransmitCECCommands(summeryLog)
    test.run(False)
