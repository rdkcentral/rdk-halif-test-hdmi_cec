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

#include "vcomponent_hdmi_cec.h"
#include "hdmi_cec_driver.h"
#include "vcomponent_hdmi_cec_device.h"
#include "vcomponent_hdmi_cec_command.h"
#include "ut_kvp_profile.h"
#include "ut_control_plane.h"

#define MAX_QUEUE_SIZE 32
#define CONTROL_PLANE_PORT 8080

typedef enum
{
  CEC_MSG_TYPE_COMMAND = 0,
  CEC_MSG_TYPE_EVENT,
  CEC_MSG_TYPE_CONFIG,
  CEC_MSG_TYPE_EXIT_REQUESTED
} vCHdmiCec_msg_type_t;

typedef struct
{
  vCHdmiCec_msg_type_t type;
  char* message;
  uint32_t size;
} vCHdmiCec_message_t;

/**HDMI CEC HAL Data structures */
typedef struct
{
  unsigned short id;
  unsigned int physical_address;
  vCHdmiCec_port_type_t type;
  bool cec_supported;
  bool arc_supported;
} vCHdmiCec_port_info_t;

typedef enum
{
  HAL_STATE_CLOSED = 0,
  HAL_STATE_OPEN,
  HAL_STATE_READY
} vCHdmiCec_hal_state_t;

typedef struct
{
  HdmiCecRxCallback_t rx_cb_func;
  void* rx_cb_data;
  HdmiCecTxCallback_t tx_cb_func;
  void* tx_cb_data;
} vCHdmiCec_callbacks_t;

typedef struct
{
  vCHdmiCec_hal_state_t state;
  struct vCHdmiCec_device_info_t* emulated_device;
  int num_ports;
  vCHdmiCec_port_info_t *ports;
  int num_devices;
  struct vCHdmiCec_device_info_t* devices_map;
  vCHdmiCec_callbacks_t callbacks;
  vCHdmiCec_logical_address_pool_t address_pool;

  pthread_t msg_handler_thread;
  uint32_t msg_count;
  vCHdmiCec_message_t msg_queue[MAX_QUEUE_SIZE];
  pthread_mutex_t msg_queue_mutex;
  pthread_cond_t msg_queue_condition;
  volatile bool exit_request;
} vCHdmiCec_hal_t;

/**Virtual Componenent Data types*/
typedef struct
{
  vCHdmiCec_hal_t * cec_hal;
  ut_kvp_instance_t *profile_instance;
  ut_controlPlane_instance_t *cp_instance;
  bool bOpened;
} vCHdmiCec_t;


/*Global variables*/
static vCHdmiCec_t* gVCHdmiCec = NULL;

const static strVal_t gPortStrVal [] = {
  { "in", (int)PORT_TYPE_INPUT  },
  { "out", (int)PORT_TYPE_OUTPUT },
  { "unknown", (int)PORT_TYPE_UNKNOWN }
};


static void EnqueueMessage(vCHdmiCec_message_t *msg, vCHdmiCec_hal_t *hal );
static vCHdmiCec_message_t* DequeueMessage(vCHdmiCec_hal_t *hal);
static void ProcessMsg( char *key, ut_kvp_instance_t *instance, void* user_data);
static void* MessageHandler(void *data);


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
  status = ut_kvp_openMemory(kvpInstance, msg, size);
  if (status != UT_KVP_STATUS_SUCCESS)
  {
      VC_LOG_ERROR("ut_kvp_openMemory() - Read Failure\n");
      ut_kvp_destroyInstance(kvpInstance);
      return NULL;
  }
  return kvpInstance;
}

