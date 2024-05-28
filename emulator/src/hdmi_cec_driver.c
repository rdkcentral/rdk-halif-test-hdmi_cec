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
#include "emulator.h"
#include "ut_log.h"
#include "ut_kvp_profile.h"
#include <assert.h>

//Todo Decouple this from UT
#define EMU_LOG(format, ...)  UT_logPrefix(__FILE__, __LINE__, UT_LOG_ASCII_YELLOW"EMU   "UT_LOG_ASCII_NC, format, ## __VA_ARGS__)


/**HDMI CEC HAL Data structures */

#define MAX_OSD_NAME_LENGTH 16

typedef enum {
  DEVICE_TYPE_TV = 0,
  DEVICE_TYPE_PLAYBACK,
  DEVICE_TYPE_AUDIO_SYSTEM,
  DEVICE_TYPE_RECORDER,
  DEVICE_TYPE_TUNER,
  DEVICE_TYPE_RESERVED
} device_type_t;

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
} cec_logical_address_t;

typedef enum {
  CEC_VERSION_UNKNOWN = 0,
  CEC_VERSION_1_2 = 1,
  CEC_VERSION_1_2A = 2,
  CEC_VERSION_1_3 = 3,
  CEC_VERSION_1_3A = 4,
  CEC_VERSION_1_4 = 5,
  CEC_VERSION_2_0 = 6
} cec_version_t;

typedef enum {
  PORT_TYPE_INPUT = 0,
  PORT_TYPE_OUTPUT
} hdmi_port_type_t;

typedef enum {
  DEVICE_POWER_STATUS_ON = 0,
  DEVICE_POWER_STATUS_STANDBY,
  DEVICE_POWER_STATUS_OFF
} device_power_status_t;

typedef struct {
  unsigned int id;
  unsigned int physical_address;
  hdmi_port_type_t type;
  bool cec_supported;
  bool arc_supported;
} hdmi_port_info_t;

typedef enum {
  HAL_STATE_CLOSED = 0,
  HAL_STATE_OPEN,
  HAL_STATE_READY
} cec_hal_state_t;

typedef enum {
  CEC_EVENT_HOTPLUG = 1,
  CEC_EVENT_COMMAND,
  CEC_EVENT_CONFIG
} cec_event_type_t;

typedef struct {
  bool connected;
  unsigned int port_id;
} hotplug_event_t;

typedef enum{
  CEC_ACTIVE_SOURCE = 0x82,
  CEC_IMAGE_VIEW_ON = 0x04,
  CEC_TEXT_VIEW_ON = 0x0D,
  CEC_INACTIVE_SOURCE = 0x9D,
  CEC_REQUEST_ACTIVE_SOURCE = 0x85,
  CEC_STANDBY = 0x36,
  CEC_UNKNOWN = 0
} cec_command_t;


typedef struct {
  unsigned char src_logical_addr;
  unsigned char dest_logical_addr;
  cec_command_t command;
} cec_command_event_t;


typedef struct {
  cec_event_type_t type;
  union {
    cec_command_event_t cec_cmd;
    hotplug_event_t hot_plug;
  };
} cec_event_t;

typedef struct {
  char name[MAX_OSD_NAME_LENGTH];
  hdmi_port_type_t type;
  unsigned int portId;
} endpoint_t;

typedef struct {
  endpoint_t ep1;
  endpoint_t ep2;
} link_t;

struct device_info_t {
  struct device_info_t* next;
  struct device_info_t* prev;
  device_type_t type;
  cec_version_t version;
  unsigned int physical_address;
  unsigned int logical_address;
  bool active_source;
  unsigned int vendor_id;
  device_power_status_t power_status;
  char osd_name[MAX_OSD_NAME_LENGTH];
};

typedef struct {
  HdmiCecRxCallback_t rx_cb_func;
  void* rx_cb_data;
  HdmiCecTxCallback_t tx_cb_func;
  void* tx_cb_data;
} cec_callbacks_t;

