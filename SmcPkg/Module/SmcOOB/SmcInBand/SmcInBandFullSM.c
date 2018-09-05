//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.07
//    Bug Fix:  
//    Reason:   Modify Debug message.
//    Auditor:  Durant Lin
//    Date:     Jun/01/2018
//
//  Rev. 1.06
//    Bug Fix:  Add smbios version for SUM.
//    Reason:   SUM required
//    Auditor:  Kasber Chen
//    Date:     Aug/04/2017
//
//  Rev. 1.05
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.04
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.03
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.02
//    Bug Fix:  Add Full SMBIOS module.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/27/2016
//
//  Rev. 1.01
//    Bug Fix:  Update Feature support Flags code for new spec.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/30/2015
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/31/2015
//
//****************************************************************************

//<AMI_FHDR_START>
//
// Name:  SmcFullSM.c
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************
#include "efi.h"
#include "Token.h"
#include <Protocol\AmiSmbios.h>
#include "SmcInBandFullSM.h"
#include "SmcLib.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>

extern SMC_OOB_PLATFORM_POLICY      *mSmcOobPlatformPolicyPtr;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   CombineFullSmBios
//
// Description: Run at ready to boot.
//
// Input:       IN EFI_EVENT   Event
//              IN VOID        *Context
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID CombineFullSmBios (UINT8* BuffAddress, UINT32* BuffSize)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT32	*pSAddr = (UINT32*)0x000F0000, *pEAddr = (UINT32*)0x000FFFFF;
    UINT8	*pTAddr = NULL;
    UINT16	Length, i;

    DEBUG ((-1, "[SMC_OOB] :: CombineFullSmBios entry.\n"));

    *BuffSize = 0;

    //
    // Search for the SMBIOS table
    //
    for( ; pSAddr <= pEAddr; pSAddr += 4) {
        if(SM_SIGNATURE == *pSAddr) {
            pTAddr = (UINT8*)((SMBIOS_TABLE_ENTRY_POINT*)pSAddr)->TableAddress;
            Length = ((SMBIOS_TABLE_ENTRY_POINT*)pSAddr)->TableLength;
            Status = EFI_SUCCESS;
            break;
        }
        Status = EFI_NOT_FOUND;
    }
    if(EFI_ERROR(Status)) return;

    //
    // Fill the header
    //
    MemCpy (&(((InBand_FullSmBios_Header*)BuffAddress)->Signature[0]), "_SM_", 4);
    ((InBand_FullSmBios_Header*)BuffAddress)->Version = mSmcOobPlatformPolicyPtr->OobConfigurations.FullSmbiosVersion;
    ((InBand_FullSmBios_Header*)BuffAddress)->FileSize = Length;
    ((InBand_FullSmBios_Header*)BuffAddress)->Checksum = 0;
    ((InBand_FullSmBios_Header*)BuffAddress)->SMBIOSMajorVer = mSmcOobPlatformPolicyPtr->OobConfigurations.SmbiosMajorVersion;
    ((InBand_FullSmBios_Header*)BuffAddress)->SMBIOSMinorVer = mSmcOobPlatformPolicyPtr->OobConfigurations.SmbiosMinorVersion;
    ((InBand_FullSmBios_Header*)BuffAddress)->SMBIOSDocrev = mSmcOobPlatformPolicyPtr->OobConfigurations.SmbiosDocVersion;

    *BuffSize = sizeof(InBand_FullSmBios_Header) + Length;

    for(i = 0; i < Length ; i++) {
        ((InBand_FullSmBios_Header*)BuffAddress)->Checksum += *(pTAddr+i);
    }

    //
    // Copy SMBIOS table to output buffer.
    //
    pBS->CopyMem((BuffAddress + sizeof(InBand_FullSmBios_Header)), pTAddr, Length);

    DEBUG ((-1, "[SMC_OOB] :: CombineFullSmBios end.\n"));

    return;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1993-2013, Supermicro Computer, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
