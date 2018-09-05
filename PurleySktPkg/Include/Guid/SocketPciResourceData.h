//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SocketPciResourceData.h

  Data format for Universal Data Structure

**/
#ifndef   __SOCKET_PCI_RESOURCE_CONFIG_DATA_H__
#define   __SOCKET_PCI_RESOURCE_CONFIG_DATA_H__


#include <UncoreCommonIncludes.h>
#include "SocketConfiguration.h"

extern EFI_GUID gEfiSocketPciResourceDataGuid;
#define SOCKET_PCI_RESOURCE_CONFIGURATION_DATA_NAME L"SocketPciResourceConfigData"

#pragma pack(1)
typedef struct {
  //
  // Used by the PciHostBridge DXE driver, these variables don't need to be exposed through setup options
  // The variables are used as a communication vehicle from the PciHostBridge DXE driver to an OEM hook
  // which updates the KTI resource map
  //
  UINT16  PciSocketIoBase[MAX_SOCKET];
  UINT16  PciSocketIoLimit[MAX_SOCKET];
  UINT32  PciSocketMmiolBase[MAX_SOCKET];
  UINT32  PciSocketMmiolLimit[MAX_SOCKET];
  UINT64  PciSocketMmiohBase[MAX_SOCKET];
  UINT64  PciSocketMmiohLimit[MAX_SOCKET];
  UINT16  PciResourceIoBase[TOTAL_IIO_STACKS];
  UINT16  PciResourceIoLimit[TOTAL_IIO_STACKS];
  UINT32  PciResourceMem32Base[TOTAL_IIO_STACKS];
  UINT32  PciResourceMem32Limit[TOTAL_IIO_STACKS];
  UINT64  PciResourceMem64Base[TOTAL_IIO_STACKS];
  UINT64  PciResourceMem64Limit[TOTAL_IIO_STACKS];
} SOCKET_PCI_RESOURCE_CONFIGURATION_DATA;
#pragma pack()

#endif // __SOCKET_PCI_RESOURCE_CONFIG_DATA_H__


