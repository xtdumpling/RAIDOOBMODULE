//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//  File History
//
//    Rev. 1.00
//    Reason:	Initialize revision.
//    Auditro:	Kasber Chen
//    Date:	10/03/2016
//
//****************************************************************************
#ifndef	_H_SmcAOC_
#define	_H_SmcAOC_

#include "SmcAOCCommon.h"

#define EFI_SMC_AOC_DXE_PROTOCOL_GUID \
	{0xfc3ba688, 0xf207, 0x4d81, 0x97, 0xe5, 0xb0, 0x7d, 0x18, 0x3c, 0x45, 0xce}

typedef EFI_STATUS (EFIAPI *DXEAOCPCIEBIFURCATION)(IN OUT IIO_CONFIG *SetupData,
        IN UINT8 DefaultIOU0[], IN UINT8 DefaultIOU1[], IN UINT8 DefaultIOU[]);

typedef struct{
    DXEAOCPCIEBIFURCATION	SmcAOCDxeBifurcation;
}DXE_SMC_AOC_PROTOCOL;

#endif
