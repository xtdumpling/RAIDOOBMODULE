//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This is an implementation of BERT table. This is a
  boottime driver and provides Whea error logging functionality.

  Copyright (c) 2009 - 2016, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

//
// Includes
//
#include <Library/DebugLib.h>
#include <Library/WheaErrorLib.h>
#include <Library/WheaPlatformHooksLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Whea/WheaSupport/WheaSupport.h>
//
// Consumed protocols
//
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/WheaBootProtocol.h>

EFI_WHEA_BOOT_PROTOCOL *mWheaBootInst;

//
// Function Prototypes
//
EFI_STATUS
EFIAPI
WheaPlatformBootEntry(
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  );

//
// Initialize BERT
//
EFI_STATUS
WheaBootStartElog( 
  IN EFI_WHEA_BOOT_PROTOCOL     *This
  )
{
  UINT32      BertSize;
  EFI_STATUS  Status = EFI_SUCCESS;

  if (This == NULL)
    return EFI_NOT_FOUND;

  BertSize = This->BertSize;

  ZeroMem ((VOID *)(UINTN)This->BertBaseAddress, BertSize); 
  
  return Status;
}

//
// End BERT logging
//
EFI_STATUS
WheaBootEndElog( 
  IN EFI_WHEA_BOOT_PROTOCOL     *This
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  return Status;
}

//
// Log Memory Error Records in BERT
//
EFI_STATUS
WheaBootElogMemory( 
  IN EFI_WHEA_BOOT_PROTOCOL   *This,
  IN UINT16                   ErrorType,
  IN MEMORY_DEV_INFO          *MemInfo
  )
{
  GENERIC_ERROR_STATUS        *ErrStsBlk;

  if(This == NULL)
    return EFI_NOT_FOUND;

  ErrStsBlk = (GENERIC_ERROR_STATUS *)(UINTN)This->BertBaseAddress;

  if (ErrStsBlk->ErrDataSize >= This->BertSize)
    return EFI_OUT_OF_RESOURCES;

  return WheaGenElogMemory (ErrorType, MemInfo, ErrStsBlk);

}

//
// Log PCIE Root Bridge Device Error Records in BERT
//
EFI_STATUS
WheaBootElogPcieRootDevBridge( 
  IN EFI_WHEA_BOOT_PROTOCOL   *This,
  IN UINT16                   ErrorType,
  IN PCIE_PCI_DEV_INFO        *ErrPcieDev

  )
{
  GENERIC_ERROR_STATUS        *ErrStsBlk;

  if(This == NULL)
    return EFI_NOT_FOUND;

  ErrStsBlk = (GENERIC_ERROR_STATUS *)(UINTN)This->BertBaseAddress;

  if (ErrStsBlk->ErrDataSize >= This->BertSize)
    return EFI_OUT_OF_RESOURCES;

  return WheaGenElogPcieRootDevBridge (ErrorType, ErrPcieDev, ErrStsBlk);
}

//
// Log PCI device Error Records in BERT
//
EFI_STATUS
WheaBootElogPciDev( 
  IN EFI_WHEA_BOOT_PROTOCOL   *This,
  IN UINT16                   ErrorType,
  IN PCIE_PCI_DEV_INFO        *ErrPcieDev

  )
{
  GENERIC_ERROR_STATUS        *ErrStsBlk;

  if(This == NULL)
    return EFI_NOT_FOUND;

  ErrStsBlk = (GENERIC_ERROR_STATUS *)(UINTN)This->BertBaseAddress;

  if (ErrStsBlk->ErrDataSize >= This->BertSize)
    return EFI_OUT_OF_RESOURCES;

  return WheaGenElogPciDev (ErrorType, ErrPcieDev, ErrStsBlk);
}

//
// Log Processor Error Records in BERT
//
EFI_STATUS
WheaBootElogProcessor( 
  IN EFI_WHEA_BOOT_PROTOCOL   *This,
  IN UINT16                   ErrorType,
  IN CPU_DEV_INFO             *CpuInfo
  )
{
  GENERIC_ERROR_STATUS        *ErrStsBlk;

  if(This == NULL)
    return EFI_NOT_FOUND;

  ErrStsBlk = (GENERIC_ERROR_STATUS *)(UINTN)This->BertBaseAddress;

  if (ErrStsBlk->ErrDataSize >= This->BertSize)
    return EFI_OUT_OF_RESOURCES;

  return WheaGenElogProcessor(ErrorType, CpuInfo, ErrStsBlk);
}


//
// Driver Entry Point
//
EFI_STATUS
EFIAPI
WheaPlatformBootEntry(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description: 

  Entry point of the Whea support driver. 

Arguments:  

  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

Returns:  

  EFI_SUCCESS:              Driver initialized successfully
  EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

--*/
{
  EFI_STATUS                          Status;
  EFI_WHEA_SUPPORT_PROTOCOL           *WheaSupport;
  EFI_HANDLE                          Handle;
  WHEA_SUPPORT_INST                   *WheaInst;

  Status = gBS->LocateProtocol (&gEfiWheaSupportProtocolGuid, NULL, &WheaSupport);
  if (EFI_ERROR(Status))return Status;

  WheaInst = WHEA_SUPPORT_INST_FROM_THIS(WheaSupport);

  // Allocate the buffer for protocol data structure
  Status = gBS->AllocatePool (
           EfiBootServicesData,
           sizeof (EFI_WHEA_BOOT_PROTOCOL),
           &mWheaBootInst
           );

  ASSERT_EFI_ERROR (Status);

  mWheaBootInst->BertBaseAddress = WheaInst->Bert->BootErrorRegion;
  mWheaBootInst->BertSize = WheaInst->Bert->BootErrorRegionLen;

  mWheaBootInst->WheaBootStartElog = WheaBootStartElog;
  mWheaBootInst->WheaBootEndElog = WheaBootEndElog;
  mWheaBootInst->WheaBootElogMemory = WheaBootElogMemory;
  mWheaBootInst->WheaBootElogPcieRootDevBridge = WheaBootElogPcieRootDevBridge;
  mWheaBootInst->WheaBootElogPciDev = WheaBootElogPciDev;
  mWheaBootInst->WheaBootElogProcessor = WheaBootElogProcessor;

// Install protocol
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiWheaBootProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  mWheaBootInst
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
