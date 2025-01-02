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
* @file TODO: test_vd_hdmi_cec_driver.c
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

#define MAX_WAIT_TIME_SECS 30
#define MAX_DATA_SIZE 64

#define CEC_IMAGE_VIEW_ON 0x04
#define CEC_TEXT_VIEW_ON 0x0D
#define CEC_STANDBY 0x36
#define CEC_SET_OSD_NAME 0x47
#define CEC_ACTIVE_SOURCE 0x82
#define CEC_REPORT_PHYSICAL_ADDRESS 0x84
#define CEC_REQUEST_ACTIVE_SOURCE 0x85
#define CEC_INACTIVE_SOURCE 0x9D
#define CEC_COMMAND_UNKNOWN 0x00

#define CMD_IMAGE_VIEW_ON "ImageViewOn"
#define CMD_TEXT_VIEW_ON "TextViewOn"
#define CMD_STANDBY "StandBy"
#define CMD_ACTIVE_SOURCE "ActiveSource"
#define CMD_REQUEST_ACTIVE_SOURCE "RequestActiveSource"
#define CMD_INACTIVE_SOURCE "InactiveSource"
#define CMD_SET_OSD_NAME "SetOsdName"
#define CMD_REPORT_PHYSICAL_ADDRESS "ReportPhysicalAddress"

#define VP_PREFIX "hdmicec/"
#define CMD_DATA_OSD_NAME "/osd_name"
#define VP_RESULT_INITIATOR "/result/initiator"
#define VP_RESULT_DESTINATION "/result/destination"
#define VP_RESULT_OPCODE "/result/opcode"
#define VP_RESULT_PARAMETER_SIZE "/result/parameters/size"
#define VP_RESULT_PARAMETER_DATA "/result/parameters/data/"

#define COUNT(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

typedef struct
{
  char* str;
  int val;
} strVal_t;

const static strVal_t gOpCode [] = {
  { CMD_IMAGE_VIEW_ON, CEC_IMAGE_VIEW_ON },
  { CMD_TEXT_VIEW_ON, CEC_TEXT_VIEW_ON },
  { CMD_SET_OSD_NAME, CEC_SET_OSD_NAME },
  { CMD_STANDBY , CEC_STANDBY },
  { CMD_ACTIVE_SOURCE, CEC_ACTIVE_SOURCE },
  { CMD_REQUEST_ACTIVE_SOURCE, CEC_REQUEST_ACTIVE_SOURCE },
  { CMD_INACTIVE_SOURCE, CEC_INACTIVE_SOURCE },
  { CMD_REPORT_PHYSICAL_ADDRESS, CEC_REPORT_PHYSICAL_ADDRESS}
};


static UT_test_suite_t * pSuite = NULL;
static int gTestGroup = 1;
static int gTestID = 1;

static uint8_t gExpectedCecCommand = CEC_COMMAND_UNKNOWN; //Set teach test before waiting for trigger
static sem_t gSemCallbackReceived;
static uint8_t gCommandValidated = CEC_COMMAND_UNKNOWN; //Set by receive callback after test validation is successful

static ut_kvp_instance_t *gValidationProfileInstance = NULL;

int GetOpCode(const strVal_t *map, int length, char* str)
{
  int result = -1;

  if(map == NULL || length <= 0 || str == NULL)
  {
    return result;
  }

  for (int i = 0;  i < length;  ++i)
  {
    if (!strcmp(str, map[i].str))
    {
        result = map[i].val;
        break;
    }
  }
  return result;
}

