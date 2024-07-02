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

#ifndef __VCHDMICEC_DEVICE_H
#define __VCHDMICEC_DEVICE_H

#include "vcomponent_hdmi_cec.h"
#include "vcCommand.h"
#include "ut_kvp.h"

#define MAX_OSD_NAME_LENGTH 16

typedef struct {
    bool allocated[LOGICAL_ADDRESS_BROADCAST];
} vCHdmiCec_logical_address_pool_t;

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
  char* str;
  int val;
} strVal_t;

/**
 * @brief Creates a map of devices in a parent-child n-ary tree.
 *
 * Recursively loads the device map from the provided profile instance.
 * To load from the root device, the prefix should be "hdmicec/device_map/0".
 *
 * @param instance Pointer to the profile instance from which to load the device map.
 * @param profile_prefix Prefix for the profile keys to load the device map.
 * @return Pointer to the newly created vCHdmiCec_device_info_t structure.
 */
struct vCHdmiCec_device_info_t* vCHdmiCec_Device_CreateMapFromProfile (ut_kvp_instance_t* instance, char* profile_prefix);

/**
 * @brief Destroys the device map from the root to all children/siblings.
 *
 * Frees up the memory used by the device map.
 *
 * @param map Pointer to the root of the device map to be destroyed.
 */
void vCHdmiCec_Device_DestroyMap(struct vCHdmiCec_device_info_t* map);

/**
 * @brief Prints the device map recursively.
 *
 * To print devices from the root, pass level = 0.
 *
 * @param map Pointer to the root of the device map to be printed.
 * @param level The level in the hierarchy from which to start printing.
 */
void vCHdmiCec_Device_PrintMap(struct vCHdmiCec_device_info_t* map, int level);

/**
 * @brief Resets a device to its initial values.
 *
 * @param device Pointer to the device to be reset.
 */
void vCHdmiCec_Device_Reset (struct vCHdmiCec_device_info_t* device);

/**
 * @brief Adds a new child to the parent device.
 *
 * @param parent Pointer to the parent device.
 * @param child Pointer to the child device to be added.
 */
void vCHdmiCec_Device_InsertChild(struct vCHdmiCec_device_info_t* parent, struct vCHdmiCec_device_info_t* child);

/**
 * @brief Removes a child from the parent device.
 *
 * Removing a child device will remove all devices connected through the child.
 *
 * @param map Pointer to the root of the device map.
 * @param name Name of the child device to be removed.
 */
void vCHdmiCec_Device_RemoveChild(struct vCHdmiCec_device_info_t* map, char* name);

/**
 * @brief Finds a device by its name.
 *
 * @param map Pointer to the root of the device map.
 * @param name Name of the device to be found.
 * @return Pointer to the device if found, NULL otherwise.
 */
struct vCHdmiCec_device_info_t* vCHdmiCec_Device_Get(struct vCHdmiCec_device_info_t* map, char* name);

/**
 * @brief Initializes the logical address pool.
 *
 * @param pool Pointer to the logical address pool to be initialized.
 */
void vCHdmiCec_Device_InitLogicalAddressPool(vCHdmiCec_logical_address_pool_t *pool);

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
void vCHdmiCec_Device_AllocatePhysicalLogicalAddresses(struct vCHdmiCec_device_info_t *map, struct vCHdmiCec_device_info_t *emulated_device, vCHdmiCec_logical_address_pool_t* pool);

/**
 * @brief Allocates an available logical address based on the device type.
 *
 * @param pool Pointer to the logical address pool.
 * @param device_type The type of device for which to allocate a logical address.
 * @return Allocated logical address.
 */
vCHdmiCec_logical_address_t vCHdmiCec_Device_AllocateLogicalAddress(vCHdmiCec_logical_address_pool_t *pool, vCHdmiCec_device_type_t device_type);

/**
 * @brief Releases an already allocated logical address back to the pool.
 *
 * @param pool Pointer to the logical address pool.
 * @param address The logical address to be released.
 */
void vCHdmiCec_Device_ReleaseLogicalAddress(vCHdmiCec_logical_address_pool_t *pool, vCHdmiCec_logical_address_t address);

/**
 * @brief Gets the value (int - enum) associated with the string from the provided array of strVal_t.
 *
 * If no match is found in the array, default_val is returned.
 *
 * @param map Pointer to the array of strVal_t.
 * @param length Length of the array.
 * @param str The string to be matched.
 * @param default_val The default value to be returned if no match is found.
 * @return The value associated with the string, or default_val if no match is found.
 */
int vCHdmiCec_GetValue(const strVal_t *map, int length, char* str, int default_val);

/**
 * @brief Gets the string associated with the value (int - enum) from the provided array of strVal_t.
 *
 * If no match is found for the value, NULL is returned.
 *
 * @param map Pointer to the array of strVal_t.
 * @param length Length of the array.
 * @param val The value to be matched.
 * @return The string associated with the value, or NULL if no match is found.
 */
char* vCHdmiCec_GetString(const strVal_t *map, int length, int val);

#endif //__VCHDMICEC_DEVICE_H
