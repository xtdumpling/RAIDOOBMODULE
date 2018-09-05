//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file MEFwUpdLcl.c
**/
//**********************************************************************
//----------------------------------------------------------------------------
// Includes
// Statements that include other files
#include <Token.h>
#include <AmiDxeLib.h>
#include <Protocol/MeFwUpdLclProtocol.h>

//----------------------------------------------------------------------------
// Function Externs
VOID
MeFwUpdateLibInit(
    VOID
);

EFI_STATUS
EFIAPI
MeUpdPlatformUnlock(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT32                      UnlockType
);

EFI_STATUS
EFIAPI
MeUpdPlatformReset(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  EFI_RESET_TYPE              ResetType
);

EFI_STATUS
EFIAPI
MeCheckFwState(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  ME_FW_STATE                 MeFwState
);

EFI_STATUS
EFIAPI
MeGetFwVersion(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  ME_FW_UPD_VERSION           *MeFwVersion
);

EFI_STATUS
EFIAPI
MeUpdHmrfpoEnable(
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This
);

EFI_STATUS
EFIAPI
FwUpdLclFunc (
    IN  ME_FW_UPDATE_LOCAL_PROTOCOL *This,
    IN  UINT8                       *FileBuffer,
    IN  UINTN                       FileLength,
    IN  UINT8                       FiremwareType
);

//----------------------------------------------------------------------------
// Local prototypes

//----------------------------------------------------------------------------
// Local Variables
ME_FW_UPDATE_LOCAL_PROTOCOL *mMeFwUpdateLclProtocol = NULL;
EFI_GUID mMeFwUpdLclProtocolGuid = ME_FW_UPD_LOCAL_PROTOCOL_GUID;

//----------------------------------------------------------------------------
// Function Definitions
/**
 * This hook is Module DXE driver entry.
 * @param ImageHandle The handle associated with this image being loaded into memory
 * @param SystemTable Pointer to the system table
**/
EFI_STATUS
EFIAPI
MEFwUpdLclEntry(
    IN  EFI_HANDLE          ImageHandle,
    IN  EFI_SYSTEM_TABLE    *SystemTable
)
{    
    EFI_STATUS  Status;
    EFI_HANDLE  pHandle = NULL;

    InitAmiLib( ImageHandle, SystemTable );
    //Prepare ME firmware update local protocol
    Status = pBS->AllocatePool( EfiBootServicesData, \
                  sizeof(ME_FW_UPDATE_LOCAL_PROTOCOL), (VOID**)&mMeFwUpdateLclProtocol );
    if( EFI_ERROR(Status) ) return Status;

    MeFwUpdateLibInit();
    mMeFwUpdateLclProtocol->FwUpdBufferAddress  = 0;
    mMeFwUpdateLclProtocol->FwUpdBufferLength   = 0;
    mMeFwUpdateLclProtocol->FwUpdLcl            = FwUpdLclFunc;
    mMeFwUpdateLclProtocol->MeFwCheckMode       = MeCheckFwState;
    mMeFwUpdateLclProtocol->MeFwGetVersion      = MeGetFwVersion;
    mMeFwUpdateLclProtocol->HmrfpoEnable        = MeUpdHmrfpoEnable;
    mMeFwUpdateLclProtocol->PlatformReset       = MeUpdPlatformReset;
    mMeFwUpdateLclProtocol->PlatformUnlock      = MeUpdPlatformUnlock;
    mMeFwUpdateLclProtocol->AmiPostMgr          = NULL;

    Status = pBS->InstallProtocolInterface( 
                    &pHandle,
                    &mMeFwUpdLclProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    mMeFwUpdateLclProtocol );
    return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************