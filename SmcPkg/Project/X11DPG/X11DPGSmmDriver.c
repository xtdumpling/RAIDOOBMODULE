//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Modify USB WAKE control GPIO pin for M/B 1.02.
//    Reason:
//    Auditor:  Jimmy Chiu
//    Date:     Jun/22/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/27/2016
//
//****************************************************************************
//****************************************************************************
//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Protocol/UsbPolicy.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>


// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

BOOLEAN gFrontUSBPort = FALSE;
BOOLEAN gRealUSBPort = FALSE;

VOID
SmcGPIOEnableUSBWake(void)
{
    if(gRealUSBPort)
        GpioSetOutputValue(Real_USB_Wake_Pin, 1);
    else
        GpioSetOutputValue(Real_USB_Wake_Pin, 0);

    if(gFrontUSBPort) {
        GpioSetOutputValue(Front_USB_Wake_Pin1, 1);
        GpioSetOutputValue(Front_USB_Wake_Pin2, 1);
    } else {
        GpioSetOutputValue(Front_USB_Wake_Pin1, 0);
        GpioSetOutputValue(Front_USB_Wake_Pin2, 0);
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPGS1SmiHandler
//
// Description: This is a template SuperM S1 SMI Handler for Porting.
//
// Input:       DispatchHandle  - EFI Handle
//              DispatchContext - Pointer to the EFI_SMM_SX_DISPATCH_CONTEXT
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
X11DPGS1SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;

    SmcGPIOEnableUSBWake();
    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPGS3SmiHandler
//
// Description: This is a template SuperM Sx SMI Handler for Porting.
//
// Input:       DispatchHandle  - EFI Handle
//              DispatchContext - Pointer to the EFI_SMM_SX_DISPATCH_CONTEXT
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
X11DPGS3SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    
    SmcGPIOEnableUSBWake();
    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPGS4SmiHandler
//
// Description: This is a template SuperM S4 SMI Handler for Porting.
//
// Input:       DispatchHandle  - EFI Handle
//              DispatchContext - Pointer to the EFI_SMM_SX_DISPATCH_CONTEXT
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
X11DPGS4SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;

    SmcGPIOEnableUSBWake();
    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPGS5SmiHandler
//
// Description: This is a template SuperM S5 SMI Handler for Porting.
//
// Input:       DispatchHandle  - EFI Handle
//              DispatchContext - Pointer to the EFI_SMM_SX_DISPATCH_CONTEXT
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
X11DPGS5SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    
    SmcGPIOEnableUSBWake();
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPGPowerButtonSmiHandler
//
// Description: This is a template SuperM Power Button SMI Handler for Porting.
//
// Input:       DispatchHandle  - EFI Handle
//              DispatchContext - Pointer to the
//                                EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
X11DPGPowerButtonSmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS Status = EFI_SUCCESS;  

    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InSmmFunction
//
// Description: Installs SuperM SMM Child Dispatcher Handler.
//
// Input:       ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//                  EFI_NOT_FOUND - The SMM Base protocol is not found.
//                  EFI_SUCCESS   - Installs SuperM SMM Child Dispatcher Handler
//                                  successfully.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS 
InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS Status;
    EFI_HANDLE Handle;    
    EFI_SMM_SX_REGISTER_CONTEXT SxDispatchContext;
    EFI_SMM_SX_DISPATCH2_PROTOCOL *pEfiSmmSxDispatch2Protocol;
    EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT Context = {EfiPowerButtonEntry};
    EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *pEfiSmmPowerButtonDispatch2Protocol;      
    EFI_GUID gEfiSetupGuid = SETUP_GUID;
    USB_SUPPORT_SETUP SetupData;
    UINTN SetupSize = sizeof(USB_SUPPORT_SETUP);

    Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid,
                                       NULL,
                                       &pEfiSmmSxDispatch2Protocol);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    if (!EFI_ERROR(Status)) {
#if defined A_S1 && A_S1 == 0x01
        SxDispatchContext.Type  = SxS1;
        SxDispatchContext.Phase = SxEntry;     
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPGS1SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);  
        if (EFI_ERROR(Status)) return EFI_SUCCESS;        
#endif  // #if defined A_S1 && A_S1 == 0x01

#if defined A_S3 && A_S3 == 0x01
        SxDispatchContext.Type  = SxS3;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPGS3SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);  
        if (EFI_ERROR(Status)) return EFI_SUCCESS;
#endif  // #if defined A_S3 && A_S3 == 0x01            

#if defined A_S4 && A_S4 == 0x01
        SxDispatchContext.Type  = SxS4;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPGS4SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);
        if (EFI_ERROR(Status)) return EFI_SUCCESS;        
#endif  // #if defined A_S1 && A_S1 == 0x01        

        SxDispatchContext.Type  = SxS5;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPGS5SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle); 
        if (EFI_ERROR(Status)) return Status;        
    }

    Status = gSmst->SmmLocateProtocol (&gEfiSmmPowerButtonDispatch2ProtocolGuid,
                                       NULL,
                                       &pEfiSmmPowerButtonDispatch2Protocol);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pEfiSmmPowerButtonDispatch2Protocol->Register (pEfiSmmPowerButtonDispatch2Protocol,
                                                            X11DPGPowerButtonSmiHandler,
                                                            &Context,
                                                            &Handle);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pRS->GetVariable(
                    L"UsbSupport",
                    &gEfiSetupGuid,
                    NULL,
                    &SetupSize,
                    &SetupData);
    if(!Status){
        gRealUSBPort = (SetupData.RealUSBPort == 1) ? TRUE : FALSE;
        gFrontUSBPort = (SetupData.FrontUSBPort == 1) ? TRUE : FALSE;
    }

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMSmiDriverInit
//
// Description: Installs SuperM SMM Child Dispatcher Handler.
//
// Input:       ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//                  EFI_NOT_FOUND - The SMM Base protocol is not found.
//                  EFI_SUCCESS   - Installs SuperM SMM Child Dispatcher Handler
//                                  successfully.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS X11DPGSmmDriverInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
