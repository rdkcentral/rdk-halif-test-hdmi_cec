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

#include "vcomponent_hdmi_cec.h"
#include "hdmi_cec_driver.h"
#include "vchdmicec_device.h"
#include "ut_log.h"
#include "ut_kvp_profile.h"


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

typedef enum
{
  CEC_EVENT_HOTPLUG = 1,
  CEC_EVENT_COMMAND,
  CEC_EVENT_CONFIG,
  CEC_EVENT_MAX
} vCHdmiCec_event_type_t;

typedef enum
{
  CEC_ACTIVE_SOURCE = 0x82,
  CEC_IMAGE_VIEW_ON = 0x04,
  CEC_TEXT_VIEW_ON = 0x0D,
  CEC_INACTIVE_SOURCE = 0x9D,
  CEC_REQUEST_ACTIVE_SOURCE = 0x85,
  CEC_STANDBY = 0x36,
  CEC_COMMAND_UNKNOWN = 0
} vCHdmiCec_command_t;

typedef struct
{
  bool connected;
  unsigned int port_id;
} vCHdmiCec_hotplug_event_t;


typedef struct
{
  unsigned char src_logical_addr;
  unsigned char dest_logical_addr;
  vCHdmiCec_command_t command;
} vCHdmiCec_command_event_t;


typedef struct
{
  vCHdmiCec_event_type_t type;
  union
  {
    vCHdmiCec_command_event_t cec_cmd;
    vCHdmiCec_hotplug_event_t hot_plug;
  };
} vCHdmiCec_event_t;


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
  //TODO
  // Eventing Queue, Thread for callback

} vCHdmiCec_hal_t;


/**Virtual Componenent Data types*/
typedef struct
{
  unsigned short cp_port;
  char* cp_path;
  vCHdmiCec_hal_t * cec_hal;
  ut_kvp_instance_t *profile_instance;
  bool bOpened;
} vCHdmiCec_t;


/*Global variables*/

/* TODO:
 * HdmiCec hal functions are taking in a int handle which is a problem in 64bit platforms like in Linux PCs.
 * When HdmiCecOpen provides a handle, the handle is lost in translation due to this explicit conversion of
 * pointer to int. Hence the subsequent HdmiCec functions dont have a reliable way to associate the integer
 * handle to a memory instance of the driver struct.
 * when the  int handle is upgraded to support a void * in the future. We should re-review if we want to use a Singleton or not at that time. 
 */
static vCHdmiCec_t* gVCHdmiCec = NULL;

const static strVal_t gPortStrVal [] = {
  { "in", (int)PORT_TYPE_INPUT  },
  { "out", (int)PORT_TYPE_OUTPUT },
  { "unknown", (int)PORT_TYPE_UNKNOWN }
};


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
  if(hal == NULL)
  {
    return;
  }

  vCHdmiCec_Device_DestroyMap(hal->devices_map);
    //Stop all events
    //Exit Eventing

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
    //TODO Open Control Plane
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

  profile_instance = gVCHdmiCec->profile_instance;
  assert(profile_instance != NULL);
  
  ut_kvp_getStringField(profile_instance, "hdmicec/emulated_device", emulated_device, MAX_OSD_NAME_LENGTH);

  cec->num_ports = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_ports");
  ports = (vCHdmiCec_port_info_t*) malloc(sizeof(vCHdmiCec_port_info_t) * cec->num_ports);
  assert(ports != NULL);

  LoadPortsInfo(profile_instance, ports, cec->num_ports);
  cec->ports = ports;

  //Setup Eventing and callback

  //Device Discovery and Network Topology
  cec->num_devices = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_devices");

  cec->devices_map = vCHdmiCec_Device_CreateMapFromProfile(profile_instance, "hdmicec/device_map/0");
  cec->emulated_device = vCHdmiCec_Device_Get(cec->devices_map, emulated_device);

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
    return HDMI_CEC_IO_ALREADY_REMOVED;
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

