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

extern int register_hdmicec_hal_l1_tests( void );
extern int register_hdmicec_hal_source_l2_tests( void );
extern int register_hdmicec_hal_sink_l2_tests( void );
extern int register_vcomponent_tests ( char* profile, unsigned short cpPort, char* cpPath );

int main(int argc, char** argv) 
{
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



    /* Register tests as required, then call the UT-main to support switches and triggering */
    UT_init( argc, argv );

    register_hdmicec_hal_l1_tests ();
    register_hdmicec_hal_source_l2_tests ();
    register_hdmicec_hal_sink_l2_tests ();
    register_vcomponent_tests(pProfilePath, cpPort, pUrl);

    UT_run_tests();

    if(pProfilePath != NULL)
    {
        free(pProfilePath);
    }

    if(pUrl != NULL)
    {
        free(pUrl);
    }
}

/** @} */ // End of HDMI CEC HAL Tests Main File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
