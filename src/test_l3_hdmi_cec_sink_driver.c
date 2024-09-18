/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:*
* Copyright 2024 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 *
 */

/**
 * @addtogroup HDMI_CEC HDMI CEC Module
 * @{
 *
 */

/**
 * @defgroup HDMI_CEC_HALTESTS HDMI CEC HAL Tests
 * @{
 *
 */


/**
 * @defgroup HDMI_CEC_HALTESTS_L3 HDMI CEC HAL Tests L2 File
 * @{
 * @parblock
 *
 * ### L2 Test Cases for HDMI CEC HAL :
 *
 *
 * ## Module's Role
 * This module includes Level 3 functional test interfaces.
 * This Test Interfaces provides a scope to create a User Test cases for HDMI CEC Sink modules that can be either Manual or automated scripts.
 *
 * **Pre-Conditions:**  None@n
 * **Dependencies:** None@n
 *
 * Refer to API Definition specification documentation : [hdmi-cec_halSpec.md](../../docs/pages/hdmi-cec_halSpec.md)
 *
 * @endparblock
 */

/**
 * @file test_l3_hdmi_cec_sink_driver.c
 *
 */

#include <ut.h>
#include <ut_log.h>
#include "hdmi_cec_driver.h"

#define TIMEOUT 5
#define REPLY_TIMEOUT 5

static int gTestGroup = 3;
static int gTestID = 1;
static int gHandle = 0;
static int cbFlag = 0;

// CEC command map table and supporting function
typedef struct {
    unsigned char cecCommand;  // CEC command code
    const char* commandName;   // Human-readable command name
    int dataLength;            // Number of data bytes required for the command
} CecCommandMap;

CecCommandMap cecCommandTable[] = {
    {0x00, "Feature Abort", 2},
    {0x04, "Image View On", 0},
    {0x0D, "Text View On", 0},
    {0x20, "Active Source", 2},
    {0x32, "Inactive Source", 2},
    {0x36, "Request Active Source", 0},
    {0x41, "Standby", 0},
    {0x44, "User Control Pressed", 1},
    {0x45, "User Control Released", 0},
    {0x46, "Give OSD Name", 0},
    {0x47, "Set OSD Name", 15},
    {0x82, "Routing Change", 4},
    {0x83, "Routing Information", 2},
    {0x86, "Report Physical Address", 3},
    {0x87, "Request Active Source", 0},
    {0x8C, "Give Device Vendor ID", 0},
    {0x89, "Device Vendor ID", 3},
    {0x90, "Vendor Command", 16},
    {0x91, "Vendor Command with ID", 16},
    {0x92, "Give Device Power Status", 0},
    {0x93, "Report Power Status", 1},
    {0x9E, "CEC Version", 1},
    {0x9F, "Get CEC Version", 0},
    {0xA0, "Get Menu Language", 0},
    {0xA1, "Set Menu Language", 3},
    {0xA5, "Report Physical Address", 3},
    {0xA6, "Request Short Audio Descriptor", 1},
    {0xC0, "Report Audio Status", 1},
    {0xC1, "Give Audio Status", 0},
    {0xC2, "Set System Audio Mode", 1},
    {0xC3, "Report Audio Descriptor", 1},
    {0xC4, "Set Audio Rate", 1},
    // Add more commands as needed
};

