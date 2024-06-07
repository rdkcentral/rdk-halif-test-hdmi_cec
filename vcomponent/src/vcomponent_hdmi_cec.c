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

#define VC_LOG(format, ...)                 UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_YELLOW"vcHdmiCec[LOG]   "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)
#define VC_LOG_ERROR(format, ...)           UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_RED"vcHdmiCec[ERROR] "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)

/**HDMI CEC HAL Data structures */

#define MAX_OSD_NAME_LENGTH 16

typedef enum
{
  DEVICE_TYPE_TV = 0,
  DEVICE_TYPE_PLAYBACK,
  DEVICE_TYPE_AUDIO_SYSTEM,
  DEVICE_TYPE_RECORDER,
  DEVICE_TYPE_TUNER,
  DEVICE_TYPE_RESERVED,
  DEVICE_TYPE_FREEUSE,
  DEVICE_TYPE_UNREGISTERED,
  DEVICE_TYPE_UNKNOWN
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
  LOGICAL_ADDRESS_BROADCAST        = 15,
  LOGICAL_ADDRESS_MAX              = 16

} vCHdmiCec_logical_address_t;

typedef enum
{
  CEC_VERSION_UNKNOWN = 0,
  CEC_VERSION_1_2 = 1,
  CEC_VERSION_1_2A = 2,
  CEC_VERSION_1_3 = 3,
  CEC_VERSION_1_3A = 4,
  CEC_VERSION_1_4 = 5,
  CEC_VERSION_2_0 = 6
} vCHdmiCec_version_t;

typedef enum
{
  PORT_TYPE_INPUT = 0,
  PORT_TYPE_OUTPUT,
  PORT_TYPE_UNKNOWN
} vCHdmiCec_port_type_t;

typedef enum
{
  POWER_STATUS_ON = 0,
  POWER_STATUS_STANDBY,
  POWER_STATUS_OFF,
  POWER_STATUS_UNKNOWN
} vCHdmiCec_power_status_t;

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
} vCHdmiCec_vendor_code_t;

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

struct vCHdmiCec_device_info_t
{
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
  unsigned short parent_port_id;
  vCHdmiCec_power_status_t power_status;
  char osd_name[MAX_OSD_NAME_LENGTH];
} vCHdmiCec_device_info_t;

typedef struct
{
  HdmiCecRxCallback_t rx_cb_func;
  void* rx_cb_data;
  HdmiCecTxCallback_t tx_cb_func;
  void* tx_cb_data;
} vCHdmiCec_callbacks_t;

typedef struct {
    bool allocated[LOGICAL_ADDRESS_MAX];
} vCHdmiCec_logical_address_pool_t;

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
} vCHdmiCec_t;

/*Common Data types*/

typedef struct
{
  char* str;
  int val;
} strVal_t;

/*Global variables*/
static vCHdmiCec_t* gVCHdmiCec = NULL;

const static strVal_t gDIStrVal [] = {
  { "TV", (int) DEVICE_TYPE_TV },
  { "PlaybackDevice", (int) DEVICE_TYPE_PLAYBACK },
  { "AudioSystem" , (int) DEVICE_TYPE_AUDIO_SYSTEM },
  { "RecordingDevice", (int) DEVICE_TYPE_RECORDER },
  { "Tuner", (int) DEVICE_TYPE_TUNER },
  { "Reserved",  (int) DEVICE_TYPE_RESERVED },
  { "FreeUse", (int) DEVICE_TYPE_FREEUSE },
  { "Unregistered", (int) DEVICE_TYPE_UNREGISTERED },
  { "Unknown", (int) DEVICE_TYPE_UNKNOWN }
};

