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
#include "hdmi_cec_driver.h"


HDMI_CEC_STATUS HdmiCecOpen(int* handle)
{
  (void)handle;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecClose(int handle)
{
  (void)handle;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecSetLogicalAddress(int handle, int* logicalAddresses, int num)
{
  (void)handle;
  (void)logicalAddresses;
  (void)num;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecGetPhysicalAddress(int handle, unsigned int* physicalAddress)
{
  (void)handle;
  (void)physicalAddress;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
  (void)handle;
  (void)logicalAddresses;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{
  (void)handle;
  (void)logicalAddresses;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecGetLogicalAddress(int handle, int* logicalAddress)
{
  (void)handle;
  (void)logicalAddress;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  (void)handle;
  (void)cbfunc;
  (void)data;
  return (int)0;
}
/** @note This API is deprecated*/
HDMI_CEC_STATUS HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void* data)
{
  (void)handle;
  (void)cbfunc;
  (void)data;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecTx(int handle, const unsigned char* buf, int len, int* result)
{
  (void)handle;
  (void)buf;
  (void)len;
  (void)result;
  return (int)0;
}
/** @note This API is deprecated */
HDMI_CEC_STATUS HdmiCecTxAsync(int handle, const unsigned char* buf, int len)
{
  (void)handle;
  (void)buf;
  (void)len;
  return (int)0;
}

