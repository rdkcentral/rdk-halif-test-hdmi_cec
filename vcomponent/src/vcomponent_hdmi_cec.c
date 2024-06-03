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

#include "hdmi_cec_driver.h"
#include "vcomponent_hdmi_cec.h"
#include "ut_log.h"
#include "ut_kvp_profile.h"
#include <assert.h>

#define VC_LOG(format, ...)  UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_YELLOW"vcHdmiCec   "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)


/**HDMI CEC HAL Data structures */

#define MAX_OSD_NAME_LENGTH 16

typedef enum {
  DEVICE_TYPE_TV = 0,
  DEVICE_TYPE_PLAYBACK,
  DEVICE_TYPE_AUDIO_SYSTEM,
  DEVICE_TYPE_RECORDER,
  DEVICE_TYPE_TUNER,
  DEVICE_TYPE_RESERVED,
  DEVICE_TYPE_UNKNOWN,
  DEVICE_TYPE_MAX
} vCHdmiCec_device_type_t;

typedef enum
{
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
  LOGICAL_ADDRESS_BROADCAST        = 15
} vCHdmiCec_logical_address_t;

typedef enum {
  CEC_VERSION_UNKNOWN = 0,
  CEC_VERSION_1_2 = 1,
  CEC_VERSION_1_2A = 2,
  CEC_VERSION_1_3 = 3,
  CEC_VERSION_1_3A = 4,
  CEC_VERSION_1_4 = 5,
  CEC_VERSION_2_0 = 6,
  CEC_VERSION_MAX = 7
} vCHdmiCec_version_t;

typedef enum {
  PORT_TYPE_INPUT = 0,
  PORT_TYPE_OUTPUT,
  PORT_TYPE_MAX
} vCHdmiCec_port_type_t;

typedef enum {
  POWER_STATUS_ON = 0,
  POWER_STATUS_STANDBY,
  POWER_STATUS_OFF,
  POWER_STATUS_MAX
} vCHdmiCec_power_status_t;

typedef struct {
  unsigned short id;
  unsigned int physical_address;
  vCHdmiCec_port_type_t type;
  bool cec_supported;
  bool arc_supported;
} vCHdmiCec_port_info_t;

typedef enum {
  HAL_STATE_CLOSED = 0,
  HAL_STATE_OPEN,
  HAL_STATE_READY
} vCHdmiCec_hal_state_t;

typedef enum {
  CEC_EVENT_HOTPLUG = 1,
  CEC_EVENT_COMMAND,
  CEC_EVENT_CONFIG,
  CEC_EVENT_MAX
} vCHdmiCec_event_type_t;

typedef struct {
  bool connected;
  unsigned int port_id;
} vCHdmiCec_hotplug_event_t;

typedef enum{
  CEC_ACTIVE_SOURCE = 0x82,
  CEC_IMAGE_VIEW_ON = 0x04,
  CEC_TEXT_VIEW_ON = 0x0D,
  CEC_INACTIVE_SOURCE = 0x9D,
  CEC_REQUEST_ACTIVE_SOURCE = 0x85,
  CEC_STANDBY = 0x36,
  CEC_UNKNOWN = 0
} vCHdmiCec_command_t;


typedef struct {
  unsigned char src_logical_addr;
  unsigned char dest_logical_addr;
  vCHdmiCec_command_t command;
} vCHdmiCec_command_event_t;


typedef struct {
  vCHdmiCec_event_type_t type;
  union {
    vCHdmiCec_command_event_t cec_cmd;
    vCHdmiCec_hotplug_event_t hot_plug;
  };
} vCHdmiCec_event_t;

struct vCHdmiCec_device_info_t {
  /*Variables to manage a non-binary tree of devices*/
  unsigned int number_children;
  struct vCHdmiCec_device_info_t* parent;
  struct vCHdmiCec_device_info_t* first_child;
  struct vCHdmiCec_device_info_t* next_sibling;

  /*Device Information*/
  vCHdmiCec_device_type_t type;
  vCHdmiCec_version_t version;
  unsigned int physical_address;
  unsigned int logical_address;
  bool active_source;
  unsigned int vendor_id;
  vCHdmiCec_power_status_t power_status;
  char osd_name[MAX_OSD_NAME_LENGTH];
} vCHdmiCec_device_info_t;

