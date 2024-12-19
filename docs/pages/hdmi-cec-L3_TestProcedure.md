# dsAudio HAL L3 Python Test Procedure

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Setting Up Test Environment](#setting-up-test-environment)
- [Streams Required](#streams-required)
- [Test Cases](#test-cases)
  - [dsAudio_L3_Runall_Sink.py](#dsaudio_l3_runall_sinkpy)
  - [dsAudio_L3_Runall_Source.py](#dsaudio_l3_runall_sourcepy)
  - [dsaudio_test01_enabledisableandverifyaudioportstatus.py](#dsaudio_test01_enabledisableandverifyaudioportstatuspy)

## Acronyms, Terms and Abbreviations

- `HAL`    - Hardware Abstraction Layer
- `L3`     - Level 3 Testing
- `DUT`    - Device Under Test
- `RAFT`   - Rapid Automation Framework for Testing
- `YAML`   - YAML Ain't Markup Language
- `HDMI`   -  High Definition Multimedia Interface
- `API`    -  Application Program Interface
- `SoC`    -  System on Chip
- `DUT`    -  Device Under Test
- `LA`     -  Logical Address
- `PA`     -  Physical Address

## Setting Up Test Environment

To execute `HAL` `L3` Python test cases, need a Python environment. Follow these steps mentioned in [HPK Public Documentation](https://github.com/rdkcentral/rdk-hpk-documentation/blob/main/README.md)

### Update Configuration Files

#### Rack Configuration File

Example Rack configuration File: [example_rack_config.yml](../../../ut/host/tests/configs/example_rack_config.yml)

For more details refer [RAFT](https://github.com/rdkcentral/python_raft/blob/1.0.0/README.md) and [example_rack_config.yml](https://github.com/rdkcentral/python_raft/blob/1.0.0/examples/configs/example_rack_config.yml)

In this file, update the configuration to define the console sessions for the `DUT` and the outbound settings:

|Console Session|Description|
|---------------|-----------|
|default|Downloads the streams required for test cases|
|ssh_hal_test|Executes the `HAL` binary for the test case|

```yaml
rackConfig:
  - dut:
      ip: "XXX.XXX.XXX.XXX"  # IP Address of the device
      description: "stb device under test"
      platform: "stb"
      consoles:
        - default:
            type: "ssh"
            port: 10022
            username: "root"
            ip: "XXX.XXX.XXX" # IP address of the device
            password: ' '
        - ssh_hal_test:
            type: "ssh"
            port: 10022
            username: "root"
            ip: "XXX.XXX.XXX" # IP address of the device
            password: ' '
      outbound:
        download_url: "http://localhost:8000/"    # download location for the CPE device
        httpProxy:   # Local Proxy if required
        workspaceDirectory: './logs/workspace'   # Local working directory
```

#### Device Configuration File

Example Device configuration File: [deviceConfig.yml](../../../ut/host/tests/configs/deviceConfig.yml)

For more details refer [RAFT](https://github.com/rdkcentral/python_raft/blob/1.0.0/README.md) and [example_device_config.yml](https://github.com/rdkcentral/python_raft/blob/1.0.0/examples/configs/example_device_config.yml)

Update below fileds in the device configuration file:

- Set the path for `target_directory` where `HAL` binaries will be copied onto the device.
- Specify the device profile path in `test/profile`
- Ensure the `platform` should match with the `DUT` `platform` in [Rack Configuration](#rack-configuration-file)

```yaml
deviceConfig:
    cpe1:
        platform: "tv"
        model: "uk"
        soc_vendor: "soc"
        target_directory: "/tmp/"  # Target Directory on device
        prompt: ""
        test:
            profile: "../../../profiles/sink/sink_hdmiCEC.yml"
```

#### Test Configuration

Example Test Setup configuration File: [hdmiCEC_testConfig.yml](../../../ut/host/tests/classes/hdmiCEC_testConfig.yml)

Execute command to run te HAL binary was provided in this file.

```yaml
hdmicec:
    description: "hdmi CEC testing profile / menu system for UT"
    test:
        artifacts:
        #List of artifacts folders, test class copies the content of folder to the target device workspace
          - "../../../bin/"
        # exectute command, this will appended with the target device workspace path
        execute: "run.sh"
        type: UT-C # C (UT-C Cunit) / C++ (UT-G (g++ ut-core gtest backend))
```

## Run Test Cases

Once the environment is set up, you can execute the test cases with the following command

```bash
python <TestCaseName.py> --config </PATH>/ut/host/tests/configs/example_rack_config.yml --deviceConfig </PATH>/ut/host/tests/configs/deviceConfig.yml
```

## Test Cases

### dsAudio_L3_Runall_Sink.py

This python file runs all the tests supported by `sink` devices

```bash
python dsAudio_L3_Runall_Sink.py --config </PATH>/ut/host/tests/configs/example_rack_config.yml --deviceConfig </PATH>/ut/host/tests/configs/deviceConfig.yml
```

### dsAudio_L3_Runall_Source.py

This python file runs all the tests supported by `source` devices

```bash
python dsAudio_L3_Runall_Source.py --config </PATH>/ut/host/tests/configs/example_rack_config.yml --deviceConfig </PATH>/ut/host/tests/configs/deviceConfig.yml
```

### dsAudio_test01_EnableDisableAndVerifyAudioPortStatus.py

#### Platform Support - test01

- Source
- Sink

#### User Input Required - test01

**Yes**: User interaction is necessary to confirm audio playback status (This will be automated later).

#### Acceptance Criteria - test01

Play **Stream #1** and confirm that audio is heard through the supported ports.

#### Expected Results - test01

The test enables the specified audio ports, plays the audio stream, and subsequently disables the ports

Success Criteria

- User should hear audio through the enabled port during playback
- User should not hear any audio when the port is disabled.

#### Test Steps - test01

- Initiate the Test:

  - Select and execute the Python file: **`dsAudio_test01_EnableDisableAndVerifyAudioPortStatus.py`**
  - The test will automatically download all required artifacts and streams, copying them to the designated target directory before commencing execution.

- Audio Playback Verification:

    The test will play the designated audio stream and prompt the user with the following:

  - Question: "Is audio playing on the enabled audio port? (Y/N)"
  - Press **Y** if audio is heard (this will mark the step as PASS).
  - Press **N** if no audio is heard (this will mark the step as FAIL).

- Audio Status Confirmation (Port Disabled):

  After confirming audio playback, the test will disable the audio port and prompt the user again:

  - Question: "Is audio playing when the port is disabled? (Y/N)"
  - Press **N** if no audio is heard (this will mark the step as PASS).
  - Press **Y** if audio is heard (this will mark the step as FAIL).

- Repeat for All Ports:

  The test will iterate through all available audio ports, enabling/disabling each one and collecting user feedback accordingly.

- Test Conclusion:

  Upon receiving user responses for all ports, the test will conclude and present a final result: PASS or FAIL based on the user inputs throughout the test execution.

