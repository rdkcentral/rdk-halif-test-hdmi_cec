/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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
* @file hdmicec_l1_tests.c
* @page HDMI_CEC_L1_Tests HDMI CEC Level 1 Tests
* **Component Owner:** Amit Patel@n
* **Component Architect:** Amit Patel@n
* **Review Team:** Amit Patel, Gerald Weatherup, Anjali Thampi@n
*
* ## Module's Role
* This module includes Level 1 functional tests (success and failure scenarios).
* This is to ensure that the API meets the operational requirements of the HDMI CEC across all vendors.
*
* **Pre-Conditions:**  None@n
* **Dependencies:** None@n
*
* Ref to API Definition specification documentation : [halSpec.md](../../../docs/halSpec.md)
*/


#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <ut.h>
#include "hdmi_cec_driver.h"
#ifdef __UT_STB__
    /**
     * Setting CEC play back logical address here.
     * More about CEC logical address read the documentation here
     * https://www.kernel.org/doc/html/v4.11/media/kapi/cec-core.html
     * https://elinux.org/CEC_(Consumer_Electronics_Control)_over_HDMI
     */
    #define DEFAULT_LOGICAL_ADDRESS 3
#else
    ///Setting the CEC sink (Dispaly divice) logical address here.
    #define DEFAULT_LOGICAL_ADDRESS 0
#endif

/**
 * @brief Ensure HdmiCecOpen api returns proper error codes, during all api invocation scenarios
 * 
 * HdmiCecOpen (): will initialize the module
 * Params of HdmiCecClose:
 *     handle - The handle used by application to uniquely identify the HAL instance.
 * Return of HdmiCecClose: respective HDMI_CEC_IO_ERROR
 * Expected error codes form HdmiCecOpen are
 * HDMI_CEC_IO_SUCCESS          - Success
 * HDMI_CEC_IO_INVALID_STATE    - Function is already open
 * HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error. Not able to simulate this scenario
 * 
 * 
 * This api ensure following conditions
 * 1. HdmiCecOpen is successful, if called first time during module initialization
 * 2. HdmiCecOpen will return HDMI_CEC_IO_INVALID_STATE during successive calls.
 * 4. Passing invalid handle address will return HDMI_CEC_IO_INVALID_ARGUMENT
 * 3. Once Hdmi Cec module is open. should be able to close the handle with HdmiCecClose api
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 001@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - repeat the call | handle | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * |03|call HdmiCecOpen(&hdmiHandle) - call api with invalid handle | handle | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |04|call HdmiCecClose(hdmiHandle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * 
 */
void test_hdmicec_hal_l1_open( void )
{
    int result;
    int handle = 0;
    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /*calling hdmicec_init second time should pass and return the valid pointer*/
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    result = HdmiCecOpen (NULL);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief Ensure HdmiCecClose api returns proper error codes, during all api invocation scenarios
 * 
 * HdmiCecClose() : will uninitialize the module
 * Params of HdmiCecClose:
 *     handle - The handle returned from the HdmiCecOpen() function.
 * Return of HdmiCecClose: respective HDMI_CEC_IO_ERROR
 * Expected error codes form HdmiCecClose are
 * HDMI_CEC_IO_SUCCESS          - Success
 * HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error.  Not able to simulate this scenario
 * 
 * This api ensure following conditions
 * 1. HdmiCecClose will return HDMI_CEC_IO_INVALID_STATE, if called before HdmiCecOpen is invoked.
 * 2. HdmiCecClose will return HDMI_CEC_IO_SUCCESS if a valid handle is passed.
 * 3. HdmiCecClose will return HDMI_CEC_IO_INVALID_STATE during successive calls.
 * 4. Passing invalid handle address will return HDMI_CEC_IO_INVALID_ARGUMENT
 * 5. Once Hdmi Cec module is open. should be able to close the handle with HdmiCecClose api
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 002@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecClose(hdmiHandle) - close interface even before opening it | handle | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecClose (handle) - call api with invalid handle | handle | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |04|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecClose() - close interface again | handle=hdmiHandle | HDMI_CEC_IO_INVALID_STATE| Should fail |
 */
void test_hdmicec_hal_l1_close( void )
{
    int result;
    int handle = 0;

    /*calling hdmicec_close before calling open, should return the invalid argument error*/
    result = HdmiCecClose (handle);//Code crash here
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    handle = INT_MAX;
    /*calling hdmicec_close with invalid handle should return the invalid argument error*/
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);  

    /*calling hdmicec_close second time */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}


