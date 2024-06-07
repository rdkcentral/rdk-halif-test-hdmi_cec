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
 * @defgroup HDMI_CEC_HAL_EMULATORTESTS HDMI CEC HAL Emulator Tests File
 * @{
 * @parblock
 *
 * ### L1 Test Cases for HDMI CEC HAL Emulator:
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
 * @file test_emulator.c
 * 
 */

#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#include <ut.h>
#include <ut_cunit.h>
#include "ut_log.h"
#include "hdmi_cec_driver.h"
#include "emulator.h"


struct emulator_info {
  char *pProfilePath;
  unsigned short cpPort;
  char* pCpPath;
  Emulator_t* handle;
} gEmulatorInfo = {NULL, 8888, NULL};



void start_emulator (void)
{
    if(gEmulatorInfo.pProfilePath != NULL)
    {
        Emulator_t* emulator = Emulator_Initialize(gEmulatorInfo.pProfilePath, gEmulatorInfo.cpPort, gEmulatorInfo.pCpPath);
        if(emulator != NULL)
        {
          gEmulatorInfo.handle = emulator;
          UT_LOG("Emulator started\n");

        }
    }
}

void stop_emulator(void)
{
    if(gEmulatorInfo.handle != NULL)
    {
        Emulator_Deinitialize(gEmulatorInfo.handle);
        gEmulatorInfo.handle = NULL;
        UT_LOG("Emulator stopped\n");
    }
}

static UT_test_suite_t * pSuite = NULL;

/**
 * @brief Register the main test(s) for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int register_emulator_tests ( char* profile, unsigned short cpPort, char* cpPath )
{
    if(profile != NULL)
    {
        gEmulatorInfo.pProfilePath = profile;
    }
    gEmulatorInfo.cpPort = cpPort;
    if(cpPath != NULL)
    {
        gEmulatorInfo.pCpPath = cpPath;
    }

    /* add a suite to the registry */
    pSuite = UT_add_suite( "[HDMI CEC Emulator Lifecycle control]", NULL, NULL );
    if ( NULL == pSuite )
    {
        return -1;
    }

    UT_add_test( pSuite, "start_emulator" , start_emulator );
    UT_add_test( pSuite, "stop_emulator" , stop_emulator );

    return 0;

}

/** @} */ // End of HDMI CEC HAL Emulator Tests File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
