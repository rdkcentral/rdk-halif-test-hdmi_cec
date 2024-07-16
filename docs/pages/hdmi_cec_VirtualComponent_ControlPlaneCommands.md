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
### One Touch Play

| Command        | YAML Payload                                                                                                                              | Action                                                                                       | How to Trigger in Real Setup                                                |
|----------------|-------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `ImageViewOn`  | <pre lang="yaml">---&#13;hdmicec:&#13;  command: ImageViewOn&#13;  initiator: Sony HomeTheatre&#13;  destination: TV</pre>                | Turns on the TV and switches to the correct HDMI input                                       | Using a playback device like a Blu-ray player or streaming device.            |
| `ActiveSource` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: ActiveSource&#13;  initiator: IPSTB&#13;  destination: TV&#13;  parameters:&#13;    physical_address: 0x1000</pre> | Indicates which device is currently the active source                                        | Automatically triggered when a device becomes the active source             |
| `TextViewOn`   | <pre lang="yaml">---&#13;hdmicec:&#13;  command: TextViewOn&#13;  initiator: TV&#13;  destination: Sony HomeTheatre</pre>                 | Turns on the TV with the text view                                                           | Using a TV remote control to turn on the TV and switch to text view mode    |

#### One Touch Record

Allows whatever is shown on the TV screen to be recorded on a selected Recording Device.

| Command        | YAML Payload                                                                                                                              | Action                                                                                       | How to Trigger in Real Setup                                                |
|----------------|-------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `RecordOn`     | <pre lang="yaml">---&#13;hdmicec:&#13;  command: RecordOn&#13;  initiator: TV&#13;  destination: RecordingDevice1</pre>                    | Starts recording on the specified device                                                     | Using a TV remote control to start recording                                |
| `RecordOff`    | <pre lang="yaml">---&#13;hdmicec:&#13;  command: RecordOff&#13;  initiator: TV&#13;  destination: RecordingDevice1</pre>                   | Stops recording on the specified device                                                      | Using a TV remote control to stop recording                                 |
| `RecordStatus` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: RecordStatus&#13;  initiator: TV&#13;  destination: RecordingDevice1&#13;  parameters:&#13;    record_status: Recording</pre> | Reports the recording status of the device                                                   | Automatically triggered when recording status changes                       |
| `RecordTVScreen` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: RecordTVScreen&#13;  initiator: TV&#13;  destination: RecordingDevice1</pre>             | Commands the recording device to record the current TV screen                                | Using a TV remote control to start recording the TV screen                  |


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
### Deck Control

| Command          | YAML Payload                                                                                                                                         | Action                                                                                       | How to Trigger in Real Setup                                               |
|------------------|------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------|----------------------------------------------------------------------------|
| `DeckControl`    | <pre lang="yaml">---&#13;hdmicec:&#13;  command: DeckControl&#13;  initiator: TV&#13;  destination: PlaybackDevice1&#13;  parameters:&#13;    deck_info: Play</pre> | Controls deck functions such as play, pause, etc.                                            | Using a TV remote control to control playback functions                    |
| `GiveDeckStatus` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: GiveDeckStatus&#13;  initiator: TV&#13;  destination: PlaybackDevice1&#13;  parameters:&#13;    status_request: 1</pre> | Requests the current status of the playback device                                           | Automatically triggered by TV to get the status of the playback device     |
| `Play`           | <pre lang="yaml">---&#13;hdmicec:&#13;  command: Play&#13;  initiator: TV&#13;  destination: PlaybackDevice1&#13;  parameters:&#13;    play_mode: PlayForward</pre> | Starts playback on the specified playback device                                             | Using a TV remote control to play content                                  |
| `DeckStatus`     | <pre lang="yaml">---&#13;hdmicec:&#13;  command: DeckStatus&#13;  initiator: PlaybackDevice1&#13;  destination: TV&#13;  parameters:&#13;    deck_info: Play</pre> | Reports the current status of the playback device                                            | Automatically triggered by the playback device to inform its status       |

#### Parameters for `DeckControl`

| Parameter        | Description                               | Values                                                |
|------------------|-------------------------------------------|-------------------------------------------------------|
| `deck_info`      | Indicates the deck control mode           | `Play`, `Pause`, `Stop`, `Rewind`, `FastForward`, `Eject`, `Seek` |
| `seek_time`      | Time to seek to in milliseconds           | `0` to `n` (where `n` is the duration of the content in ms) |

#### Parameters for `GiveDeckStatus`

| Parameter         | Description                               | Values                            |
|-------------------|-------------------------------------------|-----------------------------------|
| `status_request`  | Request type for deck status              | `0`, `1` (e.g., `1` for status)   |

#### Parameters for `Play`

| Parameter        | Description                               | Values                                 |
|------------------|-------------------------------------------|----------------------------------------|
| `play_mode`      | Indicates the play mode                   | `PlayForward`, `PlayReverse`, `Still`  |

#### Parameters for `DeckStatus`

| Parameter        | Description                               | Values                                                |
|------------------|-------------------------------------------|-------------------------------------------------------|
| `deck_info`      | Indicates the current status of the deck  | `Play`, `Pause`, `Stop`, `Rewind`, `FastForward`      |

### Example YAML Payloads

**Deck Control - Play Command:**
<pre lang="yaml">
---
hdmicec:
  command: DeckControl
  initiator: TV
  destination: PlaybackDevice1
  parameters:
    deck_info: Play
</pre>

**Give Deck Status Command:**
<pre lang="yaml">
---
hdmicec:
  command: GiveDeckStatus
  initiator: TV
  destination: PlaybackDevice1
  parameters:
    status_request: 1
</pre>

**Play Command:**
<pre lang="yaml">
---
hdmicec:
  command: Play
  initiator: TV
  destination: PlaybackDevice1
  parameters:
    play_mode: PlayForward
</pre>

**Deck Status - Playing:**
<pre lang="yaml">
---
hdmicec:
  command: DeckStatus
  initiator: PlaybackDevice1
  destination: TV
  parameters:
    deck_info: Play
</pre>


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
