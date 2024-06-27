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

#ifdef VCOMPONENT
extern int register_vcomponent_tests ( char* profilei );
#endif

int main(int argc, char** argv) 
{
#ifdef VCOMPONENT
    int opt;
    char* pProfilePath = NULL;

    while ((opt = getopt(argc, argv, "u:")) != -1)
    {
        switch(opt)
        {
            case 'u':
                UT_LOG ("Setting Profile path [%s]\n",optarg);
                pProfilePath = malloc(strlen(optarg) + 1);
                strcpy(pProfilePath, optarg);
                pProfilePath[strlen(optarg) + 1] = '\0';
                break;

            case '?':
            case ':':
                UT_LOG("unknown option: %c\n", optopt);
                break;
        }
    }
#endif
    optind = 1; //Reset argv[] element pointer for further processing

    /* Register tests as required, then call the UT-main to support switches and triggering */
    UT_init( argc, argv );

    register_hdmicec_hal_l1_tests ();
#ifdef VCOMPONENT
    register_vcomponent_tests(pProfilePath);
#endif

    UT_run_tests();

#ifdef VCOMPONENT
    if(pProfilePath != NULL)
    {
        free(pProfilePath);
    }
#endif
}

/** @} */ // End of HDMI CEC HAL Tests Main File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
