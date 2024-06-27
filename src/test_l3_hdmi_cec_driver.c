/**
*  If not stated otherwise in this file or this component's LICENSE
*  file the following copyright and licenses apply:
*
*  Copyright 2023 RDK Management
*
*  Licensed under the Apache License, Version 2.0 (the License);
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an AS IS BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/

/**
* @file TODO: test_l3_hdmi_cec_driver.c
* @page module_name TODO: Required field, name of the main module
* @subpage sub_page_name TODO: Add a function group if relevant
*
* ## Module's Role
* TODO: Explain the module's role in the system in general
* This is to ensure that the API meets the operational requirements of the module across all vendors.
*
* **Pre-Conditions:**  TODO: Add pre-conditions if any@n
* **Dependencies:** TODO: Add dependencies if any@n
*
* Ref to API Definition specification documentation :[hdmi-cec_halSpec.md](../../docs/pages/hdmi-cec_halSpec.md)
*/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

#include <ut.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>
#include "hdmi_cec_driver.h"

#define DEFAULT_LOGICAL_ADDRESS_PANEL 0

#define CEC_IMAGE_VIEW_ON 0x04
#define CEC_TEXT_VIEW_ON 0x0D
#define CEC_STANDBY 0x36
#define CEC_ACTIVE_SOURCE 0x82
#define CEC_REQUEST_ACTIVE_SOURCE 0x85
#define CEC_INACTIVE_SOURCE 0x9D

static UT_test_suite_t * pSuite = NULL;
static int gTestGroup = 1;
static int gTestID = 1;
volatile bool gImageViewOnReceived = false;

sem_t gSemCallbackReceived;
/**
 * @brief hdmicec receive message callback
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param buf receive message buffer passed
 * @param len receive message buffer length
 */
void ReceiveCallback(int handle, void *callbackData, unsigned char *buf, int len)
{
    int src, dest, opcode;
    unsigned char physical_address[4];
    UT_LOG ("\nBuffer generated: %x length: %d\n",buf, len);
    if((handle!=0) && (callbackData !=NULL) && (len>0))
    {
        UT_LOG("\nCall back data generated is \n");
        for (int index=0; index < len; index++)
        {
                UT_LOG("buf at index : %d is %x", index, buf[index]);
        }

        src = (buf[0] & 0xF0) >> 4;
        dest = buf[0] & 0x0F;
        opcode = buf[1];

        switch (opcode)
        {
            case CEC_IMAGE_VIEW_ON:
            {
                UT_LOG("\nImageViewOn triggered Src[%d] Dest[%d] \n", src, dest);
                gImageViewOnReceived = true;

            }
        }
        sem_post(&gSemCallbackReceived);
    }
    else 
    {
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
 * @brief callback to get the async send message status
 * 
 * @param handle Hdmi device handle
 * @param callbackData callback data passed
 * @param result async send status
 */
void TransmitCallback(int handle, void *callbackData, int result)
{
   if((handle!=0) && (callbackData !=NULL)) {
           //UT_ASSERT_TRUE_FATAL( (unsigned long long)callbackData== (unsigned long long)0xDEADBEEF);
           UT_LOG ("\ncallbackData returned: %x result: %d\n",callbackData, result);
   }
}


int TimedWaitForCallback(uint32_t timeOutSeconds)
{
    int s, result = -1;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeOutSeconds;
    while ((s = sem_timedwait(&gSemCallbackReceived, &ts)) == -1 && errno == EINTR)
    {
        continue;       /* Restart if interrupted by handler */
    }

   /* Check what happened */
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            result = 1;
        }
    }
    else
    {
        result = 0;
    }
    return result;
}


/**
*  HdmiCecOpen() Positive
*  HdmiCec
* 
*/
void test_l3_sink_image_view_on (void)
{
	int result;
    int handle = 0;
    gTestID = 1;

    UT_LOG("\n In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);
    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("open failed"); }

    result = HdmiCecSetRxCallback(handle, ReceiveCallback, (void*)0xDEADBEEF);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetRxCallback failed"); }

    result = HdmiCecSetTxCallback( handle, TransmitCallback, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    result = HdmiCecAddLogicalAddress( handle, DEFAULT_LOGICAL_ADDRESS_PANEL );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecAddLogicalAddress failed"); }

    gImageViewOnReceived = false;

    UT_LOG ("\nTrigger ImageViewOn\n");
    result = TimedWaitForCallback(10);
    if(result != 0 || !gImageViewOnReceived)
    {
        UT_FAIL("Failed to receive ImageViewOn"); 
    }
    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }

}

/**
 * @brief Register the main test(s) for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int test_l3_hdmi_cec_driver_register ( void )
{
	/* add a suite to the registry */
	pSuite = UT_add_suite( "[L3 hdmi_cec_driver]", NULL, NULL );
	if ( NULL == pSuite )
	{
		return -1;
	}	

	sem_init(&gSemCallbackReceived, 0, 0);
	UT_add_test( pSuite, "test_l3_active_source_trigger" ,test_l3_sink_image_view_on );

	return 0;
} 
