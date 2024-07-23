## Overview of HDMI-CEC Virtual Component Control Plane Commands

The HDMI-CEC virtual component enables comprehensive testing of HDMI-CEC functionalities through the control plane. Testers can interact with the virtual component using four primary types of commands, each serving a distinct purpose:

**Command**: Directly trigger specific HDMI-CEC commands between devices, simulating real-world user interactions.

**Event**: Simulate HDMI-CEC events like hotplug (device connection/disconnection) to test how devices react.

**Config**: Modify the virtual CEC network configuration, changing device types, connections, and capabilities.

**State**: Dynamically add or remove devices during testing and print the current status for debugging.

Each command type is structured as a YAML payload, which is outlined in detail in the following sections. These payloads allow testers to precisely control the virtual HDMI-CEC environment, replicating various scenarios and edge cases.

### 1. **Command**
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

### 2. **Event**
   This category of messages allow the test user to trigger changes in the virtual component state machine.
   
#### Example Event
Command to trigger a hotplug event from device connected in Port 2:
```yaml
hdmicec:
    command: hotplug
    port: 2
    connected: false
```

### 3. **Config**
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

### 4. **State**
Test user can trigger state changes by dynamically adding or removing device(s). Debug printing of the current status of virtual component in logs can also be triggered.

| Status       | YAML Payload                                                                                                                                            | Action                                                   |
|---------------|---------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------|
| `AddDevice`    | <pre lang="yaml">---&#13;hdmicec:&#13;  state: AddDevice&#13;  parameters:&#13;    parent: TV&#13;   name: TestDevice&#13;    type: PlaybackDevice&#13;    version: 4&#13;    active_source: false&#13;    vendor: TEST_VENDOR&#13;    pwr_status: on&#13;    port_id: 2&#13;    number_children: 0</pre> | Adds the new device as a child to given parent       |
| `RemoveDevice`   | <pre lang="yaml">---&#13;hdmicec:&#13;  state: RemoveDevice&#13;  parameters:&#13;    name: TestDevice&#13;</pre>                                   | Removes a device and its children from the Virtual Component state.        |
| `PrintStatus`| <pre lang="yaml">---&#13;hdmicec:&#13;  state: PrintStatus&#13;  parameters:&#13;    status: Devices</pre> | Prints the current network of devices       |

#### Parameters for `PrintStatus`

| Parameter     | Description                               | Values                            |
|---------------|-------------------------------------------|-----------------------------------|
| `status`      | Specific status                  | `Devices`, `Ports`, `General`  |

#### Example state trigger to add a new device to a parent port. 

```yaml
hdmicec:
    state: AddDevice
    prameters:
       parent: TV
       name: TestDevice
       type: PlaybackDevice
       version: 4
       active_source: false
       vendor: TEST_VENDOR
       pwr_status: on
       port_id: 2
       number_children: 0
```

#### Example state trigger to print status of devices connected
```yaml
hdmicec:
    state: PrintStatus
    parameters:
       status: Devices
```


## Anatomy of the YAML CEC Command

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

#### Root Element (hdmicec):

The root element of the YAML file, indicating that this is a CEC command.

#### Command:

The command key specifies the CEC command to be executed.
Example: SetOSDName is the command to set the On-Screen Display name of a device.

#### Initiator:

The initiator key indicates the device that is sending the command. All devices are identified by its unique name that is configured in the profile YAML. The virtual component
takes care of converting the device names into physical and logical addresses internaly.

Example: Sony HomeTheatre is the device initiating the command.

#### Destination:

The destination key specifies the target device that will receive the command.
Example: IPSTB is the device receiving the command.

#### Parameters:

The parameters key holds a dictionary of additional data required for the command.
Example: For the SetOSDName command, the parameters section includes an osd_name key with the value IPSTB.

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

HDMI CEC commands are categorized based on their functionalities and the end-user features they support.

Below are some of the commonly used categories of CEC commands:

### 1. One Touch Play
Commands in this category are used to turn on devices and switch them to the correct input automatically.

