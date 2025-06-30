# Unit Testing Suite For HDMI CEC HAL

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Description](#description)
- [Reference Documents](#reference-documents)
- [Notes](#notes)

## Acronyms, Terms and Abbreviations

- `HAL`- Hardware Abstraction Layer
- `HDMI` - High Definition Multimedia Interface
- `CEC` - Consumer Electronics Control
- `L1` - Functional Tests
- `L2` - Module functional Testing
- `L3` - Module testing with External Stimulus is required to validate and control device
- `API` - Application Programming Interface

## Description

This repository contains the Unit Test Suites(L1 & L2) for HDMI CEC `HAL`.

## Reference Documents

|SNo|Document Name|Document Description|Document Link|
|---|-------------|--------------------|-------------|
|1|`HAL` Specification Document|This document provides specific information on the APIs for which tests are written in this module|[hdmi-cec_halSpec.md](https://github.com/rdkcentral/rdk-halif-hdmi_cec/blob/1.3.9/docs/pages/hdmi-cec_halSpec.md)|
|2|High Level Test Spec for sink device|High Level Test Specification Documentation for the `HDMI` `CEC` Sink device|[hdmi-cec-sink_High-Level_TestSpec.md](docs/pages/hdmi-cec-sink_High-Level_TestSpec.md )|
|3|`L2` Low Level Test Spec for sink device|`L2` Low Level Test Specification and Procedure Documentation for the `HDMI` `CEC` Sink device|[hdmi-cec-sink_L2_Low-Level_TestSpec.md](docs/pages/hdmi-cec-sink_L2_Low-Level_TestSpec.md)|
|4|High Level Test Spec for Source device|High Level Test Specification Documentation for the `HDMI` `CEC` Source device|[hdmi-cec-source_High-Level_TestSpec.md](docs/pages/hdmi-cec-source_High-Level_TestSpec.md)|
|5|`L2` Low Level Test Spec for Source device|`L2` Low Level Test Specification and Procedure Documentation for the `HDMI` `CEC` Source device|[hdmi-cec-source_L2_Low-Level_TestSpec.md](docs/pages/hdmi-cec-source_L2_Low-Level_TestSpec.md)|
|6|`L3` Low Level Test Specification|`L3` Low Level Test Specification for the `HDMI` `CEC`|[hdmi-cec-L3-Low-Level_TestSpec.md](docs/pages/hdmi-cec-L3-Low-Level_TestSpec.md)|
|7|`L3` Test procedure document|`L3` Test procedure document for the `HDMI` `CEC`|[hdmi-cec-L3_TestProcedure.md](docs/pages/hdmi-cec-L3_TestProcedure.md)|

## Notes

- All APIs need to be implemented in this current version. If any API is not supported, please add stub implementation with return type HDMI_CEC_IO_OPERATION_NOT_SUPPORTED for the same.
- Building against the actual library may introduce SOC dependencies. Hence, a template SKELETON library is created without SOC dependencies. On the real platform (target), it can be mounted, copied and bound with the actual library.
- When running the binary, remember to include a profile file as an argument for designated test cases. The following example illustrates this:

```bash
./hal_test -p sink_hdmiCEC.yml
```

Alternatively, use the run.sh script with the profile file:

```bash
./run.sh -p /absolute/path/to/profile/file
```

- Profile files define the configuration for the platform available at [sink HDMI CEC](./profiles/sink/sink_hdmiCEC.yml), [source HDMI CEC](./profiles/source/source_hdmiCEC.yml), [stb source device](./profiles/stb-source-device.yaml), [tv panel](./profiles/tv_panel_5_devices.yaml )

### Setting Python environment for running the `L1` `L2` and `L3` automation test cases

- For running the `L1` `L2` and `L3` test suite, a host PC or server with a Python environment is required.
- Install Python Environment and Activation Scripts as detailed in the [HPK Documentation](https://github.com/rdkcentral/rdk-hpk-documentation/blob/main/README.md#installing-the-python-environment-for-l3-testing-suite)
- To run the `L1` and `L2` test cases follow the [hdmi-cec-L1_L2_TestProcedure.md](docs/pages/hdmi-cec-L1_L2_TestProcedure.md)
- To run the `L3` test cases follow the [hdmi-cec-L3_TestProcedure.md ](docs/pages/hdmi-cec-L3_TestProcedure.md)
