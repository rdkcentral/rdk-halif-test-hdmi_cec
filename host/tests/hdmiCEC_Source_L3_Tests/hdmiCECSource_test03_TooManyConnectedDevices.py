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
        self.tvLogicalAddress = '0'
        self.broadcastAddress = 'f'

        # Initialize the parent class
        super().__init__(self.testName, self.qcID, log)

    def testFunction(self):
        """
        Main test function to send CEC commands and validate received callback data.
        Steps:
        1. Initialize HDMI-CEC module.
        2. Initialization should fail due to no available logical address
        Returns:
            bool: Final result of the test execution.
        """

        self.log.stepStart(f"Test {self.testName} started")

        # Initialize the hdmiCEC module
        self.testhdmiCEC.initialise()

        # Should be something to verify the init failure?
        
        # Terminate the hdmiCEC module
        self.testhdmiCEC.terminate()

        #return finalResult

if __name__ == '__main__':
    # Configure the summary log file
    summerLogName = os.path.splitext(os.path.basename(__file__))[0] + "_summery"
    summeryLog = logModule(summerLogName, level=logModule.INFO)

    # Create an instance of the test class and execute the test
    test = hdmiCEC_test02_ReceiveCECCommands(summeryLog)
    test.run(False)