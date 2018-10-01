//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix : N/A
//    Reason  : Add Support SMC LSI OOB Module.
//    Auditor : Durant Lin
//    Date    : Sep/28/2018
//
//  Rev. 1.05
//    Bug Fix:  Send all HII data for sum and fix checksum error.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/09/2016
//
//  Rev. 1.04
//    Bug Fix:  Fix some variables didn't be sent to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/26/2016
//
//  Rev. 1.03
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.02
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.01
//    Bug Fix:  Add InBand HII data for sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/21/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     May/31/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcInBand.h
//
// Description: OOB In-Band feature header file.
//
//<AMI_FHDR_END>
//**********************************************************************

#ifndef	_H_SmcInBand_
#define	_H_SmcInBand_

#include "SmcInBandBiosCfg.h"
#include "SmcInBandSmbios.h"
#include "SmcFeatureFlag.h"
#include <Library/BaseMemoryLib.h>

#define	SWSMI_E8_SUBCMD_COMBINE_HII 1
#define	SWSMI_E8_SUBCMD_COMBINE_FULL_SMBIOS 2
#define	SWSMI_E8_SUBCMD_COMBINE_BIOS_CFG 3

//
// OOB SMRAM Map :
//
// HII          384K
// Full SMBIOS  256K
// BIOS Config  384K
//
// Total Size   1024K
//
#define	SMC_OOB_SMRAM_TOTAL_SIZE (1024 * 1024)

#define	SMC_OOB_SMRAM_HII_SIZE (384 * 1024) // 384K
#define	SMC_OOB_SMRAM_FULL_SMBIOS_SIZE (256 * 1024) // 256K
#define	SMC_OOB_SMRAM_BIOS_CFG_SIZE (384 * 1024) // 384K

#define	SMC_OOB_SMRAM_HII_OFFSET 0
#define	SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET (SMC_OOB_SMRAM_HII_OFFSET + SMC_OOB_SMRAM_HII_SIZE)
#define	SMC_OOB_SMRAM_BIOS_CFG_OFFSET (SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET + SMC_OOB_SMRAM_FULL_SMBIOS_SIZE)

#pragma pack(1)

typedef struct{
    UINT8	HeaderLength[2];
    UINT8	SubCommand;
    UINT8	TotalDataSize[4];
    UINT8	FileType;
    UINT8	ReturnStatus;
    UINT8	DataBufferAddress[8];
}InBand_Base_Header;

#pragma pack()

#define FREE_BUFFER(_x) if(_x != NULL) { gBS->FreePool(_x);  }

#endif
