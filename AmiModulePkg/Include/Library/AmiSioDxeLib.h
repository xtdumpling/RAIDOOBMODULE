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
//**********************************************************************

/** @file 
  @brief
    Definitions of the AMI SIO DXE Library.

**/

#ifndef __AMI_SIO_DXE_LIB_H__
#define __AMI_SIO_DXE_LIB_H__

//-------------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------------
#include <Efi.h>
#include <AmiGenericSio.h>
#include <AcpiRes.h>

//-------------------------------------------------------------------------
// Constants, Macros and Type Definitions
//-------------------------------------------------------------------------
typedef struct _IO_DECODE_DATA{
    UINT16          BaseAdd;
    //!!!Attention!!!If type is 0xFF, UID is a IO legth
    UINT8           UID;
    SIO_DEV_TYPE    Type;
} IO_DECODE_DATA;

typedef struct _SIO_DEVICE_INIT_DATA{
    UINT16      Reg16;
    UINT8       AndData8;   // 0x00 means register don't need AndMask
                            // only write OrData8 to regisrer.
    UINT8       OrData8;
} SIO_DEVICE_INIT_DATA;

typedef struct _DXE_DEVICE_INIT_DATA{
    UINT8       Reg8;
    UINT8       AndData8;   // 0x00 means register don't need AndMask
                            // only write OrData8 to regisrer.
    UINT8       OrData8;
} DXE_DEVICE_INIT_DATA;

typedef struct _SIO_DATA{
    UINT16           Addr;
    //AND mask value, 0xFF means register don't need AndMask and 
    //only write OrData8 to regisrer.
    UINT8           DataMask; 
    //OR mask value.  
    UINT8           DataValue;
} SIO_DATA;

// SIO DECODE list creation code must be in this order
typedef EFI_STATUS (IO_RANGE_DECODE)(
    IN  VOID            *LpcPciIo,
    IN  UINT16          DevBase, 
    IN  UINT8           UID, 
    IN  SIO_DEV_TYPE    Type
); 

//-------------------------------------------------------------------------
//Variable, Prototype, and External Declarations
//-------------------------------------------------------------------------

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: GetPrsFromTable
//
// Description:
//This function will get _PRS from mSpioDeviceList[] table
//"Index" is the entry # in mSpioDeviceList[]
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS GetPrsFromTable(SIO_DEV2* Dev, UINTN Index);


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: GetSioLdVarName
//
// Description:
//This function generate Variable Name associated with each SIO Logical Device.
//"Volatile" or None "Volatile"
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STRING GetSioLdVarName(SIO_DEV2* SioLd, BOOLEAN Volatile);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
// Procedure: Set***Prs
//
// Description:
// These functions will Create Possible Resource Settings Buffer for ***
// SIO Logical Device
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
//
EFI_STATUS SioDxeLibSetFdcPrs(SIO_DEV2 *Dev);

EFI_STATUS SioDxeLibSetPs2kPrs(SIO_DEV2 *Dev);

EFI_STATUS SioDxeLibSetPs2mPrs(SIO_DEV2 *Dev);

EFI_STATUS SioDxeLibSetUartPrs(SIO_DEV2 *Dev);

EFI_STATUS SioDxeLibSetLptPrs(SIO_DEV2 *Dev, BOOLEAN UseDma);

EFI_STATUS SioDxeLibSetGamePrs(SIO_DEV2 *Dev);

EFI_STATUS SioDxeLibSetMpu401Prs(SIO_DEV2 *Dev);


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: SioDxeLibGetPrsFromAml
//
// Description:
//These function will collect _PRS from Aml code which support ACPI
//"Index" is the entry # in mSpioDeviceList[]
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS SioDxeLibGetPrsFromAml(SIO_DEV2* Dev, UINT8 *PrsName ,UINTN Index);


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AmiSioLibSetLpcDeviceDecoding
//
// Description: This function sets LPC Bridge Device Decoding
//
// Input:       *LpcPciIo - Pointer to LPC PCI IO Protocol(NULL in PEI)
//              Base      - I/O base address, if Base is 0 means disabled the
//                          decode of the device
//              DevUid    - The device Unique ID
//              Type      - Device Type, please refer to AMISIO.h
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Set successfully.
//                  EFI_UNSUPPORTED - There is not proper Device Decoding
//                                    register for the device UID.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiSioLibSetLpcDeviceDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT8                    DevUid,
    IN SIO_DEV_TYPE             Type );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   AmiSioLibSetLpcGenericDecoding
