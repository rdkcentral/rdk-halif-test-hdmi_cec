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
 * @addtogroup HPK HPK
 * @{
 * @addtogroup HDMI_CEC HDMI_CEC
 * @{
 *
 */

/**
 * @defgroup HDMI_CEC_TESTS HDMI_CEC_TESTS
 * @{
 * @par
 * Unit Testing Suite for HDMI CEC HAL
 */

/**
* @file hdmicec_l2_tests.c
* @page HDMI_CEC_L2_Tests HDMI CEC Level 2 Tests
*
* ## Module's Role
* This module includes Level 2 functional tests (success and failure scenarios).
* This is to ensure that the API meets the operational requirements of the HDMI CEC across all vendors.
*
* **Pre-Conditions:**  None@n
* **Dependencies:** None@nMI
*
* Ref to API Definition specification documentation : [halSpec.md](../../../docs/halSpec.md)
*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>

#include <ut.h>
#include "hdmi_cec_driver.h"

//Set the MACRO for the stb platforms
//#define __UT_STB__ 1
 
//@todo Need to change to 1 second. It must be 1 sec
#define CEC_RESPONSE_TIMEOUT 10
#define CEC_GET_CEC_VERSION (0x9F)
#define CEC_VERSION (0x9E)
#define CEC_DEVICE_VENDOR_ID (0x87)
#define CEC_GIVE_CEC_DEVICE_VENDOR_ID (0x8C)
#define CEC_GIVE_DEVICE_POWER_STATUS (0x8F)
#define CEC_REPORT_POWER_STATUS (0x90)
#define CEC_IMAGE_VIEW_ON (0x04)
#define CEC_STANDBY (0x36)
#define CEC_GIVE_PHYSICAL_ADDRESS (0x83)
#define CEC_REPORT_PHYSICAL_ADDRESS (0x84)
#define CEC_ACTIVE_SOURCE (0x82)
#define CEC_SET_STREAM_PATH (0x86)
#define CEC_GIVE_OSD_NAME (0x46)
#define CEC_SET_OSD_NAME (0x47)
#define CEC_SET_OSD_NAME_VAL1 (0x54)
#define CEC_SET_OSD_NAME_VAL2 (0x56)


#define CEC_POWER_ON (0x00)
#define CEC_POWER_OFF (0x01)
#define CEC_BROADCAST_ADDR (0xF)
#define CEC_TUNER_ADDR (0x3)
#define CEC_TV_ADDR (0x0)

#define CEC_BACK_TO_BACK_SEND_LIMIT 15

//@todo This section will be replaced with UT_LOG() when the feature is available
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
 * @brief Expected cec message buffer in the L2 scenario
 * 
 */
unsigned char cec_opcodeExpected_g = 0x00;

/**
 * @brief Status variable to check if expected cec message received in the respective L2 scenario
 * 
 */
int cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

/**
 * @brief  Status variable to hold the receiver power status
 * 
 */
unsigned char cec_powerStatusReceived_g = CEC_POWER_OFF;

/**
 * @brief  variable to hold the physical address of the receiver
 * 
 */
unsigned char cec_physicalAddressReceived1_g = 0x00;
unsigned char cec_physicalAddressReceived2_g = 0x00;

/**
 * @brief  Status variable to hold the cec version value
 * 
 */
unsigned char cec_version_g = 0x00;

/**
 * @brief Status variable to check if Hdmi RX callback is triggered in respective L2 scenarios
 * 
 */
bool cec_isCallbackTriggered_g = false;

/**
 * @brief Status variable to check if ping is triggered
 * 
 */
bool cec_isPingTriggered_g = false;

/**
 * @brief Semaphore to notify the CEC callback
 * 
 */
sem_t cec_sem_g;

/**
 * @brief variable to hold the semaphore time out
 * 
 */
struct timespec cec_ts_g;

/**
 * @brief callback to receive the HDMI CEC receive messages
 * Ensure  getting correct reply messages form the respective CEC peer devices, for all the CEC message queries form this device
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf receive message buffer passed
 * @param len receive message buffer length
 */
void DriverReceiveCallback_hal_l2(int handle, void *callbackData, unsigned char *buf, int len)
{
    UT_ASSERT_TRUE(len>0); 
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL((bool)(!callbackData));
    UT_ASSERT_PTR_NULL((bool)(!buf));
    //UT_ASSERT_TRUE( (unsigned long long)(callbackData) == (unsigned long long)0xDEADBEEF);
    //@todo need to identify why callback is not equal
    cec_isPingTriggered_g = true;
    CEC_LOG_DEBUG ("\nCall back data generated is \n");
    for (int index=0; index < len; index++) {
        CEC_LOG_DEBUG ("buf at index : %d is %x", index, buf[index]);
    }
    if (len>1){
        CEC_LOG_DEBUG ("\nBuffer generated: %x length: %d\n",buf[1], len);
        if (buf[1] == cec_opcodeExpected_g){
            cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SUCCESS;
            //If power status is received from the other device store it
            if (CEC_REPORT_POWER_STATUS == buf[1]){
                cec_powerStatusReceived_g = buf[2];
                CEC_LOG_INFO ("\nPower status received is : %x",cec_powerStatusReceived_g);
            } else if (CEC_REPORT_PHYSICAL_ADDRESS == buf[1]){
                cec_physicalAddressReceived1_g = buf[2];
                cec_physicalAddressReceived2_g = buf[3];
                CEC_LOG_DEBUG ("\nPhysical address received is : %x %x", cec_physicalAddressReceived1_g, cec_physicalAddressReceived2_g);
            }else if (CEC_VERSION == buf[1]){
                cec_version_g = buf[2];
                CEC_LOG_DEBUG ("\ncec version received is : %x", cec_version_g);
            } else if (CEC_DEVICE_VENDOR_ID == buf[1]){
                CEC_LOG_DEBUG ("\ncec vendor id received is : %x", buf[2]);
            }
            sem_post(&cec_sem_g);

        }
    }
}

/**
 * @brief callback to get the async send message status
 * Ensure all the async CEC queries from the device is successfully delivered.
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param result async send status.
 */
void DriverTransmitCallback_hal_l2(int handle, void *callbackData, int result)
{
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL((bool)(!callbackData));
    //UT_ASSERT_TRUE( (unsigned long long)callbackData== (unsigned long long)0xDEADBEEF);
    //@todo need to identify why callback is not equal
    CEC_LOG_DEBUG ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SUCCESS;
}

/**
 * @brief finding the receiver device on the network for the test 
 * and getting its logical address
 * 
 * @param handle Hdmi device handle
 * @param logicalAddress logical address of the device
 * @param receiverLogicalAddress logical address of the receiver
 */
void getReceiverLogicalAddress (int handle, int logicalAddress, unsigned char* receiverLogicalAddress) {
    int ret=0;
    unsigned char buf[] = {0x00};
    cec_isPingTriggered_g = false;
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
            clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += 1;
            sem_timedwait(&cec_sem_g, &cec_ts_g);
            CEC_LOG_DEBUG ("\n buf is : 0x%x ret value is  : 0x%x result is : 0x%x \n", buf[0], ret, result);
	        //@todo need to check why following condition is not working.
            if (((HDMI_CEC_IO_SENT_AND_ACKD  == ret)||(HDMI_CEC_IO_SUCCESS==ret))&& (HDMI_CEC_IO_SUCCESS == result) ){
                *receiverLogicalAddress = addr;
                CEC_LOG_DEBUG ("\n Logical address of the receiver is : 0x%x\n", *receiverLogicalAddress); break;
                break;
            }
        }
    }
}


