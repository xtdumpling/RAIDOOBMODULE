//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
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
// $Header: /Alaska/BIN/Board/OEMPort.c 26    8/08/11 11:10a Artems $
//
// $Revision: 26 $
//
// $Date: 8/08/11 11:10a $
//*****************************************************************************

//<AMI_FHDR_START>
//============================================================================
// Name:        OEMPort.C
//
// Description: This file contains code for OEM related code
//============================================================================
//<AMI_FHDR_END>



//============================================================================
// Includes

#include <Token.h>
#include <Efi.h>
#include <Pei.h>
#include <AmiPeiLib.h>
#include <AmiDxeLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Setup.h>

//============================================================================
// GUID Definitions
//static EFI_GUID gPeiStallPpiGuid = EFI_PEI_STALL_PPI_GUID;

//============================================================================
// Type definitions
typedef BOOLEAN (ONE_PARAM_FUNCTION)(
    IN EFI_PEI_SERVICES **PeiServices
);

//============================================================================
// externs
extern ONE_PARAM_FUNCTION* IsRecoveryList[];

//============================================================================
// Function Definitions

//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure: CallOneParamHooks
//
// Description: 
//  This function calls a list of function defined at build time and 
//  return whether the functions completed successfully or not
//
// Input:
//  IN EFI_PEI_SERVICES **PeiServices - pointer to the PeiServices Table
//
// Output:
//  BOOLEAN 
//  - - TRUE - all functions returned correctly
//  - - FALSE - one of the functions failed during execution
//
// Notes: 
//  Similar to CallTwoParamHooks except that these function only require
//  the PeiServices pointer as a parameter
//          
//**********************************************************************
//<AMI_PHDR_END>
BOOLEAN CallOneParamHooks(
    IN ONE_PARAM_FUNCTION* FunctionList[], IN EFI_PEI_SERVICES **PeiServices
){
    UINTN i;
    BOOLEAN Result = FALSE;
    for(i=0; FunctionList[i] && !Result; i++) 
        Result = FunctionList[i](PeiServices);
    return Result;
}