/**
 * @brief Ensure HdmiCecAddLogicalAddress api returns proper error codes, during all api invocation scenarios
 * 
 * HdmiCecAddLogicalAddress (): will block until the intended logical address is secured by the module.
 * Params of HdmiCecAddLogicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     logicalAddress - The logical address to be acquired
 * Return of HdmiCecAddLogicalAddress: respective HDMI_CEC_IO_ERROR
 * 
 * Expected error codes form HdmiCecAddLogicalAddress are
 * @retval HDMI_CEC_IO_SUCCESS                    - POLL message is sent successfully and not ACK'd by any device on the bus.
 * @retval HDMI_CEC_IO_INVALID_STATE              - Module is not initialised
 * @retval HDMI_CEC_IO_INVALID_ARGUMENT           - Parameter passed to this function is invalid
 * @retval HDMI_CEC_IO_GENERAL_ERROR              - Underlying undefined platform error. Not able to simulate this scenario
 * @retval HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE - POLL message is sent and ACK'd by a device on the bus. Not able to simulate this scenario
 * @retval HDMI_CEC_IO_SENT_FAILED                - POLL message send failed. Not able to simulate this scenario
 * 
 * This api ensure following conditions
 * 1. Module is not initialised error is returned if api is called with out initialising the module.
 * 2. Able to successfully add the logical address once module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed api returns Module is not initialised error.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 003@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecAddLogicalAddress(handle, logicalAddress) - trying to add logical address even before opening the module | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api with invalid handle | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |05|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |06|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |07|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api after module is closed | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should fail |
 */
