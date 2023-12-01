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
#include "ut_log.h"
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

static int gTestGroup = 1;
static int gTestID = 1;

#define CEC_GET_CEC_VERSION (0x9F)
#define CEC_DEVICE_VENDOR_ID (0x87)
#define CEC_BROADCAST_ADDR (0xF)

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
    UT_ASSERT_TRUE(len>0); 
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL((bool)(!callbackData));
    UT_ASSERT_PTR_NULL((bool)(!buf));
    //UT_ASSERT_TRUE( (unsigned long long)callbackData== (unsigned long long)0xDEADBEEF);
    //@todo need to identify why callback is not equal
    cec_isPingTriggeredl1_g = true;
    UT_LOG ("\nCall back data generated is \n");
    for (int index=0; index < len; index++) {
        UT_LOG ("buf at index : %d is %x", index, buf[index]);
    }
}

/**
 * @brief callback to get the async send message status
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param result async send status
 */
void DriverTransmitCallback(int handle, void *callbackData, int result)
{
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL((bool)(!callbackData));
    //UT_ASSERT_TRUE( (unsigned long long)callbackData== (unsigned long long)0xDEADBEEF);
    //@todo need to identify why callback is not equal
    UT_LOG ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("Check failed"); }
}

/**
 * @brief get the logical address of the receiver
 * @param handle Hdmi device handle
 * @param logicalAddress logical address of the device
 * @param receiverLogicalAddress logical address of the receiver
 */
