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
#include "ut_kvp.h"

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
  unsigned short id;
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
   device_type_t type;
   cec_version_t version;
   unsigned int physical_address;
   unsigned char logical_address;
   bool active_source;
   unsigned int vendor_id;
   device_power_status_t power_status;
   char osd_name[MAX_OSD_NAME_LENGTH];
} device_info_t;

typedef struct {
  HdmiCecRxCallback_t rx_cb_func;
  void* rx_cb_data;
  HdmiCecTxCallback_t tx_cb_func;
  void* tx_cb_data;
} cec_callbacks_t;


typedef struct {
  cec_hal_state_t state;

  device_info_t emulated_device;
  int num_ports;
  hdmi_port_info_t *ports;
  int num_devices;
  device_info_t* connected_devices;

  cec_callbacks_t callbacks;
  //TODO
  // Eventing Queue, Thread for callback

  //TODO
  //Handle of Profile config KVP
} hdmi_cec_t;


/**Emulator Data types*/

typedef struct {
  unsigned short cp_port;
  char* cp_path;
  hdmi_cec_t * cec_hal;
  ut_kvp_instance_t *profile_instance;
} cec_emulator_t;

static cec_emulator_t* gEmulator = NULL;



static device_type_t GetDeviceType(char *type)
{
  const static struct  {
    device_type_t val;
    const char* str_type;
  } dimap [] = {
    { DEVICE_TYPE_TV, "TV" },
    { DEVICE_TYPE_PLAYBACK, "PlaybackDevice" },
    { DEVICE_TYPE_AUDIO_SYSTEM, "AudioSystem" },
    { DEVICE_TYPE_RECORDER, "RecordingDevice" },
    { DEVICE_TYPE_TUNER, "Tuner" },
    { DEVICE_TYPE_RESERVED, "Reserved" }
  };

  device_type_t result = DEVICE_TYPE_TV;

  for (int i = 0;  i < sizeof (dimap) / sizeof (dimap[0]);  ++i)
  {
    if (!strcmp (type, dimap[i].str_type))
    {
        result = dimap[i].val;    
    }
  }

  return result;
}

