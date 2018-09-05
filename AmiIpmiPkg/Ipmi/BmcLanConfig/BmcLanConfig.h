//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file BmcLanConfig.h
    Header file for BmcLanConfig modular part

**/

#ifndef _BMC_LAN_CONFIG_H_
#define _BMC_LAN_CONFIG_H_

//----------------------------------------------------------------------

#include "Token.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Uefi/UefiBaseType.h>
#include <Include/IpmiNetFnTransportDefinitions.h>
#include <Protocol/IPMITransportProtocol.h>
#include <ServerMgmtSetupVariable.h>

//----------------------------------------------------------------------

//
// NetHeader.h header includes
//
typedef     UINT32                      IP4_ADDR;
#define     EFI_IP_PROTO_ICMP           0x01

#define     EFI_IP4_TO_U32(EfiIpAddr)   (*(IP4_ADDR*)((EfiIpAddr).Addr))
#define     EFI_IP4_NTOHL(EfiIp)        (NTOHL (EFI_IP4_TO_U32 ((EfiIp))))
#define     SEC_TO_NS                   10000000
#define     IPMI_STALL                  1000

#pragma pack(1)
typedef struct {
  UINT8         BmcLan;
  UINT8         StationIp[4];
  UINT8         Subnet[4];
  UINT8         RouterIp[4];
  UINT8         RouterMac[6];
} BMC_IP_BIOS_SETTINGS;
#pragma pack()

VOID
SetZeroIp(
  OUT  CHAR16      ZeroIp[]
  );

VOID
SetZeroMac(
  OUT  CHAR16      ZeroMac[]
  );

EFI_STATUS
ConvertToIpAddress(
  OUT  UINT8      Destination[],
  IN  CHAR16      Source[]
  );

EFI_STATUS
ConvertToMacAddress(
  OUT UINT8       Destination[],
  IN  CHAR16      Source[]
  );

EFI_STATUS 
SetStaticBmcNetworkParameters(
  IN  UINT8       LanChannelNumber,
  IN  BMC_IP_BIOS_SETTINGS    BmcLanIpSetupValues
  );

EFI_STATUS
SetIpAddressSource (
  IN  UINT8      LanChannelNumber,
  IN  UINT8      IpAddessSource
);

EFI_STATUS
IpmiWaitSetInProgressClear (
  UINT8         LanChannelNumber                   // Lan Channel Number
);

VOID
SetZeroIp6(
  IN OUT  CHAR16      ZeroIp6[] 
);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
