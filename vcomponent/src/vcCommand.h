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

#ifndef __VCCOMMAND_H
#define __VCCOMMAND_H

#include "stdint.h"

#define VCCOMMAND_MAX_DATA_SIZE 64

#define CEC_MSG_PREFIX "hdmicec"

#define CEC_MSG_EVENT "event"
#define CEC_MSG_COMMAND "command"
#define CEC_MSG_CONFIG "config"

#define CEC_CMD_INITIATOR "initiator"
#define CEC_CMD_DESTINATION "destination"

#define CEC_BROADCAST "broadcast"

/*Custom Commands*/
#define CMD_HOTPLUG "HotPlug"

/*CEC Commands*/
#define CMD_IMAGE_VIEW_ON "ImageViewOn"
#define CMD_TEXT_VIEW_ON "TextViewOn"
#define CMD_STANDBY "StandBy"
#define CMD_ACTIVE_SOURCE "ActiveSource"
#define CMD_REQUEST_ACTIVE_SOURCE "ActiveSource"
#define CMD_INACTIVE_SOURCE "InactiveSource"
#define CMD_SET_OSD_NAME "SetOSDName"
#define CMD_DATA_OSD_NAME "osd_name"

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

typedef enum
{
  CEC_OPCODE_UNKNOWN = 0,
  CEC_IMAGE_VIEW_ON = 0x04,
  CEC_TEXT_VIEW_ON = 0x0D,
  CEC_STANDBY = 0x36,
  CEC_SET_OSD_NAME = 0x49,
  CEC_ACTIVE_SOURCE = 0x82,
  CEC_REQUEST_ACTIVE_SOURCE = 0x85,
  CEC_INACTIVE_SOURCE = 0x9D,
} vcCommand_opcode_t;

typedef enum
{
  CEC_POWER_STATUS_ON                          = 0x00,
  CEC_POWER_STATUS_STANDBY                     = 0x01,
  CEC_POWER_STATUS_IN_TRANSITION_STANDBY_TO_ON = 0x02,
  CEC_POWER_STATUS_IN_TRANSITION_ON_TO_STANDBY = 0x03,
  CEC_POWER_STATUS_UNKNOWN                     = 0x99
} vcCommand_power_status_t;

typedef enum
{
  DEVICE_TYPE_TV = 0,
  DEVICE_TYPE_RECORDER,
  DEVICE_TYPE_RESERVED,
  DEVICE_TYPE_TUNER,
  DEVICE_TYPE_PLAYBACK,
  DEVICE_TYPE_AUDIO_SYSTEM,
  DEVICE_TYPE_FREEUSE,
  DEVICE_TYPE_UNREGISTERED,
  DEVICE_TYPE_UNKNOWN
} vcCommand_device_type_t;

typedef enum
{
  LOGICAL_ADDRESS_UNKNOWN          = -1,
  LOGICAL_ADDRESS_TV               = 0,
  LOGICAL_ADDRESS_RECORDINGDEVICE1 = 1,
  LOGICAL_ADDRESS_RECORDINGDEVICE2 = 2,
  LOGICAL_ADDRESS_TUNER1           = 3,
  LOGICAL_ADDRESS_PLAYBACKDEVICE1  = 4,
  LOGICAL_ADDRESS_AUDIOSYSTEM      = 5,
  LOGICAL_ADDRESS_TUNER2           = 6,
  LOGICAL_ADDRESS_TUNER3           = 7,
  LOGICAL_ADDRESS_PLAYBACKDEVICE2  = 8,
  LOGICAL_ADDRESS_RECORDINGDEVICE3 = 9,
  LOGICAL_ADDRESS_TUNER4           = 10,
  LOGICAL_ADDRESS_PLAYBACKDEVICE3  = 11,
  LOGICAL_ADDRESS_RESERVED1        = 12,
  LOGICAL_ADDRESS_RESERVED2        = 13,
  LOGICAL_ADDRESS_FREEUSE          = 14,
  LOGICAL_ADDRESS_UNREGISTERED     = 15,
  LOGICAL_ADDRESS_BROADCAST        = 15,
} vcCommand_logical_address_t;

typedef enum
{
  CEC_VERSION_UNKNOWN = 0,
  CEC_VERSION_1_2 = 1,
  CEC_VERSION_1_2A = 2,
  CEC_VERSION_1_3 = 3,
  CEC_VERSION_1_3A = 4,
  CEC_VERSION_1_4 = 5,
  CEC_VERSION_2_0 = 6
} vcCommand_version_t;

