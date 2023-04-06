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
     * Set CEC play back logical address here
     * More about CEC logical address read the documentation here
     * https://www.kernel.org/doc/html/v4.11/media/kapi/cec-core.html
     * https://elinux.org/CEC_(Consumer_Electronics_Control)_over_HDMI
     */
    #define DEFAULT_LOGICAL_ADDRESS 3
#else
    /// Set the CEC sink (Display device) logical address here
    #define DEFAULT_LOGICAL_ADDRESS 0
#endif

/**
 * @brief Ensure HdmiCecOpen() returns correct error codes during all of this API's invocation scenarios
 * 
 * This test case ensures the following conditions :
 * 1. HdmiCecOpen() is successful, if called first time during module initialization
 * 2. HdmiCecOpen() will return HDMI_CEC_IO_INVALID_STATE during successive calls
 * 4. Passing invalid handle address will return HDMI_CEC_IO_INVALID_ARGUMENT
 * 3. Once Hdmi Cec module is open, should be able to close the handle with HdmiCecClose()
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 001@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - repeat the call | handle | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |03|call HdmiCecOpen(&hdmiHandle) - call API with invalid handle | handle | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|call HdmiCecClose(hdmiHandle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * 
 */
void test_hdmicec_hal_l1_open( void )
{
    int result;
    int handle = 0;

    result = HdmiCecOpen( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecOpen( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen( NULL );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

}

/**
 * @brief Ensure HdmiCecClose() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This ensure the following conditions :
 * 1. HdmiCecClose() will return HDMI_CEC_IO_INVALID_STATE, if called before HdmiCecOpen() is invoked
 * 2. HdmiCecClose() will return HDMI_CEC_IO_SUCCESS if a valid handle is passed
 * 3. HdmiCecClose() will return HDMI_CEC_IO_INVALID_STATE during successive calls
 * 4. Passing invalid handle address will return HDMI_CEC_IO_INVALID_ARGUMENT
 * 5. Once Hdmi Cec module is open. should be able to close the handle with HdmiCecClose()
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 002@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecClose(hdmiHandle) - close interface even before opening it | handle | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecClose (handle) - call with invalid handle | handle | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecClose (handle) - close interface again | handle=hdmiHandle | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_close( void )
{
    int result;
    int handle = 0;

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecClose( NULL );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );  

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );
}


/**
 * @brief Ensure HdmiCecAddLogicalAddress() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions :
 * 1. Module not initialised error is returned if called without initialising
 * 2. Able to successfully add the logical address once module is initialized
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Once module is closed, return not initialised error
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 003@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:**None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecAddLogicalAddress(handle, logicalAddress) - trying to add logical address even before opening the module | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(0, logicalAddress) - call with invalid handle | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|call HdmiCecAddLogicalAddress(0, -1) - call with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|call HdmiCecAddLogicalAddress(0, 0xF) - call with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call after module is closed | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_addLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( 0, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( 0, -1 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( 0, 0xF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

}

/**
 * @brief Ensure HdmiCecRemoveLogicalAddress() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions :
 * 1. Module not initialised error is returned if called without initialising
 * 2. Removing the unallocated logical address should give success
 * 3. Able to successfully remove the logical address once module is initialized
 * 4. Removing logical address twice should give success.
 * 4. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 5. Once module is closed return not initialised error
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 004@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - trying to remove logical address even before opening the module | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - removing unallocated logical address | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - remove allocated logical address | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - remove same logical address again | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecRemoveLogicalAddress(0, logicalAddress) - call with invalid handle | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - call with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |09|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call with invalid logical address | handle | logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |10|call HdmiCecClose(handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecRemoveLogicalAddress(handle, logicalAddress) - call after module is closed | handle | logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_removeLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS;


    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    result = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress( 0, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    logicalAddress = 0xF;
    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    logicalAddress = -1;
    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecClose(handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

}

/**
 * @brief Ensure HdmiCecGetLogicalAddress() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Able to successfully get the logical address once module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed returns not initialised error
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 005@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecGetLogicalAddress(handle, devType, &logicalAddress) - trying to get logical address even before opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecGetLogicalAddress(handle, devType, &logicalAddress) - call api with valid arguments after opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecGetLogicalAddress(0, devType, &logicalAddress) - call api with invalid handle | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |09|call HdmiCecGetLogicalAddress(0, devType, NULL) - call api with invalid logical address handle | handle | devType | &logicalAddress  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |10|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecGetLogicalAddress(handle, devType, &logicalAddress)  - call api after module is closed | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
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
 * @brief Ensure HdmiCecSetRxCallback() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. Setting RX callback even before module is initialized, Should Pass.
 * 2. Able to successfully set the RX callback after module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Setting the RX callback after module is closed should, fail.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 006@n

 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0) - trying to set RX callback even before opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0) - set RX call back with valid parameters | handle | DriverReceiveCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|call HdmiCecSetRxCallback(0, NULL, 0) - set RX call back with invalid handle | handle | DriverReceiveCallback | data address  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|call HdmiCecSetRxCallback(handle, NULL, 0) - set RX call back with NULL callback | handle | DriverReceiveCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0)  - call api after module is closed | handle | DriverReceiveCallback | data address | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_setRxCallback( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This dummy variable

    //Calling api before open, should pass
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

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
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief Ensure HdmiCecSetTxCallback() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. Setting TX callback even before initialization should pass
 * 2. Able to successfully set the TX callback after initialization
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Setting the TX callback after module is closed, should pass
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 007@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0) - trying set TX callback even before opening the module | handle | devType | &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0) - set TX call back with valid parameters | handle | DriverTransmitCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecSetTxCallback(0, NULL, 0) - set TX call back with invalid handle | handle | DriverTransmitCallback | data address  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|call HdmiCecSetTxCallback(handle, NULL, 0)  - set RX call back with NULL callback | handle | DriverTransmitCallback | data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0)  - call after module is closed | handle | DriverTransmitCallback | data address | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_setTxCallback( void )
{
    int result;
    int handle = 0;

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, 0 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen ( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, 0 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback(0, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecSetTxCallback( handle, NULL, 0 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, 0 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );
}

/**
 * @brief Ensure HdmiCecTx() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Able to successfully transmit the cec messages once module is initialized
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Once module is closed returns not initialised error
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 008@n
 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecTx(handle, buf, len, &ret) - trying to send cec messages, before opening the module | handle | buf | len | &ret | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecTx(handle, buf, len, &ret) - send the cec message after correct module initialization | handle | buf | len | &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTx(handle, buf, len, NULL) - call cec message send with invalid ret | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|call HdmiCecTx(handle, NULL, len, &ret) - call cec message send with invalid buffer | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|call HdmiCecTx(0, buf, len, &ret) - call cec message send with invalid handle | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |09|call HdmiCecTx(handle, buf, INT_MIN, &ret) - call cec message send with invalid buffer length | handle | buf | len | &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |10|call HdmiCecClose(handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecTx(handle, buf, len, &ret)  - call module is closed | handle | buf | len | &ret | HDMI_CEC_IO_INVALID_STATE| Should Pass |
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

#ifndef __UT_STB__
    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    //Set logical address for TV.
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
#endif

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
 * @brief Ensure HdmiCecTxAsync() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Able to successfully transmit the cec messages once module is initialized.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed returns not initialised error
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 009@n

 * 
 * **Pre-Conditions:**@n
 * None
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|call HdmiCecTxAsync(handle, buf, len) - trying to send cec messages, before opening the module | handle | buf | len | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle | logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle | devType | &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecTxAsync(handle, buf, len) - send the cec message after correct module initialization | handle | buf | len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTxAsync(handle, NULL, len) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|call HdmiCecTx(handle, NULL, len, &ret) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|call HdmiCecTxAsync(0, buf, len) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |09|call HdmiCecTxAsync(handle, buf, INT_MIN) - call cec message send with invalid argument | handle | buf | len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |10|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|call HdmiCecTxAsync(handle, buf, len)  - call api after module is closed | handle | buf | len | HDMI_CEC_IO_INVALID_STATE| Should Pass |
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

#ifndef __UT_STB__
    //Set logical address for TV.
    logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
#endif

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
