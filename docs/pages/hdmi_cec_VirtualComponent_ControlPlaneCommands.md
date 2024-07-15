## Control Plane Commands
Hdmi Cec virtual component allows CEC commands to be triggered by a test user through the control plane. Below are the different types of control plane messages
that can be triggered.

1. **Command**
HDMI Consumer Electronics Control (CEC) commands are designed to allow control of multiple devices through a single remote control and enable devices to communicate with each other. These commands are categorized based on their functionalities and the end-user features they support.

#### Example YAML CEC Command

```yaml
---
hdmicec:
  command: SetOSDName
  initiator: Sony HomeTheatre
  destination: IPSTB
  parameters:
    osd_name: IPSTB
```

2. **Event**
   This category of messages allow the test user to trigger changes in the virtual component state machine.
   
#### Example Event
Command to trigger a hotplug event from device connected in Port 2:
```yaml
hdmicec:
    command: hotplug
    port: 2
    connected: false
```

3. **Config**
Test user can also trigger a re-configuration of the initial profile with which the emulator state machine was set up, like the device type (Sink or Source) and the list of devices in the network etc.

#### Example config change trigger
```yaml
---
hdmicec:
  config:
    emulated_device: IPSTB
    number_ports: 1
    ports:
      - id: 1
        type: out
        cec_supported: true
        arc_supported: false
    number_devices: 3
    device_map:
      - name: Sony TV
        type: TV
        version: 4
        active_source: false
        vendor_info: SONY
        pwr_status: on
        number_chlidren: 2
        children:
         -  name: IPSTB
            type: PlaybackDevice
            version: 4 
            active_source: true
            vendor: TEST_VENDOR
            pwr_status: on
            port_id: 1
            num_children: 0
            
         -  name: Chromecast
            type: PlaybackDevice
            version: 4
            active_source: false
            vendor: GOOGLE
            pwr_status: on
            port_id: 1
            num_children: 0
```
### Anatomy of the YAML CEC Command

The YAML representation of CEC commands provides a structured and human-readable format for defining and triggering HDMI-CEC commands.
These YAML commands closely follow the CEC specifications but are more descriptive in nature.

```yaml
---
hdmicec:
  command: # The CEC command to be executed (e.g., SetOSDName, ActiveSource, UserControlPressed, etc.)
  initiator: # The device sending the command (e.g., Sony HomeTheatre, IPSTB, TV, etc.)
  destination: # The device receiving the command (e.g., TV, IPSTB, Broadcast, etc.)
  parameters: # Optional: Additional parameters required for the command
    # Add specific parameters below as needed for the command
    osd_name: # Example: OSD name to be set (for SetOSDName command)
    device_name: # Example: Physical address of the device (for ActiveSource or ReportPhysicalAddress commands)
    ui_command: # Example: User interface command (for UserControlPressed command)
    device_type: # Example: Device type (for ReportPhysicalAddress command)
    deck_info: # Example: Deck status information (for DeckStatus command)
    tuner_info: # Example: Tuner status information
```

### Components of the YAML CEC Command

### Root Element (hdmicec):

The root element of the YAML file, indicating that this is a CEC command.

### Command:

The command key specifies the CEC command to be executed.
Example: SetOSDName is the command to set the On-Screen Display name of a device.

### Initiator:

The initiator key indicates the device that is sending the command. All devices are identified by its unique name that is configured in the profile YAML. The virtual component
takes care of converting the device names into physical and logical addresses internaly.

Example: Sony HomeTheatre is the device initiating the command.

### Destination:

The destination key specifies the target device that will receive the command.
Example: IPSTB is the device receiving the command.

### Parameters:

The parameters key holds a dictionary of additional data required for the command.
Example: For the SetOSDName command, the parameters section includes an osd_name key with the value IPSTB.

Below is an explanation of the key components of a YAML CEC command, using an example for clarity.


### Using the Parameters Section in Different CEC Commands

