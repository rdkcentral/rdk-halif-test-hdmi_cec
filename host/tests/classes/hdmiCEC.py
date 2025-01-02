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

import re
import os
import sys

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

    Provides functionalities for initializing, configuring, and controlling
    HDMI CEC (Consumer Electronics Control) operations in the test environment.
    """

    def __init__(self, moduleConfigProfileFile:str, session=None, targetWorkspace:str="/tmp"):
        """
        Initialize the HDMI CEC Class with configuration settings.

        Args:
            moduleConfigProfileFile (str): Path to the profile configuration file for the HDMI CEC module.
            session: Optional session object for managing interactions with the device.
            targetWorkspace (str, optional): Target workspace directory on the device. Defaults to "/tmp".

        Returns:
            None
        """
        self.moduleName = "hdmicec"
        self.testConfigFile = os.path.join(dir_path, "hdmiCEC_testConfig.yml")
        self.testSuite = "L3 HDMICEC Functions"
        cecResponseFile = "cec_responses.yml"

        # Prepare the profile file on the target workspace
        self.moduleConfigProfile = ConfigRead( moduleConfigProfileFile , self.moduleName)
        profileOnTarget = os.path.join(targetWorkspace, os.path.basename(moduleConfigProfileFile))
        cecResponseFileOnTarget = os.path.join(targetWorkspace, cecResponseFile)
        self.testConfig    = ConfigRead(self.testConfigFile, self.moduleName)
        self.testConfig.test.execute = os.path.join(targetWorkspace, self.testConfig.test.execute) + f" -p {profileOnTarget}" + f" -p {cecResponseFileOnTarget}"
        self.utMenu        = UTSuiteNavigatorClass(self.testConfig, None, session)
        self.testSession   = session
        self.utils         = utBaseUtils()

        # Copy required artifacts to the target workspace
        for artifact in self.testConfig.test.artifacts:
            filesPath = os.path.join(dir_path, artifact)
            self.utils.rsync(self.testSession, filesPath, targetWorkspace)

        # Copy the profile configuration to the target workspace
        self.utils.scpCopy(self.testSession, moduleConfigProfileFile, targetWorkspace)

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
        Initializes the HDMI CEC module.

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

    def addLogicalAddress(self, logicalAddress:str='0'):
        """
        Add a logical address for the device.

        Args:
            logicalAddress (str, optional): Logical address of the device. Defaults to '0'. Value ranges from '0' - 'f'

        Returns:
            None
        """
        promptWithAnswers = [
                {
                    "query_type": "direct",
                    "query": "Enter Logical Address:",
                    "input": logicalAddress
                }
        ]
        result = self.utMenu.select(self.testSuite, "Add Logical Address", promptWithAnswers)

    def removeLogicalAddress(self):
        """
        Remove the logical address of the device.

        Args:
            None

        Returns:
            None
        """
        result = self.utMenu.select(self.testSuite, "Remove Logical Address")

    def getLogicalAddress(self):
        """
        Get the logical address of the device.

        Args:
            None.

        Returns:
            str: Logical address of the device. Value ranges from '0' - 'f'
        """
        result = self.utMenu.select( self.testSuite, "Get Logical Address")
        connectionStatusPattern = r"Result HdmiCecGetLogicalAddress\(IN:handle:\[0x[0-9A-F]+\], OUT:logicalAddress:\[([0-9A-Fa-f]+)\]\)"
        logicalAddress = self.searchPattern(result, connectionStatusPattern)

        return logicalAddress

    def getPhysicalAddress(self):
        """
        Get the physical address of the device.

        Args:
            None.

        Returns:
            str: Physical Address of the DUT.
        """
        result = self.utMenu.select( self.testSuite, "Get Phyiscal Address")

        addressBytes = None
        pattern = r"Result HdmiCecGetPhysicalAddress\(IN:handle:.*OUT:physicalAddress:\[([\da-fA-F]\.[\da-fA-F]\.[\da-fA-F]\.[\da-fA-F])\]"
        physicalAddress = self.searchPattern(result, pattern)
        if physicalAddress:
            parts = physicalAddress.split(".")
            high_byte = hex(int(parts[0], 16) << 4 | int(parts[1], 16))
            low_byte = hex(int(parts[2], 16) << 4 | int(parts[3], 16))
            addressBytes = [f"0x{int(high_byte, 16):02X}",f"0x{int(low_byte, 16):02X}"]

        return addressBytes

    def cecTransmitCmd(self, destLogicalAddress:str, cecCommand:str, cecData:list=None):
        """
        Transmit or broadcast a CEC command to a specified destination.

        Args:
            destLogicalAddress (str): Destination logical address.
            cecCommand (str): CEC command in hexadecimal.
            cecData (list, optional): List of data bytes to include in the transmission.

        Returns:
            None
        """

        promptWithAnswers = [
                {
                    "query_type": "direct",
                    "query": "Enter a valid Destination Logical Address:",
                    "input": destLogicalAddress
                },
                {
                    "query_type": "direct",
                    "query": "Enter CEC Command (in hex):",
                    "input": cecCommand
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

        pattern = r"Result HdmiCecTx\(.*OUT:result:\[(.*?)\]\).*HDMI_CEC_STATUS:\[(.*?)\]"

        txStatus = self.searchPattern(result, pattern)

        status = False
        if destLogicalAddress == 'f':
            if txStatus == "HDMI_CEC_IO_SENT_BUT_NOT_ACKD":
                status = True
        else:
            if txStatus == "HDMI_CEC_IO_SENT_AND_ACKD":
                status = True

        return status

    def readCallbackDetails (self):
        """
        Parses the callback logs from the device.

        Args:
            None.

        Returns:
            dict: A dictionary with two keys:
                - "Received": A list of dictionaries containing details about received opcodes.
                - "Response": A list of dictionaries containing details about sent response opcodes.
                Each dictionary contains the following keys:
                    - "Opcode" (str): The opcode value in hexadecimal.
                    - "Description" (str): A textual description of the opcode.
                    - "Initiator" (str): The initiator address in hexadecimal.
                    - "Destination" (str): The destination address in hexadecimal.
                    - "Data" (list): The data associated with the opcode.
        """
        result = {"Received": [], "Response": []}

        callbackLogs = self.testSession.read_all()

        received_pattern = re.compile(
            r"Received Opcode: \[([^\]]+)\] \[([^\]]+)\] Initiator: \[([^\]]+)\], Destination: \[([^\]]+)\] Data: \[(.*?)\]"
        )
        sent_pattern = re.compile(
            r"Sent Response Opcode: \[([^\]]+)\] \[([^\]]+)\] Initiator: \[([^\]]+)\], Destination: \[([^\]]+)\] Data: \[(.*?)\]"
        )

        def parse_data_field(data_field):
            # Split the data field into an array of hex values
            return ["0x" + value.strip() for value in data_field.split(":")]

        for match in received_pattern.finditer(callbackLogs):
            opcode, description, initiator, destination, data = match.groups()
            result["Received"].append({
                "Opcode": opcode,
                "Description": description,
                "Initiator": initiator,
                "Destination": destination,
                "Data": parse_data_field(data)
            })

        for match in sent_pattern.finditer(callbackLogs):
            opcode, description, initiator, destination, data = match.groups()
            result["Response"].append({
                "Opcode": opcode,
                "Description": description,
                "Initiator": initiator,
                "Destination": destination,
                "Data": parse_data_field(data)
            })

        return result

    def getDeviceType(self):
        """
        Retrieves the type of dut.

        Args:
            None.

        Returns:
            str: The type of device:
                - "sink" -  sink device.
                - "source" - source device.
                - None if the device type is unknown or unsupported.
        """
        type = self.moduleConfigProfile.fields.get("type")

        return type

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

    # Broadcast 0x85 cec command
    test.cecTransmitCmd('f', '0x85')

    # Transmitt 0x04 cec command to '1'
    test.cecTransmitCmd('1', '0x85')

    # Read the callback details
    result = test.readCallbackDetails()

    # Close the device
    test.terminate()

    shell.close()
