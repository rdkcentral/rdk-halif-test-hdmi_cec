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
#include <stdbool.h>
#include <time.h>

#include <ut.h>
#include "hdmi_cec_driver.h"

//Set the MACRO for the stb platforms
//#define __UT_STB__ 1

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

#define CEC_GET_CEC_VERSION (0x9F)
#define CEC_DEVICE_VENDOR_ID (0x87)

//#TODO This section will be replaced with UT_LOG() when the feature is available
#define CEC_ENABLE_CEC_LOG_DEBUG 1 // Set to 0 to disable debug logging
#define CEC_ENABLE_CEC_LOG_WARNING 1 // Set to 0 to disable warning logging
#define CEC_ENABLE_CEC_LOG_INFO 1 // Set to 0 to disable info logging

#define CEC_LOG_DEBUG(...) do { \
    if (CEC_ENABLE_CEC_LOG_DEBUG) { \
        printf("\n[DEBUG] " __VA_ARGS__); \
        printf("\n"); \
    } \
} while (0)

#define CEC_LOG_WARNING(...) do { \
    if (CEC_ENABLE_CEC_LOG_WARNING) { \
        printf("\n[WARNING] " __VA_ARGS__); \
        printf("\n"); \
    } \
} while (0)

#define CEC_LOG_INFO(...) do { \
    if (CEC_ENABLE_CEC_LOG_INFO) { \
        printf("\n[INFO] " __VA_ARGS__); \
        printf("\n"); \
    } \
} while (0)

#define CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time) \
    do { \
        start_time = clock(); \
    } while (0)

#define CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, responseTimeInMilliSecs) \
    do { \
        end_time = clock(); \
        responseTimeInMilliSecs = ((double) (end_time - start_time)) / (CLOCKS_PER_SEC / 1000); \
    } while (0)

/**
 * @brief Ensure HdmiCecOpen() returns correct error codes during all of this API's invocation scenarios
 * 
 * This test case ensures the following conditions :
 * 1. HdmiCecOpen() is successful, if called first time during module initialization
 * 2. HdmiCecOpen() will return HDMI_CEC_IO_INVALID_STATE during successive calls
 * 4. Passing invalid handle address will return HDMI_CEC_IO_INVALID_ARGUMENT
 * 3. Once Hdmi Cec module is open, should be able to close the handle with HdmiCecClose()
 * HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 001@n
 * 
 * **Pre-Conditions:**@n
 * None.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - call API with invalid handle | handle | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecOpen() - repeat the Call | handle | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |04|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * 
 */
