//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**@file
  Header file for SMM Access Driver.

  This file includes package header files, library classes and protocol, PPI & GUID definitions.

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.
**/

#ifndef _SMM_ACCESS_DRIVER_H
#define _SMM_ACCESS_DRIVER_H

#include <PiDxe.h>
#include <IndustryStandard/Pci.h>
#include <Setup/IioUniversalData.h>

#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>

#include <Iio/IioRegs.h>
#include <Cpu/CpuDataStruct.h>
#include <Cpu/CpuRegs.h>
#include <Cpu/CpuBaseLib.h>
//
// Driver Consumed Protocol Prototypes
//
#include <Protocol/IioUds.h>
#include <Protocol/PciRootBridgeIo.h>

//
// Driver Consumed GUID Prototypes
//
#include <Guid/SmramMemoryReserve.h>

//
// Driver produced protocol
//
#include <Protocol/SmmAccess2.h>

//
// Constant definition
//
#define  MAX_SMRAM_RANGES    4

//
// Private data structure
//
#define  SMM_ACCESS_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('i', 's', 'm', 'a')

typedef struct {
  UINTN                            Signature;
  EFI_HANDLE                       Handle;
  EFI_SMM_ACCESS2_PROTOCOL         SmmAccess;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo;  
  UINTN                            NumberRegions;
  EFI_SMRAM_DESCRIPTOR             SmramDesc[MAX_SMRAM_RANGES];
  UINT8                            TsegSize;
  UINT64                           SMMRegionState;  
  UINT8                            ActualNLIioBusNumber;
} SMM_ACCESS_PRIVATE_DATA;


#define SMM_ACCESS_PRIVATE_DATA_FROM_THIS(a) \
  CR ( \
  a, \
  SMM_ACCESS_PRIVATE_DATA, \
  SmmAccess, \
  SMM_ACCESS_PRIVATE_DATA_SIGNATURE \
  )


//
// Prototypes
// Driver model protocol interface
//
/**
  
  This is the standard EFI driver point that detects
  whether there is an proper chipset in the system
  and if so, installs an SMM Access Protocol.

  @param ImageHandle  -  Handle for the image of this driver.
  @param SystemTable  -  Pointer to the EFI System Table.

  @retval EFI_SUCCESS      -  Protocol successfully started and installed.
  @retval EFI_UNSUPPORTED  -  Protocol can't be started.

**/
EFI_STATUS
EFIAPI
SmmAccessDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
;

/**
  
  This routine accepts a request to "open" a region of SMRAM.  The
  region could be legacy ABSEG, HSEG, or TSEG near top of physical memory.
  The use of "open" means that the memory is visible from all boot-service
  and SMM agents.

  @param This             -  Pointer to the SMM Access Interface.
  @param DescriptorIndex  -  Region of SMRAM to Open.

  @retval EFI_SUCCESS            -  The region was successfully opened.
  @retval EFI_DEVICE_ERROR       -  The region could not be opened because locked by
                            @retval chipset.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.

**/
EFI_STATUS
EFIAPI
Open (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
;

/**
  
  This routine accepts a request to "close" a region of SMRAM.  This is valid for 
  compatible SMRAM region.

  @param This             -  Pointer to the SMM Access Interface.
  @param DescriptorIndex  -  Region of SMRAM to Close.

  @retval EFI_SUCCESS            -  The region was successfully closed.
  @retval EFI_DEVICE_ERROR       -  The region could not be closed because locked by
                            @retval chipset.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.
  
**/
EFI_STATUS
EFIAPI
Close (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
;

/**

  This routine accepts a request to "lock" SMRAM.  The
  region could be legacy AB or TSEG near top of physical memory.
  The use of "lock" means that the memory can no longer be opened
  to BS state..

  @param This             -  Pointer to the SMM Access Interface.
  @param DescriptorIndex  -  Region of SMRAM to Lock.

  @retval EFI_SUCCESS            -  The region was successfully locked.
  @retval EFI_DEVICE_ERROR       -  The region could not be locked because at least
                            @retval one range is still open.
  @retval EFI_INVALID_PARAMETER  -  The descriptor index was out of bounds.
  
**/
EFI_STATUS
EFIAPI
Lock (
  IN EFI_SMM_ACCESS2_PROTOCOL *This
  )
;

/**
  
  This routine services a user request to discover the SMRAM
  capabilities of this platform.  This will report the possible
  ranges that are possible for SMRAM access, based upon the 
  memory controller capabilities.

  @param This          -  Pointer to the SMRAM Access Interface.
  @param SmramMapSize  -  Pointer to the variable containing size of the
                          buffer to contain the description information.
  @param SmramMap      -  Buffer containing the data describing the Smram
                          region descriptors.

  @retval EFI_BUFFER_TOO_SMALL  -  The user did not provide a sufficient buffer.
  @retval EFI_SUCCESS           -  The user provided a sufficiently-sized buffer.

**/
EFI_STATUS
EFIAPI
GetCapabilities (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL     *This,
  IN OUT UINTN                   *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR    *SmramMap
  )
;
VOID
SyncRegionState2SmramDesc(
  IN BOOLEAN  OrAnd,
  IN UINT64    Value
  );

#endif