static void ParseCommand(vCHdmiCec_hal_t *hal, char* cmd, int size, vCHdmiCec_command_t *cec_cmd)
{
  char str[UT_KVP_MAX_ELEMENT_SIZE];
  vCHdmiCec_opcode_t opcode = CEC_OPCODE_UNKNOWN;
  struct vCHdmiCec_device_info_t *src, *dest;
  vCHdmiCec_logical_address_t la;

  if(cec_cmd == NULL)
  {
    return;
  }

  vCHdmiCec_Command_Clear(cec_cmd);
  ut_kvp_instance_t *kvpInstance = KVPInstanceOpen(cmd, size);
  ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_MSG_COMMAND, str, UT_KVP_MAX_ELEMENT_SIZE);
  opcode = vCHdmiCec_Command_GetOpCode(str);
  if(opcode == CEC_OPCODE_UNKNOWN)
  {
    VC_LOG_ERROR("ParseCommand: Opcode[%s] Unknown", str);
    return;
  }
  VC_LOG("ParseCommand: Opcode[%s]", str);

  ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_CMD_INITIATOR, str, UT_KVP_MAX_ELEMENT_SIZE);
  src = vCHdmiCec_Device_Get(hal->devices_map, str);
  if(src == NULL)
  {
    VC_LOG_ERROR("ParseCommand: Initiator[%s] Unknown", str);
    return;
  }
  VC_LOG("ParseCommand: Initiator[%s] ", str);
  ut_kvp_getStringField(kvpInstance, CEC_MSG_PREFIX"/"CEC_CMD_DESTINATION, str, UT_KVP_MAX_ELEMENT_SIZE);
  if(strcmp(str, CEC_BROADCAST) != 0)
  {
    dest = vCHdmiCec_Device_Get(hal->devices_map, str);
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
  vCHdmiCec_Command_Format(cec_cmd, src->logical_address, la, opcode);

  switch (opcode)
  {
    case CEC_ACTIVE_SOURCE:
    case CEC_INACTIVE_SOURCE:
    {
      uint8_t buf[4];
      buf[0] = (src->physical_address >> 12) & 0x0F;
      buf[1] = (src->physical_address >> 8) & 0x0F;
      buf[2] = (src->physical_address >> 4) & 0x0F;
      buf[3] = src->physical_address & 0x0F;
      vCHdmiCec_Command_PushBackArray(cec_cmd, buf, sizeof(buf));
    }
    break;

    default:
    {
    }
    break;
  }
  ut_kvp_destroyInstance(kvpInstance);
}


static void ProcessMsg( char *key, ut_kvp_instance_t *instance, void* user_data)
{
  vCHdmiCec_message_t msg;
  vCHdmiCec_t *vc = (vCHdmiCec_t*) user_data;

  if(vc == NULL || vc->cec_hal == NULL || vc->cec_hal->state != HAL_STATE_READY)
  {
    return;
  }

  if(strstr(key, CEC_MSG_COMMAND))
  {
    msg.type = CEC_MSG_TYPE_COMMAND;
  }
  else if(strstr(key, CEC_MSG_EVENT))
  {
    msg.type = CEC_MSG_TYPE_EVENT;
  }
  else if(strstr(key, CEC_MSG_CONFIG))
  {
    msg.type = CEC_MSG_TYPE_CONFIG;
  }
  else
  {
    VC_LOG_ERROR("ProcessMsg: Unknown Message Type [%s]", key);
    return;
  }

  msg.message = ut_kvp_getData(instance);
  msg.size = strlen(msg.message);
  EnqueueMessage(&msg, vc->cec_hal);
}

static void EnqueueMessage(vCHdmiCec_message_t *msg, vCHdmiCec_hal_t *hal )
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

static vCHdmiCec_message_t* DequeueMessage(vCHdmiCec_hal_t *hal)
{
    vCHdmiCec_message_t *msg;

    pthread_mutex_lock(&hal->msg_queue_mutex);
    while (hal->msg_count == 0)
    {
        pthread_cond_wait(&hal->msg_queue_condition, &hal->msg_queue_mutex);
    }
    msg = &hal->msg_queue[0]; //Get the top
    //Adjust the queue
    for (int i = 0; i < hal->msg_count - 1; i++)
    {
        hal->msg_queue[i] = hal->msg_queue[i + 1];
    }
    hal->msg_count--;
    pthread_mutex_unlock(&hal->msg_queue_mutex);
    return msg;
}

