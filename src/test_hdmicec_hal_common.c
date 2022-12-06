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

#include <string.h>
#include <stdlib.h>
#include <setjmp.h>

//ccec/drivers/include/ccec/drivers/hdmi_cec_driver.h
#include "hdmi_cec_driver.h"

#include <ut.h>

int handle_g;

int internalInit( void )
{
    int result;
    //result = HdmiCecOpen (&handle_g);
    return 0;
}

int internalClean( void )
{
    int result;

    /*calling hdmicec_close should pass */
    if (NULL != handle_g) {
        //result = HdmiCecClose (handle_g);
    }
    return 0;
}

