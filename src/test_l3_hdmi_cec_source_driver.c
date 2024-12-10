/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:*
* Copyright 2024 RDK Management
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
 * @defgroup HDMI_CEC_HALTESTS_L3 HDMI CEC HAL Tests L2 File
 * @{
 * @parblock
 *
 * ### L2 Test Cases for HDMI CEC HAL :
 *
 *
 * ## Module's Role
 * This module includes Level 3 functional test interfaces.
 * This Test Interfaces provides a scope to create a User Test cases for HDMI CEC Source modules that can be either Manual or automated scripts.
 *
 * **Pre-Conditions:**  None@n
 * **Dependencies:** None@n
 *
 * Refer to API Definition specification documentation : [hdmi-cec_halSpec.md](../../docs/pages/hdmi-cec_halSpec.md)
 *
 * @endparblock
 */

/**
 * @file test_l3_hdmi_cec_source_driver.c
 *
 */

#include <unistd.h>
#include <ut.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>
#include <ut_control_plane.h>
#include "hdmi_cec_driver.h"

#define TIMEOUT 5
#define REPLY_TIMEOUT 5

#define HDMI_CEC_MAX_PAYLOAD 128
#define HDMI_CEC_MAX_OSDNAME 15

#define UT_LOG_MENU_INFO UT_LOG_INFO

// CEC command map table and supporting function
typedef struct
{
    uint8_t cecCommand;  // CEC command code
    const char* commandName;   // Human-readable command name
    int32_t dataLength;            // Number of data bytes required for the command
} CecCommandMap;

CecCommandMap cecCommandTable[] =
{
    {0x00, "Feature Abort", 2},
    {0x04, "Image View On", 0},
    {0x0D, "Text View On", 0},
    {0x20, "Active Source", 2},
    {0x32, "Inactive Source", 2},
    {0x36, "Request Active Source", 0},
    {0x41, "Standby", 0},
    {0x44, "User Control Pressed", 1},
    {0x45, "User Control Released", 0},
    {0x46, "Give OSD Name", 0},
    {0x47, "Set OSD Name", 15},
    {0x82, "Routing Change", 4},
    {0x83, "Routing Information", 2},
    {0x86, "Report Physical Address", 3},
    {0x87, "Request Active Source", 0},
    {0x8C, "Give Device Vendor ID", 0},
    {0x89, "Device Vendor ID", 3},
    {0x90, "Vendor Command", 16},
    {0x91, "Vendor Command with ID", 16},
    {0x92, "Give Device Power Status", 0},
    {0x93, "Report Power Status", 1},
    {0x9E, "CEC Version", 1},
    {0x9F, "Get CEC Version", 0},
    {0xA0, "Get Menu Language", 0},
    {0xA1, "Set Menu Language", 3},
    {0xA5, "Report Physical Address", 3},
    {0xA6, "Request Short Audio Descriptor", 1},
    {0xC0, "Report Audio Status", 1},
    {0xC1, "Give Audio Status", 0},
    {0xC2, "Set System Audio Mode", 1},
    {0xC3, "Report Audio Descriptor", 1},
    {0xC4, "Set Audio Rate", 1},
    // Add more commands as needed
};

typedef enum HDMI_CEC_DEVICE_TYPE_T
{
    HDMI_CEC_RECORDER = 1,
    HDMI_CEC_AMPLIFIER,
    HDMI_CEC_TUNER,
    HDMI_CEC_PLAYBACK,
    HDMI_CEC_AUDIO,
    HDMI_CEC_TV,
    HDMI_CEC_RESERVED
} HDMI_CEC_DEVICE_TYPE;

const static ut_control_keyStringMapping_t cecDeviceType_mapTable[] =
{
  {"Recorder",        (int32_t)HDMI_CEC_RECORDER},
  {"Amplifier",       (int32_t)HDMI_CEC_AMPLIFIER},
  {"Tuner",           (int32_t)HDMI_CEC_TUNER},
  {"Playback Device", (int32_t)HDMI_CEC_PLAYBACK},
  {"Audio System",    (int32_t)HDMI_CEC_AUDIO},
  {"TV",              (int32_t)HDMI_CEC_TV},
  {"Reserved",        (int32_t)HDMI_CEC_RESERVED}
};

typedef enum HDMI_CEC_POWER_STATUS_T
{
    HDMI_CEC_ON = 0,
    HDMI_CEC_STANDBY,
    HDMI_CEC_TRANSIT_STANDBY,
    HDMI_CEC_TRANSIT_ON
} HDMI_CEC_POWER_STATUS;

const static ut_control_keyStringMapping_t cecPowerStatus_mapTable[] =
{
  {"On",                    (int32_t)HDMI_CEC_ON},
  {"Standby",               (int32_t)HDMI_CEC_STANDBY},
  {"Transition to Standby", (int32_t)HDMI_CEC_TRANSIT_STANDBY},
  {"Transition to On",      (int32_t)HDMI_CEC_TRANSIT_ON}
};

typedef enum HDMI_CEC_FEATURE_ABORT_REASON_T
{
    HDMI_CEC_UNRECOGNIZE = 0,
    HDMI_CEC_NOTCORRECTMODE,
    HDMI_CEC_CANNOTPROVIDE,
    HDMI_CEC_INVALID,
    HDMI_CEC_REFUSED,
    HDMI_CEC_UNABLE
} HDMI_CEC_FEATURE_ABORT_REASON;

const static ut_control_keyStringMapping_t cecFeatureAbortReason_mapTable[] =
{
  {"Unrecognized opcode",            (int32_t)HDMI_CEC_UNRECOGNIZE},
  {"Not in Correct mode to respond", (int32_t)HDMI_CEC_NOTCORRECTMODE},
  {"Cannot provide source",          (int32_t)HDMI_CEC_CANNOTPROVIDE},
  {"Invalid operand",                (int32_t)HDMI_CEC_INVALID},
  {"Refused",                        (int32_t)HDMI_CEC_REFUSED},
  {"Unable to determine",            (int32_t)HDMI_CEC_UNABLE}
};
/* cecError_t */
const static ut_control_keyStringMapping_t cecError_mapTable [] =
{
  {"HDMI_CEC_IO_SUCCESS",           (int32_t)HDMI_CEC_IO_SUCCESS},
  {"HDMI_CEC_IO_SENT_AND_ACKD",     (int32_t)HDMI_CEC_IO_SENT_AND_ACKD},
  {"HDMI_CEC_IO_SENT_BUT_NOT_ACKD", (int32_t)HDMI_CEC_IO_SENT_BUT_NOT_ACKD},
  {"HDMI_CEC_IO_SENT_FAILED",       (int32_t)HDMI_CEC_IO_SENT_FAILED},
  {"HDMI_CEC_IO_NOT_OPENED",        (int32_t)HDMI_CEC_IO_NOT_OPENED},
  {"HDMI_CEC_IO_INVALID_ARGUMENT",  (int32_t)HDMI_CEC_IO_INVALID_ARGUMENT},
  {"HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE", (int32_t)HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE},
  {"HDMI_CEC_IO_GENERAL_ERROR",     (int32_t)HDMI_CEC_IO_GENERAL_ERROR},
  {"HDMI_CEC_IO_ALREADY_OPEN",      (int32_t)HDMI_CEC_IO_ALREADY_OPEN},
  {"HDMI_CEC_IO_ALREADY_REMOVED",   (int32_t)HDMI_CEC_IO_ALREADY_REMOVED},
  {"HDMI_CEC_IO_INVALID_OUTPUT",    (int32_t)HDMI_CEC_IO_INVALID_OUTPUT},
  {"HDMI_CEC_IO_INVALID_HANDLE",    (int32_t)HDMI_CEC_IO_INVALID_HANDLE},
  {"HDMI_CEC_IO_OPERATION_NOT_SUPPORTED", (int32_t)HDMI_CEC_IO_OPERATION_NOT_SUPPORTED},
  {"HDMI_CEC_IO_NOT_ADDED",         (int32_t)HDMI_CEC_IO_NOT_ADDED},
  {"HDMI_CEC_IO_MAX",               (int32_t)HDMI_CEC_IO_MAX},
  {  NULL, -1 }
};

static int32_t gTestGroup = 3;
static int32_t gTestID = 1;
static int32_t gHandle = 0;
static int32_t gLogicalAddress = -1;
static uint32_t gPhysicalAddress = -1;
static uint8_t  *gPhysicalAddressBytes;
static HDMI_CEC_DEVICE_TYPE gDeviceType = HDMI_CEC_TV;
static HDMI_CEC_POWER_STATUS gPowerStatus = HDMI_CEC_ON;
static uint8_t gDeviceVendorID[] = {0x00, 0x00, 0x01};  // Example Vendor ID: 0x000001
static uint8_t gCECVersion = 0x05; // CEC Version: 1.4 (0x05)
static uint8_t gAudioDelay = (100/2) + 1; //100msec audio delay ((number of milliseconds/2) + 1)
static uint8_t gVideoDelay = (200/2) + 1; //100msec video delay ((number of milliseconds/2) + 1)
static uint8_t gBroadcastAddress = 0xF;

//(Bits 1-0) : Audio Ouput Compensated (0 - N/A, 1 - TV audio output is delay compensated, 2 - TV audio output is NOT delay compensated, 3 - TV audio output is Par delay compensated)
//(Bit 2) : 0 - normal latency, 1 - low latency
//Bit(7-3) : Reserved
uint8_t gLatencyFlag = 0x00;


/**
* @brief CEC Command with data size mapping function.
*
* This function maps the provided CEC command with the data length required for the CEC Command.
* This will help the test to consider the correct size of the data required for the CEC Command.
*
*/
static int32_t getCecCommandInfo(uint8_t cecCommand, const char** commandName, int32_t* dataLength)
{
    UT_LOG_INFO("In %s(IN: cecCommand: [0x%02X])\n", __FUNCTION__, cecCommand);

    int32_t tableSize = sizeof(cecCommandTable) / sizeof(CecCommandMap);

    for (int32_t i = 0; i < tableSize; i++)
    {
        if (cecCommandTable[i].cecCommand == cecCommand)
        {
            *commandName = cecCommandTable[i].commandName;
            *dataLength = cecCommandTable[i].dataLength;
            UT_LOG_INFO("Out %s(OUT: commandName: [%s], OUT: dataLength: [%d])\n", __FUNCTION__, *commandName, *dataLength);
            return 0; // Command found
        }
    }

    UT_LOG_INFO("Out %s(OUT: Command not found)\n", __FUNCTION__);
    return -1; // Command not found
}

static void handleImageViewOn()
{
    UT_LOG_INFO("Image View On command received.\n");
    // Perform any device-specific action for "Image View On" if needed.
    UT_LOG_INFO("Image View On processed.\n");
}

static void handleActiveSource(uint8_t *buf, int32_t len)
{
    if (len >= 4)
    {
        uint16_t physicalAddress = (buf[2] << 8) | buf[3]; // Combine bytes to form the physical address
        UT_LOG_INFO("Active Source command received. Physical Address: [0x%04X]\n", physicalAddress);
        // Process Active Source as needed.
    }
    else
    {
        UT_LOG_ERROR("Active Source command received with insufficient data.\n");
    }
}

static void handleGivePhysicalAddress(int32_t handle, uint8_t initiator, uint8_t destination, uint8_t *pPhysicalAddress, HDMI_CEC_DEVICE_TYPE deviceType)
{
    uint8_t response[] = { (destination << 4) | initiator, 0x84, (pPhysicalAddress[3] << 4) | pPhysicalAddress[2], (pPhysicalAddress[1] << 4 ) | pPhysicalAddress[0], deviceType};
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_INFO("Reported Physical Address response sent with result: %d\n", result);
}

static void handleReportPhysicalAddress(uint8_t *buf, int32_t len)
{
    if (len >= 5)
        UT_LOG_INFO("Reported Physical Address: 0x%02X:0x%02X:0x%02X:0x%02X, Type: %s\n", (buf[2] >> 4) & 0x0F, buf[2] & 0x0F, (buf[3] >> 4) & 0x0F, buf[3] & 0x0F, UT_Control_GetMapString(cecDeviceType_mapTable, buf[4]));
    else
        UT_LOG_ERROR("Reported Physical command received with insufficient data.\n");
}

static void handleGetCECVersion(int32_t handle, uint8_t initiator, uint8_t destination, uint8_t version)
{
    uint8_t response[] = { (destination << 4) | initiator, 0x9E, version };
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_INFO("CEC Version response sent with result: %d\n", result);
}

static void handleCECVersion(uint8_t *buf, int32_t len)
{
    if (len >= 3)
        UT_LOG_INFO("Received CEC Version: %x\n", buf[2]);
    else
        UT_LOG_ERROR("CEC Version command received with insufficient data.\n");
}

static void handleGiveDeviceVendorID(int32_t handle, uint8_t initiator, uint8_t destination, uint8_t *pDeviceVendorId)
{
    uint8_t response[] = { (destination << 4) | initiator, 0x87, pDeviceVendorId[0], pDeviceVendorId[1], pDeviceVendorId[2]};
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_INFO("Device Vendor ID response sent with result: %d\n", result);
}

static void handleDeviceVendorID(uint8_t *buf, int32_t len)
{
    if (len >= 5)
        UT_LOG_INFO("Received Device Vendor ID: %x%x%x\n", buf[2], buf[3], buf[4]);
    else
        UT_LOG_ERROR("Give Device vendor ID command received with insufficient data.\n");
}

static void handleStandby()
{
    UT_LOG_INFO("Standby command received. Initiating standby actions.\n");
    // Implement device-specific standby actions here, such as turning off the display.
}

static void handleGiveDeviceInfo(int32_t handle, uint8_t initiator, uint8_t destination)
{
    uint8_t response[] = { (destination << 4) | initiator, 0xA1, 'V', 'T', 'S', ' ', 'D', 'e', 'v', 'i', 'c', 'e' }; // Device Info: "VTS Device"
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_INFO("Device Info response sent with result: %d\n", result);
}

static void handleOsdDisplay(uint8_t *buf, int32_t len)
{
    if (len > 3)
    {
        uint8_t buffer[HDMI_CEC_MAX_OSDNAME] = {0};
        uint8_t *temp = buffer;
        UT_LOG_INFO("OSD Display Control: %d", buf[2]);
        for (int32_t i = 3; i < len; i++)
        {
            int32_t len = 0;
            len = snprintf((char*)temp, HDMI_CEC_MAX_OSDNAME, "%c", buf[i]);
            temp += len;
        }
        UT_LOG_INFO("OSD Display message received: %s", buffer);
    }
    else
    {
        UT_LOG_ERROR("OSD Display message received with insufficient data.\n");
    }
}

static void handleGiveOSDName(int32_t handle, uint8_t initiator, uint8_t destination)
{
    uint8_t response[] = { (destination << 4) | initiator, 0x47, 'V', 'T', 'S', ' ', 'D', 'e', 'v', 'i', 'c', 'e' }; // OSD Name: "VTS Device"
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_INFO("OSD Name response sent with result: %d sourc/destination:0x%2x\n", result,response[0]);
}

static void handleSetOSDName(uint8_t *buf, int32_t len)
{
    if (len > 2)
    {
        uint8_t buffer[HDMI_CEC_MAX_OSDNAME] = {0};
        uint8_t *temp = buffer;
        for (int32_t i = 2; i < len; i++)
        {
            int32_t len = 0;
            len = snprintf((char*)temp, HDMI_CEC_MAX_OSDNAME, "%c", buf[i]);
            temp += len;
        }
        UT_LOG_INFO("OSD Name received: %s", buffer);
    }
    else
    {
        UT_LOG_ERROR("OSD Name message received with insufficient data.\n");
    }
}

static void handleGivePowerStatus(int32_t handle, uint8_t initiator, uint8_t destination, HDMI_CEC_POWER_STATUS powerStatus)
{
    uint8_t response[] = { (destination << 4) | initiator, 0x90, powerStatus };
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_INFO("Power Status response sent with result: %d\n", result);
}

static void handleReportPowerStatus(uint8_t *buf, int32_t len)
{
    if (len >= 3)
    {
        UT_LOG_INFO("Power Status: %s\n", UT_Control_GetMapString(cecPowerStatus_mapTable, buf[2]));
    }
    else
    {
        UT_LOG_ERROR("Power Status received with insufficient data.\n");
    }
}

static void handleFeatureAbort(uint8_t *buf, int32_t len)
{
    if (len >= 4)
    {
        UT_LOG_INFO("Feature Abort: Opcode: 0x%02X, Reason: %s\n", buf[2], UT_Control_GetMapString(cecFeatureAbortReason_mapTable, buf[3]));
    }
    else
    {
        UT_LOG_ERROR("Feature Abort received with insufficient data.\n");
    }
}

static void sendFeatureAbort(int32_t handle, uint8_t initiator, uint8_t destination, uint8_t opcode, uint8_t reason)
{
    uint8_t response[] = { (destination << 4) | initiator, 0x00, opcode, reason }; // Abort with reason
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_WARNING("Feature Abort sent for opcode: 0x%02X with reason: 0x%02X, result: %d\n", opcode, reason, result);
}

static void handleSetMenuLanguage(uint8_t *buf, int32_t len)
{
    if (len >= 5)
    {
        UT_LOG_INFO("OSD Menu Language received: %c%c%c", buf[2], buf[2], buf[3]);
    }
    else
    {
        UT_LOG_ERROR("OSD Menu Language received with insufficient data");
    }
}

static void handleCurrentLatency(int32_t handle, uint8_t *buf, int32_t len, uint8_t *pPhysicalAddress, uint8_t videoDelay, uint8_t audioDelay, uint8_t latency)
{
    uint8_t response[] = {((gLogicalAddress << 4) | gBroadcastAddress), 0xA8, buf[2], buf[3], videoDelay, latency, audioDelay};

    int32_t result;

    if (len >= 4)
    {
        if ((buf[2] == (pPhysicalAddress[3] << 4) | pPhysicalAddress[2]) &&
            (buf[3] == (pPhysicalAddress[1] << 4) | pPhysicalAddress[0]))
        {
            response[4] = videoDelay;
            response[5] = latency;
            response[6] = audioDelay;

            HdmiCecTx(handle, response, sizeof(response), &result);
        }

    }
    else
    {
        UT_LOG_ERROR("Latency query received with insufficient data.\n");
    }
}

static void handleRequestActiveSource(int32_t handle, uint8_t *pPhysicalAddress)
{
    uint8_t response[] = { ((gLogicalAddress << 4) | gBroadcastAddress), 0x82, ((pPhysicalAddress[3] << 4) | pPhysicalAddress[2]), ((pPhysicalAddress[1] << 4 ) | pPhysicalAddress[0])};
    int32_t result;
    HdmiCecTx(handle, response, sizeof(response), &result);
    UT_LOG_INFO("Requested Active Sources response sent with result: %d\n", result);
}

/**
 * @brief This function clears the stdin buffer.
 *
 * This function clears the stdin buffer.
 */
static void readAndDiscardRestOfLine(FILE *in)
{
    int32_t c;
    while ((c = fgetc(in)) != EOF && c != '\n');
}

static void readInt(int32_t *value)
{
    scanf("%d", value);
    readAndDiscardRestOfLine(stdin);
}

static void readHex(int32_t *value)
{
    scanf("%x", value);
    readAndDiscardRestOfLine(stdin);
}

static void onRxDataReceived(int32_t handle, void *callbackData, uint8_t *buf, int32_t len)
{
    UT_LOG_INFO("In %s(IN: handle: [%p], IN: callbackData: [%p], IN: buf: [%p], IN: len: [%d])\n", __FUNCTION__, handle, callbackData, buf, len);

    if ((handle != 0) && (callbackData != NULL) && (len > 0))
    {
        // Parse the command
        uint8_t initiator = (buf[0] >> 4) & 0xF;  // Extract initiator address
        uint8_t destination = buf[0] & 0xF;       // Extract destination address
        uint8_t opcode;                           // Command opcode
        if( len == 1)
        {
            UT_LOG_INFO("Received Ping message Initiator: [0x%02X], Destination: [0x%02X]", initiator, destination);
            UT_LOG_INFO("Out %s\n", __FUNCTION__);
            return;
        }

        opcode = buf[1];

        UT_LOG_INFO("Initiator: [0x%02X], Destination: [0x%02X], Opcode: [0x%02X]", initiator, destination, opcode);
        if (len > 2)
        {
            uint8_t buffer[HDMI_CEC_MAX_PAYLOAD] = {0};
            uint8_t *temp = buffer;
            // Log each byte received in the buffer
            for (int32_t index = 2; index < len; index++)
            {
                int32_t len = 0;
                len = snprintf((char*)temp, HDMI_CEC_MAX_PAYLOAD, "0x%02X, ", buf[index]);
                temp += len;
            }
            buffer[strlen(buffer)-2] = '\0';
            UT_LOG_INFO("Payload: [%s]", buffer);
        }

        // Handle each opcode with its corresponding function
        switch (opcode)
        {
            // CEC Commands no response sent
            case 0x00: handleFeatureAbort(buf, len); break;
            case 0x04: handleImageViewOn(); break;
            case 0x32: handleSetMenuLanguage(buf, len); break;
            case 0x82: handleActiveSource(buf, len); break;
            case 0x84: handleReportPhysicalAddress(buf, len); break;
            case 0x87: handleDeviceVendorID(buf, len); break;
            case 0x36: handleStandby(); break;
            case 0x64: handleOsdDisplay(buf, len); break;
            case 0x90: handleReportPowerStatus(buf, len); break;
            case 0x9E: handleCECVersion(buf, len); break;
            case 0x47: handleSetOSDName(buf, len); break;

            // CEC commands with response
            case 0x83: handleGivePhysicalAddress(handle, initiator, destination, gPhysicalAddressBytes, gDeviceType); break;
            case 0x8F: handleGivePowerStatus(handle, initiator, destination, gPowerStatus); break;
            case 0x8C: handleGiveDeviceVendorID(handle, initiator, destination, gDeviceVendorID); break;
            case 0x9F: handleGetCECVersion(handle, initiator, destination, gCECVersion); break;
            case 0x46: handleGiveOSDName(handle, initiator, destination); break;

            // Broadcasting CEC commands with response
            case 0x85: handleRequestActiveSource(handle, gPhysicalAddressBytes); break;
            case 0xA7: handleCurrentLatency(handle, buf, len, gPhysicalAddressBytes, gVideoDelay, gAudioDelay, gLatencyFlag); break;
            default:
                UT_LOG_WARNING("Unhandled opcode: [0x%02X]\n", opcode);
                sendFeatureAbort(handle, initiator, destination, opcode, 0x04); // Feature Abort: Unrecognized opcode
                break;
        }

        // Clear the buffer after processing
        memset(buf, 0, len);
    }
    else
    {
        // Log specific errors based on failed conditions
        if (handle == 0)
        {
            UT_LOG_ERROR("Error: Invalid handle.\n");
        }
        if (callbackData == NULL)
        {
            UT_LOG_ERROR("Error: Null callback data.\n");
        }
        if (len <= 0)
        {
            UT_LOG_ERROR("Error: Invalid length.\n");
        }
    }

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}



/**
* @brief Initialization of the HAL CEC Module
*
* This test provides a scope to open the HAL CEC module and preserve the handle.
*
* **Test Group ID:** 03@n
*
* **Test Case ID:** 001@n
*
* **Pre-Conditions:** None@n
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 1 to before running any test.
*
*/

void test_l3_hdmi_cec_source_hal_Init(void)
{
   gTestID = 1;
    HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;

    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    // Step 1: Call HdmiCecOpen()
    UT_LOG_INFO("Calling HdmiCecOpen(OUT:handle:[])");
    status = HdmiCecOpen(&gHandle);
    UT_LOG_INFO("Result HdmiCecOpen(OUT:handle:[0x%0X]) HDMI_CEC_STATUS:[%s]",gHandle, UT_Control_GetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);
    assert(gHandle != 0);

    // Step 2: Register the call back
    UT_LOG_INFO("Calling HdmiCecSetRxCallback(IN:handle:[0x%0X], IN:cbfunc:[0x%0X])",gHandle, onRxDataReceived);
    status = HdmiCecSetRxCallback(gHandle, onRxDataReceived,(void*)0xABABABAB);
    UT_LOG_INFO("Result HdmiCecSetRxCallback(IN:handle:[0x%0X], IN:cbfunc:[0x%0X]) HDMI_CEC_STATUS:[%s]",gHandle,onRxDataReceived, UT_Control_GetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Calling HdmiCecGetPhysicalAddress(IN:handle:[0x%0X], OUT:physicalAddress:[])", gHandle);
    status = HdmiCecGetPhysicalAddress(gHandle, &gPhysicalAddress);
    UT_LOG_INFO("Result HdmiCecGetPhysicalAddress(IN:handle:[0x%0X], OUT:physicalAddress:[%d]) HDMI_CEC_STATUS:[%s]", gHandle, gPhysicalAddress, UT_Control_GetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);

    gPhysicalAddressBytes = (uint8_t*)&gPhysicalAddress;

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}


/**
* @brief Test to get the logical address
*
* This test provides a scope to check the assigned logical address of the device.
*
* **Test Group ID:** 03@n
*
* **Test Case ID:** 002@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be initalized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 3 and shall read the Logical address displayed on the console.
*
*/
void test_l3_hdmi_cec_source_hal_GetLogicalAddress(void)
{
    gTestID = 2;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;
    int32_t logicalAddress = -1;

    UT_LOG_INFO("Calling HdmiCecGetLogicalAddress(IN:handle:[0x%0X], OUT:logicalAddress:[])", gHandle);
    status = HdmiCecGetLogicalAddress(gHandle, &logicalAddress);
    UT_LOG_INFO("Result HdmiCecGetLogicalAddress(IN:handle:[0x%0X], OUT:logicalAddress:[%d]) HDMI_CEC_STATUS:[%s])", gHandle, logicalAddress, UT_Control_GetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief  Test provides a scope to Transmit the CEC Command
*
* This test provides an interface to user/automation tool to transmit a CEC Command.
* Necessary input should be provided to the test.
*
* **Test Group ID:** 03@n
*
* **Test Case ID:** 003@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be initalized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 4 and shall provide the necessary source and destination logical address, 
* CEC command, data lenght and data.
*
*/
void test_l3_hdmi_cec_source_hal_TransmitHdmiCecCommand(void) {
    gTestID = 3;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    int32_t sourceLogicalAddress = gLogicalAddress;
    int32_t destinationLogicalAddress = -1;
    uint8_t buf[16] = {0};
    int32_t len = 0, cecCommand = 0;
    int32_t result;
    const char* commandName;
    int32_t expectedDataLength;

    assert(sourceLogicalAddress != -1);

    // Reading inputs from the user or test framework
    UT_LOG_MENU_INFO("Enter a valid Destination Logical Address: ");
    readInt(&destinationLogicalAddress);

    UT_LOG_MENU_INFO("Enter CEC Command (in hex): ");
    readHex(&cecCommand);

    // Validate the CEC command and get the expected data length
    if (getCecCommandInfo(cecCommand, &commandName, &expectedDataLength) != 0)
    {
        // Command not found in the map, prompt the user for additional information
        UT_LOG_WARNING("CEC command 0x%02X is not recognized. It might be a vendor-specific command.", cecCommand);

        UT_LOG_MENU_INFO("Enter the number of data bytes for the CEC command:");
        readInt(&expectedDataLength);
        commandName = "Vendor Specific Command";
    }
    else
    {
        UT_LOG_INFO("CEC Command: %s (0x%02X), expects %d data byte(s)", commandName, cecCommand, expectedDataLength);
    }

    // If the command has data bytes, prompt for them
    if (expectedDataLength > 0)
    {
        for (int32_t i = 0; i < expectedDataLength; i++)
        {
            int32_t byte_data;
            UT_LOG_MENU_INFO("Enter Databyte[%d] (in hex):", i);
            readHex(&byte_data);
            buf[i + 2] = (uint8_t)byte_data; // +2 to account for the first two bytes
        }
    }

    // Building the CEC message
    buf[0] = (uint8_t)((sourceLogicalAddress << 4) | (destinationLogicalAddress & 0x0F));
    buf[1] = (uint8_t)cecCommand;
    len = expectedDataLength + 2; // 1 byte for logical addresses, 1 byte for CEC Command

    // Logging the transmission attempt
    UT_LOG_INFO("Calling HdmiCecTx(IN:handle:[0x%0X], IN:buf:[%p], IN:len:[%d], OUT:result:[])", gHandle, buf, len);
    int32_t status = HdmiCecTx(gHandle, buf, len, &result);
    UT_LOG_INFO("Result HdmiCecTx(IN:handle:[0x%0X], IN:buf:[%p], IN:len:[%d], OUT:result:[%d]) HDMI_CEC_STATUS:[%s]", gHandle, buf, len, result, UT_Control_GetMapString(cecError_mapTable,status));

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}

/**
* @brief Test to get the physical address
*
* This test provides a scope to read the physical address of the device.
*
* **Test Group ID:** 03@n
*
* **Test Case ID:** 004@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be initalized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 6 to read the physical address of the device 
* device connected in the network.
*
*/
void test_l3_hdmi_cec_source_hal_GetPhysicalAddress(void)
{
    gTestID = 4;
    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    HDMI_CEC_STATUS status = HDMI_CEC_IO_SUCCESS;
    uint32_t physicalAddress = -1;

    UT_LOG_INFO("Calling HdmiCecGetPhysicalAddress(IN:handle:[0x%0X], OUT:physicalAddress:[])", gHandle);
    status = HdmiCecGetPhysicalAddress(gHandle, &physicalAddress);
    UT_LOG_INFO("Result HdmiCecGetPhysicalAddress(IN:handle:[0x%0X], OUT:physicalAddress:[%d]) HDMI_CEC_STATUS:[%s]", gHandle, physicalAddress, UT_Control_GetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);

    UT_LOG_INFO("Out %s\n", __FUNCTION__);

/**
* @brief Test to close the HDMI CEC device.
*
* This test provides a scope to close the created HDMI CEC handle.
*
* **Test Group ID:** 03@n
*
* **Test Case ID:** 05@n
*
* **Pre-Conditions:** @n
* HDMI-CEC Module should be initalized through Test 1 before calling this test.
*
* **Dependencies:** None@n
*
* **User Interaction:** @n
* User or Automation tool should select the Test 8 to close the created HDMI CEC handle.
*
*/
void test_l2_hdmi_cec_source_hal_Close(void)
{
    gTestID = 5;
    HDMI_CEC_STATUS status;

    UT_LOG_INFO("In %s [%02d%03d]\n", __FUNCTION__, gTestGroup, gTestID);

    UT_LOG_INFO("Calling HdmiCecClose(IN:handle:[0x%0X])", gHandle);
    status = HdmiCecClose(gHandle);
    UT_LOG_INFO("Result HdmiCecClose(IN:handle:[0x%0X]) HDMI_CEC_STATUS:[%s]", gHandle, UT_Control_GetMapString(cecError_mapTable,status));
    assert(status == HDMI_CEC_IO_SUCCESS);
    gHandle = 0;

    UT_LOG_INFO("Out %s\n", __FUNCTION__);
}


static UT_test_suite_t * pSuite = NULL;

/**
 * @brief Register the main tests for this module
 *
 * @return int - 0 on success, otherwise failure
 */
int test_register_hdmicec_hal_source_l3_tests(void)
{
    // Create the test suite
    pSuite = UT_add_suite_withGroupID("[L3 HDMICEC Source ]", NULL, NULL,UT_TESTS_L3);
    if (pSuite == NULL)
    {
         return -1;
    }
    // List of test function names and strings

    UT_add_test( pSuite, "Init HDMI CEC Source", test_l3_hdmi_cec_source_hal_Init);
    UT_add_test( pSuite, "Get Logical Address Source", test_l3_hdmi_cec_source_hal_GetLogicalAddress);
    UT_add_test( pSuite, "Transmit CEC Command Source", test_l3_hdmi_cec_source_hal_TransmitHdmiCecCommand);
    UT_add_test( pSuite, "Get Physical Address Source", test_l3_hdmi_cec_source_hal_GetPhysicalAddress);
    UT_add_test( pSuite, "Close HDMICEC Source", test_l2_hdmi_cec_source_hal_Close);

    return 0;
}


/** @} */ // End of HDMI CEC HAL Tests L1 File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
