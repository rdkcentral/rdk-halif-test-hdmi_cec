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
 * @defgroup HDMI_CEC_HALTESTS_L1 HDMI CEC HAL Tests L1 File
 * @{
 * @parblock
 *
 * ### L1 Test Cases for HDMI CEC HAL :
 *
 * This module includes Level 1 functional tests (success and failure scenarios).
 * This is to ensure that the API meets the operational requirements of the module across all vendors.
 *
 * **Pre-Conditions:** @n
 * **Dependencies:** None@n
 *
 * Refer to API Definition specification documentation : [hdmi-cec_halSpec.md](../../docs/pages/hdmi-cec_halSpec.md)
 *
 * @endparblock
 *
 */

/**
 * @file test_l1_hdmi_cec_driver.c
 *
 */

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>

#include <ut.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>
#include "hdmi_cec_driver.h"

/**
     * Set CEC play back logical address here
     * More about CEC logical address read the documentation here
     * https://www.kernel.org/doc/html/v4.11/media/kapi/cec-core.html
     * https://elinux.org/CEC_(Consumer_Electronics_Control)_over_HDMI
*/
#define DEFAULT_LOGICAL_ADDRESS_STB 3

/// Set the CEC sink (Display device) logical address here
#define DEFAULT_LOGICAL_ADDRESS_PANEL 0

static int gTestGroup = 1;
static int gTestID = 1;
static bool extendedEnumsSupported=false;

#define CEC_GET_CEC_VERSION (0x9F)
#define CEC_BROADCAST_ADDR (0xF)

#define CHECK_FOR_EXTENDED_ERROR_CODE( result, enhanced, old )\
{\
   if ( extendedEnumsSupported == true )\
   {\
      UT_ASSERT_EQUAL( enhanced, result );\
   }\
   else\
   {\
       UT_ASSERT_EQUAL( old, result );\
   }\
}


/**
 * @brief Status variable to check if ping is triggered
 *
 */
bool cec_isPingTriggeredl1_g = false;

/**
 * @brief Semaphore to notify the CEC callback
 *
 */
sem_t cec_seml1_g;

/**
 * @brief variable to hold the semaphore time out
 *
 */
struct timespec cec_tsl1_g;


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
    UT_LOG ("\nBuffer generated: %x length: %d\n",buf, len);
    if((handle!=0) && (callbackData !=NULL) && (len>0)) {
            cec_isPingTriggeredl1_g = true;
            UT_LOG("\nCall back data generated is \n");
            for (int index=0; index < len; index++) {
                    UT_LOG("buf at index : %d is %x", index, buf[index]);
            }
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
}

/**
 * @note This API is no more needed as the test using this API is deprecated.
 * @brief callback to get the async send message status
 *
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param result async send status
 */
void DriverTransmitCallback(int handle, void *callbackData, int result )
{
   if((handle!=0) && (callbackData !=NULL)) {
           //UT_ASSERT_TRUE_FATAL( (unsigned long long)callbackData== (unsigned long long)0xDEADBEEF);
           UT_LOG ("\ncallbackData returned: %x result: %d\n",callbackData, result);
   }
}
/**
 * @brief Validate negative scenarios for  HdmiCecOpen()
 *
 * This test case ensures the following conditions :
 * 1. HdmiCecOpen() is successful, if called first time during module initialization
 * 2. HdmiCecOpen() will return HDMI_CEC_IO_ALREADY_OPEN during successive calls
 * 4. Passing invalid handle address will return HDMI_CEC_IO_INVALID_ARGUMENT
 * 3. Once Hdmi Cec module is open, should be able to close the handle with HdmiCecClose()
 * HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
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
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - call API with invalid handle | handle | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecOpen() - repeat the Call | handle | HDMI_CEC_IO_ALREADY_OPEN| Should Pass |
 * |04|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 */
void test_hdmicec_hal_l1_open_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 1;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    //Check Null even before calling the positive case
    result = HdmiCecOpen( NULL );
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("open failed"); }

    result = HdmiCecOpen( &handle );
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_ALREADY_OPEN,HDMI_CEC_IO_SUCCESS);

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Validate positive scenarios for HdmiCecOpen()
 *
 * This test case ensures the following conditions :
 * 1. HdmiCecOpen() is successful, if called first time during module initialization
 * 2. Once Hdmi Cec module is open, should be able to close the handle with HdmiCecClose()
 * HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 002@n
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
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 */
void test_hdmicec_hal_l1_open_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 2;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("open failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Validate negative scenarios for HdmiCecClose()
 *
 * HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 004@n
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
 * |01|Call HdmiCecClose() - close interface even before opening it | handle | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecClose () - call with invalid handle | handle=0 | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecClose () - close interface again | handle=hdmiHandle | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_close_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 4;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecClose( handle );
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("open failed"); }

    result = HdmiCecClose( 0 );
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    result = HdmiCecClose( handle );
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for HdmiCecClose()
 *
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 005@n
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
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 */
void test_hdmicec_hal_l1_close_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 5;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("open failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("open failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate negative scenarios for HdmiCecGetPhysicalAddress()
 *
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 006@n
 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecGetPhysicalAddress() - trying to get logical address, even before opening the module | handle, physicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetPhysicalAddress() - call the API with invalid handle | handle=0, physicalAddress | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecGetPhysicalAddress() - call API with invalid physical address | handle, physicalAddress=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|Call HdmiCecGetPhysicalAddress() - call API with valid physical address pointer and ensure address is in its range | handle, physicalAddress  | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecClose() - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecGetPhysicalAddress()  - call the API after module is closed | handle, physicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 *
 *
 */
void test_hdmicec_hal_l1_getPhysicalAddress_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    unsigned int physicalAddress = 0;
    gTestID = 6;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_GENERAL_ERROR);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetPhysicalAddress(0, &physicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_HANDLE);

    result = HdmiCecGetPhysicalAddress(handle, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL("HdmiCecGetPhysicalAddress failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    //Calling API after close,
    result = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_GENERAL_ERROR);
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for HdmiCecGetPhysicalAddress()
 *
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 007@n
 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecGetPhysicalAddress() - call API with valid handle and physical address pointer | handle, physicalAddress  | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecClose() - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 */
void test_hdmicec_hal_l1_getPhysicalAddress_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    unsigned int physicalAddress = 0;
    gTestID = 7;
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_KVP_PROFILE_GET_STRING("hdmicec/type",typeString);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    if (HDMI_CEC_IO_SUCCESS  != result) { UT_FAIL("HdmiCecGetPhysicalAddress failed"); }
    if(((strncmp(typeString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0) && ((physicalAddress > 0xffff) || (physicalAddress <= 0x0000) )) || \
            ((strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0) && (physicalAddress != 0x0000))){
	    UT_FAIL("Invalid physicalAddress");
    }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate negative scenarios for HdmiCecAddLogicalAddress()
 *
 *  This test case is only applicable for sink devices
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 008@n
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
 * |01|Call HdmiCecAddLogicalAddress() - trying to add logical address even before opening the module | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecAddLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=-1 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=0x10 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecAddLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 *
 * HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE case is updated in separate L1 case
 */
void test_hdmicec_hal_l1_addLogicalAddress_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = INT_MAX;
    gTestID = 8;
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_KVP_PROFILE_GET_STRING("hdmicec/type",typeString);
    if (strncmp(typeString, "source", UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {

        result = HdmiCecAddLogicalAddress(handle, logicalAddress);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

        result = HdmiCecOpen(&handle);
        // if init is failed no need to proceed further
        UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecAddLogicalAddress(0, logicalAddress);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

        result = HdmiCecAddLogicalAddress(handle, -1);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

        result = HdmiCecAddLogicalAddress(handle, 0x10);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

        result = HdmiCecClose(handle);
        UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecAddLogicalAddress(handle, logicalAddress);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

    } else if(strncmp(typeString, "sink", UT_KVP_MAX_ELEMENT_SIZE) == 0){
        result = HdmiCecAddLogicalAddress(handle, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_NOT_OPENED, HDMI_CEC_IO_INVALID_ARGUMENT);

        result = HdmiCecOpen(&handle);
        // if init is failed no need to proceed further
        UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecAddLogicalAddress(0, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_INVALID_HANDLE, HDMI_CEC_IO_INVALID_ARGUMENT);

        result = HdmiCecAddLogicalAddress(handle, -1);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_INVALID_ARGUMENT, HDMI_CEC_IO_GENERAL_ERROR);

        result = HdmiCecAddLogicalAddress(handle, 0x10);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_INVALID_ARGUMENT, HDMI_CEC_IO_GENERAL_ERROR);

        result = HdmiCecClose(handle);
        UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecAddLogicalAddress(handle, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_NOT_OPENED, HDMI_CEC_IO_INVALID_ARGUMENT);

    }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Validate positive scenarios for HdmiCecAddLogicalAddress()
 *
 *  This test case is only applicable for sink devices
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 009@n
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
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecAddLogicalAddress() - call with valid arguments. API is applicable only for sink devices  | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass for sink devices |
 * |03|Call HdmiCecRemoveLogicalAddress() - call with valid arguments. API is applicable only for sink devices  | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass for sink devices |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 * HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE case is updated in separate L1 case
 */
void test_hdmicec_hal_l1_addLogicalAddress_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;
    gTestID = 9;
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_KVP_PROFILE_GET_STRING("hdmicec/type",typeString);
    if(strncmp(typeString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0)
        logicalAddress = DEFAULT_LOGICAL_ADDRESS_STB;

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (((strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0) && (HDMI_CEC_IO_SUCCESS != result)) || \
            ((strncmp(typeString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0) && (HDMI_CEC_IO_OPERATION_NOT_SUPPORTED != result))){
        UT_FAIL("HdmiCecAddLogicalAddress call failed");
    }

    //Remove Logic address for Sink devices.
    if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {
        result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );
    }
    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Validate negative scenarios for  HdmiCecRemoveLogicalAddress()
 *
 *  This test case is only applicable for sink devices
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 012@n
 *
 * **Pre-Conditions:**@n
 * None.
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecRemoveLogicalAddress() - trying to remove logical address even before opening the module | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecRemoveLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecRemoveLogicalAddress() - call with invalid logical address | handle, logicalAddress=0x10 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |04|Call HdmiCecRemoveLogicalAddress() - call with invalid logical address | handle, logicalAddress=-1  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecRemoveLogicalAddress() - Try to remove with out adding the logical address| handle, logicalAddress | HDMI_CEC_IO_NOT_ADDED| Should Pass |
 * |06|Call HdmiCecAddLogicalAddress() - call with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecRemoveLogicalAddress() - remove allocated logical address.  | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass|
 * |08|Call HdmiCecRemoveLogicalAddress() - remove same logical address again. | handle, logicalAddress | HDMI_CEC_IO_NOT_ADDED| Should Pass |
 * |09|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecRemoveLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_removeLogicalAddress_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;
    gTestID = 12;
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    UT_KVP_PROFILE_GET_STRING("hdmicec/type", typeString);
    if (strncmp(typeString, "source", UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {
        logicalAddress = DEFAULT_LOGICAL_ADDRESS_STB;
        result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_NOT_OPENED, HDMI_CEC_IO_INVALID_ARGUMENT);

        result = HdmiCecOpen(&handle);
        // if init is failed no need to proceed further
        UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecRemoveLogicalAddress(0, logicalAddress);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

        result = HdmiCecRemoveLogicalAddress(handle, 0x10);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

        result = HdmiCecRemoveLogicalAddress(handle, -1);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

        result = HdmiCecClose(handle);
        UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
        UT_ASSERT_EQUAL(result,HDMI_CEC_IO_OPERATION_NOT_SUPPORTED);

    } else if (strncmp(typeString, "sink", UT_KVP_MAX_ELEMENT_SIZE) == 0){
        result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_NOT_OPENED, HDMI_CEC_IO_INVALID_ARGUMENT);

        result = HdmiCecOpen(&handle);
        // if init is failed no need to proceed further
        UT_ASSERT_EQUAL_FATAL(result, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecRemoveLogicalAddress(0, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_INVALID_HANDLE, HDMI_CEC_IO_INVALID_ARGUMENT);

        result = HdmiCecRemoveLogicalAddress(handle, 0x10);
        if (HDMI_CEC_IO_INVALID_ARGUMENT != result)
        {
            UT_FAIL("HdmiCecRemoveLogicalAddress failed");
        }

        result = HdmiCecRemoveLogicalAddress(handle, -1);
        if (HDMI_CEC_IO_INVALID_ARGUMENT != result)
        {
            UT_FAIL("HdmiCecRemoveLogicalAddress failed");
        }

        result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_NOT_ADDED, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecAddLogicalAddress(handle, logicalAddress);
        if (HDMI_CEC_IO_SUCCESS != result)
        {
            UT_FAIL("HdmiCecAddLogicalAddress failed");
        }

        result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
        if (HDMI_CEC_IO_SUCCESS != result)
        {
            UT_FAIL("HdmiCecRemoveLogicalAddress failed");
        }

        result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_NOT_ADDED, HDMI_CEC_IO_SUCCESS);

        result = HdmiCecClose(handle);
        if (HDMI_CEC_IO_SUCCESS != result)
        {
            UT_FAIL_FATAL("close failed");
        }

        result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
        CHECK_FOR_EXTENDED_ERROR_CODE(result, HDMI_CEC_IO_NOT_OPENED, HDMI_CEC_IO_INVALID_ARGUMENT);
    }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Validate positive scenarios for HdmiCecRemoveLogicalAddress()
 *
 *  This test case is only applicable for sink devices
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 013@n
 *
 * **Pre-Conditions:**@n
 * None.
 *
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecAddLogicalAddress() - call with a valid logical address | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecRemoveLogicalAddress() - remove allocated logical address. This API is only applicable for sink devices. | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass|
 * |04|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 */
void test_hdmicec_hal_l1_removeLogicalAddress_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;
    gTestID = 13;
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_KVP_PROFILE_GET_STRING("hdmicec/type",typeString);
    if(strncmp(typeString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0)
        logicalAddress = DEFAULT_LOGICAL_ADDRESS_STB;
    result = HdmiCecOpen(&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {
        result = HdmiCecAddLogicalAddress( handle, logicalAddress );
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );
    }
 
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    if (((strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0) && (HDMI_CEC_IO_SUCCESS != result)) || \
            ((strncmp(typeString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0) && (HDMI_CEC_IO_OPERATION_NOT_SUPPORTED != result))){
        UT_FAIL("HdmiCecRemoveLogicalAddress call failed");
    }

    result = HdmiCecClose(handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}



/**
 * @brief Validate negative scenarios for HdmiCecGetLogicalAddress()
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 015@n
 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecGetLogicalAddress() - trying to get logical address, even before opening the module | handle, &logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call the api, with invalid handle | handle=0, &logicalAddress | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call API with invalid logical address | handle, &logicalAddress=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecGetLogicalAddress() - call API before, HdmiCecAddLogicalAddress() call, should return 0x0F | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecGetLogicalAddress() - call API with valid arguments; should return the logical address added by the caller | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecGetLogicalAddress() - call API with valid arguments; should return 0x0F once the logical address is removed. | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecGetLogicalAddress()  - call the API after module is closed | handle, &logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sinkDevice_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;
    int logicalAddressCrossCheck = INT_MIN;
    gTestID = 15;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(0, &logicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecGetLogicalAddress(handle, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddressCrossCheck);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }
    if((int)0xF!= logicalAddressCrossCheck){
	    UT_FAIL("Invalid logicalAddress ");
    }

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddressCrossCheck);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }
    if(logicalAddress != logicalAddressCrossCheck){
	    UT_FAIL("logicalAddress and logicalAddressCrossCheck are not same");
    }

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddressCrossCheck);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }
    if((int)0xF!= logicalAddressCrossCheck){
	    UT_FAIL("Invalid logicalAddress");
    }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for HdmiCecGetLogicalAddress()
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 016@n
 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecAddLogicalAddress() - add logical address
 * |03|Call HdmiCecGetLogicalAddress() - call API with valid arguments; should return the logical address added by the caller | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sinkDevice_positive ( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 16;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }
    if(logicalAddress != 0x0f){
             UT_FAIL("Invalid logicalAddress");
     }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate negative scenarios for HdmiCecGetLogicalAddress()
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 017@n
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
 * |01|Call HdmiCecGetLogicalAddress() - trying to get logical address, even before opening the module | handle, &logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call the api, with invalid handle | handle=0, &logicalAddress | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call API with invalid logical address | handle, &logicalAddress=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecGetLogicalAddress() - call API with valid arguments in source devices should return a valid logical address between 0x01 and 0x0F, excluding both the values. | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecGetLogicalAddress()  - call the API after module is closed | handle, &logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sourceDevice_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 17;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecGetLogicalAddress(handle,  &logicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(0,  &logicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecGetLogicalAddress(handle, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }
    if(logicalAddress<0 || logicalAddress>0x0F){
	    UT_FAIL("Invalid logicalAddress");
    }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for HdmiCecGetLogicalAddress()
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 018@n
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
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecGetLogicalAddress() - call API with valid arguments in source devices should return a valid logical address between 0x00 and 0x0F, excluding both the values. | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sourceDevice_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;

    gTestID = 18;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }
    if(logicalAddress< 0 || (logicalAddress>0x0F)){
            UT_LOG("Invalid logicalAddress 0x%x\n",logicalAddress);
    }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate negative scenarios for HdmiCecSetRxCallback()
 *
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 019@n
 *
 *
 * **Pre-Conditions:**None@n
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecSetRxCallback() - call the API before open | handle, DriverReceiveCallback, data | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetRxCallback() - call the API with invalid arguments | handle=0, DriverReceiveCallback, data  | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass|
 * |05|Call HdmiCecSetRxCallback() - call the API after close | handle, DriverReceiveCallback, data | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 *
 */
void test_hdmicec_hal_l1_setRxCallback_negative ( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 19;
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_KVP_PROFILE_GET_STRING("hdmicec/type",typeString);

    //Calling API before open, should pass
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );
    //Add Logic Address for Sink devices.
    if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {
        result = HdmiCecAddLogicalAddress( handle, logicalAddress );
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );
    }

    result = HdmiCecSetRxCallback(0, DriverReceiveCallback, (void*)0xDEADBEEF);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    //Remove Logic address for Sink devices.
    if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {
        result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );
    }
    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    //Calling API after close, should return success
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for  HdmiCecSetRxCallback()
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 020@n
 *
 *
 * **Pre-Conditions:**None@n
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecAddLogicalAddress() - Add Logic address for Sink device | handle, logic address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetRxCallback() - set RX Call back with valid parameters | handle, DriverReceiveCallback, data=0xDEADBEEF | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecSetRxCallback()  - set RX Call back with NULL callback | handle, Null pointer, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecRemoveLogicalAddress() - Remove Logic address for Sink device | handle, logic address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 */
void test_hdmicec_hal_l1_setRxCallback_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 20;
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_KVP_PROFILE_GET_STRING("hdmicec/type",typeString);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );
    //Add Logic address for Sink devices
    if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {
        result = HdmiCecAddLogicalAddress( handle, logicalAddress );
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );
    }
    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetRxCallback failed"); }

    result = HdmiCecSetRxCallback( handle, NULL, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetRxCallback failed"); }

    //Remove Logic address for Sink devices.
    if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0)
    {
        result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );
    }
    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @note This test is deprecated as API is deprecated
 * @brief Ensure HdmiCecSetTxCallback() returns correct error codes, during the negative API invocation scenarios
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 021@n
 *
 * **Pre-Conditions:**None@n
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecSetTxCallback() - trying set TX callback even before opening the module | handle, devType, &logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetTxCallback() - set TX Call back with invalid handle | handle=0, DriverTransmitCallback, data address  | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecSetTxCallback()  - call the API, after module is closed | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_NOT_OPENED| Should Pass | 
 */