typedef struct {
  cec_hal_state_t state;
  int num_ports;
  hdmi_port_info_t *ports;
  int num_devices;
  struct device_info_t* devices;
  struct device_info_t* emulated_device;
  int num_links;
  link_t* network;
  cec_callbacks_t callbacks;
  //TODO
  // Eventing Queue, Thread for callback

} hdmi_cec_t;


/**Emulator Data types*/
typedef struct {
  unsigned short cp_port;
  char* cp_path;
  hdmi_cec_t * cec_hal;
} cec_emulator_t;

static cec_emulator_t* gEmulator = NULL;

const static struct  {
  device_type_t val;
  char* str_type;
} gDIMap [] = {
  { DEVICE_TYPE_TV, "TV" },
  { DEVICE_TYPE_PLAYBACK, "PlaybackDevice" },
  { DEVICE_TYPE_AUDIO_SYSTEM, "AudioSystem" },
  { DEVICE_TYPE_RECORDER, "RecordingDevice" },
  { DEVICE_TYPE_TUNER, "Tuner" },
  { DEVICE_TYPE_RESERVED, "Reserved" }
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
  device_power_status_t status;
  char* str;
} gPSMap [] = {
  { DEVICE_POWER_STATUS_ON, "on" },
  { DEVICE_POWER_STATUS_OFF, "off" },
  { DEVICE_POWER_STATUS_STANDBY, "standby" }
};

static char* GetDeviceTypeStr(device_type_t type)
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

