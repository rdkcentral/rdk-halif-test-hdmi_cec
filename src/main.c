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

#ifdef VCOMPONENT
extern int register_vcomponent_tests ( char* profile, unsigned short cpPort, char* cpPath );
#endif

int main(int argc, char** argv)
{
    ut_kvp_status_t status;
    char szReturnedString[UT_KVP_MAX_ELEMENT_SIZE];

#ifdef VCOMPONENT
    int opt;
    char* pProfilePath = NULL;
    unsigned short cpPort = 8888;
    char* pUrl = NULL;

    while ((opt = getopt(argc, argv, "p:c:u:")) != -1)
    {
        switch(opt)
        {
            case 'p':
                UT_LOG ("Setting Profile path [%s]\n",optarg);
                pProfilePath = malloc(strlen(optarg) + 1);
                strcpy(pProfilePath, optarg);
                pProfilePath[strlen(optarg) + 1] = '\0';
                break;
            case 'c':
                cpPort = atoi(optarg);
                UT_LOG ("Setting control plane port [%d]\n",cpPort);
                break;

            case 'u':
                UT_LOG ("Setting control plane path [%s]\n",optarg);
                pUrl = malloc(strlen(optarg) + 1);
                strcpy(pUrl, optarg);
                pUrl[strlen(optarg) + 1] = '\0';
                break;
            case '?':
            case ':':
                UT_LOG("unknown option: %c\n", optopt);
                break;
        }
    }
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

    register_hdmicec_hal_l1_tests ();

    if(strncmp(szReturnedString,"source",UT_KVP_MAX_ELEMENT_SIZE) == 0) {
	register_hdmicec_hal_source_l2_tests ();
    }

    if(strncmp(szReturnedString,"sink",UT_KVP_MAX_ELEMENT_SIZE) == 0) {
	register_hdmicec_hal_sink_l2_tests ();
    }
#ifdef VCOMPONENT
    register_vcomponent_tests(pProfilePath, cpPort, pUrl);
#endif

    UT_run_tests();

#ifdef VCOMPONENT
    if(pProfilePath != NULL)
    {
        free(pProfilePath);
    }

    if(pUrl != NULL)
    {
        free(pUrl);
    }
#endif
}

/** @} */ // End of HDMI CEC HAL Tests Main File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