void test_hdmicec_hal_l1_open( void )
{
    int result;
    int handle = 0;

    //Check Null even before calling the positive case.
    result = HdmiCecOpen( NULL );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecOpen( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );
    
    result = HdmiCecOpen( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

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
 * HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 002@n
 * 
 * **Pre-Conditions:**@n
 * None.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None@n
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecClose() - close interface even before opening it | handle | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecClose () - call with invalid handle | handle=0 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |03|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecClose () - close interface again | handle=hdmiHandle | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_close( void )
{
    int result;
    int handle = 0;

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecClose( 0 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );  

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );
}

/**
 * @brief Ensure HdmiCecGetPhysicalAddress() not crashing, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. API is not crashing if called without initialising
 * 5. Passing invalid handle to API is not crashing it.
 * 6. Once module is closed, API is not crashing
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 003@n
 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecGetPhysicalAddress() - trying to get logical address, even before opening the module | handle, physicalAddress | None| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call the api, with invalid handle | handle=0, physicalAddress | None| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call api with invalid physical address | handle, physicalAddress=NULL  | None| Should Pass |
 * |05|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecGetLogicalAddress()  - call the api after module is closed | handle, physicalAddress | None| Should Pass |
 */
void test_hdmicec_hal_l1_getPhysicalAddress( void )
{
    int result = 0;
    int handle = 0;
    unsigned int physicalAddress = 0;

    HdmiCecGetPhysicalAddress(handle, &physicalAddress);

    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    HdmiCecGetPhysicalAddress(0, &physicalAddress);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close,
    HdmiCecGetPhysicalAddress(handle, &physicalAddress);
}



/**
 * @brief Ensure HdmiCecAddLogicalAddress() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for sink devices.
 * 
 * This test ensure following conditions :
 * 1. Module not initialised error is returned if called without initialising
 * 2. Sink device should able to successfully add the logical address once module is initialized
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Once module is closed, return not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 004@n
 * 
 * **Pre-Conditions:**@n
 * None.
 * 
 * **Dependencies:**None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecAddLogicalAddress() - trying to add logical address even before opening the module | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecAddLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=-1 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecAddLogicalAddress() - call with invalid logical address. Only supported value is 0 | handle, logicalAddress=0x03 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=0x0F | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecAddLogicalAddress() - call with valid arguments. API is applicable only for sink devices.  | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass for sink devices |
 * |08|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecAddLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_addLogicalAddress_sinkDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    result = HdmiCecAddLogicalAddress( 0, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( handle, -1 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( handle, 0x3 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( handle, 0xF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

}

/**
 * @brief Ensure HdmiCecAddLogicalAddress() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for source devices.
 * This test ensure following conditions :
 * 1. Module not initialised error is returned if called without initialising
 * 2. Source device shouldn't support this API.
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Once module is closed, return not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 005@n
 * 
 * **Pre-Conditions:**@n
 * None.
 * 
 * **Dependencies:**None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecAddLogicalAddress() - trying to add logical address even before opening the module | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecAddLogicalAddress() - call with valid arguments. API is not valid for source devices. | handle, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should pass. |
 * |04|Call HdmiCecAddLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=-1 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecAddLogicalAddress() - call with invalid logical address. Only supported value is 0 | handle, logicalAddress=0x03 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=0x0F | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecAddLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_addLogicalAddress_sourceDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    result = HdmiCecAddLogicalAddress( 0, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( handle, -1 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( handle, 0x3 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecAddLogicalAddress( handle, 0xF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

}

/**
 * @brief Ensure HdmiCecRemoveLogicalAddress() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for sink devices.
 * 
 * This test ensure following conditions :
 * 1. Module not initialised error is returned if called without initialising
 * 2. Removing the unallocated logical address should give success for sink devices.
 * 3. Sink devices should able to successfully remove the logical address once module is initialized
 * 4. Removing logical address twice should give success for sink devices.
 * 5. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 6. Once module is closed, API invocation should return not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 006@n
 * 
 * **Pre-Conditions:**@n
 * None.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecRemoveLogicalAddress() - trying to remove logical address even before opening the module | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecRemoveLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|Call HdmiCecRemoveLogicalAddress() - call with invalid logical address | handle, logicalAddress=0xF | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=-1 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecRemoveLogicalAddress() - removing unallocated logical address | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass|
 * |07|Call HdmiCecRemoveLogicalAddress() - remove allocated logical address. API only applicable for sink devices. | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass|
 * |08|Call HdmiCecRemoveLogicalAddress() - remove same logical address again. API only applicable for sink devices. | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecAddLogicalAddress() - call with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|Call HdmiCecRemoveLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_removeLogicalAddress_sinkDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    result = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    result = HdmiCecRemoveLogicalAddress( 0, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecRemoveLogicalAddress( handle, 0xF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecRemoveLogicalAddress( handle,  -1 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //TODO if we try to remove it again. Should throw an error. It should return invalid argument. It will be done in next phase.
    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecClose(handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

}


/**
 * @brief Ensure HdmiCecRemoveLogicalAddress() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for source devices.
 * 
 * This test ensure following conditions :
 * 1. Module not initialised error is returned if called without initialising
 * 2. Source devices shouldn't support remove API
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Once module is closed return not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 007@n
 * 
 * **Pre-Conditions:**@n
 * None.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecRemoveLogicalAddress() - trying to remove logical address even before opening the module | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecRemoveLogicalAddress() - remove API is not applicable for source devices.| handle, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should pass|
 * |04|Call HdmiCecRemoveLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecRemoveLogicalAddress() - call with invalid logical address | handle, logicalAddress=0xF | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |09|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecRemoveLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_removeLogicalAddress_sourceDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    result = HdmiCecOpen(&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecRemoveLogicalAddress( 0, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    logicalAddress = 0xF;
    result = HdmiCecRemoveLogicalAddress( handle, 0xF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    logicalAddress = -1;
    result = HdmiCecRemoveLogicalAddress( handle, -1 );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT );

    result = HdmiCecClose(handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

}


/**
 * @brief Ensure HdmiCecGetLogicalAddress() returns correct error codes, during all of this API's invocation scenarios
 *  This test case is only applicable for sink devices.
 * TODO: Unused variable devType in HdmiCecGetLogicalAddress. Need to remove.
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Sink devices should get the same logical address added by the caller
 * 3. Sink devices should return 0xF logical address after remove api is invoked.
 * 5. Passing invalid handle to API, returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 6. Once module is closed, API returns not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 008@n
 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecGetLogicalAddress() - trying to get logical address, even before opening the module | handle, devType, &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call the api, with invalid handle | handle=0, devType, &logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call api with invalid logical address | handle, devType, &logicalAddress=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecGetLogicalAddress() - call api before add logical address, should return 0x0F | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecGetLogicalAddress() - call api with valid arguments; should return the logical address added by the caller | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecGetLogicalAddress() - call api with valid arguments; should return 0x0F once the logical address is removed. | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecGetLogicalAddress()  - call the api after module is closed | handle, devType, &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sinkDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int logicalAddressCrossCheck = INT_MIN;
    int devType = 0;//Trying some dev type.

    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    result = HdmiCecGetLogicalAddress(0, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecGetLogicalAddress(handle, devType,  NULL);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddressCrossCheck);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_TRUE( (int)0xF!= logicalAddressCrossCheck);

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddressCrossCheck);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_TRUE( logicalAddress!= logicalAddressCrossCheck);

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddressCrossCheck);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_TRUE( (int)0xF!= logicalAddressCrossCheck);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    devType = 3;//Trying some dev type
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
}

/**
 * @brief Ensure HdmiCecGetLogicalAddress() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for source devices.
 * TODO: Unused variable devType in HdmiCecGetLogicalAddress. Need to remove.
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Able to successfully get the logical address once module is initialized. Source devices should return a valid logical address between 0x00 and 0x0F.
 * 3. Passing invalid handle to API, returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed, API returns not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 009@n
 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecGetLogicalAddress() - trying to get logical address, even before opening the module | handle, devType, &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call the api, with invalid handle | handle=0, devType, &logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call api with invalid logical address | handle, devType, &logicalAddress=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecGetLogicalAddress() - call api with valid arguments in source devices should return a valid logical address between 0x00 and 0x0F, excluding both the values. | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecGetLogicalAddress()  - call the api after module is closed | handle, devType, &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sourceDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type.

    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    result = HdmiCecGetLogicalAddress(0, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecGetLogicalAddress(handle, devType,  NULL);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_TRUE(logicalAddress>0x0E && logicalAddress<0x01);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    devType = 3;//Trying some dev type
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
}

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
    CEC_LOG_DEBUG ("\nBuffer generated: %x length: %d\n",buf, len);
    UT_ASSERT_TRUE(len>0); 
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL((bool)(!callbackData));
    UT_ASSERT_PTR_NULL((bool)(!buf));
    //UT_ASSERT_TRUE( (unsigned long long)callbackData== (unsigned long long)0xDEADBEEF);
    //TODO need to identify why callback is not equal
    CEC_LOG_DEBUG ("\nCall back data generated is \n");
    for (int index=0; index < len; index++) {
        CEC_LOG_DEBUG ("buf at index : %d is %x", index, buf[index]);
    }
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
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL((bool)(!callbackData));
    //UT_ASSERT_TRUE( (unsigned long long)callbackData== (unsigned long long)0xDEADBEEF);
    //TODO need to identify why callback is not equal
    CEC_LOG_DEBUG ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief Ensure HdmiCecSetRxCallback() returns, correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. Setting RX callback even before module is initialized, Should fail.
 * 2. Able to successfully set the RX callback, after module is initialized.
 * 3. Passing invalid handle to the API, returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Setting the RX callback after the module is closed should fail.
 * 5. Setting the RX callback with invalid args after the module is closed should fail.
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 010@n
 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecSetRxCallback() - trying to set RX callback even before opening the module | handle ,devType, &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecAddLogicalAddress() - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecSetRxCallback() - set RX Call back with invalid handle | handle=0, DriverReceiveCallback | data address  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecSetRxCallback() - set RX Call back with valid parameters | handle, DriverReceiveCallback, data=0xDEADBEEF | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecSetRxCallback() - unset the RX Call back with NULL | handle, DriverReceiveCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecSetRxCallback()  - call api after module is closed | handle, DriverReceiveCallback, data address | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |10|Call HdmiCecSetRxCallback()  - call api after module is closed with invalid parameter | handle=0, DriverReceiveCallback, data address | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_setRxCallback( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This dummy variable

    //Calling api before open, should pass
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    //Set logical address for TV.
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using NULL callback
    result = HdmiCecSetRxCallback(0, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);


    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should return success
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    HdmiCecSetRxCallback(0, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
}

/**
 * @brief Ensure HdmiCecSetTxCallback() returns correct error codes, during all of this API's invocation scenarios
 * 
 * This test ensure following conditions:
 * 1. Setting TX callback even before initialization, should pass
 * 2. Able to successfully set the TX callback after the initialization
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Setting the TX callback after module is closed, should fail.
 * 5. Setting the TX callback with invalid args after the module is closed should fail.
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 011@n
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
 * |01|Call HdmiCecSetTxCallback() - trying set TX callback even before opening the module | handle, devType, &logicalAddress | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetTxCallback() - set TX Call back with invalid handle | handle=0, DriverTransmitCallback, data address  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|Call HdmiCecSetTxCallback() - set TX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecSetTxCallback()  - set RX Call back with NULL callback | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecSetTxCallback()  - call the API, after module is closed | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |08|Call HdmiCecSetTxCallback()  - call the API, after module is closed with invalid handle | handle=0, DriverTransmitCallback, data address | HDMI_CEC_IO_INVALID_STATE| Should Pass | 
 */
void test_hdmicec_hal_l1_setTxCallback( void )
{
    int result;
    int handle = 0;

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecOpen ( &handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    result = HdmiCecSetTxCallback(0, NULL, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback( handle, NULL, (void*)0xDEADBEEF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecClose( handle );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );

    result = HdmiCecSetTxCallback( 0, DriverTransmitCallback, (void*)0xDEADBEEF );
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE );
}

/**
 * @brief Ensure HdmiCecTx() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for sink devices.
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Sink device should able to successfully transmit the cec messages once module is initialized and logical address is added.
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Once module is closed returns not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 *  HDMI_CEC_IO_SENT_BUT_NOT_ACKD : is verified part of L2. since all the device disconnected tests handled in L2
 *  HDMI_CEC_IO_SENT_FAILED : Underlying bus error. cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 012@n
 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecTx() - trying to send cec messages, before opening the module | handle, buf, len, &ret | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecTx() - trying to send cec message, for sink device even before setting logical address | handle, buf, len, &ret | HDMI_CEC_IO_SENT_FAILED| Specific to sink devices. Should Pass |
 * |04|Call HdmiCecAddLogicalAddress() - call add logical address for sink devices with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Specific to sink devices. Should Pass |
 * |05|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments. API should return the logical address added in the above step | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecTx() - invoke send cec message with invalid ret | handle, buf, len, &ret=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecTx() - invoke send cec message with invalid buffer | handle, buf=NULL, len, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecTx() - invoke send cec message with invalid handle | handle=0, buf, len, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |09|Call HdmiCecTx() - invoke send cec message with invalid buffer length | handle, buf, len=INT_MIN, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |10|Call HdmiCecTx() - send the cec message with valid arguments after module initialization | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|Call HdmiCecTx() - back to back send to ensure it is passing | handle, buf, len. &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |12|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |13|Call HdmiCecTx()  -  invoke send cec message once module is closed | handle, buf, len, &ret | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sinkDevice( void )
{
    int result=HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This is dummy variable

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    //Calling api before open, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SENT_FAILED);

    //Set logical address for TV.
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    buf[0] = 0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
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

    CEC_LOG_DEBUG ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed.*/
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
}

/**
 * @brief Ensure HdmiCecTx() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for source devices.
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Source device should able to successfully transmit the cec messages once module is initialized
 * 3. Passing invalid handle returns HDMI_CEC_IO_INVALID_ARGUMENT
 * 4. Once module is closed returns not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 *  HDMI_CEC_IO_SENT_BUT_NOT_ACKD : is verified part of L2
 *  HDMI_CEC_IO_SENT_FAILED : Underlying bus error. cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 013@n
 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecTx() - trying to send cec messages, before opening the module | handle, buf, len, &ret | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecTx() - invoke send cec message with invalid ret | handle, buf, len, &ret=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecTx() - invoke send cec message with invalid buffer | handle, buf=NULL, len, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecTx() - invoke send cec message with invalid handle | handle=0, buf, len, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecTx() - invoke send cec message with invalid buffer length | handle, buf, len=INT_MIN, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecTx() - send the cec message with valid arguments after module initialization | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecTx() - back to back send to ensure it is passing | handle, buf, len. &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|Call HdmiCecTx()  -  invoke send cec message once module is closed | handle, buf, len, &ret | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sourceDevice( void )
{
    //#TODO transmit back to back functionality also.
    int result=HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This is dummy variable

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    //Calling api before open, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    buf[0] = 0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
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

    CEC_LOG_DEBUG ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed.*/
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
}