char* GetOpCodeString(const strVal_t *map, int length, int val)
{
  char* result = NULL;

  if(map == NULL || length <= 0)
  {
    return NULL;
  }

  for (int i = 0;  i < length;  ++i)
  {
    if (val == (int)map[i].val)
    {
        result = map[i].str;
        break;
    }
  }
  return result;
}


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
    uint8_t src, dest, opcode;
    uint8_t expected_src, expected_dest, expected_opcode;
    uint8_t physical_address[4];
    char* expected_command = NULL;
    ut_kvp_instance_t *vp_instance;
    char szReturnedString[UT_KVP_MAX_ELEMENT_SIZE];
    char field_name[UT_KVP_MAX_ELEMENT_SIZE];
    uint8_t parameter_data[UT_KVP_MAX_ELEMENT_SIZE];
    uint8_t parameter_size = 0;
    int length = 0, field_len = 0;

    UT_LOG ("\nBuffer generated: %x length: %d\n",buf, len);

    UT_ASSERT_MSG( handle != 0, "Error: Invalid handle.");
    UT_ASSERT_MSG( callbackData != NULL, "Error: Null callback data.");
    UT_ASSERT_MSG( len > 0, "Error: Invalid length.");

    vp_instance = (ut_kvp_instance_t *)callbackData;

    UT_LOG("\nCall back data generated is \n");
    for (int index=0; index < len; index++)
    {
            UT_LOG("buf at index : %d is %x", index, buf[index]);
    }

    src = (buf[0] & 0xF0) >> 4;
    dest = buf[0] & 0x0F;
    opcode = buf[1];

    expected_command = GetOpCodeString(gOpCode, COUNT(gOpCode), gExpectedCecCommand);
    strcpy(field_name, VP_PREFIX);
    strcpy(field_name + strlen(VP_PREFIX), expected_command);
    length = strlen(field_name);

    strcpy(field_name + length, VP_RESULT_INITIATOR);
    expected_src = ut_kvp_getUInt8Field (vp_instance, field_name);

    strcpy(field_name + length, VP_RESULT_DESTINATION);
    expected_dest = ut_kvp_getUInt8Field (vp_instance, field_name);

    strcpy(field_name + length, VP_RESULT_OPCODE);
    expected_opcode = ut_kvp_getUInt8Field (vp_instance, field_name);

    strcpy(field_name + length, VP_RESULT_PARAMETER_SIZE);
    parameter_size = ut_kvp_getUInt8Field (vp_instance, field_name);
    field_len = strlen(field_name);
    strcpy(field_name + length, VP_RESULT_PARAMETER_DATA);

    for (int i = 0; i < parameter_size; ++i)
    {
        int num_len = snprintf( NULL, 0, "%d", i );
        snprintf( field_name + field_len + 1 , num_len + 1, "%d", i );
        parameter_data[i] = ut_kvp_getUInt8Field (vp_instance, field_name);
        UT_LOG("Expected Parameter data at [%d]: 0x%02x",i,parameter_data[i]);
    }

    UT_LOG("\nExpected Command[%s] Src[%d] Dest[%d] \n", (GetOpCodeString(gOpCode, COUNT(gOpCode), gExpectedCecCommand)),expected_src, expected_dest);
    UT_LOG("\nCommand[%s] triggered Src[%d] Dest[%d] \n", (GetOpCodeString(gOpCode, COUNT(gOpCode), opcode)),src, dest);

    switch (opcode)
    {
        case CEC_ACTIVE_SOURCE:
        case CEC_INACTIVE_SOURCE:
        {
            UT_LOG("\nPhysicalAddress[%d.%d.%d.%d]\n",(buf[2] >> 4) & 0x0F, buf[2] & 0x0F, (buf[3] >> 4) & 0xF0, buf[3] & 0x0F);
        }
        break;
        case CEC_SET_OSD_NAME:
        {
            char str[len - 1];
            memcpy(str, &buf[2], len-2);
            str[len-2] = '\0';
            UT_LOG("\nosd_name[%s]\n", str);
        }
        break;
    }
    //Lets see if this the opcode that was expected. If it is not test will timeout
    if(gExpectedCecCommand == opcode)
    {
        //Lets validate the addresses and opcode
        if(src == expected_src && dest == expected_dest && opcode == expected_opcode)
        {
            UT_LOG("\nParameter size[%d] received size[%d]\n", parameter_size, len - 2);
            //Expected parameter size should be equal to length of cec buffer received minus 2 (Address, opcode bytes)
            gCommandValidated = gExpectedCecCommand;
            if(parameter_size > 0)
            {
                if((parameter_size != (len - 2)) || (memcmp(parameter_data, &buf[2], parameter_size) != 0))
                {
                    gCommandValidated = CEC_COMMAND_UNKNOWN;
                }
            }
        }
        //Even if the validation fails, signal the test as the opcode is same.
        sem_post(&gSemCallbackReceived);
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

static void validate_receive_callback_data(uint8_t expected_cmd, const char *cmd_str, const char *func, uint32_t timeOutSecs)
{
	int result;
    int handle = 0;
    gTestID = 1;

    UT_LOG("\n In %s [%02d%03d]\n", func, gTestGroup, gTestID);
    result = HdmiCecOpen( &handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("open failed"); }

    result = HdmiCecSetRxCallback(handle, ReceiveCallback, (void*)gValidationProfileInstance);
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetRxCallback failed"); }

    result = HdmiCecSetTxCallback( handle, TransmitCallback, (void*)0xDEADBEEF );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecSetTxCallback failed"); }

    result = HdmiCecAddLogicalAddress( handle, DEFAULT_LOGICAL_ADDRESS_PANEL );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL("HdmiCecAddLogicalAddress failed"); }

    gExpectedCecCommand = expected_cmd;
    gCommandValidated = CEC_COMMAND_UNKNOWN;

    UT_LOG ("\nTrigger %s\n", cmd_str);
    result = TimedWaitForCallback(timeOutSecs);
    if(result != 0)
    {
        UT_FAIL("Failed to receive expected command in callback");
    }
    if(gCommandValidated != gExpectedCecCommand)
    {
        UT_FAIL("Test Validation failed");
    }
    result = HdmiCecClose( handle );
    if (HDMI_CEC_IO_SUCCESS != result) { UT_FAIL_FATAL("close failed"); }
}