void test_hdmicec_hal_l1_addLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;

    //Calling api before open, should give invalid argument
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using invalid handle
    result = HdmiCecAddLogicalAddress(0, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecAddLogicalAddress(0, -1);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecAddLogicalAddress(0, 0xF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief Ensure HdmiCecRemoveLogicalAddress api returns proper error codes, during all api invocation scenarios.
 * 
 * HdmiCecRemoveLogicalAddress (): This function releases the previously acquired logical address
 * Params of HdmiCecRemoveLogicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     logicalAddress - The logicalAddresses to be released
 * Return of HdmiCecRemoveLogicalAddress: respective HDMI_CEC_IO_ERROR
 * Expected error codes form HdmiCecAddLogicalAddress are
 * HDMI_CEC_IO_SUCCESS          - Success
 * HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error. Not able to simulate this scenario
 * 
 * This api ensure following conditions
 * 1. Module is not initialised error is returned if api is called with out initialising the module.
 * 2. Removing the unallocated logical address should give success.
 * 3. Able to successfully remove the logical address once module is initialized.
 * 4. Removing logical address twice should give success.
 * 4. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 5. Once module is closed api returns Module is not initialised error.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 004@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - trying to remove logical address even before opening the module | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - removing unallocated logical address | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - remove allocated logical address | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - remove logical address again | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - call api with invalid handle | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |08|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - call api with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |09|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |10|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call api after module is closed | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should fail |
 */
void test_hdmicec_hal_l1_removeLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS;


    //Calling api before open, should give invalid argument
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress); //Code crash here
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress(0, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    //trying with invalid value
    logicalAddress = 0xF;
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    logicalAddress = -1;
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief Ensure HdmiCecGetLogicalAddress api returns proper error codes, during all api invocation scenarios.
 * 
 * HdmiCecGetLogicalAddress (): function get the logical address for the specified device type.
 * Params of HdmiCecGetLogicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     devType - The device type (tuner, record, playback etc.).
 *     logicalAddress - The logical address acquired. Should be valid address.
 * Return of HdmiCecGetLogicalAddress: respective HDMI_CEC_IO_ERROR
 * 
 * Expected error codes form HdmiCecAddLogicalAddress are
 * HDMI_CEC_IO_SUCCESS          - Success
 * HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * HDMI_CEC_IO_GENERAL_ERROR    - Underlying undefined platform error. Not able to simulate this scenario
 * 
 * This api ensure following conditions
 * 1. Module is not initialised error is returned if api is called with out initialising the module.
 * 2. Able to successfully get the logical address once module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed api returns Module is not initialised error.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 005@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - trying to get logical address even before opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call api with valid arguments after opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call api with invalid handle | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |09|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call api with invalid logical address handle | handle | devType | &logicalAddress  | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |10|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress)  - call api after module is closed | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should fail |
 */
void test_hdmicec_hal_l1_getLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    //Calling api before open, should give invalid argument
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using invalid devType
    result = HdmiCecGetLogicalAddress(0, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecGetLogicalAddress(0, devType,  NULL);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    //Using invalid devType
    devType = INT_MAX;//Trying some dev type
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT); printf ("\n%s: devType = INT_MAX Err result %d\n",__FUNCTION__, result);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    devType = 3;//Trying some dev type
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief callback to receive the hdmicec receive messages
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf receive message buffer passed
 * @param len receive message buffer length
 */
void DriverReceiveCallback(int handle, void *callbackData, unsigned char *buf, int len)
{
    int result = HDMI_CEC_IO_GENERAL_ERROR;
    printf ("\nBuffer generated: %x length: %d\n",buf, len);
    if (len>0){
        result = HDMI_CEC_IO_SUCCESS;
    }
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief callback to get the async send message status
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param result async send status.
 */
void DriverTransmitCallback(int handle, void *callbackData, int result)
{
    printf ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief Ensure HdmiCecSetRxCallback api returns proper error codes, during all api invocation scenarios.
 * 
 * HdmiCecSetRxCallback (): Sets CEC packet Receive callback.  
 *
 * HdmiCecSetRxCallback function sets a callback function to be invoked for each packet arrival.   
 * The packet contained in the buffer is expected to follow this format:
 *
 * (ref <HDMI Specification 1-4> Section <CEC 6.1>)
 * 
 * complete packet = header block + data block;
 * header block = destination logical address (4-bit) + source address (4-bit)
 * data   block = opcode block (8-bit) + operand block (N-bytes)
 * 
 * Params of HdmiCecSetRxCallback:
 * handle                    - The handle returned from the HdmiCecOpen(() function
 * cbfunc                    - Function pointer to be invoked when a complete packet is received
 * data                      - Callback data
 * 
 * Expected error codes form HdmiCecAddLogicalAddress are
 * HDMI_CEC_IO_SUCCESS          - Success
 * HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * 
 * This api ensure following conditions
 * 1. Able to successfully set the RX callback even before module is initialized.
 * 2. Able to successfully set the RX callback after module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Able to successfully set the RX callback after module is closed.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 006@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0) - trying set RX callback even before opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0) - set RX call back with valid parameters | handle | DriverReceiveCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|call HdmiCecSetRxCallback(0, NULL, 0) - set RX call back with invalid handle | handle | DriverReceiveCallback | data address  | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |08|call HdmiCecSetRxCallback(handle, NULL, 0) - set RX call back with NULL callback | handle | DriverReceiveCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0)  - call api after module is closed | handle | DriverReceiveCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_setRxCallback( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This dummy variable

    //Calling api before open, should pass
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    //Set logical address for TV.
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using NULL callback
    result = HdmiCecSetRxCallback(0, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should return success
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief Ensure HdmiCecSetTxCallback api returns proper error codes, during all api invocation scenarios.
 * 
 * HdmiCecSetTxCallback (): function sets a callback function to be invoked once the async transmit
 * result is available. This is only necessary if application choose to transmit
 * the packet asynchronously.
 *
 * HdmiCecSetTxCallback function should block if callback invocation is in progress.
 * Params of HdmiCecSetRxCallback:
 * handle                    - The handle returned from the HdmiCecOpen() function.
 * cbfunc                    - Function pointer to be invoked when a complete packet is transmitted
 * data                      - Callback data
 * Return of HdmiCecSetRxCallback: respective HDMI_CEC_IO_ERROR
 * 
 * Expected error codes form HdmiCecAddLogicalAddress are
 * HDMI_CEC_IO_SUCCESS          - Success
 * HDMI_CEC_IO_INVALID_STATE    - Module is not initialised
 * HDMI_CEC_IO_INVALID_ARGUMENT - Parameter passed to this function is invalid
 * 
 * This api ensure following conditions
 * 1. Able to successfully set the TX callback even before module is initialized.
 * 2. Able to successfully set the TX callback after module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Able to successfully set the TX callback after module is closed.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 007@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0) - trying set TX callback even before opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0) - set TX call back with valid parameters | handle | DriverTransmitCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecSetTxCallback(0, NULL, 0) - set TX call back with invalid handle | handle | DriverTransmitCallback | data address  | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |05|call HdmiCecSetTxCallback(handle, NULL, 0)  - set RX call back with NULL callback | handle | DriverTransmitCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0)  - call api after module is closed | handle | DriverTransmitCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_setTxCallback( void )
{
    int result;
    int handle = 0;

    //Calling api before open, should pass
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using NULL callback
    result = HdmiCecSetTxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using NULL callback
    result = HdmiCecSetTxCallback(0, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should return success
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief Ensure HdmiCecTx api returns proper error codes, during all api invocation scenarios.
 * 
 * HdmiCecTx (): function writes a complete CEC packet onto the CEC bus and wait for ACK.
 * application should check for result when return value of the function is 0;
 *
 * The bytes in buf that is to be transmitted should follow the buffer
 * byte format required for receiving buffer. (See detailed description from 
 * HdmiCecSetRxCallback)
 *
 * Params of HdmiCecTx:
 *  handle                              - The handle returned from the HdmiCecOpen() function.
 *  buf                                 - The buffer contains a complete CEC packet to send.
 *  len                                 - Number of bytes in the packet.
 *  result                             - send status buffer. Possible results are SENT_AND_ACKD,
 *                    SENT_BUT_NOT_ACKD (e.g. no follower at the destination),
 *                    SENT_FAILED (e.g. collision).
 * Return of HdmiCecTx: respective HDMI_CEC_IO_ERROR
 * 
 * Expected error codes form HdmiCecAddLogicalAddress are
 * HDMI_CEC_IO_SUCCESS                    - Success
 * HDMI_CEC_IO_INVALID_STATE              - Module is not initialised
 * HDMI_CEC_IO_INVALID_ARGUMENT           - Parameter passed to this function is invalid
 * HDMI_CEC_IO_GENERAL_ERROR              - Underlying undefined platform error. Not able to simulate this scenario
 * HDMI_CEC_IO_SENT_AND_ACKD              - Cec message is send and acknowledged. Not able to simulate this scenario
 * HDMI_CEC_IO_SENT_BUT_NOT_ACKD          - Message send but not acknowledged by the receiver. Not able to simulate this scenario
 * HDMI_CEC_IO_SENT_FAILED                - send message failed. Not able to simulate this scenario
 * HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE - receiver logical address is not available. Not able to simulate this scenario
 *  
 * This api ensure following conditions
 * 1. Module is not initialised error, is returned if api is called with out initialising the module.
 * 2. Able to successfully transmit the cec messages once module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed api returns Module is not initialised error.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 008@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecTx(handle, buf, len, &ret) - trying to send cec messages, before opening the module | handle | buf | len | &ret | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecTx(handle, buf, len, &ret) - send the cec message after proper module initialization | handle | buf | len | &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTx(handle, buf, len, NULL) - call cec message send with invalid argument | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |07|call HdmiCecTx(handle, NULL, len, &ret) - call cec message send with invalid argument | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |08|call HdmiCecTx(0, buf, len, &ret) - call cec message send with invalid argument | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |09|call HdmiCecTx(handle, buf, INT_MIN, &ret) - call cec message send with invalid argument | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |10|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecTx(handle, buf, len, &ret)  - call api after module is closed | handle | buf | len | &ret | HDMI_CEC_IO_INVALID_STATE| Should fail |
 */
void test_hdmicec_hal_l1_hdmiCecTx( void )
{
    int result=HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This is dummy variable

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, 0x9F};

    //Calling api before open, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    //Set logical address for TV.
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf[0] = 0x0F; printf ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    printf ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Invalid input */
    result = HdmiCecTx(handle, buf, len, NULL);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid input */
    result = HdmiCecTx(handle, NULL, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid input */
    result = HdmiCecTx(0, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid input */
    result = HdmiCecTx(handle, buf, INT_MIN, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief Ensure HdmiCecTxAsync api returns proper error codes, during all api invocation scenarios.
 * 
 * HdmiCecTxAsync (): function writes a complete CEC packet onto the CEC bus but does not wait 
 * for ACK. The result will be reported via HdmiCecRxCallback_t if return value
 * of this function is 0.
 *
 * Params of HdmiCecTxAsync:
 *  handle                              - The handle returned from the HdmiCecOpen() function.
 *  buf                                 - The buffer contains a complete CEC packet to send.
 *  len                                 - Number of bytes in the packet.
 * Return of HdmiCecTxAsync: respective HDMI_CEC_IO_ERROR
 * 
 * Expected error codes form HdmiCecAddLogicalAddress are
 * HDMI_CEC_IO_SUCCESS                    - Success
 * HDMI_CEC_IO_INVALID_STATE              - Module is not initialised
 * HDMI_CEC_IO_INVALID_ARGUMENT           - Parameter passed to this function is invalid
 * HDMI_CEC_IO_GENERAL_ERROR              - Underlying undefined platform error. Not able to simulate this scenario
 * 
 * This api ensure following conditions
 * 1. Module is not initialised error, is returned if api is called with out initialising the module.
 * 2. Able to successfully transmit the cec messages once module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed api returns Module is not initialised error.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 009@n
 * **Priority:** Medium@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** Module must be installed for use@n
 * **User Interaction:** N/A
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecTxAsync(handle, buf, len) - trying to send cec messages, before opening the module | handle | buf | len | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecTxAsync(handle, buf, len) - send the cec message after proper module initialization | handle | buf | len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTxAsync(handle, NULL, len) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |07|call HdmiCecTx(handle, NULL, len, &ret) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |08|call HdmiCecTxAsync(0, buf, len) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |09|call HdmiCecTxAsync(handle, buf, INT_MIN) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should fail |
 * |10|call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecTxAsync(handle, buf, len)  - call api after module is closed | handle | buf | len | HDMI_CEC_IO_INVALID_STATE| Should fail |
 * 
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This is dummy variable

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, 0x9F};

    //Calling api before open, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len); //Code crash here
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV.
    logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf[0] = 0x0F; printf ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Invalid input */
    result = HdmiCecTxAsync(handle, NULL, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecTxAsync(0, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid input */
    result = HdmiCecTxAsync(handle, buf, INT_MIN);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

static UT_test_suite_t *pSuite = NULL;

/**
 * @brief Register the main tests for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int test_hdmicec_hal_l1_register( void )
{
    /* add a suite to the registry */
    pSuite = UT_add_suite("[L1 hdmicec-generic]", NULL, NULL);
    if (NULL == pSuite)
    {
        return -1;
    }

    UT_add_test( pSuite, "open", test_hdmicec_hal_l1_open);
    UT_add_test( pSuite, "close", test_hdmicec_hal_l1_close);
    UT_add_test( pSuite, "addLogicalAddress", test_hdmicec_hal_l1_addLogicalAddress);
    UT_add_test( pSuite, "removeLogicalAddress", test_hdmicec_hal_l1_removeLogicalAddress);
    UT_add_test( pSuite, "getLogicalAddress", test_hdmicec_hal_l1_getLogicalAddress);
    UT_add_test( pSuite, "setRxCallback", test_hdmicec_hal_l1_setRxCallback);
    UT_add_test( pSuite, "setTxCallback", test_hdmicec_hal_l1_setTxCallback);
    UT_add_test( pSuite, "hdmiCecTx", test_hdmicec_hal_l1_hdmiCecTx);
    UT_add_test( pSuite, "hdmiCecTxAsync", test_hdmicec_hal_l1_hdmiCecTxAsync);

    return 0;
}
