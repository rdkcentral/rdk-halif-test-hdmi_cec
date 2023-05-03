#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "hdmi_cec_driver.h"


int HdmiCecOpen(int* handle)
{
  /*TODO: Implement Me!*/
  (void)handle;
  return (int)0;
}

int HdmiCecClose(int handle)
{
  /*TODO: Implement Me!*/
  (void)handle;
  return (int)0;
}

int HdmiCecSetLogicalAddress(int handle, int* logicalAddresses, int num)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)logicalAddresses;
  (void)num;
  return (int)0;
}

void HdmiCecGetPhysicalAddress(int handle, unsigned int* physicalAddress)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)physicalAddress;
}

int HdmiCecAddLogicalAddress(int handle, int logicalAddresses)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)logicalAddresses;
  return (int)0;
}

int HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)logicalAddresses;
  return (int)0;
}

int HdmiCecGetLogicalAddress(int handle, int devType, int* logicalAddress)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)devType;
  (void)logicalAddress;
  return (int)0;
}

int HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void* data)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)cbfunc;
  (void)data;
  return (int)0;
}

int HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void* data)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)cbfunc;
  (void)data;
  return (int)0;
}

int HdmiCecTx(int handle, const unsigned char* buf, int len, int* result)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)buf;
  (void)len;
  (void)result;
  return (int)0;
}

int HdmiCecTxAsync(int handle, const unsigned char* buf, int len)
{
  /*TODO: Implement Me!*/
  (void)handle;
  (void)buf;
  (void)len;
  return (int)0;
}

