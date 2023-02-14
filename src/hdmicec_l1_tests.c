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

#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <ut.h>
#include "hdmi_cec_driver.h"
#ifdef __UT_STB__
    //Setting CEC play back logical address here.
    //More about CEC logical address read the documentation here
    //https://www.kernel.org/doc/html/v4.11/media/kapi/cec-core.html
    //https://elinux.org/CEC_(Consumer_Electronics_Control)_over_HDMI
    #define DEFAULT_LOGICAL_ADDRESS 3
#else
    //Setting the CEC sink (Dispaly divice) logical address here.
    #define DEFAULT_LOGICAL_ADDRESS 0
#endif

/**
 * @brief This function will do the unit testing of HdmiCecOpen ()
 * 
 * HdmiCecOpen () opens an instance of CEC driver.
 * Params of HdmiCecOpen
 *    handle - The handle used by application to uniquely identify the driver instance.
 * Return of HdmiCecOpen: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecOpen () API implementation is handling
 * the invalid call sequences to the API properly.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities.
 * In all the invalid call sequence 
 * scenarios API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS: will be returned if HdmiCecOpen is executed successfully.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if HdmiCecOpen is called second time or 
 * underlying platform implementation is failed.
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
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

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief This function will do the unit testing of HdmiCecClose ()
 * 
 * HdmiCecClose () close an instance of CEC driver.
 * Params of HdmiCecClose:
 *     handle - The handle returned from the HdmiCecOpen() function.
 * Return of HdmiCecClose: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecClose API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API properly.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecClose () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
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

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);  

    /*calling hdmicec_close second time */
    result = HdmiCecClose (handle); //#FIXME Seems like HdmiCecClose Getting stuck in amlogc HAL
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    handle = INT_MAX;
    /*calling hdmicec_close with invalid handle should return the invalid argument error*/
    result = HdmiCecClose (handle); //#FIXME Seems like HdmiCecClose Getting stuck in amlogc HAL
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}


/**
 * @brief This function will do the unit testing of HdmiCecSetLogicalAddress ()
 * 
 * HdmiCecSetLogicalAddress () Sets the Logical Addresses claimed by host device.
 * HdmiCecSetLogicalAddress function sets multiple logical addresses used by the host.  The host
 * has claimed these logical address through the Discovery process.  Once 
 * set, the host shall receive all CEC packets destined to these addresses.
 * Params of HdmiCecSetLogicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     logicalAddresses - The logicalAddresses set or replace addresses claimed by host. A null value clears the current list. 
 *     num - The number of logical addresses.  If any logical address in the list
 *                 cannot be set, none of them should be set upon return. Success
 *                 return indicates that all addresses in the list are set. Maximum possible value is 0xE
 * 
 * Return of HdmiCecSetLogicalAddress: respective HDMI_CEC_IO_ERROR
 * 
 * 
 * This UT function will ensure underlying HdmiCecSetLogicalAddress () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API .
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecSetLogicalAddress () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
 */
void test_hdmicec_hal_l1_setLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddresses [] = {4, 8, 11, 12, 13};
    int num = sizeof(logicalAddresses)/sizeof(int);

    //Calling api before open, should give invalid state
    result = HdmiCecSetLogicalAddress(handle, logicalAddresses, num);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );


    //+ve call
    result = HdmiCecSetLogicalAddress(handle, logicalAddresses, num);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Using invalid params
    result = HdmiCecSetLogicalAddress(0, logicalAddresses, num);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecSetLogicalAddress(handle, NULL, num);;
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecSetLogicalAddress(handle, logicalAddresses, 0);;
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecSetLogicalAddress(handle, logicalAddresses, 0xF);;
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    result = HdmiCecSetLogicalAddress(handle, logicalAddresses, num);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}


/**
 * @brief This function will do the unit testing of HdmiCecGetPhysicalAddress ()
 * 
 * HdmiCecGetPhysicalAddress () function get the Physical address for the device.
 * Params of HdmiCecGetPhysicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     physicalAddress - physical address acquired
 * Return of HdmiCecGetPhysicalAddress: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecGetPhysicalAddress () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecGetPhysicalAddress () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
 */
