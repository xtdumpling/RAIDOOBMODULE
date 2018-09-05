
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name: SBPeiDebugger.C
//
// Description: This file contains PEI stage debugger code for SB template
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <Efi.h>
#include <AmiLib.h>
#include <AmiCspLib.h>
#include <AmiDebugPort.h>
#include "token.h"

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: SBPEIDBG_Initialize
//
// Description: This eLink function is used to initialize the South Bridge
//    for PEI Debugger support
//
// Input:  DebugPort  Debug transport interface structure
//
// Output:  EFI_STATUS
//
// Notes:  Normally no PORTING REQUIRED
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SBPEIDBG_Initialize (
 IN OUT PEI_DBG_PORT_INFO *DebugPort
)
{
 
 return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: FindCapPtrDbg
//
// Description: This function searches the PCI address space for the PCI
//              device specified for a particular capability ID and returns
//              the offset in the PCI address space if one found
//
// Input:  Bus      PCI Bus number
//         DevFunc  PCI Device and function number
//         CapId    Capability ID to look for
//
// Output:  Capability ID location if one found
//          Otherwise returns 0
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 
FindCapPtrDbg(
 IN UINT8 Bus,
 IN UINT8 DevFunc,
 IN UINT8 CapId
)
{
 // Porting for lewisburg PCH
 return 0;          //No device.
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
