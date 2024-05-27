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
* Ref to API Definition specification documentation : [hdmi-cec_halSpec.md](../../docs/pages/hdmi-cec_halSpec.md)
*/

#include <ut.h>
#include <ut_log.h>
#include "hdmi_cec_driver.h"

static int gTestGroup = 2;
static int gTestID = 1;

/**
* @brief Test for getting the default logical address of the HDMI CEC driver
*
* This test case validates the functionality of getting the default logical address of the HDMI CEC driver. It first opens a connection to the HDMI CEC driver, then retrieves the logical address, checks if the address is correct, and finally closes the connection. This test is important to ensure that the HDMI CEC driver is correctly assigning the default logical address.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 001@n
*
* **Test Procedure:**
* Refer to UT specification documentation [hdmi_cec_driver_L2_Low-Level_TestSpecification.md](../../docs/pages/hdmi_cec_driver_L2_Low-Level_TestSpecification.md)
*/

void test_l2_hdmi_cec_driver_GetDefaultLogicalAddress(void)
{
    gTestID = 1;
    int handle = 0;
    int logicalAddress = 0;
    HDMI_CEC_STATUS status;

    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    // Step 1: Call the pre-requisite API HdmiCecOpen()
    UT_LOG_DEBUG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_NOT_EQUAL_FATAL(handle, 0);

    // Step 2: Call the API HdmiCecGetLogicalAddress()
    UT_LOG_DEBUG("Invoking HdmiCecGetLogicalAddress with handle: %d", handle);
    status = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecGetLogicalAddress failed with status: %d", status);
        HdmiCecClose(handle); // Cleanup if test fails
        return;
    }
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Step 3: Check the logical address
    UT_LOG_DEBUG("Checking logical address: %d", logicalAddress);
    UT_ASSERT_EQUAL(logicalAddress, 0x0F);

    // Step 4: Call the post-requisite API HdmiCecClose()
    UT_LOG_DEBUG("Invoking HdmiCecClose with handle: %d", handle);
    status = HdmiCecClose(handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief This test checks the functionality of adding and getting logical addresses in HDMI CEC driver
*
* This test case is designed to validate the correct operation of the HdmiCecAddLogicalAddress and HdmiCecGetLogicalAddress APIs. It does this by first opening a handle to the HDMI CEC driver, then adding logical addresses to it, and finally retrieving those addresses to verify they were correctly added. The test ensures that the APIs return the expected status codes and that the retrieved addresses match the ones that were added.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 002@n
*
* **Test Procedure:**
* Refer to UT specification documentation [hdmi_cec_driver_L2_Low-Level_TestSpecification.md](../../docs/pages/hdmi_cec_driver_L2_Low-Level_TestSpecification.md)
*/
void test_l2_hdmi_cec_driver_AddAndGetLogicalAddress(void)
{
    gTestID = 2;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int handle;
    HDMI_CEC_STATUS status;
    int logicalAddress;

    // Step 1: Call the pre-requisite API HdmiCecOpen
    status = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    UT_LOG_DEBUG("Invoking HdmiCecOpen with valid handle");

    // Step 2: Call the API HdmiCecAddLogicalAddress and HdmiCecGetLogicalAddress for each logical address
    for (int i = 0x00; i <= 0x0F; i++)
    {
        status = HdmiCecAddLogicalAddress(handle, i);
        UT_LOG_DEBUG("Invoking HdmiCecAddLogicalAddress with handle: %d and logicalAddress: %d", handle, i);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

        status = HdmiCecGetLogicalAddress(handle, &logicalAddress);
        UT_LOG_DEBUG("Invoking HdmiCecGetLogicalAddress with handle: %d and logicalAddress: %d", handle, logicalAddress);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
        UT_ASSERT_EQUAL(logicalAddress, i);

        status = HdmiCecRemoveLogicalAddress(handle, i);
        UT_LOG_DEBUG("Invoking HdmiCecRemoveLogicalAddress with handle: %d and logicalAddress: %d", handle, i);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    }

    // Step 4: Call the post-requisite API HdmiCecClose
    status = HdmiCecClose(handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    UT_LOG_DEBUG("Invoking HdmiCecClose with handle: %d", handle);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief Test the removal of a logical address in the HDMI CEC driver
*
* This function tests the removal of a logical address in the HDMI CEC driver. It first opens the driver, adds a logical address, removes the logical address, checks that the logical address has been removed, and finally closes the driver.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 003@n
*
* **Test Procedure:**
* Refer to UT specification documentation [hdmi_cec_driver_L2_Low-Level_TestSpecification.md](../../docs/pages/hdmi_cec_driver_L2_Low-Level_TestSpecification.md)
*/

void test_l2_hdmi_cec_driver_RemoveLogicalAddress(void)
{
    gTestID = 3;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int handle;
    int logicalAddress = 0x00; // valid logical address
    HDMI_CEC_STATUS status;
    int getLogicaladdress;

    // Call the pre-requisite API HdmiCecOpen
    status = HdmiCecOpen(&handle);
    UT_LOG_DEBUG("Invoking HdmiCecOpen with handle: %d", handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);

    // Invoke the API HdmiCecAddLogicalAddress
    status = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_LOG_DEBUG("Invoking HdmiCecAddLogicalAddress with handle: %d and logicalAddress: %d", handle, logicalAddress);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Invoke the API HdmiCecGetLogicalAddress
    status = HdmiCecGetLogicalAddress(handle, &getLogicaladdress);
    UT_LOG_DEBUG("Invoking HdmiCecGetLogicalAddress with handle: %d", handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL(logicalAddress, getLogicaladdress);

    // Invoke the API HdmiCecRemoveLogicalAddress
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_LOG_DEBUG("Invoking HdmiCecRemoveLogicalAddress with handle: %d and logicalAddress: %d", handle, logicalAddress);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Invoke the API HdmiCecGetLogicalAddress
    status = HdmiCecGetLogicalAddress(handle, &getLogicaladdress);
    UT_LOG_DEBUG("Invoking HdmiCecGetLogicalAddress with handle: %d", handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL(getLogicaladdress, 0x0F);

    // Call the post-requisite API HdmiCecClose
    status = HdmiCecClose(handle);
    UT_LOG_DEBUG("Invoking HdmiCecClose with handle: %d", handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief Test for broadcasting HDMI CEC command
*
* This function tests the HDMI CEC driver's ability to broadcast a HDMI CEC command when logical address is removed. It first opens the HDMI CEC HAL, adds a logical address, removes the logical address, sends a broadcast HDMI CEC command, and finally closes the HDMI CEC HAL.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 004@n
*
* **Test Procedure:**
* Refer to UT specification documentation [hdmi_cec_driver_L2_Low-Level_TestSpecification.md](../../docs/pages/hdmi_cec_driver_L2_Low-Level_TestSpecification.md)
*/

void test_l2_hdmi_cec_driver_BroadcastHdmiCecCommand(void)
{
    gTestID = 4;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int handle;
    int logicalAddresses = 0x0; // valid logical address
    unsigned char buf[] = {0x0F, 0x84, 0x00, 0x00}; // valid CEC message
    int len = sizeof(buf);
    int result;

    HDMI_CEC_STATUS status = HdmiCecOpen(&handle);
    UT_LOG_DEBUG("Invoking HdmiCecOpen with handle: %d", handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecOpen failed with status: %d\n", status);
        return;
    }

    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG_DEBUG("Invoking HdmiCecAddLogicalAddress with handle: %d and logicalAddress: %d", handle, logicalAddresses);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecAddLogicalAddress failed with status: %d\n", status);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG_DEBUG("Invoking HdmiCecRemoveLogicalAddress with handle: %d and logicalAddress: %d", handle, logicalAddresses);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecRemoveLogicalAddress failed with status: %d\n", status);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecTx(handle, buf, len, &result);
    UT_LOG_DEBUG("Invoking HdmiCecTx with handle: %d", handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecTx failed with status: %d\n", status);
        HdmiCecClose(handle);
        return;
    }

    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SENT_AND_ACKD);
    if (result != HDMI_CEC_IO_SENT_AND_ACKD)
    {
        UT_LOG_ERROR("HdmiCecTx result: %d\n", result);
    }

    status = HdmiCecClose(handle);
    UT_LOG_DEBUG("Invoking HdmiCecClose with handle: %d", handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecClose failed with status: %d\n", status);
    }

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief Test to verify the physical address of the HDMI CEC driver
*
* This test case verifies the physical address of the HDMI CEC driver by calling the HdmiCecGetPhysicalAddress() API. The test ensures that the physical address obtained is less than F.F.F.F. The test also includes error handling and assertions to check the return values of the APIs. The test ensures that the post-requisite API HdmiCecClose() is called in case of any test failure after the successful execution of the pre-requisite API HdmiCecOpen().
*
* **Test Group ID:** 02@n
* **Test Case ID:** 005@n
*
* **Test Procedure:**
* Refer to UT specification documentation [hdmi_cec_driver_L2_Low-Level_TestSpecification.md](../../docs/pages/hdmi_cec_driver_L2_Low-Level_TestSpecification.md)
*/

void test_l2_hdmi_cec_driver_VerifyPhysicalAddress(void)
{
    gTestID = 5;
    int handle;
    unsigned int physicalAddress;
    HDMI_CEC_STATUS status;

    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    // Step 1: Call the pre-requisite API HdmiCecOpen()
    UT_LOG_DEBUG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);

    // Step 2: Call the API HdmiCecGetPhysicalAddress()
    UT_LOG_DEBUG("Invoking HdmiCecGetPhysicalAddress with handle: %d", handle);
    status = HdmiCecGetPhysicalAddress(handle, &physicalAddress);

    // Step 3: Check the return status of HdmiCecGetPhysicalAddress()
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecGetPhysicalAddress failed with status: %d", status);
        status = HdmiCecClose(handle); // Closing the handle as the test failed
        UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    }

    // Step 4: Verify that the physical address obtained is less than F.F.F.F
    UT_LOG_DEBUG("Checking if physical address: %u is less than F.F.F.F", physicalAddress);
    UT_ASSERT_TRUE(physicalAddress < 0xFFFF);

    // Step 5: Call the post-requisite API HdmiCecClose()
    UT_LOG_DEBUG("Invoking HdmiCecClose with handle: %d", handle);
    status = HdmiCecClose(handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief This test function is designed to test the transmission of CEC commands in the HDMI CEC driver.
*
* This test function tests the transmission of CEC commands in the HDMI CEC driver. It opens a connection, adds a logical address, sends a CEC command, removes the logical address, and then closes the connection. The test asserts that each operation returns the expected status.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 006@n
*
* **Test Procedure:**
* Refer to UT specification documentation [hdmi_cec_driver_L2_Low-Level_TestSpecification.md](../../docs/pages/hdmi_cec_driver_L2_Low-Level_TestSpecification.md)
*/

void test_l2_hdmi_cec_driver_TransmitCECCommand(void)
{
    gTestID = 6;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int handle;
    int logicalAddresses = 0x4; // Example logical address
    unsigned char buf[] = {0x47, 0x9F}; // Example CEC message
    int len = sizeof(buf);
    int result;

    HDMI_CEC_STATUS status = HdmiCecOpen(&handle);
    UT_LOG_DEBUG("Invoking HdmiCecOpen with valid handle");
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecOpen failed with status: %d\n", status);
        return;
    }

    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG_DEBUG("Invoking HdmiCecAddLogicalAddress with handle: %d and logicalAddress: %d", handle, logicalAddresses);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecAddLogicalAddress failed with status: %d\n", status);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecTx(handle, buf, len, &result);
    UT_LOG_DEBUG("Invoking HdmiCecTx with handle: %d", handle);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);
    if (status != HDMI_CEC_IO_SENT_BUT_NOT_ACKD)
    {
        UT_LOG_ERROR("HdmiCecTx failed with status: %d\n", status);
        HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG_DEBUG("Invoking HdmiCecRemoveLogicalAddress with handle: %d and logicalAddress: %d", handle, logicalAddresses);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecRemoveLogicalAddress failed with status: %d\n", status);
        HdmiCecClose(handle);
        return;
    }

    status = HdmiCecClose(handle);
    UT_LOG_DEBUG("Invoking HdmiCecClose with handle: %d", handle);
    UT_ASSERT_EQUAL_FATAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG_ERROR("HdmiCecClose failed with status: %d\n", status);
    }

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
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
    if (pSuite == NULL)
    {
        return -1;
    }
    // List of test function names and strings

    UT_add_test( pSuite, "l2_hdmi_cec_driver_GetDefaultLogicalAddress", test_l2_hdmi_cec_driver_GetDefaultLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_AddAndGetLogicalAddress", test_l2_hdmi_cec_driver_AddAndGetLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_RemoveLogicalAddress", test_l2_hdmi_cec_driver_RemoveLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_BroadcastHdmiCecCommand", test_l2_hdmi_cec_driver_BroadcastHdmiCecCommand);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_VerifyPhysicalAddress", test_l2_hdmi_cec_driver_VerifyPhysicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_TransmitCECCommand", test_l2_hdmi_cec_driver_TransmitCECCommand);

    return 0;
}