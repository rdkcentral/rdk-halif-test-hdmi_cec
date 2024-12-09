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

import subprocess
import os
import sys
from enum import Enum, auto
import re
import yaml

# Add parent directory to the system path for module imports
dir_path = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(dir_path, "../"))

from raft.framework.plugins.ut_raft.configRead import ConfigRead
from raft.framework.plugins.ut_raft.utSuiteNavigator import UTSuiteNavigatorClass
from raft.framework.plugins.ut_raft.interactiveShell import InteractiveShell
from raft.framework.plugins.ut_raft.utBaseUtils import utBaseUtils

class hdmiCECClass():
    """
     HDMI CEC Class.

    This module provides common functionalities and extensions for the HDMI CEC Module.
    """

    def __init__(self, moduleConfigProfileFile :str, session=None, targetWorkspace="/tmp"):
        """
        Initializes the HDMI CEC Class instance with configuration settings.

        Args:
            moduleConfigProfileFile  (str): Path to the device profile configuration file.
            session: Optional; session object for the user interface.

        Returns:
            None
        """
        self.moduleName = "hdmicec"
        self.testConfigFile = os.path.join(dir_path, "hdmiCEC_testConfig.yml")
        self.testSuite = "L3 HDMICEC Sink Functions"

        # Load configurations for device profile and menu
        self.moduleConfigProfile = ConfigRead( moduleConfigProfileFile , self.moduleName)
        self.testConfig    = ConfigRead(self.testConfigFile, self.moduleName)
        self.testConfig.test.execute = os.path.join(targetWorkspace, self.testConfig.test.execute)
        self.utMenu        = UTSuiteNavigatorClass(self.testConfig, None, session)
        self.testSession   = session
        self.utils         = utBaseUtils()
        self.ports = self.moduleConfigProfile.fields.get("Ports")

        for artifact in self.testConfig.test.artifacts:
            filesPath = os.path.join(dir_path, artifact)
            self.utils.rsync(self.testSession, filesPath, targetWorkspace)

        # Start the user interface menu
        self.utMenu.start()

    def searchPattern(self, haystack, pattern):
        """
        Searches for the first occurrence of a specified pattern in the provided string.

        Args:
            haystack (str): The string to be searched.
            pattern (str): The regular expression pattern to search for.

        Returns:
            str: The first capturing group of the match if found; otherwise, None.

        Notes:
            - The pattern should contain at least one capturing group (parentheses).
            - If no match is found, None is returned.
        """
        match = re.search(pattern, haystack)
        if match:
            return match.group(1)
        return None

    def initialise(self):
        """
        Initializes the HDMI CEC module for sink.

        Args:
            None.

        Returns:
            None
        """
        result = self.utMenu.select( self.testSuite, "Init HDMI CEC")

    def terminate(self):
        """
        Terminates the hdmi cec module

        Args:
            None

        Returns:
            None
        """
        result = self.utMenu.select(self.testSuite, "Close HDMI CEC")

    def addLogicalAddress(self, logicalAddress:int):
        """
        Adding the logical address of a specific device.
        For now Sink to support only the logical address 0.

        Args:
            logicalAddress (int): The Logical address of the DUT.  This will be fixed to zero for a sink device for now.

        Returns:
            None
        """
        promptWithAnswers = [
                {
                    "query_type": "direct",
                    "query": "Enter Logical Address:",
                    "input": str(logicalAddress)
                }
        ]
        result = self.utMenu.select(self.testSuite, "Add Logical Address", promptWithAnswers)

    def removeLogicalAddress(self):
        """
        Remove logical address.

        Args:
            logicalAddress (int): The Logical address of the DUT that should be removed.

        Returns:
            None
        """
        result = self.utMenu.select(self.testSuite, "Remove Logical Address")

    def getLogicalAddress(self):
        """
        Retrieves the Logical Address of the DUT.

        Args:
            None.

        Returns:
            int: Logical address of the device.
        """
        result = self.utMenu.select( self.testSuite, "Get Logical Address")
        connectionStatusPattern = r"Result HdmiCecGetLogicalAddress\(IN:handle:[.*\], OUT:logicalAddress:[.*\]) HDMI_CEC_STATUS:[.*\])"
        logicalAddress = self.searchPattern(result, connectionStatusPattern)

        return logicalAddress

    def getPhysicalAddress(self):
        """
        Retrieve the Physical Address of the DUT.

        Args:
            None.

        Returns:
            int: Physical Address of the DUT.
        """
        result = self.utMenu.select( self.testSuite, "Get Phyiscal Address")
        typeStatusPattern = r"Result HdmiCecGetPhysicalAddress\(IN:handle:[.*\], OUT:physicalAddress:[.*\]) HDMI_CEC_STATUS:[.*\]"
        physicalAddress = self.searchPattern(result, typeStatusPattern)

        return physicalAddress

    def cecTransmitCmd(self, destLogicalAddress:int, cecCommand:int, cecData:list=None):
        """
        Transmit/Broadcast the CEC command and data to the respective destination.

        Args:
            None.

        Returns:
            None
        """

        promptWithAnswers = [
                {
                    "query_type": "direct",
                    "query": "Enter a valid Destination Logical Address:",
                    "input": str(destLogicalAddress)
                },
                {
                    "query_type": "direct",
                    "query": "Enter CEC Command (in hex):",
                    "input": str(cecCommand)
                },
        ]

        if cecData:
            for byte in cecData:
               promptWithAnswers.append(
                   {
                       "query_type": "direct",
                       "query": "Enter Databyte",
                       "input": str(byte)
                   })

        result = self.utMenu.select( self.testSuite, "Transmit CEC Command",promptWithAnswers)

    def __del__(self):
        """
        Cleans up and de-initializes the hdmi cec  helper by stopping the test menu.

        Args:
            None.

        Returns:
            None
        """
        self.utMenu.stop()

# Test and example usage code
if __name__ == '__main__':

    shell = InteractiveShell()
    shell.open()

    platformProfile = dir_path + "/../../../profiles/sink/sink_hdmiCEC.yaml"

    # test the class assuming that it's optional
    test = hdmiCECClass(platformProfile, shell)

    # Initialize the hdmiCEC module
    test.initialise()

    # Add the logical Address. For now 0 only.
    test.addLogicalAddress(0)

    # Get Physical Address
    physicalAddress = test.getPhysicalAddress()

    # Close the device
    test.terminate()

    shell.close()
