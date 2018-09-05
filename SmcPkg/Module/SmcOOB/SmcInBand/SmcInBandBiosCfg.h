//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.08
//    Bug Fix:  
//    Reason: Move VarGuid to platformPolicyDxe.
//    Auditor:  Durant Lin
//    Date:     Aug/07/2018
//
//  Rev. 1.07
//    Bug Fix:  Hide unused forms to fix sum default value errors.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Dec/19/2016
//
//  Rev. 1.06
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.05
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.04
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.03
//    Bug Fix:  Extend FileSize in InBand header.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/22/2016
//
//  Rev. 1.02
//    Bug Fix:  Add InBand HII data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/21/2016
//
//  Rev. 1.01
//    Bug Fix:  Add BIOS config InBand function for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/18/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/31/2016
//
//****************************************************************************

#ifndef	_H_SmcInBandBiosCfg_
#define	_H_SmcInBandBiosCfg_

#include "Token.h"
#include <Uefi.h>
#include <AmiLib.h>

#pragma pack(1)

//<AMI_THDR_START>
//----------------------------------------------------------------------------
// Name: InBand_Smbios_Data_Header
//
// Description: Structure for InBand SMbios Data header.
//
// Fields: Name             Type                    Description
//----------------------------------------------------------------------------
// "Signature[4]"           UINT8                   Signature - "_BIN"
// Version                  UINT16                  Version
// FileSize                 UINT32                  File size
// Checksum                 UINT8                   Checksum (exclude header)
// Flag                     UINT8                   bit 0 -->1 : Data come from SW or BMC
//                                                  bit 1 -->1 : BIOS config load default
// "Reserve[6]"             UINT8                   Reserve area
//----------------------------------------------------------------------------
//<AMI_THDR_END>

typedef struct {
    UINT8	Signature[4];
    UINT16	Version;
    UINT32	FileSize;	//excluding header
    UINT8	Checksum;	//excluding header
    UINT8	Flag;
    UINT8	Reserve[6];
}InBand_BIOS_Cfg_Header;


typedef struct
{
    UINT32 signature;
    UINT16 size;
    UINT32 next: 24;
    UINT32 flags: 8;
    UINT8 guid;
    CHAR8 name[1];
} NVAR;

#pragma pack()

EFI_STATUS
CombineBiosCfg(
    IN OUT	UINT8	*TotalDataBuffer,
    IN OUT	UINT32	*TotalDataBufferPtr
);

EFI_STATUS
InBandUpdateBiosCfg(VOID);

#endif
