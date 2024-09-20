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
#include <assert.h>
#include <pthread.h>

#include "hdmi_cec_driver.h"
#include "vcHdmiCec.h"
#include "vcDevice.h"
#include "vcCommand.h"
#include "ut_kvp_profile.h"
#include "ut_control_plane.h"

#define MAX_QUEUE_SIZE 32
#define CONTROL_PLANE_PORT 8080

typedef enum
{
  CEC_MSG_TYPE_NONE = 0,
  CEC_MSG_TYPE_COMMAND,
  CEC_MSG_TYPE_EVENT,
  CEC_MSG_TYPE_CONFIG,
  CEC_MSG_TYPE_STATE,
  CEC_MSG_TYPE_EXIT_REQUESTED
} vcHdmiCec_msg_type_t;

typedef struct
{
  vcHdmiCec_msg_type_t type;
  char* message;
  uint32_t size;
} vcHdmiCec_message_t;

/**HDMI CEC HAL Data structures */
typedef struct
{
  unsigned short id;
  unsigned int physical_address;
  vcDevice_port_type_t type;
  bool cec_supported;
  bool arc_supported;
} vcHdmiCec_port_info_t;

typedef enum
{
  HAL_STATE_CLOSED = 0,
  HAL_STATE_OPEN,
  HAL_STATE_READY
} vcHdmiCec_hal_state_t;

typedef struct
{
  HdmiCecRxCallback_t rx_cb_func;
  void* rx_cb_data;
  HdmiCecTxCallback_t tx_cb_func;
  void* tx_cb_data;
} vcHdmiCec_callbacks_t;

typedef struct
{
  vcHdmiCec_hal_state_t state;
  struct vcDevice_info_t* emulated_device;
  int num_ports;
  vcHdmiCec_port_info_t *ports;
  int num_devices;
  struct vcDevice_info_t* devices_map;
  vcHdmiCec_callbacks_t callbacks;
  vcDevice_logical_address_pool_t address_pool;

  pthread_t msg_handler_thread;
  uint32_t msg_count;
  vcHdmiCec_message_t msg_queue[MAX_QUEUE_SIZE];
  pthread_mutex_t msg_queue_mutex;
  pthread_cond_t msg_queue_condition;
  volatile bool exit_request;
} vcHdmiCec_hal_t;

/**Virtual Componenent Data types*/
typedef struct
{
  vcHdmiCec_hal_t * cec_hal;
  ut_kvp_instance_t *profile_instance;
  ut_controlPlane_instance_t *cp_instance;
  bool bOpened;
} vcHdmiCec_internal_t;


/*Global variables*/

/* TODO:
 * HdmiCec hal functions are taking in a int handle which is a problem in 64bit platforms like in Linux PCs.
 * When HdmiCecOpen provides a handle, the handle is lost in translation due to this explicit conversion of
 * pointer to int. Hence the subsequent HdmiCec functions dont have a reliable way to associate the integer
 * handle to a memory instance of the driver struct.
 * when the  int handle is upgraded to support a void * in the future. We should re-review if we want to use a Singleton or not at that time. 
 */
static vcHdmiCec_internal_t* gvcHdmiCec = NULL;

const static vcCommand_strVal_t gPortStrVal [] = {
  { "in", (int)PORT_TYPE_INPUT  },
  { "out", (int)PORT_TYPE_OUTPUT },
  { "unknown", (int)PORT_TYPE_UNKNOWN }
};

const static vcCommand_strVal_t gMsgStrVal [] = {
  { CEC_MSG_PREFIX"/"CEC_MSG_COMMAND, (int)CEC_MSG_TYPE_COMMAND },
  { CEC_MSG_PREFIX"/"CEC_MSG_CONFIG, (int)CEC_MSG_TYPE_CONFIG },
  { CEC_MSG_PREFIX"/"CEC_MSG_EVENT, (int)CEC_MSG_TYPE_EVENT },
  { CEC_MSG_PREFIX"/"CEC_MSG_STATE, (int)CEC_MSG_TYPE_STATE }
};