| Command        | YAML Payload                                                                                                                         | Action                                                     | How to Trigger in Real Setup                          |
|----------------|--------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------|-------------------------------------------------------|
| `ActiveSource` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "ActiveSource"&#13;  initiator: "IPSTB"&#13;  destination: "Broadcast"&#13;  parameters:&#13;    device_name: "TV"</pre> | Announces that the initiator is the active source          | Automatically triggered when a device becomes active  |
| `ImageViewOn`  | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "ImageViewOn"&#13;  initiator: "DVD Player"&#13;  destination: "TV"</pre>             | Switches the TV to the initiator's input                   | Using a DVD player remote control to turn on the TV   |
| `TextViewOn`   | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "TextViewOn"&#13;  initiator: "DVD Player"&#13;  destination: "TV"</pre>              | Switches the TV to the initiator's input with text display | Using a DVD player remote control to turn on the TV   |

#### Example YAML Payloads

**Active Source:**
<pre lang="yaml">
---
hdmicec:
  command: "ActiveSource"
  initiator: "IPSTB"
  destination: "Broadcast"
  parameters:
    device_name: "TV"
</pre>

**Image View On:**
<pre lang="yaml">
---
hdmicec:
  command: "ImageViewOn"
  initiator: "DVD Player"
  destination: "TV"
</pre>

**Text View On:**
<pre lang="yaml">
---
hdmicec:
  command: "TextViewOn"
  initiator: "DVD Player"
  destination: "TV"
</pre>

### 2. One Touch Record

Allows whatever is shown on the TV screen to be recorded on a selected Recording Device.

| Command       | YAML Payload                                                                                                                                            | Action                                                   | How to Trigger in Real Setup                                   |
|---------------|---------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------|----------------------------------------------------------------|
| `RecordOn`    | <pre lang="yaml">---&#13;hdmicec:&#13;  command: RecordOn&#13;  initiator: TV&#13;  destination: RecordingDevice&#13;  parameters:&#13;    source: Tuner</pre> | Starts recording on the specified recording device       | Using a TV remote control to start recording on a DVR          |
| `RecordOff`   | <pre lang="yaml">---&#13;hdmicec:&#13;  command: RecordOff&#13;  initiator: TV&#13;  destination: RecordingDevice</pre>                                   | Stops recording on the specified recording device        | Using a TV remote control to stop recording on a DVR           |
| `RecordStatus`| <pre lang="yaml">---&#13;hdmicec:&#13;  command: RecordStatus&#13;  initiator: RecordingDevice&#13;  destination: TV&#13;  parameters:&#13;    status: Recording</pre> | Reports the current recording status of the device       | Automatically triggered by the recording device to inform status|

#### Parameters for `RecordOn`

| Parameter     | Description                               | Values                            |
|---------------|-------------------------------------------|-----------------------------------|
| `source`      | Source of the recording                   | `Tuner`, `ExternalInput`, `Auxiliary`  |

#### Parameters for `RecordStatus`

| Parameter     | Description                               | Values                            |
|---------------|-------------------------------------------|-----------------------------------|
| `status`      | Indicates the current recording status    | `Recording`, `NoRecording`, `InsufficientSpace`, `AlreadyRecording` |

#### Example YAML Payloads

**Record On - Tuner Source:**
<pre lang="yaml">
---
hdmicec:
  command: RecordOn
  initiator: TV
  destination: RecordingDevice
  parameters:
    source: Tuner
</pre>

**Record Off:**
<pre lang="yaml">
---
hdmicec:
  command: RecordOff
  initiator: TV
  destination: RecordingDevice
</pre>

**Record Status - Recording:**
<pre lang="yaml">
---
hdmicec:
  command: RecordStatus
  initiator: RecordingDevice
  destination: TV
  parameters:
    status: Recording
</pre>

### 3. Routing Control
These commands manage the routing of signals between devices, ensuring the correct source is displayed on the TV.