/* cecError_t */
const static ut_control_keyStringMapping_t cecError_mapTable [] = {
  {"HDMI_CEC_IO_SUCCESS",           (int32_t)HDMI_CEC_IO_SUCCESS}
  {"HDMI_CEC_IO_SENT_AND_ACKD",     (int32_t)HDMI_CEC_IO_SENT_AND_ACKD},
  {"HDMI_CEC_IO_SENT_BUT_NOT_ACKD", (int32_t)HDMI_CEC_IO_SENT_BUT_NOT_ACKD},
  {"HDMI_CEC_IO_SENT_FAILED",       (int32_t)HDMI_CEC_IO_SENT_FAILED},
  {"HDMI_CEC_IO_NOT_OPENED",        (int32_t)HDMI_CEC_IO_NOT_OPENED},
  {"HDMI_CEC_IO_INVALID_ARGUMENT",  (int32_t)HDMI_CEC_IO_INVALID_ARGUMENT},
  {"HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE", (int32_t)HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE},
  {"HDMI_CEC_IO_GENERAL_ERROR",     (int32_t)HDMI_CEC_IO_GENERAL_ERROR},
  {"HDMI_CEC_IO_ALREADY_OPEN",      (int32_t)HDMI_CEC_IO_ALREADY_OPEN},
  {"HDMI_CEC_IO_ALREADY_REMOVED",   (int32_t)HDMI_CEC_IO_ALREADY_REMOVED},
  {"HDMI_CEC_IO_INVALID_OUTPUT",    (int32_t)HDMI_CEC_IO_INVALID_OUTPUT},
  {"HDMI_CEC_IO_INVALID_HANDLE",    (int32_t)HDMI_CEC_IO_INVALID_HANDLE},
  {"HDMI_CEC_IO_OPERATION_NOT_SUPPORTED", (int32_t)HDMI_CEC_IO_OPERATION_NOT_SUPPORTED},
  {"HDMI_CEC_IO_NOT_ADDED",         (int32_t)HDMI_CEC_IO_NOT_ADDED},
  {"HDMI_CEC_IO_MAX",               (int32_t)HDMI_CEC_IO_MAX}
  {  NULL, -1 }
};


/**
* @brief CEC Command with data size mapping function.
*
* This function maps the provided CEC command with the data length required for the CEC Command.
* This will help the test to consider the correct size of the data required for the CEC Command.
*
* **Test Group ID:** 03@n
* **Test Case ID:** 000@n
*
*/
int getCecCommandInfo(unsigned char cecCommand, const char** commandName, int* dataLength) {
    UT_LOG_INFO("In %s(IN: cecCommand: [0x%02X])\n", __FUNCTION__, cecCommand);

    int tableSize = sizeof(cecCommandTable) / sizeof(CecCommandMap);

    for (int i = 0; i < tableSize; i++) {
        if (cecCommandTable[i].cecCommand == cecCommand) {
            *commandName = cecCommandTable[i].commandName;
            *dataLength = cecCommandTable[i].dataLength;
            UT_LOG_INFO("Out %s(OUT: commandName: [%s], OUT: dataLength: [%d])\n", __FUNCTION__, *commandName, *dataLength);
            return 0; // Command found
        }
    }

    UT_LOG_INFO("Out %s(OUT: Command not found)\n", __FUNCTION__);
    return -1; // Command not found
}

/**
* @brief Rx Callback function
*
* This function is a Receive Call back function called when data is received from a connected device.
* This function should be registered through HdmiCecSetRxCallback().
*
* **Test Group ID:** 03@n
* **Test Case ID:** 000@n
*
*/

/* Callback function */
void onRxDataReceived(int handle, void *callbackData, unsigned char *buf, int len) {
    UT_LOG_INFO("In %s(IN: handle: [%d], IN: callbackData: [%p], IN: buf: [%p], IN: len: [%d])\n", __FUNCTION__, handle, callbackData, buf, len);

    if ((handle != 0) && (callbackData != NULL) && (len > 0)) {
        UT_LOG_INFO("CEC Data Received\n");

        // Log each byte received in the buffer
        for (int index = 0; index < len; index++) {
            UT_LOG_INFO("Buffer at index [%d]: [0x%02X]\n", index, buf[index]);
        }

        // Clear the buffer after processing
        memset(buf, 0, len);
        UT_LOG_INFO("Buffer cleared after processing.\n");

    } else {
        // Log specific errors based on failed conditions
        if (handle == 0) {
            UT_LOG_ERROR("Error: Invalid handle.\n");
        }
        if (callbackData == NULL) {
            UT_LOG_ERROR("Error: Null callback data.\n");
        }
        if (len <= 0) {
            UT_LOG_ERROR("Error: Invalid length.\n");
        }
    }

    cbFlag = 1;
    UT_LOG_INFO("Out %s(OUT: cbFlag set to [1])\n", __FUNCTION__);
}

/**
* @brief Initialization of the HAL CEC Module
*
* This test provides a scope to open the HAL CEC module and preserve the handle.

*
* **Test Group ID:** 03@n
*
* **Test Case ID:** 001@n
*
* **Pre-Conditions:** None@n
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 1 to before running any test.
*
*/

