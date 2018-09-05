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
//**********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------
//
// Name: FwCapsuleInfo.c
//
// Description:   File contains hook FwCapsuleInfo() which is called from  Recovery.LoadRecoveryCapsule
//                Hook use dot update the default Recovery File name and Size if defaults are 
//                overridden in FlashUpd EFI Var. 
//----------------------------------------------------------------------
//<AMI_FHDR_END>

// Module specific Includes
#include "Efi.h"
#include "Pei.h"
#include "Token.h"
#include <AmiPeiLib.h>
#include <Hob.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <FlashUpd.h>

//------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------
const UINTN RecoveryCapImageSize = FWCAPSULE_IMAGE_SIZE;

AMI_FLASH_UPDATE_BLOCK gFlashUpdDesc;

//------------------------------------------------------------------------
// Local prototypes
//------------------------------------------------------------------------
EFI_STATUS
FwCapsuleInfo (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID          **pCapsuleName,
  IN OUT UINTN         *pCapsuleSize,
  OUT   BOOLEAN        *ExtendedVerification
);
//<AMI_PHDR_START>
//----------------------------------------------------------------------
//
// Procedure:    FwCapsuleInfo
//
// Description:  Updates the Recovery File name and size if defaults are
//               overridden in FlashUpd EFI Var.
//               Called from Recovery LoadRecoveryCapsule.
//
//    pCapsuleName  Pointer to the variable containing a Recovery File name
//    pCapsuleSize  Pointer to the size of recovery image capsule, in bytes.
//    ExtendedVerification Indicates to Recovery module whether Fw Capsule
//                  Recovery path will perform image size check.
//
// Output:
//    EFI_SUCCESS
//----------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
FwCapsuleInfo (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN OUT VOID          **pCapsuleName,
  IN OUT UINTN         *pCapsuleSize,
  OUT   BOOLEAN        *ExtendedVerification
){
    EFI_STATUS          Status;
    UINTN               Size;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable = NULL;
    EFI_GUID gFlashUpdGuid  = FLASH_UPDATE_GUID;

    if(!pCapsuleName && !pCapsuleSize)
        return EFI_UNSUPPORTED;

    if(ExtendedVerification != NULL)
        *ExtendedVerification = TRUE;

    if(pCapsuleSize != NULL)
    {
        DEBUG ((SecureMod_DEBUG_LEVEL, "FW Capsule Info\nDefault Size %X\n", *(UINT32*)pCapsuleSize));
        if(*pCapsuleSize < RecoveryCapImageSize)
            *pCapsuleSize = RecoveryCapImageSize;

        if(pCapsuleName != NULL)
        {
            DEBUG ((SecureMod_DEBUG_LEVEL, "Default Name %a, Size %X\n", *pCapsuleName, *(UINT32*)pCapsuleSize));
        // Detect if we are in Flash Update mode and set some recovery global variables
        // Read "FlashOp" Variable to update global RecoveryFileName, Size
            Status = (*PeiServices)->LocatePpi( PeiServices,
                                        &gEfiPeiReadOnlyVariable2PpiGuid,
                                        0,
                                        NULL,
                                        &ReadOnlyVariable );

            if(EFI_ERROR(Status))
               return Status;

            Size = sizeof(AMI_FLASH_UPDATE_BLOCK);
            Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable,
                                        FLASH_UPDATE_VAR,
                                        &gFlashUpdGuid,
                                        NULL,
                                        &Size,
                                        &gFlashUpdDesc );
            if(!EFI_ERROR(Status))
            {
                if(gFlashUpdDesc.FlashOpType == FlRecovery && gFlashUpdDesc.FwImage.AmiRomFileName[0] != 0)
                    *pCapsuleName = (VOID*)gFlashUpdDesc.FwImage.AmiRomFileName;

                *pCapsuleSize = (UINTN)(gFlashUpdDesc.ImageSize);
                (*PeiServices)->SetBootMode(PeiServices, BOOT_ON_FLASH_UPDATE);
            }
            DEBUG ((SecureMod_DEBUG_LEVEL, "ReFlash variable %r\nImage Name %a, Size %X\n", Status, *pCapsuleName, *(UINT32*)pCapsuleSize));
        }
    }
    return EFI_SUCCESS;
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