static void TeardownHal (vcHdmiCec_hal_t* hal);
static void EnqueueMessage(vcHdmiCec_hal_t *hal, vcHdmiCec_message_t *msg);
static void DequeueMessage(vcHdmiCec_hal_t *hal, vcHdmiCec_message_t* out_msg);
static void ProcessMsg( char *key, ut_kvp_instance_t *instance, void* user_data);
static void* MessageHandler(void *data);
static ut_kvp_instance_t* KVPInstanceOpen(char* msg, int size);
static void ParseCommand(vcHdmiCec_hal_t *hal, char* cmd, int size, vcCommand_t *cec_cmd);
static void LoadPortsInfo (ut_kvp_instance_t* instance, vcHdmiCec_port_info_t* ports, unsigned int nPorts);
static void ResetMessage(vcHdmiCec_message_t *msg);
static void PrintStatus(vcHdmiCec_hal_t *cec);
static void PrintDevicesInfo(vcHdmiCec_hal_t *cec);
static void PrintPortsInfo(vcHdmiCec_hal_t *cec);

static ut_kvp_instance_t* KVPInstanceOpen(char* msg, int size)
{
  ut_kvp_instance_t *kvpInstance = NULL;
  ut_kvp_status_t status;

  if (msg == NULL)
  {
      return NULL;
  }
  kvpInstance = ut_kvp_createInstance();
  assert(kvpInstance != NULL);
  //ut_kvp_openMemory expects msg to be malloc'ed.
  // The ownership of the msg is transferred to ut_kvp_openMemory.
  // ut_kvp_destroyInstance will take care of freeing the msg.
  status = ut_kvp_openMemory(kvpInstance, msg, size);
  if (status != UT_KVP_STATUS_SUCCESS)
  {
      VC_LOG_ERROR("ut_kvp_openMemory() - Read Failure\n");
      ut_kvp_destroyInstance(kvpInstance);
      return NULL;
  }
  return kvpInstance;
}

static void ParseCommand(vcHdmiCec_hal_t *hal, char* cmd, int size, vcCommand_t *cec_cmd)
{
  char str[UT_KVP_MAX_ELEMENT_SIZE];
  vcCommand_opcode_t opcode = CEC_OPCODE_UNKNOWN;
  struct vcDevice_info_t *src, *dest;
  vcCommand_logical_address_t la;
  assert(cmd != NULL);

  vcCommand_Clear(cec_cmd);
  ut_kvp_instance_t *kvpInstance = KVPInstanceOpen(cmd, size);
  assert(kvpInstance != NULL);
  ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_MSG_COMMAND, str, UT_KVP_MAX_ELEMENT_SIZE);
  opcode = vcCommand_GetOpCode(str);
  if(opcode == CEC_OPCODE_UNKNOWN)
  {
    VC_LOG_ERROR("ParseCommand: Opcode[%s] Unknown", str);
    return;
  }
  VC_LOG("ParseCommand: Opcode[%s]", str);

  ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_CMD_INITIATOR, str, UT_KVP_MAX_ELEMENT_SIZE);
  src = vcDevice_Get(hal->devices_map, str);
  if(src == NULL)
  {
    VC_LOG_ERROR("ParseCommand: Initiator[%s] Unknown", str);
    return;
  }
  VC_LOG("ParseCommand: Initiator[%s] ", str);
  ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_CMD_DESTINATION, str, UT_KVP_MAX_ELEMENT_SIZE);
  if(strcmp(str, CEC_BROADCAST) != 0)
  {
    dest = vcDevice_Get(hal->devices_map, str);
    if(dest == NULL)
    {
      VC_LOG_ERROR("ParseCommand: Destination[%s] Unknown", str);
      return;
    }
  }

  VC_LOG("ParseCommand: Destination[%s] ", str);

  if(dest == NULL)
  {
    la = LOGICAL_ADDRESS_BROADCAST;
  }
  else 
  {
    la = dest->logical_address;
  }
  vcCommand_Format(cec_cmd, src->logical_address, la, opcode);

  switch (opcode)
  {
    case CEC_ACTIVE_SOURCE:
    case CEC_INACTIVE_SOURCE:
    {
      uint8_t buf[2];
      buf[0] = (src->physical_address >> 8) & 0xFF;
      buf[1] = src->physical_address & 0xFF;
      vcCommand_PushBackArray(cec_cmd, buf, sizeof(buf));
    }
    break;

    case CEC_SET_OSD_NAME:
    {
      ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CMD_DATA_OSD_NAME, str, UT_KVP_MAX_ELEMENT_SIZE);
      vcCommand_PushBackArray(cec_cmd, (uint8_t *)str, strlen(str));
    }
    break;

    default:
    {
    }
    break;
  }
  ut_kvp_destroyInstance(kvpInstance);
}