void getReceiverLogicalAddressL1 (int handle, int logicalAddress, unsigned char* receiverLogicalAddress) {
    int ret=0;
    unsigned char buf[] = {0x00};
    cec_isPingTriggeredl1_g = false;
    //Ping all logical address and determine which device is connected
    for(int i=0; i< CEC_BROADCAST_ADDR; i++ ) {
        unsigned char addr = i & 0x0F; 
        if (logicalAddress != addr) {
            buf[0] = ((logicalAddress&0x0F)<<4)|addr;
	        //No need to check the return status of HdmiCecTx since function will called
	        //to check the hdmi disconnected conditions also
	        ret = HDMI_CEC_IO_SUCCESS;
            int result = HdmiCecTx(handle, buf, sizeof (buf), &ret);

            //Wait for response delay for the reply
            clock_gettime(CLOCK_REALTIME, &cec_tsl1_g); cec_tsl1_g.tv_sec += 1;
            sem_timedwait(&cec_seml1_g, &cec_tsl1_g);
            UT_LOG ("\n buf is : 0x%x ret value is  : 0x%x result is : 0x%x \n", buf[0], ret, result);
	        //@todo need to check why following condition is not working.
            if (((HDMI_CEC_IO_SENT_AND_ACKD  == ret)||(HDMI_CEC_IO_SUCCESS==ret))&& (HDMI_CEC_IO_SUCCESS == result) ){
                *receiverLogicalAddress = addr;
                UT_LOG ("\n Logical address of the receiver is : 0x%x\n", *receiverLogicalAddress); break;
                break;
            } else {
                UT_LOG ("\n failed to receive logical address  ret:0x%x result:0x%x\n", ret, result);
            }
        }
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
    int result;
    int handle = 0;
    gTestID = 1;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    //Check Null even before calling the positive case
    result = HdmiCecOpen( NULL );
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("open failed"); }

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("open failed"); }
    
    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_ALREADY_OPEN != result) { UT_FAIL ("open failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }
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
    int result;
    int handle = 0;
    gTestID = 2;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    
    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("open failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Ensure HdmiCecOpen() returns HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE when
 * none of the device specific logical address is not available.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 003@n
 * 
 * **Pre-Conditions:**@n
 * Connect other four cec enabled playback source devices to the network
 * and ensure none of the playback device logical address is available 
 * to allocate for the current device.
 * 
 * **Dependencies:** None@n
 * **User Interaction:** None
 * 
 * **Test Procedure:**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call HdmiCecOpen() - open interface when not all the logical address are in use | handle | HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE| Should Pass |
 * |02|Call HdmiCecClose () - call with invalid handle | handle=0 | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * 
 */
void test_hdmicec_hal_l1_open_logical_address_unavailable_source ( void )
{
    int result;
    int handle = 0;
    gTestID = 3;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_LOG ("\nPlease connect other 4 cec enabled playback devices to the cec network. \
                  Please enter any key to continue"); getchar ();
    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE != result) { UT_FAIL ("Check failed"); }
    
    //The above open is failed handle should be null
    UT_ASSERT_TRUE(handle==0);

     //Here handle = 0 since open failed and close should fail.
    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("close failed"); }
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
 * |02|Call HdmiCecClose () - call with invalid handle | handle=0 | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |03|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecClose () - close interface again | handle=hdmiHandle | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_close_negative( void )
{
    int result;
    int handle = 0;
    gTestID = 4;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("open failed"); }

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("open failed"); }

    result = HdmiCecClose( 0 );
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("close failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }  

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("close failed"); }
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
    int result;
    int handle = 0;
    gTestID = 5;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("open failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }  

    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("open failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); } 

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
    int result = 0;
    int handle = 0;
    unsigned int physicalAddress = 0;
    gTestID = 6;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    if (HDMI_CEC_IO_SUCCESS  != result) { UT_FAIL ("HdmiCecGetPhysicalAddress failed"); }


    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetPhysicalAddress(0, &physicalAddress);
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("HdmiCecGetPhysicalAddress failed"); }

    result = HdmiCecGetPhysicalAddress(handle, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecGetPhysicalAddress failed"); }


    result = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    if (HDMI_CEC_IO_SUCCESS  != result) { UT_FAIL ("HdmiCecGetPhysicalAddress failed"); }
    unsigned int maxVal = (((0x04 &0xF0 ) << 20)|( (0x04 &0x0F ) << 16) |((0x04 & 0xF0) << 4)  | (0x04 & 0x0F));
    //Max possible physical address is 4.4.4.4
    if (physicalAddress>maxVal) {
        UT_FAIL ("physicalAddress miss match failed");
    }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }

    //Calling API after close,
    result = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    if (HDMI_CEC_IO_SUCCESS  != result) { UT_FAIL ("HdmiCecGetPhysicalAddress failed"); }
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
    int result = 0;
    int handle = 0;
    unsigned int physicalAddress = 0;
    gTestID = 7;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    if (HDMI_CEC_IO_SUCCESS  != result) { UT_FAIL ("HdmiCecGetPhysicalAddress failed"); }
    UT_ASSERT_TRUE(physicalAddress==0xffff);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }

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
 * |05|Call HdmiCecAddLogicalAddress() - call with invalid logical address. Only supported value is 0 | handle, logicalAddress=0x03 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=0x0F | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|Call HdmiCecAddLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * 
 * HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE case is updated in separate L1 case
 * @todo SKY will come back how to simulate HDMI_CEC_IO_SENT_FAILED
 */
void test_hdmicec_hal_l1_addLogicalAddress_sinkDevice_negative( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;
    gTestID = 8;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );


    result = HdmiCecAddLogicalAddress( 0, logicalAddress );
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecAddLogicalAddress( handle, -1 );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecAddLogicalAddress( handle, 0x3 );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecAddLogicalAddress( handle, 0xF );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }
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
 * |03|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * 
 * HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE case is updated in separate L1 case
 * @todo SKY will come back how to simulate HDMI_CEC_IO_SENT_FAILED
 */
void test_hdmicec_hal_l1_addLogicalAddress_sinkDevice_positive( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;
    gTestID = 9;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Ensure HdmiCecAddLogicalAddress() returns correct error codes, during all of this API's invocation scenarios
 * 
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 * 
 * HdmiCecAddLogicalAddress not supported for source devices. Hence this API doesn't have any positive scenarios for source devices.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 010@n
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
 * |03|Call HdmiCecAddLogicalAddress() - call with valid arguments. API is not valid for source devices. | handle, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should pass. |
 * |04|Call HdmiCecAddLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |05|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=-1 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecAddLogicalAddress() - call with invalid logical address. Only supported value is 0 | handle, logicalAddress=0x03 | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecAddLogicalAddress() - call with invalid logical address | handle, logicalAddress=0x0F | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecAddLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_addLogicalAddress_sourceDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;
    gTestID = 10;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( 0, logicalAddress );
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecAddLogicalAddress( handle, -1 );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecAddLogicalAddress( handle, 0x3 );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecAddLogicalAddress( handle, 0xF );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("Close failed"); }

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}


