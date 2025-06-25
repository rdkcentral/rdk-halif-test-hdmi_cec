# Unit Testing Suite For HDMI CEC HAL

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Description](#description)
- [Reference Documents](#reference-documents)
- [How to build the test suite](#how-to-build-the-test-suite)
- [Notes](#notes)
- [Manual way of running the L1 and L2 test cases](#manual-way-of-running-the-l1-and-l2-test-cases)
- [Setting Python environment for running the L1 L2 and L3 automation test cases](#setting-python-environment-for-running-the-l1-l2-and-l3-automation-test-cases)

## Acronyms, Terms and Abbreviations

- `HAL`- Hardware Abstraction Layer
- `HDMI` - High Definition Multimedia Interface
- `CEC` - Consumer Electronics Control
- `L1` - Functional Tests
- `L2` - Module functional Testing
- `L3` - Module testing with External Stimulus is required to validate and control device
- `API` - Application Programming Interface
- `High-Level Test Specification` : These specification will provide a broad overview of the system's functionality from the callers' perspective. It focuses on major use cases, system behavior, and overall caller experience.
- `Low-Level Test Specification` : These specification will deeper into the technical details. They will define specific test cases with inputs, expected outputs, and pass/fail criteria for individual functionalities, modules, or APIs.

## Description

This repository contains the Unit Test Suites(`L1` , `L2` and `L3`) for HDMI CEC `HAL`.

## Reference Documents

|SNo|Document Name|Document Description|Document Link|
|---|-------------|--------------------|-------------|
|1|`HAL` Specification Document|This document provides specific information on the APIs for which tests are written in this module|[hdmi-cec_halSpec.md](https://github.com/rdkcentral/rdk-halif-hdmi_cec/blob/1.3.9/docs/pages/hdmi-cec_halSpec.md)|
|2|High Level Test Spec for sink device|High Level Test Specification Documentation for the `HDMI` `CEC` Sink device|[hdmi-cec-sink_High-Level_TestSpec.md](docs/pages/hdmi-cec-sink_High-Level_TestSpec.md )|
|3|`L2` Low Level Test Spec for sink device|`L2` Low Level Test Specification and Procedure Documentation for the `HDMI` `CEC` Sink device|[hdmi-cec-sink_L2_Low-Level_TestSpec.md](docs/pages/hdmi-cec-sink_L2_Low-Level_TestSpec.md)|
|4|High Level Test Spec for Source device|High Level Test Specification Documentation for the `HDMI` `CEC` Source device|[hdmi-cec-source_High-Level_TestSpec.md](docs/pages/hdmi-cec-source_High-Level_TestSpec.md)|
|5|`L2` Low Level Test Spec for Source device|`L2` Low Level Test Specification and Procedure Documentation for the `HDMI` `CEC` Source device|[hdmi-cec-source_L2_Low-Level_TestSpec.md](docs/pages/hdmi-cec-source_L2_Low-Level_TestSpec.md)|
|6|`L1` and `L2` Test procedure document|`L1` and `L2` Test procedure document for the `HDMI` `CEC`|[hdmi-cec-L1_L2_TestProcedure.md](docs/pages/hdmi-cec-L1_L2_TestProcedure.md)|
|7|`L3` Low Level Test Specification|`L3` Low Level Test Specification for the `HDMI` `CEC`|[hdmi-cec-L3-Low-Level_TestSpec.md](docs/pages/hdmi-cec-L3-Low-Level_TestSpec.md)|
|8|`L3` Test procedure document|`L3` Test procedure document for the `HDMI` `CEC`|[hdmi-cec-L3_TestProcedure.md](docs/pages/hdmi-cec-L3_TestProcedure.md)|

## Notes

- All APIs need to be implemented in this current version. If any API is not supported, please add stub implementation with return type HDMI_CEC_IO_OPERATION_NOT_SUPPORTED for the same.
- Building against the actual library may introduce SOC dependencies. Hence, a template SKELETON library is created without SOC dependencies. On the real platform (target), it can be mounted, copied and bound with the actual library.
- When running the binary, remember to include a profile file as an argument for designated test cases.

## How to build the test suite

In order to build the test suits, there is a requirement to supply the toolchain, either as a vendor independent toolchain, an SDK, or an RDK Docker Toolchain .

For more information about RDK Docker Toolchain, please check [RDK Docker Toolchain](https://github.com/rdkcentral/ut-core/wiki/FAQ:-RDK-Docker-Toolchain)

- Export the Toolchain Path:

```bash
export TOOLCHAIN_PATH=/opt/toolchains/rdk-glibc-x86_64/sysroots/armv7at2hf-neon-rdk-linux-gnueabi
export CC="arm-rdk-linux-gnueabi-gcc  -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard --sysroot=$TOOLCHAIN_PATH"
```

- Clone the `HAL` Repository:

```bash
git clone git@github.com:rdkcentral/rdk-halif-hdmi_cec.git
```

- Set Project Version:

If you want to build the test repository with a specific tag or branch, set the UT_PROJECT_VERSION variable accordingly. If not set, the default main branch will be used.

```bash
export UT_PROJECT_VERSION=1.0.1  # Build using a specific tag
```

or

```bash
export UT_PROJECT_VERSION=develop  # Build using a specific branch
```

- Build Targets:

 There are two targets for the platform

  1. linux - (default)
  2. arm - TARGET=arm

```bash
build_ut.sh TARGET=arm
```

- `build_ut.sh` script will clone the hal-test repo, ut-core and generate a `hal_test_RCECHal` binary in `ut/bin/` upon successful completion.

### Manual way of running the `L1` and `L2` test cases

```bash
 ./hal_test_RCECHal -p sink_hdmiCEC.yml
 ```

Alternatively, use the run.sh script with the profile file:
```bash
./run.sh -p /absolute/path/to/profile/file
 ```

- Profile files define the configuration for the platform available here 
  [sink_profile_yaml_file](https://github.com/rdkcentral/rdk-halif-test-hdmi_cec/blob/main/profiles/sink/sink_hdmiCEC.yml),[source_profile_yaml_file](https://github.com/rdkcentral/rdk-halif-test-hdmi_cec/blob/main/profiles/source/source_hdmiCEC.yml)

### Setting Python environment for running the `L1` `L2` and `L3` automation test cases

- For running the `L1` `L2` and `L3` test suite, a host PC or server with a Python environment is required.
- Install Python Environment and Activation Scripts as detailed in the [HPK Documentation](https://github.com/rdkcentral/rdk-hpk-documentation/blob/main/README.md#installing-the-python-environment-for-l3-testing-suite)
- To run the `L1` and `L2` test cases follow the [hdmi-cec-L1_L2_TestProcedure.md](docs/pages/hdmi-cec-L1_L2_TestProcedure.md)
- To run the `L3` test cases follow the [hdmi-cec-L3_TestProcedure.md ](docs/pages/hdmi-cec-L3_TestProcedure.md)
