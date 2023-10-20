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
* @file test_hdmi_cec_driver.c
* @page hdmi_cec_driver Level 1 Tests
*
* ## Module's Role
* This module includes Level 1 functional tests (success and failure scenarios).
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

#define CEC_GET_CEC_VERSION (0x9F)
#define CEC_DEVICE_VENDOR_ID (0x87)

// For source
#define __STB__ 1

/**
 * @brief hdmicec receive message callback
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf receive message buffer passed
 * @param len receive message buffer length
 */
void DriverReceiveCallback(int handle, void *callbackData, unsigned char *buf, int len)
{
    int index = 0;
    UT_LOG("Buffer generated: %x length: %d",buf, len);
    UT_ASSERT_TRUE(len>0); 
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NOT_NULL(callbackData);
    UT_ASSERT_PTR_NOT_NULL(buf);
    UT_LOG("Call back data generated is ");
    for (index=0; index < len; index++) 
    {
        UT_LOG("buf at index : %d is %x", index, buf[index]);
    }
}

/**
 * @brief hdmicec transmit message callback
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf transmit message buffer passed
 * @param result transmit message result
 */
void DriverTransmitCallback(int handle, void *callbackData, int result)
{
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL(((unsigned long long)(!callbackData)));
    UT_LOG("callbackData returned: %x result: %d",callbackData, result);
    if (HDMI_CEC_IO_SUCCESS != result) 
    { 
        UT_FAIL ("Check failed"); 
    }
}

/**
* @brief This test ensures the correct functionality of the HdmiCecOpen() function 
*
* The HdmiCecOpen() function of the HDMI CEC driver is being tested by invoking it with a valid handle and checking if the status returned is HDMI_CEC_IO_SUCCESS.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 001@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
*
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecOpen() function with a valid handle | handle to be opened = &handle | Returned status should be HDMI_CEC_IO_SUCCESS | Test execution should be successful |
*/ 
void test_l1_hdmi_cec_driver_positive1_HdmiCecOpen(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecOpen...");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with a valid handle.");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with a valid handle.");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecOpen...");
}

/**
* @brief Tests for a failure response when trying to open an already opened HDMI CEC driver
*
* This test verifies that the API HdmiCecOpen returns the expected failure response if invoked twice consecutively, i.e., it checks that the CEC driver cannot be opened twice.
* 
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 002@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data | Expected Result | Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecOpen API first time | handle (an integer pointer) | The function is supposed to open the CEC driver and return success status. | Should be successful |
* | 02 | Invoke HdmiCecOpen API second time | handle (an integer pointer) | The function should fail and return "HDMI_CEC_IO_ALREADY_OPEN", as the driver is already opened. | Should return "HDMI_CEC_IO_ALREADY_OPEN" |
*/

void test_l1_hdmi_cec_driver_negative1_HdmiCecOpen (void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecOpen...");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen first time");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns %d", status);  
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecOpen twice consecutively");  
    status = HdmiCecOpen(&handle); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_ALREADY_OPEN);
    UT_LOG("HdmiCecOpen returns %d", status);

    UT_LOG("Invoking HdmiCecClose with a valid handle.");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecOpen...");
}

/**
* @brief Testing the behavior of the HdmiCecOpen function when invoked with a NULL argument
*
* This test case is designed to evaluate the robustness of the HdmiCecOpen API when it is presented with an invalid argument (@NULL). In particular, this test will validate if the function correctly identifies the invalid input and returns the appropriate error code.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 003@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invocation of HdmiCecOpen API with NULL handle | handle = NULL | The expected result is an error code denoting invalid argument | Should handle invalid argument gracefully |
*/

void test_l1_hdmi_cec_driver_negative2_HdmiCecOpen( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecOpen...");
    int *handle = NULL;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with NULL handle.");
    status = HdmiCecOpen(handle);
    UT_LOG("HdmiCecOpen returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecOpen...");
}

/**
* @brief Unit test for the function HdmiCecOpen
*
* This test case verifies that opening a HDMI CEC connection thrice consecutively will be successful.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 006@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console 
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecOpen function first time | handle = uninitialized | Function call should return HDMI_CEC_IO_SUCCESS | Function call should be successful |
* | 02 | Invoke HdmiCecOpen function second time | handle = value returned from previous call | Function call should return HDMI_CEC_IO_SUCCESS | Function call should be successful |
* | 03 | Invoke HdmiCecOpen function third time | handle = value returned from previous calls | Function call should return HDMI_CEC_IO_SUCCESS | Function call should be successful |
*/
void test_l1_hdmi_cec_driver_positive2_HdmiCecOpen( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive2_HdmiCecOpen...");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with a valid handle.");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecOpen with a valid handle.");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecOpen thrice consecutively.");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with a valid handle.");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive2_HdmiCecOpen...");
}

/**
* @brief Validate the successful closure of an HDMI CEC driver
*
* This test case aims to validate the scenario when HdmiCecClose API is invoked with a valid handle. The priority of this test case is high because the ability to successfully close the driver is essential to the basic functionality of the HDMI CEC system.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 008@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecClose() with a valid handle | handle = valid value | HDMI_CEC_IO_SUCCESS is returned | The handle for this test is assumed as valid. Successful closure is indicated by HDMI_CEC_IO_SUCCESS.|
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecClose(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecClose...");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle.");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecClose...");
}

/**
* @brief Test that the HDMI CEC driver can successfully close a previously open handle
*
* This test aims to verify that the HDMI CEC driver API, HdmiCecClose(), can successfully close a valid handle, and returns a success status upon a second close call on the same handle.@n
*
* **Test Group ID:** Basic: 01
* **Test Case ID:** 009
* **Priority:** High
* @n
* **Pre-Conditions:** None
* **Dependencies:** None
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call HdmiCecClose() with a valid handle| handle = 1 | HDMI_CEC_IO_SUCCESS | The handle has been successfully closed |
* | 02 | Call HdmiCecClose() again with the same handle| handle = 1 | HDMI_CEC_IO_SUCCESS | The call should still be successful even though the handle was already closed |
*/
    
void test_l1_hdmi_cec_driver_positive2_HdmiCecClose(void) 
{        
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive2_HdmiCecClose...");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecClose twice with the same valid handle.");  
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive2_HdmiCecClose...");
}

/**
 * @brief To test the failure case for the function HdmiCecClose.
 *
 * In this test, the function HdmiCecClose is invoked with an invalid handle to test its behavior and to ensure it does not cause any crashes and returns the expected error code.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 010@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoking the function HdmiCecClose with an invalid handle. | handle = 0 | The function should fail gracefully and return the error code HDMI_CEC_IO_INVALID_HANDLE. | This ensures that the test protects against invalid handle input. |
 */
void test_l1_hdmi_cec_driver_negative1_HdmiCecClose(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecClose...");
    int handle = 0;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    handle = 0;
    UT_LOG("Invoking HdmiCecClose with zero handle.");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecClose...");
}

/**
* @brief This test checks the HDMI CEC driver's behavior when HdmiCecClose() is called without a preceding HdmiCecOpen() call.
*
* The aim is to validate that the HDMI CEC driver functionality is robust and handles erroneous API usage correctly. This test verifies the driver's error handling by purposely evoking HdmiCecClose with a handle, but without a preceding call to the HdmiCecOpen function. The driver should return an error status indicating that the IO is not opened.@n
* 
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 011@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call HdmiCecClose without preceding call to HdmiCecOpen | handle=2 | HDMI_CEC_IO_NOT_OPENED | Should return error indicating IO not opened |
*/
void test_l1_hdmi_cec_driver_negative2_HdmiCecClose(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecClose...");
    int handle = 1;
    HDMI_CEC_STATUS status; 

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecClose without preceding call to HdmiCecOpen().");
    status = HdmiCecClose(handle);
    UT_LOG(" HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecClose...");
}

/**
 * @brief This unit test is for validating the functionality of the method HdmiCecAddLogicalAddress
 *
 * This test checks if the function HdmiCecAddLogicalAddress successfully adds a logical address to the HDMI CEC driver when the input parameters are valid@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 012@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Opening a Handle to HDMI CEC | - | An integer being the handle is returned, implying a successful opening of a handle | Should open a handle successfully |
 * | 02 | Adding Logical Address to HDMI CEC | handle = 0x0, logicalAddresses = 0x0 | Return value should be equal to HDMI_CEC_IO_SUCCESS | The return value being HDMI_CEC_IO_SUCCESS implies a successful adding of logical address |
 */
void test_l1_hdmi_cec_driver_positive1_HdmiCecAddLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecAddLogicalAddress_sink...");
    int handle;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", ret); 
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecAddLogicalAddress_sink...\n");
}

/**
* @brief Test negative cases for function HdmiCecAddLogicalAddress
*
* In this test, we are checking the behavior of the HdmiCecAddLogicalAddress function when called without an active HdmiCecOpen session.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 013@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call function HdmiCecAddLogicalAddress without invoking HdmiCecOpen. | handle = 0, logicalAddresses = 0x0 | Return value should be equal to HDMI_CEC_IO_NOT_OPENED | Function should not operate without a successful call to HdmiCecOpen |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_sink...");
    int handle = 1;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecAddLogicalAddress without calling HdmiCecOpen().");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG(" HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_sink...\n");
}

/**
* @brief API Testing of HdmiCecAddLogicalAddress with negative conditions
*
* This test executes the HdmiCecAddLogicalAddress API with incorrect values for handle and logicalAddresses to ensure the API behaves correctly in negative conditions. 
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 014@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data  |Expected Result |Notes|
* | :----: | ---------  | ----------|--------------| -----|
* | 01 | Call HdmiCecAddLogicalAddress with invalid handle| handle = -1, logicalAddresses = 0x0  | HDMI_CEC_IO_INVALID_HANDLE  | Should behave as expected when given incorrect values |
*/
void test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_sink...\n");
    int handle;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    handle = -1;
    UT_LOG("Invoking HdmiCecAddLogicalAddress with invalid handle.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_sink...\n");
}

