//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file PlatformPeiSystemInfoDIMM.c

    Gathers DIMM information
*/

#include <Library/PeiServicesLib.h>
#include "PlatformPeiSystemInformation.h"
#include "Pei.h"


#include <Library/HobLib.h>
#include <Guid/MemoryMapData.h>
#include <Include/Setup/IioUniversalData.h>


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//  Procedure:   InitializeDimmData
//
//  Description: Gets the DIMM's data from the system memory HOB
//
//  Input:       VOID
//
//  Output:      Print DIMM data to screen
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
InitializeDimmData (IN  CONST EFI_PEI_SERVICES        **PeiServices,
		AMI_PEI_TEXT_OUT	*This
)
{
    EFI_STATUS          Status = EFI_UNSUPPORTED;
    EFI_HOB_GUID_TYPE   *GuidHob;    
    struct              SystemMemoryMapHob  *mSystemMemoryMap;
    UINT32                        MemInfoTotalMem;
    UINT16                        MemInfoMemFreq;
    
    
    CHAR8		RasMode[12]; 
    CHAR8                   TextString[160];
    
    DEBUG((EFI_D_INFO,"[PEI System Information] InitializeDimmData \n"));

    GuidHob = GetFirstGuidHob (&gEfiMemoryMapGuid);
    if ( GuidHob != NULL)
        mSystemMemoryMap = (struct SystemMemoryMapHob *) GET_GUID_HOB_DATA (GuidHob);
    else {
        mSystemMemoryMap = NULL;
        Status = EFI_DEVICE_ERROR;
        return Status;
    }
    
    MemInfoTotalMem = (UINT32)(mSystemMemoryMap->memSize << 6);
    MemInfoMemFreq  = mSystemMemoryMap->memFreq;
    
	//TODO: Make this a switch
    if (mSystemMemoryMap->MemSetup.RASmode == 0) {
	    AsciiSPrint(RasMode, sizeof (RasMode), "Indep");
    } else if (mSystemMemoryMap->MemSetup.RASmode == CH_MIRROR) {
	    AsciiSPrint(RasMode, sizeof (RasMode), "Mirror");
    } else if (mSystemMemoryMap->MemSetup.RASmode == CH_LOCKSTEP) {
	    AsciiSPrint(RasMode, sizeof (RasMode), "Lockstep");
    } else if (mSystemMemoryMap->MemSetup.RASmode == RK_SPARE) {
	    AsciiSPrint(RasMode, sizeof (RasMode), "Rank Spare");
    } else if (mSystemMemoryMap->MemSetup.RASmode == CH_SL) {
	    AsciiSPrint(RasMode, sizeof (RasMode), "SP/LK");
    } else if (mSystemMemoryMap->MemSetup.RASmode == CH_MS) {
	    AsciiSPrint(RasMode, sizeof (RasMode), "MR/SP");
    }
    
    Status = This->SetAttribute(This, 0xF, 0, 0);
    AsciiSPrint (TextString, 160, "Memory Info: Memory Size: %dGB  Memory Speed: %dMHz RAS Mode: %a\n", 
		    MemInfoTotalMem >> 10, MemInfoMemFreq, RasMode);
    Status = This->WriteString(This, 0, TextString);
    Status = This->SetAttribute(This, 0x7, 0, 0);
      
    return EFI_SUCCESS;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