/*
static void ResetMessage(vCHdmiCec_message_t* msg)
{
  msg->size = 0;
  if(msg->message != NULL)
  {
    free(msg->message);
    msg->message = NULL;
  }
}
*/
static void* MessageHandler(void *data)
{
  vCHdmiCec_hal_t *hal = (vCHdmiCec_hal_t *)data;
  vCHdmiCec_message_t *msg;

  if (hal == NULL)
  {
    return NULL;
  }

  while (!hal->exit_request)
  {
    msg = DequeueMessage(hal);

    switch (msg->type)
    {
      case CEC_MSG_TYPE_EXIT_REQUESTED:
      {
        VC_LOG("EXIT REQUESTED in MessageHandler\n");
        hal->exit_request = true;
      }
      break;

      case CEC_MSG_TYPE_COMMAND:
      {
        vCHdmiCec_command_t cmd;
        uint32_t len;
        uint8_t cec_data[VCHDMICEC_MAX_DATA_SIZE];
        ParseCommand(hal, msg->message, msg->size, &cmd);
        len = vCHdmiCec_Command_GetRawBytes(&cmd, cec_data, VCHDMICEC_MAX_DATA_SIZE);
        if(hal->callbacks.rx_cb_func != NULL)
        {
          hal->callbacks.rx_cb_func((int)hal, hal->callbacks.rx_cb_data, cec_data, len);
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

      default:
      {

      }
      break;
    }
    //ResetMessage(msg);
  }
  return NULL;
}

void LoadPortsInfo (ut_kvp_instance_t* instance, vCHdmiCec_port_info_t* ports, unsigned int nPorts)
{
  char *prefix = "hdmicec/ports/";
  char tmp[256];

  if (instance == NULL || ports == NULL || nPorts <= 0)
  {
    return;
  }

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
    ports[i].type = vCHdmiCec_GetValue(gPortStrVal, sizeof(gPortStrVal)/sizeof(strVal_t), type, (int)PORT_TYPE_UNKNOWN);

    strcpy(tmp + strlen(prefix) + length, "/cec_supported");
    ports[i].cec_supported = ut_kvp_getBoolField(instance, tmp);

    strcpy(tmp + strlen(prefix) + length, "/arc_supported");
    ports[i].arc_supported = ut_kvp_getBoolField(instance, tmp);

  }
}

void TeardownHal (vCHdmiCec_hal_t* hal)
{
  vCHdmiCec_message_t msg = {0};
  if(hal == NULL)
  {
    return;
  }

  if ( hal->msg_handler_thread )
  {
    memset(&msg, 0, sizeof(msg));
    msg.type = CEC_MSG_TYPE_EXIT_REQUESTED;
    EnqueueMessage(&msg, hal);
    if (pthread_join(hal->msg_handler_thread, NULL) != 0)
    {
      VC_LOG_ERROR("Failed to join msg_handler_thread from instance\n");
    }
  }
  hal->msg_handler_thread = 0;
  vCHdmiCec_Device_DestroyMap(hal->devices_map);

  if(hal->ports)
  {
    free (hal->ports);
  }
  hal->callbacks.rx_cb_func = NULL;
  hal->callbacks.tx_cb_func = NULL;
  free(hal);
  
}

vComponent_HdmiCec_t* vComponent_HdmiCec_Initialize( void )
{
  vCHdmiCec_t *result = NULL;
  result = (vCHdmiCec_t*)malloc(sizeof(vCHdmiCec_t));
  if(result == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Out of memory");
    return NULL;
  }
  result->profile_instance = ut_kvp_createInstance();
  assert(result->profile_instance != NULL);
  result->cec_hal = NULL;
  result->bOpened = false;
  result->cp_instance = NULL;

  gVCHdmiCec = result;
  return (vComponent_HdmiCec_t *)result;
}

vComponent_HdmiCec_Status_t vComponent_HdmiCec_Open( vComponent_HdmiCec_t* pVCHdmiCec, char* pProfilePath, bool enableCPMsgs )
{

  ut_kvp_status_t status;
  vCHdmiCec_t* vCHdmiCec = (vCHdmiCec_t*)pVCHdmiCec;

  if(vCHdmiCec == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Invalid handle param");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }
  if(gVCHdmiCec == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Deinitialize: Already Deinitialized");
    return VC_HDMICEC_STATUS_NOT_INITIALIZED;
  }
  if(vCHdmiCec != gVCHdmiCec)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Invalid handle");
    return VC_HDMICEC_STATUS_INVALID_HANDLE;
  }

  if(vCHdmiCec->bOpened)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Already Opened");
    return VC_HDMICEC_STATUS_ALREADY_OPENED;
  }
  
if(pProfilePath == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Invalid Profile path");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }

  status = ut_kvp_open(vCHdmiCec->profile_instance, pProfilePath);
  if(status != UT_KVP_STATUS_SUCCESS)
  {
    VC_LOG_ERROR("ut_kvp_open: status: %d", status);
    assert(status == UT_KVP_STATUS_SUCCESS);
    return VC_HDMICEC_STATUS_PROFILE_READ_ERROR;
  }

  if(enableCPMsgs)
  {
    vCHdmiCec->cp_instance = UT_ControlPlane_Init(CONTROL_PLANE_PORT);
    assert(vCHdmiCec->cp_instance != NULL);
    UT_ControlPlane_RegisterCallbackOnMessage(gVCHdmiCec->cp_instance, "hdmicec/command", &ProcessMsg, (void*) vCHdmiCec);
    UT_ControlPlane_Start(vCHdmiCec->cp_instance);
  }
  vCHdmiCec->bOpened = true;
  return VC_HDMICEC_STATUS_SUCCESS;
}

vComponent_HdmiCec_Status_t vComponent_HdmiCec_Close( vComponent_HdmiCec_t* pVCHdmiCec )
{
  vCHdmiCec_t* vCHdmiCec = (vCHdmiCec_t*)pVCHdmiCec;

  if(vCHdmiCec == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Deinitialize: Invalid handle param");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }

  if(vCHdmiCec != gVCHdmiCec)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Deinitialize: Invalid handle");
    return VC_HDMICEC_STATUS_INVALID_HANDLE;
  }

  if(gVCHdmiCec == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Deinitialize: Already Deinitialized");
    return VC_HDMICEC_STATUS_NOT_INITIALIZED;
  }

  if(!vCHdmiCec->bOpened)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Not Opened");
    return VC_HDMICEC_STATUS_NOT_OPENED;
  }

  //Should we enforce HdmiCecClose to be called before?
  if(vCHdmiCec->cec_hal != NULL && vCHdmiCec->cec_hal->state != HAL_STATE_CLOSED)
  {
    HdmiCecClose((int)vCHdmiCec->cec_hal);
  }
  vCHdmiCec->bOpened = false;

  return VC_HDMICEC_STATUS_SUCCESS;
}


