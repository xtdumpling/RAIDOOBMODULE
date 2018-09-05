//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************

//**********************************************************************
/** @file NetworkStackSetup.h
    NetworkStackSetup definitions

**/
//**********************************************************************
#ifndef __NETWORK_STACK_SETUP_H__
#define __NETWORK_STACK_SETUP_H__

#define NETWORK_STACK_GUID \
  { 0xD1405D16, 0x7AFC, 0x4695, 0xBB, 0x12, 0x41, 0x45, 0x9D, 0x36, 0x95, 0xA2 }

#pragma pack(1)
typedef struct{
  UINT8             Enable;
  UINT8             Ipv4Pxe;
  UINT8             Ipv6Pxe;
  UINT8             IpsecCertificate;
  UINT8             PxeBootWaitTime;
  UINT8             MediaDetectCount;
  UINT8             Ipv4Http;
  UINT8             Ipv6Http;
} NETWORK_STACK;
#pragma pack()

#define MIN_MEDIA_DETECT_COUNT       1
#define MAX_MEDIA_DETECT_COUNT      50
#define DEFAULT_MEDIA_DETECT_COUNT  MIN_MEDIA_DETECT_COUNT  // Should be within MIN_MEDIA_DETECT_COUNT and MAX_MEDIA_DETECT_COUNT range

extern EFI_GUID gEfiNetworkStackSetupGuid;

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
