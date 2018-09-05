//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.02
//    Bug Fix : Added PC 0xB2 W/A for system stops at PXE option ROM.
//              (Refer to Grangeville Trunk revision #32.)
//    Reason  : Bug Fixed
//    Auditor : Joe Jhang
//    Date    : May/17/2017
//
//  Rev. 1.01
//    Bug Fix:  Fixed Power Button 4 sec shutdown
//    Reason:   
//    Auditor:  Sunny Yang
//    Date:     Jun/15/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/18/2014
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
#if B2_WORKAROUND_SUPPORT
#include <Protocol/PchTcoSmiDispatch.h>
#endif //B2_WORKAROUND_SUPPORT
#include <Library/SmmServicesTableLib.h>
#include "SmcCspLibSb.h"
// Produced Protocols

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

#ifndef ACPI_SUPPORT
#define ACPI_SUPPORT                    0
#endif

#define PM1_CNT		0x04
#define PM1_CNT_S5	0x1C00
#define PM1_CNT_SLP_EN	0x2000

#define SET_SYSTEM_POWER_STATE         0x80 
#define SET_DEVICE_POWER_STATE         0x80
#define EFI_ACPI_POWER_STATE_S0        0x0
#define EFI_ACPI_POWER_STATE_S1        0x1
#define EFI_ACPI_POWER_STATE_S3        0x3
#define EFI_ACPI_POWER_STATE_S4        0x4
#define EFI_ACPI_POWER_STATE_S5        0x5
#define EFI_ACPI_DEVICE_STATE_D0       0x0
#define EFI_ACPI_DEVICE_STATE_D1       0x1
#define EFI_ACPI_DEVICE_STATE_D3       0x3
// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
BOOLEAN     gIsPwrBtnOverride = FALSE;		//SMC R1.01
UINT8 gSmcPwrBtnFn = 1;

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetSuperMSmiContext
//
// Description: This is a template SuperM SMI GetContext for Porting.
//
// Input:       None
//
// Output:      None
//
// Notes:       Here is the control flow of this function:
//              1. Check if SuperM Smi source.
//              2. If yes, return TRUE.
//              3. If not, return FALSE.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

BOOLEAN GetSuperMSmiContext (VOID)
{
    // Porting if needed
    return FALSE;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMSmiHandler
//
// Description: This is a template SuperM SMI Handler for Porting.
//
// Input:       None
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID SuperMSmiHandler (VOID)
{
    // Porting if needed
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMSwSmiHandler
//
// Description: This is a template SuperM software SMI Handler for Porting.
//
// Input:       DispatchHandle  - EFI Handle
//              DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
//
// Output:      None
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
/*
VOID SuperMSwSmiHandler (
    IN EFI_HANDLE                   DispatchHandle,
    IN EFI_SMM_SW_DISPATCH_CONTEXT  *DispatchContext )
{
    // Porting if needed
}
*/


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMS1SmiHandler
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
SuperMS1SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    
#if WOL_SUPPORT
    SMC_Enable_WOL();
#endif
#if WOR_SUPPORT
    SMC_Enable_WOR();
#endif
	
    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMS3SmiHandler
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
SuperMS3SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    
#if WOL_SUPPORT
    SMC_Enable_WOL();
#endif
#if WOR_SUPPORT
    SMC_Enable_WOR();
#endif
    
    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMS4SmiHandler
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
SuperMS4SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;

#if WOL_SUPPORT
    SMC_Enable_WOL();
#endif
#if WOR_SUPPORT
    SMC_Enable_WOR();
#endif    
//    IoOr32 (GPIO_BASE_ADDRESS + 0x0C, (1 << 25)); //Turn off Power LED, GPIO25
    
    return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMS5SmiHandler
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
SuperMS5SmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS    Status = EFI_SUCCESS;
    
#if WOL_SUPPORT
    SMC_Enable_WOL();
#endif
#if WOR_SUPPORT
    SMC_Enable_WOR();
#endif    
//    IoOr32 (GPIO_BASE_ADDRESS + 0x0C, (1 << 25)); //Turn off Power LED, GPIO25
    
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMPowerButtonSmiHandler
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
SuperMPowerButtonSmiHandler (
    IN EFI_HANDLE        DispatchHandle,
    IN CONST VOID        *DispatchContext,
    IN OUT VOID          *CommonBuffer,
    IN OUT UINTN         *CommonBufferSize
)
{
    EFI_STATUS Status = EFI_SUCCESS;  


    if (gSmcPwrBtnFn){
	    IoWrite16 (PM_BASE_ADDRESS + PM1_CNT, PM1_CNT_S5+PM1_CNT_SLP_EN);
    }
    	

#if WOL_SUPPORT
    SMC_Enable_WOL();
#endif
#if WOR_SUPPORT
    SMC_Enable_WOR();
#endif    

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SuperMChildDispatcher
//
// Description: This is an entry for SuperM SMM Child Dispatcher Handler.
//
// Input:       SmmImageHandle       - EFI Handler
//              *CommunicationBuffer - OPTIONAL
//              *SourceSize          - OPTIONAL
//
// Output:      EFI_STATUS
//              EFI_HANDLER_SUCCESS
//
// Referrals:   GetSuperMSmiContext, SuperMSmiHandler 
//
// Notes:       Here is the control flow of this function:
//              1. Read SMI source status registers.
//              2. If source, call handler.
//              3. Repeat #2 for all sources registered.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS SuperMChildDispatcher (
    IN EFI_HANDLE   SmmImageHandle,
    IN OUT VOID     *CommunicationBuffer OPTIONAL,
    IN OUT UINTN    *SourceSize OPTIONAL )
{
    if (GetSuperMSmiContext()) SuperMSmiHandler();

    return EFI_HANDLER_SUCCESS;
}

#if B2_WORKAROUND_SUPPORT
//<AMI_PHDR_START>                                      //EIP104199 >>
//----------------------------------------------------------------------------
//
// Procedure:   TcoSmiCallback
//
// Description:
//
// Input:
//
// Output:      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static VOID TcoSmiCallback (
  IN  EFI_HANDLE                              DispatchHandle
  )
{
  //
  // Reset system
  //
  IoWrite8(0xcf9, 0x06);
  EFI_DEADLOOP();

}
#endif //B2_WORKAROUND_SUPPORT
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
    EFI_STATUS                   Status;
    EFI_HANDLE                   Handle;    
    EFI_SMM_SX_REGISTER_CONTEXT           SxDispatchContext;
    EFI_SMM_SX_DISPATCH2_PROTOCOL         *pEfiSmmSxDispatch2Protocol;
    EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT Context = {EfiPowerButtonEntry};
    EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL *pEfiSmmPowerButtonDispatch2Protocol;      
    EFI_GUID mSetupGuid = SETUP_GUID;
    SETUP_DATA SetupData;
    UINTN SetupSize = sizeof(SetupData);
#if B2_WORKAROUND_SUPPORT
    PCH_TCO_SMI_DISPATCH_PROTOCOL           *PchTcoSmiDispatchProtocol;
#endif //B2_WORKAROUND_SUPPORT

    Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid,
                                       NULL,
                                       &pEfiSmmSxDispatch2Protocol);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    if (!EFI_ERROR(Status)) {
#if defined A_S1 && A_S1 == 0x01
        SxDispatchContext.Type  = SxS1;
        SxDispatchContext.Phase = SxEntry;     
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       SuperMS1SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);  
        if (EFI_ERROR(Status)) return EFI_SUCCESS;        
