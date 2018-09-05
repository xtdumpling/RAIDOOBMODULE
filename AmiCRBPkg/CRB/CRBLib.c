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

/** @file CRBLib.c
    This file contains Chipset Reference Board related code that is needed for both PEI & DXE stage.
    To avoid code duplication this file is made as a library and linked both in PEI & DXE CRB FFS.
    Note: MAKE SURE NO PEI OR DXE SPECIFIC CODE IS NEEDED
*/

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------
#include <CRBLib.h>
#include "IioUniversalData.h"
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MmPciBaseLib.h>
#include <AmiPcieSegBusLib/AmiPcieSegBusLib.h>
#include <Library/PcdLib.h> 
#include "Token.h"

#include <Include/SetupLibInternal.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Build/GenericSetupDataDefinition.h>

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: NbRuntimeShadowRamWrite
//
// Description: This function provides runtime interface to enable/disable
//              writing in E000-F000 segment
//
// Input:       BOOLEAN
//              TRUE - Enable writing
//              FALSE - Disable writing
//
// Output:      VOID
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID 
NbRuntimeShadowRamWrite (
  IN BOOLEAN             Enable 
  )
{
  EFI_STATUS              Status;
  UINT8                   StackSeg;
  UINT8                   StackBus;
  UINT8                   SocId;
  UINTN                   Address;
  static UINT32           PAM0123Reg[MAX_SOCKET];
  static UINT32           PAM456Reg[MAX_SOCKET];
  static BOOLEAN          PamRwEnabled = FALSE;

  for (SocId = 0; SocId < MAX_SOCKET; SocId++) {
      
    //Check Socket/Stack valid bit. 
    if (!AmiIsStackValid (SocId, IIO_PSTACK0))
      continue;
      
    Status = AmiGetPcieSegmentBus (SocId, IIO_PSTACK0, &StackSeg, &StackBus);
    
    DEBUG((EFI_D_INFO, "AmiGetPcieSegmentBus - Status = %r StackSeg : %x, StackBus = %x\n", Status, StackSeg, StackBus));
    

    if (EFI_ERROR(Status))
      continue;
     
    Address = MmPciAddress (StackSeg, StackBus, PCI_SAD_ALL_DEVICE_NUMBER, PCI_SAD_ALL_FUNC_NUMBER, 0x00);
    
    DEBUG((EFI_D_INFO, "MmPciAddress - Address = %x\n", Address));
    
    //check PAM Lock bit. If the PAM is locked, we cannot modify the PAM attributes.
    //The lock is bypassed by SMM writes.
    //if (MmioRead32 (Address | R_PCI_SAD_ALL_PAM0123) & BIT0)
      //break;
    
    if (Enable && !PamRwEnabled) {   

      //Preserve PAM register value to restore.
      PAM0123Reg[SocId] = MmioRead32 (Address + R_PCI_SAD_ALL_PAM0123);
      PAM456Reg[SocId] = MmioRead32 (Address + R_PCI_SAD_ALL_PAM456);
      
      DEBUG((EFI_D_INFO, "MmioRead32 PAM0123Reg[%d] = %x PAM456Reg[%d] = %x\n",SocId, PAM0123Reg[SocId], SocId, PAM456Reg[SocId]));
      //0F0000-0FFFFF Attribute - Normal DRAM Operation: All reads and writes are serviced by DRAM
      MmioWrite32 ((Address + R_PCI_SAD_ALL_PAM0123), (PAM0123Reg[SocId] | PAM0123_DRAM_RW_ENABLE));
        
      //0D8000-0DBFFF / 0DC000-0DFFFF / 0E0000-0E3FFF / 0E4000-0E7FFF / 0E8000-0EBFFF / 0EC000-0EFFFF Attribute - Normal DRAM Operation: All reads and writes are serviced by DRAM
      MmioWrite32 ((Address + R_PCI_SAD_ALL_PAM456), (PAM456Reg[SocId] | PAM456_DRAM_RW_ENABLE));
        
    } else if (!Enable && PamRwEnabled){
      DEBUG((EFI_D_INFO, "MmioWrite32 PAM0123Reg[%d] = %x PAM456Reg[%d] = %x\n",SocId, PAM0123Reg[SocId], SocId, PAM456Reg[SocId]));
	  //Restore PAM register
      //0F0000-0FFFFF Attribute - DRAM Disabled: All accesses are directed to DMI
      MmioWrite32 ((Address + R_PCI_SAD_ALL_PAM0123), PAM0123Reg[SocId]);
        
      //0E0000-0E3FFF / 0E4000-0E7FFF / 0E8000-0EBFFF / 0EC000-0EFFFF Attribute - DRAM Disabled: All accesses are directed to DMI
      MmioWrite32 ((Address + R_PCI_SAD_ALL_PAM456), PAM456Reg[SocId]);
    } 
  }
  
  PamRwEnabled = Enable;
  
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: OemRuntimeShadowRamWrite
//
// Description: This function calls NbRuntimeShadowRamWrite() method to enable/disable
//              writing in E000-F000 segment
//
// Input:       BOOLEAN
//              TRUE - Enable writing
//              FALSE - Disable writing
//
// Output:      VOID
//---------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID OemRuntimeShadowRamWrite(
    IN BOOLEAN Enable
)
{
    NbRuntimeShadowRamWrite(Enable);
}

