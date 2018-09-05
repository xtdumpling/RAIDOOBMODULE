//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file TerminalName.c
    EFI_COMPONENT_NAME_PROTOCOL: GetDriverName(), GetControllerName().

**/

//---------------------------------------------------------------------------
#include "Terminal.h"

//---------------------------------------------------------------------------

BOOLEAN LanguageCodesEqual(
    IN CONST CHAR8* LangCode1,
    IN CONST CHAR8* LangCode2
);

CHAR16 gName[] = L"AMI Terminal Driver";

/**
    Gets the driver name.

    @param  This
    @param  Language
    @param  DriverName

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalGetDriverName (
    IN EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN CHAR8                        *Language,
    OUT CHAR16                      **DriverName
)
{
    if (!Language || !DriverName) return EFI_INVALID_PARAMETER;

    if ( !LanguageCodesEqual( Language, This->SupportedLanguages) ) {
        return EFI_UNSUPPORTED;
    }

    *DriverName = gName;
    return EFI_SUCCESS;
}

/**
    returns EFI_UNSUPPORTED.

    @param  This
    @param  ControllerHandle
    @param  ChildHandle OPTIONAL
    @param  Language
    @param  ControllerName

    @retval EFI_STATUS

**/

EFI_STATUS
EFIAPI
TerminalGetControllerName (
    IN EFI_COMPONENT_NAME_PROTOCOL  *This,
    IN EFI_HANDLE                   ControllerHandle,
    IN EFI_HANDLE                   ChildHandle OPTIONAL,
    IN CHAR8                        *Language,
    OUT CHAR16                      **ControllerName
)
{
    return EFI_UNSUPPORTED;
}

EFI_COMPONENT_NAME_PROTOCOL gComponentName = {
    TerminalGetDriverName,
    TerminalGetControllerName,
    0
};

//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