const static strVal_t gVCStrVal [] = {
  {"TOSHIBA", VENDOR_CODE_TOSHIBA},
  {"SAMSUNG", VENDOR_CODE_SAMSUNG},
  {"DENON", VENDOR_CODE_DENON},
  {"MARANTZ", VENDOR_CODE_MARANTZ},
  {"LOEWE", VENDOR_CODE_LOEWE},
  {"ONKYO", VENDOR_CODE_ONKYO},
  {"MEDION", VENDOR_CODE_MEDION},
  {"TOSHIBA2", VENDOR_CODE_TOSHIBA},
  {"APPLE", VENDOR_CODE_APPLE},
  {"HARMAN_KARDON2", VENDOR_CODE_HARMAN_KARDON2},
  {"GOOGLE", VENDOR_CODE_GOOGLE},
  {"AKAI", VENDOR_CODE_AKAI},
  {"AOC", VENDOR_CODE_AOC},
  {"PANASONIC", VENDOR_CODE_PANASONIC},
  {"PHILIPS", VENDOR_CODE_PHILIPS},
  {"DAEWOO", VENDOR_CODE_DAEWOO},
  {"YAMAHA", VENDOR_CODE_YAMAHA},
  {"GRUNDIG", VENDOR_CODE_GRUNDIG},
  {"PIONEER", VENDOR_CODE_PIONEER},
  {"LG", VENDOR_CODE_LG},
  {"SHARP", VENDOR_CODE_SHARP},
  {"SONY", VENDOR_CODE_SONY},
  {"BROADCOM", VENDOR_CODE_BROADCOM},
  {"SHARP2", VENDOR_CODE_SHARP2},
  {"VIZIO", VENDOR_CODE_VIZIO},
  {"BENQ", VENDOR_CODE_BENQ},
  {"HARMAN_KARDON", VENDOR_CODE_HARMAN_KARDON},
  {"UNKNOWN", VENDOR_CODE_UNKNOWN},
  };

const static strVal_t gPSStrVal [] = {
  { "on", (int)POWER_STATUS_ON  },
  { "off", (int)POWER_STATUS_OFF  },
  { "standby", (int)POWER_STATUS_STANDBY },
  { "unknown", (int)POWER_STATUS_UNKNOWN }
};

const static strVal_t gPortStrVal [] = {
  { "in", (int)PORT_TYPE_INPUT  },
  { "out", (int)PORT_TYPE_OUTPUT },
  { "unknown", (int)PORT_TYPE_UNKNOWN }
};

int GetValByStr(const strVal_t *map, int length, char* str, int default_val)
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

