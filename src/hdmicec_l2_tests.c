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
* @file hdmicec_l2_tests.c
* @page HDMI_CEC_L2_Tests HDMI CEC Level 2 Tests
*
* ## Module's Role
* This module includes Level 2 functional tests (success and failure scenarios).
* This is to ensure that the API meets the operational requirements of the HDMI CEC across all vendors.
*
* **Pre-Conditions:**  None@n
* **Dependencies:** None@n
*
* Ref to API Definition specification documentation : [halSpec.md](../../../docs/halSpec.md)
*/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <ut.h>
#include "hdmi_cec_driver.h"

#define HDMICEC_RESPONSE_TIMEOUT 2
#define HDMICEC_USER_INTERACTION_PAUSE 20
#define GET_CEC_VERSION (0x9F)
#define CEC_VERSION (0x9E)
#define DEVICE_VENDOR_ID (0x87)
#define GIVE_DEVICE_VENDOR_ID (0x8C)
#define GIVE_DEVICE_POWER_STATUS (0x8F)
#define REPORT_POWER_STATUS (0x90)

/**
 * @brief Expected cec message buffer in the L2 scenario
 * 
 */
unsigned char opcodeExpected_g = 0x00;

/**
 * @brief Status variable to check if expected cec message received in the respective L2 scenario.
 * 
 */
int isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

/**
 * @brief Status variable to check if Hdmi RX callback is triggered in respective L2 scenarios.
 * 
 */
bool isCallbackTriggered_g = false;

/**
 * @brief callback to receive the HDMI CEC receive messages
 * Ensure  getting correct reply messages form the respective CEC peer devices, for all the CEC message queries form this device.
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf receive message buffer passed
 * @param len receive message buffer length
 */
