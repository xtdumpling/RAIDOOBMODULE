/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file Recovery.c
   
  Tiano PEIM to provide the platform recoveyr functionality.

**/

#include <Setup/IioUniversalData.h>
#include "PlatformEarlyInit.h"

#include <Ppi/BlockIo.h>

//
// Capsule Types supported in this platform module
//
#include <Guid/CapsuleOnFatFloppyDisk.h>
#include <Guid/CapsuleOnFatIdeDisk.h>
#include <Guid/CapsuleOnFatUsbDisk.h>
#include <Guid/CapsuleOnDataCD.h>
#include <Ppi/Cache.h>

#include <Ppi/RecoveryModule.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Ppi/PchInit.h>

#include <Library/PeiServicesLib.h>

//
// Required Service
//
EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI          *This
  );

VOID
AssertNoCapsulesError (
  IN EFI_PEI_SERVICES **PeiServices
  );

VOID
AssertMediaDeviceError (
  IN EFI_PEI_SERVICES **PeiServices
  );

VOID
ReportLoadCapsuleSuccess (
  IN EFI_PEI_SERVICES **PeiServices
  );

VOID
CheckIfMediaPresentOnBlockIoDevice (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN OUT BOOLEAN        *MediaDeviceError,
  IN OUT BOOLEAN        *MediaPresent
  );

//
// Module globals
//
STATIC EFI_PEI_RECOVERY_MODULE_PPI  mRecoveryPpi = { PlatformRecoveryModule };

STATIC EFI_PEI_PPI_DESCRIPTOR         mRecoveryPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRecoveryModulePpiGuid,
  &mRecoveryPpi
};

/**

  Provide the functionality of the Recovery Module.

  @param PeiServices  -  General purpose services available to every PEIM.
    
  @retval EFI_SUCCESS  -  If the interface could be successfully
                  @retval installed.

**/
EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;
// APTIOV_SERVER_OVERRIDE_RC_START : AMI Recovery Module is used
  //Status = PeiServicesInstallPpi (&mRecoveryPpiList);
  Status = EFI_SUCCESS;
// APTIOV_SERVER_OVERRIDE_RC_END : AMI Recovery Module is used
  return Status;
}

