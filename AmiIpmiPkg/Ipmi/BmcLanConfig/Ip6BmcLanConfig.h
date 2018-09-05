//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file Ip6BmcLanConfig.h
    Header file for Ip6BmcLanConfig.c file.

**/

#ifndef _IP6_BMC_LAN_CONFIG_H_
#define _IP6_BMC_LAN_CONFIG_H_

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
#include "ServerMgmtSetupVariable.h"

//----------------------------------------------------------------------

//
// NetHeader.h header includes
//

#define     IPMI_STALL                  1000


EFI_STATUS
IpmiWaitSetInProgressClear (
  IN UINT8         LanChannelNumber                   // Lan Channel Number
);

EFI_STATUS
GetIp6Supported (
  IN UINT8      LanChannelNumber //LAN channel number
);

EFI_STATUS
SetIp6AddressSource (
  IN  UINT8      LanChannelNumber, // LAN channel number
  IN  UINT8      IsStaticSource    // Parameter used to select Static or Dynamic source
);

EFI_STATUS
SetIp6RouterAddressSource (
  IN  UINT8      LanChannelNumber // LAN channel number
);

EFI_STATUS
EFIAPI
ConvertStrToBmcIp6Address (
  IN CONST CHAR16                *String,     // String need to be converted.
  OUT      IPMI_IPV6_ADDRESS     *Ip6Address  // Converted Ipv6 address value.
);

EFI_STATUS
SetIp6StaticAddressParam (
  IN UINT8             LanChannelNumber,   // LAN channel number
  IN IPMI_IPV6_ADDRESS Ip6Addr,            // Address value need to be set.
  IN  UINT8            PrefixLength        // Prefix length need to be set
);

EFI_STATUS
SetIp6StaticRouterAddressParam (
  IN  UINT8             LanChannelNumber, // LAN channel number
  IN  IPMI_IPV6_ADDRESS Ip6Addr           // Address value need to be set
);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
