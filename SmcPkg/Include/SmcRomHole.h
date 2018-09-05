//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  File History
//
//  Rev. 1.04
//    Bug Fix:  Support SmcRomHole preserve in IPMI.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Jan/06/2017
//
//  Rev. 1.03
//    Bug Fix:  Move $SMC to 1st. 4K in ROMHOLE.
//    Reason:   
//    Auditor:  Kasber Chen
//    Date:     Sep/01/2016
//
//  Rev. 1.02
//    Bug Fix:  Re-write SmcOutBand module.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/02/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix system hangs at Sum InBand command.
//    Reason:   Because of using Boot Service in SMM. Separate SmcRomHole into two libraries - DXE and SMM.
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/12/2016
//
//  Rev. 1.00
//    Bug Fix:  Renew the SMC ROM Hole.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/04/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcRomHole.h
//
// Description: OOB library header file.
//
//<AMI_FHDR_END>
//**********************************************************************
#ifndef	_SMCROMHOLE_H_
#define	_SMCROMHOLE_H_

#include "token.h"
#include <Protocol/FlashProtocol.h>

#ifndef offsetof
#define offsetof(st, m)	((UINTN)((char*)&((st*)0)->m - (char*)0))
#endif

typedef enum{
    FULL_REGION = 0x00,
    OA3_REGION = 0x01,
    OA2_REGION,
    SMBIOS_FOR_BMC_REGION,
    AFU_CMD_REGION,
    INBAND_REGION,
    CURRENT_SMBIOS_REGION,
    PREVIOUS_SMBIOS_REGION
}SMC_ROMHOLE_MEMBER;

#pragma pack(1)

typedef struct{
    UINT8	OA3		[2  * 1024];
    UINT8	OA2		[2  * 1024];
    UINT8	SmBiosForBmc	[4  * 1024];
    UINT8	AfuCMD		[4  * 1024];
    UINT8	SmcInBand	[12 * 1024];
    UINT8	CurrentSmBios	[4  * 1024];
    UINT8	PreviousSmbios	[4  * 1024];
}SMC_ROM_HOLE_MAP;

#pragma pack()

UINT32
SmcRomHoleSize(
    IN	UINT8	Operation
);

UINT32
SmcRomHoleOffset(
    IN	UINT8	Operation
);

EFI_STATUS
SmcRomHoleReadRegion(
    IN	UINT8	Operation,
    OUT	UINT8	*pBuffer
);

EFI_STATUS
SmcRomHoleWriteRegion(
    IN	UINT8	Operation,
    IN	UINT8	*pBuffer
);

//
// Please use "AllocateBuffer" to alocate memory and use "FreeBuffer" to free memory.
// Separate SmcRomHole into two libraries - DXE and SMM.
// In DXE, "AllocateBuffer" uses "gBS->AllocatePool" to allocate memory.
// In SMM, "AllocateBuffer" uses "pSmst->SmmAllocatePool" to allocate memory.
// "FreeBuffer" is the same.
//
EFI_STATUS AllocateBuffer (VOID** Buffer, UINTN BufferSize);
EFI_STATUS FreeBuffer (UINT8* Buffer);


#endif // _SMCROMHOLE_H_
