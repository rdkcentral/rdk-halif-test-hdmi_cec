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
 * @defgroup HDMI_CEC_HALTESTS_Register HDMI CEC HAL Tests Register File
 * @{
 * @parblock
 *
 * ### Registration of tests for HDMI CEC HAL :
 *
 * Registration of tests for HDMI CEC HAL.
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
* @file test_register.c
*
*/


/* L1 Testing Functions */
extern int test_hdmidec_hal_l1_register( void );

/* L2 Testing Functions */
extern int test_register_hdmicec_hal_source_l2_tests( void );
extern int test_register_hdmicec_hal_sink_l2_tests( void );

/* L3 Testing Functions */
extern int test_register_hdmicec_hal_l3_tests(void);

int register_hdmicec_hal_l1_tests( void )
{
    int registerFailed=0;

    registerFailed |= test_hdmidec_hal_l1_register();

    return registerFailed;
}

int register_hdmicec_hal_source_l2_tests( void )
{
    int registerFailed=0;

    registerFailed |= test_register_hdmicec_hal_source_l2_tests();

    return registerFailed;
}
int register_hdmicec_hal_sink_l2_tests( void )
{
    int registerFailed=0;

    registerFailed |= test_register_hdmicec_hal_sink_l2_tests();

    return registerFailed;
}

int register_hdmicec_hal_l3_tests( void )
{
    int registerFailed=0;

    registerFailed |= test_register_hdmicec_hal_l3_tests();

    return registerFailed;
}


/** @} */ // End of HDMI CEC HAL Tests Register File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
