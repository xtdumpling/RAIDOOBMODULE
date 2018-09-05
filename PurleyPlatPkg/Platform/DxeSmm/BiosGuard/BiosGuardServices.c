/** @file
  BIOS Guard Driver implements the BIOS Guard Host Controller Compatibility Interface.

@copyright
  Copyright (c) 2011 - 2016 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by the
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and is uniquely
  identified as "Intel Reference Module" and is licensed for Intel
  CPUs and chipsets under the terms of your license agreement with
  Intel or your vendor. This file may be modified by the user, subject
  to additional terms of the license agreement.

@par Specification Reference:
**/

#include "BiosGuardServices.h"
#include <Guid/PlatformInfo.h>
#include <Library/SetupLib.h>

///
/// Global variables
///
static EFI_SMM_SYSTEM_TABLE2               *gSmst;
BIOSGUARD_PROTOCOL                  *mBiosGuardProtocol;
EFI_SMM_CPU_SERVICE_PROTOCOL        *mSmmCpuService;
BIOSGUARD_HOB                       *mBiosGuardHobPtr;
EFI_SMM_IO_TRAP_DISPATCH2_PROTOCOL  *mPchIoTrap;
static EFI_ACPI_SDT_PROTOCOL        *pAcpiSdt     = NULL;
static EFI_ACPI_TABLE_PROTOCOL      *mAcpiTable   = NULL;
UINT8                               mDfxPopulateBGDirectory = 0;
EFI_BOOT_MODE                       mBootMode;
volatile BOOLEAN                    mReadTrigParamMsr;
PCH_POLICY_HOB                      *mPchPolicyHob = NULL;

/**
  This function is triggered by the BIOS update tool with an IO trap. It executes
  BIOS Guard protocol execute with the true flag indicating that there is an update package
  in the DPR region of memory.

  @param DispatchHandle      - Not used
  @param CallbackContext     - Not used
  @param CommBuffer          - Not used
  @param CommBufferSize      - Not used
  
  @retval None.

**/
VOID
EFIAPI
BiosGuardUpdateBios (
  IN EFI_HANDLE                             DispatchHandle,
  IN CONST VOID                             *CallbackContext,
  IN OUT VOID                               *CommBuffer,
  IN OUT UINTN                              *CommBufferSize
  )
{
  ///
  /// Invoke BIOS Guard Services for updating BIOS
  ///
  mBiosGuardProtocol->Execute (mBiosGuardProtocol, TRUE);

  if (mBootMode == BOOT_ON_FLASH_UPDATE) {
    ///
    /// A complete BGUP is provided for Capsule and Tool updates. Therefore, the BGUP header should not 
    /// be overwritten in the event another erase or write operation occurs. The status from the IO Trap 
    /// is alternately placed at the BGUP Certificate address.
    ///
    CopyMem ((UINT64 *)mBgupCertificate, &mBiosGuardFullStatus, sizeof (UINT64));
  } else {
    ///
    /// Based on the interface defined for Tools implementation, the status from the IO Trap needs to be 
    /// placed at the address of the BGUP, essentially overwriting the header. 
    /// The BGUP Header, along with the Script, Data and BGUP Certificate are written by the tools into DPR memory, 
    /// therefore, we can safely overwrite the information in that address after execution as the next time tools does an 
    /// update operation, it will pass the complete package providing the proper BGUP Header
    ///
    CopyMem (mBiosGuardUpdatePackagePtr, &mBiosGuardFullStatus, sizeof (UINT64));
  }

  return;
}


/**
  This method registers and sets up the IOTRAP and NVS area for the BIOS Guard tools interface

  @param DispatchHandle      - Not used
  @param CallbackContext     - Not used
  @param CommBuffer          - Not used
  @param CommBufferSize      - Not used
  
  @retval None

**/
VOID
EFIAPI
BiosGuardToolsInterfaceInit (
// APTIOV_SERVER_OVERRIDE_RC_START : Changes for AMI BIOS GUARD
  VOID
#if 0
  IN EFI_HANDLE                             DispatchHandle,
  IN CONST VOID                             *CallbackContext,
  IN OUT VOID                               *CommBuffer,
  IN OUT UINTN                              *CommBufferSize
#endif
// APTIOV_SERVER_OVERRIDE_RC_END : Changes for AMI BIOS GUARD
  )
{
  EFI_STATUS                                 Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL               *GlobalNvsAreaProtocol;
  BIOS_ACPI_PARAM                            *GlobalNvsArea;
  EFI_HANDLE                                 PchIoTrapHandle;
  EFI_SMM_IO_TRAP_REGISTER_CONTEXT           PchIoTrapContext;

  ///
  /// Locate Global NVS and update BIOS Guard DPR size & Memory address for ACPI tables
  ///
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  GlobalNvsArea                          = GlobalNvsAreaProtocol->Area;
  GlobalNvsArea->BiosGuardMemAddress     = 0;
  GlobalNvsArea->BiosGuardMemSize        = 0;
  GlobalNvsArea->BiosGuardIoTrapAddress  = 0;
  
// APTIOV_SERVER_OVERRIDE_RC_START : Changes for AMI BIOS GUARD
  Status = gSmst->SmmLocateProtocol (&gEfiSmmIoTrapDispatch2ProtocolGuid, NULL, (VOID **) &mPchIoTrap);
  ASSERT_EFI_ERROR (Status);
// APTIOV_SERVER_OVERRIDE_RC_END : Changes for AMI BIOS GUARD
  
  ///
  /// Locate BIOS Guard SMM Protocol
  ///
  Status = gSmst->SmmLocateProtocol (&gSmmBiosGuardProtocolGuid, NULL, (VOID **) &mBiosGuardProtocol);
  ASSERT_EFI_ERROR (Status);
  if (mBiosGuardProtocol != NULL) {
    ///
    /// Register BIOS Guard IO TRAP handler
    ///
    PchIoTrapHandle               = NULL;
    PchIoTrapContext.Type         = ReadWriteTrap;
    PchIoTrapContext.Length       = 4;
    PchIoTrapContext.Address      = 0;
    Status = mPchIoTrap->Register (
                           mPchIoTrap,
                           (EFI_SMM_HANDLER_ENTRY_POINT2)BiosGuardUpdateBios,
                           &PchIoTrapContext,
                           &PchIoTrapHandle
                           );
    ASSERT_EFI_ERROR (Status);

    GlobalNvsArea->BiosGuardMemAddress     = mBiosGuardMemAddress;
    GlobalNvsArea->BiosGuardMemSize        = (UINT8) RShiftU64(mBiosGuardMemSize, 20);
    GlobalNvsArea->BiosGuardIoTrapAddress  = PchIoTrapContext.Address;
    // APTIOV_SERVER_OVERRIDE_RC_START : Changes for AMI BIOS GUARD
    GlobalNvsArea->BiosGuardIoTrapLength  = (UINT8)PchIoTrapContext.Length;
    // APTIOV_SERVER_OVERRIDE_RC_END : Changes for AMI BIOS GUARD
  }
}