static void HandleStateMessages( vcHdmiCec_hal_t *hal, char* cmd, int size)
{
  char str[UT_KVP_MAX_ELEMENT_SIZE];
  ut_kvp_instance_t *kvpInstance = KVPInstanceOpen(cmd, size);
  struct vcDevice_info_t *device = NULL, *parent = NULL;
  assert(kvpInstance != NULL);
  ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_MSG_STATE, str, UT_KVP_MAX_ELEMENT_SIZE);

  if(!strcmp(str, CEC_MSG_STATE_ADD_DEVICE))
  {
     device = vcDevice_CreateMapFromProfile(kvpInstance, "hdmicec/parameters");
     if(device == NULL)
     {
        VC_LOG_ERROR("HandleStateMessages: AddDevice failed to create device");
        ut_kvp_destroyInstance(kvpInstance);
        return;
     }
     ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_CMD_PARAMETERS"/parent", str, UT_KVP_MAX_ELEMENT_SIZE);
     parent = vcDevice_Get(hal->devices_map, str);
     if(parent == NULL)
     {
        VC_LOG_ERROR("HandleStateMessages: AddDevice failed to get parent");
        vcDevice_DestroyMap(device);
        ut_kvp_destroyInstance(kvpInstance);
        return;
     }
     //Check if the new device can be added to a free port
     if(parent == hal->emulated_device && parent->type == DEVICE_TYPE_TV)
     {
        if(parent->number_children >= hal->num_ports)
        {
          VC_LOG_ERROR("HandleStateMessages: AddDevice: No free port to Add Device");
          vcDevice_DestroyMap(device);
          ut_kvp_destroyInstance(kvpInstance);
          return;
        }
     }
     vcDevice_InsertChild(parent, device);
     vcDevice_AllocatePhysicalLogicalAddresses(hal->devices_map, hal->emulated_device,&hal->address_pool);
     //Now we have added the device sucessfully. Lets announce the device.
     {
        vcCommand_t cmd;
        uint32_t len;
        uint8_t buf[2];
        uint8_t cec_data[VCCOMMAND_MAX_DATA_SIZE];
        vcCommand_Clear(&cmd);
        vcCommand_Format(&cmd, device->logical_address, LOGICAL_ADDRESS_BROADCAST, CEC_REPORT_PHYSICAL_ADDRESS);
        buf[0] = (device->physical_address >> 8) & 0xFF;
        buf[1] = device->physical_address & 0xFF;
        vcCommand_PushBackArray(&cmd, buf, sizeof(buf));
        vcCommand_PushBackByte(&cmd, (uint8_t)device->type);
        len = vcCommand_GetRawBytes(&cmd, cec_data, VCCOMMAND_MAX_DATA_SIZE);
        if(hal->callbacks.rx_cb_func != NULL)
        {
          hal->callbacks.rx_cb_func((intptr_t)hal, hal->callbacks.rx_cb_data, cec_data, len);
        }
      }
  }
  else if(!strcmp(str, CEC_MSG_STATE_REMOVE_DEVICE))
  {
     ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_CMD_PARAMETERS"/name", str, UT_KVP_MAX_ELEMENT_SIZE);
     device = vcDevice_Get(hal->devices_map, str);
     if(device == NULL)
     {
        VC_LOG_ERROR("HandleStateMessages: RemoveDevice failed to get device");
        ut_kvp_destroyInstance(kvpInstance);
        return;
     }
     vcDevice_RemoveChild(hal->devices_map, str);
  }
  else if(!strcmp(str, CEC_MSG_STATE_PRINT_STATUS))
  {
    ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_CMD_PARAMETERS"/status", str, UT_KVP_MAX_ELEMENT_SIZE);
    if(!strcmp(str, "Devices"))
    {
      PrintDevicesInfo(hal);
    }
    else if(!strcmp(str, "Ports"))
    {
      PrintPortsInfo(hal);
    }
    else
    {
      PrintStatus(hal);
    }
  }
  else
  {
    VC_LOG_ERROR("Unknown State Message: %s", str);
  }
}