vComponent_HdmiCec_Status_t vComponent_HdmiCec_Deinitialize(vComponent_HdmiCec_t *pVCHdmiCec)
{
  vCHdmiCec_t* vCHdmiCec = (vCHdmiCec_t*)pVCHdmiCec;

  if(vCHdmiCec == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Deinitialize: Invalid handle param");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }

  if(vCHdmiCec != gVCHdmiCec)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Deinitialize: Invalid handle");
    return VC_HDMICEC_STATUS_INVALID_HANDLE;
  }

  if(gVCHdmiCec == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Deinitialize: Already Deinitialized");
    return VC_HDMICEC_STATUS_NOT_INITIALIZED;
  }

  if(vCHdmiCec->bOpened)
  {
    vComponent_HdmiCec_Close(pVCHdmiCec);
  }
  if(vCHdmiCec->cp_instance != NULL)
  {
    UT_ControlPlane_Exit(vCHdmiCec->cp_instance);
  }
  ut_kvp_destroyInstance(vCHdmiCec->profile_instance);
  free(vCHdmiCec);
  gVCHdmiCec = NULL;
  return VC_HDMICEC_STATUS_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecOpen(int* handle)
{
  char emulated_device[MAX_OSD_NAME_LENGTH];
  vCHdmiCec_hal_t* cec;
  ut_kvp_instance_t *profile_instance;
  vCHdmiCec_port_info_t* ports;

  if(handle == NULL)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }

  if(gVCHdmiCec == NULL)
  {
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(gVCHdmiCec->cec_hal != NULL)
  {
    return HDMI_CEC_IO_ALREADY_OPEN;
  }

  cec = (vCHdmiCec_hal_t*)malloc(sizeof(vCHdmiCec_hal_t));
  if(cec == NULL) 
  {
    VC_LOG_ERROR( "HdmiCecOpen: Out of Memory" );
    return HDMI_CEC_IO_GENERAL_ERROR;
  }

  memset(cec, 0, sizeof(vCHdmiCec_hal_t));

  profile_instance = gVCHdmiCec->profile_instance;
  assert(profile_instance != NULL);
  
  ut_kvp_getStringField(profile_instance, "hdmicec/emulated_device", emulated_device, MAX_OSD_NAME_LENGTH);

  cec->num_ports = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_ports");
  ports = (vCHdmiCec_port_info_t*) malloc(sizeof(vCHdmiCec_port_info_t) * cec->num_ports);
  assert(ports != NULL);

  LoadPortsInfo(profile_instance, ports, cec->num_ports);
  cec->ports = ports;

  //Setup Eventing and callback
  cec->exit_request = false;
  pthread_mutex_init( &cec->msg_queue_mutex, NULL );
  pthread_cond_init( &cec->msg_queue_condition, NULL );
  pthread_create(&cec->msg_handler_thread, NULL, MessageHandler, (void*) cec );


  //Device Discovery and Network Topology
  cec->num_devices = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_devices");

  cec->devices_map = vCHdmiCec_Device_CreateMapFromProfile(profile_instance, "hdmicec/device_map/0");
  cec->emulated_device = vCHdmiCec_Device_Get(cec->devices_map, emulated_device);
  if(cec->emulated_device == NULL)
  {
    VC_LOG_ERROR("HdmiCecOpen: Couldnt load emulated device info");
    assert(cec->emulated_device != NULL);
    TeardownHal(cec);
    return HDMI_CEC_IO_GENERAL_ERROR;
  }
  vCHdmiCec_Device_InitLogicalAddressPool(&cec->address_pool);
  vCHdmiCec_Device_AllocatePhysicalLogicalAddresses(cec->devices_map, cec->emulated_device, &cec->address_pool);

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

  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("Emulated Device               : %s", cec->emulated_device->osd_name);
  VC_LOG("Number of Ports               : %d", cec->num_ports);
  VC_LOG("Number of devices in Network  : %d", cec->num_devices);
  VC_LOG("===========================");

  vCHdmiCec_Device_PrintMap(cec->devices_map, 0);
  VC_LOG("=================================");

  *handle = (int) cec;
  gVCHdmiCec->cec_hal = cec;
  cec->state = HAL_STATE_READY;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecClose(int handle)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecClose: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecClose: Invalid Handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }

  TeardownHal(gVCHdmiCec->cec_hal);
  gVCHdmiCec->cec_hal = NULL;

  return HDMI_CEC_IO_SUCCESS;
}


