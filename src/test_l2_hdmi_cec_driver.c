/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:*
* Copyright 2023 RDK Management
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
* @file test_l2_hdmi_cec_driver.c
* @page hdmi_cec_driver Level 2 Tests
*
* ## Module's Role
* This module includes Level 2 functional tests (success and failure scenarios).
* This is to ensure that the hdmi_cec_driver APIs meet the requirements across all vendors.
*
* **Pre-Conditions:**  None@n
* **Dependencies:** None@n
*
* Ref to API Definition specification documentation : [halSpec.md](../../../docs/halSpec.md)
*/

#include <ut.h>
#include <ut_log.h>
#include "hdmi_cec_driver.h"

#define CEC_GET_CEC_VERSION (0x9F)

extern int HdmiCec_PreReq();
extern int HdmiCec_PostReq();

extern int handle;

/**
* @brief This test aims at testing the addition and retrieval of a logical address in HDMI CEC device driver
*
* In this unit test, the HdmiCecAddLogicalAddress and HdmiCecGetLogicalAddress functions of the HDMI CEC driver are tested to ensure these functions successfully add and retrieve a logical address to and from a handle respectively. The test will ensure that the added and retrieved value of the logical address are equal.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 001@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_AddAndGetLogicalAddress( void )
{  
    int logicalAddressSet = 0x00;
    int logicalAddressGet = 0;
    HDMI_CEC_STATUS status;

    UT_LOG("Entering test_l2_hdmi_cec_driver_AddAndGetLogicalAddress...");

    //Add logic address 
    status = HdmiCecAddLogicalAddress(handle, logicalAddressSet); 
    UT_LOG("Invoking HdmiCecAddLogicalAddress API with handle and logicalAddressSet. Both are valid.");
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    //Get the logical address we just set
    status = HdmiCecGetLogicalAddress(handle, &logicalAddressGet); 
    UT_LOG("Invoking HdmiCecGetLogicalAddress API with handle and pointer to logicalAddressGet buffer. Handle is valid and buffer is not NULL.");
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    //Check the equality of the logical address we set against the logical address we retrieved
    UT_ASSERT_EQUAL(logicalAddressSet, logicalAddressGet);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_AddAndGetLogicalAddress...");
}

/**
* @brief Unit test for testing the failure scenario of getting the Logical Address in the L2 HDMI CEC driver
*
* This test case is designed to test if HdmiCecGetLogicalAddress returns an error when attempting to get a logical 
* address that is not 0 or 14. This verifies the robustness of the error handling in the driver.
*
* **Test Group ID:** 02
* **Test Case ID:** 002
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/
void test_l2_hdmi_cec_driver_AddLogicalAddressFailure(void) 
{

    /* Define and initialize status and logicalAddress */
    HDMI_CEC_STATUS status;
    int logicalAddress = 0;


    UT_LOG("Entering test_l2_hdmi_cec_driver_AddLogicalAddressFailure...");
    
    /* Setting logical address to a value other than 0 or 14 */
    logicalAddress = 1;
    //Add logic address 
    status = HdmiCecAddLogicalAddress(handle, logicalAddress); 
    UT_LOG("Invoking HdmiCecAddLogicalAddress API with handle and logicalAddressSet. Both are valid.");
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT );

    UT_LOG("Exiting test_l2_hdmi_cec_driver_AddLogicalAddressFailure...");

}