/**
  * @brief Ensure HdmiCecAddLogicalAddress() returns HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE when tying to add an existing logic address
  * 
  *  This test case is only applicable for sink devices
  * 
  * **Test Group ID:** Basic: 01@n
  * **Test Case ID:** 011@n
  * 
  * **Pre-Conditions:**@n
  * Device should be connected to another CEC enabled TV
  * 
  * **Dependencies:**None@n
  * **User Interaction:** None
  * 
  * **Test Procedure:**@n
  * |Variation / Step|Description|Test Data|Expected Result|Notes|
  * |:--:|---------|----------|--------------|-----|
  * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
  * |02|Call HdmiCecAddLogicalAddress() - call with valid arguments. Logical address 0 is already allocated | handle, logicalAddress = 0 | HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE | Should pass. |
  * |03|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
  *
  *@todo Need to check why this test is not working. Need to check with SKY if this is a valid case. Need to verify with another vendor TV also SKY will come back.
  */
void test_hdmicec_hal_l1_addLogicalAddressWithAddressInUse_sinkDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 11;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_LOG ("\nPlease connect another CEC enabled sink device to the device. Please enter any key to continue"); getchar ();
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );
    
    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }
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
 * |04|Call HdmiCecRemoveLogicalAddress() - call with invalid logical address | handle, logicalAddress=0xF | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |05|Call HdmiCecRemoveLogicalAddress() - Try to remove with out adding the logical address| handle, logicalAddress | HDMI_CEC_IO_ALREADY_REMOVED| Should Pass |
 * |06|Call HdmiCecAddLogicalAddress() - call with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecRemoveLogicalAddress() - remove allocated logical address. API only applicable for sink devices. | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass|
 * |08|Call HdmiCecRemoveLogicalAddress() - remove same logical address again. API only applicable for sink devices. | handle, logicalAddress | HDMI_CEC_IO_ALREADY_REMOVED| Should Pass |
 * |09|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecRemoveLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_removeLogicalAddress_sinkDevice_negative( void )
{
    int result;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    gTestID = 12;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecOpen(&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecRemoveLogicalAddress( 0, logicalAddress );
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( handle, 0xF );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( handle,  -1 );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_ALREADY_REMOVED != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_ALREADY_REMOVED != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecClose(handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}

/**
 * @brief Validate positive scenarios for HdmiCecRemoveLogicalAddress()
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
void test_hdmicec_hal_l1_removeLogicalAddress_sinkDevice_positive( void )
{
    int result;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    gTestID = 13;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen(&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecClose(handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }

}


/**
 * @brief Ensure HdmiCecRemoveLogicalAddress() returns correct error codes, during all of this API's invocation scenarios
 *  This test case is only applicable for source devices
 * 
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 014@n
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
 * |01|Call HdmiCecRemoveLogicalAddress() - trying to remove logical address even before opening the module | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecRemoveLogicalAddress() - remove API is not applicable for source devices.| handle, logicalAddress | HDMI_CEC_IO_INVALID_ARGUMENT| Should pass|
 * |04|Call HdmiCecRemoveLogicalAddress() - call with invalid handle | handle=0, logicalAddress | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |05|Call HdmiCecRemoveLogicalAddress() - call with invalid logical address | handle, logicalAddress=0xF | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |06|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecRemoveLogicalAddress() - call after module is closed | handle, logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_removeLogicalAddress_sourceDevice( void )
{
    int result;
    int handle = 0;
    int logicalAddress = DEFAULT_LOGICAL_ADDRESS;
    gTestID = 14;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecOpen(&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecRemoveLogicalAddress( 0, logicalAddress );
    if (HDMI_CEC_IO_INVALID_HANDLE  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    logicalAddress = 0xF;
    result = HdmiCecRemoveLogicalAddress( handle, 0xF );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    logicalAddress = -1;
    result = HdmiCecRemoveLogicalAddress( handle, -1 );
    if (HDMI_CEC_IO_INVALID_ARGUMENT  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecClose(handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("close failed"); }

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

}


/**
 * @brief Validate negative scenarios for HdmiCecGetLogicalAddress()
 * 
 *  This test case is only applicable for sink devices
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
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int logicalAddressCrossCheck = INT_MIN;
    gTestID = 15;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(0, &logicalAddress);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddressCrossCheck);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_ASSERT_TRUE( (int)0xF!= logicalAddressCrossCheck);

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddressCrossCheck);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_ASSERT_TRUE( logicalAddress!= logicalAddressCrossCheck);

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddressCrossCheck);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_ASSERT_TRUE( (int)0xF!= logicalAddressCrossCheck);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for HdmiCecGetLogicalAddress()
 * 
 *  This test case is only applicable for sink devices
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
 * |02|Call HdmiCecGetLogicalAddress() - call API with valid arguments; should return the logical address added by the caller | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sinkDevice_positive ( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int logicalAddressCrossCheck = INT_MIN;
    gTestID = 16;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecAddLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddressCrossCheck);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_ASSERT_TRUE( logicalAddress!= logicalAddressCrossCheck);

    result = HdmiCecRemoveLogicalAddress( handle, logicalAddress );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecRemoveLogicalAddress failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate negative scenarios for HdmiCecGetLogicalAddress()
 * 
 *  This test case is only applicable for source devices
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
 * |05|Call HdmiCecGetLogicalAddress() - call API with valid arguments in source devices should return a valid logical address between 0x00 and 0x0F, excluding both the values. | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call HdmiCecGetLogicalAddress()  - call the API after module is closed | handle, &logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_getLogicalAddress_sourceDevice_negative( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 17;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecGetLogicalAddress(handle,  &logicalAddress);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(0,  &logicalAddress);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_ASSERT_TRUE(logicalAddress>0x0E && logicalAddress<0x01);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for HdmiCecGetLogicalAddress()
 *  This test case is only applicable for source devices
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
    int result;
    int handle = 0;
    int logicalAddress = 0;

    gTestID = 18;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }
    UT_ASSERT_TRUE(logicalAddress>0x0E && logicalAddress<0x01);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
}

/**
 * @brief Validate negative scenarios for HdmiCecSetRxCallback()
 * 
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 019@n
 * 
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
 * |01|Call HdmiCecSetRxCallback() - call the API before open | handle, DriverReceiveCallback, data | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetRxCallback() - call the API with invalid arguments | handle=0, DriverReceiveCallback, data  | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass|
 * |05|Call HdmiCecSetRxCallback() - call the API after close | handle, DriverReceiveCallback, data | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * 
 */
void test_hdmicec_hal_l1_setRxCallback_negative ( void )
{
    int result;
    int handle = 0;
    gTestID = 20;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    //Calling API before open, should pass
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); } 

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetRxCallback(0, DriverReceiveCallback, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); } 

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    //Calling API after close, should return success
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); } 

    HdmiCecSetRxCallback(0, DriverReceiveCallback, 0);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); } 
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Validate positive scenarios for  HdmiCecSetRxCallback()
 * 
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 020@n
 * 
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
 * |02|Call HdmiCecAddLogicalAddress() - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecSetRxCallback() - set RX Call back with valid parameters | handle, DriverReceiveCallback, data=0xDEADBEEF | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecSetRxCallback() - unset the RX Call back with NULL params| handle, DriverReceiveCallback=NULL, data=0 | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecClose () - close interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * 
 */