typedef enum
{
  VENDOR_CODE_TOSHIBA = 0x000039,
  VENDOR_CODE_SAMSUNG = 0x0000F0,
  VENDOR_CODE_DENON = 0x0005CD,
  VENDOR_CODE_MARANTZ = 0x000678,
  VENDOR_CODE_LOEWE = 0x000982,
  VENDOR_CODE_ONKYO = 0x0009B0,
  VENDOR_CODE_MEDION = 0x000CB8,
  VENDOR_CODE_TOSHIBA2 = 0x000CE7,
  VENDOR_CODE_APPLE = 0x0010FA,
  VENDOR_CODE_HARMAN_KARDON2 = 0x001950,
  VENDOR_CODE_GOOGLE = 0x001A11,
  VENDOR_CODE_AKAI = 0x0020C7,
  VENDOR_CODE_AOC = 0x002467,
  VENDOR_CODE_PANASONIC = 0x008045,
  VENDOR_CODE_PHILIPS = 0x00903E,
  VENDOR_CODE_DAEWOO = 0x009053,
  VENDOR_CODE_YAMAHA = 0x00A0DE,
  VENDOR_CODE_GRUNDIG = 0x00D0D5,
  VENDOR_CODE_PIONEER = 0x00E036,
  VENDOR_CODE_LG = 0x00E091,
  VENDOR_CODE_SHARP = 0x08001F,
  VENDOR_CODE_SONY = 0x080046,
  VENDOR_CODE_BROADCOM = 0x18C086,
  VENDOR_CODE_SHARP2 = 0x534850,
  VENDOR_CODE_VIZIO = 0x6B746D,
  VENDOR_CODE_BENQ = 0x8065E9,
  VENDOR_CODE_HARMAN_KARDON = 0x9C645E,
  VENDOR_CODE_UNKNOWN = 0
} vcCommand_vendor_code_t;


typedef struct
{
  vcCommand_logical_address_t initiator;
  vcCommand_logical_address_t destination;
  vcCommand_opcode_t opcode;
  bool opcode_set;
  uint8_t parameter_data[VCCOMMAND_MAX_DATA_SIZE]; 
  uint32_t parameter_size;
}vcCommand_t;

typedef struct
{
  char* str;
  int val;
} vcCommand_strVal_t;

/**
 * @brief Clears the specified HDMI CEC command.
 *
 * Resets the command structure to its initial state.
 *
 * @param cmd Pointer to the vcCommand_t structure to be cleared.
 */
void vcCommand_Clear(vcCommand_t* cmd);

/**
 * @brief Formats an HDMI CEC command.
 *
 * Sets the initiator, destination, and opcode of the specified command.
 *
 * @param cmd Pointer to the vcCommand_command_t structure to be formatted.
 * @param initiator The logical address of the initiator.
 * @param destination The logical address of the destination.
 * @param opcode The opcode of the command.
 */
void vcCommand_Format(vcCommand_t* cmd,
                                vcCommand_logical_address_t initiator,
                                vcCommand_logical_address_t destination,
                                vcCommand_opcode_t opcode);

/**
 * @brief Appends a byte of data to the HDMI CEC command.
 *
 * Adds the specified byte to the end of the command's data payload.
 *
 * @param cmd Pointer to the vcCommand_t structure.
 * @param data The byte of data to be appended.
 */
void vcCommand_PushBackByte(vcCommand_t* cmd, uint8_t data);

/**
 * @brief Appends an array of data to the HDMI CEC command.
 *
 * Adds the specified array of bytes to the end of the command's data payload.
 *
 * @param cmd Pointer to the vcCommand_t structure.
 * @param data Pointer to the array of data to be appended.
 * @param len The length of the array.
 */
void vcCommand_PushBackArray(vcCommand_t* cmd, uint8_t* data, uint32_t len);

/**
 * @brief Retrieves the raw bytes of the HDMI CEC command.
 *
 * Copies the command's data payload into the provided buffer.
 *
 * @param cmd Pointer to the vcCommand_t structure.
 * @param data Pointer to the buffer where the data will be copied.
 * @param len The maximum number of bytes to copy.
 * @return The number of bytes copied.
 */
uint32_t vcCommand_GetRawBytes(vcCommand_t* cmd, uint8_t* data, uint32_t len);

/**
 * @brief Converts a string representation of an opcode to its corresponding value.
 *
 * @param codeStr The string representation of the opcode.
 * @return The corresponding vcCommand_opcode_t value.
 */
vcCommand_opcode_t vcCommand_GetOpCode(char* codeStr);

/**
 * @brief Gets the value (int - enum) associated with the string from the provided array of strVal_t.
 *
 * If no match is found in the array, default_val is returned.
 *
 * @param map Pointer to the array of strVal_t.
 * @param length Length of the array.
 * @param str The string to be matched.
 * @param default_val The default value to be returned if no match is found.
 * @return The value associated with the string, or default_val if no match is found.
 */
int vcCommand_GetValue(const vcCommand_strVal_t *map, int length, char* str, int default_val);

/**
 * @brief Gets the string associated with the value (int - enum) from the provided array of strVal_t.
 *
 * If no match is found for the value, NULL is returned.
 *
 * @param map Pointer to the array of strVal_t.
 * @param length Length of the array.
 * @param val The value to be matched.
 * @return The string associated with the value, or NULL if no match is found.
 */
char* vcCommand_GetString(const vcCommand_strVal_t *map, int length, int val);



#endif //__vcCommand_COMMAND_H