/**

  Registers an IO Trap to be used to set up BIOS Guard Tools interface

  @param  Protocol   Points to the protocol's unique identifier.
  @param  Interface  Points to the interface instance.
  @param  Handle     The handle on which the interface was installed.

  @retval Status - IO Trap successfully registered or not

**/
EFI_STATUS
BiosGuardRegisterToolsIoTrap (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              PchIoTrapHandle;
  EFI_SMM_IO_TRAP_REGISTER_CONTEXT        PchIoTrapContext;

  Status = gSmst->SmmLocateProtocol (&gEfiSmmIoTrapDispatch2ProtocolGuid, NULL, &mPchIoTrap);

  PchIoTrapHandle               = NULL;
  PchIoTrapContext.Type         = ReadTrap;
  PchIoTrapContext.Length       = 4;
  PchIoTrapContext.Address      = 0;
  Status = mPchIoTrap->Register (
                        mPchIoTrap,
                        (EFI_SMM_HANDLER_ENTRY_POINT2)BiosGuardToolsInterfaceInit,
                        &PchIoTrapContext,
                        &PchIoTrapHandle
                        );
  ASSERT_EFI_ERROR (Status);

  mBiosGuardHobPtr->BiosGuardToolsIntIoTrapAdd = (UINT64) PchIoTrapContext.Address;

  return Status;
}

/**

  Entry point for the BIOS Guard protocol driver.

  @param[in] ImageHandle        Image handle of this driver.
  @param[in] SystemTable        Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_ERROR             Driver exits abnormally.
**/
EFI_STATUS
EFIAPI
InstallBiosGuardProtocol (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE      *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_BASE2_PROTOCOL                    *SmmBase;
  BIOSGUARD_INSTANCE                        *BiosGuardInstance;
  VOID                                      *BiosGuardProtocolAddr;
// APTIOV_SERVER_OVERRIDE_RC_START : Changes for AMI BIOS GUARD
#if 0
  EFI_HANDLE                                PchIoTrapHandle;
  EFI_SMM_IO_TRAP_REGISTER_CONTEXT          PchIoTrapContext;
  VOID                                      *Registration;
#endif
  BOOLEAN                                   BiosGuardToolsInterface;
// APTIOV_SERVER_OVERRIDE_RC_END : Changes for AMI BIOS GUARD

  ///
  /// Locate SMM Base Protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, &SmmBase);
  ASSERT_EFI_ERROR (Status);
  // APTIOV_SERVER_OVERRIDE_RC_START : Changes for AMI BIOS GUARD
  BiosGuardToolsInterface  = FALSE;
  // APTIOV_SERVER_OVERRIDE_RC_END : Changes for AMI BIOS GUARD

  ///
  /// Initialize our module variables
  ///
  Status = SmmBase->GetSmstLocation (SmmBase, &gSmst);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate pool for BIOS Guard protocol instance
  ///
  Status = gSmst->SmmAllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (BIOSGUARD_INSTANCE),
                  (VOID **) &BiosGuardInstance
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (BiosGuardInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *) BiosGuardInstance, sizeof (BIOSGUARD_INSTANCE));
  BiosGuardInstance->Handle = NULL;
  BiosGuardProtocolAddr     = NULL;

  if ((AsmReadMsr64 (EFI_PLATFORM_INFORMATION)) & B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL) {
    if ((AsmReadMsr64 (MSR_PLAT_FRMW_PROT_CTRL)) & B_MSR_PLAT_FRMW_PROT_CTRL_EN) {
      mBootMode = GetBootModeHob ();

      ///
      /// Initialize the BIOS Guard protocol instance
      ///
      Status = BiosGuardProtocolConstructor (BiosGuardInstance);
      if (!EFI_ERROR (Status)) {
        BiosGuardProtocolAddr = &(BiosGuardInstance->BiosGuardProtocol);
      }
      ///
      /// Initialize tools support only if we are in Legacy flow
      ///
      if (mBiosGuardUpdatePackageInTseg == FALSE) {
        // APTIOV_SERVER_OVERRIDE_RC_START : Changes for AMI BIOS GUARD
        BiosGuardToolsInterface = TRUE;
#if 0
        Status = gSmst->SmmLocateProtocol (&gEfiSmmIoTrapDispatch2ProtocolGuid, NULL, &mPchIoTrap);
        if(EFI_ERROR(Status)) {
          //
          // Smm IO Trap Dispatch not installed yet, install a notify for when it is available
          //
          Registration = NULL;
          Status = gSmst->SmmRegisterProtocolNotify (
                     &gEfiSmmIoTrapDispatch2ProtocolGuid,
                     BiosGuardRegisterToolsIoTrap,
                     &Registration
                     );
        } else {
          //
          // Register IO Trap for the Tools interface init
          //
          PchIoTrapHandle               = NULL;
          PchIoTrapContext.Type         = ReadTrap;
          PchIoTrapContext.Length       = 4;
          PchIoTrapContext.Address      = 0;
          Status = mPchIoTrap->Register (
                               mPchIoTrap,
                               (EFI_SMM_HANDLER_ENTRY_POINT2)BiosGuardToolsInterfaceInit,
                               &PchIoTrapContext,
                               &PchIoTrapHandle
                               );
          ASSERT_EFI_ERROR (Status);

          mBiosGuardHobPtr->BiosGuardToolsIntIoTrapAdd = PchIoTrapContext.Address;
        }
#endif
        // APTIOV_SERVER_OVERRIDE_RC_END : Changes for AMI BIOS GUARD
      }

      Status = gSmst->SmmLocateProtocol(
                  &gEfiSmmCpuServiceProtocolGuid,
                  NULL,
                  &mSmmCpuService
                  );
      ASSERT_EFI_ERROR(Status);

    } else {
      DEBUG ((EFI_D_INFO, "BIOS Guard Feature supported but disabled\n"));
    }
  } else {
    DEBUG ((EFI_D_WARN, "BIOS Guard Feature is not supported\n"));
  }

  ///
  /// Install the SMM BIOSGUARD_PROTOCOL interface
  ///
  Status = gSmst->SmmInstallProtocolInterface (
                  &(BiosGuardInstance->Handle),
                  &gSmmBiosGuardProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  BiosGuardProtocolAddr
                  );
  if (EFI_ERROR (Status)) {
    gSmst->SmmFreePool (BiosGuardInstance);
  // APTIOV_SERVER_OVERRIDE_RC_START : Changes for AMI BIOS GUARD
  } else {
    if (BiosGuardToolsInterface == TRUE) {
      BiosGuardToolsInterfaceInit();
    }
  // APTIOV_SERVER_OVERRIDE_RC_END : Changes for AMI BIOS GUARD
  }

  return Status;
}

