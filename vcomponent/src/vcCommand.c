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

#include "vcCommand.h"
#include "vcDevice.h"

const static vcCommand_strVal_t gOpCodeStrVal [] = {
  { CMD_FEATURE_ABORT, (int) CEC_FEATURE_ABORT },
  { CMD_IMAGE_VIEW_ON, (int) CEC_IMAGE_VIEW_ON },
  { CMD_TUNER_STEP_INCREMENT, (int) CEC_TUNER_STEP_INCREMENT },
  { CMD_TUNER_STEP_DECREMENT, (int) CEC_TUNER_STEP_DECREMENT },
  { CMD_TUNER_DEVICE_STATUS, (int) CEC_TUNER_DEVICE_STATUS },
  { CMD_GIVE_TUNER_DEVICE_STATUS, (int) CEC_GIVE_TUNER_DEVICE_STATUS },
  { CMD_RECORD_ON, (int) CEC_RECORD_ON },
  { CMD_RECORD_STATUS, (int) CEC_RECORD_STATUS },
  { CMD_RECORD_OFF, (int) CEC_RECORD_OFF },
  { CMD_TEXT_VIEW_ON, (int) CEC_TEXT_VIEW_ON },
  { CMD_RECORD_TV_SCREEN, (int) CEC_RECORD_TV_SCREEN },
  { CMD_GIVE_DECK_STATUS, (int) CEC_GIVE_DECK_STATUS },
  { CMD_DECK_STATUS, (int) CEC_DECK_STATUS },
  { CMD_DECK_CONTROL, (int) CEC_DECK_CONTROL },
  { CMD_PLAY, (int) CEC_PLAY },
  { CMD_TUNE_DIGITAL_SERVICE, (int) CEC_TUNE_DIGITAL_SERVICE },
  { CMD_TUNE_ANALOG_SERVICE, (int) CEC_TUNE_ANALOG_SERVICE },
  { CMD_TUNE_SOURCE, (int) CEC_TUNE_SOURCE },
  { CMD_TUNE_CHANNEL, (int) CEC_TUNE_CHANNEL },
  { CMD_GIVE_PHYSICAL_ADDRESS, (int) CEC_GIVE_PHYSICAL_ADDRESS },
  { CMD_REPORT_PHYSICAL_ADDRESS, (int) CEC_REPORT_PHYSICAL_ADDRESS },
  { CMD_GIVE_OSD_NAME, (int) CEC_GIVE_OSD_NAME },
  { CMD_SET_OSD_NAME, (int) CEC_SET_OSD_NAME },
  { CMD_GIVE_AUDIO_STATUS, (int) CEC_GIVE_AUDIO_STATUS },
  { CMD_SYSTEM_AUDIO_MODE_REQUEST, (int) CEC_SYSTEM_AUDIO_MODE_REQUEST },
  { CMD_SET_SYSTEM_AUDIO_MODE, (int) CEC_SET_SYSTEM_AUDIO_MODE },
  { CMD_SYSTEM_AUDIO_MODE_STATUS, (int) CEC_SYSTEM_AUDIO_MODE_STATUS },
  { CMD_USER_CONTROL_PRESSED, (int) CEC_USER_CONTROL_PRESSED },
  { CMD_USER_CONTROL_RELEASED, (int) CEC_USER_CONTROL_RELEASED },
  { CMD_SET_STREAM_PATH, (int) CEC_SET_STREAM_PATH },
  { CMD_STANDBY, (int) CEC_STANDBY },
  { CMD_GIVE_DEVICE_VENDOR_ID, (int) CEC_GIVE_DEVICE_VENDOR_ID },
  { CMD_DEVICE_VENDOR_ID, (int) CEC_DEVICE_VENDOR_ID },
  { CMD_GIVE_CEC_VERSION, (int) CEC_GIVE_CEC_VERSION },
  { CMD_CEC_VERSION, (int) CEC_CEC_VERSION },
  { CMD_GIVE_DEVICE_POWER_STATUS, (int) CEC_GIVE_DEVICE_POWER_STATUS },
  { CMD_REPORT_POWER_STATUS, (int) CEC_REPORT_POWER_STATUS },
  { CMD_SET_MENU_LANGUAGE, (int) CEC_SET_MENU_LANGUAGE },
  { CMD_GIVE_DEVICE_FEATURE, (int) CEC_GIVE_DEVICE_FEATURE },
  { CMD_REPORT_DEVICE_FEATURE, (int) CEC_REPORT_DEVICE_FEATURE },
  { CMD_GIVE_AUDIO_MODE_STATUS, (int) CEC_GIVE_AUDIO_MODE_STATUS },
  { CMD_SET_OSD_STRING, (int) CEC_SET_OSD_STRING },
  { CMD_REQUEST_ACTIVE_SOURCE, (int) CEC_REQUEST_ACTIVE_SOURCE },
  { CMD_ACTIVE_SOURCE, (int) CEC_ACTIVE_SOURCE },
  { CMD_INACTIVE_SOURCE, (int) CEC_INACTIVE_SOURCE },
  { CMD_GIVE_SYSTEM_AUDIO_MODE_STATUS, (int) CEC_GIVE_SYSTEM_AUDIO_MODE_STATUS },
  { CMD_ROUTING_CHANGE, (int) CEC_ROUTING_CHANGE },
  { CMD_ROUTING_INFORMATION, (int) CEC_ROUTING_INFORMATION },
  { CMD_SET_AUDIO_RATE, (int) CEC_SET_AUDIO_RATE },
  { CMD_INITIATE_ARC, (int) CEC_INITIATE_ARC },
  { CMD_REPORT_ARC_INITIATED, (int) CEC_REPORT_ARC_INITIATED },
  { CMD_REPORT_ARC_TERMINATED, (int) CEC_REPORT_ARC_TERMINATED },
  { CMD_REQUEST_ARC_INITIATION, (int) CEC_REQUEST_ARC_INITIATION },
  { CMD_REQUEST_ARC_TERMINATION, (int) CEC_REQUEST_ARC_TERMINATION },
  { CMD_TERMINATE_ARC, (int) CEC_TERMINATE_ARC }
};

