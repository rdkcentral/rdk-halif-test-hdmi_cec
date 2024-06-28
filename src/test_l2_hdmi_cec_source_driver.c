/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:*
* Copyright 2024 RDK Management
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
 * @defgroup HDMI_CEC_HALTESTS_L2_SOURCE HDMI CEC Source HALTEST L2 File for Source
 * @{
 * @parblock
 *
 * ### L2 Tests for HDMICEC Source HAL :
 * This module includes Level 2 functional tests (success and failure scenarios).
 * This is to ensure that the deepSleepMgr APIs meet the requirements across all vendors.
 *
 * **Pre-Conditions:**  None@n
 * **Dependencies:** None@n
 *
 * Ref to API Definition specification documentation : Refer to API Definition specification documentation : [hdmi-cec_halSpec.md](../../docs/pages/hdmi-cec_halSpec.md)
 *
 * @endparblock
 */
/**
 * @file test_l2_hdmi_cec_source_driver.c
 *
 */
/**
*
* ## Module's Role
* This module includes Level 2 functional tests (success and failure scenarios).
* This is to ensure that the hdmi_cec_driver APIs meet the requirements across all vendors.
*
* **Pre-Conditions:**  None@n
* **Dependencies:** None@n
*
* Ref to API Definition specification documentation : [hdmi-cec_halSpec.md](../../docs/pages/hdmi-cec_halSpec.md)
*/



#include <ut.h>
#include <ut_log.h>
#include "hdmi_cec_driver.h"

static int gTestGroup = 2;
static int gTestID = 1;

/**
* @brief This test validates the unavailability of logical address in HDMI CEC HAL
*
* This test case is designed to validate the scenario when the logical address
* is unavailable in the HDMI CEC HAL. The test invokes the HdmiCecOpen function
* and checks if the returned status is HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE.
* If the status is not HDMI_CEC_IO_SUCCESS, it invokes the HdmiCecClose function
* and checks if the returned status is HDMI_CEC_IO_SUCCESS.
*
* **Test Group ID:** 02@n
* **Test Case ID:** 001@n
*
* **Test Procedure:**
* Refer to UT specification documentation
* [hdmi_cec_source_hal_source_L2_Low-Level_TestSpecification.md](../docs/pages/hdmi-cec-source_L2_Low-Level_TestSpec.md)
*/

void test_l2_hdmi_cec_source_hal_ValidateLogicalAddressUnavailability(void)
{
    gTestID = 1;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int handle;
    HDMI_CEC_STATUS status;

    UT_LOG_DEBUG("Invoking HdmiCecOpen with valid handle");
    status = HdmiCecOpen(&handle);
    UT_LOG_DEBUG("Returned status: %d and handle: %d", status, handle);
    // Return value should be HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE, when the DUT is not connected to a Sink device.
    UT_ASSERT_EQUAL(status, HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE);

    if (status == HDMI_CEC_IO_SUCCESS || status == HDMI_CEC_IO_ALREADY_OPEN)
    {
        UT_LOG_DEBUG("Invoking HdmiCecClose with handle: %d", handle);
        HDMI_CEC_STATUS close_status = HdmiCecClose(handle);
        UT_LOG_DEBUG("Returned status: %d", close_status);

        UT_ASSERT_EQUAL_FATAL(close_status, HDMI_CEC_IO_SUCCESS);
    }

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

static UT_test_suite_t * pSuite = NULL;

/**
 * @brief Register the main tests for this module
 *
 * @return int - 0 on success, otherwise failure
 */

int test_register_hdmicec_hal_source_l2_tests(void)
{
    // Create the test suite
    pSuite = UT_add_suite("[L2 HDMICEC Source Test Case]", NULL, NULL);
    if (pSuite == NULL)
    {
        return -1;
    }
    // List of test function names and strings

    UT_add_test( pSuite, "L2_ValidateLogicalAddressUnavailability_source", test_l2_hdmi_cec_source_hal_ValidateLogicalAddressUnavailability);

    return 0;
}

/** @} */ // End of HDMI_CEC_HALTESTS_L2_SOURCE
/** @} */ // End of HDMI_CEC_HALTESTS
/** @} */ // End of HDMI_CEC Module
/** @} */ // End of HPK

