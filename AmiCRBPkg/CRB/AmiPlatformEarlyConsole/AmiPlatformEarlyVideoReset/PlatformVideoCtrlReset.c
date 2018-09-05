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

/** @file VideoCtrlReset.c
    LIB driver for Resetting the Video Controller Programming done in the PEI 

**/

#include <Include/AmiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/AmiDxeTextOut.h>
#include <AmiDxeLib.h>

typedef struct {
    UINT8   Bus;
    UINT8   Dev;
    UINT8   Func;
} DEV_PATH;


#if PLATFORM_TYPE == 0 //Platform is Neon City
DEV_PATH mVideoVidPath [] = {
		{0x0,0x1C,0x5},		//PCIe-PCI bridge
		{0x1,0x00,0x0},		//Video Bridge
		{0x2,0x00,0x0} 		//Video Controller
};
#else			//Platform is Lightning Ridge
DEV_PATH mVideoVidPath [] = {
		{0x0,0x1D,0x3},		//PCIe-PCI bridge
		{0x1,0x00,0x0},		//Video Bridge
		{0x2,0x00,0x0} 		//Video Controller
};
#endif

extern BOOLEAN             VideoDeviceInUse;
VOID                *gPciProtocolNotifyReg;

#define PCIE_REG_ADDR(Bus,Device,Function,Offset) \
  (((Offset) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))

void FvPciWrite16(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT16 data) {
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));

    *(volatile UINT16 *)regAddr = data;

}

void FvPciRead16(UINT32 CfgBase, UINT8 bus, UINT8 dev, UINT8 func, UINT16 reg, UINT16 *data) {
    UINT8 *regAddr;

    regAddr = (UINT8 *)(CfgBase | PCIE_REG_ADDR(bus, dev, func, reg));
    *data = *(volatile UINT16 *)regAddr;

}

/*
 * DisableVgaDecode- Disables legacy VGA decode down to the video controller
 *
 * Input:
 *
 * Output:
 *   
 */
VOID DisableVgaDecode() {
    UINT8  		bus, dev, func, devPathIdx;
    UINT16 		data16 = 0;
    UINT32 		base;
    DEV_PATH    	*DevPath;
    UINT8       	DevPathEntries;
    UINT32      	PciExpressCfgBase;

    DevPath            = mVideoVidPath;  
    DevPathEntries     = 3;             
    PciExpressCfgBase  = 0x80000000;               
        
    base = PciExpressCfgBase;
    //Root Bus is 0
    bus = 0;

    //
    // Iterate through all the bridges in the dev path and disable VGA decode.
    //
    for (devPathIdx = 0; devPathIdx < DevPathEntries-1; devPathIdx++) {
        dev = DevPath[devPathIdx].Dev;
        func = DevPath[devPathIdx].Func;
        //
        //Disable VGA decode
        //
        FvPciRead16(base, bus, dev, func, 0x3E, &data16);
        data16 &= 0xF7;
        FvPciWrite16(base, bus, dev, func, 0x3E, data16);

        //
        // Next device in the path should be on bus+1
        //
        bus++;
    } 

    return;
}

/**
    Notification function for Pci Protocol 
    This function stops the video protocol after PCI enumeration 

    @param Event - Event which caused this handler
    @param Context - Context passed during Event Handler registration

    @return VOID

**/

VOID
PciProtocolCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *TextOutContext 
)
{

    EFI_STATUS		Status = EFI_SUCCESS;
    AMI_DXE_TEXT_OUT	*TextOut = NULL;
    
    DEBUG((EFI_D_INFO,"[Early Video] \n"__FUNCTION__));

    //Disable VGA Decode in pci bridge devices
    DisableVgaDecode();   
    
    VideoDeviceInUse=TRUE;   
    
    // Kill the Event
    pBS->CloseEvent(Event);
    return;
}

/**
    Install the Callback to Reset the Video Controller Programming done in the PEI Phase. 

    @param   None 

    @retval  EFI_NOT_FOUND
**/
EFI_STATUS 
AmiVideoControllerReset(
    VOID
)
{
	EFI_STATUS		Status = EFI_NOT_FOUND;
	EFI_EVENT           PciProtocolEvent;
	
    // Install the Callback function to Reset the Video Controller programming. 
    // Porting Required.
    //
	DEBUG((EFI_D_INFO,"[Platform Early Video] Platform Controller Reset"));
    //Create notification for PciEnumerationComplete
    
    Status = pBS->CreateEvent (EVT_NOTIFY_SIGNAL, 
                                    TPL_CALLBACK,
                                    PciProtocolCallBack, 
                                    NULL, 
                                    &PciProtocolEvent);
    ASSERT_EFI_ERROR(Status);
    
    if (!EFI_ERROR(Status)) {
	    Status = pBS->RegisterProtocolNotify (
			    &gBdsConnectDriversProtocolGuid,
			    PciProtocolEvent, 
			    &gPciProtocolNotifyReg
			    );
	    
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