void test_hdmicec_hal_l1_getPhysicalAddress( void )
{
    int result = HDMI_CEC_IO_GENERAL_ERROR;
    int handle = 0;
    unsigned int physicalAddress = 0;

    //Calling api before open, should give invalid argument
    HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_STATE);

    /* Positive result */
    result = HdmiCecOpen (&handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS );


    HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    //Passing valid handle to the HdmiCecGetPhysicalAddress api should pass

    HdmiCecGetPhysicalAddress(handle, NULL);
    //TODO: passing NULL shouldn't cash.

    /*calling hdmicec_close should pass */
    result = HdmiCecClose (handle);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api after close, should give invalid argument
    HdmiCecGetPhysicalAddress(handle, &physicalAddress);
    //TODO: Calling after close shouldn't crash.

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief This function will do the unit testing of HdmiCecAddLogicalAddress ()
 * 
 * HdmiCecAddLogicalAddress (): The function shall return HDMI_CEC_IO_SUCCESS if the POLL message is sent
 * successfully and not ACK'd by any device on the bus. From this point on the
 * driver shall forward all received messages with destination being the acquired
 * logical address. Driver should ACK all POLL messages destined to this logical
 * address.
 * Params of HdmiCecGetPhysicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     logicalAddresses - The logical address to be acquired. Min value is 0x0 Max value is 0xE
 * Return of HdmiCecGetPhysicalAddress: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecAddLogicalAddress () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecAddLogicalAddress () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
 */
void test_hdmicec_hal_l1_addLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddress = INT_MAX;

    //Calling api before open, should give invalid argument
    result = HdmiCecAddLogicalAddress(handle, logicalAddress); //Code crash here
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
 * @brief This function will do the unit testing of HdmiCecRemoveLogicalAddress ()
 * 
 * HdmiCecRemoveLogicalAddress (): function releases the previously acquired logical address.  Once
 * released, driver should not ACK any POLL message destined to the
 * released address.
 * Params of HdmiCecRemoveLogicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     logicalAddresses - The logicalAddresses to be released. Min value is 0x0 Max value is 0xE
 * Return of HdmiCecRemoveLogicalAddress: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecRemoveLogicalAddress () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecRemoveLogicalAddress () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
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

    //Calling api before HdmiCecAddLogicalAddress, should give invalid argument
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecAddLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);

    //Calling api before HdmiCecAddLogicalAddress, should give invalid argument
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
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

    //Calling api after close, should give invalid argument
    result = HdmiCecRemoveLogicalAddress(handle, logicalAddress);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief This function will do the unit testing of HdmiCecGetLogicalAddress ()
 * 
 * HdmiCecGetLogicalAddress (): function get the logical address for the specified device type.
 * Params of HdmiCecGetLogicalAddress:
 *     handle - The handle returned from the HdmiCecOpen() function.
 *     devType - The device type (tuner, record, playback etc.).
 *     logicalAddress - The logical address acquired. Should be valid address.
 * Return of HdmiCecGetLogicalAddress: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecGetLogicalAddress () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecGetLogicalAddress () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
 */
void test_hdmicec_hal_l1_getLogicalAddress( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type

    //Calling api before open, should give invalid argument
    result = HdmiCecGetLogicalAddress(handle, devType,  &logicalAddress); //Code crash here
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
 * @brief This function will do the unit testing of HdmiCecSetRxCallback ()
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
 *     handle - The handle returned from the HdmiCecOpen(() function.
 *     cbfunc - A callback function to be invoked when a complete packet is received. This call back function is re-entrant
 *     data - the data used when invoking callback function. 
 * Return of HdmiCecSetRxCallback: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecSetRxCallback () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecSetRxCallback () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
 */
void test_hdmicec_hal_l1_setRxCallback( void )
{
    int result;
    int handle = 0;
    int logicalAddress = 0;
    int devType = 3;//Trying some dev type. This dummy variable

    //Calling api before open, should pass
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0); //Code crash here
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

    //Calling api after close, should retun success
    result = HdmiCecSetRxCallback(handle, DriverReceiveCallback, 0);
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_SUCCESS);
    /* #TODO: Unclear how the function will fail, maybe this function should be void? */
}

/**
 * @brief This function will do the unit testing of HdmiCecSetTxCallback ()
 * 
 * HdmiCecSetTxCallback (): function sets a callback function to be invoked once the async transmit
 * result is available. This is only necessary if application choose to transmit
 * the packet asynchronously.
 *
 * HdmiCecSetTxCallback function should block if callback invocation is in progress.
 * Params of HdmiCecSetRxCallback:
 *     handle  - The handle returned from the HdmiCecOpen(() function.
 *     cbfunc - Function pointer to be invoked when a complete packet is received.
 *     data - It is used when invoking callback function.
 * Return of HdmiCecSetRxCallback: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecSetTxCallback () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecSetTxCallback () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
 */
