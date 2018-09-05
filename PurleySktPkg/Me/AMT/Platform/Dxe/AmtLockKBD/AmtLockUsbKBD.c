//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
//**********************************************************************
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
//
//**********************************************************************
/** @file AmtLockUsbKBD.c
    AMT Lock USB KeyBoard Functions.

**/
#include <AmiDxeLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/AmiUsbController.h>
#include <Protocol/AlertStandardFormat.h>
//============================================================================
// Type definitions
//======================================================================
//Function Prototypes
//======================================================================
// Global and extern variables
EFI_SMM_SYSTEM_TABLE2   *gSmst2 = NULL;
EFI_USB_PROTOCOL        *gAmiUsbProtocol = NULL;

/**
    Use UsbRtKbcAccessControl() to lock keyboard.

          
    @param DispatchHandle 
    @param Context 
    @param CommBuffer 
    @param CommBufferSize 

    @retval EFI_STATUS Status


**/
EFI_STATUS AMTLockUsbKBDSmi (
  IN     EFI_HANDLE     DispatchHandle,
  IN     CONST VOID     *Context,
  IN OUT VOID           *CommBuffer,
  IN OUT UINTN          *CommBufferSize
  )
{
    gAmiUsbProtocol->UsbRtKbcAccessControl(1);

    return EFI_SUCCESS;
}
/**
    While USB_RUNTIME_DRIVER_IN_SMM is on, the
    UsbRtKbcAccessControl() will be ready after the
    gAmiUsbSmmProtocolGuid is installed.
    Register the AMTLockUsbKBDSmi after gAmiUsbSmmProtocolGuid is installed.

    @param Protocol 
    @param Interface 
    @param Handle 

    @retval EFI_STATUS Status


**/
EFI_STATUS
AmiUsbSmmProtocolCallback (
    CONST EFI_GUID  *Protocol,
    VOID            *Interface,
    EFI_HANDLE      Handle
)
{
    EFI_STATUS      Status;
    EFI_HANDLE      SmiHandle = NULL;

    Status = gSmst2->SmiHandlerRegister( &AMTLockUsbKBDSmi,
                                         &gEfiEventLegacyBootGuid, &SmiHandle );
    if( EFI_ERROR(Status) ) return Status;
    
	return Status;
}
/**
    If the BootOption is set with LockKeyboard, register the
    event with gEfiEventLegacyBootGuid or register SMI directly.
    The PiSmmIpl will trigger it to lock keyboard.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status


**/
EFI_STATUS
InSmmFunction(
    IN EFI_HANDLE          ImageHandle,
    IN EFI_SYSTEM_TABLE    *SystemTable
)
{
    EFI_STATUS Status;
    EFI_SMM_BASE2_PROTOCOL          *pEfiSmmBase2Protocol = NULL;
    ALERT_STANDARD_FORMAT_PROTOCOL  *pAlertStandardFormatProtocol = NULL;
    AMI_USB_SMM_PROTOCOL            *pAmiUsbSmmProtocol = NULL;
    VOID	                        *AmiUsbSmmProtocolReg = NULL;
    ASF_BOOT_OPTIONS                *pAsfBootOptions = NULL;
    EFI_HANDLE                      SmiHandle = NULL;
    
    Status = pBS->LocateProtocol( &gEfiUsbProtocolGuid, NULL,
                                  &gAmiUsbProtocol );
	if(EFI_ERROR(Status)) return Status;
    
    Status = pBS->LocateProtocol( &gEfiSmmBase2ProtocolGuid, NULL,
                                  &pEfiSmmBase2Protocol );
    if( !EFI_ERROR(Status) )
    {
        Status = pEfiSmmBase2Protocol->GetSmstLocation(
                    pEfiSmmBase2Protocol, &gSmst2 );
        if( EFI_ERROR(Status) ) return Status;
    }

    Status = pBS->LocateProtocol( &gAlertStandardFormatProtocolGuid, NULL,
                                  &pAlertStandardFormatProtocol );
    if( !EFI_ERROR(Status) )
    {
        Status = pAlertStandardFormatProtocol->GetBootOptions (
                        pAlertStandardFormatProtocol, &pAsfBootOptions) ;
        if( EFI_ERROR(Status) ) return Status;
    }
    
    if( pAsfBootOptions->BootOptions & LOCK_KEYBOARD )
    {
#if USB_RUNTIME_DRIVER_IN_SMM
        Status = gSmst2->SmmLocateProtocol( &gAmiUsbSmmProtocolGuid , NULL,
                                            &pAmiUsbSmmProtocol);
        if( !EFI_ERROR(Status) )
        {
            Status = gSmst2->SmiHandlerRegister( &AMTLockUsbKBDSmi,
                                                 &gEfiEventLegacyBootGuid,
                                                 &SmiHandle );
        }
        else
        {
            Status = gSmst2->SmmRegisterProtocolNotify (
                                &gAmiUsbSmmProtocolGuid,
                                AmiUsbSmmProtocolCallback,
                                &AmiUsbSmmProtocolReg );
        }
#else
        Status = gSmst2->SmiHandlerRegister( &AMTLockUsbKBDSmi,
                                             &gEfiEventLegacyBootGuid,
                                             &SmiHandle );
#endif
    }
    return Status;
}
/**
    Lock USB Keyboard Entrypoint.

    @param ImageHandle 
    @param SystemTable 

    @retval EFI_STATUS Status

**/
EFI_STATUS
AMTLockUsbKBDEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandlerEx (ImageHandle, SystemTable, InSmmFunction, NULL);
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
