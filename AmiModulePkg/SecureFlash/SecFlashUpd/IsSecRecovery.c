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
//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: IsSecRecovery.c - PEI recovery services
//
// Description:   File contains hook to determine if BootMode needs to follow 
//                recovery path due to pending Flash Update
//
//----------------------------------------------------------------------
//<AMI_FHDR_END>
// Module specific Includes
#include "Efi.h"
#include "Pei.h"
#include "Token.h"
#include <AmiPeiLib.h>
#include <Hob.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <FlashUpd.h>
#include <Capsule.h>
#include <Guid/CapsuleVendor.h>

//------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------
#if  FWCAPSULE_RECOVERY_SUPPORT == 1
// {64C96700-6B4C-480C-A3E1-B8BDE8F602B2}
#define AMI_PEI_AFTER_MRC_GUID \
 {0x64c96700, 0x6b4c, 0x480c, 0xa3, 0xe1, 0xb8, 0xbd, 0xe8, 0xf6, 0x2, 0xb2}
 EFI_GUID gAmiPeiAfterMrcGuid      = AMI_PEI_AFTER_MRC_GUID;
 EFI_GUID gFWCapsuleGuid           = APTIO_FW_CAPSULE_GUID;
#endif
static EFI_GUID gFlashUpdGuid            = FLASH_UPDATE_GUID;

//------------------------------------------------------------------------
// Local prototypes
//------------------------------------------------------------------------
EFI_STATUS ChangeBootMode (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);

EFI_STATUS ChangeBootModeAfterEndofMrc (
    IN EFI_PEI_SERVICES         **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID                     *InvokePpi
);

//------------------------------------------------------------------------
// PPIs notifications to be  installed
//------------------------------------------------------------------------
static EFI_PEI_PPI_DESCRIPTOR mFlashUpdateModePpi[] =
  {
      (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gFlashUpdBootModePpiGuid,
    NULL
};
static EFI_PEI_PPI_DESCRIPTOR mRecoveryModePpi[] =
{
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiBootInRecoveryModePpiGuid,
    NULL
};

//------------------------------------------------------------------------
// PPI event to  hook up to
//------------------------------------------------------------------------
static EFI_PEI_NOTIFY_DESCRIPTOR  mBootModePpi[] =
{
        { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
           &gEfiPeiMasterBootModePpiGuid,//&gMasterBootModeGuid,
         ChangeBootMode
        },
};

#if FWCAPSULE_RECOVERY_SUPPORT == 1
static EFI_PEI_NOTIFY_DESCRIPTOR EndOfMrcNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
      &gAmiPeiAfterMrcGuid,
      ChangeBootModeAfterEndofMrc
    },
};
#endif
//--------------------------------------------------------------------------
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetFlashUpdateVar
//
// Description:
//  This function checks if recovery flow is needed due to pending Flash Update
//
// Input:
//  PeiServices Pointer to the PEI services table
//
// Output:
//  TRUE - recovery is requested
//  FALSE - recovery is not requested
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN 
GetFlashUpdateVar (
    EFI_PEI_SERVICES    **PeiServices,
    AMI_FLASH_UPDATE_BLOCK *FlashUpdDesc,
    UINT32                  *CounterHi
){
    EFI_STATUS      Status;
    UINTN           Size;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable;

// Check FlashOp variable for Recovery Flash operation 
// Detect if we are in Flash Update mode and set some recovery global variables
// Read "FlashOp" Variable to update global RecoveryFileName, Size
    Status = (*PeiServices)->LocatePpi( PeiServices,
                                &gEfiPeiReadOnlyVariable2PpiGuid,//gPeiReadOnlyVariablePpiGuid,
                                0,
                                NULL,
                                &ReadOnlyVariable );
    if(EFI_ERROR(Status)) 
        return FALSE;

    Size = sizeof(AMI_FLASH_UPDATE_BLOCK);
    Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable,
                                    FLASH_UPDATE_VAR,
                                    &gFlashUpdGuid,
                                    NULL,
                                    &Size,
                                    FlashUpdDesc );
    if (!EFI_ERROR(Status)) 
    {
        Size = sizeof(UINT32);
        Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable,
                                        L"MonotonicCounter", 
                                        &gAmiGlobalVariableGuid,
                                        NULL, 
                                        &Size,
                                        CounterHi);
        if (EFI_ERROR(Status) || FlashUpdDesc->MonotonicCounter == 0xffffffff)
            *CounterHi = 0xffffffff;
        
        return TRUE;
    }

    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   IsFlashUpdate