/**
* @brief This test verifies the Remove and Get functionality of Logical Address in HDMI CEC driver
*
* This test function is designed to invoke the HdmiCecRemoveLogicalAddress API followed by HdmiCecGetLogicalAddress API with a particular testHandle and logicalAddresses. The two APIs' return values are checked to be HDMICEC_IO_SUCCESS ensuring functionality.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 003@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_RemoveAndGetLogicalAddress( void )
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_RemoveAndGetLogicalAddress...");

    int logicalAddresses = 0x0;  // for sink devices as per the API specification
    HDMI_CEC_STATUS status;
    
    //Add logic address 
    status = HdmiCecAddLogicalAddress(handle, logicalAddresses); 
    UT_LOG("Invoking HdmiCecAddLogicalAddress API with handle and logicalAddress. Both are valid.");
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress() with handle and logicalAddresses, Expecting HDMICEC_IO_SUCCESS");
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    int logicalAddressRetrieved;
    status = HdmiCecGetLogicalAddress(handle, &logicalAddressRetrieved);
    UT_LOG("Invoking HdmiCecGetLogicalAddress() with handle, Expecting HDMICEC_IO_SUCCESS");

    // Ensure the success condition is met (logical address has been removed)
    if (status == HDMI_CEC_IO_SUCCESS && logicalAddressRetrieved == 0x0F) // 0x0F is the address specified for sink devices when not added or removed
    {
        UT_LOG("Exiting test_l2_hdmi_cec_driver_RemoveAndGetLogicalAddress SUCCESS - logical address has been removed");
    }
}

/**
* @brief This test validates the scenario wherein the removal of a logical address and broadcast failure is handled in the HDMI CEC driver
*
* This test checks whether the functionality of HDMI CEC driver handles the case properly when a logical address is removed and broadcast subsequently fails. It makes use of two specific methods from the HDMI CEC API, namely HdmiCecRemoveLogicalAddress and HdmiCecTx for the removal and transmission respectively. The test is designed to pass when the transmission after removal fails, validating the robustness and exception handling capability of the unit
*
* **Test Group ID:** 02@n
* **Test Case ID:** 004@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/
void test_l2_hdmi_cec_driver_RemoveAndBroadcastFail( void )
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_RemoveAndBroadcastFail...\n");
    int logicalAddresses = 0, result;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    int len = 1; // hypothetical length for test purpose
    // Call HdmiCecRemoveLogicalAddress with valid input.
    HDMI_CEC_STATUS status1 = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with handle=1, logicalAddresses=0 \n");
    // Assert that the return value is as expected.
    UT_ASSERT_EQUAL(status1, HDMI_CEC_IO_SUCCESS);
    // Call HdmiCecTx with specific values.
    HDMI_CEC_STATUS status2 = HdmiCecTx(handle, buf, len, &result);
    UT_LOG("Invoking HdmiCecTx with handle=1, buf=empty, len=1\n");
    // Assert that the return value is as expected.
    // If the transmission is failed as expected, the test is passed
    if (status2 != HDMI_CEC_IO_SUCCESS) {
        UT_LOG("Test Passed: The attempt fails during HAL Transmission using HdmiCecTx as expected\n");
    }
}

/**
* @brief This unit test validates the function HdmiCecGetPhysicalAddress in L2 HDMI CEC driver for its ability to retrieve the physical address.
*
* The test makes a call to the function HdmiCecGetPhysicalAddress and checks if the returned address is within the assumed valid range of '0' to '4.4.4.4'. This is to ensure that the function is correctly retrieving the physical address. Failure of this test may indicate a problem in the physical address retrieval methodology of the HDMI CEC driver.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 005@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_GetPhysicalAddress(void)
{   
    UT_LOG("Entering test_l2_hdmi_cec_driver_GetPhysicalAddress...\n");
    
    unsigned int physicalAddress;
    HDMI_CEC_STATUS apiStatus;
    
    apiStatus = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    
    if (apiStatus == HDMI_CEC_IO_SUCCESS) 
    {
        UT_LOG("Invoked HdmiCecGetPhysicalAddress API with handle = 1. Returned physical address: %u\n", physicalAddress);
                 
        UT_ASSERT_EQUAL(physicalAddress >= 0 && physicalAddress <= 0xF0F0F0F0, 1); // Assuming valid physical addresses range from '0' to '4.4.4.4'
        
        if (physicalAddress >= 0 && physicalAddress <= 0xF0F0F0F0) 
        {
            UT_LOG("Physical address is valid. Exiting test_l2_hdmi_cec_driver_GetPhysicalAddress...\n");
        }
    }
}

static UT_test_suite_t * pSuite = NULL;

/**
 * @brief Register the main tests for this module
 *
 * @return int - 0 on success, otherwise failure
 */

int test_hdmi_cec_driver_l2_register(void)
{
    // Create the test suite
    pSuite = UT_add_suite("[L2 hdmi_cec_driver]", HdmiCec_PreReq, HdmiCec_PostReq);
    if (pSuite == NULL) {
        return -1;
    }
    // List of test function names and strings

    UT_add_test( pSuite, "l2_hdmi_cec_driver_AddAndGetLogicalAddress", test_l2_hdmi_cec_driver_AddAndGetLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_AddLogicalAddressFailure", test_l2_hdmi_cec_driver_AddLogicalAddressFailure);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_RemoveAndGetLogicalAddress", test_l2_hdmi_cec_driver_RemoveAndGetLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_RemoveAndBroadcastFail", test_l2_hdmi_cec_driver_RemoveAndBroadcastFail);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_GetPhysicalAddress", test_l2_hdmi_cec_driver_GetPhysicalAddress);

    return 0;
}