static unsigned int GetVendorCode(const char *name)
{
  const static struct  {
    const char* name;
    unsigned int code;
  } vcmap [] = {
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

  unsigned int result = 0;

  for (int i = 0;  i < sizeof (vcmap) / sizeof (vcmap[0]);  ++i)
  {
    if (!strcmp (name, vcmap[i].name))
    {
        result = vcmap[i].code;    
    }
  }

  return result;
}

static device_power_status_t GetDevicePowerStatus(char *status)
{
  device_power_status_t result = DEVICE_POWER_STATUS_OFF;
  if(!strncmp(status, "on", sizeof("on")))
  {
    result = DEVICE_POWER_STATUS_ON;
  }
  else if (!strncmp(status, "standby", sizeof("standby")))
  {
    result = DEVICE_POWER_STATUS_STANDBY;
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


void LoadDevicesInfo (ut_kvp_instance_t* instance, device_info_t* devices, unsigned int nDevices)
{
  if (devices != NULL && nDevices > 0)
  {
    char *prefix = "hdmicec/devices/";
    char tmp[256];

    for (int i = 0; i < nDevices; ++i)
    {
      char type[16];
      strcpy(tmp, prefix);
      int length = snprintf( NULL, 0, "%d", i );
      snprintf( tmp + strlen(prefix) , length + 1, "%d", i );

      strcpy(tmp + strlen(prefix) + length, "/name");
      ut_kvp_getStringField(instance, tmp, devices[i].osd_name, MAX_OSD_NAME_LENGTH);

      strcpy(tmp + strlen(prefix) + length, "/active_source");
      devices[i].active_source = ut_kvp_getBoolField(instance, tmp);

      strcpy(tmp + strlen(prefix) + length, "/pwr_status");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      devices[i].power_status = GetDevicePowerStatus(type);

      strcpy(tmp + strlen(prefix) + length, "/version");
      devices[i].version = (cec_version_t) ut_kvp_getUInt32Field(instance, tmp);

      strcpy(tmp + strlen(prefix) + length, "/vendor");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      devices[i].vendor_id = GetVendorCode(type);

      strcpy(tmp + strlen(prefix) + length, "/type");
      ut_kvp_getStringField(instance, tmp, type, sizeof(type));
      devices[i].vendor_id = GetDeviceType(type);

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

void LoadEmulatedDeviceInfo (ut_kvp_instance_t* instance, device_info_t* device_info)
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


ut_kvp_instance_t* LoadProfile(const char* path)
{
  ut_kvp_instance_t* instance = NULL;
  instance = ut_kvp_createInstance();
  if( ut_kvp_open(instance, path) == UT_KVP_STATUS_SUCCESS )
  {
    EMU_LOG("LoadProfile: [%s] Loaded", path);
  }
  else
  {
    ut_kvp_destroyInstance(instance);
    instance = NULL;
  }
  return instance;
}

void TeardownHal (hdmi_cec_t* hal)
{
  if(hal != NULL)
  {
    //Stop all events
    //Exit Eventing


    free (hal->connected_devices);
    hal->callbacks.rx_cb_func = NULL;
    hal->callbacks.tx_cb_func = NULL;
    hal->state = HAL_STATE_CLOSED;
  }
}


Emulator_t *Emulator_Initialize(char* pProfilePath, unsigned short pCPPort, char* pCPUrl)
{
  cec_emulator_t *result = NULL;
  
  if(gEmulator == NULL)
  {
    if ( pProfilePath != NULL && pCPPort > 0)
    {
      result = (cec_emulator_t*)malloc(sizeof(cec_emulator_t));
      if(result != NULL)
      {
        result->cp_path = pCPUrl;
        result->cp_port = pCPPort;

        result->profile_instance = LoadProfile(pProfilePath);
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
  if(pEmulator != NULL)
  {
     
  }
}



HDMI_CEC_STATUS HdmiCecOpen(int* handle)
{
  HDMI_CEC_STATUS result = HDMI_CEC_IO_NOT_OPENED;
  EMU_LOG("HdmiCecOpen: \n");

  if (gEmulator != NULL && gEmulator->cec_hal == NULL) {

    EMU_LOG("HdmiCecOpen: Loading emulated Device Info\n");

    hdmi_cec_t* cec = (hdmi_cec_t*)malloc(sizeof(hdmi_cec_t));
    if(cec != NULL) {
      LoadEmulatedDeviceInfo(gEmulator->profile_instance, &(cec->emulated_device));
      cec->num_ports = ut_kvp_getUInt32Field(gEmulator->profile_instance, "hdmicec/number_ports");
      EMU_LOG("HdmiCecOpen: Loading Ports Info\n");
      hdmi_port_info_t* ports = (hdmi_port_info_t*) malloc(sizeof(hdmi_port_info_t) * cec->num_ports);
      LoadPortsInfo(gEmulator->profile_instance, ports, cec->num_ports);
      cec->ports = ports;

      //Setup Eventing and callback


      //Device Discovery and Network Topology
      cec->num_devices = ut_kvp_getUInt32Field(gEmulator->profile_instance, "hdmicec/number_devices");

      device_info_t* devices = (device_info_t*) malloc(sizeof(device_info_t) * cec->num_devices);
      EMU_LOG("HdmiCecOpen: Loading connected Device Info\n");
      LoadDevicesInfo(gEmulator->profile_instance, devices, cec->num_devices);
      cec->connected_devices = devices;
      if(cec->emulated_device.type == DEVICE_TYPE_TV)
      { 
        EMU_LOG("HdmiCecOpen: Emulating a TV");
        cec->emulated_device.physical_address = 0;
        cec->emulated_device.logical_address = 0;
      }
      else
      {
        EMU_LOG("HdmiCecOpen: Emulating a Source device");
        //TODO Auto Allocate Logical addresses
      }

      *handle = (int) cec;
      EMU_LOG("HdmiCecOpen: cec[%p], int[%p]", cec, (hdmi_cec_t*)(*handle));
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
  if(gEmulator != NULL && gEmulator->cec_hal != NULL)
  {
    TeardownHal(gEmulator->cec_hal);
  }
  return (int)0;
}

/*
HDMI_CEC_STATUS HdmiCecSetLogicalAddress(int handle, int* logicalAddresses, int num)
{
  hdmi_cec_t *hal = (hdmi_cec_t*)handle;
  if (hal != NULL && hal == gEmulator->cec_hal)
  {
    //TODO: Is more than 1 logical address expected?
    hal->emulated_device.logical_address = logicalAddresses[0];
  }
  return (int)0;
}*/

HDMI_CEC_STATUS HdmiCecGetPhysicalAddress(int handle, unsigned int* physicalAddress)
{
  hdmi_cec_t *hal = (hdmi_cec_t*)handle;
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (hal != NULL)
  {
    *physicalAddress = hal->emulated_device.physical_address;
  }
  return status;
}

HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
   UT_LOG("HdmiCecAddLogicalAddress\n");
  hdmi_cec_t *hal = (hdmi_cec_t*)handle;
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;
  UT_LOG("HdmiCecAddLogicalAddress - hal[%p] - g[%p]\n", hal, gEmulator->cec_hal);
  if (hal != NULL)
  {
    if(hal->emulated_device.type != DEVICE_TYPE_TV)
    {
      //ADD Logical Address only for source device
      status = HDMI_CEC_IO_SUCCESS;
    }
  }
  UT_LOG("HdmiCecAddLogicalAddress - status %d\n",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{
  hdmi_cec_t *hal = (hdmi_cec_t*)handle;
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (hal != NULL)
  {
    if(hal->emulated_device.type != DEVICE_TYPE_TV)
    {
      //ADD Logical Address only for source device
      status = HDMI_CEC_IO_SUCCESS;
    }
  }
  return status;
}

HDMI_CEC_STATUS HdmiCecGetLogicalAddress(int handle, int* logicalAddress)
{
  UT_LOG("HdmiCecGetLogicalAddress\n");
  hdmi_cec_t *hal = (hdmi_cec_t*)handle;
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (hal != NULL)
  {
    *logicalAddress = hal->emulated_device.logical_address;
    status = HDMI_CEC_IO_SUCCESS;
  }
  UT_LOG("HdmiCecGetLogicalAddress - status %d\n",status);
  return status;
}

HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  hdmi_cec_t *hal = (hdmi_cec_t*)handle;
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (hal != NULL && cbfunc != NULL)
  {
    hal->callbacks.rx_cb_func = cbfunc;
    hal->callbacks.rx_cb_data = data;
    status = HDMI_CEC_IO_SUCCESS;
  }
  return status;
}

HDMI_CEC_STATUS HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void* data)
{
  hdmi_cec_t *hal = (hdmi_cec_t*)handle;
  HDMI_CEC_STATUS status = HDMI_CEC_IO_INVALID_HANDLE;

  if (hal != NULL && cbfunc != NULL)
  {
    hal->callbacks.tx_cb_func = cbfunc;
    hal->callbacks.tx_cb_data = data;
    status = HDMI_CEC_IO_SUCCESS;
  }
  return status;
}

HDMI_CEC_STATUS HdmiCecTx(int handle, const unsigned char* buf, int len, int* result)
{
  EMU_LOG("HdmiCecTx: Tx: ");

  return (int)0;
}

HDMI_CEC_STATUS HdmiCecTxAsync(int handle, const unsigned char* buf, int len)
{

  return (int)0;
}

