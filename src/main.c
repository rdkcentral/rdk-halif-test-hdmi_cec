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
 * @defgroup HDMI_CEC_HALTESTS_Main HDMI CEC HAL Tests Main File
 * @{
 * @parblock
 *
 * ### Tests for HDMI CEC HAL :
 *
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
* @file main.c
*
*/

#include <ut.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>

extern int register_hdmicec_hal_l1_tests( void );
extern int register_hdmicec_hal_source_l2_tests( void );
extern int register_hdmicec_hal_sink_l2_tests( void );
extern int register_hdmicec_hal_l3_tests( void );

#ifdef VCOMPONENT
extern int register_vcomponent_tests ( char* profile );
extern int test_vd_hdmi_cec_driver_register ( char* pValidationProfilePath );
#endif

int main(int argc, char** argv)
{
    ut_kvp_status_t status;
    char szReturnedString[UT_KVP_MAX_ELEMENT_SIZE];
    int registerReturn = 0;

#ifdef VCOMPONENT
    int opt;
    char* pProfilePath = NULL;
    char* pValidationProfilePath = NULL;

    while ((opt = getopt(argc, argv, "u:v:")) != -1)
    {
        switch(opt)
        {
            case 'u':
                UT_LOG ("Setting Profile path [%s]\n",optarg);
                pProfilePath = malloc(strlen(optarg) + 1);
                strcpy(pProfilePath, optarg);
                pProfilePath[strlen(optarg) + 1] = '\0';
                break;
            case 'v':
                UT_LOG ("Setting Validation Profile path [%s]\n",optarg);
                pValidationProfilePath = malloc(strlen(optarg) + 1);
                strcpy(pValidationProfilePath, optarg);
                pValidationProfilePath[strlen(optarg) + 1] = '\0';
                break;

            case '?':
            case ':':
                UT_LOG("unknown option: %c\n", optopt);
                break;
        }
    }
    optind = 1; //Reset argv[] element pointer for further processing
#endif

    /* Register tests as required, then call the UT-main to support switches and triggering */
    UT_init( argc, argv );

    status = ut_kvp_getStringField(ut_kvp_profile_getInstance(), "hdmicec/type", szReturnedString, UT_KVP_MAX_ELEMENT_SIZE);
    if (status == UT_KVP_STATUS_SUCCESS ) {
        UT_LOG_DEBUG("Device Type: %s", szReturnedString);
    }
    else {
        UT_LOG_ERROR("Failed to get the platform Device Type");
        return -1;
    }

    registerReturn = register_hdmicec_hal_l1_tests();
    if ( registerReturn == -1 )
    {
        UT_LOG_ERROR("\n register_hdmicec_hal_l1_tests() returned failure");
        return -1;
    }
#ifdef VCOMPONENT
    register_vcomponent_tests(pProfilePath);
    test_vd_hdmi_cec_driver_register (pValidationProfilePath);
#endif

    if(strncmp(szReturnedString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0) {
         register_hdmicec_hal_source_l2_tests ();
    }

    if(strncmp(szReturnedString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0) {
         register_hdmicec_hal_sink_l2_tests ();
    }

    register_hdmicec_hal_l3_tests ();

    UT_run_tests();

#ifdef VCOMPONENT
    if(pProfilePath != NULL)
    {
        free(pProfilePath);
    }
    if(pValidationProfilePath != NULL)
    {
        free(pValidationProfilePath);
    }
#endif
}

/** @} */ // End of HDMI CEC HAL Tests Main File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
