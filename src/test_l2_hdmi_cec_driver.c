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

/**
* @brief This test aims to verify the functionality of adding and getting logical address in HDMI CEC driver
*
* In this test, we open the HDMI CEC HAL, add a logical address, get the logical address and then close the HDMI CEC HAL. The test ensures that the add and get operations are successful and the logical address retrieved is the same as the one added.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 001@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/
void test_l2_hdmi_cec_driver_AddAndGetLogicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_AddAndGetLogicalAddress...");

    int handle;
    int logicalAddress = 0x0;
    int getLogicalAddress;
    HDMI_CEC_STATUS status;

    // Open HDMI CEC HAL
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returned %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Add Logical Address
    status = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logical address. Handle: %d, Logical Address: %d", handle, logicalAddress);
    if (status != HDMI_CEC_IO_SUCCESS) 
    {
        UT_LOG("HdmiCecAddLogicalAddress failed with status: %d", status);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    }

    // Get Logical Address
    status = HdmiCecGetLogicalAddress(handle, &getLogicalAddress);
    UT_LOG("Invoking HdmiCecGetLogicalAddress with handle. Handle: %d and returns Logical Address : %d", handle,getLogicalAddress);
    if (status != HDMI_CEC_IO_SUCCESS || getLogicalAddress != logicalAddress) 
    {
        UT_LOG("HdmiCecGetLogicalAddress failed with status: %d, Logical Address: %d", status, getLogicalAddress);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
        UT_ASSERT_EQUAL(getLogicalAddress, logicalAddress);
    }

    // Close HDMI CEC HAL
    status = HdmiCecClose(handle);
    UT_LOG(" HdmiCecClose returned %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_AddAndGetLogicalAddress...");
}

/**
* @brief This test validates the logical address of the HDMI CEC driver
*
* This test aims to validate the logical address of the HDMI CEC driver. It checks if the logical address is not 0 or 14, which are invalid addresses. If the address is invalid, the test fails.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 002@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_ValidateLogicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_ValidateLogicalAddress...");

    int handle;
    int logicalAddresses = 0x1;
    HDMI_CEC_STATUS status;

    // Pre-requisite API
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returned %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Test API
    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses. Handle: %d, LogicalAddresses: %d and api returns:%d", handle, logicalAddresses, status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT);

    // Post-requisite API
    status = HdmiCecClose(handle);
    UT_LOG(" HdmiCecClose returned %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_ValidateLogicalAddress...");
}

/**
* @brief This test checks the logical address of the HDMI CEC driver
*
* This test function tests the logical address handling of the HDMI CEC driver. It opens the HDMI CEC HAL, adds a logical address, removes it, gets the logical address, and finally closes the HDMI CEC HAL. The test is designed to ensure that the HDMI CEC driver correctly handles logical addresses.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 003@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_checkLogicalAddress(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_checkLogicalAddress...");

    int handle;
    int logicalAddress = 0x0;
    HDMI_CEC_STATUS status;

    // Open HDMI CEC HAL
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returned %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Add Logical Address
    status = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle=%d, logicalAddress=%d. Status=%d", handle, logicalAddress, status);    
    if (status != HDMI_CEC_IO_SUCCESS) 
    {
        UT_LOG("Test case failed at HdmiCecAddLogicalAddress");
        UT_FAIL("Test case failed at HdmiCecAddLogicalAddress");
    }

    // Remove Logical Address
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with handle=%d, logicalAddress=%d. Status=%d", handle, logicalAddress, status);
    if (status != HDMI_CEC_IO_SUCCESS) 
    {
        UT_LOG("Test case failed at HdmiCecRemoveLogicalAddress");
        UT_FAIL("Test case failed at HdmiCecRemoveLogicalAddress");
    }

    // Get Logical Address
    status = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("Invoking HdmiCecGetLogicalAddress with handle=%d. Status=%d, logicalAddress=%d", handle, status, logicalAddress);
    if (status != HDMI_CEC_IO_SUCCESS || logicalAddress != 0x0F) 
    {
        UT_LOG("Test case failed at HdmiCecGetLogicalAddress");
        UT_FAIL("Test case failed at HdmiCecGetLogicalAddress");
    }

    // Close HDMI CEC HAL
    status = HdmiCecClose(handle);
    UT_LOG(" HdmiCecClose returned %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_checkLogicalAddress...");
}

/**
* @brief This test validates the HAL transmission after address removal in HDMI CEC driver
*
* This test case is designed to validate the correct functionality of the HDMI CEC driver when a logical address is removed. The test ensures that the driver can still transmit correctly after an address removal operation. This is critical to ensure the robustness of the driver in dynamic HDMI CEC network where devices can be added or removed.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 004@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_ValidateHALTransmissionAfterAddressRemoval(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_ValidateHALTransmissionAfterAddressRemoval...");

    int handle;
    int logicalAddresses = 0x0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    int len = 1;
    int result;
    HDMI_CEC_STATUS status ;
    
    status = HdmiCecOpen(&handle);
    UT_LOG("Invoking HdmiCecOpen with handle. Status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    status = HdmiCecSetRxCallback(handle, NULL, NULL);
    UT_LOG("Invoking HdmiCecSetRxCallback with handle. Status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses. Status: %d", status);
    if (status != HDMI_CEC_IO_SUCCESS) 
    {
        UT_LOG("Test Case Fail: Add Address. Status: %d", status);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    }

    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with handle and logicalAddresses. Status: %d", status);
    if (status != HDMI_CEC_IO_SUCCESS) 
    {
        UT_LOG("Test Case Fail: Remove Address. Status: %d", status);
        UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    }

    status = HdmiCecTx(handle, buf, len, &result);
    UT_LOG("Invoking HdmiCecTx with handle, buf, len and result. Status: %d", status);
    if (status != HDMI_CEC_IO_SUCCESS) 
    {
        UT_LOG("Test Case Pass: Broadcast Command. Status: %d", status);
        UT_PASS("Test case Pass. Broadcasting should not succeed after deleting logical address.");
    }

    status = HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecClose with handle. Status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l2_hdmi_cec_driver_ValidateHALTransmissionAfterAddressRemoval...");
}

/**
* @brief This test verifies the physical address allocation in the HDMI CEC driver
*
* In this test, the HDMI CEC driver's ability to allocate and retrieve a valid physical address is tested. The test involves opening a connection, retrieving the physical address, validating the address, and then closing the connection. The test ensures that the driver can correctly manage physical addresses, which is crucial for the correct operation of the HDMI CEC protocol.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 005@n
*
* **Test Procedure:**
* Refer to UT specification documentation [l2_hdmi_cec_driver_test_specification.md](l2_hdmi_cec_driver_test_specification.md)
*/

