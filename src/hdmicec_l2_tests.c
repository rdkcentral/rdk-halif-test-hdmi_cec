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

/**
 * @brief Expected cec message buffer in the L2 scenario
 * 
 */
unsigned char bufferExpected_g = 0x00;

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
 * @brief callback to receive the hdmicec receive messages
 * Ensure  getting correct reply messages form the respective CEC peer devices, for all the CEC message queries form this device.
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
    UT_ASSERT_PTR_NULL(callbackData);
    UT_ASSERT_PTR_NULL(buf);
    UT_ASSERT_TRUE( (unsigned long long)callbackData!= (unsigned long long)0xDEADBEEF);
    UT_ASSERT_TRUE(buf[1] == bufferExpected_g);
    if (len>1){
        printf ("\nBuffer generated: %x length: %d\n",buf[1], len);
        if (buf[1] == bufferExpected_g){
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
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL(callbackData);
    UT_ASSERT_TRUE( (unsigned long long)callbackData!= (unsigned long long)0xDEADBEEF);
    printf ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    isExpectedBufferReceived_g = HDMI_CEC_IO_SUCCESS;
}

/**
 * @brief callback to receive the hdmicec receive messages
 * Ensure  getting correct reply messages form the respective CEC peer devices, for all the CEC message queries form this device.
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf receive message buffer passed
 * @param len receive message buffer length
 */
void DriverReceiveCallback_hal_l2HdmiDisconnected(int handle, void *callbackData, unsigned char *buf, int len)
{
    UT_ASSERT_TRUE(len>0); 
    UT_ASSERT_TRUE(handle!=0);
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
    UT_ASSERT_TRUE(handle!=0);
    UT_ASSERT_PTR_NULL(callbackData);
    UT_ASSERT_TRUE( (unsigned long long)callbackData!= (unsigned long long)0xDEADBEEF);
    printf ("\ncallbackData returned: %x result: %d\n",callbackData, result);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);
}