/**
  Initialize BIOS Guard protocol instance.

  @param[in] BiosGuardInstance  Pointer to BiosGuardInstance to initialize

  @retval EFI_SUCCESS           The protocol instance was properly initialized
  @retval EFI_NOT_FOUND         BIOS Guard Binary module was not found.
  @retval EFI_OUT_OF_RESOURCES  Allocated memory could not be freed.
**/
EFI_STATUS
BiosGuardProtocolConstructor (
  BIOSGUARD_INSTANCE *BiosGuardInstance
  )
{
  EFI_STATUS                    Status;
  BGPDT                         *Bgpdt;
// APTIOV_SERVER_OVERRIDE_RC_START 
// Remove reading BIOS Guard Module from FV
#if 0
  UINTN                         i;
  UINTN                         NumHandles;
  EFI_HANDLE                    *Buffer;
  UINTN                         Size;
  UINT32                        FvStatus;
  EFI_FV_FILETYPE               FileType;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FwVol;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
#endif
  UINT32                        BiosGuardModuleSize;  //Changes for geting info from Hob.
  // Replace "Setup" with "IntelSetup
  UINTN                         VariableSize;
  SYSTEM_CONFIGURATION          SetupData;
  EFI_SMM_VARIABLE_PROTOCOL     *mSmmVariable;
// APTIOV_SERVER_OVERRIDE_RC_END
  UINTN                         NumPages;
  EFI_PHYSICAL_ADDRESS          Addr;
  EFI_PHYSICAL_ADDRESS          BiosGuardModule;
  EFI_HOB_GUID_TYPE             *GuidHob;
  EFI_PLATFORM_INFO             *PlatformInfoHobPtr;
  // APTIOV_SERVER_OVERRIDE_RC_START
//  EFI_GUID                      BiosGuardModuleGuid  = gBiosGuardModuleGuid;
  // APTIOV_SERVER_OVERRIDE_RC_END
  EFI_GUID                      BiosGuardHobGuid     = gBiosGuardHobGuid;

  mBiosGuardUpdatePackageInTseg = FALSE;

// APTIOV_SERVER_OVERRIDE_RC_START
  #if 0
  Status = GetOptionData(&gEfiSetupVariableGuid, OFFSET_OF (SYSTEM_CONFIGURATION, DfxPopulateBGDirectory), &mDfxPopulateBGDirectory, sizeof(mDfxPopulateBGDirectory));
  ASSERT_EFI_ERROR (Status);
  #endif
  Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmVariableProtocolGuid,
                      NULL,
                      &mSmmVariable
                      );
  ASSERT_EFI_ERROR (Status);

  VariableSize = sizeof(SYSTEM_CONFIGURATION);
  Status = mSmmVariable->SmmGetVariable (
                            L"IntelSetup",
                            &gEfiSetupVariableGuid,
                            NULL,
                            &VariableSize,
                            &SetupData
                            );
  ASSERT_EFI_ERROR(Status);
  
  mDfxPopulateBGDirectory = SetupData.DfxPopulateBGDirectory;
// Remove reading BIOS Guard Module from FV
  BiosGuardModuleSize      = 0;
//  FwVol           = NULL;
//  FvStatus        = 0;
  // APTIOV_SERVER_OVERRIDE_RC_END
  NumPages        = 0;
  Addr            = 0;
  // APTIOV_SERVER_OVERRIDE_RC_START
//  Size            = 0;
  // APTIOV_SERVER_OVERRIDE_RC_END
  DEBUG ((EFI_D_INFO, "BiosGuardServices Init Start\n"));
// APTIOV_SERVER_OVERRIDE_RC_START
// Remove reading BIOS Guard Module from FV
#if 0
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &NumHandles,
                  &Buffer
                  );
  ASSERT_EFI_ERROR (Status);

  for (i = 0; i < NumHandles; i++) {
    Status = gBS->HandleProtocol (
                    Buffer[i],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);

    ///
    /// Locate BIOS Guard Binary.
    ///
    Status = FwVol->ReadFile (
                    FwVol,
                    &BiosGuardModuleGuid,
                    NULL,
                    &Size,
                    &FileType,
                    &Attributes,
                    &FvStatus
                    );
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  FreePool (Buffer);
  ASSERT (Size);
  if ((FwVol == NULL) || (Size == 0)) {
    return EFI_NOT_FOUND;
  }
#endif
// APTIOV_SERVER_OVERRIDE_RC_END
  NumPages = BIOSGUARD_MEMORY_PAGES + ALIGNMENT_IN_PAGES;

// APTIOV_SERVER_OVERRIDE_RC_START : Changes for geting info from Hob.
  mBiosGuardHobPtr = GetFirstGuidHob (&BiosGuardHobGuid);
  if (mBiosGuardHobPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "BIOS Guard HOB not available\n"));
    return EFI_NOT_FOUND;
  }

  if (mBiosGuardHobPtr->BiosGuardModulePtr == (EFI_PHYSICAL_ADDRESS) NULL) {
    return EFI_NOT_FOUND;
  }

  BiosGuardModuleSize  = *(UINT16*)(mBiosGuardHobPtr->BiosGuardModulePtr + BIOSGUARD_MODULE_SIZE_OFFSET);
  DEBUG ((DEBUG_ERROR, "BIOS Guard Module Size: %x\n", BiosGuardModuleSize));
  if (BiosGuardModuleSize == 0) {
    return EFI_NOT_FOUND;
  }
