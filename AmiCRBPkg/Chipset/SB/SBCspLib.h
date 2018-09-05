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
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        SBCspLib.h
//
// Description: 
//
// Notes:       
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef __SBLIB_H__
#define __SBLIB_H__

#include <efi.h>
#include <token.h>
#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\BootScriptSave.h>
                                        // [EIP21061]>
#include <Protocol\PciIo.h>
#include <Protocol\AmiSio.h>
                                        // <[EIP21061]

                                        // (P042709A)>
//#include <Include\CmosAccess.h>
#if CMOS_MANAGER_SUPPORT
#include <CmosAccess.h>
#endif
                                        // <(P042709A)

#if CSM_SUPPORT
#include <Protocol\LegacyInterrupt.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CSM_SUPPORT
UINT8
SBGen_GetPIRQIndex (
  UINT8 PIRQRegister);
#endif

BOOLEAN SbLib_GetSmiState(VOID);
VOID SbLib_SmiDisable(VOID);
VOID SbLib_SmiEnable(VOID);
VOID SbDelayUs(IN UINT16 wCount);       // (P052209B)

                                        // (P121409A)>
#if SMM_SUPPORT                         // (P062409B)>
EFI_STATUS SbSmmSaveRestoreStates(IN BOOLEAN Save);
#endif                                  // <(P062409B)

                                       // [EIP21061]>
EFI_STATUS SbLib_SetLpcDeviceDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type
);

EFI_STATUS SbLib_SetLpcGenericDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable
);
                                        // <[EIP21061]

                                        // (P042709A)>
#if CMOS_MANAGER_SUPPORT
EFI_STATUS ReadWriteCmosBank2 (
    IN EFI_PEI_SERVICES         **PeiServices,  // NULL in DXE phase
    IN CMOS_ACCESS_TYPE         AccessType,
    IN UINT16                   CmosRegister,
    IN OUT UINT8                *CmosParameterValue
);
#endif
                                        // <(P042709A)
#ifdef __cplusplus
}
#endif
#endif


#ifndef _SB_ICSPLIB_H_
#define _SB_ICSPLIB_H_
//-----------------------------------------------------------------------
//
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

#ifndef __CSP_LIB__
#define __CSP_LIB__

VOID    CPULib_DisableInterrupt();
VOID    CPULib_EnableInterrupt();

#endif //#ifndef __CSP_LIB__

BOOLEAN IsTopSwapOn(
    VOID
);

VOID SetTopSwap(
    IN BOOLEAN On
);
// ---------------
//#endif

#endif
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