/**
* @brief Test to verify the behavior of the HdmiCecAddLogicalAddress API with invalid input
* 
* This test case validates the HdmiCecAddLogicalAddress API's handling of an input error.@n When the logicalAddresses parameter is set to a non-default value, the API should return an INVALID_ARGUMENT error.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 015@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Open the HDMI CEC interface. Check the return handle before proceeding. | handle = HdmiCecOpen() | Handle should be valid | |
* | 02 | Invoke HdmiCecAddLogicalAddress API with invalid logicalAddresses set to 0x1 | logicalAddresses = 0x1 | The API should return an INVALID_ARGUMENT error | |
* | 03 | Check the return status. | ret = HdmiCecAddLogicalAddress(handle, logicalAddresses) | ret should be equal to HDMI_CEC_IO_INVALID_ARGUMENT | This case ensures that the API behaves correctly when sending a non-default value. |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_sink...\n");
    int handle;
    int logicalAddresses = 0x1;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with logicalAddresses set to a value other than 0x0.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_sink...\n");
}

/**
* @brief The purpose of this test is to verify that the 'HdmiCecAddLogicalAddress' function does not function after 'HdmiCecClose' is called.
*
* In this test, the call to 'HdmiCecClose()' closes the HDMI channel and immediately after that,  'HdmiCecAddLogicalAddress' is invoked to attempt to add a logical address, which is expected to fail.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:**  016@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Open HDMI CEC channel then close it | handle = HDMI_CEC channel ID, logicalAddresses = 0x0 | HDMI_CEC channel ID is generated | No error during opening and closing |
* | 02 | Call HdmiCecAddLogicalAddress API after closing HDMI CEC channel | handle = HDMI_CEC channel ID closed in the previous step, logicalAddresses = 0x0 | HDMI_CEC_IO_NOT_OPENED error code| HDMI CEC channel was already closed |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_sink...\n"); 
    int handle;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress after calling HdmiCecClose(). ");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_sink...\n");
}

/**
 * @brief This unit test is for validating the functionality of the method HdmiCecAddLogicalAddress
 *
 * This test checks if the function HdmiCecAddLogicalAddress successfully adds a logical address to the HDMI CEC driver when the input parameters are valid@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 017@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Opening a Handle to HDMI CEC | - | An integer being the handle is returned, implying a successful opening of a handle | Should open a handle successfully |
 * | 02 | Invoke HdmiCecAddLogicalAddress HDMI CEC for source device | handle = 0x0, logicalAddresses = 3 | HDMI_CEC_IO_INVALID_ARGUMENT | Should return error |
 */
void test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_source...");
    int handle;
    int logicalAddresses = 3;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with valid handle and logicalAddresses .");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", ret); 
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_source...\n");
}

/**
* @brief Test negative cases for function HdmiCecAddLogicalAddress
*
* In this test, we are checking the behavior of the HdmiCecAddLogicalAddress function when called without an active HdmiCecOpen session.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 018@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call function HdmiCecAddLogicalAddress without invoking HdmiCecOpen. | handle = 0, logicalAddresses = 0x0 | Return value should be equal to HDMI_CEC_IO_NOT_OPENED | Function should not operate without a successful call to HdmiCecOpen |
*/

void test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_source...");
    int handle = 0;
    int logicalAddresses = 3;
    HDMI_CEC_STATUS ret;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecAddLogicalAddress without calling HdmiCecOpen().");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG(" HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_source...\n");
}

/**
* @brief API Testing of HdmiCecAddLogicalAddress with negative conditions
*
* This test executes the HdmiCecAddLogicalAddress API with incorrect values for handle and logicalAddresses to ensure the API behaves correctly in negative conditions. 
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 019@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description  | Test Data |Expected Result|Notes|
* | :----:  | --------- | ---------- |-------------- | -----|
* | 01 | Call HdmiCecAddLogicalAddress with invalid values | handle = -1, logicalAddresses = 0x0 | HDMI_CEC_IO_INVALID_HANDLE | Should behave as expected when given incorrect values |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_source...\n");
    int handle ;
    int logicalAddresses = 3;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    handle = -1;
    UT_LOG("Invoking HdmiCecAddLogicalAddress with invalid handle.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_source...\n");
}

/**
* @brief Test to verify the behavior of the HdmiCecAddLogicalAddress API with invalid input
* 
* This test case validates the HdmiCecAddLogicalAddress API's handling of an input error.@n When the logicalAddresses parameter is set to a non-default value, the API should return an INVALID_ARGUMENT error.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 020@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Open the HDMI CEC interface. Check the return handle before proceeding. | handle = HdmiCecOpen() | Handle should be valid | |
* | 02 | Invoke HdmiCecAddLogicalAddress API with  invalid source logicalAddresses set to 0x0 | logicalAddresses = 0x0 | ret should be equal to HDMI_CEC_IO_INVALID_ARGUMENT | This case ensures that the API behaves correctly when sending a non-default value. |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_source...\n");
    int handle;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with logicalAddresses set to invalid logical address.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_source...\n");
}

/**
* @brief The purpose of this test is to verify that the 'HdmiCecAddLogicalAddress' function does not function after 'HdmiCecClose' is called.
*
* In this test, the call to 'HdmiCecClose()' closes the HDMI channel and immediately after that,  'HdmiCecAddLogicalAddress' is invoked to attempt to add a logical address, which is expected to fail.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:**  021@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Open HDMI CEC channel then close it | handle = HDMI_CEC channel ID, logicalAddresses = 0x0 | HDMI_CEC channel ID is generated | No error during opening and closing |
* | 02 | Call HdmiCecAddLogicalAddress API after closing HDMI CEC channel | handle = HDMI_CEC channel ID closed in the previous step, logicalAddresses = 0x0 | HDMI_CEC_IO_NOT_OPENED error code| HDMI CEC channel was already closed |
*/
void test_l1_hdmi_cec_driver_negative5_HdmiCecAddLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative5_HdmiCecAddLogicalAddress_source...\n"); 
    int handle;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress after calling HdmiCecClose(). ");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecAddLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative5_HdmiCecAddLogicalAddress_source...\n");
}

/**
* @brief Test positive cases of HdmiCecRemoveLogicalAddress function
*
* This unit test checks if HdmiCecRemoveLogicalAddress function works properly with correct handle and logicalAddresses@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 022@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecRemoveLogicalAddress with valid handle and logicalAddresses | handle = 1 , logicalAddresses = 0x0 | Return value should be HDMI_CEC_IO_SUCCESS, indicating success in removing logical address | Should be successful |
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecRemoveLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecRemoveLogicalAddress_sink...\n");
    int handle = 1; 
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", ret); 
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with valid handle and logicalAddresses = 0x0");
    ret = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecRemoveLogicalAddress_sink...\n");
}

/**
 * @brief This test checks if the HDMI CEC driver correctly handles removal of a logical address that has been already removed
 *
 * We test the behavior of the HDMI CEC driver when attempting to remove a logical address from its list of valid addresses. Specifically, we want to see if the driver can correctly report that the address is already removed when the method HdmiCecRemoveLogicalAddress is invoked twice with the same address.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 023@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke HdmiCecRemoveLogicalAddress method with valid handle and logicalAddresses set to 0x0 | handle = 1, logicalAddresses = 0x0 | Return status HDMI_CEC_IO_ALREADY_REMOVED, implying successful removal | This should be successful |
 */
void test_l1_hdmi_cec_driver_positive2_HdmiCecRemoveLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive2_HdmiCecRemoveLogicalAddress_sink...\n");
    int handle; 
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with valid handle and logicalAddresses = 0x0");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress twice with valid handle and logicalAddresses = 0x0. ");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_ALREADY_REMOVED);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive2_HdmiCecRemoveLogicalAddress_sink...\n");
}

/**
* @brief Unit test to verify the behavior of 'HdmiCecRemoveLogicalAddress' function
*
* This unit test is designed to verify the correctness of the 'HdmiCecRemoveLogicalAddress' function. It ensures that the function can handle the removal of a logical address in the HDMI CEC driver and responds as expected when provided with invalid logical address.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 024@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console. @n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoking HdmiCecRemoveLogicalAddress with valid handle and logicalAddresses not equal to 0x0 | handle = 1, logicalAddresses = 0x1 | should be HDMI_CEC_IO_INVALID_ARGUMENT | Should return error |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_sink...\n");
    int handle;
    int logicalAddresses = 0x1;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with valid handle and logicalAddresses not equal to 0x0.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_sink...\n");
}

/**
* @brief Tests the behavior of the HdmiCecRemoveLogicalAddress API when called with an invalid handle
*
* This test checks if the HdmiCecRemoveLogicalAddress method returns the correct error status when it is invoked with an invalid handle. This is important to verify the robustness of the API when dealing with erroneous inputs.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 025@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HDMI CEC driver function with an invalid handle | handle = -1 | The function should return an error: HDMI_CEC_IO_INVALID_HANDLE | The return status validates that the API is robust against erroneous inputs|
*/

void test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_sink...\n");
    int handle;
    HDMI_CEC_STATUS status;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    status = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    handle = -1;
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with invalid handle.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_sink...\n");
}

/**
* @brief This test checks the behavior of the HdmiCecRemoveLogicalAddress function when proper sequences are not followed.
*
* This test case performs a negative test by calling the HdmiCecRemoveLogicalAddress function before calling the HdmiCecOpen function and checks if the proper error status, HDMI_CEC_IO_NOT_OPENED, is returned.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 026@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call HdmiCecRemoveLogicalAddress function with handle 1 and logical address 0x0 before calling HdmiCecOpen | handle = 1, logical address = 0x0 | The returned status should be HDMI_CEC_IO_NOT_OPENED | This step simulates an incorrect use of the API |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_sink...\n");
    int handle = 1;
    HDMI_CEC_STATUS status;
    int logicalAddresses = 0x0;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress before calling HdmiCecOpen.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_sink...\n");
}

/**
* @brief Testing HdmiCecRemoveLogicalAddress functionality after HdmiCecClose
*
* This test is designed to confirm that the removal of a logical address on the HDMI CEC after the HDMI CEC is closed returns the correct status. The test specifically checks whether `HDMI_CEC_IO_NOT_OPENED` status is returned when HdmiCecRemoveLogicalAddress is invoked after HdmiCecClose.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 027@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Close HDMI CEC | handle = 1 | N/A | HDMI CEC should close successfully |
* | 02 | Attempt to remove a logical address on the HDMI CEC after it's closed | handle = 1, address = 0x0 | HDMI_CEC_IO_NOT_OPENED | IRC not opened due to previous closure, results in `HDMI_CEC_IO_NOT_OPENED` status |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_sink...\n");
    int handle;
    HDMI_CEC_STATUS ret;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", ret); 
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress after HdmiCecClose.");
    ret = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG(" HdmiCecRemoveLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_sink...\n");
}

/**
* @brief This test aims to verify the functionality of HdmiCecRemoveLogicalAddress API when passed with 0x0 as logical address.
*
* The HdmiCecRemoveLogicalAddress API is tested for invalid argument (0x0). This test scenario is important to ensure that the system recognizes and appropriately handles invalid arguments.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 028@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecRemoveLogicalAddress with handle as 1 and logicalAddress as 0x0 | handle = 1, logicalAddress = 0x0 | The status should be HDMI_CEC_IO_INVALID_ARGUMENT | The system should recognize and handle the invalid argument correctly |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_source...\n");
    int handle = 1;
    HDMI_CEC_STATUS status;
    int logicalAddresses = 0x0;    

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress in source device with logicalAddresses equal to 0x0.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_source...\n");
}

/**
* @brief Test to verify that HdmiCecRemoveLogicalAddress returns HDMI_CEC_IO_INVALID_ARGUMENT when provided with a non-zero logical address.
*
* This test is designed to confirm that the HdmiCecRemoveLogicalAddress function from the HDMI CEC driver rejects an invalid argument. The function should return HDMI_CEC_IO_INVALID_ARGUMENT when it is invoked with a logical address other than 0x0.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 029@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecRemoveLogicalAddress function with a non-zero logical address. | handle = 1, logical address = 0x1 | The function should return HDMI_CEC_IO_INVALID_ARGUMENT. | The function is expected to reject an invalid argument and should respond with an HDMI_CEC_IO_INVALID_ARGUMENT error status. |
*/