/**
*  Positive cases that receive callback data
*/
void test_vd_sink_image_view_on (void)
{
    validate_receive_callback_data(CEC_IMAGE_VIEW_ON, CMD_IMAGE_VIEW_ON, __FUNCTION__, MAX_WAIT_TIME_SECS);
}

void test_vd_sink_text_view_on (void)
{
    validate_receive_callback_data(CEC_TEXT_VIEW_ON, CMD_TEXT_VIEW_ON, __FUNCTION__, MAX_WAIT_TIME_SECS);
}

void test_vd_sink_active_source (void)
{
    validate_receive_callback_data(CEC_ACTIVE_SOURCE, CMD_ACTIVE_SOURCE, __FUNCTION__, MAX_WAIT_TIME_SECS);
}

void test_vd_sink_inactive_source (void)
{
    validate_receive_callback_data(CEC_INACTIVE_SOURCE, CMD_INACTIVE_SOURCE, __FUNCTION__, MAX_WAIT_TIME_SECS);
}

void test_vd_sink_set_osd_name (void)
{
    validate_receive_callback_data(CEC_SET_OSD_NAME, CMD_SET_OSD_NAME, __FUNCTION__, MAX_WAIT_TIME_SECS);
}

void test_vd_sink_add_device (void)
{
    validate_receive_callback_data(CEC_REPORT_PHYSICAL_ADDRESS, CMD_REPORT_PHYSICAL_ADDRESS, __FUNCTION__, MAX_WAIT_TIME_SECS);
}
/**
 * @brief Register the main test(s) for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int test_vd_hdmi_cec_driver_register ( char* validation_profile )
{
    ut_kvp_status_t status;
    if(validation_profile == NULL)
    {
        UT_FAIL("validation_profile NULL");
        return -1;
    }

    gValidationProfileInstance = ut_kvp_createInstance();
    assert(gValidationProfileInstance != NULL);
    status = ut_kvp_open(gValidationProfileInstance, validation_profile);
    if(status != UT_KVP_STATUS_SUCCESS)
    {
        UT_LOG_ERROR("ut_kvp_open: status: %d", status);
        assert(status == UT_KVP_STATUS_SUCCESS);
        return -1;
    }

	/* add a suite to the registry */
	pSuite = UT_add_suite( "[L3 hdmi_cec_driver]", NULL, NULL );
	if ( NULL == pSuite )
	{
		return -1;
	}

	sem_init(&gSemCallbackReceived, 0, 0);
	UT_add_test( pSuite, "test_vd_sink_image_view_on" ,test_vd_sink_image_view_on );
    UT_add_test( pSuite, "test_vd_sink_text_view_on" ,test_vd_sink_text_view_on );
    UT_add_test( pSuite, "test_vd_sink_active_source" ,test_vd_sink_active_source);
    UT_add_test( pSuite, "test_vd_sink_inactive_source" ,test_vd_sink_inactive_source );
    UT_add_test( pSuite, "test_vd_sink_set_osd_name" ,test_vd_sink_set_osd_name );
    UT_add_test( pSuite, "test_vd_sink_add_device" ,test_vd_sink_add_device );

	return 0;
}