typedef struct {
  HdmiCecRxCallback_t rx_cb_func;
  void* rx_cb_data;
  HdmiCecTxCallback_t tx_cb_func;
  void* tx_cb_data;
} vCHdmiCec_callbacks_t;

typedef struct {
  vCHdmiCec_hal_state_t state;
  struct vCHdmiCec_device_info_t* emulated_device;
  int num_ports;
  vCHdmiCec_port_info_t *ports;
  int num_devices;
  struct vCHdmiCec_device_info_t* devices_map;

  vCHdmiCec_callbacks_t callbacks;
  //TODO
  // Eventing Queue, Thread for callback

} vCHdmiCec_hal_t;


/**Emulator Data types*/
typedef struct {
  unsigned short cp_port;
  char* cp_path;
  vCHdmiCec_hal_t * cec_hal;
} vCHdmiCec_t;

static vCHdmiCec_t* gVCHdmiCec = NULL;

struct  {
  vCHdmiCec_device_type_t val;
  char* str_type;
} gDIMap [] = {
  { DEVICE_TYPE_TV, "TV" },
  { DEVICE_TYPE_PLAYBACK, "PlaybackDevice" },
  { DEVICE_TYPE_AUDIO_SYSTEM, "AudioSystem" },
  { DEVICE_TYPE_RECORDER, "RecordingDevice" },
  { DEVICE_TYPE_TUNER, "Tuner" },
  { DEVICE_TYPE_RESERVED, "Reserved" },
  { DEVICE_TYPE_UNKNOWN, "Unknown"}
};

const static struct  {
    char* name;
    unsigned int code;
  } gVCMap [] = {
    {"TOSHIBA", 0x000039},
    {"SAMSUNG", 0x0000F0},
    {"DENON", 0x0005CD},
    {"MARANTZ", 0x000678},
    {"LOEWE", 0x000982},
    {"ONKYO", 0x0009B0},
    {"MEDION", 0x000CB8},
    {"TOSHIBA2", 0x000CE7},
    {"APPLE", 0x0010FA},
    {"HARMAN_KARDON2", 0x001950},
    {"GOOGLE", 0x001A11},
    {"AKAI", 0x0020C7},
    {"AOC", 0x002467},
    {"PANASONIC", 0x008045},
    {"PHILIPS", 0x00903E},
    {"DAEWOO", 0x009053},
    {"YAMAHA", 0x00A0DE},
    {"GRUNDIG", 0x00D0D5},
    {"PIONEER", 0x00E036},
    {"LG", 0x00E091},
    {"SHARP", 0x08001F},
    {"SONY", 0x080046},
    {"BROADCOM", 0x18C086},
    {"SHARP2", 0x534850},
    {"VIZIO", 0x6B746D},
    {"BENQ", 0x8065E9},
    {"HARMAN_KARDON", 0x9C645E},
    {"UNKNOWN", 0},
  };

const static struct  {
  vCHdmiCec_power_status_t status;
  char* str;
} gPSMap [] = {
  { POWER_STATUS_ON, "on" },
  { POWER_STATUS_OFF, "off" },
  { POWER_STATUS_STANDBY, "standby" }
};

static char* GetDeviceTypeStr(vCHdmiCec_device_type_t type)
{
  char* result = NULL;
  for (int i = 0;  i < sizeof (gDIMap) / sizeof (gDIMap[0]);  ++i)
  {
    if (type == gDIMap[i].val)
    {
        result = gDIMap[i].str_type;
        break;
    }
  }
  return result;
}

static vCHdmiCec_device_type_t GetDeviceType(char *type)
{

  vCHdmiCec_device_type_t result = DEVICE_TYPE_TV;

  for (int i = 0;  i < sizeof (gDIMap) / sizeof (gDIMap[0]);  ++i)
  {
    if (!strcmp (type, gDIMap[i].str_type))
    {
        result = gDIMap[i].val;   
        break; 
    }
  }

  return result;
}