#if KBC_SUPPORT
//<AMI_PHDR_START>
//**********************************************************************
//
// Procedure: IsCtrlHomePressed
//
// Description: 
//  This function detects if the Ctrl + Home key press has been made.
//  Is function is only included if KBC_SUPPORT is enabled.  It enables
//  the keyboard checks for the Ctrl + Home key press and then disables
//  the keyboard
//
// Input:
//  IN EFI_PEI_SERVICES **PeiServices - pointer to the PeiServices Table
//
// Output:
//  BOOLEAN 
//  - - TRUE - Ctrl + Home was pressed
//  - - FALSE - Ctrl + Home not detected
//
// Notes:
//  
//**********************************************************************
//<AMI_PHDR_END>
/*
BOOLEAN IsCtrlHomePressed( EFI_PEI_SERVICES **PeiServices){
    UINT8               Index;
    UINT8               KeyValue = 0;
    UINT8               KeyScan = 0;
    UINT8               LoopCount = 20;
    EFI_STATUS          Status;
    EFI_PEI_STALL_PPI   *StallPpi;
    UINT8               KbcSts = 0;
    UINT8               KbcSts1 = 0;
    UINT8               KbcSts2 = 0;
    BOOLEAN             KbcOBF = TRUE;
    BOOLEAN             KbcIBF = TRUE;
    BOOLEAN             KbcIBF1 = TRUE;
    UINT8               KbcRes = 0;
    EFI_PEI_PCI_CFG2_PPI   *PciCfg = (*PeiServices)->PciCfg;
    EFI_PEI_CPU_IO_PPI    *CpuIo = (*PeiServices)->CpuIo;

    Status = (**PeiServices).LocatePpi (PeiServices, &gPeiStallPpiGuid, 0,
                        NULL, (VOID **)&StallPpi);
    if (EFI_ERROR(Status)) return FALSE;

    //Enable KBD
    KbcSts = CpuIo->IoRead8 ( PeiServices, CpuIo, 0x64 );
    // Check KeyBoard Presence 
    if(KbcSts != 0xFF) {
        //Save the Sys Bit for later use  
        KbcRes = KbcSts & 0x04;
        CpuIo->IoWrite8 ( PeiServices, CpuIo, 0x64, 0x60 );  
                    
        // Wait for Input Buffer Empty  
        while(KbcIBF){
            KbcIBF = FALSE;
            KbcSts1 = CpuIo->IoRead8 ( PeiServices, CpuIo, 0x64 );
            if((KbcSts1 & 0x02) == 02) {
                KbcIBF = TRUE;
            }
        }
        // Keyboard enable, Mouse disable
        CpuIo->IoWrite8 ( PeiServices, CpuIo, 0x60, KbcRes | 0x61 );          
      
        // Wait for Input Buffer Empty 
        while(KbcIBF1){
            KbcIBF1 = FALSE;
            KbcSts2 = CpuIo->IoRead8 ( PeiServices, CpuIo, 0x64 );
            if((KbcSts2 & 0x02) == 02) {
                KbcIBF1 = TRUE;
            }
        } 
    }  

    // Loop for Key scan
    for (Index = 0 ; Index < LoopCount ; Index++) {
        // Wait atleast for 2ms
        StallPpi->Stall( PeiServices, StallPpi, 6000); 
        // Read Key stroke
        if ((CpuIo->IoRead8 ( PeiServices, CpuIo, 0x64 )) & 0x01) {
            KeyValue = (CpuIo->IoRead8 ( PeiServices, CpuIo, 0x60 ));
        }
        // Check for valid key
        if (KeyValue == 0x1d){
            KeyScan |= (1 << 0); // Set BIT0 if it is CTRL key
        } else 
            if (KeyValue == 0x47){
                KeyScan |= (1 << 1); // Set BIT1 if it is HOME key 
            }
        // Check for CTRL_HOME Combination
        if ((KeyScan & 0x03) == 0x03) {
            // Disable KeyBoard Controller
            CpuIo->IoWrite8 ( PeiServices, CpuIo, 0x64, 0x60 );
            CpuIo->IoWrite8 ( PeiServices, CpuIo, 0x60, 0x30 );
            return TRUE;
        }
    }
    // Disable KeyBoard Controller
    CpuIo->IoWrite8 ( PeiServices, CpuIo, 0x64, 0x60 );
    CpuIo->IoWrite8 ( PeiServices, CpuIo, 0x60, 0x30 );

    return FALSE; // No valid key is pressed
}
*/
#endif        // KBC_SUPPORT

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: IsRecovery
//
// Description: 
//  This function determines if the system is supposed to boot in recovery mode
//
// Input:   
//  **PeiServices - pointer to the Pei Services function and data structure
//
// Output:    
//  TRUE - The system is booting in recovery mode
//  FALSE - Normal Boot
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN 
IsRecovery (
    EFI_PEI_SERVICES    **PeiServices,
    EFI_PEI_PCI_CFG2_PPI   *PciCfg,
    EFI_PEI_CPU_IO_PPI    *CpuIo
)
{
    BOOLEAN IsRecovery;
    EFI_BOOT_MODE BootMode;

    // If we already in recovery mode, just return TRUE
    if (  !EFI_ERROR((*PeiServices)->GetBootMode((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode))
        && BootMode == BOOT_IN_RECOVERY_MODE
    ) return TRUE;

    IsRecovery=CallOneParamHooks(IsRecoveryList, PeiServices);
 
    //You can use IsRecovery eLink to install customer handlers
    //or just add the code here
    //Check recovery jumper
    //IsRecovery = (IoRead16(GPIO_BASE_ADDRESS + ...) & 0x...);

    if (IsRecovery) PEI_PROGRESS_CODE((CONST EFI_PEI_SERVICES **)PeiServices,PEI_RECOVERY_USER);
    return IsRecovery;
}

static EFI_GUID SetupVariableGuid = SETUP_GUID;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: IsNvramDataCompatible
//
// Description: 
//  This function determines if the current NVRAM data 
//  (mainly Setup-related variables) is compatible with the current firmware.
//
// Input:   
//  **PeiServices - pointer to a pointer to the PEI Services Table. 
//  *ReadVariablePpi - pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. 
//                     The pointer can be used to read and enumerate existing 
//                     NVRAM variables.
//
// Output:    
//  TRUE - the NVRAM data is compatible
//  FALSE - the NVRAM data is not compatible
//
// Notes: 
//  This function is called by NVRAM PEIM. 
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN IsNvramDataCompatible(
    IN EFI_PEI_SERVICES **PeiServices,
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
    UINTN Size = 0;
    EFI_STATUS Status;

    Status=ReadVariablePpi->GetVariable(
        ReadVariablePpi, L"Setup",
        &SetupVariableGuid, NULL,
        &Size, NULL
    );
    if (Status!=EFI_BUFFER_TOO_SMALL) return TRUE;
    return Size==sizeof(SETUP_DATA);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: DxeIsNvramDataCompatible
//
// Description: 
//  This function determines if the current NVRAM data 
//  (mainly Setup-related variables) is compatible with the current firmware.
//  This function is called by NVRAM Driver. 
//
// Input:       
//  GetVariable - pointer to EFI_GET_VARIABLE function. The pointer can be used 
//                to read existing NVRAM variables.
//
// Output:    
//  TRUE - the NVRAM data is compatible
//  FALSE - the NVRAM data is not compatible
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
BOOLEAN DxeIsNvramDataCompatible(
    IN EFI_GET_VARIABLE GetVariable
){
    UINTN Size = 0;
    EFI_STATUS Status;

    Status=GetVariable(
        L"Setup",
        &SetupVariableGuid, NULL,
        &Size, NULL
    );
    if (Status!=EFI_BUFFER_TOO_SMALL) return TRUE;
    return Size==sizeof(SETUP_DATA);
}
//IEISugarBay.c
BOOLEAN ForceRecovery(EFI_PEI_SERVICES **PeiServices){
    return FORCE_RECOVERY;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