void test_hdmicec_hal_l1_setTxCallback( void )
{
    int result;
    int handle = 0;

    //Calling api before open, should pass
    result = HdmiCecSetTxCallback(handle, DriverTransmitCallback, 0); //Code crash here
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
 * @brief This function will do the unit testing of HdmiCecTx ()
 * 
 * HdmiCecTx (): function writes a complete CEC packet onto the CEC bus and wait for ACK.
 * application should check for result when return value of the function is 0;
 *
 * The bytes in buf that is to be transmitted should follow the buffer
 * byte format required for receiving buffer. (See detailed description from 
 * HdmiCecSetRxCallback)
 *
 * Params of HdmiCecTx:
 *    handle - The handle returned from the HdmiCecOpen(() function.
 *    buf - The buffer contains a complete CEC packet.
 *    len - Number of bytes in the packet.
 *    result - Output of the send. Possible results are SENT_AND_ACKD,
 *                    SENT_BUT_NOT_ACKD (e.g. no follower at the destination),
 *                    SENT_FAILED (e.g. collision).
 * Return of HdmiCecTx: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecTx () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecTx () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
 */
void test_hdmicec_hal_l1_hdmiCecTx( void )
{
    int result=SENT_AND_ACKD;
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
    result = HdmiCecTx(handle, buf, len, NULL); //#TODO Seems like amlogc HAL crashes here.
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    /* Invalid input */
    result = HdmiCecTx(handle, NULL, len, &ret); //#TODO Seems like amlogc HAL crashes here.
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
 * @brief This function will do the unit testing of HdmiCecTxAsync ()
 * 
 * HdmiCecTxAsync (): function writes a complete CEC packet onto the CEC bus but does not wait 
 * for ACK. The result will be reported via HdmiCecRxCallback_t if return value
 * of this function is 0.
 *
 * Params of HdmiCecTxAsync:
 *     handle - The handle returned from the HdmiCecOpen(() function.
 *     buf - Buffer contains a complete CEC packet.
 *     len - Number of bytes in the packet.
 * Return of HdmiCecTxAsync: respective HDMI_CEC_IO_ERROR
 * 
 * This UT function will ensure underlying HdmiCecTxAsync () API implementation is handling
 * the invalid arguments passed and invalid call sequences to the API.
 * This UT implementation will verify it by calling the function in all
 * invalid possibilities and passing invalid arguments to the respective APIs
 * In all the invalid cases
 * API should return the expected error codes defined in the respective HAL
 * documentation. Please see all the expected error codes and respective scenarios
 * in which the error codes will be returned.
 * HDMI_CEC_IO_SUCCESS  : will be returned if HdmiCecTxAsync () is executed successfully.
 * HDMI_CEC_IO_INVALID_ARGUMENT Indicates error due to invalid parameter value.
 * HDMI_CEC_IO_INVALID_STATE : will be returned if this api is called before calling HdmiCecOpen()
 * HDMI_CEC_IO_GENERAL_ERROR: Not able to simulate this condition with the UT implementation
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
    result = HdmiCecTxAsync(handle, NULL, len); //#TODO Axi6 crashes here
    UT_ASSERT_EQUAL( result, HDMI_CEC_IO_INVALID_ARGUMENT);

    result = HdmiCecTxAsync(0, buf, len); //#TODO Axi6 crashes here
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
    UT_add_test( pSuite, "getPhysicalAddress", test_hdmicec_hal_l1_getPhysicalAddress);
    UT_add_test( pSuite, "addLogicalAddress", test_hdmicec_hal_l1_addLogicalAddress);
    UT_add_test( pSuite, "removeLogicalAddress", test_hdmicec_hal_l1_removeLogicalAddress);
    UT_add_test( pSuite, "getLogicalAddress", test_hdmicec_hal_l1_getLogicalAddress);
    UT_add_test( pSuite, "setRxCallback", test_hdmicec_hal_l1_setRxCallback);
    UT_add_test( pSuite, "setTxCallback", test_hdmicec_hal_l1_setTxCallback);
    UT_add_test( pSuite, "hdmiCecTx", test_hdmicec_hal_l1_hdmiCecTx);
    UT_add_test( pSuite, "hdmiCecTxAsync", test_hdmicec_hal_l1_hdmiCecTxAsync);
    UT_add_test( pSuite, "setLogicalAddress", test_hdmicec_hal_l1_setLogicalAddress);

    return 0;
}