void test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_source...\n");
    int handle = 1;
    int logicalAddresses = 0x1;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress in source device with logicalAddresses not equal to 0x0.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_source...\n");
}

/**
 * @brief Validate whether the HdmiCecRemoveLogicalAddress API returns an error when called with an invalidated handle
 *
 * This test specifically checks the behavior of the HdmiCecRemoveLogicalAddress API when it is called with handle '1' after being invalidated by the HdmiCecClose API.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 030@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke HdmiCecClose with handle '1' to invalidate it |  handle = 1 | API call should be successful | - |
 * | 02 | Invoke HdmiCecRemoveLogicalAddress with the invalidated handle and logical address '0x0'  |  handle = 1, logical address = 0x0  | Should return HDMI_CEC_IO_INVALID_ARGUMENT | Should return error |
 */
void test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_source...\n");
    int handle;
    HDMI_CEC_STATUS status;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecRemoveLogicalAddress in source device after HdmiCecClose.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_source...\n");
}

/**
* @brief Tests the behavior of the HdmiCecRemoveLogicalAddress API when called with an invalid handle
*
* This test checks if the HdmiCecRemoveLogicalAddress method returns the correct error status when it is invoked with an invalid handle. This is important to verify the robustness of the API when dealing with erroneous inputs.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 031@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HDMI CEC driver function with an invalid handle | handle = -1 | The function should return an error: HDMI_CEC_IO_INVALID_HANDLE | The return status validates that the API is robust against erroneous inputs|
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_source...\n");
    int handle;
    HDMI_CEC_STATUS status;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    handle = -1;
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress with invalid handle.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_source...\n");
}

/**
* @brief This test checks the behavior of the HdmiCecRemoveLogicalAddress function when proper sequences are not followed.
*
* This test case performs a negative test by calling the HdmiCecRemoveLogicalAddress function before calling the HdmiCecOpen function and checks if the proper error status, HDMI_CEC_IO_NOT_OPENED, is returned.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 032@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call HdmiCecRemoveLogicalAddress function with handle 1 and logical address 0x0 before calling HdmiCecOpen | handle = 1, logical address = 0x0 | The returned status should be HDMI_CEC_IO_NOT_OPENED | This step simulates an incorrect use of the API |
*/
void test_l1_hdmi_cec_driver_negative5_HdmiCecRemoveLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative5_HdmiCecRemoveLogicalAddress_source...\n");
    int handle = 1;
    HDMI_CEC_STATUS status;
    int logicalAddresses = 0x0;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecRemoveLogicalAddress before calling HdmiCecOpen.");
    status = HdmiCecRemoveLogicalAddress(handle, logicalAddresses);
    UT_LOG("HdmiCecRemoveLogicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative5_HdmiCecRemoveLogicalAddress_source...\n");
}

/**
* @brief Test to validate HdmiCecGetLogicalAddress API for positive scenario
*
* The purpose of this test is to ensure the correct functioning of HdmiCecGetLogicalAddress API with a valid handle and pointer to get the logical address. The test is important to ensure that this basic functionality of the API is working as expected.
*
* **Test Group ID:** Basic: 01
* **Test Case ID:** 033
* **Priority:** High
* @n
* **Pre-Conditions:** None
* **Dependencies:** None
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call HdmiCecGetLogicalAddress API with a valid handle and pointer | handle = valid_handle, pointer = &logicalAddress | HDMI_CEC_STATUS should return HDMI_CEC_IO_SUCCESS and logicalAddress should return 0x0F | The API call should be successful |
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_sink...\n");
    int handle;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecGetLogicalAddress with valid handle and pointer.");
    ret = HdmiCecGetLogicalAddress(handle, &logicalAddresses);
    UT_LOG("HdmiCecGetLogicalAddress logicalAddress: %d and returns: %d\n", logicalAddresses, ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);
    if(logicalAddresses == 0x0F)
    {
 	    UT_LOG("logicalAddress is %x which is a valid value.", logicalAddresses);
        UT_PASS("HdmiCecGetLogicalAddress validation success");
    }
	else
    {
	    UT_LOG("logicalAddress is %x which is a invalid value.", logicalAddresses);
        UT_FAIL("HdmiCecGetLogicalAddress validation failed");
    } 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_sink...\n");
}


/**
* @brief Test to validate HdmiCecGetLogicalAddress API for positive scenario
*
* The purpose of this test is to ensure the correct functioning of HdmiCecGetLogicalAddress API with a valid handle and pointer to get the logical address after HdmiCecAddLogicalAddress . The test is important to ensure that this basic functionality of the API is working as expected.
*
* **Test Group ID:** Basic: 01
* **Test Case ID:** 033
* **Priority:** High
* @n
* **Pre-Conditions:** None
* **Dependencies:** None
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Call HdmiCecGetLogicalAddress API with a valid handle and pointer after HdmiCecAddLogicalAddress| handle = valid_handle, pointer = &logicalAddress | HDMI_CEC_STATUS should return HDMI_CEC_IO_SUCCESS and logicalAddress should return 0x0 | The API call should be successful |
*/
void test_l1_hdmi_cec_driver_positive2_HdmiCecGetLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive2_HdmiCecGetLogicalAddress_sink...\n");
    int handle;
    int logicalAddresses = 0x0;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", ret); 
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecGetLogicalAddress with valid handle and pointer.");
    ret = HdmiCecGetLogicalAddress(handle, &logicalAddresses);
    UT_LOG("HdmiCecGetLogicalAddress logicalAddress: %d and returns: %d\n", logicalAddresses, ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);
    if(logicalAddresses == 0x0)
    {
 	    UT_LOG("logicalAddress is %x which is a valid value.", logicalAddresses);
        UT_PASS("HdmiCecGetLogicalAddress validation success");
    }
	else
    {
	    UT_LOG("logicalAddress is %x which is a invalid value.", logicalAddresses);
        UT_FAIL("HdmiCecGetLogicalAddress validation failed");
    } 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive2_HdmiCecGetLogicalAddress_sink...\n");
}

/**
* @brief This unit test function verifies the functionality of the HdmiCecGetLogicalAddress function.
*
* This test function tests the ability of HdmiCecGetLogicalAddress to successfully retrieve the logical address of the HDMI CEC device by providing it a valid handle as a parameter.
*
* **Test Group ID:** Basic: 01
* **Test Case ID:** 034
* **Priority:** High
* @n
* **Pre-Conditions:** None
* **Dependencies:** None
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description| Test Data| Expected Result| Notes|
* | :-------: | -------- | ----- | ------- | --------- |
* | 01 | Call the function HdmiCecGetLogicalAddress with a valid handle and a pointer to save the result | handle = valid_handle | The function should return HDMI_CEC_IO_SUCCESS and the logical address should be within range (0x01 to 0x0E) | This test case should pass |
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_source...\n");
    int handle;
    int logicalAddress;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecGetLogicalAddress with valid handle and pointer.");
    ret = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG(" HdmiCecGetLogicalAddress Returned logicalAddress: %d and status: %d\n", logicalAddress, ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    if(logicalAddress >= 0x01 && logicalAddress <= 0x0E)
    {
 	    UT_LOG("logicalAddress is %x which is a valid value.", logicalAddress);
        UT_PASS("HdmiCecGetLogicalAddress validation success");
    }
	else
    {
	    UT_LOG("logicalAddress is %x which is a invalid value.", logicalAddress);
        UT_FAIL("HdmiCecGetLogicalAddress validation failed");
    } 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_source...\n");
}

/**
* @brief This test is designed to verify the negative scenario for the function 'HdmiCecGetLogicalAddress' when invoked with NULL pointer.
*
* This test ensures the HDMI CEC driver's 'HdmiCecGetLogicalAddress' API handles invalid arguments correctly by returning the correct error status. This is vital to ensure robustness and fault tolerance of the HDMI CEC driver system.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 035@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke 'HdmiCecGetLogicalAddress' API with NULL pointer as argument. | handle = valid_handle, pointer = NULL | Return status should be 'HDMI_CEC_IO_INVALID_ARGUMENT' | This test is checking the HDCEC driver's ability to handle invalid arguments correctly |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_sink...\n");
    int handle;
    HDMI_CEC_STATUS ret;
    int *logicalAddress = NULL;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    ret = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", ret); 
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecGetLogicalAddress with NULL pointer.");
    ret = HdmiCecGetLogicalAddress(handle, logicalAddress);
    UT_LOG("HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_sink...\n");
}

/**
* @brief This test is designed to verify the negative scenario for the function 'HdmiCecGetLogicalAddress' when invoked with NULL pointer.
*
* This test ensures the HDMI CEC driver's 'HdmiCecGetLogicalAddress' API handles invalid arguments correctly by returning the correct error status. This is vital to ensure robustness and fault tolerance of the HDMI CEC driver system.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 036@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke 'HdmiCecGetLogicalAddress' API with NULL pointer as argument. | handle = valid_handle, pointer = NULL | Return status should be 'HDMI_CEC_IO_INVALID_ARGUMENT' | This test is checking the HDCEC driver's ability to handle invalid arguments correctly |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_source...\n");
    int handle;
    HDMI_CEC_STATUS ret;
    int *logicalAddress = NULL;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecGetLogicalAddress with NULL pointer.");
    ret = HdmiCecGetLogicalAddress(handle, logicalAddress);
    UT_LOG("HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_source...\n");
}

/**
 * @brief Test to verify the proper handling of invalid handle in HdmiCecGetLogicalAddress API
 *
 * This test validates the negative scenario where an invalid handle is passed to HdmiCecGetLogicalAddress API. The expected return value for an invalid handle is HDMI_CEC_IO_INVALID_HANDLE.
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 037@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke HdmiCecGetLogicalAddress with invalid handle| handle = 0 | HdmiCecGetLogicalAddress returns HDMI_CEC_IO_INVALID_HANDLE | This behavior is expected when the handle is invalid. |
 */
