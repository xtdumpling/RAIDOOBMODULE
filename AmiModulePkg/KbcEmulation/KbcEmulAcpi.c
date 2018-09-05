//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file KbcEmulAcpi.c
    This file contains Emulation SMI disable function on ACPI 
    enable/Disable SMI 

**/

//---------------------------------------------------------------------------
#include <Token.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include "KbcEmul.h"

#if EMULATION_ACPI_ENABLE_DISPATCH
#include <Protocol/AcpiModeEnable.h>
#endif

//---------------------------------------------------------------------------

extern BOOLEAN  AcpiEmulationDisable;

/**
    This routine will be called when ACPI enabled or disabled SMI happens.

    @param DispatchHandle  - Handle to the Dispatcher

    @retval VOID

**/

VOID
EFIAPI
KbcEmulationDisable (
    IN EFI_HANDLE   DispatchHandle
)
{
    TrapEnable(FALSE);
    AcpiEmulationDisable=TRUE;
    return;
}

/**
  Function that register's Callback which will be invoked on AcpiModeEnable
  SMI.

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @return Status Code
**/

EFI_STATUS
EFIAPI
CreateKbcEmulAcpiEnLink (
    IN CONST EFI_GUID  *Protocol,
    IN VOID            *Interface,
    IN EFI_HANDLE      Handle 
) 
{
    EFI_STATUS                  Status;
    EFI_ACPI_DISPATCH_PROTOCOL  *AcpiEnDispatch;
    EFI_HANDLE                  EnableHandle;
       
    Status= gSmst1->SmmLocateProtocol(&gEfiAcpiEnDispatchProtocolGuid, NULL, &AcpiEnDispatch);
    if (EFI_ERROR(Status)) { 
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    Status = AcpiEnDispatch->Register(AcpiEnDispatch, KbcEmulationDisable, &EnableHandle);
    ASSERT_EFI_ERROR(Status);
    return Status;
    
}

/**
  Function that register's Callback which will be invoked on AcpiModeDisable
  SMI.

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @return Status Code
**/

EFI_STATUS
EFIAPI
CreateKbcEmulAcpiDisLink (
    IN CONST EFI_GUID  *Protocol,
    IN VOID            *Interface,
    IN EFI_HANDLE      Handle 
) 
{
    EFI_STATUS                  Status;
    EFI_ACPI_DISPATCH_PROTOCOL  *AcpiDisDispatch;
    EFI_HANDLE                  DisableHandle;
    
    Status= gSmst1->SmmLocateProtocol(&gEfiAcpiDisDispatchProtocolGuid, NULL, &AcpiDisDispatch);
    if (EFI_ERROR(Status)) { 
        ASSERT_EFI_ERROR(Status);
        return Status;
    }
    Status = AcpiDisDispatch->Register(AcpiDisDispatch, KbcEmulationDisable, &DisableHandle);
    ASSERT_EFI_ERROR(Status);
    return Status;
}
/**
    This routine will be called by KbcEmulation Init 
    to register the callback function on Acpi Enable/Disable SMI.

    @param VOID

    @retval EFI_STATUS

**/

EFI_STATUS
RegisterAcpiEnableCallBack ()
{
    EFI_STATUS                  Status;
    void                        *KbcEmulAcpiReg;
    EFI_ACPI_DISPATCH_PROTOCOL  *AcpiEnDisDispatch;

    Status= gSmst1->SmmLocateProtocol(&gEfiAcpiEnDispatchProtocolGuid, NULL, &AcpiEnDisDispatch);
    if (!EFI_ERROR(Status)) {
        CreateKbcEmulAcpiEnLink(NULL, NULL, NULL);
    } else {
        Status = gSmst1->SmmRegisterProtocolNotify (
                                      &gEfiAcpiEnDispatchProtocolGuid,
                                      CreateKbcEmulAcpiEnLink,
                                      &KbcEmulAcpiReg
                                      );
        ASSERT_EFI_ERROR(Status);
    }
    Status= gSmst1->SmmLocateProtocol(&gEfiAcpiDisDispatchProtocolGuid, NULL, &AcpiEnDisDispatch);
    if (!EFI_ERROR(Status)) {
        CreateKbcEmulAcpiDisLink(NULL, NULL, NULL);
    } else {
        Status = gSmst1->SmmRegisterProtocolNotify (
                                          &gEfiAcpiDisDispatchProtocolGuid,
                                          CreateKbcEmulAcpiDisLink,
                                          &KbcEmulAcpiReg
                                          );
        ASSERT_EFI_ERROR(Status);
    }
    return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

