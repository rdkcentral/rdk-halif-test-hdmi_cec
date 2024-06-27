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
#include <stdbool.h>

#include "vcomponent_hdmi_cec_command.h"
#include "vcomponent_hdmi_cec_device.h"

const static strVal_t gOpCodeStrVal [] = {
  { CMD_IMAGE_VIEW_ON, (int) CEC_IMAGE_VIEW_ON },
  { CMD_TEXT_VIEW_ON, (int) CEC_TEXT_VIEW_ON },
  { CMD_STANDBY , (int) CEC_STANDBY },
  { CMD_ACTIVE_SOURCE, (int) CEC_ACTIVE_SOURCE },
  { CMD_REPORT_PHYSICAL_ADDRESS, (int) CEC_REPORT_PHYSICAL_ADDRESS },
  { CMD_REQUEST_ACTIVE_SOURCE,  (int) CEC_REQUEST_ACTIVE_SOURCE },
  { CMD_GIVE_DEVICE_POWER_STATUS, (int) CEC_GIVE_DEVICE_POWER_STATUS },
  { CMD_REPORT_DEVICE_POWER_STATUS, (int) CEC_REPORT_DEVICE_POWER_STATUS },
  { CMD_INACTIVE_SOURCE, (int) CEC_INACTIVE_SOURCE }
};

void vCHdmiCec_Command_Clear(vCHdmiCec_command_t* cmd)
{
  if (cmd == NULL)
  {
    return;
  }

  cmd->initiator = LOGICAL_ADDRESS_UNKNOWN;
  cmd->destination = LOGICAL_ADDRESS_UNKNOWN;
  cmd->opcode = CEC_OPCODE_UNKNOWN;
  cmd->opcode_set = false;
  memset(cmd->parameter_data, 0, VCHDMICEC_MAX_DATA_SIZE);
  cmd->parameter_size = 0;
}

void vCHdmiCec_Command_Format(vCHdmiCec_command_t* cmd,
                                vCHdmiCec_logical_address_t initiator,
                                vCHdmiCec_logical_address_t destination,
                                vCHdmiCec_opcode_t opcode)
{
  if (cmd == NULL)
  {
    return;
  }
  vCHdmiCec_Command_Clear(cmd);
  cmd->initiator = initiator;
  cmd->destination = destination;
  if(opcode != CEC_OPCODE_UNKNOWN)
  {
    cmd->opcode = opcode;
    cmd->opcode_set = true;
  }
}

void vCHdmiCec_Command_PushBackByte(vCHdmiCec_command_t* cmd, uint8_t data)
{
  if (cmd == NULL)
  {
    return;
  }
/*
  if (cmd->initiator == LOGICAL_ADDRESS_UNKNOWN && cmd->destination == LOGICAL_ADDRESS_UNKNOWN)
  {
    cmd->initiator   = (vCHdmiCec_logical_address_t) (data >> 4);
    cmd->destination = (vCHdmiCec_logical_address_t) (data & 0xF);
  }
  else if (!cmd->opcode_set)
  {  
    cmd->opcode_set = true;
    cmd->opcode = (vCHdmiCec_opcode_t) data;
  }
  else */
  if(cmd->parameter_size < VCHDMICEC_MAX_DATA_SIZE)
  {
    cmd->parameter_data[cmd->parameter_size++] = data;
  }
}

void vCHdmiCec_Command_PushBackArray(vCHdmiCec_command_t* cmd, uint8_t* data, uint32_t len)
{  
  int i = 0;
  if (cmd == NULL || data == NULL || len > VCHDMICEC_MAX_DATA_SIZE)
  {
    return;
  }
  for (i = 0; i < len; i++)
  {
    vCHdmiCec_Command_PushBackByte(cmd, data[i]);
  }
}

uint32_t vCHdmiCec_Command_GetRawBytes(vCHdmiCec_command_t* cmd, uint8_t* buf, uint32_t buf_len)
{
  uint32_t frame_ptr = 0;
  uint32_t frame_size = cmd->parameter_size + 2;

  if ( cmd == NULL )
  {
    return 0;
  }

  if( buf == NULL || buf_len < frame_size)
  {
    return frame_size;
  }

  buf[frame_ptr++] = (cmd->initiator << 4) | cmd->destination;

  if(cmd->opcode_set)
  {
    buf[frame_ptr++] = cmd->opcode;
  }

  for (int i = 0; i < cmd->parameter_size; ++i) {
    buf[frame_ptr++] = cmd->parameter_data[i];
  }
  
  return frame_size;
}

vCHdmiCec_opcode_t vCHdmiCec_Command_GetOpCode(char* codeStr)
{
  if (codeStr == NULL)
  {
    return CEC_OPCODE_UNKNOWN;
  }
  return ((vCHdmiCec_opcode_t) vCHdmiCec_GetValue(gOpCodeStrVal, sizeof(gOpCodeStrVal)/sizeof(strVal_t), codeStr, CEC_OPCODE_UNKNOWN));
}