void test_hdmicec_hal_l1_setRxCallback_positive( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 20;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    //Set logical address for TV
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) {
        /*Cleanup before exiting */
        result = HdmiCecClose (handle);
        if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
        UT_FAIL ("HdmiCecAddLogicalAddress failed");
    }

    //Get logical address for STB
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    //Using NULL callback
    result = HdmiCecSetRxCallback(0, DriverReceiveCallback, 0);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); }

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); }


    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("ChHdmiCecSetRxCallbackeck failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Ensure HdmiCecSetTxCallback() returns correct error codes, during the negative API invocation scenarios
 * 
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 021@n
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
 * |01|Call HdmiCecSetTxCallback() - trying set TX callback even before opening the module | handle, devType, &logicalAddress | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 * |02|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetTxCallback() - set TX Call back with invalid handle | handle=0, DriverTransmitCallback, data address  | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call HdmiCecSetTxCallback()  - call the API, after module is closed | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_NOT_OPENED| Should Pass | 
 */
void test_hdmicec_hal_l1_setTxCallback_negative( void )
{
    int result;
    int handle = 0;
    gTestID = 21;


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    result = HdmiCecOpen ( &handle );
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback(0, NULL, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }
    
    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_NOT_OPENED  != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief Ensure HdmiCecSetTxCallback() returns success, during the positive API invocation scenarios
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 022@n
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
 * |01|Call HdmiCecOpen() - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call HdmiCecSetTxCallback() - set TX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call HdmiCecSetTxCallback()  - set RX Call back with NULL callback | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * 
 */
void test_hdmicec_hal_l1_setTxCallback_positive( void )
{
    int result;
    int handle = 0;
    gTestID = 22;


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    result = HdmiCecOpen ( &handle );
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    result = HdmiCecSetTxCallback( handle, DriverTransmitCallback, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    result = HdmiCecSetTxCallback( handle, NULL, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}


/**
 * @brief Ensure HdmiCecTx() returns correct error codes, during all of this API's invocation scenarios
 *  This test case is only applicable for sink devices
 * 
 *  HDMI_CEC_IO_GENERAL_ERROR : is platform specific and cannot be simulated
 *  HDMI_CEC_IO_SENT_BUT_NOT_ACKD : is verified part of L2. since all the device disconnected tests handled in L2
 *  HDMI_CEC_IO_SENT_FAILED : Underlying bus error. cannot be simulated
 * @todo  SKY will come back on how to simulate HDMI_CEC_IO_SENT_FAILED.
 * 
 * **Test Group ID:** Basic: 01@n
 * **Test Case ID:** 023@n
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
 * |03|Call HdmiCecTx() - trying to send cec message, for sink device even before setting logical address | handle, buf, len, &ret | HDMI_CEC_IO_SENT_FAILED| Specific to sink devices. Should Pass |
 * |04|Call HdmiCecAddLogicalAddress() - call add logical address for sink devices with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Specific to sink devices. Should Pass |
 * |05|Call HdmiCecGetLogicalAddress() - call get logical address with valid arguments. API should return the logical address added in the above step | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call HdmiCecTx() - invoke send cec message with invalid ret | handle, buf, len, &ret=NULL  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |07|Call HdmiCecTx() - invoke send cec message with invalid buffer | handle, buf=NULL, len, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |08|Call HdmiCecTx() - invoke send cec message with invalid handle | handle=0, buf, len, &ret  | HDMI_CEC_IO_INVALID_HANDLE| Should Pass |
 * |09|Call HdmiCecTx() - invoke send cec message with invalid buffer length | handle, buf, len=INT_MIN, &ret  | HDMI_CEC_IO_INVALID_ARGUMENT| Should Pass |
 * |10|Call HdmiCecTx() - send the cec message with valid arguments after module initialization | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS and ret=HDMI_CEC_IO_SENT_AND_ACKD| Should Pass |
 * |11|Call HdmiCecTx() - back to back send to ensure it is passing | handle, buf, len. &ret | HDMI_CEC_IO_SUCCESS and ret=HDMI_CEC_IO_SENT_AND_ACKD | Should Pass | 
 * |12|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |13|Call HdmiCecTx()  -  invoke send cec message once module is closed | handle, buf, len, &ret | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_negative( void )
{
    int result=HDMI_CEC_IO_SENT_AND_ACKD;
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
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SENT_FAILED != result) { UT_FAIL ("HdmiCecTx failed"); }

    //Set logical address for TV
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) {
        /*Cleanup before exiting */
        result = HdmiCecClose (handle);
        if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecAddLogicalAddress failed"); }
        UT_FAIL ("HdmiCecAddLogicalAddress failed");
    }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    /* Invalid input */
    result = HdmiCecTx(handle, buf, len, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Invalid input */
    result = HdmiCecTx(handle, NULL, len, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Invalid input */
    result = HdmiCecTx(0, buf, len, &ret);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Invalid input */
    result = HdmiCecTx(handle, buf, INT_MIN, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTx failed"); }

    UT_LOG ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_AND_ACKD != ret) { UT_FAIL ("HdmiCecTx failed"); }

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed.*/
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_AND_ACKD != ret) { UT_FAIL ("HdmiCecTx failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTx failed"); }
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
 * |05|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_positive( void )
{
    int result=HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 24;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    //Set logical address for TV
    //logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) {
        /*Cleanup before exiting */
        result = HdmiCecClose (handle);
        if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
        UT_FAIL ("HdmiCecAddLogicalAddress failed");
    }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle,  &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }


    UT_LOG ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_AND_ACKD != ret) { UT_FAIL ("HdmiCecTx failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

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
 * |08|Call HdmiCecTx() - send the cec message with valid arguments after module initialization | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS and ret=HDMI_CEC_IO_SENT_AND_ACKD| Should Pass |
 * |09|Call HdmiCecTx() - back to back send to ensure it is passing | handle, buf, len. &ret | HDMI_CEC_IO_SUCCESS and ret=HDMI_CEC_IO_SENT_AND_ACKD| Should Pass |
 * |10|Call HdmiCecClose() - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|Call HdmiCecTx()  -  invoke send cec message once module is closed | handle, buf, len, &ret | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTx_sourceDevice_negative( void )
{
    int result=HDMI_CEC_IO_SENT_AND_ACKD;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    gTestID = 25;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Sender as 3 and broadcast
    unsigned char buf[] = {0x3F, CEC_GET_CEC_VERSION};

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    //Calling API before open, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    /* Invalid input */
    result = HdmiCecTx(handle, buf, len, NULL);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Invalid input */
    result = HdmiCecTx(handle, NULL, len, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Invalid input */
    result = HdmiCecTx(0, buf, len, &ret);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecTx failed"); }

    /* Invalid input */
    result = HdmiCecTx(handle, buf, INT_MIN, &ret);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTx failed"); }

    UT_LOG ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_AND_ACKD != ret) { UT_FAIL ("Check failed"); }

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed*/
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_AND_ACKD != ret) { UT_FAIL ("HdmiCecTx failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTx failed"); }
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
    int result=HDMI_CEC_IO_SENT_AND_ACKD;
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
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    UT_LOG ("\n hdmicec logicalAddress: 0x%x\n", (logicalAddress&0xFF)<<4);
    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTx(handle, buf, len, &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_AND_ACKD != ret) { UT_FAIL ("HdmiCecTx failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
}

/**
 * @brief This function will try to ping an unavailable logical address and ensure ACK error happens
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 027@n
 *
 *
 * *Pre-Conditions :** @n
 *  All of the device HDMI cable should be disconnected
 *
 **Dependencies :** N/A @n
 *
 * **Test Procedure :**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call `HdmiCecTx()` - Try to ping an unavailable logical address and ensure ACK error | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_portDisconnected_sink( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    unsigned char buf[] = {0x03};
    gTestID = 27;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_LOG ("\nPlease disconnect All the HDMI ports. Please enter any key to continue"); getchar ();

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); }

    //Set logical address for TV.
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) {
        /*Cleanup before exiting */
        result = HdmiCecClose (handle);
        if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
        UT_FAIL ("HdmiCecAddLogicalAddress failed");
    }

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecTx(handle, buf, sizeof(buf), &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_BUT_NOT_ACKD != ret) { UT_FAIL ("HdmiCecTx failed"); }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
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
 * |08|Call HdmiCecTxAsync() - send the cec message after correct module initialization | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecTxAsync() - back to back send to ensure it is passing | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |11|Call HdmiCecTxAsync()  - call API after module is closed | handle, buf, len | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sinkDevice_negative( void )
{
    int result=0;
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
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    //Set logical address for TV
    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SENT_FAILED != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) {
        /*Cleanup before exiting */
        result = HdmiCecClose (handle);
        if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
        UT_FAIL ("HdmiCecAddLogicalAddress failed");
    }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    /* Invalid input */
    result = HdmiCecTxAsync(handle, NULL, len);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    result = HdmiCecTxAsync(0, buf, len);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /* Invalid input */
    result = HdmiCecTxAsync(handle, buf, INT_MIN);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTxAsync failed"); }


    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed*/
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTxAsync failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
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
    int result=0;
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
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    //Set logical address for TV
    logicalAddress = DEFAULT_LOGICAL_ADDRESS;

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) {
        /*Cleanup before exiting */
        result = HdmiCecClose (handle);
        if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
        UT_FAIL ("HdmiCecAddLogicalAddress failed");
    }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }


    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
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
 * |07|Call HdmiCecTxAsync() - send the cec message after correct module initialization | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|Call HdmiCecTxAsync() - back to back send to ensure it is passing | handle, buf, len | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |09|Call HdmiCecClose () - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |10|Call HdmiCecTxAsync()  - call API after module is closed | handle, buf, len | HDMI_CEC_IO_NOT_OPENED| Should Pass |
 */
void test_hdmicec_hal_l1_hdmiCecTxAsync_sourceDevice_negative( void )
{
    int result=0;
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
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    /* Invalid input */
    result = HdmiCecTxAsync(handle, NULL, len);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    result = HdmiCecTxAsync(0, buf, len);
    if (HDMI_CEC_IO_INVALID_HANDLE != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /* Invalid input */
    result = HdmiCecTxAsync(handle, buf, INT_MIN);
    if (HDMI_CEC_IO_INVALID_ARGUMENT != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    buf [1] = CEC_DEVICE_VENDOR_ID;
    /*Back to back send and ensure send is not failed*/
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    //Calling API after close, should give invalid argument
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_NOT_OPENED != result) { UT_FAIL ("HdmiCecTxAsync failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
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
    int result=0;
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
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetTxCallback failed"); }

    buf[0] = 0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    //Get logical address
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    buf[0] = ((logicalAddress&0xFF)<<4)|0x0F; UT_LOG ("\n hdmicec buf: 0x%x\n", buf[0]);

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf, len);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTxAsync failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }

    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

/**
 * @brief This function will try to ping an unavailable logical address and ensure ACK error happens
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 032@n
 *
 *
 * *Pre-Conditions :** @n
 *  All of the device HDMI cable should be disconnected
 *
 **Dependencies :** N/A @n
 *
 * **Test Procedure :**@n
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |01|Call `HdmiCecOpen()` - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |02|Call `HdmiCecSetRxCallback()` - set RX Call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|Call `HdmiCecAddLogicalAddress()` - Call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|Call `HdmiCecGetLogicalAddress()` - Call get logical address with valid arguments | handle, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|Call `HdmiCecTx()` - Try to ping an unavailable logical address and ensure ACK error | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|Call `HdmiCecSetRxCallback()` - unregister RX Call back | handle, cbfunc=NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|Call `HdmiCecClose ()` - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l1_portDisconnected_source( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    unsigned char buf[] = {0x00};
    gTestID = 32;


    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    UT_LOG ("\nPlease disconnect All the HDMI ports. Please enter any key to continue"); getchar ();

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); }

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, &logicalAddress);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecGetLogicalAddress failed"); }

    result = HdmiCecTx(handle, buf, sizeof(buf), &ret);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecTx failed"); }
    if (HDMI_CEC_IO_SENT_BUT_NOT_ACKD != ret) { UT_FAIL ("HdmiCecTx failed"); }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecSetRxCallback failed"); }

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL ("HdmiCecClose failed"); }
    UT_LOG("\n Exit %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
}

static UT_test_suite_t *pSuiteNegative = NULL;
static UT_test_suite_t *pSuiteHdmiDisConnected = NULL;
static UT_test_suite_t *pSuitePositive = NULL;

/**
 * @brief Register the main tests for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int test_hdmicec_hal_l1_register( void )
{
    /* add a suite to the registry */
    pSuiteNegative = UT_add_suite("[L1 hdmicec-generic negative]", NULL, NULL);
    pSuiteHdmiDisConnected = UT_add_suite("[L1 test hdmi disconnected]", NULL, NULL);
    pSuitePositive = UT_add_suite("[L1 hdmicec-generic positive]", NULL, NULL);
    if (NULL == pSuiteNegative)
    {
        return -1;
    }

    //
    UT_add_test( pSuiteNegative, "open", test_hdmicec_hal_l1_open_negative);
    UT_add_test( pSuiteNegative, "close", test_hdmicec_hal_l1_close_negative);
    UT_add_test( pSuiteNegative, "getPhysicalAddress", test_hdmicec_hal_l1_getPhysicalAddress_negative);

    UT_add_test( pSuiteNegative, "setRxCallback", test_hdmicec_hal_l1_setRxCallback_negative);
    UT_add_test( pSuiteNegative, "setTxCallback", test_hdmicec_hal_l1_setTxCallback_negative);

#ifndef __UT_STB__
    UT_add_test( pSuiteNegative, "addLogicalAddressSink", test_hdmicec_hal_l1_addLogicalAddress_sinkDevice_negative);
    UT_add_test( pSuiteNegative, "removeLogicalAddressSink", test_hdmicec_hal_l1_removeLogicalAddress_sinkDevice_negative);
    UT_add_test( pSuiteNegative, "getLogicalAddressSink", test_hdmicec_hal_l1_getLogicalAddress_sinkDevice_negative);
    UT_add_test( pSuiteNegative, "hdmiCecTxSink", test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_negative);
    UT_add_test( pSuiteNegative, "hdmiCecTxAsyncSink", test_hdmicec_hal_l1_hdmiCecTxAsync_sinkDevice_negative);
    UT_add_test( pSuiteNegative, "addLogicalAddressWithAddressInUseSink", test_hdmicec_hal_l1_addLogicalAddressWithAddressInUse_sinkDevice);
    UT_add_test( pSuiteHdmiDisConnected, "portDisconnectedSink", test_hdmicec_hal_l1_portDisconnected_sink);
#else
    UT_add_test( pSuite, "getLogicalAddressSource", test_hdmicec_hal_l1_getLogicalAddress_sourceDevice_negative);
    UT_add_test( pSuite, "hdmiCecTxSource", test_hdmicec_hal_l1_hdmiCecTx_sourceDevice_negative);
    UT_add_test( pSuite, "hdmiCecTxAsyncSource", test_hdmicec_hal_l1_hdmiCecTxAsync_sourceDevice_negative);
    UT_add_test( pSuite, "open_logical_address_unavailable_source", test_hdmicec_hal_l1_open_logical_address_unavailable_source);
    UT_add_test( pSuiteHdmiDisConnected, "portDisconnectedSource", test_hdmicec_hal_l1_portDisconnected_source);
#endif //end of __UT_STB__

    UT_add_test( pSuitePositive, "openPositive", test_hdmicec_hal_l1_open_positive);
    UT_add_test( pSuitePositive, "closePositive", test_hdmicec_hal_l1_close_positive);
    UT_add_test( pSuitePositive, "getPhysicalAddressPositive", test_hdmicec_hal_l1_getPhysicalAddress_positive);

    UT_add_test( pSuitePositive, "setRxCallbackPositive", test_hdmicec_hal_l1_setRxCallback_positive);
    UT_add_test( pSuitePositive, "setTxCallbackPositive", test_hdmicec_hal_l1_setTxCallback_positive);

#ifndef __UT_STB__
    UT_add_test( pSuitePositive, "addLogicalAddressSinkPositive", test_hdmicec_hal_l1_addLogicalAddress_sinkDevice_positive);
    UT_add_test( pSuitePositive, "removeLogicalAddressSinkPositive", test_hdmicec_hal_l1_removeLogicalAddress_sinkDevice_positive);
    UT_add_test( pSuitePositive, "getLogicalAddressSinkPositive", test_hdmicec_hal_l1_getLogicalAddress_sinkDevice_positive);
    UT_add_test( pSuitePositive, "hdmiCecTxSinkPositive", test_hdmicec_hal_l1_hdmiCecTx_sinkDevice_positive);
    UT_add_test( pSuitePositive, "hdmiCecTxAsyncSinkPositive", test_hdmicec_hal_l1_hdmiCecTxAsync_sinkDevice_positive);
#else
    UT_add_test( pSuitePositive, "getLogicalAddressSourcePositive", test_hdmicec_hal_l1_getLogicalAddress_sourceDevice_positive);
    UT_add_test( pSuitePositive, "hdmiCecTxSourcePositive", test_hdmicec_hal_l1_hdmiCecTx_sourceDevice_positive);
    UT_add_test( pSuitePositive, "hdmiCecTxAsyncSourcePositive", test_hdmicec_hal_l1_hdmiCecTxAsync_sourceDevice_positive);
#endif //end of __UT_STB__

    return 0;
}

/** @} */ // End of HDMI CEC HAL Tests L1 File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
