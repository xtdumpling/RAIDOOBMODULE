/*++

Copyright (c)  2007 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PartialMirrorHandler.c
    
Abstract:

  Address based Partial Mirroring implementation file. 

Revision History:

--*/

#include "PartialMirrorHandler.h"

//
// Modular variables needed by this driver
//

struct SystemMemoryMapHob             *mSystemMemoryMap=NULL;

EFI_STATUS
EFIAPI
InitPartialMirrorHandler (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  Initializes the PartialMirror SMI Handler Driver.

Arguments:

  ImageHandle  -  The image handle of SMI Handler Driver.
  SystemTable  -  The starndard EFI system table.
    
Returns:

  EFI_OUT_OF_RESOURCES  -  Insufficient resources to complete function.
  EFI_SUCCESS           -  Function has completed successfully.
  Other                 -  Error occured during execution. 

--*/
{
  EFI_STATUS                           Status = EFI_SUCCESS;
  EFI_SMM_VARIABLE_PROTOCOL            *SmmVariable;
  ADDRESS_RANGE_MIRROR_VARIABLE_DATA   *MemoryCurrent = NULL;
  UINTN                                AddressBasedMirrorDataSize = 0;
  UINT32                               AddressBaseMirrorAttribute = ADDRESS_BASED_MIRROR_VARIABLE_ATTRIBUTE;
  EFI_HOB_GUID_TYPE                    *GuidHob;

  //
  // Search for the Memory Map GUID HOB.  If it is not present, then there's nothing we can do. It may not exist on the update path.
  // Dont forget to update this structure after mem hotAdd/hotRemove/Migration otherwise PatrolScrub and MemCopy engine would not work correctly
  //
  mSystemMemoryMap = NULL;
  GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  mSystemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA (GuidHob);

  if(mSystemMemoryMap != NULL) {
      Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&SmmVariable);
      ASSERT_EFI_ERROR (Status);
      AddressBasedMirrorDataSize = sizeof(mSystemMemoryMap->RasMeminfo.MirrorCurrentType);
      MemoryCurrent = &(mSystemMemoryMap->RasMeminfo.MirrorCurrentType);
      Status = SmmVariable->SmmSetVariable(
                                           ADDRESS_RANGE_MIRROR_VARIABLE_CURRENT,
                                           &gAddressBasedMirrorGuid,
                                           AddressBaseMirrorAttribute,
                                           AddressBasedMirrorDataSize,
                                           MemoryCurrent
                                          );
      DEBUG((EFI_D_ERROR, "MemoryCurrent->MirrorVersion %x\n", MemoryCurrent->MirrorVersion));
      DEBUG((EFI_D_ERROR, "MemoryCurrent->MirroredAmountAbove4GB 0x%x\n", MemoryCurrent->MirroredAmountAbove4GB));
      DEBUG((EFI_D_ERROR, "MemoryCurrent->MirrorMemoryBelow4GB 0x%x\n", MemoryCurrent->MirrorMemoryBelow4GB));
      DEBUG((EFI_D_ERROR, "MemoryCurrent->MirrorStatus %x\n", MemoryCurrent->MirrorStatus));

      DEBUG((EFI_D_ERROR, "MemoryRequest->MirrorVersion %x\n", mSystemMemoryMap->RasMeminfo.MirrorRequestType.MirrorVersion));
      DEBUG((EFI_D_ERROR, "MemoryRequest->MirroredAmountAbove4GB 0x%x\n", mSystemMemoryMap->RasMeminfo.MirrorRequestType.MirroredAmountAbove4GB));
      DEBUG((EFI_D_ERROR, "MemoryRequest->MirrorMemoryBelow4GB 0x%x\n", mSystemMemoryMap->RasMeminfo.MirrorRequestType.MirrorMemoryBelow4GB));
      DEBUG((EFI_D_ERROR, "MemoryRequest->MirrorStatus %x\n", mSystemMemoryMap->RasMeminfo.MirrorRequestType.MirrorStatus));
      
  }
  
  return EFI_SUCCESS;
}




