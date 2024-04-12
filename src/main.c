/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:*
* Copyright 2023 RDK Management
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
 
#include <ut.h>
#include "hdmi_cec_driver.h"
 
extern int register_hal_l2_tests( void );

int handle = 0;

int HdmiCec_PreReq()
{
    HDMI_CEC_STATUS status;
    status = HdmiCecOpen(&handle);
    if (status == HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("HdmiCecOpen() returned success and handle value is %d", handle);
        return 0;
    }
    else
    {
        UT_LOG("HdmiCecOpen() returned failure");
        UT_FAIL_FATAL(" Hdmi_cec pre-requisite failed");
    }
    return -1;
}
 
int HdmiCec_PostReq()
{
    HDMI_CEC_STATUS status;
    status = HdmiCecClose(handle);
    if (status == HDMI_CEC_IO_SUCCESS)
    {
        UT_LOG("HdmiCecClose() returned success");
        return 0;
    }
    else
    {
        UT_LOG("HdmiCecClose returned failure");
        UT_FAIL_FATAL("Hdmi_cec post-requisite failed");
    }
    return -1;
}

int main(int argc, char** argv)
{
    int registerReturn = 0;
    /* Register tests as required, then call the UT-main to support switches and triggering */
    UT_init( argc, argv );
    /* Check if tests are registered successfully */
    registerReturn = register_hal_l2_tests();
    if (registerReturn == 0)
    {
        printf("register_hal_l2_tests() returned success");
    }
    else
    {
        printf("register_hal_l2_tests() returned failure");
        return 1;
    }
    /* Begin test executions */
    UT_run_tests();
    return 0;
}