// APTIOV_SERVER_OVERRIDE_RC_END : Changes for geting info from Hob.

  ///
  /// Allocate memory buffer for BIOS Guard Module
  ///
  Status = (gSmst->SmmAllocatePages)(AllocateAnyPages, EfiRuntimeServicesData, NumPages, &Addr);
  if (EFI_ERROR(Status) || Addr == (EFI_PHYSICAL_ADDRESS) NULL) {
    DEBUG ((DEBUG_ERROR, "Allocation of buffer for BIOS Guard Module failed.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  ///
  /// Align address to 256K.
  ///
  BiosGuardModule  = Addr &~(ALIGN_256KB - 1);
  BiosGuardModule  = BiosGuardModule < Addr ? (BiosGuardModule + ALIGN_256KB) : BiosGuardModule;

// APTIOV_SERVER_OVERRIDE_RC_START
  // Remove reading BIOS Guard Module from FV
  ///
  /// Read BIOS Guard Module into prepared buffer.
  ///
#if 0
  Status = FwVol->ReadFile (
                  FwVol,
                  &BiosGuardModuleGuid,
                  ((VOID **) &BiosGuardModule),
                  &Size,
                  &FileType,
                  &Attributes,
                  &FvStatus
                  );
  ASSERT (Size);
  if ((FwVol == NULL) || (Size == 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Freeform type Binary integration adds 4 bytes of data at the beginning of binary which needs to be tuncated
  // to align binary to the requirement
  //
  CopyMem((UINT32*)BiosGuardModule, (UINT32 *)((UINT8*)BiosGuardModule + 4), Size);

  mBiosGuardHobPtr = GetFirstGuidHob (&BiosGuardHobGuid);
  if (mBiosGuardHobPtr == NULL) {
    DEBUG ((EFI_D_ERROR, "BIOS Guard HOB not available\n"));
    return EFI_NOT_FOUND;
  }
#endif
// Changes for geting info from Hob.
  ///
  /// Checking if the size of pages to free is not zero
  ///
  if (Addr != BiosGuardModule) {
    ///
    /// Free all allocated pages till start of the aligned memory address
    ///
    Status = (gSmst->SmmFreePages) (
                       Addr,
                       EFI_SIZE_TO_PAGES (BiosGuardModule - Addr)
                       );

    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Freeing of allocated pages till start of the aligned memory address failed.\n"));
      return EFI_OUT_OF_RESOURCES;
    }
  }

  ///
  /// Checking if the size of pages to free is not zero
  ///
  if (((Addr + (ALIGN_256KB * 2)) - (BiosGuardModule + ALIGN_256KB)) != 0) {
    ///
    /// Free all allocated pages after the end of the aligned memory address
    ///
    Status = (gSmst->SmmFreePages) (
                       BiosGuardModule + ALIGN_256KB,
                       EFI_SIZE_TO_PAGES ((Addr + (ALIGN_256KB * 2)) - (BiosGuardModule + ALIGN_256KB))
                       );
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Freeing of allocated pages after the end of the aligned memory address failed.\n"));
      return EFI_OUT_OF_RESOURCES;
    }
  }

  ///
  /// Copy BIOS Guard Module into prepared buffer.
  ///
  CopyMem((VOID*)BiosGuardModule,(VOID*)mBiosGuardHobPtr->BiosGuardModulePtr,BiosGuardModuleSize);
// APTIOV_SERVER_OVERRIDE_RC_END

  GuidHob = GetFirstGuidHob (&gEfiPlatformInfoGuid);
  if (GuidHob == NULL) {
    DEBUG ((EFI_D_ERROR, "Platform Info HOB not available\n"));
    return EFI_NOT_FOUND;
  }
  PlatformInfoHobPtr = GET_GUID_HOB_DATA (GuidHob);

  ///
  /// Allocate pool for BGPDT Data
  ///
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    mBiosGuardHobPtr->Bgpdt.BgpdtSize,
                    (VOID **) &Bgpdt
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Bgpdt, &mBiosGuardHobPtr->Bgpdt, mBiosGuardHobPtr->Bgpdt.BgpdtSize);

  ///
  /// Determine if BGUP is to be placed in TSEG
  /// If the size allocated to BIOS Guard in DPR is 0, BGUP will be stored in TSEG
  /// Otherwise, BGUP will use the memory allocated within DPR
  ///
  if (PlatformInfoHobPtr->MemData.BiosGuardMemSize == 0) {
    mBiosGuardUpdatePackageInTseg = TRUE;
  }

  ///
  /// Legacy runtime flow or Capsule Update based flow, allocate BIOS Guard variables in DPR
  ///
  if (mBiosGuardUpdatePackageInTseg == FALSE) {
    ///
    /// First, initialize the BIOS Guard memory address in the pre-allocated space within DPR
    ///
    mBiosGuardMemAddress        = PlatformInfoHobPtr->MemData.BiosGuardPhysBase;
    mBiosGuardMemSize           = (UINT32) LShiftU64(PlatformInfoHobPtr->MemData.BiosGuardMemSize, 20);

    ///
    /// The BGUP uses the majority of the space within the allocated region and uses the initial block of memory
    ///
    mBiosGuardUpdatePackagePtr  = (BGUP *) mBiosGuardMemAddress;

    ///
    /// A subset of the memory is allotted for the space required for the BIOS Guard certificate after the BGUP
    ///
    mBgupCertificate            = (EFI_PHYSICAL_ADDRESS) (mBiosGuardMemAddress + mBiosGuardMemSize - BGUPC_MEMORY_OFFSET);

    ///
    /// A final allocation is made for the BIOS Guard Log at the end of the buffer after the certificate
    ///
    mBiosGuardLogPtr            = (BIOSGUARD_LOG *) (mBiosGuardMemAddress + mBiosGuardMemSize - BIOSGUARD_LOG_MEMORY_OFFSET);
  } else {
    ///
    /// Non-BIOS update flow, allocate BGUP pointer within TSEG
    ///
    mBiosGuardMemSize = BGUP_TSEG_BUFFER_SIZE;
    NumPages = mBiosGuardMemSize / EFI_PAGE_SIZE;
    Status = (gSmst->SmmAllocatePages) (
                  AllocateAnyPages, 
                  EfiRuntimeServicesData, 
                  NumPages,
                  &mBiosGuardMemAddress
                  );
    if (Status != EFI_SUCCESS) {
      DEBUG ((DEBUG_ERROR, "Memory for BGUP not allocated in SMM!\n"));
      ASSERT_EFI_ERROR (Status);
      return Status;
    }

    ///
    /// During runtime, the TSEG memory is only allocated for BGUP
    ///
    mBiosGuardUpdatePackagePtr  = (BGUP *) mBiosGuardMemAddress;

    ///
    /// Initialize the Certificate memory and Log memory to NULL as it isn't availble in the standard path
    ///
    mBgupCertificate = (EFI_PHYSICAL_ADDRESS) NULL;
    mBiosGuardLogPtr = NULL;
  }

  ///
  /// Save the Log & BGUP headers into a temporary location so it can be used for re-initialization of the log between BiosGuardProtocolExecute calls
  ///
  CopyMem (&mBiosGuardLogTemp, &mBiosGuardHobPtr->BiosGuardLog, sizeof (BIOSGUARD_LOG));
  CopyMem (&mBiosGuardBgupHeaderTemp, &mBiosGuardHobPtr->BgupHeader, sizeof (BGUP_HEADER));

  ///
  /// Initialize the BIOS Guard Update Package with the package header and zero out the rest of the buffer
  ///
  ZeroMem(mBiosGuardUpdatePackagePtr, mBiosGuardMemSize);
  CopyMem (&mBiosGuardUpdatePackagePtr->BgupHeader, &mBiosGuardHobPtr->BgupHeader, sizeof (BGUP_HEADER));
  mBiosGuardUpdatePackagePtr->BgupHeader.ScriptSectionSize  = 0;
  mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize    = 0;
  mBiosGuardUpdateCounter = 0;

  ///
  /// Set Begin command
  ///
  mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++] = BIOSGUARD_COMMAND_BEGIN;

  ///
  /// Initialize the BIOS Guard protocol instance
  ///
  BiosGuardInstance->Signature                  = BIOSGUARD_SIGNATURE;
  BiosGuardInstance->BiosGuardProtocol.Write    = BiosGuardProtocolWrite;
  BiosGuardInstance->BiosGuardProtocol.Erase    = BiosGuardProtocolBlockErase;
  BiosGuardInstance->BiosGuardProtocol.Execute  = BiosGuardProtocolExecute;
  BiosGuardInstance->BiosOffset                 = (((UINT32) LShiftU64(mBiosGuardHobPtr->TotalFlashSize, 10)) - ((UINT32) LShiftU64(mBiosGuardHobPtr->BiosSize, 10)));

  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardModule]        = BiosGuardModule;
  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardModule]       |= LShiftU64 (BIOSGUARD_DIRECTORY_BIOSGUARD_MODULE_ENTRY, 56);
  BiosGuardInstance->BiosGuardDirectory[EnumBgpdt]                  = (EFI_PHYSICAL_ADDRESS) Bgpdt;
  BiosGuardInstance->BiosGuardDirectory[EnumBgpdt]                 |= LShiftU64 (BIOSGUARD_DIRECTORY_BGPDT_ENTRY, 56);
  BiosGuardInstance->BiosGuardDirectory[EnumBgup]                   = (EFI_PHYSICAL_ADDRESS) mBiosGuardUpdatePackagePtr;
  BiosGuardInstance->BiosGuardDirectory[EnumBgup]                  |= LShiftU64(BIOSGUARD_DIRECTORY_BGUP_ENTRY, 56);
  BiosGuardInstance->BiosGuardDirectory[EnumBgupCertificate]        = 0;
  BiosGuardInstance->BiosGuardDirectory[EnumBgupCertificate]       |= LShiftU64(BIOSGUARD_DIRECTORY_UNDEFINED_ENTRY, 56);
  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardLog]           = 0;
  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardLog]          |= LShiftU64(BIOSGUARD_DIRECTORY_UNDEFINED_ENTRY, 56);
  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardDirectoryEnd]  = 0;
  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardDirectoryEnd] |= LShiftU64 (BIOSGUARD_DIRECTORY_END_MARKER, 56);

  GuidHob = GetFirstGuidHob(&gPchPolicyHobGuid);
  if (GuidHob != NULL) {
    mPchPolicyHob = GET_GUID_HOB_DATA(GuidHob);
  }

  return EFI_SUCCESS;
}

