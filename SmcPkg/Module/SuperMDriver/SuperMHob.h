//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2015 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/18/2014
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SuperMHob.h
//
// Description:
//  This file include SuperM Hob data structure
//
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

//#include <Hob.h>
#include <Library\HobLib.h>

#define EFI_SMCBOARDINFO_HOB_GUID \
  { \
     0xa0e5a45f, 0x5374, 0x46b3, 0xb5, 0x1e, 0xd3, 0x10, 0xb2, 0xc4, 0x8a, 0xae \
  }

EFI_GUID gSmcBoardInfoHobGuid = EFI_SMCBOARDINFO_HOB_GUID;

typedef struct _SMCHOBINFO {
	EFI_HOB_GUID_TYPE 	EfiHobGuidType;
	UINT16			MBSVID;
	UINT8  			GPU_AdjustVID_Support;
	UINT8  			BMCCMD70_SetGPU_DID[4][4];
	UINT8   		SystemConfiguration[10];
	UINT8   		HardwareInfo[10];
	UINT8  			BMC_Present;
	UINT32			PCB_Revision;
	UINT8  			SMCCheckTPMPresent;
} SMCHOBINFO;