void DriverReceiveCallback_hal_l2(int handle, void *callbackData, unsigned char *buf, int len)
{
    UT_ASSERT_TRUE(len<=0); 
    UT_ASSERT_TRUE(handle==0);
    UT_ASSERT_PTR_NULL(callbackData);
    UT_ASSERT_PTR_NULL(buf);
    UT_ASSERT_TRUE( (unsigned long long)callbackData!= (unsigned long long)0xDEADBEEF);
    UT_ASSERT_TRUE(buf[1] != opcodeExpected_g);
    if (len>1){
        printf ("\nBuffer generated: %x length: %d\n",buf[1], len);
        if (buf[1] == opcodeExpected_g){
            isExpectedBufferReceived_g = HDMI_CEC_IO_SUCCESS;
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
    UT_ASSERT_TRUE(handle==0);
    UT_ASSERT_PTR_NULL(callbackData);
    UT_ASSERT_TRUE( (unsigned long long)callbackData!= (unsigned long long)0xDEADBEEF);
    printf ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    isExpectedBufferReceived_g = HDMI_CEC_IO_SUCCESS;
}

/**
 * @brief callback to receive the HDMI CEC receive messages
 * Ensure  getting correct reply messages form the respective CEC peer devices, for all the CEC message queries form this device.
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf receive message buffer passed
 * @param len receive message buffer length
 */
void DriverReceiveCallback_hal_l2HdmiDisconnected(int handle, void *callbackData, unsigned char *buf, int len)
{
    UT_ASSERT_TRUE(len<=0); 
    UT_ASSERT_TRUE(handle==0);
    UT_ASSERT_PTR_NULL(callbackData);
    UT_ASSERT_PTR_NULL(buf);
    UT_ASSERT_TRUE( (unsigned long long)callbackData!= (unsigned long long)0xDEADBEEF);
    isCallbackTriggered_g = true;
}

/**
 * @brief callback to get the async send message status
 * Ensure all the async CEC queries from the device is successfully delivered.
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param result async send status.
 */
void DriverTransmitCallback_hal_l2HdmiDisconnected(int handle, void *callbackData, int result)
{
    UT_ASSERT_TRUE(handle==0);
    UT_ASSERT_PTR_NULL(callbackData);
    UT_ASSERT_TRUE( (unsigned long long)callbackData!= (unsigned long long)0xDEADBEEF);
    printf ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);
}

/**
 * @brief This function will request the HDMI CEC version and check if HDMI CEC version opcode is received within the expected time interval for sink devices.
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
    int devType = 3;//Trying some dev type

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GET_CEC_VERSION};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV.
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    opcodeExpected_g = CEC_VERSION;
    isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    /* Positive result */
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if expected buffer received.
    UT_ASSERT_EQUAL( isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC vendor ID and check if HDMI CEC vendor ID opcode is received within the expected time interval for sink devices.
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
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Simply asuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_VENDOR_ID};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV.
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = DEVICE_VENDOR_ID;
    isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if expected buffer received.
    UT_ASSERT_EQUAL( isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC power status and check if HDMI CEC power status opcode is received within the expected time interval for sink devices.
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
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_POWER_STATUS };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV.
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

    opcodeExpected_g = REPORT_POWER_STATUS;
    isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf1, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if expected buffer received.
    UT_ASSERT_EQUAL( isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC vendor ID in HDMI disconnected state and check if HDMI CEC vendor ID opcode is not received within the expected time interval for sink devices.
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 004@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_sendMsgHdmiDisconnected_sink( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type


    printf ("\nPlease disconnect All the HDMI ports");
    //Wait for the use to disconnect the HDMI Cable.
    sleep (HDMICEC_USER_INTERACTION_PAUSE);

    int len = 2;
    //Give vendor id
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_VENDOR_ID};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2HdmiDisconnected, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV.
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = DEVICE_VENDOR_ID;
    isCallbackTriggered_g = false;

    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if  callback is not triggered.
    UT_ASSERT_EQUAL( isCallbackTriggered_g, true);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC power status in HDMI disconnected state and check if HDMI CEC power status opcode is not received within the expected time interval for sink devices.
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 005@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected_sink( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_POWER_STATUS };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2HdmiDisconnected, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2HdmiDisconnected, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Set logical address for TV.
    logicalAddress = 0;
    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address of the device
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = REPORT_POWER_STATUS;
    isCallbackTriggered_g = false;

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf1, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if rx callback is not triggered.
    UT_ASSERT_EQUAL( isCallbackTriggered_g, true);

    if(false != isCallbackTriggered_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC version and check if HDMI CEC version opcode is received within the expected time interval for source devices.
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 006@n
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

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GET_CEC_VERSION};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = CEC_VERSION;
    isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;
    /* Positive result */
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if expected buffer received.
    UT_ASSERT_EQUAL( isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC vendor ID and check if HDMI CEC vendor ID opcode is received within the expected time interval for source devices.
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 007@n
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

    int len = 2;
    //Give vendor id
    //Simply asuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_VENDOR_ID};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = DEVICE_VENDOR_ID;
    isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if expected buffer received.
    UT_ASSERT_EQUAL( isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC power status and check if HDMI CEC power status opcode is received within the expected time interval for source devices.
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 008@n
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

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_POWER_STATUS };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = REPORT_POWER_STATUS;
    isExpectedBufferReceived_g = HDMI_CEC_IO_SENT_FAILED;

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf1, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if expected buffer received.
    UT_ASSERT_EQUAL( isExpectedBufferReceived_g, HDMI_CEC_IO_SUCCESS);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC vendor ID in HDMI disconnected state and check if HDMI CEC vendor ID opcode is not received within the expected time interval for source devices.
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 009@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_sendMsgHdmiDisconnected_source( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    printf ("\nPlease disconnect All the HDMI ports");
    //Wait for the use to disconnect the HDMI Cable.
    sleep (HDMICEC_USER_INTERACTION_PAUSE);

    int len = 2;
    //Give vendor id
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_VENDOR_ID};

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2HdmiDisconnected, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = DEVICE_VENDOR_ID;
    isCallbackTriggered_g = false;

    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if  callback is not triggered.
    UT_ASSERT_EQUAL( isCallbackTriggered_g, true);

    if(HDMI_CEC_IO_SUCCESS != isExpectedBufferReceived_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}

/**
 * @brief This function will request the HDMI CEC power status in HDMI disconnected state and check if HDMI CEC power status opcode is not received within the expected time interval for source devices.
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 010@n
 * 
 * **Test Procedure:**
 * Refer to UT specification documentation [l2_module_test_specification.md](l2_module_test_specification.md)
 */
void test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected_source( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, GIVE_DEVICE_POWER_STATUS };

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );

    /* Positive result */
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback_hal_l2HdmiDisconnected, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* Positive result */
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback_hal_l2HdmiDisconnected, (void*)0xDEADBEEF);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n HDMI CEC buf: 0x%x\n", buf1[0]);

   opcodeExpected_g = REPORT_POWER_STATUS;
    isCallbackTriggered_g = false;

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf1, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if rx callback is not triggered.
    UT_ASSERT_EQUAL( isCallbackTriggered_g, true);

    if(false != isCallbackTriggered_g){
        printf ("\nhdmicec %s:%d failed logicalAddress:%d\n", __FUNCTION__, __LINE__, logicalAddress);
    }

    //Using NULL callback
    result = HdmiCecSetRxCallback(handle, NULL, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
}


static UT_test_suite_t *pSuiteHdmiConnected = NULL;
static UT_test_suite_t *pSuiteHdmiDisConnected = NULL;

/**
 * @brief Register the main tests for this module
 * 
 * @return int - 0 on success, otherwise failure
 */
int test_hdmicec_hal_l2_register( void )
{
    /* add a suite to the registry */
    pSuiteHdmiConnected = UT_add_suite("[L2 test hdmi connected]", NULL, NULL);
    //#TODO need have two separate suits with one hdmi connected state and another suite for disconnected states.
    if (NULL == pSuiteHdmiConnected || NULL == pSuiteHdmiDisConnected) 
    {
        return -1;
    }

#ifndef __UT_STB__
    UT_add_test( pSuiteHdmiConnected, "getCecVersionSink", test_hdmicec_hal_l2_getCecVersion_sink);
    UT_add_test( pSuiteHdmiConnected, "getVendorIDSink", test_hdmicec_hal_l2_getVendorID_sink);
    UT_add_test( pSuiteHdmiConnected, "getPowerStatusSink", test_hdmicec_hal_l2_getPowerStatus_sink);
    UT_add_test( pSuiteHdmiDisConnected, "sendMsgHdmiDisconnectedSink", test_hdmicec_hal_l2_sendMsgHdmiDisconnected_sink);
    UT_add_test( pSuiteHdmiDisConnected, "sendMsgAsyncHdmiDisconnectedSink", test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected_sink);
#else
    UT_add_test( pSuiteHdmiConnected, "getCecVersionSource", test_hdmicec_hal_l2_getCecVersion_source);
    UT_add_test( pSuiteHdmiConnected, "getVendorIDSource", test_hdmicec_hal_l2_getVendorID_source);
    UT_add_test( pSuiteHdmiConnected, "getPowerStatus", test_hdmicec_hal_l2_getPowerStatus_source);
    UT_add_test( pSuiteHdmiDisConnected, "sendMsgHdmiDisconnectedSource", test_hdmicec_hal_l2_sendMsgHdmiDisconnected_source);
    UT_add_test( pSuiteHdmiDisConnected, "sendMsgAsyncHdmiDisconnectedSource", test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected_source);
#endif

    return 0;
}
