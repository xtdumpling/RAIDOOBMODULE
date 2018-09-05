//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file PlatformVideoInit.c
    LIB driver for initializing the Video in PEI 

**/

#include <Include/AmiLib.h>
#include <Library/DebugLib.h>

#include <PlatformVideoInit.h>

extern EFI_GUID gVideoInitDoneSentGuid;
extern UINT32 VideoInit(VIDEO_PARAMETERS parameters);

CONST EFI_PEI_PPI_DESCRIPTOR gVideoInitDoneSentSignalPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gVideoInitDoneSentGuid,
  NULL
};

/**
    Initializes video controller with VGA standard init. 

    @param   None 

    @retval  EFI_NOT_FOUND
**/
EFI_STATUS 
AmiVideoInit
(
    IN  EFI_PEI_SERVICES  **PeiServices
)
{
    VIDEO_PARAMETERS    *DevInfo;
    EFI_STATUS          Status;  
    
    Status = (*PeiServices)->AllocatePool( PeiServices,
    		sizeof(VIDEO_PARAMETERS),
    		&DevInfo
    		);
    if (EFI_ERROR(Status)) {
        return Status;    
    }
    
    // Fill the platform related parameters for Video Init
    DevInfo->RootBus            = mVideoVidPath[0].Bus;
    DevInfo->DevPath            = mVideoVidPath;   //Pointer to a buffer of dev path structure that define the location of the video controller in the PCI topology  
    DevInfo->DevPathEntries     = 3;                    //Count of entries in the preceding buffer.        
    DevInfo->PciExpressCfgBase  = (UINTN) PcdGet64(PcdPciExpressBaseAddress);           //The base address of PCI Express Memory Mapped Configuration Space.
    DevInfo->MemBase            = (UINTN) PcdGet64(PcdPciExpressBaseAddress) + 0x10000000;           //The base address of the region where MMIO BARs are assigned when configuring the VGA controller
    DevInfo->MemSizeMax         = 0x08000000;           //The maximum amount of memory to allow during BAR assignment.
    DevInfo->IoBase             = 0x2000;               //The base address of the region where IO BARs are assigned when configuring the VGA controller    
    DevInfo->IoSizeMax          = 0x1000;               //The maximum amount of IO to allow during BAR assignment. 
    DevInfo->FontMap            = &mVideoFontMapInfo;   //Pointer to a buffer of font map information structures that define the font map(s) to load.
    DevInfo->FontMapCount       = 1;                    //Count of entries in the preceding buffer.
    
    
    //Simics
    if (IsSimicsPlatform() == TRUE) {
	    DevInfo->DevPath            = mSimulatedVideoVidPath;   //Pointer to a buffer of dev path structure that define the location of the video controller in the PCI topology 
	    DevInfo->DevPathEntries     = 1;                    //Count of entries in the preceding buffer.  
	    DevInfo->RootBus            = mSimulatedVideoVidPath[0].Bus;
    }    

    // Initialize VGA Hardware
    Status = VideoInit (*DevInfo); 
    
    if(Status == 0){
	    Status = (*PeiServices)->InstallPpi(PeiServices, &gVideoInitDoneSentSignalPpi);
    }
    
    return Status;
}

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