void test_hdmicec_hal_l1_setTxCallback_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 21;


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecOpen ( &handle );
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback(0, NULL, (void*)0xDEADBEEF);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @note This test is deprecated as API is deprecated
 * @brief Ensure HdmiCecSetTxCallback() returns success, during the positive API invocation scenarios
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 022@n
 *
 * **Pre-Conditions:**None@n
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecSetTxCallback() - set TX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetTxCallback()  - set TX Call back with NULL callback | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 *
 */
void test_hdmicec_hal_l1_setTxCallback_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    gTestID = 22;


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen ( &handle );
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    result = HdmiCecSetTxCallback( handle, NULL, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Ensure HdmiCecTx() returns correct error codes, during all of this API's invocation scenarios
 *  This test case is only applicable for sink devices
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *  HDMI_CEC_IO_SENT_BUT_NOT_ACKD : is verified part of L2. since all the device disconnected tests handled in L2
 *  HDMI_CEC_IO_SENT_FAILED : Underlying bus error. cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 023@n
 *
 * **Pre-Conditions:** None@n
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecTx() - trying to send cec messages, before opening the module | handle, buf, len, &ret | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecTx() - trying to send cec message, for sink device even before setting logical address | handle, buf, len, &ret | HDMI_CEC_IO_SENT_FAILED| Specific to sink devices. Should Pass |
 * |04|Call HdmiCecAddLogicalAddress() - call add logical address for sink devices with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Specific to sink devices. Should Pass |
 * |05|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments. API should return the logical address added in the above step | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecTx() - invoke send cec message with invalid ret | handle, buf, len, &ret=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecTx() - invoke send cec message with invalid buffer | handle, buf=NULL, len, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecTx() - invoke send cec message with invalid handle | handle=0, buf, len, &ret  | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |09|Call HdmiCecTx() - invoke send cec message with invalid buffer length | handle, buf, len=INT_MIN, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |10|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|Call HdmiCecTx()  -  invoke send cec message once module is closed | handle, buf, len, &ret | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_negative( void )
{
    HDMI_CEC_STATUS result =HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 23;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    //Calling API before open, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;

    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SENT_FAILED != result) { UT_FAIL("HdmiCecTx failed"); }

    //Set logical address for TV
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecAddLogicalAddress failed"); }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    /* Invalid return variable */
    result = HdmiCecTx(handle, buf, len, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTx failed"); }

    /* Invalid buffer */
    result = HdmiCecTx(handle, NULL, len, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTx failed"); }

    /* Invalid device handle */
    result = HdmiCecTx(0, buf, len, &ret);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid length */
    result = HdmiCecTx(handle, buf, INT_MIN, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTx failed"); }

    /* Remove Logical address*/
    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecRemoveLogicalAddress failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Ensure HdmiCecTx() returns success, during the positive API's invocation scenarios
 *  This test case is only applicable for sink devices
 *
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 024@n
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
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecAddLogicalAddress() - call add logical address for sink devices with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Specific to sink devices. Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments. API should return the logical address added in the above step | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecTx() - send the cec message with valid arguments after module initialization | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS and ret=HDMI_CEC_IO_SENT_AND_ACKD| Should Pass |
 * |02|Call HdmiCecremoveLogicalAddress() - call add logical address for sink devices with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Specific to sink devices. Should Pass |
 * |05|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int initiatorAddress = 0;
    int destinationAddress = 0;
    gTestID = 24;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 0 and to CEC address 3
    unsigned char buf[] = {0x03, CEC_GET_CEC_VERSION};


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;

    //Set logical address for TV
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecAddLogicalAddress failed"); }

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle,  &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }


    // Extract and log initiator and destination addresses from buf
    initiatorAddress = (buf[0] & 0xF0) >> 4;    // Upper nibble of buf[0]
    destinationAddress = (buf[0] & 0x0F);       // Lower nibble of buf[0]
    UT_LOG("\n hdmicec initiator address (from buf): 0x%x\n", initiatorAddress);
    UT_LOG("\n hdmicec destination address (from buf): 0x%x\n", destinationAddress);
    buf[0] = ((logicalAddress&0xFF)<<4)|(0x03); UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_BUT_NOT_ACKD  != ret) { UT_FAIL("HdmiCecTx failed"); }

    /* Remove Logical address*/
    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecRemoveLogicalAddress failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}


/**
 * @brief Ensure HdmiCecTx() returns correct error codes, during the negative API's invocation scenarios
 *  This test case is only applicable for source devices
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulate
 *  HDMI_CEC_IO_SENT_BUT_NOT_ACKD : is verified part of disconnected test of L1
 *  HDMI_CEC_IO_SENT_FAILED : Underlying bus error. cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 025@n
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
 * |01|Call HdmiCecTx() - trying to send cec messages, before opening the module | handle, buf, len, &ret | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecTx() - invoke send cec message with invalid ret | handle, buf, len, &ret=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecTx() - invoke send cec message with invalid buffer | handle, buf=NULL, len, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecTx() - invoke send cec message with invalid handle | handle=0, buf, len, &ret  | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |07|Call HdmiCecTx() - invoke send cec message with invalid buffer length | handle, buf, len=INT_MIN, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecTx()  -  invoke send cec message once module is closed | handle, buf, len, &ret | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sourceDevice_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 25;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    //Calling API before open, should give not open error
    result = HdmiCecTx(handle, buf, len, &ret);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    /* Invalid return variable */
    result = HdmiCecTx(handle, buf, len, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTx failed"); }

    /* Invalid buffer */
    result = HdmiCecTx(handle, NULL, len, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTx failed"); }

    /* Invalid device handle */
    result = HdmiCecTx(0, buf, len, &ret);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid message length */
    result = HdmiCecTx(handle, buf, INT_MIN, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTx failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Ensure HdmiCecTx() returns correct success, during positive API's invocation scenarios
 *  This test case is only applicable for source devices
 *
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 026@n
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
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecTx() - send the cec message with valid arguments after module initialization | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS and ret=HDMI_CEC_IO_SENT_AND_ACKD| Should Pass |
 * |04|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sourceDevice_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 26;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    UT_LOG ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_BUT_NOT_ACKD != ret) { UT_FAIL("HdmiCecTx failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @note This test is deprecated as API is deprecated
 * @brief Ensure HdmiCecTxAsync() returns correct error codes, during all of the negative API invocation scenarios.
 *  This test case is only applicable for sink devices
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 028@n

 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecTxAsync() - trying to send cec messages, before opening the module | handle, buf, len | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecAddLogicalAddress() - call add logical address with valid arguments| handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf=NULL, len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle=0, buf, len | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |07|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf, len=INT_MIN | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecTxAsync()  - call API after module is closed | handle, buf, len | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sinkDevice_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 28;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    //Calling API before open, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len); //Code crash here
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    //Set logical address for TV
    logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;

    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SENT_FAILED != result) { UT_FAIL("HdmiCecTxAsync failed"); }

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) {
        UT_FAIL("HdmiCecAddLogicalAddress failed");
    }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    /* Invalid buffer */
    result = HdmiCecTxAsync(handle, NULL, len);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTxAsync failed"); }

    result = HdmiCecTxAsync(0, buf, len);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid message length */
    result = HdmiCecTxAsync(handle, buf, INT_MIN);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTxAsync failed"); }


    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @note This test is deprecated as API is deprecated
 * @brief Ensure HdmiCecTxAsync() returns success, during all of the positive API's invocation scenarios.
 *  This test case is only applicable for sink devices
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 029@n

 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecAddLogicalAddress() - call add logical address with valid arguments| handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecTxAsync() - send the cec message after correct module initialization | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sinkDevice_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 29;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    //Set logical address for TV
    logicalAddress = DEFAULT_LOGICAL_ADDRESS_PANEL;

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecAddLogicalAddress failed"); }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }


    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecTxAsync failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @note This test is deprecated as API is deprecated
 * @brief Ensure HdmiCecTxAsync() returns correct error codes, during all of the negative API invocation scenarios
 *  This test case is only applicable for source devices
 *
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 030@n

 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecTxAsync() - trying to send cec messages, before opening the module | handle, buf, len | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf=NULL, len | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle=0, buf, len | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |06|Call HdmiCecTxAsync() - call cec message send with invalid argument | handle, buf, len=INT_MIN | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecTxAsync()  - call API after module is closed | handle, buf, len | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sourceDevice_negative( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 30;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    //Calling API before open, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len); //Code crash here
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    /* Invalid buffer */
    result = HdmiCecTxAsync(handle, NULL, len);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTxAsync failed"); }

    result = HdmiCecTxAsync(0, buf, len);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_INVALID_HANDLE,HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid message length */
    result = HdmiCecTxAsync(handle, buf, INT_MIN);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL("HdmiCecTxAsync failed"); }

    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len);
    CHECK_FOR_EXTENDED_ERROR_CODE(result,HDMI_CEC_IO_NOT_OPENED,HDMI_CEC_IO_INVALID_ARGUMENT);

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @note This test is deprecated as API is deprecated
 * @brief Ensure HdmiCecTxAsync() returns success, during all of the positive API's invocation scenarios
 *  This test case is only applicable for source devices
 *
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 031@n

 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device
 *
 * **Dependencies:** None@n
 * **User Interaction:** None
 *
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecTxAsync() - send the cec message after correct module initialization | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sourceDevice_positive( void )
{
    HDMI_CEC_STATUS result = HDMI_CEC_IO_SUCCESS;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 31;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecGetLogicalAddress failed"); }

    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecTxAsync failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}


static UT_test_suite_t *pSuite = NULL;


/**
 * @brief Register sink tests
 *
 * @return int - 0 on success, otherwise failure
 */
int test_hdmidec_hal_l1_register(void)
{
    char typeString[UT_KVP_MAX_ELEMENT_SIZE];

    extendedEnumsSupported = UT_KVP_PROFILE_GET_BOOL("hdmicec/features/extendedEnumsSupported");

    UT_KVP_PROFILE_GET_STRING("hdmicec/type",typeString);
    if(strncmp(typeString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0){
        pSuite = UT_add_suite("[L1 HDMI CEC Source TestCase]", NULL, NULL);
    }
     if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0){
        pSuite = UT_add_suite("[L1 HDMI CEC SINK TestCase]", NULL, NULL);
    }
    if (NULL == pSuite)
    {
        return -1;
    }

    if(strncmp(typeString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0){
        UT_add_test( pSuite, "getLogicalAddress_Positive", test_hdmicec_hal_l1_getLogicalAddress_sinkDevice_positive);
        UT_add_test( pSuite, "getLogicalAddress_negative", test_hdmicec_hal_l1_getLogicalAddress_sinkDevice_negative);
        UT_add_test( pSuite, "Tx_Positive", test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_positive);
        UT_add_test( pSuite, "Tx_negative", test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_negative);
    }

    if(strncmp(typeString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0){
        UT_add_test( pSuite, "getLogicalAddress_Positive", test_hdmicec_hal_l1_getLogicalAddress_sourceDevice_positive);
        UT_add_test( pSuite, "getLogicalAddress_negative", test_hdmicec_hal_l1_getLogicalAddress_sourceDevice_negative);
        UT_add_test( pSuite, "Tx_Positive", test_hdmicec_hal_l1_hdmiCecTx_sourceDevice_positive);
        UT_add_test( pSuite, "Tx_negative", test_hdmicec_hal_l1_hdmiCecTx_sourceDevice_negative);
    }

    UT_add_test( pSuite, "open_Positive", test_hdmicec_hal_l1_open_positive);
    UT_add_test( pSuite, "open_negative", test_hdmicec_hal_l1_open_negative);
    UT_add_test( pSuite, "close_Positive", test_hdmicec_hal_l1_close_positive);
    UT_add_test( pSuite, "close_negative", test_hdmicec_hal_l1_close_negative);
    UT_add_test( pSuite, "getPhysicalAddress_Positive", test_hdmicec_hal_l1_getPhysicalAddress_positive);
    UT_add_test( pSuite, "getPhysicalAddress_negative", test_hdmicec_hal_l1_getPhysicalAddress_negative);
    UT_add_test( pSuite, "setRxCallback_Positive", test_hdmicec_hal_l1_setRxCallback_positive);
    UT_add_test( pSuite, "setRxCallback_negative", test_hdmicec_hal_l1_setRxCallback_negative);
    UT_add_test( pSuite, "addLogicalAddress_Positive", test_hdmicec_hal_l1_addLogicalAddress_positive);
    UT_add_test( pSuite, "addLogicalAddress_negative", test_hdmicec_hal_l1_addLogicalAddress_negative);
    UT_add_test( pSuite, "removeLogicalAddress_Positive", test_hdmicec_hal_l1_removeLogicalAddress_positive);
    UT_add_test( pSuite, "removeLogicalAddress_negative", test_hdmicec_hal_l1_removeLogicalAddress_negative);

    return 0;
}

/** @} */ // End of HDMI CEC HAL Tests L1 File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
