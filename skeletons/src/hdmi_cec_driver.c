#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "hdmi_cec_driver.h"


HDMI_CEC_STATUS HdmiCecOpen(int* handle)
{
  /*TODO: Implement Me!*/
  (void)handle;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecClose(int handle)
{
  /*TODO: Implement Me!*/
  (void)handle;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecSetLogicalAddress(int handle, int* logicalAddresses, int num)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)logicalAddresses;
  (void)num;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecGetPhysicalAddress(int handle, unsigned int* physicalAddress)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)physicalAddress;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)logicalAddresses;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)logicalAddresses;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecGetLogicalAddress(int handle, int* logicalAddress)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)logicalAddress;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)cbfunc;
  (void)data;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void* data)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)cbfunc;
  (void)data;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecTx(int handle, const unsigned char* buf, int len, int* result)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)buf;
  (void)len;
  (void)result;
  return (int)0;
}

HDMI_CEC_STATUS HdmiCecTxAsync(int handle, const unsigned char* buf, int len)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)buf;
  (void)len;
  return (int)0;
}

