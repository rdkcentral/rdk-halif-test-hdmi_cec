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
void onRxDataReceived(int handle, void *callbackData, unsigned char *buf, int len)
{    
    UT_LOG ("\nBuffer generated: %x length: %d\n",buf, len);
    if((handle!=0) && (callbackData !=NULL) && (len>0)) {
             UT_LOG("\nCall back data generated is \n");
            for (int index=0; index < len; index++) {
                    UT_LOG("buf at index : %d is %x \n", index, buf[index]);
            }

	    memset(buf, 0, len);
    }
    else {
        if (handle == 0) {
            UT_FAIL("Error: Invalid handle.\n");
        }
        if (callbackData == NULL) {
            UT_FAIL("Error: Null callback data.\n");
        }
        if (len <= 0) {
            UT_FAIL("Error: Invalid length.\n");
        }
    }
	cbFlag = 1;
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
    HDMI_CEC_STATUS status;

    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    // Step 1: Call HdmiCecOpen()
    UT_LOG_DEBUG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&gHandle);
    //UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    //UT_ASSERT_NOT_EQUAL_FATAL(gHandle, 0);
    if ((status != HDMI_CEC_IO_SUCCESS) || (gHandle == NULL))
	    UT_LOG_ERROR("Invoking HdmiCecOpen failed");

	// Step 2: Register the call back 
	status = HdmiCecSetRxCallback(gHandle, onRxDataReceived,(void*)0xABABABAB);
	//UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);

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

   
    HDMI_CEC_STATUS status;
    int logicalAddress;

    UT_LOG_DEBUG("Enter Logical Address to add: ");
    scanf("Logical Address: %d", &logicalAddress);

	/* Check that logical address should be valid one */

    status = HdmiCecAddLogicalAddress(gHandle,logicalAddress );
    UT_LOG_DEBUG("Invoking HdmiCecAddLogicalAddress with handle: %d and logicalAddress: %d", gHandle, logicalAddress);
    //UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecAddLogicalAddress failed with status: %d\n", status);        
    }   

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

	HDMI_CEC_STATUS status;
	int logicalAddress = -1;

    status = HdmiCecGetLogicalAddress(gHandle, &logicalAddress);
    UT_LOG_DEBUG("Invoking HdmiCecGetLogicalAddress with handle: %d and logicalAddress: %d", gHandle, logicalAddress);
    //UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecAddLogicalAddress failed with status: %d\n", status);
    }  

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
void test_l3_hdmi_cec_sink_hal_TransmitHdmiCecCommand(void)
{
    gTestID = 4;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int sourceLogicalAddress = -1; // Example source logical address
    int destinationLogicalAddress = -1; // Example destination logical address
    unsigned char buf[16] = {0}; // Example CEC message buffer
    int len = 0, cecCommand = 0, cecCommandDataLength = 0;
    int result;

    // Reading inputs from the user or test framework
    UT_LOG_DEBUG("Source Logical Address:");
    scanf("%d",&sourceLogicalAddress);

    UT_LOG_DEBUG("Destination Logical Address: ");
    scanf("%d",&destinationLogicalAddress);

    UT_LOG_DEBUG("Enter CEC Command: ");
    scanf("%x", &cecCommand);
    printf("CEC Command:%x \n",cecCommand);    
    UT_LOG_DEBUG("Length of CEC Command data: ");
    scanf("%d",&cecCommandDataLength);

    // Reading CEC Command data
    for (int i = 0; i < cecCommandDataLength; i++) {
	UT_LOG_DEBUG("Databyte[%d]=",i);
        scanf("%x", &buf[i + 2]); // +2 to account for the first two bytes
    }

    // Building the CEC message
    buf[0] = (unsigned char)((sourceLogicalAddress << 4) | (destinationLogicalAddress & 0x0F));
    buf[1] = (unsigned char)cecCommand;

    len = cecCommandDataLength + 2; // 1 byte for logical addresses, 1 byte for CEC Command

    // Assume HdmiCecTx is a function to send the CEC command
    int status = HdmiCecTx(gHandle, buf, len, &result);

    UT_LOG_DEBUG("Invoking HdmiCecTx with handle: %d status:%d result:%d \n", gHandle, status, result);
    //UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    //UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    if ((result != HDMI_CEC_IO_SENT_BUT_NOT_ACKD) && (status != HDMI_CEC_IO_SUCCESS)) {
        UT_LOG_ERROR("HdmiCecTx failed with status: %d result:%d\n", status, result);
    }

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

	sleep(REPLY_TIMEOUT);  // Wait for a reply

	if (cbFlag != 1)
	{
		UT_LOG_ERROR("HdmiCecRx failed to receive data");
	}

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

        HDMI_CEC_STATUS status;
        int physicalAddress = -1;

    status = HdmiCecGetPhysicalAddress(gHandle, &physicalAddress);
    UT_LOG_DEBUG("Invoking HdmiCecGetPhysicalAddress with handle: %d and physical Address: %d", gHandle, physicalAddress);
    //UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecgetPhysicalAddress failed with status: %d\n", status);
    }

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

    HDMI_CEC_STATUS status;
    int logicalAddress;

    UT_LOG_DEBUG("Enter Logical Address to Remove: ");
    scanf("Logical Address: %d", &logicalAddress);

    // Invoke the API HdmiCecRemoveLogicalAddress
    status = HdmiCecRemoveLogicalAddress(gHandle, logicalAddress);
    UT_LOG_DEBUG("Invoking HdmiCecRemoveLogicalAddress with handle: %d and logicalAddress: %d", gHandle, logicalAddress);
   // UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecRemoveLogicalAddress failed with status: %d\n", status);
    }

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

    // Step 1: Call the pre-requisite API HdmiCecOpen()
    UT_LOG_DEBUG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecClose(gHandle);
    //UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    //UT_ASSERT_NOT_EQUAL_FATAL(gHandle, 0);

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
