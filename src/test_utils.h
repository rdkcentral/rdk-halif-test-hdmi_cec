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
 * @addtogroup Device_Settings Device Settings Module
 * @{
 */

/**
 * @addtogroup Device_Settings_HALTEST Device Settings HAL Tests
 * @{
 */

/**
 * @defgroup Device_Settings_UTILS Device Settings HAL Tests Utility File
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
* @file test_utils.h
*
*/
#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include "hdmi_cec_driver.h"

#define TEST_UTIL_KVP_SIZE             256
#define TEST_UTIL_DEVICE_TYPE_SIZE     8
#define TEST_UTIL_DEVICE_NAME_SIZE     64
#define TEST_UTIL_DS_MODULE_NAME_SIZE  32



typedef enum _eCecDeviceType {
    cecDeviceNone     = 0,
    cecDeviceSink     = 1,
    cecDevicesSource  = 2,
}eCecDeviceType_t;

/* Global variables */
extern char    gDeviceType[TEST_UTIL_DEVICE_TYPE_SIZE];
extern char    gDeviceName[TEST_UTIL_DEVICE_NAME_SIZE];
extern int32_t gSourceType;
extern int32_t gCecDeviceType;
extern bool gExtendedEnumSupported;


/*Function prototypes */
int test_utils_parseconfig();
void test_utils_parseconfig_term();

#endif //__TEST_UTILS_H__
/** @} */ // End of Device_Settings_UTILS
/** @} */ // End of Device_Settings_HALTEST
/** @} */ // End of Device_Settings
/** @} */ // End of HPK