static void ProcessMsg( char *key, ut_kvp_instance_t *instance, void* user_data)
{
  vcHdmiCec_message_t msg;
  char *message;
  vcHdmiCec_internal_t *vc = (vcHdmiCec_internal_t*) user_data;
  assert(vc != NULL);
  if(vc->cec_hal == NULL || vc->cec_hal->state != HAL_STATE_READY)
  {
    VC_LOG_ERROR("ProcessMsg: HAL not ready", key);
    return;
  }
  msg.type = vcCommand_GetValue(gMsgStrVal, COUNT_OF(gMsgStrVal), key, CEC_MSG_TYPE_NONE);

  if(msg.type == CEC_MSG_TYPE_NONE)
  {
    VC_LOG_ERROR("ProcessMsg: Unknown Message Type [%s]", key);
    return;
  }
  if(msg.type != CEC_MSG_TYPE_EXIT_REQUESTED)
  {
    message = ut_kvp_getData(instance);
    msg.message = strdup(message);
    //ut_kvp_openMemory expects msg to be malloc'ed.
    //The ownership of the string is transferred to ut_kvp_openMemory. Do not free the string
    msg.size = strlen(msg.message);
  }
  EnqueueMessage(vc->cec_hal, &msg);
}

static void EnqueueMessage(vcHdmiCec_hal_t *hal, vcHdmiCec_message_t *msg)
{
    pthread_mutex_lock(&hal->msg_queue_mutex);
    if (hal->msg_count < MAX_QUEUE_SIZE)
    {
      hal->msg_queue[hal->msg_count].size = msg->size;
      hal->msg_queue[hal->msg_count].type = msg->type;
      hal->msg_queue[hal->msg_count].message = msg->message;
      hal->msg_count++;
      pthread_cond_signal(&hal->msg_queue_condition);
    }
    pthread_mutex_unlock(&hal->msg_queue_mutex);
}

static void DequeueMessage(vcHdmiCec_hal_t *hal, vcHdmiCec_message_t* out_msg)
{
    pthread_mutex_lock(&hal->msg_queue_mutex);
    while (hal->msg_count == 0)
    {
        pthread_cond_wait(&hal->msg_queue_condition, &hal->msg_queue_mutex);
    }
    *out_msg = hal->msg_queue[0]; //Copy the data in top element to out var.
    //Reset the Top
    ResetMessage(&hal->msg_queue[0]);

    //Adjust the queue
    for (int i = 0; i < hal->msg_count - 1; i++)
    {
        hal->msg_queue[i] = hal->msg_queue[i + 1];
    }
    hal->msg_count--;
    pthread_mutex_unlock(&hal->msg_queue_mutex);
}

static void ResetMessage(vcHdmiCec_message_t *msg)
{
  assert(msg != NULL);
  //ut_kvp_openMemory expects msg to be malloc'ed. The ownership of the string is transferred to ut_kvp_openMemory.
  // Do not free msg->message
  msg->message = NULL;
  msg->size = 0;
  msg->type = CEC_MSG_TYPE_NONE;
}

static void* MessageHandler(void *data)
{
  vcHdmiCec_hal_t *hal = (vcHdmiCec_hal_t *)data;
  vcHdmiCec_message_t msg;

  if (hal == NULL)
  {
    return NULL;
  }

  while (!hal->exit_request)
  {
    DequeueMessage(hal, &msg);
    switch (msg.type)
    {
      case CEC_MSG_TYPE_EXIT_REQUESTED:
      {
        VC_LOG("EXIT REQUESTED in MessageHandler\n");
        hal->exit_request = true;
      }
      break;

      case CEC_MSG_TYPE_COMMAND:
      {
        vcCommand_t cmd;
        uint32_t len;
        uint8_t cec_data[VCCOMMAND_MAX_DATA_SIZE];
        ParseCommand(hal, msg.message, msg.size, &cmd);
        len = vcCommand_GetRawBytes(&cmd, cec_data, VCCOMMAND_MAX_DATA_SIZE);
        if(hal->callbacks.rx_cb_func != NULL)
        {
          hal->callbacks.rx_cb_func((intptr_t)hal, hal->callbacks.rx_cb_data, cec_data, len);
        }
      }
      break;

      case CEC_MSG_TYPE_EVENT:
      {

      }
      break;

      case CEC_MSG_TYPE_CONFIG:
      {

      }
      break;

      case CEC_MSG_TYPE_STATE:
      {
        HandleStateMessages(hal, msg.message, msg.size);
      }
      break;

      default:
      {

      }
      break;
    }
    ResetMessage(&msg);
  }
  return NULL;
}

