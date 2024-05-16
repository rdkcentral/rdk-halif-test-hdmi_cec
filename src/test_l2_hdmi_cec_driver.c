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
#include <stdlib.h>
#include <time.h>

/**
* @brief Test to verify the default logical address of HDMI CEC driver
*
* This test verifies that the default logical address returned by the HDMI CEC driver is 0x0F. The test first initializes the HDMI CEC HAL using HdmiCecOpen(), then retrieves the logical address using HdmiCecGetLogicalAddress(), and finally checks that the returned logical address is 0x0F. The HDMI CEC HAL is then uninitialized using HdmiCecClose().
*
* **Test Group ID:** 02@n
* **Test Case ID:** 001@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_GetDefaultLogicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_GetDefaultLogicalAddress...");

    int handle;
    int logicalAddress;
    HDMI_CEC_STATUS status;

    // Step 1: Call the pre-requisite API HdmiCecOpen() to initialize the HDMI CEC HAL.
    status = HdmiCecOpen(&handle);
    UT_LOG("Invoking HdmiCecOpen() with valid handle. Return status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Step 2: Call the API HdmiCecGetLogicalAddress() with the handle obtained from HdmiCecOpen() and a pointer to an integer to store the logical address.
    status = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("Invoking HdmiCecGetLogicalAddress() with handle from HdmiCecOpen() and valid logicalAddress pointer. Return status: %d, Logical Address: %d", status, logicalAddress);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Step 3: Check that the logical address returned is 0x0F, which is the default logical address for sink devices when no logical address has been added or removed.
    UT_ASSERT_EQUAL(logicalAddress, 0x0F);

    // Step 4: Call the post-requisite API HdmiCecClose() with the handle obtained from HdmiCecOpen() to uninitialize the HDMI CEC HAL.
    status = HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecClose() with handle from HdmiCecOpen(). Return status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_GetDefaultLogicalAddress...");
}

/**
* @brief Test the sequence of operations: HdmiCecOpen, HdmiCecAddLogicalAddress, HdmiCecGetLogicalAddress, and HdmiCecClose
*
* This function tests the sequence of operations: HdmiCecOpen, HdmiCecAddLogicalAddress, HdmiCecGetLogicalAddress, and HdmiCecClose. It uses the CUnit framework's assertion macros to check that each operation returns the expected status code. It also checks that the logical address returned by HdmiCecGetLogicalAddress matches the address that was set by HdmiCecAddLogicalAddress.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 002@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_AddAndGetLogicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_AddAndGetLogicalAddress...");

    int handle = 0;
    int logicalAddress = 0;
    HDMI_CEC_STATUS status;

    // Step 1: Call HdmiCecOpen
    status = HdmiCecOpen(&handle);
    UT_LOG("Invoking HdmiCecOpen(). Status: %d, Handle: %d", status, handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_NOT_EQUAL(handle, 0);

    // Generate a random logical address between 0x00 and 0x0F
    srand(time(NULL));
    int randomLogicalAddress = rand() % 16;

    // Step 2: Call HdmiCecAddLogicalAddress
    status = HdmiCecAddLogicalAddress(handle, randomLogicalAddress);
    UT_LOG("Invoking HdmiCecAddLogicalAddress() with handle: %d and logicalAddress: %d. Status: %d", handle, randomLogicalAddress, status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Step 3: Call HdmiCecGetLogicalAddress
    status = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("Invoking HdmiCecGetLogicalAddress() with handle: %d. Status: %d, LogicalAddress: %d", handle, status, logicalAddress);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL(logicalAddress, randomLogicalAddress);

    // Step 4: Call HdmiCecClose
    status = HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecClose() with handle: %d. Status: %d", handle, status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_AddAndGetLogicalAddress...");
}

/**
* @brief This test function tests the removal of a logical address from the HDMI CEC driver
*
* This function tests the `HdmiCecRemoveLogicalAddress` API by first initializing the HDMI CEC HAL with `HdmiCecOpen`, adding a logical address with `HdmiCecAddLogicalAddress`, removing the logical address with `HdmiCecRemoveLogicalAddress`, verifying the removal with `HdmiCecGetLogicalAddress`, and finally closing the HDMI CEC HAL with `HdmiCecClose`. The function uses `UT_ASSERT_EQUAL` to assert that the return values of the APIs are as expected. If any assertion fails, the function will immediately return and the remaining steps will not be executed. The function also logs the status and output values of each API call.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 003@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/
void test_l2_hdmi_cec_driver_RemoveLogicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_RemoveLogicalAddress...");

    int handle;
    int logicalAddress;
    HDMI_CEC_STATUS status;

    // Step 1: Call HdmiCecOpen
    status = HdmiCecOpen(&handle);
    UT_LOG("Invoked HdmiCecOpen with output handle. Status: %d, Handle: %d", status, handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    if (status == HDMI_CEC_IO_SUCCESS)
    {
        // Generate a random logical address between 0x0 and 0xF
        srand(time(NULL));
        logicalAddress = rand() % 16;

        // Step 2: Call HdmiCecAddLogicalAddress
        status = HdmiCecAddLogicalAddress(handle, logicalAddress);
        UT_LOG("Invoked HdmiCecAddLogicalAddress with input handle and logicalAddress. Status: %d, Handle: %d, LogicalAddress: %d", status, handle, logicalAddress);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

        if (status == HDMI_CEC_IO_SUCCESS)
        {
            // Step 3: Call HdmiCecRemoveLogicalAddress
            status = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
            UT_LOG("Invoked HdmiCecRemoveLogicalAddress with input handle and logicalAddress. Status: %d, Handle: %d, LogicalAddress: %d", status, handle, logicalAddress);
            UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

            // Step 4: Call HdmiCecGetLogicalAddress
            status = HdmiCecGetLogicalAddress(handle, &logicalAddress);
            UT_LOG("Invoked HdmiCecGetLogicalAddress with input handle and output logicalAddress. Status: %d, Handle: %d, LogicalAddress: %d", status, handle, logicalAddress);
            UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
            UT_ASSERT_EQUAL(logicalAddress, 0x0F);
        }

        // Step 5: Call HdmiCecClose
        status = HdmiCecClose(handle);
        UT_LOG("Invoked HdmiCecClose with input handle. Status: %d, Handle: %d", status, handle);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    }

    UT_LOG("Exiting test_l2_hdmi_cec_driver_RemoveLogicalAddress...");
}

/**
* @brief Test the scenario where a broadcast command is sent after deleting a logical address
*
* This function tests the scenario where a broadcast command is sent after deleting a logical address. It uses the CUnit framework's `UT_ASSERT_EQUAL` macro to assert that the return values of the API calls are as expected. If an assertion fails, the function will immediately return and the remaining code will not be executed. The `UT_LOG` function is used for logging.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 004@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/
void test_l2_hdmi_cec_driver_TestBroadcastCommandAfterDeletingLogicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_TestBroadcastCommandAfterDeletingLogicalAddress...");

    int handle;
    int logicalAddresses;
    int result;
    unsigned char buf[10] = {0};
    int len = sizeof(buf);

    srand(time(NULL));
    logicalAddresses = rand() % 16; // Random value between 0 and 15

    HDMI_CEC_STATUS status = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if(status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("Invoking HdmiCecOpen failed with status: %d", status);
        return;
    }

    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if(status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("Invoking HdmiCecAddLogicalAddress failed with status: %d", status);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if(status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("Invoking HdmiCecRemoveLogicalAddress failed with status: %d", status);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecTx(handle, buf, len, &result);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SENT_FAILED);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SENT_FAILED);
    if(status != HDMI_CEC_IO_SENT_FAILED)
    {
        UT_LOG("Invoking HdmiCecTx failed with status: %d", status);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecClose(handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if(status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("Invoking HdmiCecClose failed with status: %d", status);
    }

    UT_LOG("Exiting test_l2_hdmi_cec_driver_TestBroadcastCommandAfterDeletingLogicalAddress...");
}

/**
* @brief This test checks the functionality of the HDMI CEC driver's GetPhysicalAddress API
*
* This test case is designed to validate the functionality of the GetPhysicalAddress API of the HDMI CEC driver. The test involves initializing the HDMI CEC HAL using the HdmiCecOpen() API, obtaining the physical address using the HdmiCecGetPhysicalAddress() API, validating the obtained physical address, and finally closing the HDMI CEC HAL using the HdmiCecClose() API. The test ensures that all the APIs function as expected and the physical address obtained is within the valid range.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 005@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_GetPhysicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_GetPhysicalAddress...");

    int handle;
    unsigned int physicalAddress;
    HDMI_CEC_STATUS status;

    // Step 1: Call the pre-requisite API HdmiCecOpen() to initialize the HDMI CEC HAL.
    status = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if(status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("HdmiCecOpen failed with status: %d", status);
        return;
    }

    // Step 2: Call the API HdmiCecGetPhysicalAddress() with the handle obtained from HdmiCecOpen() and a pointer to an unsigned integer to store the physical address.
    status = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    UT_LOG("Invoking HdmiCecGetPhysicalAddress with handle: %d", handle);

    // Step 3: Check the return status of HdmiCecGetPhysicalAddress(). It should return HDMI_CEC_IO_SUCCESS for a valid handle and physical address.
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if(status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("HdmiCecGetPhysicalAddress failed with status: %d", status);
        // Call the post-requisite API HdmiCecClose() with the handle obtained from HdmiCecOpen().
        HdmiCecClose(handle);
        return;
    }

    // Step 4: Validate the physical address obtained. It should be in the range of 0 to (((0x04 &0xF0 ) << 20)|( (0x04 &0x0F ) << 16) |((0x04 & 0xF0) << 4)  | (0x04 & 0x0F)).
    unsigned int maxAddress = (((0x04 &0xF0 ) << 20)|( (0x04 &0x0F ) << 16) |((0x04 & 0xF0) << 4)  | (0x04 & 0x0F));
    UT_ASSERT_TRUE(physicalAddress >= 0 && physicalAddress <= maxAddress);
    if(!(physicalAddress >= 0 && physicalAddress <= maxAddress))
    {
        UT_LOG("Physical address: %u is out of range", physicalAddress);
        // Call the post-requisite API HdmiCecClose() with the handle obtained from HdmiCecOpen().
        HdmiCecClose(handle);
        return;
    }

    // Step 5: Call the post-requisite API HdmiCecClose() with the handle obtained from HdmiCecOpen().
    status = HdmiCecClose(handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if(status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("HdmiCecClose failed with status: %d", status);
    }

    UT_LOG("Exiting test_l2_hdmi_cec_driver_GetPhysicalAddress...");
}

/**
* @brief Test the transmission of CEC command in HDMI CEC driver
*
* This test case tests the transmission of CEC command in HDMI CEC driver. It checks if the CEC command is transmitted successfully and the correct status is returned. This is important to ensure the HDMI CEC driver is working as expected.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 006@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_TransmitCECCommand(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_TransmitCECCommand...");

    int handle;
    HDMI_CEC_STATUS status;
    unsigned char cecPacket[16] = {0x0F, 0x9E, 0x00}; // CEC Version command to non-existent logical address
    int len = 3;
    int result;

    // Step 1: Call HdmiCecOpen
    status = HdmiCecOpen(&handle);
    UT_LOG("Invoking HdmiCecOpen(). Status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Step 2: Prepare a CEC packet is already done

    // Step 3: Call HdmiCecTx
    status = HdmiCecTx(handle, cecPacket, len, &result);
    UT_LOG("Invoking HdmiCecTx() with handle: %d, cecPacket: valid, len: %d. Status: %d, Result: %d", handle, len, status, result);

    // Step 4: Check the return status of HdmiCecTx
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    // Step 5: Check the send status
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    // Step 6: Call HdmiCecClose
    status = HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecClose() with handle: %d. Status: %d", handle, status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_TransmitCECCommand...");
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
    pSuite = UT_add_suite("[L2 hdmi_cec_driver]", NULL, NULL);
    if (pSuite == NULL) {
        return -1;
    }
    // List of test function names and strings

    UT_add_test( pSuite, "l2_hdmi_cec_driver_GetDefaultLogicalAddress", test_l2_hdmi_cec_driver_GetDefaultLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_AddAndGetLogicalAddress", test_l2_hdmi_cec_driver_AddAndGetLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_RemoveLogicalAddress", test_l2_hdmi_cec_driver_RemoveLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_TestBroadcastCommandAfterDeletingLogicalAddress", test_l2_hdmi_cec_driver_TestBroadcastCommandAfterDeletingLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_GetPhysicalAddress", test_l2_hdmi_cec_driver_GetPhysicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_TransmitCECCommand", test_l2_hdmi_cec_driver_TransmitCECCommand);

    return 0;
}