/**
  Set MSR 0x115 with BIOS Guard DIRECTORY Address.
  Trigger MSR 0x116 to invoke BIOS Guard Binary.
  Read MSR 0x115 to get BIOS Guard Binary Status.

  @param[in] BiosGuardInstance              Pointer to BiosGuardInstance to use
**/
VOID
EFIAPI
BiosGuardModuleExecute (
  IN VOID          *BiosGuardInstancePtr
  )
{
  BIOSGUARD_INSTANCE *BiosGuardInstance;
  BiosGuardInstance = BiosGuardInstancePtr;

  AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_TRIG_PARAM, (UINT64) BiosGuardInstance->BiosGuardDirectory);
  AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_TRIGGER, 0);

  return;
}

/**

  Reads MSR 0x115 and sets it to the Bios Guard instance for module execution status.

  @param BiosGuardInstance        - Pointer to BiosGuardInstance to use
  
  @retval None

**/
VOID
ReadTrigParamMsr (
  IN BIOSGUARD_INSTANCE          *BiosGuardInstance
  )
{
  BiosGuardInstance->MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_TRIG_PARAM);
  mReadTrigParamMsr = TRUE;
  return ;
}


/**

  Set MSR 0x115 with BIOS Guard DIRECTORY Address.

  @param BiosGuardInstance        - Pointer to BiosGuardInstance to initialize
  
  @retval None

**/
VOID
SetTrigParamToBiosGuardDirectory (
  IN BIOSGUARD_INSTANCE          *BiosGuardInstance
  )
{
  AsmWriteMsr64 (MSR_PLAT_FRMW_PROT_TRIG_PARAM, (UINT64) BiosGuardInstance->BiosGuardDirectory);
  return ;
}

