# Unit Testing Suite For HDMI CEC HAL

## Table of Contents

- [Acronyms, Terms and Abbreviations](#acronyms-terms-and-abbreviations)
- [Description](#description)
- [Reference Documents](#reference-documents)
- [Notes](#notes)

## Acronyms, Terms and Abbreviations

- `L1` - Level 1 Functional Tests
- `L2` - Level 2 Unit Tests
- `L3` - Level 3 External Stimulas Tests
- `HAL`- Hardware Abstraction Layer

## Description

This repository contains the Unit Test Suitesfor HDMI CEC `HAL`.

## Reference Documents

<!-- Need to update links to rdkcentral and point to branch main-->
|#|Document Name|Document Description|
|---|-------------|--------------------|
|1|[hdmi-cec_halSpec.md -v1.3.7](https://github.com/rdkcentral/rdk-halif-hdmi_cec/blob/1.3.7/docs/pages/hdmi-cec_halSpec.md "hdmi-cec_halSpec.md")|This document provides specific information on the APIs for which tests are written in this module||
|2|[test_l1_hdmi_cec_driver.c](src/test_l1_hdmi_cec_driver.c)|`L1` Test Case File for this module |
|3|[hdmi_cec_sink_tests.md](docs/pages/hdmi_cec_sink_tests.md)| Hdmi CEC Sink Testing requirements for `L2/3`|

## Notes

- All APIs need to be implemented in this current version. If any API is not supported, please add stub implementation with return type `HDMI_CEC_IO_OPERATION_NOT_SUPPORTED` for the same.
- Building against the actual library may introduce SOC dependencies. Hence, a template SKELETON library is created without SOC dependencies. On the real platform (target), it can be mounted, copied and bound with the actual library.