static void LoadPortsInfo (ut_kvp_instance_t* instance, vcHdmiCec_port_info_t* ports, unsigned int nPorts)
{
  char *prefix = "hdmicec/ports/";
  char tmp[UT_KVP_MAX_ELEMENT_SIZE];
  
  assert(instance != NULL);
  assert(ports != NULL);

  for (int i = 0; i < nPorts; ++i)
  {
    char type[8];
    strcpy(tmp, prefix);
    int length = snprintf( NULL, 0, "%d", i );
    snprintf( tmp + strlen(prefix) , length + 1, "%d", i );

    strcpy( tmp + strlen(prefix) + length  , "/id");
    ports[i].id = ut_kvp_getUInt32Field(instance, tmp);

    strcpy(tmp + strlen(prefix) + length, "/type");
    ut_kvp_getStringField(instance, tmp, type, sizeof(type));
    ports[i].type = vcCommand_GetValue(gPortStrVal, COUNT_OF(gPortStrVal), type, (int)PORT_TYPE_UNKNOWN);

    strcpy(tmp + strlen(prefix) + length, "/cec_supported");
    ports[i].cec_supported = ut_kvp_getBoolField(instance, tmp);

    strcpy(tmp + strlen(prefix) + length, "/arc_supported");
    ports[i].arc_supported = ut_kvp_getBoolField(instance, tmp);

  }
}

static void PrintStatus(vcHdmiCec_hal_t *cec)
{
  assert(cec != NULL);
  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("Emulated Device               : %s", cec->emulated_device->osd_name);
  VC_LOG("Number of Ports               : %d", cec->num_ports);
  VC_LOG("Number of devices in Network  : %d", cec->num_devices);
  VC_LOG("===========================");

  vcDevice_PrintMap(cec->devices_map, 0);
  VC_LOG("=================================");
}

static void PrintDevicesInfo(vcHdmiCec_hal_t *cec)
{
  assert(cec != NULL);
  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("Number of devices in Network  : %d", cec->num_devices);
  VC_LOG("===========================");
  vcDevice_PrintMap(cec->devices_map, 0);
  VC_LOG("=================================");
}

static void PrintPortsInfo(vcHdmiCec_hal_t *cec)
{
  assert(cec != NULL);
  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("Number of Ports               : %d", cec->num_ports);
  VC_LOG("=================================");
  for(int i = 0; i < cec->num_ports; ++i)
  {
    VC_LOG("Port Id        : %d", cec->ports[i].id);
    VC_LOG("Port type      : %s", vcCommand_GetString(gPortStrVal, COUNT_OF(gPortStrVal), (int)cec->ports[i].type));
    VC_LOG("CEC Supported  : %s", (cec->ports[i].cec_supported ? "true" : "false"));
    VC_LOG("ARC Supported  : %s", (cec->ports[i].arc_supported ? "true" : "false"));
  }
  VC_LOG("=================================");
}

static void TeardownHal (vcHdmiCec_hal_t* hal)
{
  vcHdmiCec_message_t msg = {0};
  if(hal == NULL)
  {
    return;
  }

  if ( hal->msg_handler_thread )
  {
    memset(&msg, 0, sizeof(msg));
    msg.type = CEC_MSG_TYPE_EXIT_REQUESTED;
    EnqueueMessage(hal, &msg);
    if (pthread_join(hal->msg_handler_thread, NULL) != 0)
    {
      VC_LOG_ERROR("Failed to join msg_handler_thread from instance\n");
    }
  }
  hal->msg_handler_thread = 0;
  vcDevice_DestroyMap(hal->devices_map);

  if(hal->ports)
  {
    free (hal->ports);
  }
  hal->callbacks.rx_cb_func = NULL;
  hal->callbacks.tx_cb_func = NULL;
  free(hal);
  
}