/**
 * @brief This function will do the functionality verification of  HdmiCec get version query
 * This function will send the query the hdmicec version using hdmi get cec version opcode
 * and check if hdmi set cec version opcode is received form the other end with in the
 * expected time interval
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 001@n
 * **Priority:** Low@n
 * 
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Test Procedure:**
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecSetTxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set TX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTx(handle, buf, len, &ret) - send the cec message to get the cec version after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecSetTxCallback(handle, NULL, 0) - unregister TX call back | handle, NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l2_getCecVersion( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    int len = 2;
    //Get CEC Version. return expected is opcode: CEC Version :43 9E 05
    //Simply assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, 0x9F};

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

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n hdmicec buf: 0x%x\n", buf1[0]);

    bufferExpected_g = 0x9E;
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
 * @brief This function will do the functionality verification of  HdmiCec get vendor id
 * This function will send the query the vendor id using hdmi get vendor ID opcode
 * and check if hdmi set vendor ID opcode is received form the other end with in the
 * expected time interval
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 002@n
 * **Priority:** Low@n
 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Test Procedure:**
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecSetTxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set TX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTx(handle, buf, len, &ret) - send the cec message to get the vendor id after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecSetTxCallback(handle, NULL, 0) - unregister TX call back | handle, NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l2_getVendorID( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Simply asuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, 0x8C};

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

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n hdmicec buf: 0x%x\n", buf1[0]);

    bufferExpected_g = 0x87;
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
 * @brief This function will do the functionality verification of  HdmiCec get power status
 * This function will send the query the power status using hdmi get power status opcode
 * and check if hdmi set power status opcode is received form the other end with in the
 * expected time interval
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 003@n
 * **Priority:** Low@n
 *
 * **Pre-Conditions:**@n
 * Connect at least one CEC enabled device.
 * 
 * **Test Procedure:**
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecSetTxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set TX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTx(handle, buf, len, &ret) - send the cec message to get the power status after correct module initialization and ensure response is received with in expected response delay time. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecSetTxCallback(handle, NULL, 0) - unregister TX call back | handle, NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l2_getPowerStatus( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, 0x8F };

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

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n hdmicec buf: 0x%x\n", buf1[0]);

    bufferExpected_g = 0x90;
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
 * @brief This function will do the functionality verification of  HdmiCec get vendor id
 * This function will send the query the vendor id using hdmi get vendor ID opcode
 * and check if hdmi set vendor ID opcode is received form the other end with in the
 * expected time interval
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 002@n
 * **Priority:** Low@n
 *
 * **Pre-Conditions:**@n
 * All of the device HDMI ports should be disconnected.
 * 
 * **Test Procedure:**
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecSetTxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set TX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTx(handle, buf, len, &ret) - send the cec message to get the vendor id after correct module initialization and ensure receive callback is not triggered. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecSetTxCallback(handle, NULL, 0) - unregister TX call back | handle, NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l2_sendMsgHdmiDisconnected( void )
{
    int result=0;
    int ret=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Simply asuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, 0x8C};

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

    //Get logical address.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n hdmicec buf: 0x%x\n", buf1[0]);

    bufferExpected_g = 0x87;
    isCallbackTriggered_g = false;

    result = HdmiCecTx(handle, buf1, len, &ret);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SENT_BUT_NOT_ACKD);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if  callback is not triggered.
    UT_ASSERT_EQUAL( isCallbackTriggered_g, false);

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
 * @brief This function will do the functionality verification of  HdmiCec get power status
 * This function will send the query the power status using hdmi get power status opcode
 * and check if hdmi set power status opcode is received form the other end with in the
 * expected time interval
 * 
 * **Test Group ID:** 02@n
 * **Test Case ID:** 003@n
 * **Priority:** Low@n
 *
 * **Pre-Conditions:**@n
 * All of the device HDMI ports should be disconnected.
 * 
 * **Test Procedure:**
 * |Variation / Step|Description|Test Data|Expected Result|Notes|
 * |:--:|---------|----------|--------------|-----|
 * |02|call HdmiCecOpen(&hdmiHandle) - open interface | handle | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |03|call HdmiCecSetTxCallback(handle, DriverReceiveCallback_hal_l2, 0xDEADBEEF) - set TX call back with valid parameters | handle, DriverTransmitCallback, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |04|call HdmiCecAddLogicalAddress(handle, logicalAddress) - call add logical address with valid arguments | handle, logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |05|call HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress) - call get logical address with valid arguments | handle, devType, &logicalAddress | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |06|call HdmiCecTx(handle, buf, len, &ret) - send the cec message to get the power status after correct module initialization and ensure ensure receive callback is not triggered. | handle, buf, len, &ret | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |07|call HdmiCecSetTxCallback(handle, NULL, 0) - unregister TX call back | handle, NULL, data address | HDMI_CEC_IO_SUCCESS| Should Pass |
 * |08|call HdmiCecClose (handle) - close interface | handle=hdmiHandle | HDMI_CEC_IO_SUCCESS| Should Pass |
 */
void test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected( void )
{
    int result=0;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    int len = 2;
    //Give vendor id
    //Assuming sender as 3 and broadcast
    unsigned char buf1[] = {0x3F, 0x8F };

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

    //Get logical address for STB.
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    buf1[0] = ((logicalAddress&0xFF)<<4)|0x0F; printf ("\n hdmicec buf: 0x%x\n", buf1[0]);

    bufferExpected_g = 0x90;
    isCallbackTriggered_g = false;

    /* Positive result */
    result = HdmiCecTxAsync(handle, buf1, len);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Wait for 2 sec for the reply
    sleep (HDMICEC_RESPONSE_TIMEOUT);
    //Check if rx callback is not triggered.
    UT_ASSERT_EQUAL( isCallbackTriggered_g, false);

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

static UT_test_suite_t *pSuite = NULL;

/**
 * @brief Register the main tests for this module
 * 
 * @return int - 0 on success, otherwise failure
 */
int test_hdmicec_hal_l2_register( void )
{
    /* add a suite to the registry */
    pSuite = UT_add_suite("[L2 test_Example]", NULL, NULL);
    if (NULL == pSuite) 
    {
        return -1;
    }

    UT_add_test( pSuite, "getCecVersion", test_hdmicec_hal_l2_getCecVersion);
    UT_add_test( pSuite, "getVendorID", test_hdmicec_hal_l2_getVendorID);
    UT_add_test( pSuite, "getPowerStatus", test_hdmicec_hal_l2_getPowerStatus);
    UT_add_test( pSuite, "sendMsgHdmiDisconnected", test_hdmicec_hal_l2_sendMsgHdmiDisconnected);
    UT_add_test( pSuite, "sendMsgAsyncHdmiDisconnected", test_hdmicec_hal_l2_sendMsgAsyncHdmiDisconnected);

    return 0;
}
