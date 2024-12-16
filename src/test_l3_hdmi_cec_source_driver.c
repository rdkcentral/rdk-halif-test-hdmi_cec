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
#define HDMI_CEC_KVP_SIZE 128
#define HDMI_CEC_TYPE_SIZE 16

#define UT_LOG_MENU_INFO UT_LOG_INFO

typedef struct
{
    uint8_t cecCommand;       // CEC command code
    const char* commandName;  // Human-readable command name
    int32_t dataLength;       // Number of data bytes required for the command
} CecCommandMap;

typedef struct cecResponse
{
    uint8_t cecCommand;    //CEC opcode
    uint32_t payloadSize;  //CEC payload size
    uint8_t type[UT_KVP_MAX_ELEMENT_SIZE];  //Type of the opcode Broadcast/ Direct
    uint8_t payload[HDMI_CEC_MAX_PAYLOAD]; //CEC Payload
} cecResponse_t;

CecCommandMap cecCommandTable[] = {
    {0x00, "Feature Abort", 2},
    {0x04, "Image View On", 0},
    {0x05, "Tuner Step Increment", 0},
    {0x06, "Tuner Step Decrement", 0},
    {0x07, "Tuner Device Status", 8},
    {0x08, "Give Tuner Device Status", 0},
    {0x09, "Record On", 8},
    {0x0A, "Record Status", 8},
    {0x0B, "Record Off", 0},
    {0x0D, "Text View On", 0},
    {0x0F, "Record TV Screen", 0},
    {0x1A, "Give Deck Status", 0},
    {0x1B, "Deck Status", 1},
    {0x32, "Set Menu Language", 3},
    {0x33, "Clear Analog Timer", 0},
    {0x34, "Set Analog Timer", 8},
    {0x35, "Timer Status", 3},
    {0x36, "Standby", 0},
    {0x41, "Play", 0},
    {0x42, "Deck Control", 1},
    {0x43, "Timer Cleared Status", 1},
    {0x44, "User Control Pressed", 1},
    {0x45, "User Control Released", 0},
    {0x46, "Give OSD Name", 0},
    {0x47, "Set OSD Name", 14},
    {0x64, "Set OSD String", 14},
    {0x67, "Set Timer Program Title", 14},
    {0x70, "System Audio Mode Request", 2},
    {0x71, "Give Audio Status", 0},
    {0x72, "Set System Audio Mode", 1},
    {0x7A, "Report Audio Status", 1},
    {0x7D, "Give System Audio Mode Status", 0},
    {0x7E, "System Audio Mode Status", 1},
    {0x80, "Routing Change", 4},
    {0x81, "Routing Information", 2},
    {0x82, "Active Source", 2},
    {0x83, "Give Physical Address", 0},
    {0x84, "Report Physical Address", 3},
    {0x85, "Request Active Source", 0},
    {0x86, "Set Stream Path", 2},
    {0x87, "Device Vendor ID", 3},
    {0x89, "Vendor Command", 14},
    {0x8A, "Vendor Remote Button Down", 1},
    {0x8B, "Vendor Remote Button Up", 0},
    {0x8C, "Give Device Vendor ID", 0},
    {0x8D, "Menu Request", 1},
    {0x8E, "Menu Status", 1},
    {0x8F, "Give Device Power Status", 0},
    {0x90, "Report Power Status", 1},
    {0x91, "Get Menu Language", 0},
    {0x92, "Select Analog Service", 4},
    {0x93, "Select Digital Service", 4},
    {0x97, "Set Digital Timer", 6},
    {0x99, "Clear Digital Timer", 0},
    {0x9A, "Set Audio Rate", 1},
    {0x9D, "Inactive Source", 2},
    {0x9E, "CEC Version", 1},
    {0x9F, "Get CEC Version", 0},
    {0xA0, "Vendor Command With ID", 17},
    {0xA1, "Clear External Timer", 0},
    {0xA2, "Set External Timer", 9},
    {0xA7, "Request Current Latency", 2},
    {0xA8, "Report Current Latency", 5},
    {0xC0, "Initiate ARC", 0},
    {0xC1, "Report ARC Initiated", 0},
    {0xC2, "Report ARC Terminated", 0},
    {0xC3, "Request ARC Initiation", 0},
    {0xC4, "Request ARC Termination", 0},
    {0xC5, "Terminate ARC", 0},
    {0xFF, "Abort", 0}
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
static uint8_t gBroadcastAddress = 0xF;

/**
* @brief CEC Command with data size mapping function.
*
* This function maps the provided CEC command with the data length required for the CEC Command.
* This will help the test to consider the correct size of the data required for the CEC Command.
*
*/
static int32_t getCecCommandInfo(uint8_t cecCommand, const char** commandName, int32_t* dataLength)
{

    int32_t tableSize = sizeof(cecCommandTable) / sizeof(CecCommandMap);

    for (int32_t i = 0; i < tableSize; i++)
    {
        if (cecCommandTable[i].cecCommand == cecCommand)
        {
            *commandName = cecCommandTable[i].commandName;
            *dataLength = cecCommandTable[i].dataLength;
            return 0; // Command found
        }
    }

    return -1; // Command not found
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

static bool getCommandResponse(uint8_t opcode, cecResponse_t *pResponse)
{
    uint32_t numCommands = 0;
    char key_string[HDMI_CEC_KVP_SIZE] = {0};

    memset(pResponse, 0, sizeof(cecResponse_t));

    numCommands = UT_KVP_PROFILE_GET_LIST_COUNT("hdmicec/cec_responses");

    for(uint32_t i = 0; i < numCommands; i++)
    {
        uint8_t command;
        uint8_t found;

        snprintf(key_string, HDMI_CEC_KVP_SIZE, "hdmicec/cec_responses/%d/command" , i);
        command = UT_KVP_PROFILE_GET_UINT8(key_string);

        if(command != opcode)
        {
            continue;
        }
        snprintf(key_string, HDMI_CEC_KVP_SIZE, "hdmicec/cec_responses/%d/response/command" , i);
        found = ut_kvp_fieldPresent(ut_kvp_profile_getInstance(), key_string);

        if(found)
        {
            pResponse->cecCommand = UT_KVP_PROFILE_GET_UINT8(key_string);

            snprintf(key_string, HDMI_CEC_KVP_SIZE, "hdmicec/cec_responses/%d/response/type" , i);
            UT_KVP_PROFILE_GET_STRING(key_string, (char*)pResponse->type);

            snprintf(key_string, HDMI_CEC_KVP_SIZE, "hdmicec/cec_responses/%d/response/payload" , i);
            pResponse->payloadSize = UT_KVP_PROFILE_GET_LIST_COUNT(key_string);

            for(uint8_t j = 0; j < pResponse->payloadSize; j++)
            {
                if((command == 0xA8 || command == 0x84) && j < 2) //Needs this devices physical address
                {
                    pResponse->payload[j] = gPhysicalAddressBytes[j];
                    continue;
                }
                else{
                    snprintf(key_string, HDMI_CEC_KVP_SIZE, "hdmicec/cec_responses/%d/response/payload/%d" , i, j);
                    pResponse->payload[j] = UT_KVP_PROFILE_GET_UINT8(key_string);

                }
                
            }
            return true;
        }
    }
    return false;
}

static void sendResponse(int32_t handle, uint8_t initiator, uint8_t destination,
                         uint8_t *buf, int32_t len, cecResponse_t *pCecResponse)
{
    uint8_t prBuffer[HDMI_CEC_MAX_PAYLOAD] = {0};
    uint8_t response[HDMI_CEC_MAX_PAYLOAD] = {0};

    if (pCecResponse->payloadSize)
    {
        int32_t result;
        const char* commandName;
        int32_t expectedDataLength;

        if (!strcmp((char*)pCecResponse->type, "Direct"))
        {
            response[0] = (destination << 4) | initiator;
        }
        else
        {
            //Send braodcast message
            response[0] = (gLogicalAddress << 4) | gBroadcastAddress;
        }
        response[1] = pCecResponse->cecCommand;

        for (int32_t index = 0; index < pCecResponse->payloadSize; index++)
        {
            response[index + 2] = pCecResponse->payload[index];
        }

        {
            uint8_t *temp = prBuffer;
            // Log each byte received in the buffer
            for (int32_t index = 0; index < pCecResponse->payloadSize + 2; index++)
            {
                int32_t len = 0;
                len = snprintf((char*)temp, HDMI_CEC_MAX_PAYLOAD, "%02X:", response[index]);
                temp += len;
            }
            prBuffer[strlen((char*)prBuffer)-1] = '\0';

        }

        HdmiCecTx(handle, response, pCecResponse->payloadSize + 2, &result);

        getCecCommandInfo(pCecResponse->cecCommand, &commandName, &expectedDataLength);

        UT_LOG_INFO("Sent Response Opcode: [0x%02X] [%s] Initiator: [%x], Destination: [%x] Data: [%s]\n",
                                 pCecResponse->cecCommand, commandName, destination, initiator, prBuffer);
    }
}

static void onRxDataReceived(int32_t handle, void *callbackData, uint8_t *buf, int32_t len)
{
    UT_LOG_INFO("In %s(IN: handle: [%p], IN: callbackData: [%p], IN: buf: [%p], IN: len: [%d])\n", __FUNCTION__, handle, callbackData, buf, len);

    const char* commandName;
    int32_t expectedDataLength;

    if ((handle != 0) && (callbackData != NULL) && (len > 0))
    {
        // Parse the command
        uint8_t initiator = (buf[0] >> 4) & 0xF;  // Extract initiator address
        uint8_t destination = buf[0] & 0xF;       // Extract destination address
        uint8_t opcode;                           // Command opcode
        cecResponse_t cecResponse = {0};          // cec response
        uint8_t prBuffer[HDMI_CEC_MAX_PAYLOAD] = {0};
        bool result = false;

        if( len == 1)
        {
            UT_LOG_INFO("Received Ping message Initiator: [%x], Destination: [%x]", initiator, destination);
            goto exit;
        }

        opcode = buf[1];

        if(getCecCommandInfo(opcode, &commandName, &expectedDataLength) != 0)
        {
            UT_LOG_WARNING("CEC command 0x%02X is not recognized", opcode);
            goto exit;
        }

        {
            uint8_t *temp = prBuffer;
            // Log each byte received in the buffer
            for (int32_t index = 0; index < len; index++)
            {
                int32_t len = 0;
                len = snprintf((char*)temp, HDMI_CEC_MAX_PAYLOAD, "%02X:", buf[index]);
                temp += len;
            }
            prBuffer[strlen((char*)prBuffer)-1] = '\0';
        }

        UT_LOG_INFO("Received Opcode: [0x%02X] [%s] Initiator: [%x], Destination: [%x] Data: [%s]\n", opcode, commandName, initiator, destination, prBuffer);

        result = getCommandResponse(opcode, &cecResponse);

        UT_LOG_INFO("result: %d\n", result);

        sendResponse(handle, initiator, destination, buf, len, &cecResponse);
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
exit:
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

    UT_LOG_INFO("Calling HdmiCecGetLogicalAddress(IN:handle:[0x%0X], OUT:logicalAddress:[])", gHandle);
    status = HdmiCecGetLogicalAddress(gHandle, &gLogicalAddress);
    UT_LOG_INFO("Result HdmiCecGetLogicalAddress(IN:handle:[0x%0X], OUT:logicalAddress:[%d]) HDMI_CEC_STATUS:[%s]", gHandle, gLogicalAddress, UT_Control_GetMapString(cecError_mapTable,status));
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
    readHex(&destinationLogicalAddress);

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
    assert(status == HDMI_CEC_IO_SUCCESS);

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
}

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

    UT_add_test( pSuite, "Init HDMI CEC", test_l3_hdmi_cec_source_hal_Init);
    UT_add_test( pSuite, "Get Logical Address", test_l3_hdmi_cec_source_hal_GetLogicalAddress);
    UT_add_test( pSuite, "Transmit CEC Command", test_l3_hdmi_cec_source_hal_TransmitHdmiCecCommand);
    UT_add_test( pSuite, "Get Physical Address", test_l3_hdmi_cec_source_hal_GetPhysicalAddress);
    UT_add_test( pSuite, "Close HDMI CEC", test_l2_hdmi_cec_source_hal_Close);

    return 0;
}


/** @} */ // End of HDMI CEC HAL Tests L1 File
/** @} */ // End of HDMI CEC HAL Tests
/** @} */ // End of HDMI CEC Module
/** @} */ // End of HPK