/**
 * @brief This function will request the version from the connected devices and check if the valid opcode is received within the expected time interval
 *  In oder to be deterministic opcode should be fixed
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 001@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_getCecVersion_sink( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 0;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TUNER_ADDR;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GET_CEC_VERSION};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    //if logical address assignment is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    cec_opcodeExpected_g = CEC_VERSION;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    /* Positive result */
    CEC_LOG_INFO ("\nRequests for the cec version");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    //@todo Add a print to show the response time for future analysis. See if cec buffer data is passed to logging module
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the vendor ID from the connected devices and check if the valid opcode is received within the expected time interval 
 * In oder to be deterministic opcode should be fixed
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 002@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_getVendorID_sink( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 0;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TUNER_ADDR;

    int len = 2;
    //Give vendor id
    //Simply asuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GIVE_CEC_DEVICE_VENDOR_ID};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    //if logical address assignment is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    cec_opcodeExpected_g = CEC_DEVICE_VENDOR_ID;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    CEC_LOG_INFO ("\nRequests for the vendor id");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }

    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the power status from the connected devices and check if the valid opcode is received within the expected time interval
 *  In oder to be deterministic opcode should be fixed
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 003@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_getPowerStatus_sink( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 0;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TUNER_ADDR;

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GIVE_DEVICE_POWER_STATUS };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    //if logical address assignment is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    CEC_LOG_INFO ("\nRequests for the power status");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTxAsync(handle, buf1, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }

    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecSetTxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will toggle the current power state of the connected device
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 004@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_TogglePowerState_sink( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 0;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TUNER_ADDR;

    //Assuming sender as 3 and broadcast
    //Set the receiver to CEC_STANDBY state
    unsigned char buf1[] = {0x3F, CEC_STANDBY };
    unsigned char buf4[] = {0x3F, CEC_SET_STREAM_PATH, 0x00, 0x00 };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    //if logical address assignment is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf1: 0x%x\n", buf1[0]);
    /* Positive result */
    //Broadcast set power state to CEC_STANDBY here
    buf1[1] = CEC_STANDBY;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nSend CEC_STANDBY message to receiver");
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);

    cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    //After response delay check if power status set to CEC_STANDBY
    buf1[1] = CEC_GIVE_DEVICE_POWER_STATUS;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nRequest power status form the receiver");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
     UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);
    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }
    //Ensure power status returned is CEC_STANDBY form the receiver
    UT_ASSERT_EQUAL( cec_powerStatusReceived_g, CEC_POWER_OFF);


    //Request receiver physical address here
    cec_opcodeExpected_g = CEC_REPORT_PHYSICAL_ADDRESS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    //Request physical address from the receiver
    //buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress;
    buf1[1] = CEC_GIVE_PHYSICAL_ADDRESS;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nRequest physical address form the receiver");
    CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);
    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }


    //Broadcast set stream path with receiver physical address
    buf4[0] = ((logicalAddress&0xFF)<<4)|0x0F; 
    buf4[1] = CEC_SET_STREAM_PATH;
    buf4[2] = cec_physicalAddressReceived1_g;
    buf4[3] = cec_physicalAddressReceived2_g;
    CEC_LOG_DEBUG ("\n HDMI CEC buf4: 0x%x 0x%x 0x%x 0x%x\n", buf4[0], buf4[1], buf4[2], buf4[3]);
    CEC_LOG_INFO ("\nBroadcast new set stream path with receiver physical address");
    result = HdmiCecTx(handle, buf4, sizeof(buf4), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    CEC_LOG_DEBUG ("\n Please ensure connected device power status changed.\n");

    //Check the current power status now. Sender and receiver is same here. Just change the valid opcode
    cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    buf1[1] = CEC_GIVE_DEVICE_POWER_STATUS;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nRequest power status form the receiver");
    CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for the response delay
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);
    //Check expected power state is received
    UT_ASSERT_EQUAL( cec_powerStatusReceived_g, CEC_POWER_ON);
    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
        CEC_LOG_INFO ("\nNot able to  change the power status.");
    } else {
        CEC_LOG_INFO ("\nPower status changed");
    }

    //Using NULL callback
    result = HdmiCecSetTxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the vendor ID when HDMI is in disconnected state and will confirm that response is not received within the expected time interval
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 005@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_validateHdmiCecConnection_sink( void )
{
    //@todo need to check why this function crashes when reciever device is connected
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 0;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_BROADCAST_ADDR;
    unsigned char buf1[] = {0x03, CEC_STANDBY };

    CEC_LOG_INFO ("\nPlease disconnect All the HDMI ports. Please enter any key to continue"); getchar ();

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV
    //@todo need to find out why add logical address is required for sink devices. Sreeni will come back
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    //if logical address assignment is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    //This point receiver address should be broadcast address. Since no other receiver is connected
    UT_ASSERT_TRUE(CEC_BROADCAST_ADDR==receiverLogicalAddress);
    if(CEC_BROADCAST_ADDR!=receiverLogicalAddress){
        CEC_LOG_INFO ("\nGot the receiver address: 0x%x and receiver is connected.\n", __FUNCTION__, __LINE__, receiverLogicalAddress);
    } else {
        CEC_LOG_INFO ("\nReceiver is not connected");
    }

    buf1[0] = ((logicalAddress&0xFF)<<4)|0x3; CEC_LOG_DEBUG ("\n HDMI CEC buf1: 0x%x\n", buf1[0]);
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will ensure back to back CEC message send is working as expected for sink devices
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 006@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_back_to_back_send_sink( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 0;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TUNER_ADDR;
    int ret=0;

    CEC_LOG_INFO ("\nPlease connect more than one cec device to the network \
         and run back to back send parallel . Please enter any key to continue"); getchar ();

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GIVE_DEVICE_POWER_STATUS };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    //if logical address assignment is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    for (int index=0; index < CEC_BACK_TO_BACK_SEND_LIMIT; index++) {
        /* Positive result */
        //Use HdmiCecTx. Even if introduce small delay to ensure send failure is
        //not happening
        CEC_LOG_INFO ("\nRequests for the power status");
        buf1[1] = CEC_GIVE_DEVICE_POWER_STATUS;
        result = HdmiCecTx(handle, buf1, len, &ret);
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
        UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);
	if (ret != HDMI_CEC_IO_SENT_AND_ACKD) {
		CEC_LOG_INFO ("\nError code ret during back to back send is: %d", ret);
	}
    
        CEC_LOG_INFO ("\nRequests vendor id");
        buf1[1] = CEC_GIVE_CEC_DEVICE_VENDOR_ID;
        result = HdmiCecTx(handle, buf1, len, &ret);
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
        UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);
	if (ret != HDMI_CEC_IO_SENT_AND_ACKD) {
		CEC_LOG_INFO ("\nError code ret during back to back send is: %d", ret);
	}

    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecSetTxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}