/**
  This service will write BIOSGUARD_DIRECTORY MSR and invoke the BIOS Guard Module by writing to PLAT_FRMW_PROT_TRIGGER MSR for writing/erasing to flash.
  BIOS should invoke BIOSGUARD_PROTOCOL.Write() or BIOSGUARD_PROTOCOL.Erase() function prior to calling BIOSGUARD_PROTOCOL.Execute() for flash writes/erases (except for IoTrapBasedRequest).
  Write()/Erase() function will render BIOS Guard script during execution.
  Execute() function will implement the following steps:
  1. Update BIOS Guard directory with address of BGUP.
  2. All the AP's except the master thread are put to sleep.
  3. BIOS Guard module is invoked from BSP to execute desired operation.
  If IoTrapBasedRequest flag is set to true, BGUP (BGUP Header + BIOS Guard Script + Update data) is part of data that is passed to SMI Handler. SMI Handler invokes BIOS Guard module to process the update.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This                Pointer to the BIOSGUARD_PROTOCOL instance.
  @param[in] IoTrapBasedRequest  Flag to indicate flash update is requested from OS runtime via IO Trap (i.e. a Tool)

  @retval EFI_SUCCESS            Successfully completed flash operation.
  @retval EFI_INVALID_PARAMETER  The parameters specified are not valid.
  @retval EFI_UNSUPPORTED        The CPU or SPI memory is not supported.
  @retval EFI_DEVICE_ERROR       Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
BiosGuardProtocolExecute (
  IN BIOSGUARD_PROTOCOL *This,
  IN BOOLEAN            IoTrapBasedRequest
  )
{
  EFI_STATUS                  Status;
  BIOSGUARD_INSTANCE          *BiosGuardInstance;
  UINT16                      BiosGuardStatus;
  UINT16                      BiosGuardAdditionalData;
  UINT16                      BiosGuardTerminalLine;
  UINT8                       BiosGuardSE;
  UINTN                       Index;
  UINT8                       RetryIteration;
  BOOLEAN                     DisallowedUpdate;
  EFI_PROCESSOR_INFORMATION   ProcessorInfo;
  UINTN                       LegacySocketIndex;
  BOOLEAN                     BspIsLegacySocket;
  UINTN                       SpiBaseAddress;

  ///
  /// Initialize local variables
  ///
  BiosGuardStatus         = ERR_OK;
  BiosGuardAdditionalData = ERR_OK;
  BiosGuardTerminalLine   = ERR_OK;
  BiosGuardSE             = ERR_OK;
  DisallowedUpdate        = FALSE;
  Status                  = EFI_SUCCESS;
  BspIsLegacySocket       = TRUE;
  mReadTrigParamMsr       = FALSE;
  LegacySocketIndex       = 0;

  //DEBUG ((DEBUG_INFO, "BiosGuardProtocolExecute\n"));  // Only needed for debug

  BiosGuardInstance = BIOSGUARD_INSTANCE_FROM_BIOSGUARDPROTOCOL (This);

  ///
  /// Prior to execution of the BIOS Guard module, reinitialize the BIOS Guard Log area & BIOS Guard Binary Return Status
  ///
  if (mBiosGuardLogPtr != NULL) {
    ZeroMem(mBiosGuardLogPtr, BIOSGUARD_LOG_MEMORY_SIZE);
    CopyMem(mBiosGuardLogPtr, &mBiosGuardLogTemp, sizeof(BIOSGUARD_LOG));
  }
  BiosGuardInstance->MsrValue = ERR_LAUNCH_FAIL;

  if (IoTrapBasedRequest == FALSE) {
    ///
    /// If Update Package has been created by the BIOS during POST then complete the script
    /// and create update Pkg
    ///

    ///
    /// First, finalize the script by adding the "End" command
    ///
    mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++]  = BIOSGUARD_COMMAND_END;
    mBiosGuardUpdatePackagePtr->BgupHeader.ScriptSectionSize           = (mBiosGuardUpdateCounter * 8);

    ///
    /// Copy the BIOS Guard Update Data member variable into the BGUP buffer directly after the "End" command
    ///
    CopyMem (
            &mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter],
            &mBiosGuardUpdateData,
            (mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize)
            );
  } else {
    ///
    /// If the Update Package was retrieved from the OS via Global NVS and IO Trap then require it to be signed
    /// Update Bgup Certificate pointer to dynamically point to certificate based on image size
    ///
    // APTIOV_SERVER_OVERRIDE_RC_START : Do not need update BGUPC address for AMI BGT.
    //mBgupCertificate = (EFI_PHYSICAL_ADDRESS)(mBiosGuardMemAddress + sizeof(BGUP_HEADER) + mBiosGuardUpdatePackagePtr->BgupHeader.ScriptSectionSize + mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize);
    // APTIOV_SERVER_OVERRIDE_RC_END : Do not need update BGUPC address for AMI BGT.
    if (mBiosGuardUpdatePackagePtr->BgupHeader.PkgAttributes) {
      BiosGuardInstance->BiosGuardDirectory[EnumBgupCertificate]  = mBgupCertificate;
      BiosGuardInstance->BiosGuardDirectory[EnumBgupCertificate] |= LShiftU64(BIOSGUARD_DIRECTORY_BGUP_CERTIFICATE_ENTRY, 56);
      BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardLog]     = (EFI_PHYSICAL_ADDRESS) mBiosGuardLogPtr;
      BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardLog]    |= LShiftU64(BIOSGUARD_DIRECTORY_BIOSGUARD_LOG_ENTRY, 56);
    } else {
      /// BIOS Updates will not be allowed to be passed through when there is no certificate required
      DisallowedUpdate = TRUE;
    }
  }

  if (!DisallowedUpdate) {

    Status = mSmmCpuService->GetProcessorInfo(mSmmCpuService, gSmst->CurrentlyExecutingCpu, &ProcessorInfo);
    ASSERT_EFI_ERROR(Status);

    //
    // Find out if the Smm BSP is on the legacy socket.  If it is not, then we need to locate
    // a CPU Index that is on the legacy socket
    //
    if (ProcessorInfo.Location.Package != 0) {
      BspIsLegacySocket = FALSE;
      for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
        mSmmCpuService->GetProcessorInfo(mSmmCpuService, Index, &ProcessorInfo);
        if (ProcessorInfo.Location.Package == 0) {
          LegacySocketIndex = Index;
          break;
        }
      }
    }

    ///
    /// Prior to launching the BIOS Guard Binary on the BSP, each of the APs must launch the BIOS Guard binary so that
    /// they can be accounted for and placed in sleep by the BIOS Guard binary.  Cannot assume Index 0 is SMM BSP.
    ///
    for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
      if (Index != gSmst->CurrentlyExecutingCpu) {
        Status = EFI_NOT_READY;
        for (RetryIteration = 0; (RetryIteration < BIOSGUARD_AP_SAFE_RETRY_LIMIT) && (Status != EFI_SUCCESS); RetryIteration++) {
          Status = gSmst->SmmStartupThisAp (BiosGuardModuleExecute, Index, (VOID *) BiosGuardInstance);
          if (Status != EFI_SUCCESS) {
            ///
            /// SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
            ///
            MicroSecondDelay (BIOSGUARD_WAIT_PERIOD);
          }
        }
      }
    }
    SpiBaseAddress = MmPciBase(
            DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_SPI,
            PCI_FUNCTION_NUMBER_PCH_SPI
            );
    //
    // Write clear BC_SYNC_SS prior trigger MSR_PLAT_FRMW_PROT_TRIGGER.
    //
    if (mPchPolicyHob != NULL) {
      if (mPchPolicyHob->LockDownConfig.BiosLock == TRUE) {
        MmioOr16(SpiBaseAddress + R_PCH_SPI_BC, B_PCH_SPI_BC_SYNC_SS);
      }
    }
    BiosGuardModuleExecute((VOID *)BiosGuardInstance);

    //
    // Read MSR_PLAT_FRMW_PROT_TRIG_PARAM to get BIOS Guard Binary status
    //
    if (BspIsLegacySocket) {
      BiosGuardInstance->MsrValue = AsmReadMsr64 (MSR_PLAT_FRMW_PROT_TRIG_PARAM);
    } else {
      //
      // If we executed on non-legacy socket, we must read status from legacy socket MSR
      // This may need a couple tries if the thread is not ready yet following BiG execution
      //
      for (Index = 0; Index < 100; Index++) {
        Status = gSmst->SmmStartupThisAp (ReadTrigParamMsr, LegacySocketIndex, (VOID *)BiosGuardInstance);
        if (EFI_ERROR(Status)) {
          MicroSecondDelay (10);
        } else {
          //
          // Wait for Legacy Socket thread to signal it read Trig Param MSR
          //
          while(mReadTrigParamMsr == FALSE);
          break;
        }
      }
    }

    BiosGuardStatus = (UINT16)RShiftU64(
      (BiosGuardInstance->MsrValue & LShiftU64(V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_MASK,
      N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_OFFSET)),
      N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_STATUS_OFFSET
      );

    switch (BiosGuardStatus) {
      case ERR_OK:
        Status = EFI_SUCCESS;
        break;

      case ERR_RANGE_VIOLATION:
      case ERR_SFAM_VIOLATION:
      case ERR_EXEC_LIMIT:
      case ERR_INTERNAL_ERROR:
        Status = EFI_DEVICE_ERROR;
        break;

      case ERR_UNSUPPORTED_CPU:
      case ERR_UNDEFINED_FLASH_OBJECT:
      case ERR_LAUNCH_FAIL:
        Status = EFI_UNSUPPORTED;
        break;

      default:

        case ERR_BAD_DIRECTORY:
        case ERR_BAD_BGPDT:
        case ERR_BAD_BGUP:
        case ERR_SCRIPT_SYNTAX:
        case ERR_INVALID_LINE:
        case ERR_BAD_BGUPC:
        case ERR_BAD_SVN:
        case ERR_UNEXPECTED_OPCODE:
        case ERR_OVERFLOW:
          Status = EFI_INVALID_PARAMETER;
    }
  } else {
    Status = EFI_UNSUPPORTED;
  }

  if (EFI_ERROR (Status)) {
    BiosGuardAdditionalData = (UINT16) RShiftU64 (
                    (BiosGuardInstance->MsrValue & LShiftU64 (V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_MASK,
                                                          N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_OFFSET)),
                    N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_DATA_OFFSET
                    );
    BiosGuardTerminalLine = (UINT16) RShiftU64 (
                    (BiosGuardInstance->MsrValue & LShiftU64 (V_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_MASK,
                                                          N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_OFFSET)),
                    N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_TERMINAL_OFFSET
                    );
    BiosGuardSE = (UINT8) RShiftU64 (
                    (BiosGuardInstance->MsrValue & B_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE),
                    N_MSR_PLAT_FRMW_PROT_TRIG_PARAM_SE_OFFSET
                    );
    DEBUG ((EFI_D_ERROR, "BIOS Guard Status          = 0x%X\n", BiosGuardStatus));
    DEBUG ((EFI_D_ERROR, "BIOS Guard Additional Data = 0x%X\n", BiosGuardAdditionalData));
    DEBUG ((EFI_D_ERROR, "BIOS Guard Terminal Line   = 0x%X\n", BiosGuardTerminalLine));
    DEBUG ((EFI_D_ERROR, "BIOS Guard SE              = 0x%X\n", BiosGuardSE));
    DEBUG ((EFI_D_ERROR, "BIOS Guard Disallowed Upd  = %s\n", (DisallowedUpdate)?L"TRUE":L"FALSE"));
  }

  mBiosGuardFullStatus = BiosGuardInstance->MsrValue;

  BiosGuardInstance->BiosGuardDirectory[EnumBgupCertificate] = 0;
  BiosGuardInstance->BiosGuardDirectory[EnumBgupCertificate] |= LShiftU64(BIOSGUARD_DIRECTORY_UNDEFINED_ENTRY, 56);
  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardLog]    = 0;
  BiosGuardInstance->BiosGuardDirectory[EnumBiosGuardLog]    |= LShiftU64(BIOSGUARD_DIRECTORY_UNDEFINED_ENTRY, 56);
  ZeroMem(mBiosGuardUpdatePackagePtr, mBiosGuardMemSize);
  CopyMem (&mBiosGuardUpdatePackagePtr->BgupHeader, &mBiosGuardBgupHeaderTemp, sizeof (BGUP_HEADER));
  mBiosGuardUpdatePackagePtr->BgupHeader.ScriptSectionSize    = 0;
  mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize      = 0;
  mBiosGuardUpdateCounter = 0;

  ///
  /// Prep for the next script execution by adding the Begin command
  ///
  mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++] = BIOSGUARD_COMMAND_BEGIN;

  if (mDfxPopulateBGDirectory && !EFI_ERROR(Status)){

    //
    // Set Script Size to be the minimally allowed size
    //
    mBiosGuardUpdatePackagePtr->BgupHeader.ScriptSectionSize = 0x10;

    //
    // Write default BEGIN and END opcodes to script location
    //
    mBiosGuardUpdatePackagePtr->BgupBuffer[0] = 0x01;
    mBiosGuardUpdatePackagePtr->BgupBuffer[1] = 0xFF;

    //
    // Leave Trig Param MSR with the BIOS Guard Directory value if successful- MSR 0x115 is per package
    //
    if (BspIsLegacySocket) {
      SetTrigParamToBiosGuardDirectory (BiosGuardInstance);
    } else {
      gSmst->SmmStartupThisAp (SetTrigParamToBiosGuardDirectory, LegacySocketIndex, BiosGuardInstance);
    }
  }

  return Status;
}

/**
  This service fills BIOS Guard script buffer for flash writes.
  BIOS should invoke this function prior to calling BIOSGUARD_PROTOCOL.Execute() with all the relevant data required for flash write.
  This function will not invoke BIOS Guard Module, only create script required for writing to flash.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This           Pointer to the BIOSGUARD_PROTOCOL instance.
  @param[in] Offset         This value specifies the offset from the start of the SPI Flash component where BIOS Image is located.
  @param[in] DataByteCount  Number of bytes in the data portion.
  @param[in] Buffer         Pointer to caller-allocated buffer containing the data sent.
**/
VOID
EFIAPI
BiosGuardProtocolWrite (
  IN BIOSGUARD_PROTOCOL *This,
  IN UINTN              Offset,
  IN UINT32             DataByteCount,
  IN OUT UINT8          *Buffer
  )
{
  BIOSGUARD_INSTANCE *BiosGuardInstance;

  BiosGuardInstance = BIOSGUARD_INSTANCE_FROM_BIOSGUARDPROTOCOL (This);

  ///
  /// Set Buffer Offset Index immediate command
  ///
  mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++] =
          (LShiftU64 ((UINTN) mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize, 32)) |
          (LShiftU64 (BIOSGUARD_B0_INDEX, 16)) |
          BIOSGUARD_COMMAND_SET_BUFFER_INDEX;
  ///
  /// Set Flash Index immediate command
  ///
  mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++] = (LShiftU64 ((BiosGuardInstance->BiosOffset + Offset), 32)) | (LShiftU64 (BIOSGUARD_F0_INDEX, 16)) | BIOSGUARD_COMMAND_SET_FLASH_INDEX;
  ///
  /// Write to Flash Index from Buffer Offset Index with specific Size command
  ///
  mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++] = (LShiftU64 (DataByteCount, 32)) | (LShiftU64 (BIOSGUARD_B0_INDEX, 24)) | (LShiftU64 (BIOSGUARD_F0_INDEX, 16)) | BIOSGUARD_COMMAND_WRITE_IMM;

  ///
  /// Place the data from the caller into the global BIOS Guard Update data
  ///
  CopyMem (&mBiosGuardUpdateData[mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize], Buffer, DataByteCount);

  ///
  /// Update the size of the data section to match the input data size
  ///
  mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize += DataByteCount;

  return;
}