The `parameters` section in the YAML CEC command is used to provide additional data required for executing specific CEC commands. The parameters vary depending on the command being issued. Below are examples and descriptions of how the `parameters` section can be used in different CEC commands.

#### Example 1: SetOSDName

The `SetOSDName` command sets the On-Screen Display (OSD) name for a device. This command requires an `osd_name` parameter to specify the name to be set.

```yaml
---
hdmicec:
  command: SetOSDName
  initiator: Sony HomeTheatre
  destination: IPSTB
  parameters:
    osd_name: IPSTB
```

#### Example 2: ActiveSource

The ActiveSource command is used to indicate which device is currently the active source. This command requires a physical_address parameter.
```yaml
---
hdmicec:
  command: ActiveSource
  initiator: IPSTB
  destination: TV
  parameters:
    physical_address: 0x1000
```

#### Example 3: UserControlPressed

The UserControlPressed command simulates a button press on the remote control. This command requires a ui_command parameter to specify the user interface command.
```yaml
---
hdmicec:
  command: UserControlPressed
  initiator: TV
  destination: IPSTB
  parameters:
    ui_command: Select
```

## HDMI-CEC Command Categories Based on End User Features
HDMI CEC commands are categorized basedon their functionalities and the end-user features they support.

Below are some of the commonly used categories of CEC commands:

#### One Touch Play
Commands in this category are used to turn on devices and switch them to the correct input automatically.
- **ImageViewOn**: Turns on the TV and switches to the input associated with the device.
- **SetTextViewOn**: Turns on the TV and switches to the input for text-based content.

#### Routing Control
These commands manage the routing of signals between devices, ensuring the correct source is displayed on the TV.
- **ActiveSource**: Indicates that a device is the active source.
- **RequestActiveSource**: Requests the active source device to broadcast its address.
- **SetOSDName**: Changes the On-Screen Display (OSD) name of a device.
- **GiveOSDName**: Requests the OSD name from a device.

#### System Information
Commands in this category are used to exchange information about devices, such as addresses and device types.
- **GivePhysicalAddress**: Requests the physical address of the device.
- **ReportPhysicalAddress**: Reports the physical address and device type.

#### Deck Control
These commands control the playback devices, such as recorders and players.
- **DeckStatus**: Reports the deck status, such as Play, Stop, etc.
- **RecordOn**: Initiates recording on a recording device.
- **RecordOff**: Stops recording on a recording device.
- **RecordStatus**: Reports the record status from a device.

#### Tuner Control
Commands that manage the tuner functions of a device, such as selecting channels or services.
- **GiveTunerDeviceStatus**: Requests the tuner device status.
- **SelectDigitalService**: Selects a digital service on the tuner.
- **TunerDeviceStatus**: Reports the tuner device status.

#### OSD Display
These commands manage the On-Screen Display settings and information.
- **GiveOSDName**: Requests the OSD name from a device.
- **SetOSDName**: Changes the OSD name of a device.

#### Device Menu Control
Commands to control the device's menu interface, enabling navigation and selection of menu items.
- **UICommand**: Sends a UI command, such as "show_menu", to a device.

#### Remote Control Passthrough
Allows remote control commands to be sent directly to devices.
- **UserControlPressed**: Sends a user control press event, such as pressing a button.
- **UserControlReleased**: Sends a user control release event.

#### Power Status
These commands manage the power state of devices, such as putting them into standby mode.
- **Standby**: Puts all devices in standby mode.
- **RequestPowerStatus**: Requests the power status of all devices.
- **ReportPowerStatus**: Reports the power status of a device.

#### System Audio Control
Commands to control the audio settings of the system, such as volume and audio mode.
- **GiveSystemAudioModeStatus**: Requests the audio mode status from an audio system.
- **SystemAudioModeStatus**: Reports the system audio mode status.
- **GiveAudioStatus**: Requests the audio status from an audio system.
- **ReportAudioStatus**: Reports the audio status from a device.