HDMI_CEC_STATUS HdmiCecGetPhysicalAddress(int handle, unsigned int* physicalAddress)
{

  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
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

  *physicalAddress = gVCHdmiCec->cec_hal->emulated_device->physical_address;
  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecAddLogicalAddress: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecAddLogicalAddress: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }


  if(gVCHdmiCec->cec_hal->emulated_device->type != DEVICE_TYPE_TV || logicalAddresses != 0)
  {
    VC_LOG_ERROR("HdmiCecAddLogicalAddress: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  //ADD Logical Address only for Sink device
  gVCHdmiCec->cec_hal->emulated_device->logical_address = logicalAddresses;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{

  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
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
  if(gVCHdmiCec->cec_hal->emulated_device->type != DEVICE_TYPE_TV || logicalAddresses != 0)
  {
    VC_LOG_ERROR("HdmiCecRemoveLogicalAddress: Invalid Argument");
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  if(gVCHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
  {
    //Looks like logical address is already removed. 
    VC_LOG_ERROR("HdmiCecRemoveLogicalAddress: Logical Address not added");
    return HDMI_CEC_IO_NOT_ADDED;
  }
  //Reset back to 0x0F
  gVCHdmiCec->cec_hal->emulated_device->logical_address = 0x0F;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecGetLogicalAddress(int handle, int* logicalAddress)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
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

  *logicalAddress = gVCHdmiCec->cec_hal->emulated_device->logical_address;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecSetRxCallback: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecSetRxCallback: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }


  gVCHdmiCec->cec_hal->callbacks.rx_cb_func = cbfunc;
  gVCHdmiCec->cec_hal->callbacks.rx_cb_data = data;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void* data)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    VC_LOG_ERROR("HdmiCecSetTxCallback: Not Opened");
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    VC_LOG_ERROR("HdmiCecSetTxCallback: Invalid handle");
    return HDMI_CEC_IO_INVALID_HANDLE;
  }

  gVCHdmiCec->cec_hal->callbacks.tx_cb_func = cbfunc;
  gVCHdmiCec->cec_hal->callbacks.tx_cb_data = data;
  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecTx(int handle, const unsigned char* buf, int len, int* result)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
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

  if(gVCHdmiCec->cec_hal->emulated_device->type == DEVICE_TYPE_TV && gVCHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
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
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
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

  if((gVCHdmiCec->cec_hal->emulated_device->type == DEVICE_TYPE_TV && gVCHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
            || gVCHdmiCec->cec_hal->callbacks.tx_cb_func == NULL)
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

