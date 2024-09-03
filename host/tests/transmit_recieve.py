#!/usr/bin/env python3
#** *****************************************************************************
# *
# * If not stated otherwise in this file or this component's LICENSE file the
# * following copyright and licenses apply:
# *
# * Copyright 2023 RDK Management
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

# Add the framework path to system
dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(dir_path)

from tests.test_helpers import test_helper

class transmitReceive(test_helper):
 
    def __init__(self):
        print('unit2 - Initializing test controller')
        super().__init__("framework_test", "1")

    def testFunction(self):
        """This function will cause the HDMI Source PS3 to become activate, and validate the data send is correct"

        Returns:
            bool: true
        """
        self.ut_core_API("hdmiCec","Init") # -> API call
        # Network Configuration --hdmiSetup="https://github.com/rdkcentral/rdk-halif-test-hdmi_cec/blob/develop/assets/validation_profiles/configPS3.yaml" --controlMsg="https://github.com/rdkcentral/rdk-halif-test-hdmi_cec/blob/develop/assets/validation_profiles/activateSource.yaml"
        self.ut_controlSendMsg( args.networkSetup )
        self.ut_core_API("hdmiCec","Open") # -> API call
        # Message to send and validate
        self.ut_controlSendMsg( args.controlMsg)
        """
        SerialOutputCallback: data: [0x20,0x00]
        ActiveSource:
            input:
                initiator: SONY PS3
                destination: TVPanel
            result:
                initiator: 4
                destination: 0
                opcode: 0x82   #Opcode as defined in HDMI CEC Specification
                parameters:
                    size: 2
                    data: [0x20,0x00] #Physical address 2.0.0.0
        """
        ## Callback occurs, needs decoding
        callbackMessage = self.waitForCECCallback()
        self.checkResult( callbackMessage, messageUrl=args.controlMsg ) # Decode the callback, and break it into it's parts
        self.ut_core("hdmiCec","addLogicalAddress", "0" ) # -> API call
        # Configure the network for logical address for sink device
        result = self.ut_core_API("hdmi","getLogicalAddress", "xbox")
        self.ut_core_API("hdmiCec","DeInit") #  -> API Call
        return True

        self.ut_controlSendMsg( messageUrl="https://github.com/rdkcentral/rdk-halif-test-hdmi_cec/blob/main/assets/control_messages/activesource.yaml" )


if __name__ == '__main__':

    test = basicTest()
    test.run()
  

# Make the TV choose an active Source from it's HDMI list


#-> test -> ut_controlMessage.py -> hdmiMessageDecoder.py -> Config/Control -> Rack -> Raft_HDMIModule() -> PulseHDMI -> Module()
#                                                                           vDevice -> sendToPort()
#                                   tvSettingsDecoder -> Config/Control -> Rack -> tvpanelsettings -> Module()
#                                   deepSleepManager -> Config/Control -> Rack -> Module()
#                                                                         vDevice -> Module()
#                                   powerManager -> Config/Control -> Rack -> Module()
#                                   deviceSettings -> Config/Control -> Rack -> Module()
#                                   RMFAudio -> Config/Control -> Rack -> Module()
#                                   ClosedCaptions -> Config/Control -> Rack -> Module()

#  result                                                    -> APIMessage -> ut_control.py -> interfaceHDMI( "setLogicAddress.yaml" )

