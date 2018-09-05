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
/** @file BootGuardDxe.c
    Chain of trust for Dxe

**/
//**********************************************************************

#include "BootGuardDxe.h"

/**
  Determine if Boot Guard is supported

  @retval TRUE  - Processor is Boot Guard capable.
  @retval FALSE - Processor is not Boot Guard capable.

**/
BOOLEAN
IsBootGuardSupported (
  VOID
  )
{
  UINT64          BootGuardBootStatus;
  UINT32          BootGuardAcmStatus;
  UINT64          BootGuardCapability;

  BootGuardBootStatus = *(UINT64 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_BOOTSTATUS);
  BootGuardAcmStatus  = *(UINT32 *)(UINTN)(TXT_PUBLIC_BASE + R_CPU_BOOT_GUARD_ACM_STATUS);
  BootGuardCapability = AsmReadMsr64 (MSR_BOOT_GUARD_SACM_INFO) & B_BOOT_GUARD_SACM_INFO_CAPABILITY;

  if (BootGuardCapability != 0) {
    DEBUG ((DEBUG_INFO, "Processor supports Boot Guard.\n"));
    return TRUE;
  } else {
    DEBUG ((DEBUG_ERROR, "Processor does not support Boot Guard.\n"));
    return FALSE;
  }
}

VOID
BootGuardDxeCallback (
    IN EFI_EVENT Event,
    IN VOID      *Context
)
{
    EFI_INPUT_KEY           Key;
    EFI_STATUS              Status;

    gST->ConOut->ClearScreen (gST->ConOut);

    gST->ConOut->OutputString (
        gST->ConOut,
        L"WARNING : BOOTGUARD - PEI to DXE HANDOFF VERIFICATION FAILED\n\r"
    );

    gST->ConOut->OutputString (
        gST->ConOut,
        L"System will shutdown\n\r"
    );

    gST->ConOut->OutputString (
        gST->ConOut,
        L"Press any key\n\r"
    );

    do {
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    } while (Status != EFI_SUCCESS);

    gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    CpuDeadLoop ();

    gBS->CloseEvent (Event);
}

VOID
BootGuardDxeRegisterCallBack (
    VOID
)
{
    EFI_EVENT   Event;
    VOID        *NotifyReg;
    EFI_STATUS  Status;

#ifdef ECP_FLAG
    Status = gBS->CreateEvent (
                 EFI_EVENT_NOTIFY_SIGNAL,
                 EFI_TPL_CALLBACK,
                 BootGuardDxeCallback,
                 NULL,
                 &Event
             );
#else
    Status = gBS->CreateEvent (
		 EVT_NOTIFY_SIGNAL,
                 TPL_CALLBACK,
                 BootGuardDxeCallback,
                 NULL,
                 &Event
             );
#endif
    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return;
    }

    Status = gBS->RegisterProtocolNotify (
                 &gNotifyProtocolGuid ,
                 Event,
                 &NotifyReg
             );

    if ( EFI_ERROR(Status) ) {
        ASSERT_EFI_ERROR (Status);
        return;
    }

    return;
}

EFI_STATUS
BootGuardDxeEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_GUID                                       AmiBootGuardHobGuid = AMI_BOOT_GUARD_HOB_GUID;
    AMI_BOOT_GUARD_HOB           *AmiBootGuardHobPtr;
    EFI_BOOT_MODE                         BootMode;
    EFI_PEI_HOB_POINTERS             HobList;

    if ( IsBootGuardSupported() == FALSE ) {
        return   EFI_SUCCESS;
    }

    DEBUG ((EFI_D_INFO, "[BootGuardDxe.c] : Entry Point...\n"));

    //
    // Check Boot Type
    //
    EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
    if (HobList.Header->HobType != EFI_HOB_TYPE_HANDOFF) {
        DEBUG ((EFI_D_ERROR, "[BootGuardDxe.c] : Get HOB fail\n"));
        return EFI_SUCCESS;
    }

    BootMode  = HobList.HandoffInformationTable->BootMode;
    if ( (BootMode == BOOT_IN_RECOVERY_MODE) || (BootMode == BOOT_ON_FLASH_UPDATE) ) {
        DEBUG ((EFI_D_ERROR, "[BootGuardDxe.c] : In the BOOT_IN_RECOVERY_MODE or BOOT_ON_FLASH_UPDATE\n"));
        return   EFI_SUCCESS;
    }

    AmiBootGuardHobPtr = GetFirstGuidHob (&AmiBootGuardHobGuid);
    if (AmiBootGuardHobPtr == NULL) {
        DEBUG ((EFI_D_ERROR, "[BootGuardDxe.c] : AmiBootGuard DXE Hob not available\n"));
        return   EFI_NOT_FOUND;
    }

    if ( AmiBootGuardHobPtr->AmiBootGuardVerificationforPEItoDXEFlag == 0 ) {
        BootGuardDxeRegisterCallBack();
    }

    DEBUG ((EFI_D_INFO, "[BootGuardDxe.c] : Entry End...\n"));

    return   EFI_SUCCESS;
}

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