| Command              | YAML Payload                                                                                                                                              | Action                                                                                         | How to Trigger in Real Setup                          |
|----------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------|-------------------------------------------------------|
| `RoutingChange`      | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "RoutingChange"&#13;  initiator: "AV Receiver"&#13;  destination: "Broadcast"&#13;  parameters:&#13;    from_device: "DVD Player"&#13;    to_device: "TV"</pre> | Notifies that the source device has changed from one to another                                | Automatically triggered when the AV Receiver changes the input source |
| `RoutingInformation` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "RoutingInformation"&#13;  initiator: "AV Receiver"&#13;  destination: "Broadcast"&#13;  parameters:&#13;    device_name: "TV"</pre> | Provides information about the current routing path                                           | Automatically triggered to inform the routing path change |
| `RequestActiveSource`| <pre lang="yaml">---&#13;hdmicec:&#13;  command: "RequestActiveSource"&#13;  initiator: "TV"&#13;  destination: "Broadcast"</pre>                          | Requests the active source device to identify itself                                           | Using a TV remote control to switch inputs            |
| `SetStreamPath`      | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "SetStreamPath"&#13;  initiator: "TV"&#13;  destination: "Broadcast"&#13;  parameters:&#13;    device_name: "AV Receiver"</pre> | Sets the stream path to the specified device                                                  | Using a TV remote control to select the AV Receiver   |
| `InactiveSource`     | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "InactiveSource"&#13;  initiator: "DVD Player"&#13;  destination: "TV"</pre>                              | Notifies that the initiator device is no longer the active source                              | Automatically triggered when the DVD Player is turned off |
| `ReportPhysicalAddress` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "ReportPhysicalAddress"&#13;  initiator: "AV Receiver"&#13;  destination: "Broadcast"&#13;  parameters:&#13;    device_name: "TV"</pre> | Provides the physical address of the device                                                   | Automatically triggered when a device is connected    |
| `GivePhysicalAddress` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "GivePhysicalAddress"&#13;  initiator: "TV"&#13;  destination: "AV Receiver"</pre>                       | Requests the physical address of the destination device                                        | Using a TV remote control to identify the AV Receiver |

#### Example YAML Payloads

**Routing Change:**
<pre lang="yaml">
---
hdmicec:
  command: "RoutingChange"
  initiator: "AV Receiver"
  destination: "Broadcast"
  parameters:
    from_device: "DVD Player"
    to_device: "TV"
</pre>

**Routing Information:**
<pre lang="yaml">
---
hdmicec:
  command: "RoutingInformation"
  initiator: "AV Receiver"
  destination: "Broadcast"
  parameters:
    device_name: "TV"
</pre>

**Request Active Source:**
<pre lang="yaml">
---
hdmicec:
  command: "RequestActiveSource"
  initiator: "TV"
  destination: "Broadcast"
</pre>

**Set Stream Path:**
<pre lang="yaml">
---
hdmicec:
  command: "SetStreamPath"
  initiator: "TV"
  destination: "Broadcast"
  parameters:
    device_name: "AV Receiver"
</pre>

**Inactive Source:**
<pre lang="yaml">
---
hdmicec:
  command: "InactiveSource"
  initiator: "DVD Player"
  destination: "TV"
</pre>


### 4. Deck Control
These commands control the playback devices, such as recorders and players.

| Command          | YAML Payload                                                                                                                                         | Action                                                                                       | How to Trigger in Real Setup                                               |
|------------------|------------------------------------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------|----------------------------------------------------------------------------|
| `DeckControl`    | <pre lang="yaml">---&#13;hdmicec:&#13;  command: DeckControl&#13;  initiator: TV&#13;  destination: PlaybackDevice1&#13;  parameters:&#13;    deck_info: Play</pre> | Controls deck functions such as play, pause, etc.                                            | Using a TV remote control to control playback functions                    |
| `GiveDeckStatus` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: GiveDeckStatus&#13;  initiator: TV&#13;  destination: PlaybackDevice1&#13;  parameters:&#13;    status_request: On</pre> | Requests the current status of the playback device                                           | Automatically triggered by TV to get the status of the playback device     |
| `Play`           | <pre lang="yaml">---&#13;hdmicec:&#13;  command: Play&#13;  initiator: TV&#13;  destination: PlaybackDevice1&#13;  parameters:&#13;    play_mode: PlayForward</pre> | Starts playback on the specified playback device                                             | Using a TV remote control to play content                                  |
| `DeckStatus`     | <pre lang="yaml">---&#13;hdmicec:&#13;  command: DeckStatus&#13;  initiator: PlaybackDevice1&#13;  destination: TV&#13;  parameters:&#13;    deck_info: Play</pre> | Reports the current status of the playback device                                            | Automatically triggered by the playback device to inform its status       |

#### Parameters for `DeckControl`

