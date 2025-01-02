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
sys.path.append(os.path.join(dir_path, "../../"))
sys.path.append(os.path.join(dir_path, "../"))

from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.plugins.ut_raft.utUserResponse import utUserResponse
from raft.framework.plugins.ut_raft import utHelperClass
from raft.framework.core.logModule import logModule
from classes.hdmiCEC import hdmiCECClass

class hdmiCECHelperClass(utHelperClass):

    """
    Helper class for managing HDMI CEC tests.

    This class extends the `utHelperClass` and provides functionality for preparing
    and cleaning up HDMI CEC tests.
    """

    def __init__(self, testName:str, qcId:str, log:logModule=None ):
        """
        Initializes the test helper class with test name, setup configuration, and session management.

        Args:
            testName (str): Name of the test.
            qcId (str): Quality Control (QC) ID of the test.
            log (logModule, optional): Parent log module instance for logging. Defaults to None.
        """
        self.testName  = ""
        self.moduleName = "hdmicec"
        self.rackDevice = "dut"

        # Initialize the base helper class
        super().__init__(testName, qcId, log)

        # Open Sessions hal test
        self.hal_session = self.dut.getConsoleSession("ssh_hal_test")

        deviceTestSetup = self.cpe.get("test")

         # Create user response Class
        self.testUserResponse = utUserResponse()

        # Get path to device profile file
        self.moduleConfigProfileFile = os.path.join(dir_path, deviceTestSetup.get("profile"))

        self.targetWorkspace = self.cpe.get("target_directory")
        self.targetWorkspace = os.path.join(self.targetWorkspace, self.moduleName)

        self.testCECCommands = os.path.join(dir_path, "hdmiCECTestCommands.yml")
        hdmicec = ConfigRead(self.testCECCommands, self.moduleName)
        self.cecCommands = hdmicec.fields.get(self.testName)
        self.testLogicalAddress = hdmicec.logicalAddress

    def testPrepareFunction(self):
        """
        Cleans up the test environment by deinitializing the HDMI CEC instance.

        Args:
            powerOff (bool, optional): Flag to indicate whether to power off the device. Defaults to True.

        Returns:
            bool
        """

        # Create the hdmiCEC class
        self.testhdmiCEC = hdmiCECClass(self.moduleConfigProfileFile, self.hal_session, self.targetWorkspace)
        self.deviceType  = self.testhdmiCEC.getDeviceType()

        return True

    def testEndFunction(self, powerOff=True):

        super().testEndFunction(powerOff)
        # Clean up the hdmiCEC instance
        del self.testhdmiCEC

