# Snag list / Upgrade for HDMI CEC HAL tests
 
|Version|Comment|
|--------|-------|
|1.0|Initial release|
 

# Table Of Contents
 
- [Change the API return type](Change-the-API-return-type)
- [Remove the unused the arguments](Remove-the-unused-the-arguments)
- [Logging](Logging)
 
 See also [featureRequirements](ut-core-featureRequirements.md)

## Change the API return type
 
- Change the return type of following `API` to `HDMI_CEC_IO_ERROR`
    `HdmiCecOpen ()`, `HdmiCecClose ()`, `HdmiCecAddLogicalAddress ()`, `HdmiCecRemoveLogicalAddress ()`, `HdmiCecGetLogicalAddress()`, `HdmiCecGetLogicalAddress()`, `HdmiCecGetPhysicalAddress()`, `HdmiCecSetRxCallback()`, `HdmiCecSetTxCallback()`, `HdmiCecTx()`, `HdmiCecTxAsync()`

## Remove the unused the arguments
 
- Remove the used arguments devType form the `API` `HdmiCecGetLogicalAddress()`

## Logging

- Use the ut-core loggings once the module is ready.