//
// Description: This function set LPC Bridge Generic Decoding
//
// Input:       *LpcPciIo - Pointer to LPC PCI IO Protocol
//              Base      - I/O base address
//              Length    - I/O Length
//              Enabled   - Enable/Disable the generic decode range register
//
// Output:      EFI_STATUS
//                  EFI_SUCCESS - Set successfully.
//                  EFI_UNSUPPORTED - This function is not implemented or the
//                                    Length more than the maximum supported
//                                    size of generic range decoding.
//                  EFI_INVALID_PARAMETER - the Input parameter is invalid.
//                  EFI_OUT_OF_RESOURCES - There is not available Generic
//                                         Decoding Register.
//                  EFI_NOT_FOUND - the generic decode range will be disabled
//                                  is not found.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS AmiSioLibSetLpcGenericDecoding (
    IN VOID      				*LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable );

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: SioLibLaunchInitRoutine
//
// Description:
//  This is a routine to launch Init Function
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS SioLibLaunchInitRoutine(SIO_DEV2 *Device, SIO_INIT_STEP InitStep);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: SioLibLaunchResourceRoutine
//
// Description:
//  This is a routine to launch SetResource Function for Not standard SIO Devices.
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS SioLibLaunchResourceRoutine(SIO_DEV2 *Device, SIO_RESOURCE_REG_TYPE RegType);

// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: LaunchCompRoutine
//
// Description:
//  This is a routine to launch Init Function from Compatibility Module.
//
// Input:
//
// Output:
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS EFIAPI SioLibLaunchCompRoutine(SIO_DEV2 *Device, SIO_INIT_STEP InitStep);

void EFIAPI GetValueWithIOx (
	IN UINT16 IndexP,
	IN UINT16 DataP,
	IN UINT8  BankReg,
    IN UINT8  BankNo,
    IN UINT8  LpcReg,
    OUT UINTN *Value
);

void EFIAPI ProgramIsaRegisterTable(
    IN  UINT16  Index,
    IN  UINT16  Data,
    IN  SIO_DEVICE_INIT_DATA *Table,
    IN  UINT8   Count
);

void EFIAPI ProgramRtRegisterTable(
    IN  UINT16  Base,
    IN  SIO_DEVICE_INIT_DATA  *Table,
    IN  UINT8   Count
);

VOID EFIAPI SioLib_BootScriptIO(
		UINT16 Reg16,
		UINT8 Data8,
		EFI_S3_SAVE_STATE_PROTOCOL *SaveState
		);

VOID EFIAPI SioLib_BootscriptLdnSel(
		UINT16 idxReg,
		UINT16 dataReg,
		UINT8  LdnSel,
		UINT8  Ldn,
		EFI_S3_SAVE_STATE_PROTOCOL *SaveState
		);

VOID EFIAPI SioLib_BootScriptSioS3SaveTable(
		UINT16 idxReg,
		UINT16 dataReg,
		SIO_DEVICE_INIT_DATA  *Table,
		UINT8 Count,
		EFI_S3_SAVE_STATE_PROTOCOL *SaveState
		);

VOID EFIAPI SioLib_BootScriptRTS3SaveTable(
		UINT16 Base,
		SIO_DEVICE_INIT_DATA  *Table,
		UINT8 Count,
		EFI_S3_SAVE_STATE_PROTOCOL *SaveState
		);


#endif //__AMI_SIO_DXE_LIB_H__
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2009, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