vcHdmiCec_t* vcHdmiCec_Initialize( void )
{
  vcHdmiCec_internal_t *result = NULL;
  result = (vcHdmiCec_internal_t*)malloc(sizeof(vcHdmiCec_internal_t));
  if(result == NULL)
  {
    VC_LOG_ERROR("vcHdmiCec_Initialize: Out of memory");
    return NULL;
  }
  result->profile_instance = ut_kvp_createInstance();
  assert(result->profile_instance != NULL);
  result->cec_hal = NULL;
  result->bOpened = false;
  result->cp_instance = NULL;

  gvcHdmiCec = result;
  return (vcHdmiCec_t *)result;
}

vcHdmiCec_Status_t vcHdmiCec_Open( vcHdmiCec_t* pvcHdmiCec, char* pProfilePath, bool enableCPMsgs )
{

  ut_kvp_status_t status;
  vcHdmiCec_internal_t* vcHdmiCec = (vcHdmiCec_internal_t*)pvcHdmiCec;

  if(vcHdmiCec == NULL || vcHdmiCec != gvcHdmiCec)
  {
    VC_LOG_ERROR("vcHdmiCec_Open: Invalid handle");
    return VC_HDMICEC_STATUS_INVALID_HANDLE;
  }

  if(vcHdmiCec->bOpened)
  {
    VC_LOG_ERROR("vcHdmiCec_Open: Already Opened");
    return VC_HDMICEC_STATUS_SUCCESS;
  }
  
if(pProfilePath == NULL)
  {
    VC_LOG_ERROR("vcHdmiCec_Open: Invalid Profile path");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }

  status = ut_kvp_open(vcHdmiCec->profile_instance, pProfilePath);
  if(status != UT_KVP_STATUS_SUCCESS)
  {
    VC_LOG_ERROR("ut_kvp_open: status: %d", status);
    assert(status == UT_KVP_STATUS_SUCCESS);
    return VC_HDMICEC_STATUS_PROFILE_READ_ERROR;
  }
  vcHdmiCec->bOpened = false;

  if(enableCPMsgs)
  {
    vcHdmiCec->cp_instance = UT_ControlPlane_Init(CONTROL_PLANE_PORT);
    assert(vcHdmiCec->cp_instance != NULL);
    UT_ControlPlane_RegisterCallbackOnMessage(gvcHdmiCec->cp_instance, "hdmicec/command", &ProcessMsg, (void*) vcHdmiCec);
    UT_ControlPlane_RegisterCallbackOnMessage(gvcHdmiCec->cp_instance, "hdmicec/state", &ProcessMsg, (void*) vcHdmiCec);
    UT_ControlPlane_Start(vcHdmiCec->cp_instance);
  }
  vcHdmiCec->bOpened = true;
  return VC_HDMICEC_STATUS_SUCCESS;
}

vcHdmiCec_Status_t vcHdmiCec_Close( vcHdmiCec_t* pvcHdmiCec )
{
  vcHdmiCec_internal_t* vcHdmiCec = (vcHdmiCec_internal_t*)pvcHdmiCec;

  if(vcHdmiCec == NULL || vcHdmiCec != gvcHdmiCec)
  {
    VC_LOG_ERROR("vcHdmiCec_Close: Invalid handle");
    return VC_HDMICEC_STATUS_INVALID_HANDLE;
  }

  if(!vcHdmiCec->bOpened)
  {
    VC_LOG_ERROR("vcHdmiCec_Close: Not Opened");
    return VC_HDMICEC_STATUS_NOT_OPENED;
  }

  //Should we enforce HdmiCecClose to be called before?
  if(vcHdmiCec->cec_hal != NULL && vcHdmiCec->cec_hal->state != HAL_STATE_CLOSED)
  {
    vcHdmiCec_Close(pvcHdmiCec);
  }
  vcHdmiCec->bOpened = false;

  return VC_HDMICEC_STATUS_SUCCESS;
}