static char* GetVendorName(int code)
{
  char* result = NULL;

  for (int i = 0;  i < sizeof (gVCMap) / sizeof (gVCMap[0]);  ++i)
  {
    if (code == gVCMap[i].code)
    {
        result = gVCMap[i].name;   
        break; 
    }
  }

  return result;
}

static unsigned int GetVendorCode(char *name)
{
  unsigned int result = 0;

  for (int i = 0;  i < sizeof (gVCMap) / sizeof (gVCMap[0]);  ++i)
  {
    if (!strcmp (name, gVCMap[i].name))
    {
        result = gVCMap[i].code;   
        break; 
    }
  }

  return result;
}

static char* GetDevicePowerStatusStr(vCHdmiCec_power_status_t status)
{
  char* result = NULL;

  for (int i = 0;  i < sizeof (gPSMap) / sizeof (gPSMap[0]);  ++i)
  {
    if (status == gPSMap[i].status)
    {
        result = gPSMap[i].str;
        break; 
    }
  }
  return result;
}

static vCHdmiCec_power_status_t GetDevicePowerStatus(char *name)
{
  vCHdmiCec_power_status_t result = POWER_STATUS_OFF;

  for (int i = 0;  i < sizeof (gPSMap) / sizeof (gPSMap[0]);  ++i)
  {
    if (!strcmp (name, gPSMap[i].str))
    {
        result = gPSMap[i].status;
        break; 
    }
  }

  return result;
}


static vCHdmiCec_port_type_t GetPortType(char *type)
{
  vCHdmiCec_port_type_t result = PORT_TYPE_OUTPUT;

  if(!strncmp(type, "in", sizeof("in")))
  {
    result = PORT_TYPE_INPUT;
  }
  return result;
}

void LoadPortsInfo (ut_kvp_instance_t* instance, vCHdmiCec_port_info_t* ports, unsigned int nPorts)
{
  if (ports != NULL && nPorts > 0)
  {
    char *prefix = "hdmicec/ports/";
    char tmp[256];

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
      ports[i].type = GetPortType(type);

      strcpy(tmp + strlen(prefix) + length, "/cec_supported");
      ports[i].cec_supported = ut_kvp_getBoolField(instance, tmp);

      strcpy(tmp + strlen(prefix) + length, "/arc_supported");
      ports[i].arc_supported = ut_kvp_getBoolField(instance, tmp);

    }
  }
}

void ResetDevice(struct vCHdmiCec_device_info_t* device)
{
  if(device == NULL)
  {
    assert(device != NULL);
    return;
  }
  device->active_source = false;
  device->logical_address = 0x0F;
  device->physical_address = 0x0F;
  device->parent = NULL;
  device->first_child = NULL;
  device->next_sibling = NULL;
  device->vendor_id = 0;
  device->number_children = 0;
}


/* Load the device info into the passed in vCHdmiCec_device_info_t*
* prefix can be 
*   "hdmicec/device_map/0"
*   "hdmicec/device_map/0/children/0"
*   "hdmicec/device_map/0/children/1"
*   "hdmicec/device_map/0/children/1/children/0"
*/
void LoadDeviceInfo (ut_kvp_instance_t* instance, char* prefix, struct vCHdmiCec_device_info_t* device)
{
  char tmp[strlen(prefix) + 64];
  char type[32];

  if (device == NULL || instance == NULL || prefix == NULL)
  {
    assert(device != NULL);
    assert(instance != NULL);
    assert(prefix != NULL);
    return;
  }
  strcpy(tmp, prefix);

  VC_LOG("LoadDeviceInfo: Loading %s", tmp);
  strcpy(tmp + strlen(prefix), "/name");
  ut_kvp_getStringField(instance, tmp, device->osd_name, MAX_OSD_NAME_LENGTH);

  strcpy(tmp + strlen(prefix), "/active_source");
  device->active_source = ut_kvp_getBoolField(instance, tmp);

  strcpy(tmp + strlen(prefix), "/pwr_status");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->power_status = GetDevicePowerStatus(type);

  strcpy(tmp + strlen(prefix), "/version");
  device->version = (vCHdmiCec_version_t) ut_kvp_getUInt32Field(instance, tmp);

  strcpy(tmp + strlen(prefix), "/vendor");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->vendor_id = GetVendorCode(type);

  strcpy(tmp + strlen(prefix), "/type");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->type = GetDeviceType(type);

  strcpy(tmp + strlen(prefix), "/number_children");
  device->number_children = (vCHdmiCec_version_t) ut_kvp_getUInt32Field(instance, tmp);
}


