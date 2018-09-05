//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Fix booting shell SwSmi problem for automation tool.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/01/2015
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   
//    Auditor:  Salmon Chen
//    Date:     Jan/29/2015
//
//****************************************************************************
//****************************************************************************
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
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcSwSMI.c
//
// Description: This file contains code for SW SMI events
//
//<AMI_FHDR_END>
//*************************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <Token.h>
#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#if FIXED_BOOT_ORDER_SUPPORT
#include "FixedBootOrder\FixedBootOrder.h"
#endif
// Produced Protocols

// Consumed Protocols
//#include <Protocol\SmmBase2.h>
#include <Protocol\SmmSwDispatch2.h>


//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------
// SMM Nvram Control Protocol Definitions
typedef EFI_STATUS (*SHOW_BOOT_TIME_VARIABLES)(BOOLEAN Show);
typedef struct{
    SHOW_BOOT_TIME_VARIABLES ShowBootTimeVariables;
} AMI_NVRAM_CONTROL_PROTOCOL;

AMI_NVRAM_CONTROL_PROTOCOL *gNvramControl = NULL;

UINT8 SMCI_Flag = 0;

AMI_NVRAM_CONTROL_PROTOCOL *LocateNvramControlSmmProtocol(VOID){
    static EFI_GUID gAmiNvramControlProtocolGuid = \
    { 0xf7ca7568, 0x5a09, 0x4d2c, { 0x8a, 0x9b, 0x75, 0x84, 0x68, 0x59, 0x2a, 0xe2 } };
    return GetSmstConfigurationTable(&gAmiNvramControlProtocolGuid);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SmcSwSMIHandler
//
// Description: SMI Handler for boot to UEFI USB/build-in Shell.
//
// Input:       PI 0.91, 1.0
//                  DispatchHandle   - SMI dispatcher handle
//                  *DispatchContext - Pointer to the dispatch context
//              PI 1.1, 1.2
//                  DispatchHandle  - SMI dispatcher handle
//                  *DispatchContext- Points to an optional S/W SMI context
//                  CommBuffer      - Points to the optional communication
//                                    buffer
//                  CommBufferSize  - Points to the size of the optional
//                                    communication buffer
//
// Output:      EFI_STATUS if the new SMM PI is applied.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS SmcSwSMIHandler (
    IN EFI_HANDLE       DispatchHandle,
    IN CONST VOID       *DispatchContext OPTIONAL,
    IN OUT VOID         *CommBuffer OPTIONAL,
    IN OUT UINTN        *CommBufferSize OPTIONAL)
{
    // Porting if needed
    
    UINT8               Boot_Select;
#if FIXED_BOOT_ORDER_SUPPORT
    EFI_STATUS          Status;
    EFI_GUID            SetupGuid = SETUP_GUID;
    SETUP_DATA          SetupData;
    UINTN               VariableSize; 
    UINTN               i, j;
    UINT16              NewBootOrder[50];    
#endif
	
    Boot_Select = IoRead8(0xB3);
    
  if( (SMCI_Flag == 0) && (Boot_Select == 0x53) ) SMCI_Flag = 1; 
  else if( (SMCI_Flag == 1) && (Boot_Select == 0x4D) ) SMCI_Flag = 2;
  else if( (SMCI_Flag == 2) && (Boot_Select == 0x43) ) SMCI_Flag = 3;
  else if( (SMCI_Flag == 3) && (Boot_Select == 0x49) ) SMCI_Flag = 4;
  else if( SMCI_Flag == 4 ) SMCI_Flag = 5;
  else SMCI_Flag = 0;
                 
#if FIXED_BOOT_ORDER_SUPPORT
    if(SMCI_Flag == 5)
    {   
      gNvramControl = LocateNvramControlSmmProtocol();  
      if( gNvramControl ) gNvramControl->ShowBootTimeVariables( TRUE );   
    
      VariableSize = sizeof (SETUP_DATA);
      Status = pRS->GetVariable(L"Setup", &SetupGuid, NULL,
                    &VariableSize, &SetupData);        

      if( Boot_Select == 0xC0 )NewBootOrder[0] = 7;      // Boot to Shell
      else if( Boot_Select == 0xC1 )NewBootOrder[0] = 14;// Boot to PXE 
    
      for(i=0,j=1;j<FIXED_BOOT_ORDER_GROUP_MAX_NUM; i++)
      {
    	   if(NewBootOrder[0]==SetupData.DualPriorities[i])
    	   continue;
    	   NewBootOrder[j]=SetupData.DualPriorities[i];
    	   j++;
      }

      for(i=0;i<FIXED_BOOT_ORDER_GROUP_MAX_NUM;i++)
      {
    	   SetupData.DualPriorities[i]=NewBootOrder[i];
      }    
  
      Status = pRS->SetVariable(L"Setup", 
                           &SetupGuid,
                           EFI_VARIABLE_NON_VOLATILE |
                           EFI_VARIABLE_BOOTSERVICE_ACCESS,
                           sizeof(SETUP_DATA), 
                           &SetupData);
    
      if( gNvramControl ) gNvramControl->ShowBootTimeVariables( FALSE );                           
      SMCI_Flag = 0;                         
    }
#endif
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InSmmFunction
//
// Description: Installs CRB SMM Child Dispatcher Handler.
//
// Input:       ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//                  EFI_NOT_FOUND - The SMM Base protocol is not found.
//                  EFI_SUCCESS   - Installs CRB SMM Child Dispatcher Handler
//                                  successfully.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    EFI_STATUS                    Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL *pSwDispatch;
    EFI_SMM_SW_REGISTER_CONTEXT   SwContext = {BootUEFI_SWSMI};
    EFI_HANDLE                   Handle;

    Status = InitAmiSmmLib (ImageHandle, SystemTable);
    if (EFI_ERROR(Status)) return Status;    

    Status = pSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &pSwDispatch);
    if (!EFI_ERROR(Status)) {
        Status  = pSwDispatch->Register (pSwDispatch, \
                                         SmcSwSMIHandler, \
                                         &SwContext, \
                                         &Handle);
    }

    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitializeSmcSwSMI
//
// Description: Installs Smc SwSMI Child Dispatcher Handler.
//
// Input:       ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//                  EFI_NOT_FOUND - The SMM Base protocol is not found.
//                  EFI_SUCCESS   - Installs CRB SMM Child Dispatcher Handler
//                                  successfully.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS InitializeSmcSwSMI (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
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
