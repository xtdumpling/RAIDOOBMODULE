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

//**********************************************************************
// $Header: /Alaska/SOURCE/CPU/Intel/Nehalem/CpuHobs.h 11    10/09/10 11:29p Markw $
//
// $Revision: 11 $
//
// $Date: 10/09/10 11:29p $
//*****************************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name:		CpuHobs.h
//
// Description:	Collection of CPU Hobs.
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _CPU_HOBS_H__
#define _CPU_HOBS_H__

#include <Pei.h>
#include <Hob.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push,1)

//This HOB is intended to be temporary until a better solution is available.

// {6865C455-8626-40d8-90F4-A69460A4AB5A}
#define DCA_HOB_GUID \
    {0x6865c455, 0x8626, 0x40d8, 0x90, 0xf4, 0xa6, 0x94, 0x60, 0xa4, 0xab, 0x5a}

typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    BOOLEAN Supported;
} DCA_HOB;

/////////////////////////////
////////////////////////////

//This HOB is subject to change at any CPU label and is only for the CPU module.

// {982D8C6F-F6F6-4135-A309-A4593EA56417}
#define AMI_INTERNAL_FACTORY_TDC_TDP_HOB_GUID \
    {0x982d8c6f, 0xf6f6, 0x4135, 0xa3, 0x9, 0xa4, 0x59, 0x3e, 0xa5, 0x64, 0x17}

typedef struct {
    EFI_HOB_GUID_TYPE   EfiHobGuidType;
    BOOLEAN IsSandyBridge;
    UINT8   PowerConv;
    UINT8   CurConv;
    UINT8   TimeConv;
    UINT16  Tdc;
    UINT16  Tdp;                    //For Sandy Bridge, this also POWER_LIMIT_1
    UINT8   TdpLimitTime;           //For Sandy Bridge, this is POWER_LIMIT_1_TIME
    UINT8   Vid;
//Used for PERF_TUNE_SUPPORT Start
    UINT8   OneCoreRatioLimit;
    UINT8   TwoCoreRatioLimit;
    UINT8   ThreeCoreRatioLimit;
    UINT8   FourCoreRatioLimit;
    UINT8   FiveCoreRatioLimit;
    UINT8   SixCoreRatioLimit;
	UINT16	IaCoreCurrentMax;
	UINT16  IGfxCoreCurrentMax;
//Used for PERF_TUNE_SUPPORT End
} AMI_INTERNAL_FACTORY_TDC_TDP_HOB;

#pragma pack(pop)
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

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
