/**
*  If not stated otherwise in this file or this component's LICENSE
*  file the following copyright and licenses apply:
*
*  Copyright 2024 RDK Management
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
 * @addtogroup HPK Hardware Porting Kit
 * @{
 * @par The Hardware Porting Kit
 * HPK is the next evolution of the well-defined Hardware Abstraction Layer
 * (HAL), but augmented with more comprehensive documentation and test suites
 * that OEM or SOC vendors can use to self-certify their ports before taking
 * them to RDKM for validation or to an operator for final integration and
 * deployment. The Hardware Porting Kit effectively enables an OEM and/or SOC
 * vendor to self-certify their own Video Accelerator devices, with minimal RDKM
 * assistance.
 *
 */

/**
 * @addtogroup Hdmi_Cec Hdmi Cec Module
 * @{
 */

/**
 * @addtogroup Hdmi_Cec_HALTEST Hdmi Cec HAL Tests
 * @{
 */

/**
 * @defgroup Hdmi_Cec_UTILS Hdmi Cec HAL Tests Utility File
 * @{
 * @parblock
 *
 * ### Utility functions for Device Settings HAL :
 *
 * Utility functions required for the module across all vendors.
 *
 * **Pre-Conditions:**  None @n
 * **Dependencies:** None @n
 *
 * @endparblock
 *
 */

/**
* @file test_utils.c
*
*/
#include <ut.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>
#include <stdlib.h>

#include "hdmi_cec_driver.h"
#include "test_utils.h"

/* Global Variables */
char    gDeviceType[TEST_UTIL_DEVICE_TYPE_SIZE];
char    gDeviceName[TEST_UTIL_DEVICE_NAME_SIZE];
int32_t gSourceType;
int32_t gCecDeviceType;
bool gExtendedEnumSupported;


static int test_utils_parse_hdmi_cec()
{
    char key_string[TEST_UTIL_KVP_SIZE];
    ut_kvp_status_t status;

    gExtendedEnumSupported   = ut_kvp_getBool( ut_kvp_profile_getInstance(), "extendedEnumsSupported" );
    gCecDeviceType = ut_kvp_getUInt32Field(ut_kvp_profile_getInstance(), "cecDeviceType");

    return 0;
}


/* Parse configuration file */
int test_utils_parseconfig()
{
    ut_kvp_status_t status;

    status = ut_kvp_getStringField(ut_kvp_profile_getInstance(), "Device/Type", gDeviceType, TEST_UTIL_DEVICE_TYPE_SIZE);

    if (status == UT_KVP_STATUS_SUCCESS ) {
        if (!strncmp(gDeviceType, TEST_UTIL_TYPE_SOURCE_VALUE, TEST_UTIL_DEVICE_TYPE_SIZE)) {
            gSourceType = 1;
        }
        else if(!strncmp(gDeviceType, TEST_UTIL_TYPE_SINK_VALUE, TEST_UTIL_DEVICE_TYPE_SIZE)) {
            gSourceType = 0;
        }
        else {
            UT_LOG_ERROR("Invalid platform type: %s", gDeviceType);
            return -1;
        }
    }
    else {
        UT_LOG_ERROR("Failed to get the platform type");
        return -1;
    }

    status = ut_kvp_getStringField(ut_kvp_profile_getInstance(), "Device/Name", gDeviceName, TEST_UTIL_DEVICE_NAME_SIZE);

    UT_LOG_DEBUG("Device Type: %s, Device Name: %s", gDeviceType, gDeviceName);

    status = test_utils_parse_hdmi_cec();
    if (status != UT_KVP_STATUS_SUCCESS ) {
        UT_LOG_ERROR("Failed to parse hdmi cec configurations");
        return -1;
    }
    /* Note: Add for other modules*/
    //if(gDSModule & dsVideoPort) {

    return 0;
}

void test_utils_parseconfig_term()
{
    if(gDSAudioPortConfiguration) {
        free(gDSAudioPortConfiguration);
    }

    if(gDSVideoPortConfiguration) {
        free(gDSVideoPortConfiguration);
    }
}

/** @} */ // End of Hdmi_Cec_UTILS
/** @} */ // End of Hdmi_cec_HALTEST
/** @} */ // End of Hdmi_Cec
/** @} */ // End of HPK