static device_type_t GetDeviceType(char *type)
{

  device_type_t result = DEVICE_TYPE_TV;

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

static char* GetDevicePowerStatusStr(device_power_status_t status)
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

static device_power_status_t GetDevicePowerStatus(char *name)
{
  device_power_status_t result = DEVICE_POWER_STATUS_OFF;

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


static hdmi_port_type_t GetPortType(char *type)
{
  hdmi_port_type_t result = PORT_TYPE_OUTPUT;

  if(!strncmp(type, "in", sizeof("in")))
  {
    result = PORT_TYPE_INPUT;
  }
  return result;
}

struct device_info_t* NewDevice(void)
{
  struct device_info_t* device = (struct device_info_t*)malloc(sizeof(struct device_info_t));
  if(device != NULL)
  {
    device->active_source = false;
    device->logical_address = 0x0F;
    device->physical_address = 0x0F;
    device->next = NULL;
    device->prev = NULL;
    device->vendor_id = 0;
  }
  return device;
}


void AddDevice(struct device_info_t** head, struct device_info_t* node)
{
  if(node != NULL)
  {
    if(*head == NULL)
    {
      *head = node;
      node->next = NULL;
      node->prev = NULL;
    }
    else
    {
      struct device_info_t* list = *head;
      while (list != NULL)
      {
        if(list->next == NULL)
        {
          list->next = node;
          node->prev = list;
          break;
        }
        else
        {
          list = list->next;
        }
      }
    }
  }

}

void RemoveDevice(struct device_info_t* head, char* name)
{
  struct device_info_t* list = head;
  while (list != NULL)
  {
    if(!strcmp(list->osd_name, name))
    {
      struct device_info_t* item = list;
      if(list->prev != NULL)
      {
        list->prev->next = list->next;
        if(list->next != NULL)
        {
          list->next->prev = list->prev;
        }
      }
      free(item);
      break;
    }
    list = list->next;
  }
}

void RemoveAllDevices(struct device_info_t* head)
{
  struct device_info_t* list = head;
  assert(list != NULL);
  while (list != NULL)
  {
    struct device_info_t* item = list;
    list = list->next;
    free(item);
  }
}

struct device_info_t* GetDeviceByName (struct device_info_t* head, char* name)
{
  struct device_info_t* list = head;
  struct device_info_t* result = NULL;
  while (list != NULL)
  {
    if(!strcmp(list->osd_name, name))
    {
      result = list;
      break;
    }
    list = list->next;
  }
  return result;
}

void PrintDevices(struct device_info_t* head)
{
  struct device_info_t* list = head;
  EMU_LOG(">>>>>>> >>>>> >>>> >> >> >");
  while (list != NULL)
  {
    EMU_LOG("Device                        : %s", list->osd_name);
    EMU_LOG("Type                          : %s", GetDeviceTypeStr(list->type));
    EMU_LOG("Pwr Status                    : %s", GetDevicePowerStatusStr(list->power_status));
    EMU_LOG("------------------------");
    list = list->next;
  }
  EMU_LOG("==========================\r\n");
}

void LoadDevicesInfo (ut_kvp_instance_t* instance, struct device_info_t* head, unsigned int nDevices)
{
  //We expect atleast one device (the emulated device in the list)
  if (head != NULL && nDevices > 0)
  {
    char *prefix = "hdmicec/devices/";
    char tmp[256];

    for (int i = 0; i < nDevices; ++i)
    {
      char type[16];
      struct device_info_t* device = NewDevice();
      assert(device != NULL);
      
      strcpy(tmp, prefix);
      int length = snprintf( NULL, 0, "%d", i );
      snprintf( tmp + strlen(prefix) , length + 1, "%d", i );

      strcpy(tmp + strlen(prefix) + length, "/name");
      ut_kvp_getStringField(instance, tmp, device->osd_name, MAX_OSD_NAME_LENGTH);

      strcpy(tmp + strlen(prefix) + length, "/active_source");
      device->active_source = ut_kvp_getBoolField(instance, tmp);

      strcpy(tmp + strlen(prefix) + length, "/pwr_status");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      device->power_status = GetDevicePowerStatus(type);

      strcpy(tmp + strlen(prefix) + length, "/version");
      device->version = (cec_version_t) ut_kvp_getUInt32Field(instance, tmp);

      strcpy(tmp + strlen(prefix) + length, "/vendor");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      device->vendor_id = GetVendorCode(type);

      strcpy(tmp + strlen(prefix) + length, "/type");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      device->vendor_id = GetDeviceType(type);

      AddDevice(&head, device);
    }
  }      
}

void LoadPortsInfo (ut_kvp_instance_t* instance, hdmi_port_info_t* ports, unsigned int nPorts)
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

void LoadNetwork (ut_kvp_instance_t* instance, link_t* network, unsigned int nlinks)
{
  if (network != NULL && nlinks > 0)
  {
    char *prefix = "hdmicec/links/";
    char tmp[256];

    for (int i = 0; i < nlinks; ++i)
    {
      char type[8];
      strcpy(tmp, prefix);
      int length = snprintf( NULL, 0, "%d", i );
      snprintf( tmp + strlen(prefix) , length + 1, "%d", i );

      strcpy(tmp + strlen(prefix) + length, "/0/type");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      network[i].ep1.type = GetPortType(type);

      strcpy(tmp + strlen(prefix) + length, "/0/name");
      ut_kvp_getStringField(instance, tmp, network[i].ep1.name, MAX_OSD_NAME_LENGTH);

      strcpy( tmp + strlen(prefix) + length  , "/0/id");
      network[i].ep1.portId = ut_kvp_getUInt32Field(instance, tmp);

      strcpy(tmp + strlen(prefix) + length, "/1/type");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      network[i].ep2.type = GetPortType(type);

      strcpy(tmp + strlen(prefix) + length, "/1/name");
      ut_kvp_getStringField(instance, tmp, network[i].ep2.name, MAX_OSD_NAME_LENGTH);

      strcpy( tmp + strlen(prefix) + length  , "/1/id");
      network[i].ep2.portId = ut_kvp_getUInt32Field(instance, tmp);

    }
  }
}

void LoadEmulatedDeviceInfo (ut_kvp_instance_t* instance, struct device_info_t* device_info)
{
  if(device_info != NULL)
  {
    ut_kvp_getStringField(instance, "hdmicec/name", device_info->osd_name,MAX_OSD_NAME_LENGTH);
    device_info->active_source = ut_kvp_getBoolField(instance, "hdmicec/active_source");
    char temp[128];

    ut_kvp_getStringField(instance, "hdmicec/pwr_status", temp, sizeof(temp));
    device_info->power_status = GetDevicePowerStatus(temp);

    device_info->version = (cec_version_t) ut_kvp_getUInt32Field(instance, "hdmicec/version");

    ut_kvp_getStringField(instance, "hdmicec/vendor", temp, sizeof(temp));
    device_info->vendor_id = GetVendorCode(temp);

    ut_kvp_getStringField(instance, "hdmicec/type", temp, sizeof(temp));
    device_info->type = GetDeviceType(temp);
  }
  else
  {
    EMU_LOG("LoadEmulatedDeviceInfo: Invalid Device");
  }
}

void ResetHal(hdmi_cec_t* cec)
{
  if(cec != NULL)
  {
    cec->ports = NULL;
    cec->devices = NULL;
    cec->emulated_device = NULL;
    cec->callbacks.rx_cb_func = NULL;
    cec->callbacks.tx_cb_func = NULL;
    cec->callbacks.rx_cb_data = NULL;
    cec->callbacks.tx_cb_data = NULL;
    cec->network = NULL;
    cec->num_devices = cec->num_links = cec->num_ports = 0;
    cec->state = HAL_STATE_CLOSED;
  }
}

void TeardownHal (hdmi_cec_t* hal)
{
  if(hal != NULL)
  {
    //Stop all events
    //Exit Eventing

    if(hal->devices)
    {
      RemoveAllDevices (hal->devices);
    }
    if(hal->ports)
    {
      free (hal->ports);
    }
    if(hal->network)
    {
      free(hal->network);
    }
    ResetHal(hal);
    free(hal);
  }
}


Emulator_t *Emulator_Initialize(char* pProfilePath, unsigned short pCPPort, char* pCPUrl)
{
  cec_emulator_t *result = NULL;
  ut_kvp_status_t status;
  
  if(gEmulator == NULL)
  {
    if ( pProfilePath != NULL && pCPPort > 0)
    {
      result = (cec_emulator_t*)malloc(sizeof(cec_emulator_t));
      if(result != NULL)
      {
        result->cp_path = pCPUrl;
        result->cp_port = pCPPort;
        status = ut_kvp_profile_open(pProfilePath);
        EMU_LOG("ut_kvp_profile_open: status: %d", status);
        assert(status == UT_KVP_STATUS_SUCCESS);
        result->cec_hal = NULL;

        //TODO Control Plane init
      }
    }
    else
    {
      EMU_LOG("Emulator_Initialize: Invalid Profile path and/or Control plane Port");
    }

    if ( result != NULL )
    {
      gEmulator = result;
    }
  }
  else
  {
    EMU_LOG("Emulator_Initialize: already initialised");
  }
  return (Emulator_t *) result;

}

void Emulator_Deinitialize(Emulator_t *pEmulator)
{
  cec_emulator_t* emulator = (cec_emulator_t*)pEmulator;
  assert(emulator != gEmulator);

  if(emulator != NULL)
  {
    if(emulator->cec_hal != NULL && emulator->cec_hal->state != HAL_STATE_CLOSED)
    {
      HdmiCecClose((int)emulator->cec_hal);
    }
    ut_kvp_profile_close();
    free(emulator);
    gEmulator = NULL;
    EMU_LOG("Emulator_Deinitialize: Success");
  }
  else
  {
    EMU_LOG("Emulator_Deinitialize: already deinitialized");
  }
}

HDMI_CEC_STATUS HdmiCecOpen(int* handle)
{
  HDMI_CEC_STATUS result = HDMI_CEC_IO_NOT_OPENED;
  if(handle == NULL)
  {
    result = HDMI_CEC_IO_INVALID_HANDLE;
  }
  else if (gEmulator != NULL && gEmulator->cec_hal == NULL) {

    EMU_LOG("HdmiCecOpen: Loading emulated Device Info");
    ut_kvp_instance_t *profile_instance = ut_kvp_profile_getInstance();
    hdmi_cec_t* cec = (hdmi_cec_t*)malloc(sizeof(hdmi_cec_t));
    if(cec != NULL) {
     
      ResetHal(cec);
      struct device_info_t* eDevice = NewDevice();
      assert(eDevice != NULL);
      LoadEmulatedDeviceInfo(profile_instance, eDevice);
      cec->emulated_device = eDevice;
      AddDevice(&(cec->devices) ,cec->emulated_device);

      cec->num_ports = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_ports");
      if(cec->num_ports > 0)
      {
        EMU_LOG("HdmiCecOpen: Loading Ports Info");
        hdmi_port_info_t* ports = (hdmi_port_info_t*) malloc(sizeof(hdmi_port_info_t) * cec->num_ports);
        LoadPortsInfo(profile_instance, ports, cec->num_ports);
        cec->ports = ports;
      }

      //Device Discovery and Network Topology
      cec->num_devices = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_devices");
      if(cec->num_devices > 0)
      {
        EMU_LOG("HdmiCecOpen: Loading connected Device Info");
        LoadDevicesInfo(profile_instance, cec->devices, cec->num_devices);
      }
      else
      {
        EMU_LOG("HdmiCecOpen: No Devices connected");
      }

      if(cec->emulated_device->type == DEVICE_TYPE_TV)
      { 
        EMU_LOG("HdmiCecOpen: Emulating a TV");
        cec->emulated_device->physical_address = 0;
        cec->emulated_device->logical_address = 0x0F;
      }
      else
      {
        EMU_LOG("HdmiCecOpen: Emulating a Source device");
        //TODO Auto Allocate Logical addresses
      }

      cec->num_links = ut_kvp_getUInt32Field(profile_instance, "hdmicec/number_links");
      if(cec->num_links > 0)
      {
        link_t* network = (link_t*) malloc(sizeof(link_t) * cec->num_links);
        EMU_LOG("HdmiCecOpen: Loading network of devices");
        LoadNetwork(profile_instance, network, cec->num_links);
        cec->network = network;
      }

      EMU_LOG(">>>>>>> >>>>> >>>> >> >> >");
      EMU_LOG("Emulated Device               : %s", cec->emulated_device->osd_name);
      EMU_LOG("Type                          : %s", GetDeviceTypeStr(cec->emulated_device->type));
      EMU_LOG("Pwr Status                    : %s", GetDevicePowerStatusStr(cec->emulated_device->power_status));
      EMU_LOG("Number of Ports               : %d", cec->num_ports);
      EMU_LOG("Number of discovered devices  : %d", cec->num_devices);
      EMU_LOG("Number of links               : %d", cec->num_links);
      EMU_LOG("==========================\n");

      PrintDevices(cec->devices->next);

      *handle = (int) cec;
      gEmulator->cec_hal = cec;
      cec->state = HAL_STATE_READY;
      result = HDMI_CEC_IO_SUCCESS;
    } 
    else
    {
       result = HDMI_CEC_IO_GENERAL_ERROR;
    }
  }
  else if(gEmulator->cec_hal != NULL)
  {
    EMU_LOG("HdmiCecOpen: Already opened");
    result = HDMI_CEC_IO_ALREADY_OPEN;
  }
  else
  {
    EMU_LOG("HdmiCecOpen: Emulator not Initialised");
  }

  EMU_LOG("HdmiCecOpen: result - %d", result);
  return result;
}

HDMI_CEC_STATUS HdmiCecClose(int handle)
{
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
      TeardownHal(gEmulator->cec_hal);
      gEmulator->cec_hal = NULL;
      status = HDMI_CEC_IO_SUCCESS;
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecClose - status %d",status);
  return status;
}


HDMI_CEC_STATUS HdmiCecGetPhysicalAddress(int handle, unsigned int* physicalAddress)
{
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    if(physicalAddress != NULL)
    {
      *physicalAddress = gEmulator->cec_hal->emulated_device->physical_address;
      status = HDMI_CEC_IO_SUCCESS;
    }
    else
    {
      status = HDMI_CEC_IO_INVALID_ARGUMENT;
    }
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecGetPhysicalAddress - status %d",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    if(gEmulator->cec_hal->emulated_device->type == DEVICE_TYPE_TV && logicalAddresses == 0)
    {
      //ADD Logical Address only for Sink device
      gEmulator->cec_hal->emulated_device->logical_address = logicalAddresses;
      status = HDMI_CEC_IO_SUCCESS;
    }
    else
    {
      status = HDMI_CEC_IO_INVALID_ARGUMENT;
    }
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecAddLogicalAddress - status %d",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{

  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    //Remove Logical Address only for Sink device
    if(gEmulator->cec_hal->emulated_device->type == DEVICE_TYPE_TV && logicalAddresses == 0)
    {
      if(gEmulator->cec_hal->emulated_device->logical_address == 0x0F)
      {
        //Looks like logical address is already removed. 
        status = HDMI_CEC_IO_NOT_ADDED;
      }
      else
      {
        //Reset back to 0x0F
        gEmulator->cec_hal->emulated_device->logical_address = 0x0F;
        status = HDMI_CEC_IO_SUCCESS;
      }
    }
    else
    {
      //Return Invalid argument for source devices or if logical adddress removed is other than 0
      status = HDMI_CEC_IO_INVALID_ARGUMENT;
    }
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  return status;
}

HDMI_CEC_STATUS HdmiCecGetLogicalAddress(int handle, int* logicalAddress)
{
  EMU_LOG("HdmiCecGetLogicalAddress handle[%d]", handle);

  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    if(logicalAddress != NULL)
    {
      *logicalAddress = gEmulator->cec_hal->emulated_device->logical_address;
      status = HDMI_CEC_IO_SUCCESS;
    }
    else{
      status = HDMI_CEC_IO_INVALID_ARGUMENT;
    }
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecGetLogicalAddress - status %d",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    gEmulator->cec_hal->callbacks.rx_cb_func = cbfunc;
    gEmulator->cec_hal->callbacks.rx_cb_data = data;
    status = HDMI_CEC_IO_SUCCESS;
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecSetRxCallback - status %d",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void* data)
{
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    gEmulator->cec_hal->callbacks.tx_cb_func = cbfunc;
    gEmulator->cec_hal->callbacks.tx_cb_data = data;
    status = HDMI_CEC_IO_SUCCESS;
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecSetTxCallback - status %d",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecTx(int handle, const unsigned char* buf, int len, int* result)
{
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;
  if(handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    if(gEmulator->cec_hal->emulated_device->type == DEVICE_TYPE_TV && gEmulator->cec_hal->emulated_device->logical_address == 0x0F)
    {
      //If Logical Address is not set for a sink device, we cannot transmit
      status = HDMI_CEC_IO_SENT_FAILED;
    }
    else if(buf != NULL && len > 0 && result != NULL)
    {
      int i = 0;
      EMU_LOG(">>>>>>> >>>>> >>>> >> >> >");
      EMU_LOG("HdmiCecTx: ");
      for (i = 0; i < len; i++) {
        EMU_LOG("%02X ", buf[i]);
      }
      EMU_LOG("==========================");
      *result = HDMI_CEC_IO_SENT_BUT_NOT_ACKD;
      status = HDMI_CEC_IO_SUCCESS;
    }
    else
    {
      status = HDMI_CEC_IO_INVALID_ARGUMENT;
    }
  }
  else if(gEmulator == NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecTx - status %d",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecTxAsync(int handle, const unsigned char* buf, int len)
{

  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;
  if(handle != 0 && gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    if((gEmulator->cec_hal->emulated_device->type == DEVICE_TYPE_TV
        && gEmulator->cec_hal->emulated_device->logical_address == 0x0F)
        || gEmulator->cec_hal->callbacks.tx_cb_func == NULL)
    {
      status = HDMI_CEC_IO_SENT_FAILED;
    }
    else if(buf != NULL && len > 0)
    {
      int i = 0;
      EMU_LOG(">>>>>>> >>>>> >>>> >> >> >");
      EMU_LOG("HdmiCecTxAsync: ");
      for (i = 0; i < len; i++) {
        EMU_LOG("%02X ", buf[i]);
      }
      EMU_LOG("==========================");
      status = HDMI_CEC_IO_SUCCESS;
    }
    else
    {
      status = HDMI_CEC_IO_INVALID_ARGUMENT;
    }
  }
  else if( gEmulator==NULL || gEmulator->cec_hal == NULL)
  {
    status = HDMI_CEC_IO_NOT_OPENED;
  }
  EMU_LOG("HdmiCecTxAsync - status %d",status);
  return status;
}