/**
 * @brief Ensure HdmiCecTxAsync() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for sink devices.
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Sink device is able to successfully transmit the cec messages once module is initialized and logical address is added.
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed returns not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 014@n

 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecTxAsync() - trying to send cec messages, before opening the module | handle, buf, len | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecAddLogicalAddress() - call add logical address with valid arguments| handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf=NULL, len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle=0, buf, len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf, len=INT_MIN | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecTxAsync() - send the cec message after correct module initialization | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecTxAsync() - back to back send to ensure it is passing | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|Call HdmiCecTxAsync()  - call api after module is closed | handle, buf, len | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sinkDevice( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This is dummy variable

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    //Calling api before open, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len); //Code crash here
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV.
    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SENT_FAILED);

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    buf[0] = 0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Invalid input */
    result = HdmiCecTxAsync(handle, NULL, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecTxAsync(0, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid input */
    result = HdmiCecTxAsync(handle, buf, INT_MIN);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);


    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed.*/
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
}

/**
 * @brief Ensure HdmiCecTxAsync() returns correct error codes, during all of this API's invocation scenarios.
 *  This test case is only applicable for source devices.
 * 
 * This test ensure following conditions:
 * 1. Module not initialised error is returned if called without initialising
 * 2. Source device is able to successfully transmit the cec messages once module is initialized
 * 3. Passing invalid handle to api returns HDMI_CEC_IO_INVALID_ARGUMENT.
 * 4. Once module is closed returns not initialised error
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 015@n

 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecTxAsync() - trying to send cec messages, before opening the module | handle, buf, len | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf=NULL, len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle=0, buf, len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf, len=INT_MIN | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecTxAsync() - send the cec message after correct module initialization | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|Call HdmiCecTxAsync() - back to back send to ensure it is passing | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecTxAsync()  - call api after module is closed | handle, buf, len | HDMI_CEC_IO_INVALID_STATE| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sourceDevice( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This is dummy variable

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    //Calling api before open, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len); //Code crash here
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //if init is failed no need to proceed further.
    if (HDMI_CEC_IO_SUCCESS != result) { return; }

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    buf[0] = 0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Invalid input */
    result = HdmiCecTxAsync(handle, NULL, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecTxAsync(0, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid input */
    result = HdmiCecTxAsync(handle, buf, INT_MIN);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; CEC_LOG_DEBUG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed.*/
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);
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

    //
    UT_add_test( pSuite, "open", test_hdmicec_hal_l1_open);
    UT_add_test( pSuite, "close", test_hdmicec_hal_l1_close);
    UT_add_test( pSuite, "getPhysicalAddress", test_hdmicec_hal_l1_getPhysicalAddress);

    UT_add_test( pSuite, "setRxCallback", test_hdmicec_hal_l1_setRxCallback);
    UT_add_test( pSuite, "setTxCallback", test_hdmicec_hal_l1_setTxCallback);

#ifndef __UT_STB__
    UT_add_test( pSuite, "addLogicalAddressSink", test_hdmicec_hal_l1_addLogicalAddress_sinkDevice);
    UT_add_test( pSuite, "removeLogicalAddressSink", test_hdmicec_hal_l1_removeLogicalAddress_sinkDevice);
    UT_add_test( pSuite, "getLogicalAddressSink", test_hdmicec_hal_l1_getLogicalAddress_sinkDevice);
    UT_add_test( pSuite, "hdmiCecTxSink", test_hdmicec_hal_l1_hdmiCecTx_sinkDevice);
    UT_add_test( pSuite, "hdmiCecTxAsyncSink", test_hdmicec_hal_l1_hdmiCecTxAsync_sinkDevice);
#else
    UT_add_test( pSuite, "addLogicalAddressSource", test_hdmicec_hal_l1_addLogicalAddress_sourceDevice);
    UT_add_test( pSuite, "removeLogicalAddressSource", test_hdmicec_hal_l1_removeLogicalAddress_sourceDevice);
    UT_add_test( pSuite, "getLogicalAddressSource", test_hdmicec_hal_l1_getLogicalAddress_sourceDevice);
    UT_add_test( pSuite, "hdmiCecTxSource", test_hdmicec_hal_l1_hdmiCecTx_sourceDevice);
    UT_add_test( pSuite, "hdmiCecTxAsyncSource", test_hdmicec_hal_l1_hdmiCecTxAsync_sourceDevice);
#endif

    return 0;
}