/**

  Provide the functionality of the Ea Recovery Module.

  @param PeiServices  -  General purpose services available to every PEIM.
  @param This         -  Pointer to EFI_PEI_RECOVERY_MODULE_PPI.
  
  @retval EFI_SUCCESS      -  If the interface could be successfully
                      @retval installed.
  @retval EFI_UNSUPPORTED  -  Not supported.

**/
EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI          *This
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI           *DeviceRecoveryModule;
  UINTN                                 NumberOfImageProviders;
  BOOLEAN                               ProviderAvailable;
  UINTN                                 NumberRecoveryCapsules;
  UINTN                                 RecoveryCapsuleSize;
  EFI_GUID                              DeviceId;
  BOOLEAN                               ImageFound;
  EFI_PHYSICAL_ADDRESS                  Address;
  VOID                                  *Buffer;
  EFI_CAPSULE_HEADER                    *CapsuleHeader;
  EFI_PEI_HOB_POINTERS                  Hob;
  EFI_PEI_HOB_POINTERS                  HobOld;
  EFI_HOB_CAPSULE_VOLUME                *CapsuleHob;
  BOOLEAN                               HobUpdate;
  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader;
  UINTN                                 Index;

  static EFI_GUID                       mEfiCapsuleHeaderGuid = EFI_CAPSULE_GUID;

  Index = 0;

  Status                  = EFI_SUCCESS;
  HobUpdate               = FALSE;

  ProviderAvailable       = TRUE;
  ImageFound              = FALSE;
  NumberOfImageProviders  = 0;

  DeviceRecoveryModule    = NULL;

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Recovery Entry\n"));

  //
  // Search the platform for some recovery capsule if the DXE IPL
  // discovered a recovery condition and has requested a load.
  //
  while (ProviderAvailable) {

    Status = PeiServicesLocatePpi (
              &gEfiPeiDeviceRecoveryModulePpiGuid,
              Index,
              NULL,
              &DeviceRecoveryModule
              );

    if (!EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Device Recovery PPI located\n"));
      NumberOfImageProviders++;

      Status = DeviceRecoveryModule->GetNumberRecoveryCapsules (
                                      PeiServices,
                                      DeviceRecoveryModule,
                                      &NumberRecoveryCapsules
                                      );

      DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Number Of Recovery Capsules: %d\n", NumberRecoveryCapsules));

      if (NumberRecoveryCapsules == 0) {
        Index++;
      } else {
        break;
      }
    } else {
      ProviderAvailable = FALSE;
    }
  }
  //
  // The number of recovery capsules is 0.
  //
  if (!ProviderAvailable) {
    AssertNoCapsulesError (PeiServices);
  }
  //
  // If there is an image provider, get the capsule ID
  //
  if (ProviderAvailable) {
    RecoveryCapsuleSize = 0;

    Status = DeviceRecoveryModule->GetRecoveryCapsuleInfo (
                                    PeiServices,
                                    DeviceRecoveryModule,
                                    0,
                                    &RecoveryCapsuleSize,
                                    &DeviceId
                                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Recovery Capsule Size: %d\n", RecoveryCapsuleSize));

    //
    // Only support the 2 capsule types known
    // Future enhancement is to rank-order the selection
    //
    if ((!CompareGuid (&DeviceId, &gPeiCapsuleOnFatIdeDiskGuid)) &&
        (!CompareGuid (&DeviceId, &gPeiCapsuleOnDataCDGuid)) &&
       (!CompareGuid (&DeviceId, &gPeiCapsuleOnFatUsbDiskGuid))
        ) {
      return EFI_UNSUPPORTED;
    }

    Buffer  = NULL;
    Address = (UINTN) AllocatePages ((RecoveryCapsuleSize - 1) / 0x1000 + 1);
    ASSERT (Address);

    Buffer = (UINT8 *) (UINTN) Address;

    Status = DeviceRecoveryModule->LoadRecoveryCapsule (
                                    PeiServices,
                                    DeviceRecoveryModule,
                                    0,
                                    Buffer
                                    );

    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "LoadRecoveryCapsule Returns: %r\n", Status));

    if (Status == EFI_DEVICE_ERROR) {
      AssertMediaDeviceError (PeiServices);
    }

    if (EFI_ERROR (Status)) {
      return Status;
    } else {
      ReportLoadCapsuleSuccess (PeiServices);
    }
    //
    // Update FV Hob if found
    //
    Status      = PeiServicesGetHobList (&Hob.Raw);
    HobOld.Raw  = Hob.Raw;
    while (!END_OF_HOB_LIST (Hob)) {
      if (Hob.Header->HobType == EFI_HOB_TYPE_FV) {
        DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Hob FV Length: %x\n", Hob.FirmwareVolume->Length));
        //
        // Why is it a FV hob if it is greater than 0x50000?
        //
#ifndef PC_HOOK //PC has nested MRC FV, whose size is greater than 0x50000. It should not be replaced with main FV.
        if (Hob.FirmwareVolume->Length > 0x50000) {
          HobUpdate = TRUE;
          //
          // This looks like the Hob we are interested in
          //
          DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Hob Updated\n"));
          Hob.FirmwareVolume->BaseAddress = (UINTN) Buffer;
          Hob.FirmwareVolume->Length      = RecoveryCapsuleSize;
        }
#endif
      }

      Hob.Raw = GET_NEXT_HOB (Hob);
    }
    //
    // Check if the top of the file is a firmware volume header
    //
    FvHeader      = (EFI_FIRMWARE_VOLUME_HEADER *) Buffer;
    CapsuleHeader = (EFI_CAPSULE_HEADER *) Buffer;
    if (FvHeader->Signature == EFI_FVH_SIGNATURE) {
      //
      // build FV Hob if it is not built before
      //
      if (!HobUpdate) {
        DEBUG ((EFI_D_INFO | EFI_D_LOAD, "FV Hob is not found, Build FV Hob then..\n"));
        BuildFvHob (
          (UINTN) Buffer,
          FvHeader->FvLength
          );
        if ((**PeiServices).Hdr.Revision >= 0x00010000) {
          InstallFvInfoPpi(PeiServices, Buffer);
        }
      }
      //
      // Point to the location immediately after the FV.
      //
      CapsuleHeader = (EFI_CAPSULE_HEADER *) ((UINT8 *) Buffer + FvHeader->FvLength);
    }
    //
    // Check if pointer is still within the buffer
    //
    if ((UINTN) CapsuleHeader < (UINTN) ((UINT8 *) Buffer + RecoveryCapsuleSize)) {
      //
      // Check if it is a capsule
      //
      if (CompareGuid ((EFI_GUID *) CapsuleHeader, &mEfiCapsuleHeaderGuid)) {
        //
        // Set bootmode to capsule update so the capsule hob gets the right bootmode in the hob header.
        // Status = (**PeiServices).SetBootMode (PeiServices, BOOT_ON_FLASH_UPDATE);
        //
        // Build capsule hob
        //
        Status = PeiServicesCreateHob (
                  EFI_HOB_TYPE_CV,
                  sizeof (EFI_HOB_CAPSULE_VOLUME),
                  &CapsuleHob
                  );
        if (EFI_ERROR (Status)) {
          return Status;
        }
        CapsuleHob->BaseAddress = (UINT64)CapsuleHeader + 
                                  (UINT64)(EFI_CAPSULE_HEADER *)CapsuleHeader->HeaderSize;

        CapsuleHob->Length      = (UINT64)(EFI_CAPSULE_HEADER *)CapsuleHeader->CapsuleImageSize - 
                                  (UINT64)(EFI_CAPSULE_HEADER *)CapsuleHeader->HeaderSize;

        //
        // Find main FV within the capsule.
        // Not implemented yet.
        //
      }
    }
  }

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Recovery Module Returning: %r\n", Status));
  return Status;
}