| Parameter        | Description                               | Values                                                |
|------------------|-------------------------------------------|-------------------------------------------------------|
| `deck_info`      | Indicates the deck control mode           | `Play`, `Pause`, `Stop`, `Rewind`, `FastForward`, `Eject`, `Seek` |
| `seek_time`      | Time to seek to in milliseconds           | `0` to `n` (where `n` is the duration of the content in ms) |


#### Possible Values for `status_request`

| Prameter                  | Possible Values                | Description                                                                                           |
|--------------------------|--------------------------------|-------------------------------------------------------------------------------------------------------|
| `status_request`      | `"On"`, `"Off"`, `"Once"`     | Requests status information from the device. "On" requests continuous updates, "Off" stops updates, "Once" requests a single update. |

#### Parameters for `Play`

| Parameter        | Description                               | Values                                 |
|------------------|-------------------------------------------|----------------------------------------|
| `play_mode`      | Indicates the play mode                   | `PlayForward`, `PlayReverse`, `Still`  |

#### Parameters for `DeckStatus`

| Parameter        | Description                               | Values                                                |
|------------------|-------------------------------------------|-------------------------------------------------------|
| `deck_info`      | Indicates the current status of the deck  | `Play`, `Pause`, `Stop`, `Rewind`, `FastForward`      |

#### Example YAML Payloads

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

### 5. OSD Display
These commands manage the On-Screen Display settings and information.

| Command       | YAML Payload                                                                                                            | Action                                                      | How to Trigger in Real Setup                                      |
|---------------|-------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|-------------------------------------------------------------------|
| `SetOSDString` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "SetOSDString"&#13;  initiator: "PlaybackDevice1"&#13;  destination: "TV"&#13;  parameters:&#13;    osd_string: "IPSTB"</pre> | Sets the OSD (On-Screen Display) string on the TV           | Automatically triggered when the playback device sets the OSD string |
| `SetOSDName`   | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "SetOSDName"&#13;  initiator: "TV"&#13;  destination: "Broadcast"&#13;  parameters:&#13;    osd_name: "IPSTB"</pre>     | Sets the OSD (On-Screen Display) name on all devices       | Automatically triggered when the TV sets the OSD name               |
| `GiveOSDName`  | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "GiveOSDName"&#13;  initiator: "TV"&#13;  destination: "Broadcast"</pre> | Requests the OSD (On-Screen Display) name from all devices | Using a TV remote control to request OSD name from all devices      |

#### Example YAML Payloads

**Set OSD String ("IPSTB") on TV:**
<pre lang="yaml">
---
hdmicec:
  command: "SetOSDString"
  initiator: "PlaybackDevice1"
  destination: "TV"
  parameters:
    osd_string: "IPSTB"
</pre>

**Set OSD Name ("IPSTB") on all devices:**
<pre lang="yaml">
---
hdmicec:
  command: "SetOSDName"
  initiator: "TV"
  destination: "Broadcast"
  parameters:
    osd_name: "IPSTB"
</pre>

**Give OSD Name (Request from TV):**
<pre lang="yaml">
---
hdmicec:
  command: "GiveOSDName"
  initiator: "TV"
  destination: "Broadcast"
</pre>


### 6. Device Menu Control
Commands to control the device's menu interface, enabling navigation and selection of menu items.

| Command              | YAML Payload                                                                                                                       | Action                                                      | How to Trigger in Real Setup                                      |
|----------------------|------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|-------------------------------------------------------------------|
| `MenuRequest`        | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "MenuRequest"&#13;  initiator: "TV"&#13;  destination: "PlaybackDevice1"</pre>    | Requests the menu from the playback device                  | Using a TV remote control to request the menu from the device       |
| `MenuStatus`         | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "MenuStatus"&#13;  initiator: "PlaybackDevice1"&#13;  destination: "TV"&#13;  parameters:&#13;    status: "activated"</pre> | Provides the status of the menu                             | Automatically triggered to inform the TV of the menu status        |
| `UserControlPressed` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "UserControlPressed"&#13;  initiator: "TV"&#13;  destination: "PlaybackDevice1"&#13;  parameters:&#13;    ui_command: "Select"</pre> | Sends a user control command (Select) to the playback device | Using a TV remote control to send a select command to the device   |

#### Example YAML Payloads