char* GetStrByVal(const strVal_t *map, int length, int val)
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
    ports[i].type = GetValByStr(gPortStrVal, sizeof(gPortStrVal)/sizeof(strVal_t), type, (int)PORT_TYPE_UNKNOWN);

    strcpy(tmp + strlen(prefix) + length, "/cec_supported");
    ports[i].cec_supported = ut_kvp_getBoolField(instance, tmp);

    strcpy(tmp + strlen(prefix) + length, "/arc_supported");
    ports[i].arc_supported = ut_kvp_getBoolField(instance, tmp);

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

  strcpy(tmp + strlen(prefix), "/name");
  ut_kvp_getStringField(instance, tmp, device->osd_name, MAX_OSD_NAME_LENGTH);

  strcpy(tmp + strlen(prefix), "/active_source");
  device->active_source = ut_kvp_getBoolField(instance, tmp);

  strcpy(tmp + strlen(prefix), "/pwr_status");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->power_status = GetValByStr(gPSStrVal, sizeof(gPSStrVal)/sizeof(strVal_t), type, (int)POWER_STATUS_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/version");
  device->version = (vCHdmiCec_version_t) ut_kvp_getUInt32Field(instance, tmp);

  strcpy(tmp + strlen(prefix), "/vendor");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->vendor_id = GetValByStr(gVCStrVal, sizeof(gVCStrVal)/sizeof(strVal_t), type, (int)VENDOR_CODE_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/type");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->type = GetValByStr(gDIStrVal, sizeof(gDIStrVal)/sizeof(strVal_t), type, (int)DEVICE_TYPE_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/port_id");
  device->parent_port_id = ut_kvp_getUInt32Field(instance, tmp);

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
  struct vCHdmiCec_device_info_t* tmp;
  struct vCHdmiCec_device_info_t* prev;

  if(map == NULL || name == NULL)
  {
    return;
  }
  if(map->first_child == NULL)
  {
    return;
  }

  tmp = map->first_child;
  prev = NULL;

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
  struct vCHdmiCec_device_info_t *device;
  if(instance == NULL || profile_prefix == NULL )
  {
    assert(instance != NULL);
    assert(profile_prefix != NULL);
    return NULL;
  }

  device = (struct vCHdmiCec_device_info_t *)malloc(sizeof(struct vCHdmiCec_device_info_t));
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

void InitLogicalAddressPool(vCHdmiCec_logical_address_pool_t *pool)
{
  assert(pool != NULL);
  for (int i = 0; i < LOGICAL_ADDRESS_MAX; i++) {
      pool->allocated[i] = false;
  }
}

vCHdmiCec_logical_address_t AllocateLogicalAddress(vCHdmiCec_logical_address_pool_t *pool, vCHdmiCec_device_type_t device_type)
{
  vCHdmiCec_logical_address_t possible_addresses[LOGICAL_ADDRESS_MAX];
  int num_possible_addresses = 0;

  assert(pool != NULL);

  switch (device_type)
  {
    case DEVICE_TYPE_TV:
      possible_addresses[0] = LOGICAL_ADDRESS_TV;
      num_possible_addresses = 1;
      break;
    case DEVICE_TYPE_PLAYBACK:
      possible_addresses[0] = LOGICAL_ADDRESS_PLAYBACKDEVICE1;
      possible_addresses[1] = LOGICAL_ADDRESS_PLAYBACKDEVICE2;
      possible_addresses[2] = LOGICAL_ADDRESS_PLAYBACKDEVICE3;
      num_possible_addresses = 3;
      break;
    case DEVICE_TYPE_AUDIO_SYSTEM:
      possible_addresses[0] = LOGICAL_ADDRESS_AUDIOSYSTEM;
      num_possible_addresses = 1;
      break;
    case DEVICE_TYPE_RECORDER:
      possible_addresses[0] = LOGICAL_ADDRESS_RECORDINGDEVICE1;
      possible_addresses[1] = LOGICAL_ADDRESS_RECORDINGDEVICE2;
      possible_addresses[2] = LOGICAL_ADDRESS_RECORDINGDEVICE3;
      num_possible_addresses = 3;
        break;
    case DEVICE_TYPE_TUNER:
      possible_addresses[0] = LOGICAL_ADDRESS_TUNER1;
      possible_addresses[1] = LOGICAL_ADDRESS_TUNER2;
      possible_addresses[2] = LOGICAL_ADDRESS_TUNER3;
      possible_addresses[3] = LOGICAL_ADDRESS_TUNER4;
      num_possible_addresses = 4;
      break;
    case DEVICE_TYPE_RESERVED:
      possible_addresses[0] = LOGICAL_ADDRESS_RESERVED1;
      possible_addresses[1] = LOGICAL_ADDRESS_RESERVED2;
      num_possible_addresses = 2;
      break;
    case DEVICE_TYPE_FREEUSE:
      possible_addresses[0] = LOGICAL_ADDRESS_FREEUSE;
      num_possible_addresses = 1;
      break;
    case DEVICE_TYPE_UNREGISTERED:
    default:
      possible_addresses[0] = LOGICAL_ADDRESS_UNREGISTERED;
      num_possible_addresses = 1;
      break;
  }

  for (int i = 0; i < num_possible_addresses; i++)
  {
    if (!pool->allocated[possible_addresses[i]])
    {
      pool->allocated[possible_addresses[i]] = true;
      return possible_addresses[i];
    }
  }
  return DEVICE_TYPE_UNREGISTERED; // No available addresses for the given device type
}

void ReleaseLogicalAddress(vCHdmiCec_logical_address_pool_t *pool, vCHdmiCec_logical_address_t address)
{
  assert(pool != NULL);
  if (address >= 0 && address <= LOGICAL_ADDRESS_MAX)
  {
    pool->allocated[address] = false;
  }
}

void AllocatePhysicalLogicalAddress(struct vCHdmiCec_device_info_t * map, struct vCHdmiCec_device_info_t * emulated_device, vCHdmiCec_logical_address_pool_t* pool)
{
  unsigned char physicalAddress[4];
  if(map == NULL || emulated_device == NULL || pool == NULL)
  {
    return;
  }
  //Allocate physical address
  if(map->parent == NULL)
  {
    //This is the root.
    map->physical_address = 0;
  }
  else
  {
    physicalAddress[0] = (map->parent->physical_address >> 20) & 0x0F;
    physicalAddress[1] = (map->parent->physical_address >> 16) & 0x0F;
    physicalAddress[2] = (map->parent->physical_address >> 4) & 0x0F;
    physicalAddress[3] = map->parent->physical_address & 0x0F;

    for(int i = 0; i < sizeof(physicalAddress); ++i)
    {
      if(physicalAddress[i] == 0)
      {
        physicalAddress[i] = map->parent_port_id;
        break;
      }
    }
    map->physical_address = ((physicalAddress[0] & 0x0F) << 20) | ((physicalAddress[1] & 0x0F) << 16) | ((physicalAddress[2] & 0x0F) << 4) | (physicalAddress[3] & 0x0F);
  }

  //Allocate logical address
  if(map->parent == NULL)
  {
    if(!strcmp(map->osd_name, emulated_device->osd_name))
    {
      //If the root device (TV) is the emulated device, then logical address is expected to be set by application
      map->logical_address = LOGICAL_ADDRESS_BROADCAST; 
    }
  }
  else
  {
      map->logical_address = AllocateLogicalAddress(pool, map->type);
  }

  AllocatePhysicalLogicalAddress(map->next_sibling, emulated_device, pool);
  AllocatePhysicalLogicalAddress(map->first_child, emulated_device, pool);
}

void PrintDeviceMap(struct vCHdmiCec_device_info_t* map, int level)
{
  unsigned char physicalAddress[4];
  if(map == NULL)
  {
    return;
  }
  physicalAddress[0] = (map->physical_address >> 20) & 0x0F;
  physicalAddress[1] = (map->physical_address >> 16) & 0x0F;
  physicalAddress[2] = (map->physical_address >> 4) & 0x0F;
  physicalAddress[3] = map->physical_address & 0x0F;

  VC_LOG(">>>>>>>>>>>> >>>>>>>>>> >>>>> >>>> >>> >> >");
  VC_LOG("%*cDevice          : %s", level*4,' ', map->osd_name);
  VC_LOG("%*cType            : %s", level*4,' ', GetStrByVal(gDIStrVal, sizeof(gDIStrVal)/sizeof(strVal_t), map->type));
  VC_LOG("%*cPwr Status      : %s", level*4,' ', GetStrByVal(gPSStrVal, sizeof(gPSStrVal)/sizeof(strVal_t), map->power_status));
  VC_LOG("%*cPhysical Address: %d.%d.%d.%d", level*4,' ', physicalAddress[0], physicalAddress[1], physicalAddress[2], physicalAddress[3]);
  VC_LOG("%*cLogical Address : %d", level*4,' ',map->logical_address);
  VC_LOG("-------------------------------------------");

  PrintDeviceMap(map->next_sibling, level);
  PrintDeviceMap(map->first_child, level +1);
}

struct vCHdmiCec_device_info_t* GetDeviceByName(struct vCHdmiCec_device_info_t* map, char* name)
{
  struct vCHdmiCec_device_info_t* device;
  if(map == NULL || name == NULL)
  {
    return NULL;
  }
  if(strcmp(map->osd_name, name) == 0)
  {
    return map;
  }
  device = GetDeviceByName(map->first_child, name);
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
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Invalid handle param");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }
  if(pProfilePath == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Invalid Profile path");
    return VC_HDMICEC_STATUS_INVALID_PARAM;
  }

  if(gVCHdmiCec != NULL)
  {
    //Already initialised
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Already Initialized");
    return VC_HDMICEC_STATUS_ALREADY_INITIALIZED;
  }

  result = (vCHdmiCec_t*)malloc(sizeof(vCHdmiCec_t));
  if(result == NULL)
  {
    VC_LOG_ERROR("vComponent_HdmiCec_Initialize: Out of memory");
    return VC_HDMICEC_STATUS_OUT_OF_MEMORY;
  }
  result->cp_path = pCPUrl;
  result->cp_port = cpPort;
  result->cec_hal = NULL;

  status = ut_kvp_profile_open(pProfilePath);
  if(status != UT_KVP_STATUS_SUCCESS)
  {
    VC_LOG_ERROR("ut_kvp_profile_open: status: %d", status);
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

  profile_instance = ut_kvp_profile_getInstance();
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

  cec->devices_map = CreateDeviceMap(profile_instance, "hdmicec/device_map/0");
  cec->emulated_device = GetDeviceByName(cec->devices_map, emulated_device);
  if(cec->emulated_device == NULL)
  {
    VC_LOG_ERROR("HdmiCecOpen: Couldnt load emulated device info");
    assert(cec->emulated_device != NULL);
    TeardownHal(cec);
    return HDMI_CEC_IO_GENERAL_ERROR;
  }
  InitLogicalAddressPool(&cec->address_pool);
  AllocatePhysicalLogicalAddress(cec->devices_map, cec->emulated_device, &cec->address_pool);

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
  VC_LOG("Type                          : %s", GetStrByVal(gDIStrVal, sizeof(gDIStrVal)/sizeof(strVal_t), cec->emulated_device->type));
  VC_LOG("Pwr Status                    : %s", GetStrByVal(gPSStrVal, sizeof(gPSStrVal)/sizeof(strVal_t), cec->emulated_device->power_status));
  VC_LOG("Number of Ports               : %d", cec->num_ports);
  VC_LOG("Number of devices in Network  : %d", cec->num_devices);
  VC_LOG("===========================");

  PrintDeviceMap(cec->devices_map, 0);
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