vcHdmiCec_Status_t vcHdmiCec_Deinitialize(vcHdmiCec_t *pvcHdmiCec)
{
  vcHdmiCec_internal_t* vcHdmiCec = (vcHdmiCec_internal_t*)pvcHdmiCec;

  if(vcHdmiCec == NULL || vcHdmiCec != gvcHdmiCec)
  {
    VC_LOG_ERROR("vcHdmiCec_Deinitialize: Invalid handle Handle");
    return VC_HDMICEC_STATUS_INVALID_HANDLE;
  }

  if(vcHdmiCec->bOpened)
  {
    vcHdmiCec_Close(pvcHdmiCec);
  }
  if(vcHdmiCec->cp_instance != NULL)
  {
    UT_ControlPlane_Exit(vcHdmiCec->cp_instance);
  }
  ut_kvp_destroyInstance(vcHdmiCec->profile_instance);
  free(vcHdmiCec);
  gvcHdmiCec = NULL;
  return VC_HDMICEC_STATUS_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecOpen(int* handle)
{
  char emulated_device[MAX_OSD_NAME_LENGTH];
  vcHdmiCec_hal_t* cec;
  ut_kvp_instance_t *profile_instance;
  vcHdmiCec_port_info_t* ports;

  if(handle == NULL)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }

  if(gvcHdmiCec == NULL)
  {
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(gvcHdmiCec->cec_hal != NULL)
  {
    return HDMI_CEC_IO_ALREADY_OPEN;
  }

  cec = (vcHdmiCec_hal_t*)malloc(sizeof(vcHdmiCec_hal_t));
  if(cec == NULL) 
  {
    VC_LOG_ERROR( "HdmiCecOpen: Out of Memory" );
    return HDMI_CEC_IO_GENERAL_ERROR;
  }
  memset(cec, 0, sizeof(vcHdmiCec_hal_t));

  profile_instance = gvcHdmiCec->profile_instance;
  assert(profile_instance != NULL);
  
  ut_kvp_getStringField(profile_instance, "hdmicec/emulated_device", emulated_device, MAX_OSD_NAME_LENGTH);

  cec->num_ports = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_ports");
  ports = (vcHdmiCec_port_info_t*) malloc(sizeof(vcHdmiCec_port_info_t) * cec->num_ports);
  assert(ports != NULL);

  LoadPortsInfo(profile_instance, ports, cec->num_ports);
  cec->ports = ports;

  //Setup Eventing and callback
  cec->exit_request = false;
  pthread_mutex_init( &cec->msg_queue_mutex, NULL );
  pthread_cond_init( &cec->msg_queue_condition, NULL );
  pthread_create(&cec->msg_handler_thread, NULL, MessageHandler, (void*) cec );
  memset(&cec->msg_queue, 0, sizeof(vcHdmiCec_message_t) * MAX_QUEUE_SIZE);


  //Device Discovery and Network Topology
  cec->num_devices = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_devices");

  cec->devices_map = vcDevice_CreateMapFromProfile(profile_instance, "hdmicec/device_map/0");
  cec->emulated_device = vcDevice_Get(cec->devices_map, emulated_device);

  if(cec->num_devices < 1)
  {
    VC_LOG_ERROR( "HdmiCecOpen: number of devices < 1" );
    assert(cec->num_devices >= 1);
  }
  if(cec->devices_map == NULL)
  {
    VC_LOG_ERROR( "HdmiCecOpen: device_map = NULL" );
    assert(cec->devices_map != NULL);
  }

  if(cec->emulated_device == NULL)
  {
    VC_LOG_ERROR("HdmiCecOpen: Couldnt load emulated device info");
    assert(cec->emulated_device != NULL);
    TeardownHal(cec);
    return HDMI_CEC_IO_GENERAL_ERROR;
  }
  vcDevice_InitLogicalAddressPool(&cec->address_pool);
  vcDevice_AllocatePhysicalLogicalAddresses(cec->devices_map, cec->emulated_device, &cec->address_pool);

  if(cec->emulated_device->type == DEVICE_TYPE_TV)
  { 
    VC_LOG("HdmiCecOpen: Emulating a TV");
    cec->emulated_device->physical_address = 0;
    cec->emulated_device->logical_address = 0x0F;
  }
  else
  {
    VC_LOG("HdmiCecOpen: Emulating a Source device");
    //TODO Auto Allocate Logical addresses
  }
  PrintStatus(cec);

  *handle = (intptr_t) cec;
  gvcHdmiCec->cec_hal = cec;
  cec->state = HAL_STATE_READY;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecClose(int handle)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecClose: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecClose: Invalid Handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }

  TeardownHal(gvcHdmiCec->cec_hal);
  gvcHdmiCec->cec_hal = NULL;

  return HDMI_CEC_IO_SUCCESS;
}