void vcCommand_Clear(vcCommand_t* cmd)
{
  if (cmd == NULL)
  {
    VC_LOG("vcCommand_Clear: cmd NULL");
    return;
  }

  cmd->initiator = LOGICAL_ADDRESS_UNKNOWN;
  cmd->destination = LOGICAL_ADDRESS_UNKNOWN;
  cmd->opcode = CEC_OPCODE_UNKNOWN;
  cmd->opcode_set = false;
  memset(cmd->parameter_data, 0, VCCOMMAND_MAX_DATA_SIZE);
  cmd->parameter_size = 0;
}

void vcCommand_Format(vcCommand_t* cmd,
                                vcCommand_logical_address_t initiator,
                                vcCommand_logical_address_t destination,
                                vcCommand_opcode_t opcode)
{
  if (cmd == NULL)
  {
    VC_LOG("vcCommand_Format: cmd NULL");
    return;
  }
  vcCommand_Clear(cmd);
  cmd->initiator = initiator;
  cmd->destination = destination;
  if(opcode != CEC_OPCODE_UNKNOWN)
  {
    cmd->opcode = opcode;
    cmd->opcode_set = true;
  }
}

void vcCommand_PushBackByte(vcCommand_t* cmd, uint8_t data)
{
  if (cmd == NULL)
  {
    VC_LOG("vcCommand_PushBackByte: cmd NULL");
    return;
  }
/*
  if (cmd->initiator == LOGICAL_ADDRESS_UNKNOWN && cmd->destination == LOGICAL_ADDRESS_UNKNOWN)
  {
    cmd->initiator   = (vcCommand_logical_address_t) (data >> 4);
    cmd->destination = (vcCommand_logical_address_t) (data & 0xF);
  }
  else if (!cmd->opcode_set)
  {  
    cmd->opcode_set = true;
    cmd->opcode = (vcCommand_opcode_t) data;
  }
  else */
  if(cmd->parameter_size < VCCOMMAND_MAX_DATA_SIZE)
  {
    cmd->parameter_data[cmd->parameter_size++] = data;
  }
}

void vcCommand_PushBackArray(vcCommand_t* cmd, uint8_t* data, uint32_t len)
{  
  int i = 0;
  if (cmd == NULL)
  {
    VC_LOG("vcCommand_PushBackArray: cmd NULL");
  }
  if(data == NULL || len > VCCOMMAND_MAX_DATA_SIZE)
  {
    VC_LOG("vcCommand_PushBackArray: data invalid");
    return;
  }
  for (i = 0; i < len; i++)
  {
    vcCommand_PushBackByte(cmd, data[i]);
  }
}

uint32_t vcCommand_GetRawBytes(vcCommand_t* cmd, uint8_t* buf, uint32_t buf_len)
{
  uint32_t frame_ptr = 0;
  uint32_t frame_size = cmd->parameter_size + 2;

  if ( cmd == NULL )
  {
    VC_LOG("vcCommand_GetRawBytes: cmd NULL");
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

int vcCommand_GetValue(const vcCommand_strVal_t *map, int length, const char* str, int default_val)
{
  int result = default_val;
  
  if(map == NULL || length <= 0 || str == NULL)
  {
    return result;
  }

  for (int i = 0;  i < length;  ++i)
  {
    if (!strcmp(str, map[i].str))
    {
        result = map[i].val;
        break;
    }
  }
  return result;
}

const char* vcCommand_GetString(const vcCommand_strVal_t *map, int length, int val)
{
  char* result = NULL;
  
  if(map == NULL || length <= 0)
  {
    return NULL;
  }

  for (int i = 0;  i < length;  ++i)
  {
    if (val == (int)map[i].val)
    {
        result = map[i].str;
        break;
    }
  }
  return result;
}

vcCommand_opcode_t vcCommand_GetOpCode(char* codeStr)
{
  if (codeStr == NULL)
  {
    return CEC_OPCODE_UNKNOWN;
  }
  return ((vcCommand_opcode_t) vcCommand_GetValue(gOpCodeStrVal, COUNT_OF(gOpCodeStrVal), codeStr, CEC_OPCODE_UNKNOWN));
}