**Menu Request:**
<pre lang="yaml">
---
hdmicec:
  command: "MenuRequest"
  initiator: "TV"
  destination: "PlaybackDevice1"
</pre>

**Menu Status:**
<pre lang="yaml">
---
hdmicec:
  command: "MenuStatus"
  initiator: "PlaybackDevice1"
  destination: "TV"
  parameters:
    status: "activated"
</pre>

**User Control Pressed (Select):**
<pre lang="yaml">
---
hdmicec:
  command: "UserControlPressed"
  initiator: "TV"
  destination: "PlaybackDevice1"
  parameters:
    ui_command: "Select"
</pre>

### 7. Remote Control Passthrough
Allows remote control commands to be sent directly to devices.

| Command                    | YAML Payload                                                                                                                      | Action                                                      | How to Trigger in Real Setup                                      |
|----------------------------|-----------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|-------------------------------------------------------------------|
| `UserControlPressed`       | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "UserControlPressed"&#13;  initiator: "TV"&#13;  destination: "PlaybackDevice1"&#13;  parameters:&#13;    ui_command: "Play"</pre> | Sends a user control command (Play) to the playback device | Using a TV remote control to send a play command to the device     |
| `UserControlReleased`      | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "UserControlReleased"&#13;  initiator: "TV"&#13;  destination: "PlaybackDevice1"&#13;  parameters:&#13;    ui_command: "Play"</pre> | Sends a user control command (Play release) to the playback device | Using a TV remote control to release the play command on the device |

#### Example YAML Payloads

**User Control Pressed (Play):**
<pre lang="yaml">
---
hdmicec:
  command: "UserControlPressed"
  initiator: "TV"
  destination: "PlaybackDevice1"
  parameters:
    ui_command: "Play"
</pre>

**User Control Released (Play):**
<pre lang="yaml">
---
hdmicec:
  command: "UserControlReleased"
  initiator: "TV"
  destination: "PlaybackDevice1"
  parameters:
    ui_command: "Play"
</pre>


### 8. Power Status
These commands manage the power state of devices, such as putting them into standby mode.

| Command           | YAML Payload                                                                                                                | Action                                                      | How to Trigger in Real Setup                                      |
|-------------------|-----------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|-------------------------------------------------------------------|
| `GiveDevicePowerStatus` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "GiveDevicePowerStatus"&#13;  initiator: "TV"&#13;  destination: "AudioSystem"</pre> | Requests the power status of the audio system               | Using a TV remote control to request power status from the audio system |
| `ReportPowerStatus`     | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "ReportPowerStatus"&#13;  initiator: "AudioSystem"&#13;  destination: "TV"&#13;  parameters:&#13;    power_status: "on"</pre> | Provides the power status of the audio system               | Automatically triggered to inform the TV of the audio system power status |
| `Standby`               | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "Standby"&#13;  initiator: "TV"&#13;  destination: "AudioSystem"</pre>             | Puts the audio system into standby mode                    | Using a TV remote control to send a standby command to the audio system |

#### Example YAML Payloads

**Give Device Power Status:**
<pre lang="yaml">
---
hdmicec:
  command: "GiveDevicePowerStatus"
  initiator: "TV"
  destination: "AudioSystem"
</pre>

**Report Power Status (On):**
<pre lang="yaml">
---
hdmicec:
  command: "ReportPowerStatus"
  initiator: "AudioSystem"
  destination: "TV"
  parameters:
    power_status: "on"
</pre>

**Standby:**
<pre lang="yaml">
---
hdmicec:
  command: "Standby"
  initiator: "TV"
  destination: "AudioSystem"
</pre>


### 9. System Audio Control
When an Audio Amplifier / Receiver is connected with the TV, functionality like volume can be controlled using any of the remote controls of any cec enabled devices in the system.