HDMI_CEC_STATUS HdmiCecGetPhysicalAddress(int handle, unsigned int* physicalAddress)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecGetPhysicalAddress: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecGetPhysicalAddress: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  if(physicalAddress == NULL)
  {
    VC_LOG_ERROR("HdmiCecGetPhysicalAddress: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  *physicalAddress = gvcHdmiCec->cec_hal->emulated_device->physical_address;
  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecAddLogicalAddress: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecAddLogicalAddress: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  if(gvcHdmiCec->cec_hal->emulated_device->type != DEVICE_TYPE_TV || logicalAddresses != 0)
  {
    VC_LOG_ERROR("HdmiCecAddLogicalAddress: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }
  //ADD Logical Address only for Sink device
  gvcHdmiCec->cec_hal->emulated_device->logical_address = logicalAddresses;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecRemoveLogicalAddress: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecRemoveLogicalAddress: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  //Remove Logical Address only for Sink device
  if(gvcHdmiCec->cec_hal->emulated_device->type != DEVICE_TYPE_TV || logicalAddresses != 0)
  {
    VC_LOG_ERROR("HdmiCecRemoveLogicalAddress: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }
  if(gvcHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
  {
    //Looks like logical address is already removed. 
    return HDMI_CEC_IO_ALREADY_REMOVED;
  }
  //Reset back to 0x0F
  gvcHdmiCec->cec_hal->emulated_device->logical_address = 0x0F;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecGetLogicalAddress(int handle, int* logicalAddress)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecGetLogicalAddress: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecGetLogicalAddress: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  if(logicalAddress == NULL)
  {
    VC_LOG_ERROR("HdmiCecGetLogicalAddress: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }
  *logicalAddress = gvcHdmiCec->cec_hal->emulated_device->logical_address;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecSetRxCallback: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecSetRxCallback: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  gvcHdmiCec->cec_hal->callbacks.rx_cb_func = cbfunc;
  gvcHdmiCec->cec_hal->callbacks.rx_cb_data = data;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void* data)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecSetTxCallback: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecSetTxCallback: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  gvcHdmiCec->cec_hal->callbacks.tx_cb_func = cbfunc;
  gvcHdmiCec->cec_hal->callbacks.tx_cb_data = data;
  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecTx(int handle, const unsigned char* buf, int len, int* result)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecTx: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecTx: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  if(buf == NULL || len <= 0 || result == NULL)
  {
    VC_LOG_ERROR("HdmiCecTx: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }
  if(gvcHdmiCec->cec_hal->emulated_device->type == DEVICE_TYPE_TV && gvcHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
  {
    //If Logical Address is not set for a sink device, we cannot transmit
    VC_LOG_ERROR("HdmiCecTx: Send failed");
    return HDMI_CEC_IO_SENT_FAILED;
  }

  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("HdmiCecTx: ");
  for (int i = 0; i < len; i++) {
    printf("%02X ", buf[i]);
  }
  VC_LOG("==========================");
  *result = HDMI_CEC_IO_SENT_BUT_NOT_ACKD;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecTxAsync(int handle, const unsigned char* buf, int len)
{
  if(gvcHdmiCec == NULL || gvcHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecTxAsync: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }
  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecTxAsync: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }
  if(buf == NULL || len <= 0)
  {
    VC_LOG_ERROR("HdmiCecTxAsync: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  if((gvcHdmiCec->cec_hal->emulated_device->type == DEVICE_TYPE_TV && gvcHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
            || gvcHdmiCec->cec_hal->callbacks.tx_cb_func == NULL)
  {
    //If Logical Address is not set for a sink device, we cannot transmit
    VC_LOG_ERROR("HdmiCecTxAsync: Send failed");
    return HDMI_CEC_IO_SENT_FAILED;
  }

  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("HdmiCecTxAsync: ");
  for (int i = 0; i < len; i++) {
    printf("%02X ", buf[i]);
  }
  VC_LOG("==========================");

  return HDMI_CEC_IO_SUCCESS;
}