void test_l3_hdmi_cec_sink_hal_Init(void)
{
   gTestID = 1;
   HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;

   UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

   // Step 1: Call HdmiCecOpen()
   UT_LOG_INFO("Calling HdmiCecOpen(IN:gHandle[0x%0X])",&gHandle);
   status = HdmiCecOpen(&gHandle);
   UT_LOG_INFO("Result HdmiCecSetRxCallback() cecError:[%s]",UT_ControlGetMapString(cecError_mapTable,status));
   assert(status == HDMI_CEC_IO_SUCCESS);

   // Step 2: Register the call back
   UT_LOG_INFO("Calling HdmiCecSetRxCallback(IN:handle[0x%0X],IN:cbfunc[0x%0X]",gHandle,onRxDataReceived);
   status = HdmiCecSetRxCallback(gHandle, onRxDataReceived,(void*)0xABABABAB);
   UT_LOG_INFO("Result HdmiCecSetRxCallback() cecError:[%s]",UT_ControlGetMapString(cecError_mapTable,status));
   assert(status == HDMI_CEC_IO_SUCCESS);

   UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief This test provides a scope to add the sink logical address. Usually it shall be zero.
*
* This test case provides a scope to add the available sink logical address
* to a Device under test.
*
* Note:
* This applies only for the Sink Devices.
* Source devices will get the logical address during CEC open.
*
* **Test Group ID:** 02@n
*
* **Test Case ID:** 002@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be intialized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 2 and provide the logical address.
*
*/
void test_l3_hdmi_cec_sink_hal_AddLogicalAddress(void)
{
    gTestID = 2;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;

    int logicalAddress = -1;

    UT_LOG_MENU_INFO("----------------------------------------------------------");
    UT_LOG_MENU_INFO("\t \tEnter Logical Address: ");
    UT_LOG_MENU_INFO("----------------------------------------------------------");
    scanf("%d", &logicalAddress);

    /* Check that logical address should be valid one */
    UT_LOG_INFO("Calling HdmiCecAddLogicalAddress(IN:handle:[0x%0X], IN:logicalAddress:[%d]",gHandle,logicalAddress);
    status = HdmiCecAddLogicalAddress(gHandle,logicalAddress );
    UT_LOG_INFO("Failed HdmiCecAddLogicalAddress (IN:handle:[0x%0X], IN:logicalAddress:[%d]), status[%d]",gHandle,logicalAddress,UT_ControlGetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}


/**
* @brief Test to get the logical address
*
* This test provides a scope to check the assigned logical address of the device.
*
* **Test Group ID:** 02@n
*
* **Test Case ID:** 003@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be intialized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 3 and shall read the Logical address displayed on the console.

*
*/
void test_l3_hdmi_cec_sink_hal_GetLogicalAddress(void)
{
    gTestID = 3;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;
    int logicalAddress = -1;

    UT_LOG_INFO("Calling HdmiCecGetLogicalAddress(IN: handle: [0x%0X], OUT: logicalAddress: [])", gHandle);
    status = HdmiCecGetLogicalAddress(gHandle, &logicalAddress);

    UT_LOG_INFO("HdmiCecGetLogicalAddress failed (IN: handle: [0x%0X], OUT: logicalAddress: [%d]), status: [%d])", gHandle, logicalAddress, UT_ControlGetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);
UT_ControlGetMapString(cecError_mapTable,status)
    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}



/**
* @brief  Test provides a scope to Transmit the CEC Command
*
* This test provides an interface to user/automation tool to transmit a CEC Command.
* Necessary input should be provided to the test.
*
* **Test Group ID:** 02@n
*
* **Test Case ID:** 004@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be intialized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 4 and shall provide the necessary source and destination logical address, 
* CEC command, data lenght and data.
*
*/
void test_l3_hdmi_cec_sink_hal_TransmitHdmiCecCommand(void) {
    gTestID = 4;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int sourceLogicalAddress = -1;
    int destinationLogicalAddress = -1;
    unsigned char buf[16] = {0};
    int len = 0, cecCommand = 0;
    int result;
    const char* commandName;
    int expectedDataLength;

    // Reading inputs from the user or test framework
    UT_LOG_MENU_INFO("----------------------------------------------------------");
    UT_LOG_MENU_INFO("\t \tEnter a valid Source Logical Address:");
    UT_LOG_MENU_INFO("----------------------------------------------------------");
    scanf("%d", &sourceLogicalAddress);

    UT_LOG_MENU_INFO("----------------------------------------------------------");
    UT_LOG_MENU_INFO("\t \tEnter a valid Destination Logical Address: ");
    scanf("%d", &destinationLogicalAddress);
    UT_LOG_MENU_INFO("----------------------------------------------------------");


    UT_LOG_MENU_INFO("----------------------------------------------------------");
    UT_LOG_MENU_INFO("\t \tEnter CEC Command (in hex): ");
    UT_LOG_MENU_INFO("----------------------------------------------------------");
    scanf("%x", &cecCommand);

    // Validate the CEC command and get the expected data length
    if (getCecCommandInfo(cecCommand, &commandName, &expectedDataLength) != 0) {
        // Command not found in the map, prompt the user for additional information
        UT_LOG_WARNING("CEC command 0x%02X is not recognized. It might be a vendor-specific command.", cecCommand);

	UT_LOG_MENU_INFO("----------------------------------------------------------");
        UT_LOG_MENU_INFO("\t \tPlease enter the number of data bytes for the CEC command: ");
        UT_LOG_MENU_INFO("----------------------------------------------------------");
        scanf("%d", &expectedDataLength);

        commandName = "Vendor Specific Command";
    } else {
        UT_LOG_INFO("CEC Command: %s (0x%02X), expects %d data byte(s)", commandName, cecCommand, expectedDataLength);
    }

    // If the command has data bytes, prompt for them
    if (expectedDataLength > 0) {
        for (int i = 0; i < expectedDataLength; i++) {
            UT_LOG_MENU_INFO("\t \tEnter Databyte[%d] (in hex):", i);
            scanf("%x", &buf[i + 2]); // +2 to account for the first two bytes
        }
    }

    // Building the CEC message
    buf[0] = (unsigned char)((sourceLogicalAddress << 4) | (destinationLogicalAddress & 0x0F));
    buf[1] = (unsigned char)cecCommand;
    len = expectedDataLength + 2; // 1 byte for logical addresses, 1 byte for CEC Command

    // Logging the transmission attempt
    UT_LOG_INFO("Calling HdmiCecTx(IN: handle: [0x%0X], IN: message length: [%d], IN: message data: [", gHandle, len);
    for (int i = 0; i < len; i++) {
        printf(" %02X", buf[i]);
    }
    printf(" ])");

    // Assume HdmiCecTx is a function to send the CEC command
    int status = HdmiCecTx(gHandle, buf, len, &result);
    UT_LOG_INFO("HdmiCecTx failed (IN: handle: [0x%0X], IN: length: [%d], result: [%d], status:[%d])", gHandle, len, result, UT_ControlGetMapString(cecError_mapTable,status));

    assert((result == HDMI_CEC_IO_SENT_BUT_NOT_ACKD) && (status == HDMI_CEC_IO_SUCCESS)) 

    // Optional delay after sending the command
    sleep(5);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief Test to Receive the CEC Command
*
* This test provides an interface to check whether the CEC has received any frames or not.
*
* **Test Group ID:** 02@n
*
* **Test Case ID:** 005@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be intialized through Test 1 before calling this test.
*
* **Dependencies:** @n
* Depends on the external device to send a CEC Frames with necessary command and data.
*
* **User Interaction:** @n
* User or Automation tool should select the Test 5 and make sure that the CEC Frames has been sent from an external
* device connected in the network.
*
*/
void test_l3_hdmi_cec_sink_hal_ReceiveHdmiCecCommand(void)
{
    gTestID = 5;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    UT_LOG_INFO("Waiting for HDMI CEC command (timeout: %d seconds)", REPLY_TIMEOUT);

    sleep(REPLY_TIMEOUT);  // Wait for a reply

    if (cbFlag != 1)
    {
        UT_LOG_ERROR("HdmiCecRx failed to receive data (IN: expected callback flag: [1], OUT: actual callback flag: [%d])", cbFlag);
	assert(cbFlag == 1);
    }
    else
    {
        UT_LOG_INFO("HdmiCecRx successfully received data (OUT: callback flag: [%d])", cbFlag);
    }

    // Resetting callback flag for future tests
    cbFlag = 0;

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}


/**
* @brief Test to get the physical address
*
* This test provides a scope to read the physical address of the device.
*
* **Test Group ID:** 02@n
*
* **Test Case ID:** 006@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be intialized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 6 to read the physical address of the device 
* device connected in the network.
*
*/
void test_l3_hdmi_cec_sink_hal_GetPhysicalAddress(void)
{
    gTestID = 6;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;
    int physicalAddress = -1;

    UT_LOG_INFO("Calling HdmiCecGetPhysicalAddress(IN: handle: [0x%0X], OUT: physicalAddress: [uninitialized])", gHandle);

    status = HdmiCecGetPhysiicalAddress(gHandle, &physicalAddress);
    UT_LOG_INFO("HdmiCecGetPhysicalAddress failed (IN: handle: [0x%0X], physicalAddress: [%d]), status:[%d]", gHandle, physicalAddress,UT_ControlGetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}


/**
* @brief Test to Remove logical address
*
* This test provides a scope to remove the logical address of the device. HAL API to set 
* to default logical addres 0xF once the logical address is removed.
*
* **Test Group ID:** 02@n
*
* **Test Case ID:** 07@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be intialized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 7 to delete the logical address.
*
*/
void test_l3_hdmi_cec_sink_hal_RemoveLogicalAddress(void)
{
    gTestID = 7;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;
    int logicalAddress = -1;

    UT_LOG_MENU_INFO("----------------------------------------------------------");
    UT_LOG_MENU_INFO("\t \tEnter Logical Address to Remove: ");
    UT_LOG_MENU_INFO("----------------------------------------------------------");

    scanf("%d", &logicalAddress);

    UT_LOG_INFO("Calling HdmiCecRemoveLogicalAddress(IN: handle: [0x%0X], IN: logicalAddress: [%d])", gHandle, logicalAddress);

    // Invoke the API HdmiCecRemoveLogicalAddress
    status = HdmiCecRemoveLogicalAddress(gHandle, logicalAddress);
    UT_LOG_INFO("HdmiCecRemoveLogicalAddress failed (IN: handle: [0x%0X], IN: logicalAddress: [%d], OUT: status: [%d])", gHandle, logicalAddress, UT_ControlGetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);
    
    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}
/**
* @brief Test to close the HDMI CEC device.
*
* This test provides a scope to close the created HDMI CEC handle.
*
* **Test Group ID:** 02@n
*
* **Test Case ID:** 08@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be intialized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 8 to close the created HDMI CEC handle.
*
*/
void test_l2_hdmi_cec_sink_hal_Close(void)
{
    gTestID = 8;
    HDMI_CEC_STATUS status;

    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    // Step 1: Log the pre-requisite action
    UT_LOG_INFO("Calling HdmiCecClose(IN: handle: [0x%0X])", gHandle);

    // Step 2: Call the API to close the HDMI CEC handle
    status = HdmiCecClose(gHandle);
    UT_LOG_INFO("HdmiCecClose failed (IN: handle: [0x%0X])  status: [%d]", gHandle, status);
    assert(status == HDMI_CEC_IO_SUCCESS);

    // Step 4: Log the end of the function
    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

static UT_test_suite_t * pSuite = NULL;

/**
 * @brief Register the main tests for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int test_register_hdmicec_hal_sink_l3_tests(void)
{
    // Create the test suite
    pSuite = UT_add_suite("[L3 HDMICEC Sink Functions] ", NULL, NULL);
    if (pSuite == NULL)
    {
        return -1;
    }
    // List of test function names and strings

    UT_add_test( pSuite, "L3_Init_HdmiCec", test_l3_hdmi_cec_sink_hal_Init);
    UT_add_test( pSuite, "L3_AddLogicalAddress", test_l3_hdmi_cec_sink_hal_AddLogicalAddress);
    UT_add_test( pSuite, "L3_GetLogicalAddress", test_l3_hdmi_cec_sink_hal_GetLogicalAddress);
    UT_add_test( pSuite, "L3_TransmitCecCommand", test_l3_hdmi_cec_sink_hal_TransmitHdmiCecCommand);
    UT_add_test( pSuite, "L3_ReceiveCecCommand", test_l3_hdmi_cec_sink_hal_ReceiveHdmiCecCommand);
    UT_add_test( pSuite, "L3_GetPhyiscalAddress", test_l3_hdmi_cec_sink_hal_GetPhysicalAddress);
    UT_add_test( pSuite, "L3_RemoveLogicalAddressHdmiCec_Sink", test_l3_hdmi_cec_sink_hal_RemoveLogicalAddress);
    UT_add_test( pSuite, "L3_Close_HdmiCec_Sink", test_l2_hdmi_cec_sink_hal_Close);

    return 0;
}

/** @} */ // End of HDMI CEC HAL Tests L1 File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
