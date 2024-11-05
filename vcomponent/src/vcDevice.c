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

#include "vcDevice.h"
       #include <unistd.h>

const static vcCommand_strVal_t gDIStrVal [] = {
  { "TV", (int) DEVICE_TYPE_TV },
  { "PlaybackDevice", (int) DEVICE_TYPE_PLAYBACK },
  { "AudioSystem" , (int) DEVICE_TYPE_AUDIO_SYSTEM },
  { "RecordingDevice", (int) DEVICE_TYPE_RECORDER },
  { "Tuner", (int) DEVICE_TYPE_TUNER },
  { "Reserved",  (int) DEVICE_TYPE_RESERVED },
};

const static vcCommand_strVal_t gVCStrVal [] = {
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

const static vcCommand_strVal_t gPSStrVal [] = {
  { "on", (int)CEC_POWER_STATUS_ON  },
  { "standby", (int)CEC_POWER_STATUS_STANDBY },
  { "unknown", (int)CEC_POWER_STATUS_UNKNOWN }
};

static void LoadDeviceInfo (ut_kvp_instance_t* instance, char* prefix, struct vcDevice_info_t* device);

/* Load the device info into the passed in vcDevice_info_t*
* prefix can be 
*   "hdmicec/device_map/0"
*   "hdmicec/device_map/0/children/0"
*   "hdmicec/device_map/0/children/1"
*   "hdmicec/device_map/0/children/1/children/0"
*/
static void LoadDeviceInfo (ut_kvp_instance_t* instance, char* prefix, struct vcDevice_info_t* device)
{
  char tmp[strlen(prefix) + 64];
  char type[32];

  assert(device != NULL);
  assert(instance != NULL);
  assert(prefix != NULL);

  strcpy(tmp, prefix);

  strcpy(tmp + strlen(prefix), "/name");
  ut_kvp_getStringField(instance, tmp, device->osd_name, MAX_OSD_NAME_LENGTH);

  strcpy(tmp + strlen(prefix), "/active_source");
  device->active_source = ut_kvp_getBoolField(instance, tmp);

  strcpy(tmp + strlen(prefix), "/pwr_status");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->power_status = vcCommand_GetValue(gPSStrVal, COUNT_OF(gPSStrVal), type, (int)CEC_POWER_STATUS_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/version");
  device->version = (vcCommand_version_t) ut_kvp_getUInt32Field(instance, tmp);

  strcpy(tmp + strlen(prefix), "/vendor");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->vendor_id = vcCommand_GetValue(gVCStrVal, COUNT_OF(gVCStrVal), type, (int)VENDOR_CODE_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/type");
  ut_kvp_getStringField(instance, tmp, type, sizeof(type));
  device->type = vcCommand_GetValue(gDIStrVal, COUNT_OF(gDIStrVal), type, (int)DEVICE_TYPE_UNKNOWN);

  strcpy(tmp + strlen(prefix), "/port_id");
  device->parent_port_id = ut_kvp_getUInt32Field(instance, tmp);

  strcpy(tmp + strlen(prefix), "/number_children");
  device->number_children = (vcCommand_version_t) ut_kvp_getUInt32Field(instance, tmp);
}

struct vcDevice_info_t* vcDevice_CreateMapFromProfile (ut_kvp_instance_t* instance, char* profile_prefix)
{
  struct vcDevice_info_t *device;
  if(instance == NULL )
  {
    VC_LOG("vcDevice_CreateMapFromProfile: instance NULL");
    return NULL;
  }

  if(profile_prefix == NULL)
  {
    VC_LOG("vcDevice_CreateMapFromProfile: profile_prefix NULL");
    return NULL;
  }

  device = (struct vcDevice_info_t *)malloc(sizeof(struct vcDevice_info_t));
  assert(device != NULL);
  vcDevice_Reset(device);
  LoadDeviceInfo(instance, profile_prefix, device);
  for(int j=0; j < device->number_children; j++)
  {
    char tmp[UT_KVP_MAX_ELEMENT_SIZE];
    strcpy(tmp, profile_prefix);
    strcpy(tmp + strlen(profile_prefix), "/children/");
    int length = snprintf( NULL, 0, "%d", j );
    snprintf( tmp + strlen(tmp) , length + 1, "%d", j );

    struct vcDevice_info_t *child = vcDevice_CreateMapFromProfile(instance, tmp);
    assert(child != NULL);
    vcDevice_InsertChild(device, child);
  }
  return device;
}

void vcDevice_DestroyMap(struct vcDevice_info_t* map)
{
  if(map == NULL)
  {
    return;
  }

  vcDevice_DestroyMap(map->first_child);
  vcDevice_DestroyMap(map->next_sibling);

  free(map);
}

void vcDevice_PrintMap(struct vcDevice_info_t* map, int level)
{
  unsigned char physicalAddress[4];
  if(map == NULL)
  {
    return;
  }
  physicalAddress[0] = (map->physical_address >> 12) & 0x0F;
  physicalAddress[1] = (map->physical_address >> 8) & 0x0F;
  physicalAddress[2] = (map->physical_address >> 4) & 0x0F;
  physicalAddress[3] = map->physical_address & 0x0F;

  VC_LOG(">>>>>>>>>>>> >>>>>>>>>> >>>>> >>>> >>> >> >");
  VC_LOG("%*cDevice          : %s", level*4,' ', map->osd_name);
  VC_LOG("%*cType            : %s", level*4,' ', vcCommand_GetString(gDIStrVal, COUNT_OF(gDIStrVal), map->type));
  VC_LOG("%*cPwr Status      : %s", level*4,' ', vcCommand_GetString(gPSStrVal, COUNT_OF(gPSStrVal), map->power_status));
  VC_LOG("%*cPhysical Address: %d.%d.%d.%d", level*4,' ', physicalAddress[0], physicalAddress[1], physicalAddress[2], physicalAddress[3]);
  VC_LOG("%*cLogical Address : %d", level*4,' ',map->logical_address);
  VC_LOG("-------------------------------------------");

  vcDevice_PrintMap(map->next_sibling, level);
  vcDevice_PrintMap(map->first_child, level +1);
}

void vcDevice_Reset(struct vcDevice_info_t* device)
{
  if(device == NULL)
  {
    VC_LOG("vcDevice_Reset: device NULL");
    assert(device != NULL);
  }
  device->active_source = false;
  device->logical_address = LOGICAL_ADDRESS_UNKNOWN;
  device->physical_address = 0xFFFF;
  device->parent = NULL;
  device->first_child = NULL;
  device->next_sibling = NULL;
  device->vendor_id = 0;
  device->number_children = 0;
}

void vcDevice_InsertChild(struct vcDevice_info_t* parent, struct vcDevice_info_t* child)
{
  if(parent == NULL)
  {
    VC_LOG("vcDevice_InsertChild: parent NULL");
    return;
  }
  if(child == NULL)
  {
    VC_LOG("vcDevice_InsertChild: child NULL");
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

void vcDevice_RemoveChild(struct vcDevice_info_t* map, char* name)
{
  struct vcDevice_info_t* tmp;
  struct vcDevice_info_t* prev;

  if(map == NULL)
  {
    VC_LOG("vcDevice_RemoveChild: map NULL");
    return;
  }
  if(name == NULL)
  {
    VC_LOG("vcDevice_RemoveChild: name NULL");
    return;
  }
  if(map->first_child == NULL)
  {
    VC_LOG("vcDevice_RemoveChild: first_child NULL");
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
  //Disconnect the subtree to prevent freeing the wrong nodes
  tmp->next_sibling = NULL;

  //Remove the entire tree under this device.
  vcDevice_DestroyMap(tmp);
}

struct vcDevice_info_t* vcDevice_Get(struct vcDevice_info_t* map, char* name)
{
  struct vcDevice_info_t* device;
  if(map == NULL)
  {
    return NULL;
  }
  if(name == NULL)
  {
    VC_LOG("vcDevice_Get: name NULL");
    return NULL;
  }
  if(strcmp(map->osd_name, name) == 0)
  {
    return map;
  }
  device = vcDevice_Get(map->first_child, name);
  if(device != NULL)
  {
    return device;
  }
  return vcDevice_Get(map->next_sibling, name);
}

void vcDevice_InitLogicalAddressPool(vcDevice_logical_address_pool_t *pool)
{
  if(pool == NULL)
  {
    VC_LOG("vcDevice_InitLogicalAddressPool: pool NULL");
    assert(pool != NULL);
  }

  for (int i = 0; i <= LOGICAL_ADDRESS_BROADCAST; i++) {
      pool->allocated[i] = false;
  }
}

void vcDevice_AllocatePhysicalLogicalAddresses(struct vcDevice_info_t * map, struct vcDevice_info_t * emulated_device, vcDevice_logical_address_pool_t* pool)
{
  unsigned char physicalAddress[4];
  if(emulated_device == NULL)
  {
    VC_LOG("vcDevice_AllocatePhysicalLogicalAddresses: emulated_device NULL");
    return;
  }
  if(pool == NULL)
  {
    VC_LOG("vcDevice_AllocatePhysicalLogicalAddresses: pool NULL");
    assert(pool != NULL);
  }
  if(map == NULL)
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
    physicalAddress[0] = (map->parent->physical_address >> 12) & 0x0F;
    physicalAddress[1] = (map->parent->physical_address >> 8) & 0x0F;
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
    map->physical_address = ((physicalAddress[0] & 0x0F) << 12) | ((physicalAddress[1] & 0x0F) << 8) | ((physicalAddress[2] & 0x0F) << 4) | (physicalAddress[3] & 0x0F);
  }
  //Allocate logical address only if it is not already allocated
  if(map->logical_address == LOGICAL_ADDRESS_UNKNOWN)
  {
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
        map->logical_address = vcDevice_AllocateLogicalAddress(pool, map->type);
    }
  }
  vcDevice_AllocatePhysicalLogicalAddresses(map->next_sibling, emulated_device, pool);
  vcDevice_AllocatePhysicalLogicalAddresses(map->first_child, emulated_device, pool);
}

vcCommand_logical_address_t vcDevice_AllocateLogicalAddress(vcDevice_logical_address_pool_t *pool, vcCommand_device_type_t device_type)
{
  vcCommand_logical_address_t possible_addresses[LOGICAL_ADDRESS_BROADCAST];
  int num_possible_addresses = 0;

  assert(pool != NULL);

  switch (device_type)
  {
    case DEVICE_TYPE_TV:
      possible_addresses[0] = LOGICAL_ADDRESS_TV;
      possible_addresses[1] = LOGICAL_ADDRESS_FREEUSE;
      num_possible_addresses = 2;
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
  return LOGICAL_ADDRESS_UNREGISTERED; // No available addresses for the given device type
}

void vcDevice_ReleaseLogicalAddress(vcDevice_logical_address_pool_t *pool, vcCommand_logical_address_t address)
{
  assert(pool != NULL);
  if (address >= 0 && address <= LOGICAL_ADDRESS_BROADCAST)
  {
    pool->allocated[address] = false;
  }
}





