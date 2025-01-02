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
 * @defgroup HDMI_CEC_HAL_EMULATORTESTS HDMI CEC HAL Virtual Component Tests File
 * @{
 * @parblock
 *
 * ### L1 Test Cases for HDMI CEC HAL Virtual Component:
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
 * @file test_vcomponent.c
 *
 */


#ifdef VCOMPONENT

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>

#include <ut.h>
#include <ut_cunit.h>
#include "ut_log.h"
#include "hdmi_cec_driver.h"
#include "vcHdmiCec.h"


struct vcomponent_info {
  char *pProfilePath;
  vcHdmiCec_t* handle;
};

struct vcomponent_info gVCInfo = {NULL, NULL};

void start_virtual_component (void)
{
    vcHdmiCec_Status_t status;

    if(gVCInfo.pProfilePath == NULL)
    {
        assert(gVCInfo.pProfilePath != NULL);
        return;
    }

    gVCInfo.handle = vcHdmiCec_Initialize();
    if(gVCInfo.handle == NULL)
    {
        UT_LOG("Error starting Virtual Component - vcHdmiCec_Initialize");
        return;
    }
    status = vcHdmiCec_Open(gVCInfo.handle, gVCInfo.pProfilePath, true);
    if(status != VC_HDMICEC_STATUS_SUCCESS)
    {
        UT_LOG("Error Starting Virtual Component vcHdmiCec_Open[%d]\n", status);
    }
    UT_LOG("Virtual Component started\n");
}

void stop_virtual_component(void)
{
    vcHdmiCec_Status_t status;
    if(gVCInfo.handle == NULL)
    {
        assert(gVCInfo.handle != NULL);
        return;
    }
    status = vcHdmiCec_Deinitialize(gVCInfo.handle);
    if(status != VC_HDMICEC_STATUS_SUCCESS)
    {
        UT_LOG("Error stopping Virtual Component - vcHdmiCec_Deinitialize");
    }
    gVCInfo.handle = NULL;
    UT_LOG("Virtual Component stopped\n");

}

static UT_test_suite_t * pSuite = NULL;

/**
 * @brief Register the main test(s) for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int register_vcomponent_tests ( char* profile)
{
    if(profile == NULL)
    {
        return -1;
    }
    gVCInfo.pProfilePath = profile;

    /* add a suite to the registry */
    pSuite = UT_add_suite( "[HDMI CEC Virtual Component Lifecycle control]", NULL, NULL );
    if ( NULL == pSuite )
    {
        return -1;
    }

    UT_add_test( pSuite, "start_virtual_component" , start_virtual_component );
    UT_add_test( pSuite, "stop_virtual_component" , stop_virtual_component );

    return 0;

}
#endif
/** @} */ // End of HDMI CEC HAL Virtual Component Tests File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
