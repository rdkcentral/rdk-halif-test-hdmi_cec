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

#ifndef __VCDEVICE_H
#define __VCDEVICE_H

#include "vcHdmiCec.h"
#include "vcCommand.h"
#include "ut_kvp.h"

#define MAX_OSD_NAME_LENGTH 16

typedef enum
{
  PORT_TYPE_INPUT = 0,
  PORT_TYPE_OUTPUT,
  PORT_TYPE_UNKNOWN
} vcDevice_port_type_t;

typedef struct {
    bool allocated[LOGICAL_ADDRESS_BROADCAST];
} vcDevice_logical_address_pool_t;

extern struct vcDevice_info_t
{
  /*Variables to manage a non-binary tree of devices*/
  unsigned int number_children;
  struct vcDevice_info_t* parent;
  struct vcDevice_info_t* first_child;
  struct vcDevice_info_t* next_sibling;

  /*Device Information*/
  vcCommand_device_type_t type;
  vcCommand_version_t version;
  uint16_t physical_address;
  vcCommand_logical_address_t logical_address;
  bool active_source;
  uint32_t vendor_id;
  uint16_t parent_port_id;
  vcCommand_power_status_t power_status;
  char osd_name[MAX_OSD_NAME_LENGTH];
} vcDevice_info_t;

/**
 * @brief Creates a map of devices in a parent-child n-ary tree.
 *
 * Recursively loads the device map from the provided profile instance.
 * To load from the root device, the prefix should be "hdmicec/device_map/0".
 *
 * @param instance Pointer to the profile instance from which to load the device map.
 * @param profile_prefix Prefix for the profile keys to load the device map.
 * @return Pointer to the newly created vcDevice_info_t structure.
 */
struct vcDevice_info_t* vcDevice_CreateMapFromProfile (ut_kvp_instance_t* instance, char* profile_prefix);

/**
 * @brief Destroys the device map from the root to all children/siblings.
 *
 * Frees up the memory used by the device map.
 *
 * @param map Pointer to the root of the device map to be destroyed.
 */
void vcDevice_DestroyMap(struct vcDevice_info_t* map);

/**
 * @brief Prints the device map recursively.
 *
 * To print devices from the root, pass level = 0.
 *
 * @param map Pointer to the root of the device map to be printed.
 * @param level The level in the hierarchy from which to start printing.
 */
void vcDevice_PrintMap(struct vcDevice_info_t* map, int level);

/**
 * @brief Resets a device to its initial values.
 *
 * @param device Pointer to the device to be reset.
 */
void vcDevice_Reset (struct vcDevice_info_t* device);

/**
 * @brief Adds a new child to the parent device.
 *
 * @param parent Pointer to the parent device.
 * @param child Pointer to the child device to be added.
 */
void vcDevice_InsertChild(struct vcDevice_info_t* parent, struct vcDevice_info_t* child);

/**
 * @brief Removes a child from the parent device.
 *
 * Removing a child device will remove all devices connected through the child.
 *
 * @param map Pointer to the root of the device map.
 * @param name Name of the child device to be removed.
 */
void vcDevice_RemoveChild(struct vcDevice_info_t* map, char* name);

/**
 * @brief Finds a device by its name.
 *
 * @param map Pointer to the root of the device map.
 * @param name Name of the device to be found.
 * @return Pointer to the device if found, NULL otherwise.
 */
struct vcDevice_info_t* vcDevice_Get(struct vcDevice_info_t* map, char* name);

/**
 * @brief Initializes the logical address pool.
 *
 * @param pool Pointer to the logical address pool to be initialized.
 */
void vcDevice_InitLogicalAddressPool(vcDevice_logical_address_pool_t *pool);

/**
 * @brief Allocates physical and logical addresses to all devices in the map recursively.
 *
 * If the emulated device is the root device (TV, sink), the logical address is set to 0x0F.
 * Logical addresses are picked from the provided pool.
 *
 * @param map Pointer to the root of the device map.
 * @param emulated_device Pointer to the emulated device.
 * @param pool Pointer to the logical address pool.
 */
void vcDevice_AllocatePhysicalLogicalAddresses(struct vcDevice_info_t *map, struct vcDevice_info_t *emulated_device, vcDevice_logical_address_pool_t* pool);

/**
 * @brief Allocates an available logical address based on the device type.
 *
 * @param pool Pointer to the logical address pool.
 * @param device_type The type of device for which to allocate a logical address.
 * @return Allocated logical address.
 */
vcCommand_logical_address_t vcDevice_AllocateLogicalAddress(vcDevice_logical_address_pool_t *pool, vcCommand_device_type_t device_type);

/**
 * @brief Releases an already allocated logical address back to the pool.
 *
 * @param pool Pointer to the logical address pool.
 * @param address The logical address to be released.
 */
void vcDevice_ReleaseLogicalAddress(vcDevice_logical_address_pool_t *pool, vcCommand_logical_address_t address);

#endif //__VCDEVICE_H