/**
 * @brief This function will request the version from the connected devices and check if the valid opcode is received within the expected time interval
 *  In oder to be deterministic opcode should be fixed
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 007@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_getCecVersion_source( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TV_ADDR;

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GET_CEC_VERSION};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address for STB
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   cec_opcodeExpected_g = CEC_VERSION;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    /* Positive result */
    CEC_LOG_INFO ("\nRequests for the cec version");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the vendor ID from the connected devices and check if the valid opcode is received within the expected time interval
 *  In oder to be deterministic opcode should be fixed
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 008@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_getVendorID_source( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TV_ADDR;

    int len = 2;
    //Give vendor id
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GIVE_CEC_DEVICE_VENDOR_ID};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    cec_opcodeExpected_g = CEC_DEVICE_VENDOR_ID;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    CEC_LOG_INFO ("\nRequests for the vendor id");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the power status from the connected devices and check if the valid opcode is received within the expected time interval
 *  In oder to be deterministic opcode should be fixed
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 009@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_getPowerStatus_source( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TV_ADDR;

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GIVE_DEVICE_POWER_STATUS };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address for STB
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    CEC_LOG_INFO ("\nRequests for the power status");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTxAsync(handle, buf1, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    result = HdmiCecSetTxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will toggle the current power state of the connected device
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 0010@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_TogglePowerState_source( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type
    unsigned int physicalAddress = 0x00;
    unsigned char receiverLogicalAddress = CEC_TV_ADDR;


    //@todo need to send one broadcast event here. Check image view on can be broadcasted
    //Assuming sender as 3 and broadcast
    //Set the receiver to CEC_STANDBY state
    unsigned char buf1[] = {0x3F, CEC_STANDBY };
    unsigned char buf4[] = {0x3F, CEC_ACTIVE_SOURCE, 0x00, 0x00 };
    CEC_LOG_DEBUG ("\nCEC_STANDBY opcode is not working for TV time being. This currently under investigation");
    CEC_LOG_INFO ("\nPlease set the connected display to CEC_STANDBY. Please enter any key to continue."); getchar ();

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    //@todo need to check why CEC_STANDBY signal is not working in tv panel
    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf0: 0x%x\n", buf1[0]);
    /* Positive result */
    //Broadcast set power state to CEC_STANDBY here
    buf1[1] = CEC_STANDBY;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nSend CEC_STANDBY message to receiver");
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);

    cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    //After response delay check if power status set to CEC_STANDBY
    buf1[1] = CEC_GIVE_DEVICE_POWER_STATUS;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nRequest power status form the receiver");
    clock_t start_time, end_time; CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);
    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }
    //Ensure power status returned is CEC_STANDBY form the receiver
    UT_ASSERT_EQUAL( cec_powerStatusReceived_g, CEC_POWER_OFF);


    //broadcast image view on here
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F;
    buf1[1] = CEC_IMAGE_VIEW_ON;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nSend image view on to the receiver");
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);
    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }


    //set OSD name here
    buf4[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; 
    buf4[1] = CEC_SET_OSD_NAME;
    buf4[2] = CEC_SET_OSD_NAME_VAL1;
    buf4[3] = CEC_SET_OSD_NAME_VAL2;
    CEC_LOG_INFO ("\nSend OSD name to the receiver");
    CEC_LOG_DEBUG ("\n HDMI CEC buf4: 0x%x 0x%x 0x%x 0x%x\n", buf4[0], buf4[1], buf4[2], buf4[3]);
    result = HdmiCecTx(handle, buf4, sizeof(buf4), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    CEC_LOG_DEBUG ("\n Please ensure connected device power status changed.\n");


    //Get device physical address here
    HdmiCecGetPhysicalAddress(handle, &physicalAddress);

    buf4[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; 
    buf4[1] = CEC_ACTIVE_SOURCE;
    buf4[2] = (physicalAddress >> 8) & 0xFF;;
    buf4[3] = physicalAddress & 0xFF;
    CEC_LOG_DEBUG ("\n HDMI CEC buf4: 0x%x 0x%x 0x%x 0x%x\n", buf4[0], buf4[1], buf4[2], buf4[3]);
    CEC_LOG_INFO ("\nRequest to set current device as the active device");
    result = HdmiCecTx(handle, buf4, sizeof(buf4), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for response delay for the reply
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    CEC_LOG_DEBUG ("\n Please ensure connected device power status changed.\n");


    //Check the current power status now. Sender and receiver is same here. Just change the valid opcode
    cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    buf1[1] = CEC_GIVE_DEVICE_POWER_STATUS;
    CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x 0x%x\n", buf1[0], buf1[1]);
    CEC_LOG_INFO ("\nRequest power status form the receiver");
    CEC_MEASURE_RESPONSE_TIME_START_CLOCK(start_time);
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    //Wait for the response delay
    clock_gettime(CLOCK_REALTIME, &cec_ts_g); cec_ts_g.tv_sec += CEC_RESPONSE_TIMEOUT;
    sem_timedwait(&cec_sem_g, &cec_ts_g);
    if (cec_isExpectedBufferReceived_g == HDMI_CEC_IO_SUCCESS) {
        double response_time = 0; CEC_MEASURE_RESPONSE_TIME_GET_RESPONSE_TIME(start_time, end_time, response_time);
        CEC_LOG_DEBUG ("Request response time: %.2f milliseconds\n", response_time);
    }
    //Check if expected buffer received
    UT_ASSERT_EQUAL( cec_isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);
    //Check expected power state is received
    UT_ASSERT_EQUAL( cec_powerStatusReceived_g, CEC_POWER_ON);
    if(HDMI_CEC_IO_SUCCESS != cec_isExpectedBufferReceived_g){
        CEC_LOG_INFO ("\nNot able to  change the power status.");
        CEC_LOG_DEBUG ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    } else {
        CEC_LOG_INFO ("\nPower status changed");
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecSetTxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

//@todo Need to have a scenario to evaluate the multiple CEC Commands getting sent on the network at the same time.  A test case that has two sink devices sending back to back commands for 10 to 15 times in a loop
//@todo implement a separate case to send 10 -15 back to back commands


/**
 * @brief This function will request the vendor ID when HDMI is in disconnected state and will confirm that response is not received within the expected time interval
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 011@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_validateHdmiCecConnection_source( void )
{
    //@todo need to check why this function crashes when receiver device is connected
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_BROADCAST_ADDR;
    unsigned char buf1[] = {0x3F, CEC_STANDBY };


    CEC_LOG_INFO ("\nPlease disconnect All the HDMI ports. Please enter any key to continue"); getchar ();

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    //This point receiver address should be broadcast address. Since no other receiver is connected
    UT_ASSERT_TRUE(CEC_BROADCAST_ADDR==receiverLogicalAddress);
    if(CEC_BROADCAST_ADDR!=receiverLogicalAddress){
        CEC_LOG_INFO ("\nGot the receiver address: 0x%x\n", __FUNCTION__, __LINE__, receiverLogicalAddress);
    } else {
        CEC_LOG_INFO ("\nReceiver is not connected");
    }

    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0; CEC_LOG_DEBUG ("\n HDMI CEC buf1: 0x%x\n", buf1[0]);
    result = HdmiCecTx(handle, buf1, sizeof(buf1), &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will ensure back to back CEC message send is working as expected for sink devices
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 012@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_back_to_back_send_source ( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 0;//Trying some dev type
    unsigned char receiverLogicalAddress = CEC_TUNER_ADDR;

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, CEC_GIVE_DEVICE_POWER_STATUS };

    CEC_LOG_INFO ("\nPlease connect more than one cec device to the network and run \
      back to back send parallel . Please enter any key to continue"); getchar ();

    /* Positive result */
    result = HdmiCecOpen (&handle);
    //if init is failed no need to proceed further
    UT_ASSERT_EQUAL_FATAL ( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get the receiver logical address
    getReceiverLogicalAddress (handle, logicalAddress, &receiverLogicalAddress);

    buf1[0] = ((logicalAddress&0xFF)<<4)|receiverLogicalAddress; CEC_LOG_DEBUG ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    cec_opcodeExpected_g = CEC_REPORT_POWER_STATUS;
    cec_isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    for (int index=0; index < CEC_BACK_TO_BACK_SEND_LIMIT; index++) {
        /* Positive result */
        //Use HdmiCecTx. Even if introduce small delay to ensure send failure is
        //not happening
        CEC_LOG_INFO ("\nRequests for the power status");
        buf1[1] = CEC_GIVE_DEVICE_POWER_STATUS;
        result = HdmiCecTx(handle, buf1, len, &ret);
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
        UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);
    
        CEC_LOG_INFO ("\nRequests vendor id");
        buf1[1] = CEC_GIVE_CEC_DEVICE_VENDOR_ID;
        result = HdmiCecTx(handle, buf1, len, &ret);
        UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
        UT_ASSERT_EQUAL( ret, HDMI_CEC_IO_SENT_AND_ACKD);

    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecSetTxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

static UT_test_suite_t *pSuiteHdmiConnected = NULL;
static UT_test_suite_t *pSuiteHdmiDisConnected = NULL;
static UT_test_suite_t *pSuiteHdmiBackToBackSend = NULL;

/**
 * @brief Register the main tests for this module
 * 
 * @return int - 0 on success, otherwise failure
 */
int test_hdmicec_hal_l2_register( void )
{
    /* add a suite to the registry */
    pSuiteHdmiConnected = UT_add_suite("[L2 test hdmi connected]", NULL, NULL);
    pSuiteHdmiDisConnected = UT_add_suite("[L2 test hdmi disconnected]", NULL, NULL);
    pSuiteHdmiBackToBackSend = UT_add_suite("[L2 test hdmi back to back send]", NULL, NULL);
    //@todo need have two separate suits with one hdmi connected state and another suite for disconnected states
    if (NULL == pSuiteHdmiConnected || NULL == pSuiteHdmiDisConnected || NULL == pSuiteHdmiBackToBackSend) 
    {
        return -1;
    }

#ifndef __UT_STB__
    UT_add_test( pSuiteHdmiConnected, "getCecVersionSink", test_hdmicec_hal_l2_getCecVersion_sink);
    UT_add_test( pSuiteHdmiConnected, "getVendorIDSink", test_hdmicec_hal_l2_getVendorID_sink);
    UT_add_test( pSuiteHdmiConnected, "getPowerStatusSink", test_hdmicec_hal_l2_getPowerStatus_sink);
    UT_add_test( pSuiteHdmiConnected, "getPowerStatusAndToggleSink", test_hdmicec_hal_l2_TogglePowerState_sink);
    UT_add_test( pSuiteHdmiDisConnected, "sendMsgHdmiVendorIdDisconnectedSink", test_hdmicec_hal_l2_validateHdmiCecConnection_sink);
    UT_add_test( pSuiteHdmiBackToBackSend, "back_to_back_sendSink", test_hdmicec_hal_l2_back_to_back_send_sink);
#else
    UT_add_test( pSuiteHdmiConnected, "getCecVersionSource", test_hdmicec_hal_l2_getCecVersion_source);
    UT_add_test( pSuiteHdmiConnected, "getVendorIDSource", test_hdmicec_hal_l2_getVendorID_source);
    UT_add_test( pSuiteHdmiConnected, "getPowerStatusSource", test_hdmicec_hal_l2_getPowerStatus_source);
    UT_add_test( pSuiteHdmiConnected, "getPowerStatusAndToggleSource", test_hdmicec_hal_l2_TogglePowerState_source);
    UT_add_test( pSuiteHdmiDisConnected, "sendMsgVendorIdHdmiDisconnectedSource", test_hdmicec_hal_l2_validateHdmiCecConnection_source);
    UT_add_test( pSuiteHdmiBackToBackSend, "back_to_back_sendSource", test_hdmicec_hal_l2_back_to_back_send_source);
#endif

    return 0;
}

/** @} */ // End of HDMI_CEC_TESTS HDMI_CEC_TESTS
/** @} */ // End of HDMI_CEC HDMI_CEC
/** @} */ // End of HPK