void test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_sink...\n");
    int handle;
    int logicalAddress;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    handle = 0;
    UT_LOG("Invoking HdmiCecGetLogicalAddress with invalid handle.");
    ret = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_sink...\n");
}

/**
 * @brief Test to verify the proper handling of invalid handle in HdmiCecGetLogicalAddress API
 *
 * This test validates the negative scenario where an invalid handle is passed to HdmiCecGetLogicalAddress API. The expected return value for an invalid handle is HDMI_CEC_IO_INVALID_HANDLE.
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 038@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke HdmiCecGetLogicalAddress with invalid handle| handle = 0 | HdmiCecGetLogicalAddress returns HDMI_CEC_IO_INVALID_HANDLE | This behavior is expected when the handle is invalid. |
 */
void test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_source...\n");
    int handle;
    int logicalAddress;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    handle = 0;
    UT_LOG("Invoking HdmiCecGetLogicalAddress with invalid handle.");
    ret = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG(" HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_source...\n");
}
/**
* @brief This test checks for a fail scenario where the HdmiCecGetLogicalAddress API is invoked without calling the HdmiCecOpen API first.
*
* This test tries to simulate an incorrect use of the API HdmiCecGetLogicalAddress where the prerequisite API HdmiCecOpen is not called. This checks for robustness and fail-safe mechanism where API is invoked in incorrect use cases.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 039@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Pass an invalid handle to the function HdmiCecGetLogicalAddress without calling HdmiCecOpen first | handle = invalid_handle | The returned result should be HDMI_CEC_IO_NOT_OPENED | The function HdmiCecGetLogicalAddress should handle this exception properly and return an error |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_sink...\n");
    int handle = 1;
    int logicalAddress;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecGetLogicalAddress without calling HdmiCecOpen.");
    HDMI_CEC_STATUS ret = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_sink...\n");
}

/**
* @brief This test checks for a fail scenario where the HdmiCecGetLogicalAddress API is invoked without calling the HdmiCecOpen API first.
*
* This test tries to simulate an incorrect use of the API HdmiCecGetLogicalAddress where the prerequisite API HdmiCecOpen is not called. This checks for robustness and fail-safe mechanism where API is invoked in incorrect use cases.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 040@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Pass an invalid handle to the function HdmiCecGetLogicalAddress without calling HdmiCecOpen first | handle = invalid_handle | The returned result should be HDMI_CEC_IO_NOT_OPENED | The function HdmiCecGetLogicalAddress should handle this exception properly and return an error |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_source...\n");
    int handle = 1;
    int logicalAddress;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecGetLogicalAddress without calling HdmiCecOpen.");
    HDMI_CEC_STATUS ret = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_source...\n");
}

/**
* @brief This test checks if the correct error code is returned when HdmiCecGetLogicalAddress API is invoked with a closed handle
*
* This test is important to verify the robustness of the HdmiCecGetLogicalAddress API. It tests the API's behavior when invoked with a handle that is already closed@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 041@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Obtain the HdmiCec handle and close it | handle = Obtained from HdmiCecOpen | Handle is successfully obtained and closed | Should be successful |
* | 02 | Invoke HdmiCecGetLogicalAddress API with the closed handle | handle = Closed handle, logicalAddress = Pointer to integer | API should return HDMI_CEC_IO_NOT_OPENED error | Should be successful |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_sink...\n");
    int logicalAddress;
    int handle;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecGetLogicalAddress after HdmiCecClose.");
    ret = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_sink...\n");
}

/**
* @brief This test checks if the correct error code is returned when HdmiCecGetLogicalAddress API is invoked with a closed handle
*
* This test is important to verify the robustness of the HdmiCecGetLogicalAddress API. It tests the API's behavior when invoked with a handle that is already closed@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 042@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Obtain the HdmiCec handle and close it | handle = Obtained from HdmiCecOpen | Handle is successfully obtained and closed | Should be successful |
* | 02 | Invoke HdmiCecGetLogicalAddress API with the closed handle | handle = Closed handle, logicalAddress = Pointer to integer | API should return HDMI_CEC_IO_NOT_OPENED error | Should be successful |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_source...\n");
    int logicalAddress;
    int handle;
    HDMI_CEC_STATUS ret;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    ret = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    ret = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecGetLogicalAddress after HdmiCecClose.");
    ret = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    UT_LOG("HdmiCecGetLogicalAddress returns %d\n", ret);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_source...\n");
}

/**
* @brief Test HDMI CEC Get Physical Address function with valid handle and address pointer
*
* The test defines a positive testing scenario where the HDMI CEC Get Physical Address function is invoked with a valid handle and physical address. The purpose is to validate the functional behavior of the API when provided with valid input parameters.
*
* **Test Group ID:** Basic: 01
* **Test Case ID:** 043
* **Priority:** High
* @n
* **Pre-Conditions:** None
* **Dependencies:** None
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecGetPhysicalAddress API with a valid handle and address | handle = valid handle, physicalAddress = valid physical address reference | API call should return HDMI_CEC_IO_SUCCESS| The test should pass if valid inputs are provided |
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecGetPhysicalAddress(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecGetPhysicalAddress...\n");
    unsigned int physicalAddress;
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecGetPhysicalAddress with valid handle and physicalAddress");    
    status = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    UT_LOG("HdmiCecGetPhysicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecGetPhysicalAddress...\n");
}

/**
* @brief Test Case to validate the HDMI CEC Physical Address Getter API with invalid handle.
*
* This test verifies the behavior of the HdmiCecGetPhysicalAddress API when an invalid handle is provided. It is necessary to ensure that the function recognises invalid arguments and correctly returns the corresponding error.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 044@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoking HdmiCecGetPhysicalAddress with invalid handle | handle = invalid handle | The output of function should be HDMI_CEC_IO_INVALID_HANDLE | Should throw error correctly |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecGetPhysicalAddress(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecGetPhysicalAddress...\n");
    unsigned int physicalAddress;
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    handle = -1;
    UT_LOG("Invoking HdmiCecGetPhysicalAddress with invalid handle.");
    status = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    UT_LOG("HdmiCecGetPhysicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecGetPhysicalAddress...\n");
}

/**
* @brief  Test the behavior of HdmiCecGetPhysicalAddress API when invoked with a NULL physicalAddress pointer
*
* In this test, HdmiCecGetPhysicalAddress API is invoked with a valid handle and a NULL physicalAddress pointer. The purpose is to verify if the API can handle improper arguments correctly, and return the correct error code.@n The usage for this test is to provide a proper error handling for the cases when invalid arguments are provided to the API.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 045@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecGetPhysicalAddress with valid handle and NULL physicalAddress pointer | handle = valid_handle, physicalAddress pointer = NULL | Return status should be HDMI_CEC_IO_INVALID_ARGUMENT | This test should be successful |
*/
void test_l1_hdmi_cec_driver_negative2_HdmiCecGetPhysicalAddress(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecGetPhysicalAddress...\n");
    unsigned int *physicalAddress = NULL;
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecGetPhysicalAddress with valid handle and NULL physicalAddress");
    status = HdmiCecGetPhysicalAddress(handle, physicalAddress);
    UT_LOG("HdmiCecGetPhysicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecGetPhysicalAddress...\n");
}

/**
* @brief This test verifies if the HDMI_CEC_STATUS is HDMI_CEC_IO_NOT_OPENED when HdmiCecGetPhysicalAddress() API called without opening HDMI CEC driver.
*
* The description of what is tested extracts the physical address from a handle expected to be not opened because HdmiCecOpen() has not been called.@n
* @n
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 046@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecGetPhysicalAddress without opening HDMI CEC driver | handle_without_open, physicalAddress | The status returned is HDMI_CEC_IO_NOT_OPENED | The test should be successful |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecGetPhysicalAddress(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecGetPhysicalAddress...\n");
    unsigned int physicalAddress;
    int handle = 1;
    HDMI_CEC_STATUS status;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecGetPhysicalAddress without calling HdmiCecOpen().");
    status = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    UT_LOG("HdmiCecGetPhysicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecGetPhysicalAddress...\n");
}

/**
* @brief Testing HDMI CEC Driver's ability to handle invalid calls to the HdmiCecGetPhysicalAddress after an HdmiCecClose() operation
*
* This test is designed to test the HdmiCecGetPhysicalAddress functionality after HdmiCecClose() has happened. This is a negative test to ensure the system can handle such disparate calls in a robust manner.
*
* **Test Group ID:** Basic: 01
* **Test Case ID:** 047
* **Priority:** High
* @n
* **Pre-Conditions:** None
* **Dependencies:** None
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Get the handle after closing the device | handle = get_handle_after_close() | Handle is obtained | Handle acquisition should be successful |
* | 02 | Get physical address using the acquired handle | handle, &physicalAddress | Return status should be HDMI_CEC_IO_NOT_OPENED | The operation should fail as the device is closed |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecGetPhysicalAddress(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecGetPhysicalAddress...\n");
    unsigned int physicalAddress;
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecGetPhysicalAddress after HdmiCecClose()");
    status = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    UT_LOG("HdmiCecGetPhysicalAddress returns %d\n", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecGetPhysicalAddress...\n");
}

/**
* @brief Test case to validate HDMI CEC Driver's Set RX Callback functionality with valid parameters.
*
* The objective of this test is to test the positive scenario of HDMI CEC Driver's API HdmiCecSetRxCallback. The API is tested with valid parameters to ensure the functionality behaves as expected under positive circumstances. 
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 048@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecSetRxCallback API with valid handle, callback function and data parameters. | valid_handle=1, valid_cbfunc=proper callback function, valid_data=proper data | The call to HdmiCecSetRxCallback returns HDMI_CEC_IO_SUCCESS | This step should result in successful execution and HDMI_CEC_IO_SUCCESS status returned by the API call. |
*/

void test_l1_hdmi_cec_driver_positive1_HdmiCecSetRxCallback( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecSetRxCallback...");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid parameters.");
    status = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecSetRxCallback...\n");
}