#if SMBIOS_SUPPORT && SB_WAKEUP_TYPE_FN
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: getWakeupTypeForSmbios
//
// Description: Detect and return SMBIOS wake-up type
//
// Input:  None
//
// Output: 0x01 = Other
//         0x02 = Unknown
//         0x03 = APM Timer
//         0x04 = Modem Ring
//         0x05 = LAN Remote
//         0x06 = Power Switch
//         0x07 = PCI PME#
//         0x08 = AC Power Retored
//
// Notes: Porting required 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8
getWakeupTypeForSmbios (VOID)
{
    return (PcdGet8(PcdWakeupType));
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: CrbIsNvramDataCompatible
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
BOOLEAN CrbIsNvramDataCompatible(
    IN CONST EFI_PEI_SERVICES **PeiServices,   
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi
){
    UINTN Size = 0;
    UINT8 Index = 0;
    EFI_STATUS Status;

    Status=ReadVariablePpi->GetVariable(
        ReadVariablePpi, L"Setup",
        &gEfiSetupVariableGuid, NULL,
        &Size, NULL
    );

    if ( ( Status == EFI_BUFFER_TOO_SMALL ) && ( Size != sizeof(SETUP_DATA) ) ) {
        return FALSE;
    }

    while ( mSetupInfo[Index].GuidValue != NULL ) {

        Size = 0;
        Status = ReadVariablePpi->GetVariable(
                ReadVariablePpi, mSetupInfo[Index].SetupName,
                mSetupInfo[Index].GuidValue, NULL,
                &Size, NULL
        );

        if ( ( Status == EFI_BUFFER_TOO_SMALL ) && ( Size != mSetupInfo[Index].VariableSize ) ) {
            return FALSE;
        }
        Index++;
    }

    return TRUE;

}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: CrbDxeIsNvramDataCompatible
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
BOOLEAN CrbDxeIsNvramDataCompatible(
    IN EFI_GET_VARIABLE GetVariable
){
    UINTN Size = 0;
    UINT8 Index = 0;
    EFI_STATUS Status;

    Status=GetVariable(
        L"Setup",
        &gEfiSetupVariableGuid, NULL,
        &Size, NULL
    );

    if ( ( Status == EFI_BUFFER_TOO_SMALL ) && ( Size != sizeof(SETUP_DATA) ) ) {
        return FALSE;
    }

    while ( mSetupInfo[Index].GuidValue != NULL ) {

        Size = 0;
        Status=GetVariable(
                mSetupInfo[Index].SetupName,
                mSetupInfo[Index].GuidValue, NULL,
                &Size, NULL
        );

        if ( ( Status == EFI_BUFFER_TOO_SMALL ) && ( Size != mSetupInfo[Index].VariableSize ) ) {
            return FALSE;
        }
        Index++;
    }
    
    return TRUE;
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
