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

#include "vcomponent_hdmi_cec_device.h"


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
  device->power_status = vCHdmiCec_GetValue(gPSStrVal, sizeof(gPSStrVal)/sizeof(strVal_t), type, (int)POWER_STATUS_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/version");
  device->version = (vCHdmiCec_version_t) ut_kvp_getUInt32Field(instance, tmp);

  strcpy(tmp + strlen(prefix), "/vendor");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->vendor_id = vCHdmiCec_GetValue(gVCStrVal, sizeof(gVCStrVal)/sizeof(strVal_t), type, (int)VENDOR_CODE_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/type");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->type = vCHdmiCec_GetValue(gDIStrVal, sizeof(gDIStrVal)/sizeof(strVal_t), type, (int)DEVICE_TYPE_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/port_id");
  device->parent_port_id = ut_kvp_getUInt32Field(instance, tmp);

  strcpy(tmp + strlen(prefix), "/number_children");
  device->number_children = (vCHdmiCec_version_t) ut_kvp_getUInt32Field(instance, tmp);
}

struct vCHdmiCec_device_info_t* vCHdmiCec_Device_CreateMapFromProfile (ut_kvp_instance_t* instance, char* profile_prefix)
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
  vCHdmiCec_Device_Reset(device);
  LoadDeviceInfo(instance, profile_prefix, device);
  for(int j=0; j < device->number_children; j++)
  {
    char tmp[strlen(profile_prefix) + 128];
    strcpy(tmp, profile_prefix);
    strcpy(tmp + strlen(profile_prefix), "/children/");
    int length = snprintf( NULL, 0, "%d", j );
    snprintf( tmp + strlen(tmp) , length + 1, "%d", j );

    struct vCHdmiCec_device_info_t *child = vCHdmiCec_Device_CreateMapFromProfile(instance, tmp);
    assert(child != NULL);
    vCHdmiCec_Device_InsertChild(device, child);
  }
  return device;
}

void vCHdmiCec_Device_DestroyMap(struct vCHdmiCec_device_info_t* map)
{
  if(map == NULL)
  {
    return;
  }

  vCHdmiCec_Device_DestroyMap(map->first_child);
  vCHdmiCec_Device_DestroyMap(map->next_sibling);

  free(map);
}

void vCHdmiCec_Device_PrintMap(struct vCHdmiCec_device_info_t* map, int level)
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
  VC_LOG("%*cType            : %s", level*4,' ', vCHdmiCec_GetString(gDIStrVal, sizeof(gDIStrVal)/sizeof(strVal_t), map->type));
  VC_LOG("%*cPwr Status      : %s", level*4,' ', vCHdmiCec_GetString(gPSStrVal, sizeof(gPSStrVal)/sizeof(strVal_t), map->power_status));
  VC_LOG("%*cPhysical Address: %d.%d.%d.%d", level*4,' ', physicalAddress[0], physicalAddress[1], physicalAddress[2], physicalAddress[3]);
  VC_LOG("%*cLogical Address : %d", level*4,' ',map->logical_address);
  VC_LOG("-------------------------------------------");

  vCHdmiCec_Device_PrintMap(map->next_sibling, level);
  vCHdmiCec_Device_PrintMap(map->first_child, level +1);
}

void vCHdmiCec_Device_Reset(struct vCHdmiCec_device_info_t* device)
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

void vCHdmiCec_Device_InsertChild(struct vCHdmiCec_device_info_t* parent, struct vCHdmiCec_device_info_t* child)
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

void vCHdmiCec_Device_RemoveChild(struct vCHdmiCec_device_info_t* map, char* name)
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

struct vCHdmiCec_device_info_t* vCHdmiCec_Device_Get(struct vCHdmiCec_device_info_t* map, char* name)
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
  device = vCHdmiCec_Device_Get(map->first_child, name);
  if(device != NULL)
  {
    return device;
  }
  return vCHdmiCec_Device_Get(map->next_sibling, name);
}

void vCHdmiCec_Device_InitLogicalAddressPool(vCHdmiCec_logical_address_pool_t *pool)
{
  assert(pool != NULL);
  for (int i = 0; i < LOGICAL_ADDRESS_MAX; i++) {
      pool->allocated[i] = false;
  }
}

void vCHdmiCec_Device_AllocatePhysicalLogicalAddresses(struct vCHdmiCec_device_info_t * map, struct vCHdmiCec_device_info_t * emulated_device, vCHdmiCec_logical_address_pool_t* pool)
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
      map->logical_address = vCHdmiCec_Device_AllocateLogicalAddress(pool, map->type);
  }

  vCHdmiCec_Device_AllocatePhysicalLogicalAddresses(map->next_sibling, emulated_device, pool);
  vCHdmiCec_Device_AllocatePhysicalLogicalAddresses(map->first_child, emulated_device, pool);
}

vCHdmiCec_logical_address_t vCHdmiCec_Device_AllocateLogicalAddress(vCHdmiCec_logical_address_pool_t *pool, vCHdmiCec_device_type_t device_type)
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

void vCHdmiCec_Device_ReleaseLogicalAddress(vCHdmiCec_logical_address_pool_t *pool, vCHdmiCec_logical_address_t address)
{
  assert(pool != NULL);
  if (address >= 0 && address <= LOGICAL_ADDRESS_MAX)
  {
    pool->allocated[address] = false;
  }
}

int vCHdmiCec_GetValue(const strVal_t *map, int length, char* str, int default_val)
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

char* vCHdmiCec_GetString(const strVal_t *map, int length, int val)
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