/**
* @brief Test the HdmiCecSetRxCallback function in positive test case scenario.
*
* This unit test case validates the functioning of HdmiCecSetRxCallback function under positive condition where valid handle and callback functions are provided as input
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 049@n
* **Priority:** High @n
* @n
* **Pre-Conditions:** None @n
* **Dependencies:** None @n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data | Expected Result | Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecSetRxCallback function with valid handle, callback function, and data | valid_handle = 1, valid_cbfunc, valid_data | The function should return HDMI_CEC_IO_SUCCESS | This is a positive test case for HdmiCecSetRxCallback function |
*/
void test_l1_hdmi_cec_driver_positive2_HdmiCecSetRxCallback( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive2_HdmiCecSetRxCallback...\n");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid parameters.");
    status = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback again with new data");
    status = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADC0DE);
    UT_LOG("HdmiCecSetRxCallback returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive2_HdmiCecSetRxCallback...\n");
}

/**
 * @brief This test confirms the correct behavior of the HdmiCecSetRxCallback API when invoked with null callback function.
 *
 * The HdmiCecSetRxCallback function is tested with a valid handle and null callback functions. The test is designed to validate whether the function returns the expected status when input parameters are not null.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 050@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke HdmiCecSetRxCallback with a valid handle and null callback functions | valid_handle = 1, callback1 = NULL, callback2 = NULL | The status should return HDMI_CEC_IO_SUCCESS | The test should pass successfully |
 */
void test_l1_hdmi_cec_driver_positive3_HdmiCecSetRxCallback( void ) 
{
   UT_LOG("Entering test_l1_hdmi_cec_driver_positive3_HdmiCecSetRxCallback...\n");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with null callback function.");
    status = HdmiCecSetRxCallback(handle, NULL, NULL);
    UT_LOG("HdmiCecSetRxCallback returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive3_HdmiCecSetRxCallback...\n");
}

/**
* @brief Test to validate HDMI CEC driver callback function with invalid handle
*
* This test aims to validate the behaviour of the HdmiCecSetRxCallback API when an invalid_handle is passed. The expected behavior is that the API should return an error code indicating HDMI_CEC_IO_NOT_OPENED @n
*
* **Test Group ID:** Basic: 01 @n
* **Test Case ID:** 051 @n
* **Priority:** High @n
* @n
* **Pre-Conditions:** None @n
* **Dependencies:** None @n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoking HdmiCecSetRxCallback without calling HdmiCecOpen | handle = valid , valid_cbfunc, valid_data | Return status code should be HDMI_CEC_IO_NOT_OPENED | This test should pass indicating the function responded correctly to the invalid input |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecSetRxCallback( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecSetRxCallback...\n");
    int handle = 1; 
    HDMI_CEC_STATUS status;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecSetRxCallback without calling HdmiCecOpen");
    status = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecSetRxCallback...\n");
}

/**
* @brief This test verifies that the set receiver callback function of the HDMI CEC driver handles invalid handles correctly
*
* In this unit test, the HdmiCecSetRxCallback function of the HDMI CEC Driver is invoked with an invalid handle. This test is designed to verify the robustness of the driver against invalid inputs.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 052@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the 'HdmiCecSetRxCallback' function with an invalid handle | invalid_handle = 0, valid_cbfunc, valid_data | The function should return 'HDMI_CEC_IO_INVALID_HANDLE' | Should be successful |
*/
void test_l1_hdmi_cec_driver_negative2_HdmiCecSetRxCallback( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecSetRxCallback...\n");
    int handle;
    HDMI_CEC_STATUS status; 

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    handle = 0;
    UT_LOG("Invoking HdmiCecSetRxCallback with unintialised handle.");
    status = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    
    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecSetRxCallback...\n");
}

/**
 * @brief Validate the return status of HdmiCecSetRxCallback after HdmiCecClose()
 *
 * This unit test validates the return status of the HdmiCecSetRxCallback function when it is invoked immediately after HdmiCecClose. The expected return status is HDMI_CEC_IO_NOT_OPENED.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 053@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
 * @n
 * **Test Procedure:**@n
 * 
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke the function HdmiCecSetRxCallback immediately after the function HdmiCecClose. | valid_handle = 1, valid_cbfunc = valid data pointer, valid_data = valid data | The status should be HDMI_CEC_IO_NOT_OPENED | If the function HdmiCecSetRxCallback is invoked just after the HdmiCecClose, it should return HDMI_CEC_IO_NOT_OPENED status as the IO handle is not open yet |
 */

void test_l1_hdmi_cec_driver_negative3_HdmiCecSetRxCallback( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecSetRxCallback...\n");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback immediately after HdmiCecClose(). ");
    status = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns %d", status);    
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecSetRxCallback...\n");
}

/**
* @brief This test verifies the HDMI CEC SetTxCallback functionality
* 
* This test aims to verify that the HDMI CEC SetTxCallback function works with valid parameters. The purpose of this test is to ensure robust and error-free implementation of HDMI CEC device interfacing.
*
* **Test Group ID:** Basic: 01
* **Test Case ID:** 054
* **Priority:** High
* @n
* **Pre-Conditions:** None
* **Dependencies:** None
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecSetTxCallback function with a valid handle, valid function pointer, and valid data | handle = valid handle, callback function = valid function pointer, data = valid data | The return value should be HDMI_CEC_IO_SUCCESS | The operation should be successful |
*/

void test_l1_hdmi_cec_driver_positive1_HdmiCecSetTxCallback(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecSetTxCallback...\n");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecSetTxCallback with valid handle, valid function pointer, and valid data");
    status = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG(" HdmiCecSetTxCallback returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecSetTxCallback...\n");
}

/**
* @brief Test the HdmiCecSetTxCallback function in positive test case scenario.
*
* This unit test case validates the functioning of HdmiCecSetTxCallback function under positive condition where valid handle and callback functions are provided as input
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 055@n
* **Priority:** High @n
* @n
* **Pre-Conditions:** None @n
* **Dependencies:** None @n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data | Expected Result | Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecSetTxCallback function with valid handle, callback function, and data | valid_handle = 1, valid_cbfunc, valid_data | The function should return HDMI_CEC_IO_SUCCESS | This is a positive test case for HdmiCecSetTxCallback function |
*/
void test_l1_hdmi_cec_driver_positive2_HdmiCecSetTxCallback( void ) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive2_HdmiCecSetTxCallback...\n");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid parameters.");
    status = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback again with new data");
    status = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADC0DE);
    UT_LOG("HdmiCecSetTxCallback returns: %d", status); 
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);  

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive2_HdmiCecSetTxCallback...\n");
}

/**
* @brief Testing the behavior of the HdmiCecSetTxCallback function with valid inputs.
*
* This test validates the HDMI CEC driver's behavior when HdmiCecSetTxCallback is invoked with valid parameters. It helps to ensure the driver can handle correct inputs and return the right status properly.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 056@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HDMI CEC driver's HdmiCecSetTxCallback API  with NULL function| Handle = returned by HdmiCecOpen(), Function pointer = NULL, Data = validData | Successful status indicator, HDMI_CEC_IO_SUCCESS | The function should handle these parameters successfully |
*/
void test_l1_hdmi_cec_driver_positive3_HdmiCecSetTxCallback(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive3_HdmiCecSetTxCallback...\n");
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecSetTxCallback with valid handle, NULL function pointer, and valid data.");
    status = HdmiCecSetTxCallback(handle, NULL, (void*)0xDEADBEEF);
    UT_LOG(" HdmiCecSetTxCallback returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive3_HdmiCecSetTxCallback...\n");
}

/**
 * @brief Checks if the HDMI_CEC_STATUS return type is HDMI_CEC_IO_NOT_OPENED when HdmiCecSetTxCallback is invoked without prior HdmiCecOpen call
 *
 * This test aims to verify that the HDMI_CEC_STATUS returned by HdmiCecSetTxCallback API is HDMI_CEC_IO_NOT_OPENED when the API is called with an invalid handle and without having called the HdmiCecOpen previously. 
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 057@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
 *
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke HdmiCecSetTxCallback API without prior HdmiCecOpen call| invalidHandle = NULL, anyCbFunc = any function, anyData = any data | HDMI_CEC_STATUS should be HDMI_CEC_IO_NOT_OPENED | Test case should pass |
 */
void test_l1_hdmi_cec_driver_negative1_HdmiCecSetTxCallback(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecSetTxCallback...\n");
    int handle = 1;
    HDMI_CEC_STATUS status;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecSetTxCallback without prior HdmiCecOpen call");
    status = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecSetTxCallback...\n");
}

/**
 * @brief This test aims to check if the 'HdmiCecSetTxCallback' function handles invalid handle input appropriately.
 *
 * Within this test, the 'HdmiCecSetTxCallback' function is invoked with an invalid handle along with a valid function pointer and data. Here, we test if the function is able to respond appropriately to this erroneous case by returning an HDMI_CEC_IO_INVALID_HANDLE error.
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 058@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke 'HdmiCecSetTxCallback' function with invalid handle along with a valid function pointer and data | handle = invalidHandle, function_pointer = validCbFunc, data = validData | Return value: HDMI_CEC_IO_INVALID_HANDLE | The function should appropriately handle the erroneous case of providing an invalid handle |
 */
void test_l1_hdmi_cec_driver_negative2_HdmiCecSetTxCallback(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecSetTxCallback...\n");
    int handle = -1;
    HDMI_CEC_STATUS result;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",result);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecSetTxCallback with invalid handle, valid function pointer, and valid data.");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns %d", result);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", result);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecSetTxCallback...\n");
}

/**
* @brief Test to check behavior of HdmiCecSetTxCallback after closing the HDMI
*
* This unit test focuses on exploiting the scenario where HdmiCecSetTxCallback API is invoked post closing the HDMI. The test is designed to ensure that the API behaves as expected i.e., returns an error, when it is subjected to unexpected usage.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 059@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Open HDMI and get the handle | No input data required | Successful Action | Success depends on underlying hardware & OS |
* | 02 | Now close the HDMI | handle obtained from previous step | Successful Action | Success depends on underlying hardware |
* | 03 | Attempt to set the Tx callback post closing the HDMI | handle, anyCbFunc, anyData | HDMI_CEC_IO_NOT_OPENED | Error.code as HDMI_CEC_IO_NOT_OPENED, as this is an invalid operation |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecSetTxCallback(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecSetTxCallback...\n");    
    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG(" HdmiCecOpen returns %d",status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS); 

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);
    
    UT_LOG("Invoking HdmiCecSetTxCallback after closing the HDMI.");
    status = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    status = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns %d", status);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecSetTxCallback...\n");
}

/**
* @brief Test to verify HDMI CEC driver's transmission functionality
*
* This test checks if the HDMI CEC driver's transmission function responds correctly when given certain input under defined conditions. This is important to verify in order to ensure a smooth transmission process during actual usage,@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 060@n
* **Priority:** High
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description| Test Data  | Expected Result  | Notes  |
* | :-------: | -----------| ---------  | --------- | ------------ |
* | 01 | Get the handle for the HDMI CEC driver| handle = 1 | Successfully obtained handle| Should be successful |
* | 02  | Set a buffer with predetermined data| buffer = {0x3F, CEC_GET_CEC_VERSION} | Data is successfully set | Should be successful |
*/    
void test_l1_hdmi_cec_driver_positive1_HdmiCecTx_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecTx_sink...");
    int handle;
    int len = 2;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;
    int logicalAddresses = 0x0;
    
    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    result = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx with valid input parameters");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SENT_AND_ACKD);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecTx_sink...");
}

