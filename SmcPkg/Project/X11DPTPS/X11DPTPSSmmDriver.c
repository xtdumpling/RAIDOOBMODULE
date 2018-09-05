//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jacker Yeh
//    Date:     Feb/04/2016
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
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Library/SmmServicesTableLib.h>


// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)



//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPTPSS1SmiHandler
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
X11DPTPSS1SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;


    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPTPSS3SmiHandler
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
X11DPTPSS3SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    

    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPTPSS4SmiHandler
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
X11DPTPSS4SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;


    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPTPSS5SmiHandler
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
X11DPTPSS5SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   X11DPTPSPowerButtonSmiHandler
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
X11DPTPSPowerButtonSmiHandler (
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
    EFI_GUID mSetupGuid = SETUP_GUID;
    SETUP_DATA SetupData;
    UINTN SetupSize = sizeof(SetupData);

    Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid,
                                       NULL,
                                       &pEfiSmmSxDispatch2Protocol);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    if (!EFI_ERROR(Status)) {
#if defined A_S1 && A_S1 == 0x01
        SxDispatchContext.Type  = SxS1;
        SxDispatchContext.Phase = SxEntry;     
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPTPSS1SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);  
        if (EFI_ERROR(Status)) return EFI_SUCCESS;        
#endif  // #if defined A_S1 && A_S1 == 0x01

#if defined A_S3 && A_S3 == 0x01
        SxDispatchContext.Type  = SxS3;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPTPSS3SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);  
        if (EFI_ERROR(Status)) return EFI_SUCCESS;
#endif  // #if defined A_S3 && A_S3 == 0x01            

#if defined A_S4 && A_S4 == 0x01
        SxDispatchContext.Type  = SxS4;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPTPSS4SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);
        if (EFI_ERROR(Status)) return EFI_SUCCESS;        
#endif  // #if defined A_S1 && A_S1 == 0x01        

        SxDispatchContext.Type  = SxS5;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       X11DPTPSS5SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle); 
        if (EFI_ERROR(Status)) return Status;        
    }

    Status = gSmst->SmmLocateProtocol (&gEfiSmmPowerButtonDispatch2ProtocolGuid,
                                       NULL,
                                       &pEfiSmmPowerButtonDispatch2Protocol);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pEfiSmmPowerButtonDispatch2Protocol->Register (pEfiSmmPowerButtonDispatch2Protocol,
                                                            X11DPTPSPowerButtonSmiHandler,
                                                            &Context,
                                                            &Handle);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

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

EFI_STATUS X11DPTPSSmmDriverInit (
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