/**
  This service fills BIOS Guard script buffer for erasing blocks in flash.
  BIOS should invoke this function prior to calling BIOSGUARD_PROTOCOL.Execute() with all the relevant data required for flash erase.
  This function will not invoke BIOS Guard module, only create script required for erasing each block in the flash.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This     Pointer to the BIOSGUARD_PROTOCOL instance.
  @param[in] Offset   This value specifies the offset from the start of the SPI Flash component where BIOS Image is located.
**/
VOID
EFIAPI
BiosGuardProtocolBlockErase (
  IN BIOSGUARD_PROTOCOL *This,
  IN UINTN              Offset
  )
{
  BIOSGUARD_INSTANCE *BiosGuardInstance;

  BiosGuardInstance = BIOSGUARD_INSTANCE_FROM_BIOSGUARDPROTOCOL (This);

  ///
  /// Set Flash Index immediate command
  ///
  mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++] = (LShiftU64 ((BiosGuardInstance->BiosOffset + Offset), 32)) | (LShiftU64 (BIOSGUARD_F0_INDEX, 16)) | BIOSGUARD_COMMAND_SET_FLASH_INDEX;

  ///
  /// Erase Flash Index command
  ///
  mBiosGuardUpdatePackagePtr->BgupBuffer[mBiosGuardUpdateCounter++] = (LShiftU64 (BIOSGUARD_F0_INDEX, 16)) | BIOSGUARD_COMMAND_ERASE_BLK;

  ///
  /// No change necessary to the data section size
  ///
  mBiosGuardUpdatePackagePtr->BgupHeader.DataSectionSize += 0;

  return;
}