/**
* @brief Test the functionality of the function l1_hdmi_cec_driver_positive2_HdmiCecTx
*
* This test aims to validate the proper functionality of the HDMI CEC transmitting driver. It depicts the process of transmitting HDMI CEC and assures the sturdiness of transmission. The significance of this test is to adequately ensure the sterility of HDMI CEC transmission.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 061@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Initialize the handle | Handle = valid | The operation of setting up the handle is successful | Should be successful|
* | 02 | Setup the buffer for HDMI CEC transmission | Buffer = {0x3F, CEC_GET_CEC_VERSION} | The buffer setup for HDMI CEC transmission is successful | This is the second step in which buffer contents are setup for transmission |   
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecTx_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecTx_source...");
    int handle;
    int len = 2;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx with valid input parameters");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL(ret, HDMI_CEC_IO_SENT_AND_ACKD);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecTx_source...");
}

/**
 * @brief Unit testing for HDMI CEC Transmitter.
 *
 * This test aims to evaluate the negative test case scenario of HDMI CEC Transmitter part of our application. This mainly pertains to testing the improper behavior of the HDMI CEC Transmitter, in cases such as when it may receive invalid data or unexpected data.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 062@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Initialize the HDMI CEC transmitter unit with improper data | handle = 1, buffer = {0x3F, CEC_GET_CEC_VERSION} | The unit should handle the errors gracefully, failures are expected in this negative test scenario | The system should not crash during this test |
 */
void test_l1_hdmi_cec_driver_negative1_HdmiCecTx_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecTx_sink...");
    int handle = 1;
    int len = 2;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecTx API without calling the HdmiCecOpen() function ");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecTx_sink...");
}

/**
 * @brief This test case verifies the negative behavior of the HdmiCecTx function in the HDMI CEC driver
 *
 * In this test, the HdmiCecTx driver function is invoked with an uninitialized handle and a test buffer.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 063@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke the HdmiCecTx driver function with a invalid handle and a test buffer | handle = 0, buffer = {0x3F, CEC_GET_CEC_VERSION} | The function should return an error due to the invalid handle | The test verifies the function's behavior when provided with invalid parameters |
 */
void test_l1_hdmi_cec_driver_negative2_HdmiCecTx_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecTx_sink...");
    int handle;
    int len = 2;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    result = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    handle = -1;
    UT_LOG("Invoking HdmiCecTx API with invalid handle.");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecTx_sink...");
}  

/**
* @brief Test the API HdmiCecTx() error handling when invoked with NULL buffer.
*
* This test simulates a scenario where the buffer input to the API HdmiCecTx is NULL. The API is expected to return an error code indicating the invalid argument. This test case is created to validate if the API correctly handles such edge cases and returns proper error information.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 064@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.@n
*
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTx with NULL buffer and valid handle, len and result variables. | handle=1, buffer=NULL, len=2, result=Initialized to 0 | The API should return HDMI_CEC_IO_INVALID_ARGUMENT, indicating an invalid input. | Should return HDMI_CEC_IO_INVALID_ARGUMENT for NULL buffer input |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecTx_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecTx_sink...");
    int handle;
    unsigned char *buffer = NULL;
    int len = 2;
    int logicalAddresses = 0x0;
    int ret = 0;
    HDMI_CEC_STATUS result;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    result = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx with NULL buffer.");
    result = HdmiCecTx(handle, buffer, len, &ret);
    UT_LOG("HdmiCecTx returns = %d and ret = %d",result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);    

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecTx_sink...");
}

/**
 * @brief This test verifies the negative response from 'hdmi_cec_tx' function of the HDMI CEC driver when providing an unusual handle value.
 *
 * In this test, the 'hdmi_cec_tx' function is called with a handle value of '1', an unusual value for this function, and we expect to see a failed response due to the unusual handle value.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 065@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Call 'hdmi_cec_tx' function with a handle value of 1 | handle = 1, buffer = {0x3F, CEC_GET_CEC_VERSION} | The function returns showing a failure message | Handle values are usually not at such low levels |
 */
 void test_l1_hdmi_cec_driver_negative4_HdmiCecTx_sink(void)
 {
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecTx_sink...");
    int handle;
    int len = 0;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    result = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx with 0 or negative len parameters");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecTx_sink...");
 }

/**
* @brief This test validates hdmi cec driver functionality
*
* The test verifies that the hdmi cec driver is working properly in negative conditions@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 066@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Initialize log | - | Log initialized | Should be successful |
* | 02 | Define Handle and Buffer| handle = 1, buffer = {0x3F, CEC_GET_CEC_VERSION} | Handle and Buffer defined successfully | Should be successful |
*/
void test_l1_hdmi_cec_driver_negative5_HdmiCecTx_sink(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative5_HdmiCecTx_sink...");
    int handle;
    int len = 0;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    result = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx with after HdmiCecClose");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative5_HdmiCecTx_sink...");
}

/**
 * @brief Unit testing for HDMI CEC Transmitter.
 *
 * This test aims to evaluate the negative test case scenario of HDMI CEC Transmitter part of our application. This mainly pertains to testing the improper behavior of the HDMI CEC Transmitter, in cases such as when it may receive invalid data or unexpected data.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 067@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data | Expected Result | Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Initialize the HDMI CEC transmitter unit with improper data | handle = 1, buffer = {0x3F, CEC_GET_CEC_VERSION} | The unit should handle the errors gracefully, failures are expected in this negative test scenario | The system should not crash during this test |
 */
void test_l1_hdmi_cec_driver_negative1_HdmiCecTx_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecTx_source...");
    int handle = 1;
    int len = 2;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecTx API without calling the HdmiCecOpen() function ");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecTx_source...");
}

/**
 * @brief This test case verifies the negative behavior of the HdmiCecTx function in the HDMI CEC driver
 *
 * In this test, the HdmiCecTx driver function is invoked with an uninitialized handle and a test buffer.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 068@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Invoke the HdmiCecTx driver function with invalid handle and a test buffer | handle = 0, buffer = {0x3F, CEC_GET_CEC_VERSION} | The function should return an error due to the invalid handle | The test verifies the function's behavior when provided with invalid parameters |
 */
void test_l1_hdmi_cec_driver_negative2_HdmiCecTx_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecTx_source...");
    int handle;
    int len = 2;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    handle = -1;
    UT_LOG("Invoking HdmiCecTx API with invalid handle.");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecTx_source...");
}  

/**
* @brief Test the API HdmiCecTx() error handling when invoked with NULL buffer.
*
* This test simulates a scenario where the buffer input to the API HdmiCecTx is NULL. The API is expected to return an error code indicating the invalid argument. This test case is created to validate if the API correctly handles such edge cases and returns proper error information.
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 069@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.@n
*
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTx with NULL buffer and valid handle, len and result variables. | handle=1, buffer=NULL, len=2, result=Initialized to 0 | The API should return HDMI_CEC_IO_INVALID_ARGUMENT, indicating an invalid input. | Should return HDMI_CEC_IO_INVALID_ARGUMENT for NULL buffer input |
*/

void test_l1_hdmi_cec_driver_negative3_HdmiCecTx_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecTx_source...");
    int handle;
    unsigned char *buffer = NULL;
    int len = 2;
    int result = 0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx with NULL buffer.");
    HDMI_CEC_STATUS status = HdmiCecTx(handle, buffer, len, &result);
    UT_LOG("HdmiCecTx returns %d",status);    
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT);  

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecTx_source...");
}

/**
 * @brief This test verifies the negative response from 'hdmi_cec_tx' function of the HDMI CEC driver when providing an unusual handle value.
 *
 * In this test, the 'hdmi_cec_tx' function is called with a handle value of '1', an unusual value for this function, and we expect to see a failed response due to the unusual handle value.@n
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 070@n
 * **Priority:** High@n
 * @n
 * **Pre-Conditions:** None@n
 * **Dependencies:** None@n
 * **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
 * @n
 * **Test Procedure:**@n
 * | Variation / Step | Description | Test Data |Expected Result |Notes |
 * | :----: | --------- | ---------- |-------------- | ----- |
 * | 01 | Call 'hdmi_cec_tx' function with a handle value of 1 | handle = 1, buffer = {0x3F, CEC_GET_CEC_VERSION}| The function returns showing a failure message | Handle values are usually not at such low levels |
 */
 void test_l1_hdmi_cec_driver_negative4_HdmiCecTx_source(void)
 {
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecTx_source...");
    int handle;
    int len = 0;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx with invalid 0 or negative len parameters");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecTx_source...");
 }

/**
* @brief This test validates hdmi cec driver functionality
*
* The test verifies that the hdmi cec driver is working properly in negative conditions@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 071@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Initialize log | - | Log initialized | Should be successful |
* | 02 | Define Handle and Buffer| handle = 1, buffer = {0x3F, CEC_GET_CEC_VERSION} | Handle and Buffer defined successfully | Should be successful |
*/
void test_l1_hdmi_cec_driver_negative5_HdmiCecTx_source(void)
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative5_HdmiCecTx_source...");
    int handle;
    int len = 0;
    int ret = 0;
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    HDMI_CEC_STATUS result;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid handle,callback and buffer");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTx after HdmiCecClose()");
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_LOG("HdmiCecTx returns: %d and ret = %d", result, ret);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative5_HdmiCecTx_source...");
}
/**
* @brief This test case is designed to check the successful transmission of HDMI-CEC 
*
* This test validates the correct operation of the HdmiCecTxAsync function in the HDMI-CEC driver. This involves checking if overall functionality works as intended when provided with valid inputs@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 072 @n
* **Priority:** High@n
*
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data | Expected Result | Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecTxAsync function with valid handle, buffer and length | handle = valid_handle, buf = valid_buf, len = valid_len | HDMI_CEC_IO_SUCCESS | Should be successful |
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_sink(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_sink...");
    int handle;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};
    int logicalAddresses = 0x0;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecAddLogicalAddress with handle and logicalAddresses set to 0x0.");
    result = HdmiCecAddLogicalAddress(handle, logicalAddresses);    
    UT_LOG("HdmiCecAddLogicalAddress returns %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync with valid input parameters");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_sink...");
}

/**
* @brief This test case is designed to check the successful transmission of HDMI-CEC 
*
* This test validates the correct operation of the HdmiCecTxAsync function in the HDMI-CEC driver. This involves checking if overall functionality works as intended when provided with valid inputs@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 073 @n
* **Priority:** High@n
*
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data | Expected Result | Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecTxAsync function with valid handle, buffer and length | handle = valid_handle, buf = valid_buf, len = valid_len | HDMI_CEC_IO_SUCCESS | Should be successful |
*/
void test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_source(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_source...");
    int handle;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync with valid input parameters");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    buf [1] = CEC_DEVICE_VENDOR_ID;
    UT_LOG("Invoking HdmiCecTxAsync 2nd time with valid input parameters");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_source...");
}