/*
  AssertNoCapsulesError:
  There were no recovery capsules found. 
  Case 1: Report the error that no recovery block io device/media is readable and assert.  
  Case 2: Report the error that there is no media present on any recovery block io device and assert.
  Case 3: There is media present on some recovery block io device, 
          but there is no recovery capsule on it.  Report the error and assert.   
*/
VOID
AssertNoCapsulesError (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  BOOLEAN MediaDeviceError;
  BOOLEAN MediaPresent;

  MediaDeviceError  = TRUE;
  MediaPresent      = FALSE;

  CheckIfMediaPresentOnBlockIoDevice (PeiServices, &MediaDeviceError, &MediaPresent);
/*  if (MediaDeviceError) {
    ReportStatusCode (
      (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
      (EFI_PERIPHERAL_RECOVERY_MEDIA | EFI_P_EC_MEDIA_DEVICE_ERROR)
      );

  } else if (!MediaPresent) {
    ReportStatusCode (
      (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
      (EFI_PERIPHERAL_RECOVERY_MEDIA | EFI_P_EC_MEDIA_NOT_PRESENT)
      );

  } else {
    ReportStatusCode (
      (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
      (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_EC_NO_RECOVERY_CAPSULE)
      );
  }*/
  //
  // Hang.
  //
  CpuDeadLoop();
}

#define MAX_BLOCK_IO_PPI  32

/*
  CheckIfMediaPresentOnBlockIoDevice: 
  Checks to see whether there was a media device error or to see if there is media present.  
*/
VOID
CheckIfMediaPresentOnBlockIoDevice (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN OUT BOOLEAN        *MediaDeviceError,
  IN OUT BOOLEAN        *MediaPresent
  )
{
  EFI_STATUS                      Status;
  UINTN                           BlockIoPpiInstance;
  EFI_PEI_RECOVERY_BLOCK_IO_PPI *BlockIoPpi;
  UINTN                           NumberBlockDevices;
  EFI_PEI_BLOCK_IO_MEDIA          Media;

  *MediaDeviceError = TRUE;
  *MediaPresent     = FALSE;

  for (BlockIoPpiInstance = 0; BlockIoPpiInstance < MAX_BLOCK_IO_PPI; BlockIoPpiInstance++) {
    Status = PeiServicesLocatePpi (
              &gEfiPeiVirtualBlockIoPpiGuid,
              BlockIoPpiInstance,
              NULL,
              &BlockIoPpi
              );
    if (EFI_ERROR (Status)) {
      //
      // Done with all Block Io Ppis
      //
      break;
    }

    Status = BlockIoPpi->GetNumberOfBlockDevices (
                          PeiServices,
                          BlockIoPpi,
                          &NumberBlockDevices
                          );
    if (EFI_ERROR (Status) || (NumberBlockDevices == 0)) {
      continue;
    }
    //
    // Just retrieve the first block
    //
    Status = BlockIoPpi->GetBlockDeviceMediaInfo (
                          PeiServices,
                          BlockIoPpi,
                          0,
                          &Media
                          );
    if (!EFI_ERROR (Status)) {
      *MediaDeviceError = FALSE;
      if (Media.MediaPresent) {
        *MediaPresent = TRUE;
        break;
      }
    }
  }
}

/**

    GC_TODO: add routine description

    @param PeiServices - GC_TODO: add arg description

    @retval None

**/
VOID
AssertMediaDeviceError (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
/*  ReportStatusCode (
    (EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED),
    (EFI_PERIPHERAL_RECOVERY_MEDIA | EFI_P_EC_MEDIA_DEVICE_ERROR)
    );
*/
  CpuDeadLoop ();
}

/**

    GC_TODO: add routine description

    @param PeiServices - GC_TODO: add arg description

    @retval None

**/
VOID
ReportLoadCapsuleSuccess (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  //
  // EFI_SW_PEI_PC_CAPSULE_START: (from the status code spec):
  // Loaded the recovery capsule.  About to hand off control to the capsule.
  //
/*  ReportStatusCode (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEIM_PC_CAPSULE_LOAD_SUCCESS)
    );*/
}

/**

    GC_TODO: add routine description

    @param PeiServices - GC_TODO: add arg description
    @param UdsHobPtr   - GC_TODO: add arg description

    @retval None

**/
VOID
GetIioUdsHob (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN IIO_UDS                  **UdsHobPtr
  )
{
  EFI_GUID           UniversalDataGuid = IIO_UNIVERSAL_DATA_GUID;
  EFI_HOB_GUID_TYPE  *GuidHob;

  ASSERT(UdsHobPtr);

  *UdsHobPtr = NULL;
  
  GuidHob       = GetFirstGuidHob (&UniversalDataGuid);
  if (GuidHob){
    *UdsHobPtr  = GET_GUID_HOB_DATA (GuidHob);
    return;
  }

  ASSERT(FALSE);
}
/**

  Initialize the GPIO IO selection, GPIO USE selection, and GPIO signal inversion registers

  @param PeiServices   General purpose services available to every PEIM.
  
  @retval EFI_SUCCESS   Operation success.

**/
EFI_STATUS
PlatformPchUsbInit (
  IN EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  return Status;
}