void InsertChild(struct vCHdmiCec_device_info_t* parent, struct vCHdmiCec_device_info_t* child)
{
  if (parent == NULL || child == NULL)
  {
    assert(parent != NULL);
    assert(child != NULL);
    return;
  }

  child->parent = parent;
  if(parent->first_child == NULL)
  {
    parent->first_child = child;
  }
  else
  {
    child->next_sibling = parent->first_child;
    parent->first_child = child;
  }
}

void RemoveChild(struct vCHdmiCec_device_info_t* map, char* name)
{
  if(map == NULL || name == NULL)
  {
    return;
  }
  if(map->first_child == NULL)
  {
    return;
  }
  struct vCHdmiCec_device_info_t* tmp = map->first_child;
  struct vCHdmiCec_device_info_t* prev = NULL;

  //Lets traverse to get to the device
  while(tmp != NULL && strcmp(tmp->osd_name, name)!=0)
  {
    prev = tmp;
    tmp = tmp->next_sibling;
  }

  //We may have found the device
  if(tmp == NULL)
  {
    //We didnt find a device with that name
    return;
  }
  //We found the device. Lets adjust the map
  if(prev == NULL)
  {
    //If this is the root device.
    map->first_child = tmp->next_sibling;
  }
  else
  {
    prev->next_sibling = tmp->next_sibling;
  }
  free(tmp);
}

struct vCHdmiCec_device_info_t* CreateDeviceMap (ut_kvp_instance_t* instance, char* profile_prefix)
{
  if(instance == NULL || profile_prefix == NULL )
  {
    assert(instance != NULL);
    assert(profile_prefix != NULL);
    return NULL;
  }

  struct vCHdmiCec_device_info_t *device = (struct vCHdmiCec_device_info_t *)malloc(sizeof(struct vCHdmiCec_device_info_t));
  assert(device != NULL);
  ResetDevice(device);
  LoadDeviceInfo(instance, profile_prefix, device);
  for(int j=0; j < device->number_children; j++)
  {
    char tmp[strlen(profile_prefix) + 128];
    strcpy(tmp, profile_prefix);
    strcpy(tmp + strlen(profile_prefix), "/children/");
    int length = snprintf( NULL, 0, "%d", j );
    snprintf( tmp + strlen(tmp) , length + 1, "%d", j );

    struct vCHdmiCec_device_info_t *child = CreateDeviceMap(instance, tmp);
    assert(child != NULL);
    InsertChild(device, child);
  }
  return device;

}

void PrintDeviceMap(struct vCHdmiCec_device_info_t* map)
{

  if(map == NULL)
  {
    return;
  }

  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("Device                        : %s", map->osd_name);
  VC_LOG("Type                          : %s", GetDeviceTypeStr(map->type));
  VC_LOG("Pwr Status                    : %s", GetDevicePowerStatusStr(map->power_status));
  VC_LOG("------------------------");

  PrintDeviceMap(map->first_child);
  PrintDeviceMap(map->next_sibling);
}

struct vCHdmiCec_device_info_t* GetDeviceByName(struct vCHdmiCec_device_info_t* map, char* name)
{
  if(map == NULL || name == NULL)
  {
    return NULL;
  }
  if(strcmp(map->osd_name, name) == 0)
  {
    return map;
  }
  struct vCHdmiCec_device_info_t* device = GetDeviceByName(map->first_child, name);
  if(device != NULL)
  {
    return device;
  }
  return GetDeviceByName(map->next_sibling, name);
}

void DestroyDevicesMap(struct vCHdmiCec_device_info_t* map)
{
  if(map == NULL)
  {
    return;
  }

  DestroyDevicesMap(map->first_child);
  DestroyDevicesMap(map->next_sibling);

  free(map);
}