//
// Description:
//  This function returns proper BootMode if Flash Update mode is pending
//
// Input:
//  **PeiServices - Pointer to the PEI services table
//  *BootMode     - Pointer to a BootMode variable
//
// Output:
//  BOOLEAN 
//      TRUE - recovery is requested
//      FALSE - recovery is not requested
//  BootMode - updated BootMode value if TRUE
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN 
IsFlashUpdate(
    EFI_PEI_SERVICES    **PeiServices,
    EFI_BOOT_MODE       *BootMode
){
    UINT32                  CounterHi;
    AMI_FLASH_UPDATE_BLOCK FlashUpdDesc;

    CounterHi = 0;
    FlashUpdDesc.MonotonicCounter = CounterHi;
    FlashUpdDesc.FlashOpType = FlDisabled;

//SetMode should have set FlashUpd var even if no MC var detected.
// MC check should fail Recovery
    if(/**BootMode != BOOT_IN_RECOVERY_MODE && */
        GetFlashUpdateVar(PeiServices, &FlashUpdDesc, &CounterHi) &&
        CounterHi==(UINT32)FlashUpdDesc.MonotonicCounter)
    {
        if(FlashUpdDesc.FlashOpType == FlRecovery)
        {
            *BootMode = BOOT_IN_RECOVERY_MODE;
            return TRUE;
        }
        if(FlashUpdDesc.FlashOpType == FlCapsule/* && *BootMode == BOOT_ON_S3_RESUME*/)
        {
            *BootMode = BOOT_ON_FLASH_UPDATE;
            return TRUE;  
        }
    }
    return FALSE;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ChangeBootMode
//
// Description:
//
// Input:       PeiServices      - Pointer to the PEI services table
//              NotifyDescriptor - Pointer to the descriptor for the
//                                 notification event.
//              InvokePpi        - Pointer to the PPI that was installed
//
// Output:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS ChangeBootMode (
        IN EFI_PEI_SERVICES **PeiServices,
        IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
        IN VOID *InvokePpi)
{
    EFI_STATUS           Status;
    EFI_BOOT_MODE        BootMode;

    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
    if (!EFI_ERROR(Status) && (BootMode != BOOT_IN_RECOVERY_MODE))
        if(IsFlashUpdate(PeiServices, &BootMode) && BootMode == BOOT_IN_RECOVERY_MODE)
        // Change the Boot Mode to Recovery from S3_RESUME/BOOT_ON_FLASH_UPDATE
         {
                (*PeiServices)->InstallPpi(PeiServices, mRecoveryModePpi);
                 (*PeiServices)->SetBootMode(PeiServices, BootMode);
         }

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ChangeBootModeAfterEndofMrc
//
// Description: Change BootMode to BOOT_ON_FLASH_UPDATE if Capsule update is pending
//
// Input:       PeiServices      - Pointer to the PEI services table
//              NotifyDescriptor - Pointer to the descriptor for the
//                                 notification event.
//              InvokePpi        - Pointer to the PPI that was installed
//
// Output:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if FWCAPSULE_RECOVERY_SUPPORT == 1
EFI_STATUS ChangeBootModeAfterEndofMrc (
        IN EFI_PEI_SERVICES **PeiServices,
        IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
        IN VOID *InvokePpi)
{
    EFI_STATUS           Status;
    EFI_BOOT_MODE        BootMode;

    Status = (*PeiServices)->GetBootMode( PeiServices, &BootMode );
//PEI_TRACE(((UINTN)TRACE_ALWAYS, PeiServices, "FWCapsule AfterMrc BootMode %x\n", BootMode));
//    if (!EFI_ERROR(Status) &&  (BootMode != BOOT_IN_RECOVERY_MODE))
// BOOT_ON_FLASH_UPDATE may already be set by Capsule PEI
    if(!EFI_ERROR(Status) && (BootMode == BOOT_ON_S3_RESUME))
    {
    // Change the Boot Mode to BOOT_ON_FLASH_UPDATE from S3_RESUME
       if (IsFlashUpdate(PeiServices, &BootMode) && (BootMode == BOOT_ON_FLASH_UPDATE))
           (*PeiServices)->SetBootMode(PeiServices, BootMode);
    }
//PEI_TRACE(((UINTN)TRACE_ALWAYS, PeiServices, "New BootMode %x\n", BootMode));
    //install Flash Update Boot Mode PPI. 
	//A back up only. CapsulePEI Lib->ProcessCapsuleImage should install PPI if Capsule is detected
    if(BootMode == BOOT_ON_FLASH_UPDATE)
        (*PeiServices)->InstallPpi( PeiServices, mFlashUpdateModePpi);

    return EFI_SUCCESS;
}
#endif
//<AMI_PHDR_START>
//----------------------------------------------------------------------
//
// Procedure:    IsSecRecoveryPeimEntry
//
// Description:    Installs callback on SetBootMode
//
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
IsSecRecoveryPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
)
{
#if  FWCAPSULE_RECOVERY_SUPPORT == 1
    (*PeiServices)->NotifyPpi( PeiServices, EndOfMrcNotifyList);
#endif
    return (*PeiServices)->NotifyPpi( PeiServices, mBootModePpi);
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