| Command                    | YAML Payload                                                                                                                         | Action                                                      | How to Trigger in Real Setup                                      |
|----------------------------|--------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|-------------------------------------------------------------------|
| `GiveAudioStatus`          | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "GiveAudioStatus"&#13;  initiator: "TV"&#13;  destination: "AudioSystem"</pre>   | Requests the audio system status                            | Using a TV remote control to get the status of the audio system   |
| `GiveSystemAudioModeStatus`| <pre lang="yaml">---&#13;hdmicec:&#13;  command: "GiveSystemAudioModeStatus"&#13;  initiator: "TV"&#13;  destination: "AudioSystem"</pre> | Requests the audio system mode status                      | Using a TV remote control to get the current audio system mode   |
| `ReportAudioStatus`        | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "ReportAudioStatus"&#13;  initiator: "AudioSystem"&#13;  destination: "TV"</pre>   | Provides the audio system status                            | Automatically triggered to inform the TV of the audio system status |
| `ReportShortAudioDescriptor` | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "ReportShortAudioDescriptor"&#13;  initiator: "AudioSystem"&#13;  destination: "TV"</pre> | Provides the audio capabilities of the audio system        | Automatically triggered to inform the TV of the audio capabilities |
| `RequestAudioDescriptor`   | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "RequestAudioDescriptor"&#13;  initiator: "TV"&#13;  destination: "AudioSystem"</pre> | Requests the audio capabilities from the audio system      | Using a TV remote control to request audio capabilities from the audio system |

#### Example YAML Payloads

**Give Audio Status:**
<pre lang="yaml">
---
hdmicec:
  command: "GiveAudioStatus"
  initiator: "TV"
  destination: "AudioSystem"
</pre>

**Give System Audio Mode Status:**
<pre lang="yaml">
---
hdmicec:
  command: "GiveSystemAudioModeStatus"
  initiator: "TV"
  destination: "AudioSystem"
</pre>

**Report Audio Status:**
<pre lang="yaml">
---
hdmicec:
  command: "ReportAudioStatus"
  initiator: "AudioSystem"
  destination: "TV"
</pre>

**Report Short Audio Descriptor:**
<pre lang="yaml">
---
hdmicec:
  command: "ReportShortAudioDescriptor"
  initiator: "AudioSystem"
  destination: "TV"
</pre>

**Request Audio Descriptor:**
<pre lang="yaml">
---
hdmicec:
  command: "RequestAudioDescriptor"
  initiator: "TV"
  destination: "AudioSystem"
</pre>

### 10. System Information

| Command                  | YAML Payload                                                                                                                  | Action                                                      | How to Trigger in Real Setup                                      |
|--------------------------|-------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------|-------------------------------------------------------------------|
| `GetCECVersion`           | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "GetCECVersion"&#13;  initiator: "TV"&#13;  destination: "Broadcast"</pre>   | Requests the CEC version from all devices                   | Automatically triggered to get the CEC version                   |
| `GetMenuLanguage`         | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "GetMenuLanguage"&#13;  initiator: "TV"&#13;  destination: "AudioSystem"</pre> | Requests the menu language from the audio system             | Using TV remote control to request menu language from audio system |
| `SetMenuLanguage`         | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "SetMenuLanguage"&#13;  initiator: "TV"&#13;  destination: "AudioSystem"&#13;  parameters:&#13;    menu_language: "ENG"</pre> | Sets the menu language on the audio system                   | Automatically triggered to set the menu language on the audio system |
| `ReportPhysicalAddress`   | <pre lang="yaml">---&#13;hdmicec:&#13;  command: "ReportPhysicalAddress"&#13;  initiator: "AudioSystem"&#13;  destination: "TV"&#13;  parameters:&#13;    physical_address: [16, 0]</pre> | Reports the physical address of the audio system to the TV  | Automatically triggered to report physical address to the TV      |

#### Example YAML Payloads

**Get CEC Version (Request from TV):**
<pre lang="yaml">
---
hdmicec:
  command: "GetCECVersion"
  initiator: "TV"
  destination: "Broadcast"
</pre>

**Get Menu Language (Request from TV):**
<pre lang="yaml">
---
hdmicec:
  command: "GetMenuLanguage"
  initiator: "TV"
  destination: "AudioSystem"
</pre>

**Set Menu Language ("ENG") on AudioSystem:**
<pre lang="yaml">
---
hdmicec:
  command: "SetMenuLanguage"
  initiator: "TV"
  destination: "AudioSystem"
  parameters:
    menu_language: "ENG"
</pre>

**Report Physical Address (AudioSystem to TV):**
<pre lang="yaml">
---
hdmicec:
  command: "ReportPhysicalAddress"
  initiator: "AudioSystem"
  destination: "TV"
  parameters:
    physical_address: [16, 0]
</pre>

