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

class hdmiCEC_test01_TransmitSingleStandbyCommandandValidateAck(hdmiCECHelperClass):
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
        self.testName  = "test01_TransmitSingleStanbyCommandandValidateAck"
        self.qcID = '1'

        self.testCECCommands = os.path.join(dir_path, "hdmiCECTestCommands.yml")
        hdmicec = ConfigRead(self.testCECCommands, "hdmiCEC")
        self.cecCommands = hdmicec.fields.get("test01_TransmitSingleStandbyCommandandValidateAck")
        for command in self.cecCommands:
            cec = command.get("command")
            payload = command.get("payload")
            print(cec, payload)
        super().__init__(self.testName, self.qcID, log)

    #TODO: Current version supports only manual verification.
    def testVerifyStanbyStatus(self, ack, manual=False):
        """
        Verifies the CEC Transmit Status through the result and also through the manual input.

        For manual verification, it prompts the user to confirm if CEC data is received by the destination
        device and the action has been performed.

        Args:
            ack(str): Ack result as printed on the test. Need to check that proper ACK is received.
            (bool, optional): Flag to indicate if manual verification should be used.
                                     Defaults to False for automation, True for manual.

        Returns:
            bool: True if ACK verification succeeds, False otherwise.Also True if Manual test is Y and Flase if N
        """
        if manual == True:
            return self.testUserResponse.getUserYN(f"Is Stanby Command honored? (Y/N):")
        else :
            #TODO: Add automation verification methods
            return False

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

        # Add the logical Address. For now 0 only.
        self.testhdmiCEC.addLogicalAddress(0)

        self.cecDevices = [1,2,3]#self.cecAdapter.listDevices()

        for device in self.cecDevices:
            logicalAddress = 1#int(device["address"].split('.')[0])

            # To bypass sending the message to TV
            if logicalAddress == 0 or logicalAddress == 14:
                continue

          # self.log.stepStart(f'HdmiCecTx(IN: handle: [0x%0X], IN: length: [%d], result: [%d], status:[%d]')

            for command in self.cecCommands:
                cec = command.get("command")
                payload = command.get("payload")
                #result = self.cecAdapter.startMonitoring()
                #print(result)
                # Transmit Standby command to a specific destination address
                self.testhdmiCEC.cecTransmitCmd(0, logicalAddress,cec,payload)
                #self.cecAdapter.stopMonitoring()

                #Verify the test result
                result = self.testVerifyStanbyStatus(logicalAddress, True)

            self.log.stepResult()

            # Remove the Logical Address
            self.testhdmiCEC.removeLogicalAddress(0)

        # Terminate dsAudio Module
        self.testdsAudio.terminate()

        return result

if __name__ == '__main__':
    summerLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summery"
    summeryLog = logModule(summerLogName, level=logModule.INFO)
    test = hdmiCEC_test01_TransmitSingleStandbyCommandandValidateAck(summeryLog)
    test.run(False)