void TeardownHal (vCHdmiCec_hal_t* hal)
{
  if(hal == NULL)
  {
    return;
  }

  DestroyDevicesMap(hal->devices_map);
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


vComponent_HdmiCec_Status vComponent_HdmiCec_Initialize( char* pProfilePath, unsigned short cpPort, char* pCPUrl, vComponent_HdmiCec_t** handle )
{
  vCHdmiCec_t *result = NULL;
  ut_kvp_status_t status;
  
  if(handle == NULL)
  {
    VC_LOG("vComponent_HdmiCec_Initialize: Invalid handle param");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }
  if(pProfilePath == NULL)
  {
    VC_LOG("vComponent_HdmiCec_Initialize: Invalid Profile path");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }

  if(gVCHdmiCec != NULL)
  {
    //Already initialised
    return VC_HDMICEC_STATUS_ALREADY_INITIALIZED;
  }

  result = (vCHdmiCec_t*)malloc(sizeof(vCHdmiCec_t));
  if(result == NULL)
  {
    VC_LOG("vComponent_HdmiCec_Initialize: Out of memory");
    return VC_HDMICEC_STATUS_OUT_OF_MEMORY;
  }
  result->cp_path = pCPUrl;
  result->cp_port = cpPort;
  result->cec_hal = NULL;

  status = ut_kvp_profile_open(pProfilePath);
  if(status != UT_KVP_STATUS_SUCCESS)
  {
    VC_LOG("ut_kvp_profile_open: status: %d", status);
    assert(status == UT_KVP_STATUS_SUCCESS);
    free(result);
    return VC_HDMICEC_STATUS_PROFILE_READ_ERROR;
  }

  //TODO Control Plane init
  gVCHdmiCec = result;
  *handle = (vComponent_HdmiCec_t *)result;
  return VC_HDMICEC_STATUS_SUCCESS;

}

vComponent_HdmiCec_Status vComponent_HdmiCec_Deinitialize(vComponent_HdmiCec_t *pVCHdmiCec)
{
  vCHdmiCec_t* vCHdmiCec = (vCHdmiCec_t*)pVCHdmiCec;

  if(vCHdmiCec == NULL)
  {
    VC_LOG("vComponent_HdmiCec_Deinitialize: Invalid handle param");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }

  if(vCHdmiCec != gVCHdmiCec)
  {
    VC_LOG("vComponent_HdmiCec_Deinitialize: Invalid handle");
    return VC_HDMICEC_STATUS_INVALID_HANDLE;
  }

  if(gVCHdmiCec == NULL)
  {
    VC_LOG("vComponent_HdmiCec_Deinitialize: Already Deinitialized");
    return VC_HDMICEC_STATUS_NOT_INITIALIZED;
  }

  //Should we enforce HdmiCecClose to be called before?
  if(vCHdmiCec->cec_hal != NULL && vCHdmiCec->cec_hal->state != HAL_STATE_CLOSED)
  {
    HdmiCecClose((int)vCHdmiCec->cec_hal);
  }
  ut_kvp_profile_close();
  free(vCHdmiCec);
  gVCHdmiCec = NULL;
  return VC_HDMICEC_STATUS_SUCCESS;
}



HDMI_CEC_STATUS HdmiCecOpen(int* handle)
{
  char emulated_device[MAX_OSD_NAME_LENGTH];

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

  vCHdmiCec_hal_t* cec = (vCHdmiCec_hal_t*)malloc(sizeof(vCHdmiCec_hal_t));
  if(cec == NULL) 
  {
    VC_LOG( "Out of Memory" );
    return HDMI_CEC_IO_GENERAL_ERROR;
  }

  VC_LOG("HdmiCecOpen: Loading emulated Device Info");
  ut_kvp_instance_t *profile_instance = ut_kvp_profile_getInstance();
  assert(profile_instance != NULL);
  
  ut_kvp_getStringField(profile_instance, "hdmicec/emulated_device", emulated_device, MAX_OSD_NAME_LENGTH);

  cec->num_ports = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_ports");
  VC_LOG("HdmiCecOpen: Loading Ports Info");
  vCHdmiCec_port_info_t* ports = (vCHdmiCec_port_info_t*) malloc(sizeof(vCHdmiCec_port_info_t) * cec->num_ports);
  assert(ports != NULL);

  LoadPortsInfo(profile_instance, ports, cec->num_ports);
  cec->ports = ports;

  //Setup Eventing and callback

  //Device Discovery and Network Topology
  cec->num_devices = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_devices");

  VC_LOG("HdmiCecOpen: Loading connected Device Info");

  cec->devices_map = CreateDeviceMap(profile_instance, "hdmicec/device_map/0");
  cec->emulated_device = GetDeviceByName(cec->devices_map, emulated_device);
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
  VC_LOG("Type                          : %s", GetDeviceTypeStr(cec->emulated_device->type));
  VC_LOG("Pwr Status                    : %s", GetDevicePowerStatusStr(cec->emulated_device->power_status));
  VC_LOG("Number of Ports               : %d", cec->num_ports);
  VC_LOG("Number of devices in Network  : %d", cec->num_devices);
  VC_LOG("==========================\r\n");

  PrintDeviceMap(cec->devices_map);
  VC_LOG("==========================\r\n");

  *handle = (int) cec;
  gVCHdmiCec->cec_hal = cec;
  cec->state = HAL_STATE_READY;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecClose(int handle)
{

  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
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
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }


  if(physicalAddress == NULL)
  {
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  *physicalAddress = gVCHdmiCec->cec_hal->emulated_device->physical_address;
  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }


  if(gVCHdmiCec->cec_hal->emulated_device->type != DEVICE_TYPE_TV || logicalAddresses != 0)
  {
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
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }

  //Remove Logical Address only for Sink device
  if(gVCHdmiCec->cec_hal->emulated_device->type != DEVICE_TYPE_TV || logicalAddresses != 0)
  {
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  if(gVCHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
  {
    //Looks like logical address is already removed. 
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
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }


  if(logicalAddress == NULL)
  {
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  *logicalAddress = gVCHdmiCec->cec_hal->emulated_device->logical_address;

  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
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
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
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
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }


  if(buf == NULL || len <= 0 || result == NULL)
  {
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  if(gVCHdmiCec->cec_hal->emulated_device->type == DEVICE_TYPE_TV && gVCHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
  {
    //If Logical Address is not set for a sink device, we cannot transmit
    return HDMI_CEC_IO_SENT_FAILED;
  }

  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("HdmiCecTx: ");
  for (int i = 0; i < len; i++) {
    VC_LOG("%02X ", buf[i]);
  }
  VC_LOG("==========================");
  *result = HDMI_CEC_IO_SENT_BUT_NOT_ACKD;

  
  return HDMI_CEC_IO_SUCCESS;
}

HDMI_CEC_STATUS HdmiCecTxAsync(int handle, const unsigned char* buf, int len)
{
  if(gVCHdmiCec == NULL || gVCHdmiCec->cec_hal == NULL)
  {
    return HDMI_CEC_IO_NOT_OPENED;
  }

  if(handle == 0)
  {
    return HDMI_CEC_IO_INVALID_HANDLE;
  }

  if(buf == NULL || len <= 0)
  {
    return HDMI_CEC_IO_INVALID_ARGUMENT;
  }

  if((gVCHdmiCec->cec_hal->emulated_device->type == DEVICE_TYPE_TV && gVCHdmiCec->cec_hal->emulated_device->logical_address == 0x0F)
            || gVCHdmiCec->cec_hal->callbacks.tx_cb_func == NULL)
  {
    //If Logical Address is not set for a sink device, we cannot transmit
    return HDMI_CEC_IO_SENT_FAILED;
  }

  VC_LOG(">>>>>>> >>>>> >>>> >> >> >");
  VC_LOG("HdmiCecTxAsync: ");
  for (int i = 0; i < len; i++) {
    VC_LOG("%02X ", buf[i]);
  }
  VC_LOG("==========================");

  return HDMI_CEC_IO_SUCCESS;
}

