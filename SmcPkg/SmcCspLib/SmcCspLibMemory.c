//***************************************************************************
//***************************************************************************
//**                                                                       **
//**       (C)Copyright 1993-2016 Supermicro Computer, Inc.                **
//**                                                                       **
//***************************************************************************
//***************************************************************************
//
//  Rev. 1.01
//    Bug Fix:  Fixed the memory frequency shows incorrect.
//    Reason:   Report SystemMemoryMap->memFreq as memory frequency directly.
//    Auditor:  Jimmy Chiu
//    Date:     Aug/15/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Kasber Chen
//    Date:     May/16/2016
//
//***************************************************************************
//***************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcCspLibMemory.c
//
// Description: Supermicro memory library.
//
//<AMI_FHDR_END>
//**********************************************************************
#include "token.h"
#include "EFI.h"
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include "Library/UefiLib.h"
#include <Guid/MemoryMapData.h>
#include "SmcCspLibMemory.h"

UINT16
SmcCspGetMemFreq()
{
    EFI_STATUS	Status;
    VOID	*HobList;
    struct	SystemMemoryMapHob	*SystemMemoryMap;
    EFI_HOB_GUID_TYPE	*GuidHob;
    //UINT16	MemFreqArray[18] = {
    //		    800, 1000, 1067, 1200, 1333, 1400,
    //		    1600, 1800, 1867, 2000, 2133, 2200,
    //		    2400, 2600, 2667, 2800, 2933, 3000};
    UINT16	Frequency = 0;

    Status = EfiGetSystemConfigurationTable(&gEfiHobListGuid, &HobList);
    if(!EFI_ERROR(Status)){
	GuidHob = GetFirstGuidHob(&gEfiMemoryMapGuid);
	if(GuidHob != NULL){
	    SystemMemoryMap = (struct SystemMemoryMapHob*)GET_GUID_HOB_DATA(GuidHob);
	    //if(MemFreqArray[SystemMemoryMap->memFreq] != 0){
		//Frequency = MemFreqArray[SystemMemoryMap->memFreq];
	    //}
        Frequency = SystemMemoryMap->memFreq;
	}
	else {
	    Frequency = 800;
	    Status = EFI_DEVICE_ERROR;
	}
    }
    else {
	Frequency = 800;
	Status = EFI_DEVICE_ERROR;
    }
    return Frequency;
}	
//****************************************************************************