void test_l2_hdmi_cec_driver_VerifyPhysicalAddressAllocation(void)
{
    UT_LOG("Entering test_l2_hdmi_cec_driver_VerifyPhysicalAddressAllocation...");

    int handle;
    unsigned int physicalAddress;
    HDMI_CEC_STATUS status;
    unsigned int max_address = ((0x04 & 0xF0) << 20) | ((0x04 & 0x0F) << 16) | ((0x04 & 0xF0) << 4) | (0x04 & 0x0F);

    // Call HdmiCecOpen
    status = HdmiCecOpen(&handle);
    UT_LOG("Invoking HdmiCecOpen with handle. Status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    // Call HdmiCecGetPhysicalAddress
    status = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    UT_LOG("Invoking HdmiCecGetPhysicalAddress with handle :%d and Status: %d", handle, status);
    if (status == HDMI_CEC_IO_SUCCESS)
    {
        // Check physical address range
        if (physicalAddress >= 0 && physicalAddress <= max_address)
        {
            UT_LOG("HdmiCecGetPhysicalAddress returned valid physical address: %u", physicalAddress);
            UT_PASS("Test case Pass");
        }
        else
        {
            UT_LOG("HdmiCecGetPhysicalAddress returned invalid physical address: %u", physicalAddress);
            UT_FAIL("Test case Fail");
        }
    }

    // Call HdmiCecClose
    status = HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecClose with handle. Status: %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    if (status != HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("HdmiCecClose failed with status: %d", status);
    }

    UT_LOG("Exiting test_l2_hdmi_cec_driver_VerifyPhysicalAddressAllocation...");
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

    UT_add_test( pSuite, "l2_hdmi_cec_driver_AddAndGetLogicalAddress", test_l2_hdmi_cec_driver_AddAndGetLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_ValidateLogicalAddress", test_l2_hdmi_cec_driver_ValidateLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_checkLogicalAddress", test_l2_hdmi_cec_driver_checkLogicalAddress);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_ValidateHALTransmissionAfterAddressRemoval", test_l2_hdmi_cec_driver_ValidateHALTransmissionAfterAddressRemoval);
    UT_add_test( pSuite, "l2_hdmi_cec_driver_VerifyPhysicalAddressAllocation", test_l2_hdmi_cec_driver_VerifyPhysicalAddressAllocation);

    return 0;
}