/**
* @brief This test checks whether the HdmiCecTxAsync function behaves as expected when called without module initialisation.
*
* This test invokes the HdmiCecTxAsync function without initialising the HDMI module and checks if the function correctly returns a HDMI_CEC_IO_NOT_OPENED status.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 074@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTxAsync without module initialisation | handle = NULL, buf = NULL, len = 0 | HDMI_CEC_IO_NOT_OPENED status should be returned | Test should pass if HDMI_CEC_IO_NOT_OPENED status is returned |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_sink(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_sink...");
    int handle = 1;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION}; 

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecTxAsync without initialising the module.");  
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_sink...");
}

/**
* @brief Test HdmiCecTxAsync function for invalid CEC packet
*
* This test involves testing the 'HdmiCecTxAsync' API and makes sure it returns correct error code when an invalid CEC packet is passed@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 075@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- |-----|
* | 01 | Invoke HdmiCecTxAsync API with invalid CEC packet | handle: valid handle, buf: invalid CEC packet, len: length of the buffer | The return status from HdmiCecTxAsync should match with HDMI_CEC_IO_INVALID_ARGUMENT | The test should succeed if the API returns HDMI_CEC_IO_INVALID_ARGUMENT |
*/
void test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_sink(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_sink...");
    int handle;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {}; 

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync with invalid CEC packet in buffer.");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_sink...");
}

/**
* @brief Unit test for testing HDMI CEC TxAsync API for negative scenarios
*
* This test is designed to validate the behavior of HdmiCecTxAsync function when invoked with incorrect length for HDMI CEC driver layer-1.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 076@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTxAsync with incorrect length | Handle value, Buffer value, Incorrect length  | Should return HDMI_CEC_IO_INVALID_ARGUMENT | API should handle invalid input gracefully |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_sink(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_sink...");
    int handle;
    HDMI_CEC_STATUS result;
    int len = 0;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync with incorrect length");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_sink...");
}

/**
* @brief Tests the asynchronous HDMI CEC Transmission function when given an invalid handle
*
* This test invokes the HdmiCecTxAsync function with an invalid handle to test error handling in the HDMI CEC driver. The enhancement of the error handling capability will ensure that the system is robust against incorrect or improper API usage.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 077@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTxAsync with invalid handle | handle = invalid, buf = valid, len = valid | The return status should be HDMI_CEC_IO_INVALID_HANDLE | This test case should pass, indicating the function properly handles an invalid handle |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_sink(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_sink...");
    int handle = -1;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync with invalid handle.");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_sink...");
}

/**
* @brief Unit test for negative validation of the HDMI CEC Transmission API HdmiCecTxAsync
*
* This test case validates the negative behavior of HdmiCecTxAsync API. It tests the scenario when a NULL buffer is passed to the API.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 078@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data | Expected Result | Notes |
* | :----: | --------- | ---------- | -------------- | ----- |
* | 01 | Invoking HdmiCecTxAsync with a NULL buffer | handle = valid handle, buffer = NULL, len = valid length | Return status should be HDMI_CEC_IO_INVALID_ARGUMENT | NULL buffer is an invalid argument |
*/
void test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_sink(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_sink...");
    int result = 0;
    int handle = 0;
    int len = 2;
    unsigned char *buf = NULL;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync passing a  NULL buffer.");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_sink...");
}

/**
* @brief This test aims to verify that the HDMI CEC driver behaves as expected when running HdmiCecTxAsync 
*
* This function tests the HdmiCecTxAsync API. The test aims to ensure that when the HDMI CEC driver is invoked after a HdmiCecClose call, it responds with the HDMI_CEC_IO_NOT_OPENED status.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 079@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecTxAsync API after the HdmiCecClose to verify the HDMI CEC driver's responses | handle = NULL, buf = NULL, len = 0 | The system should return HDMI_CEC_IO_NOT_OPENED | Process should be successful |
*/
void test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_sink(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_sink...");
    int handle ;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync eventually after HdmiCecClose.");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_sink...");
}

/**
* @brief This test checks whether the HdmiCecTxAsync function behaves as expected when called without module initialisation.
*
* This test invokes the HdmiCecTxAsync function without initialising the HDMI module and checks if the function correctly returns a HDMI_CEC_IO_NOT_OPENED status.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 080@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console.
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTxAsync without module initialisation | handle = NULL, buf = NULL, len = 0 | HDMI_CEC_IO_NOT_OPENED status should be returned | Test should pass if HDMI_CEC_IO_NOT_OPENED status is returned |
*/
void test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_source(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_source...");
    int handle = 1;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION}; 

    HdmiCecClose(handle);
    UT_LOG("Invoking HdmiCecTxAsync without initialising the module.");  
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_source...");
}

/**
* @brief Test HdmiCecTxAsync function for invalid CEC packet
*
* This test involves testing the 'HdmiCecTxAsync' API and makes sure it returns correct error code when an invalid CEC packet is passed@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 081@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console@n
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- |-----|
* | 01 | Invoke HdmiCecTxAsync API with invalid CEC packet | handle: valid handle, buf: invalid CEC packet, len: length of the buffer | The return status from HdmiCecTxAsync should match with HDMI_CEC_IO_INVALID_ARGUMENT | The test should succeed if the API returns HDMI_CEC_IO_INVALID_ARGUMENT |
*/
void test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_source(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_source...");
    int handle;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {}; 

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync with invalid CEC packet in buffer.");
    HDMI_CEC_STATUS status = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_source...");
}

/**
* @brief Unit test for testing HDMI CEC TxAsync API for negative scenarios
*
* This test is designed to validate the behavior of HdmiCecTxAsync function when invoked with incorrect length for HDMI CEC driver layer-1.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 082@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTxAsync with incorrect length | Handle value, Buffer value, Incorrect length  | Should return HDMI_CEC_IO_INVALID_ARGUMENT | API should handle invalid input gracefully |
*/
void test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_source(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_source...");
    int handle;
    HDMI_CEC_STATUS result;
    int len = 0;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    buf[0] = 0x0F;
    UT_LOG("Invoking HdmiCecTxAsync with incorrect length");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_source...");
}

/**
* @brief Tests the asynchronous HDMI CEC Transmission function when given an invalid handle
*
* This test invokes the HdmiCecTxAsync function with an invalid handle to test error handling in the HDMI CEC driver. The enhancement of the error handling capability will ensure that the system is robust against incorrect or improper API usage.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 083@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke HdmiCecTxAsync with invalid handle | handle = invalid, buf = valid, len = valid | The return status should be HDMI_CEC_IO_INVALID_HANDLE | This test case should pass, indicating the function properly handles an invalid handle |
*/
void test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_source(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_source...");
    int handle = -1;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync with invalid handle.");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_HANDLE);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_source...");
}

/**
* @brief Unit test for negative validation of the HDMI CEC Transmission API HdmiCecTxAsync
*
* This test case validates the negative behavior of HdmiCecTxAsync API. It tests the scenario when a NULL buffer is passed to the API.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 084@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data | Expected Result | Notes |
* | :----: | --------- | ---------- | -------------- | ----- |
* | 01 | Invoking HdmiCecTxAsync with a NULL buffer | handle = valid handle, buffer = NULL, len = valid length | Return status should be HDMI_CEC_IO_INVALID_ARGUMENT | NULL buffer is an invalid argument |
*/
void test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_source(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_source...");
    int result = 0;
    int handle = 0;
    int len = 2;
    unsigned char *buf = NULL;

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetRxCallback with valid callback and data");
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetRxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecSetTxCallback with valid callback function and data...");
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, (void*)0xDEADBEEF);
    UT_LOG("HdmiCecSetTxCallback returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync passing a  NULL buffer.");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);    
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_source...");
}

/**
* @brief This test aims to verify that the HDMI CEC driver behaves as expected when running HdmiCecTxAsync 
*
* This function tests the HdmiCecTxAsync API. The test aims to ensure that when the HDMI CEC driver is invoked after a HdmiCecClose call, it responds with the HDMI_CEC_IO_NOT_OPENED status.@n
*
* **Test Group ID:** Basic: 01@n
* **Test Case ID:** 085@n
* **Priority:** High@n
* @n
* **Pre-Conditions:** None@n
* **Dependencies:** None@n
* **User Interaction:** If user chose to run the test in interactive mode, then the test case has to be selected via console
* @n
* **Test Procedure:**@n
* | Variation / Step | Description | Test Data |Expected Result |Notes |
* | :----: | --------- | ---------- |-------------- | ----- |
* | 01 | Invoke the HdmiCecTxAsync API after the HdmiCecClose to verify the HDMI CEC driver's responses | handle = NULL, buf = NULL, len = 0 | The system should return HDMI_CEC_IO_NOT_OPENED | Process should be successful |
*/
void test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_source(void) 
{
    UT_LOG("Entering test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_source...");
    int handle ;
    HDMI_CEC_STATUS result;
    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("Invoking HdmiCecOpen with valid handle");
    result = HdmiCecOpen(&handle);
    UT_LOG("HdmiCecOpen returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecClose with valid handle");
    result = HdmiCecClose(handle);
    UT_LOG("HdmiCecClose returns: %d", result); 
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_SUCCESS);

    UT_LOG("Invoking HdmiCecTxAsync eventually after HdmiCecClose.");
    result = HdmiCecTxAsync(handle, buf, len);
    UT_LOG("HdmiCecTxAsync returns: %d", result);
    UT_ASSERT_EQUAL(result, HDMI_CEC_IO_NOT_OPENED);

    UT_LOG("Exiting test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_source...");
}

static UT_test_suite_t * pSuite = NULL;