#endif  // #if defined A_S1 && A_S1 == 0x01

#if defined A_S3 && A_S3 == 0x01
        SxDispatchContext.Type  = SxS3;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       SuperMS3SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);  
        if (EFI_ERROR(Status)) return EFI_SUCCESS;
#endif  // #if defined A_S3 && A_S3 == 0x01            
          
#if defined A_S4 && A_S4 == 0x01
        SxDispatchContext.Type  = SxS4;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       SuperMS4SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle);
        if (EFI_ERROR(Status)) return EFI_SUCCESS;        
#endif  // #if defined A_S1 && A_S1 == 0x01        

        SxDispatchContext.Type  = SxS5;
        SxDispatchContext.Phase = SxEntry;        
        Status = pEfiSmmSxDispatch2Protocol->Register (pEfiSmmSxDispatch2Protocol,
                                                       SuperMS5SmiHandler,
                                                       &SxDispatchContext,
                                                       &Handle); 
        if (EFI_ERROR(Status)) return Status;        
    }

    Status = gSmst->SmmLocateProtocol (&gEfiSmmPowerButtonDispatch2ProtocolGuid,
                                       NULL,
                                       &pEfiSmmPowerButtonDispatch2Protocol);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    Status = pRS->GetVariable(
        L"Setup",
        &mSetupGuid,
        NULL,
        &SetupSize,
        &SetupData
        );
    if(!EFI_ERROR(Status)){

        gSmcPwrBtnFn = SetupData.SmcPwrBtnFn;
    }

    Status = pEfiSmmPowerButtonDispatch2Protocol->Register (pEfiSmmPowerButtonDispatch2Protocol,
                                                            SuperMPowerButtonSmiHandler,
                                                            &Context,
                                                            &Handle);
    if (EFI_ERROR(Status)) return EFI_SUCCESS;

    //Register call backs
#if B2_WORKAROUND_SUPPORT
    //
    // Installs TCO SMI event call back to reset system if system hangs up.
    //
    PchTcoSmiDispatchProtocol = NULL;
    Status = gSmst->SmmLocateProtocol (
                      &gPchTcoSmiDispatchProtocolGuid,
                      NULL,
                      (VOID **) &PchTcoSmiDispatchProtocol
                      );
    if (!EFI_ERROR (Status)) {
      Handle = NULL;
      Status = PchTcoSmiDispatchProtocol->TcoTimeoutRegister (
                                            PchTcoSmiDispatchProtocol,
                                            TcoSmiCallback,
                                            Handle
                                            );
      if (EFI_ERROR(Status)) {
        return EFI_SUCCESS;
      }
    }
#endif //B2_WORKAROUND_SUPPORT
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

EFI_STATUS SuperMSmiDriverInit (
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
