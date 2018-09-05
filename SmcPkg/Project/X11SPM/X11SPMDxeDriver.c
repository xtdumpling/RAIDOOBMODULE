//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.06
//    Bug Fix:  Fix that BIOS create redundant SMBIOS Type 40.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Dec/20/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix BMC can't get LAN Mac address.
//    Reason:
//    Auditor:  William Hsu
//    Date:     Jan/11/2017
//
//  Rev. 1.04
//    Bug Fix:  Fix that fail to get MAC Address of onboard LAN sometimes when
//              Above 4G Enabled.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Dec/20/2016
//
//  Rev. 1.03
//    Bug Fix:  Correct SMBIOS Type 8.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Dec/16/2016
//
//  Rev. 1.02
//    Bug Fix:  Report correct MAC Address of onboard LANs to IPMI.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Dec/15/2016
//
//  Rev. 1.01
//    Bug Fix:  Correct SMBIOS Type 41.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Dec/14/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Isaac Hsu
//    Date:     Nov/18/2016
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		ProjectDxeDriver.c
//
// Description:
//  This file implement function of PROJECTDRIVER_PROTOCOL
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <PciBus.h>
#include "X11SPMDxeDriver.h"
#include "SmcLib.h"
#include "SmcSetupModify.h"
#include "HideSetupItem.h"
#include <Protocol/AmiSmbios.h>
#include <Library/MmPciBaseLib.h>

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  OVERRIDE_IOBP
//
// Description :  override IOBP setting for USB3/ SATA SI
//
// Parameters:  None
//
// Returns     :  Override IOBP setting in PEI phase
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
OVERRIDE_IOBP(VOID)
{

}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectCheckAdjustVID
//
// Description :  Send out the Project Adjust VID
//
// Parameters  :  NONE
//
// Returns     :  Project Adjust VID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI ProjectCheckAdjustVID(VOID)
{
    return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetGPUDID
//
// Description :  Get GPU device id
//
// Parameters  :  NONE
//
// Returns     :  Get GPU device id
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT16 EFIAPI GetGPUDID(VOID)
{
    return 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectGetBID
//
// Description :  Send out the Project BID
//
// Parameters  :  NONE
//
// Returns     :  Project BID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI GetProjectBID(VOID)
{
    return 0x00;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectCheckBMCPresent
//
// Description :  Check BMC Present or not
//
// Parameters  :  NONE
//
// Returns     :  1: BMC Present  0: MBC not present
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
UINT8 EFIAPI ProjectCheckBMCPresent(VOID)
{
    UINT8 ReturnVal = 0;
    return ReturnVal;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  GetOnBoardLanMacAddress
//
// Description: 
//  Get onboard lan Mac address in PEI phase. BIOS should decode
//  PCIE root port bus temporary and then to set Lan resource to
//  get Mac address. BIOS must remove all temporary setting before
//  exit this routine
//
// Input:
//      IN UINT16 LanRootPortCfg - Onbaord lan pcie root port BDF
//      IN OUT UINT8 *LanMacAddressStart1 - Point to start address of LAN 1 MAC structure
//      IN OUT UINT8 *LanMacAddressStart2 - Point to start address of LAN 2 MAC structure
//
// Output:      
//      EFI_SUCCESS (return)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
GetOnBoardLanMacAddress (
    IN  UINT16	BDF,
    IN  UINT8	*TempBuffer
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32	Data32;
    UINT16	Cmd, OldCmd = 0;
    UINT8   i;
    UINT64  Data, Bar;


    DEBUG((-1, "GetOnBoardLanMacAddress start.\n"));

    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0);
    if( (Data32 == 0xffffffff) || (Data32&0xFFF00000)!=0x37D00000 ) // X772's DID is 0x37DX
        return Status;

    TempBuffer[0]++;	// Add LAN number.
    i = (TempBuffer[0] - 1) * 6;
    MemSet(TempBuffer + i + 1, 6, 0); // Clear MAC address

    // Enable Memory Access and mastering for cmd if needed
    Cmd = *(UINT16*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x4);
    if( (Cmd&0x06)==0 ) {
        OldCmd = Cmd;
        *(UINT16*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x4) = (Cmd|0x06);
    }

    // Get Bar
    Data = (UINT64)*(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x10);
    Bar = Data;
    Data = (UINT64)*(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x14);
    Bar += (UINT64)(Data<<32);
    Bar &= 0xFFFFFFFFFFFFFFF0;

    // Get MAC Address
    Data = Bar+0x1E2120;
    gBS->CopyMem(TempBuffer + i + 1, (UINT8*)Data, 4);

    Data = Bar+0x1E2140;
    gBS->CopyMem(TempBuffer + i + 5, (UINT8*)Data, 2);

    // Restore cmd if needed
    if(OldCmd)
        *(UINT16*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x4) = OldCmd;

    DEBUG((-1, "GetOnBoardLanMacAddress end.\n"));
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  ProjectHardwareInfo
//
// Description :  Create Project own Hareware Information
//
// Parameters  :  OUT: HardWareInfoData
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI ProjectHardwareInfo(
    OUT	UINT8	*HardWareInfoData,
    OUT	UINT8	*LanMacInfoData
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT16	BDF, Bus;


    HardWareInfoData[3] = 0x00;
    BDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan1BDF));
    Bus = GetOnBoardLanBus(BDF);

    BDF = Bus << 8;
    GetOnBoardLanMacAddress(BDF, (HardWareInfoData + 3));

    // LanMacInfoData parameter data is used by 0x9E command
    // IPMI 0x9E command data:
    // Byte 0    : LAN number N  (exclude NumLan of 0x30 0x20 command)
    // Byte 1-6  : LAN port M MAC address
    // Byte 7-12 : LAN port M+1 MAC address
    if(HardWareInfoData[3]) { // For other SKUs
        LanMacInfoData[0] = 0;

        BDF = (Bus << 8) + 1;
        GetOnBoardLanMacAddress(BDF, LanMacInfoData);

        BDF = (Bus << 8) + 2;
        GetOnBoardLanMacAddress(BDF, LanMacInfoData);

        BDF = (Bus << 8) + 3;
        GetOnBoardLanMacAddress(BDF, LanMacInfoData);
    }
    else { // For X11SPM-F SKU
        BDF = (Bus << 8) + 2;
        GetOnBoardLanMacAddress(BDF, (HardWareInfoData + 3));

        LanMacInfoData[0] = 0;
        BDF = (Bus << 8) + 3;
        GetOnBoardLanMacAddress(BDF, LanMacInfoData);
    }

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  CheckAudioHandle
//
// Description :  Handle audio function in board level
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI CheckAudioHandle(VOID)
{
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  GetProjectPciEBifurcate
//
// Description :  Handle audio function in board level
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS EFIAPI GetProjectPciEBifurcate(IN OUT IIO_CONFIG *SetupData)
{
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  X11SPMSmbiosUpdate
//
// Description :  Update SMBIOS information after ReadyToBootEvent
//
// Returns     :  none
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
X11SPMSmbiosUpdate()
{

}

/*-----------------12/23/2014 8:39PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  SetProjectPciECTLE
//
// Description :  Handle PciE CTLE override in board level
// 		Instance	Function	Port	Description
//		0		D6:F0 		0	DMI without Gen3 support
//		1		D6:F1		1	Port 1 CTLE
//		3		D6:F3		2	Port 2 CTLE
//		7		D7:F0		3	Port 3 CTLE
// Input:
//	IN IIO_GLOBALS	*IioGlobalData
//	IN UINT8	Iio
//	IN UINT8	BusNumber
//
// Output:      
//      EFI_SUCCESS (return)
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>

EFI_STATUS EFIAPI
SetProjectPciECTLE(
    IN IIO_GLOBALS	*IioGlobalData,
    IN UINT8	Iio,
    IN UINT8	BusNumber
)
{
    TRACE((-1, "SetProjectPciECTLE entry.\n"));
    TRACE((-1, "SetProjectPciECTLE end.\n"));
    return EFI_SUCCESS;
}
 */

PROJECTDRIVER_PROTOCOL ProjectInterface =
{
    ProjectCheckAdjustVID,
    GetGPUDID,
    GetProjectBID,
    NULL,
    ProjectHardwareInfo,
    ProjectCheckBMCPresent,
    CheckAudioHandle,
    GetProjectPciEBifurcate,
    NULL,
    NULL,
    NULL
};


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   X11SPMReadyToBootCallBack
//
// Description: Run at ready to boot.
//
// Input:       IN EFI_EVENT   Event
//              IN VOID        *Context
//
// Output:      None
//
// Modified:
//
// Referrals:
//
// Notes:
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID
X11SPMReadyToBootCallBack (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    DEBUG((EFI_D_INFO, "X11SPMReadyToBootCallBack Entry.\n"));
    
    X11SPMSmbiosUpdate();
    
    gBS->CloseEvent(Event);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : X11SPMDriverInit
//
// Description : Project DXE initial code
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
X11SPMDxeDriverInit(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;
    EFI_EVENT	X11SPMReadyToBootEvent, HideItemEvent;
    VOID        *HideItemReg;
    static EFI_GUID guidReadyToBoot = EFI_EVENT_GROUP_READY_TO_BOOT;

    DEBUG((EFI_D_INFO, "ProjectDriver Init\n"));

    Status = gBS->InstallProtocolInterface(
		    &Handle,
		    &gProjectOwnProtocolGuid ,
		    EFI_NATIVE_INTERFACE,
		    &ProjectInterface);

    ASSERT_EFI_ERROR(Status);

    gBS->CreateEventEx(
		    EVT_NOTIFY_SIGNAL,
		    TPL_CALLBACK, 
		    X11SPMReadyToBootCallBack,
		    NULL,
		    &guidReadyToBoot,
		    &X11SPMReadyToBootEvent);

    Status = gBS->CreateEvent(
                          EVT_NOTIFY_SIGNAL,
                          TPL_CALLBACK,
                          X11SPMHideSetupItem,
                          NULL,
                          &HideItemEvent);

    if(!Status){
    	Status = gBS->RegisterProtocolNotify(
                          &gSetupModifyProtocolGuid,
                          HideItemEvent,
                          &HideItemReg);
    }

    OVERRIDE_IOBP();
    
    DEBUG((EFI_D_INFO, "Exit ProjectDriver Init\n"));

    return Status;
}