/**
 * @brief Register the main tests for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int register_hal_tests(void)
{
    // Create the test suite
    pSuite = UT_add_suite("[L1 hdmi_cec_driver]", NULL, NULL);
    if (pSuite == NULL) {
        return -1;
    }
    // List of test function names and strings
    #if defined(__STB__)
        const char* list1[] = {"l1_hdmi_cec_driver_positive1_HdmiCecOpen", "l1_hdmi_cec_driver_negative1_HdmiCecOpen", "l1_hdmi_cec_driver_negative2_HdmiCecOpen", "l1_hdmi_cec_driver_positive2_HdmiCecOpen", "l1_hdmi_cec_driver_positive1_HdmiCecClose", "l1_hdmi_cec_driver_positive2_HdmiCecClose", "l1_hdmi_cec_driver_negative1_HdmiCecClose", "l1_hdmi_cec_driver_negative2_HdmiCecClose", "l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_source", "l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_source", "l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_source", "l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_source", "l1_hdmi_cec_driver_negative5_HdmiCecAddLogicalAddress_source", "l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_source", "l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_source", "l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_source", "l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_source", "l1_hdmi_cec_driver_negative5_HdmiCecRemoveLogicalAddress_source", "l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_source","l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_source", "l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_source", "l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_source", "l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_source", "l1_hdmi_cec_driver_positive1_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative1_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative2_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative3_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative4_HdmiCecGetPhysicalAddress",  "l1_hdmi_cec_driver_positive1_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_positive2_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_positive3_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_negative1_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_negative2_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_negative3_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_positive1_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_positive2_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_positive3_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_negative1_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_negative2_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_negative3_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_positive1_HdmiCecTx_source", "l1_hdmi_cec_driver_negative1_HdmiCecTx_source","l1_hdmi_cec_driver_negative2_HdmiCecTx_source","l1_hdmi_cec_driver_negative3_HdmiCecTx_source","l1_hdmi_cec_driver_negative4_HdmiCecTx_source","l1_hdmi_cec_driver_negative5_HdmiCecTx_source", "l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_source", "l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_source", "l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_source", "l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_source", "l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_source", "l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_source", "l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_source" };
        void (*list2[])() = {test_l1_hdmi_cec_driver_positive1_HdmiCecOpen, test_l1_hdmi_cec_driver_negative1_HdmiCecOpen, test_l1_hdmi_cec_driver_negative2_HdmiCecOpen, test_l1_hdmi_cec_driver_positive2_HdmiCecOpen, test_l1_hdmi_cec_driver_positive1_HdmiCecClose, test_l1_hdmi_cec_driver_positive2_HdmiCecClose, test_l1_hdmi_cec_driver_negative1_HdmiCecClose, test_l1_hdmi_cec_driver_negative2_HdmiCecClose, test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_source, test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_source, test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_source, test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_source, test_l1_hdmi_cec_driver_negative5_HdmiCecAddLogicalAddress_source, test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_source, test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_source, test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_source, test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_source, test_l1_hdmi_cec_driver_negative5_HdmiCecRemoveLogicalAddress_source, test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_source, test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_source, test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_source, test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_source, test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_source, test_l1_hdmi_cec_driver_positive1_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative1_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative2_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative3_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative4_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_positive1_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_positive2_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_positive3_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_negative1_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_negative2_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_negative3_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_positive1_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_positive2_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_positive3_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_negative1_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_negative2_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_negative3_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_positive1_HdmiCecTx_source, test_l1_hdmi_cec_driver_negative1_HdmiCecTx_source, test_l1_hdmi_cec_driver_negative2_HdmiCecTx_source, test_l1_hdmi_cec_driver_negative3_HdmiCecTx_source,test_l1_hdmi_cec_driver_negative4_HdmiCecTx_source, test_l1_hdmi_cec_driver_negative5_HdmiCecTx_source, test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_source, test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_source, test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_source, test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_source, test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_source, test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_source, test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_source};
    #else
        const char* list1[] = {"l1_hdmi_cec_driver_positive1_HdmiCecOpen", "l1_hdmi_cec_driver_negative1_HdmiCecOpen", "l1_hdmi_cec_driver_negative2_HdmiCecOpen", "l1_hdmi_cec_driver_positive2_HdmiCecOpen", "l1_hdmi_cec_driver_positive1_HdmiCecClose", "l1_hdmi_cec_driver_positive2_HdmiCecClose", "l1_hdmi_cec_driver_negative1_HdmiCecClose", "l1_hdmi_cec_driver_negative2_HdmiCecClose", "l1_hdmi_cec_driver_positive1_HdmiCecAddLogicalAddress_sink", "l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_sink", "l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_sink", "l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_sink", "l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_sink", "l1_hdmi_cec_driver_positive1_HdmiCecRemoveLogicalAddress_sink", "l1_hdmi_cec_driver_positive2_HdmiCecRemoveLogicalAddress_sink", "l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_sink", "l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_sink", "l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_sink", "l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_sink", "l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_sink", "l1_hdmi_cec_driver_positive2_HdmiCecGetLogicalAddress_sink", "l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_sink", "l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_sink", "l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_sink", "l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_sink", "l1_hdmi_cec_driver_positive1_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative1_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative2_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative3_HdmiCecGetPhysicalAddress", "l1_hdmi_cec_driver_negative4_HdmiCecGetPhysicalAddress",  "l1_hdmi_cec_driver_positive1_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_positive2_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_positive3_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_negative1_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_negative2_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_negative3_HdmiCecSetRxCallback", "l1_hdmi_cec_driver_positive1_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_positive2_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_positive3_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_negative1_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_negative2_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_negative3_HdmiCecSetTxCallback", "l1_hdmi_cec_driver_positive1_HdmiCecTx_sink", "l1_hdmi_cec_driver_negative1_HdmiCecTx_sink", "l1_hdmi_cec_driver_negative2_HdmiCecTx_sink", "l1_hdmi_cec_driver_negative3_HdmiCecTx_sink", "l1_hdmi_cec_driver_negative4_HdmiCecTx_sink", "l1_hdmi_cec_driver_negative5_HdmiCecTx_sink", "l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_sink", "l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_sink", "l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_sink", "l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_sink", "l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_sink", "l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_sink", "l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_sink"};
        void (*list2[])() = {test_l1_hdmi_cec_driver_positive1_HdmiCecOpen, test_l1_hdmi_cec_driver_negative1_HdmiCecOpen, test_l1_hdmi_cec_driver_negative2_HdmiCecOpen, test_l1_hdmi_cec_driver_positive2_HdmiCecOpen, test_l1_hdmi_cec_driver_positive1_HdmiCecClose, test_l1_hdmi_cec_driver_positive2_HdmiCecClose, test_l1_hdmi_cec_driver_negative1_HdmiCecClose, test_l1_hdmi_cec_driver_negative2_HdmiCecClose, test_l1_hdmi_cec_driver_positive1_HdmiCecAddLogicalAddress_sink, test_l1_hdmi_cec_driver_negative1_HdmiCecAddLogicalAddress_sink, test_l1_hdmi_cec_driver_negative2_HdmiCecAddLogicalAddress_sink, test_l1_hdmi_cec_driver_negative3_HdmiCecAddLogicalAddress_sink, test_l1_hdmi_cec_driver_negative4_HdmiCecAddLogicalAddress_sink, test_l1_hdmi_cec_driver_positive1_HdmiCecRemoveLogicalAddress_sink, test_l1_hdmi_cec_driver_positive2_HdmiCecRemoveLogicalAddress_sink, test_l1_hdmi_cec_driver_negative1_HdmiCecRemoveLogicalAddress_sink, test_l1_hdmi_cec_driver_negative2_HdmiCecRemoveLogicalAddress_sink, test_l1_hdmi_cec_driver_negative3_HdmiCecRemoveLogicalAddress_sink, test_l1_hdmi_cec_driver_negative4_HdmiCecRemoveLogicalAddress_sink, test_l1_hdmi_cec_driver_positive1_HdmiCecGetLogicalAddress_sink, test_l1_hdmi_cec_driver_positive2_HdmiCecGetLogicalAddress_sink, test_l1_hdmi_cec_driver_negative1_HdmiCecGetLogicalAddress_sink, test_l1_hdmi_cec_driver_negative2_HdmiCecGetLogicalAddress_sink, test_l1_hdmi_cec_driver_negative3_HdmiCecGetLogicalAddress_sink, test_l1_hdmi_cec_driver_negative4_HdmiCecGetLogicalAddress_sink, test_l1_hdmi_cec_driver_positive1_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative1_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative2_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative3_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_negative4_HdmiCecGetPhysicalAddress, test_l1_hdmi_cec_driver_positive1_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_positive2_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_positive3_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_negative1_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_negative2_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_negative3_HdmiCecSetRxCallback, test_l1_hdmi_cec_driver_positive1_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_positive2_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_positive3_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_negative1_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_negative2_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_negative3_HdmiCecSetTxCallback, test_l1_hdmi_cec_driver_positive1_HdmiCecTx_sink, test_l1_hdmi_cec_driver_negative1_HdmiCecTx_sink, test_l1_hdmi_cec_driver_negative2_HdmiCecTx_sink, test_l1_hdmi_cec_driver_negative3_HdmiCecTx_sink, test_l1_hdmi_cec_driver_negative4_HdmiCecTx_sink, test_l1_hdmi_cec_driver_negative5_HdmiCecTx_sink, test_l1_hdmi_cec_driver_positive1_HdmiCecTxAsync_sink, test_l1_hdmi_cec_driver_negative1_HdmiCecTxAsync_sink, test_l1_hdmi_cec_driver_negative2_HdmiCecTxAsync_sink, test_l1_hdmi_cec_driver_negative3_HdmiCecTxAsync_sink, test_l1_hdmi_cec_driver_negative4_HdmiCecTxAsync_sink, test_l1_hdmi_cec_driver_negative5_HdmiCecTxAsync_sink, test_l1_hdmi_cec_driver_negative6_HdmiCecTxAsync_sink};
    #endif
    // Add tests to the suite
    for (int i = 0; i < sizeof(list1) / sizeof(list1[0]); i++) {
        UT_add_test(pSuite, list1[i], list2[i]);
    }
    return 0;
}
int main(int argc, char** argv)
{
    int registerReturn = 0;
    /* Register tests as required, then call the UT-main to support switches and triggering */
    UT_init( argc, argv );
    /* Check if tests are registered successfully */
    registerReturn = register_hal_tests();
    if (registerReturn == 0)
    {
        printf("register_hal_tests() returned success");
    }
    else
    {
        printf("register_hal_tests() returned failure");
        return 1;
    }
    /* Begin test executions */
    UT_run_tests();
    return 0;
}