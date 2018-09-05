//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix : Fix x550 patch coding error.
//    Reason  : 
//    Auditor : Isaac Hsu
//    Date    : May/18/2017
//
//  Rev. 1.02
//    Bug Fix : Add a patch for x550 drop issue.
//    Reason  : 
//    Auditor : Isaac Hsu
//    Date    : May/18/2017
//
//  Rev. 1.01
//    Bug Fix:  Add function to show LAN's MAC address on IPMI Web.
//    Reason:   
//    Auditor:  Isaac Hsu
//    Date:     Feb/02/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Isaac Hsu
//    Date:     Jan/04/2017
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
#include "X11SPLDxeDriver.h"
#include "SmcLib.h"
#include "SmcSetupModify.h"
#include "HideSetupItem.h"
#include <Protocol/AmiSmbios.h>
#include <Library/MmPciBaseLib.h>
#include <SspTokens.h>

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
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT8   i;
    UINT32  Data32;
    UINT8 Temp1;


    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0);
    if(Data32 == 0xffffffff || Data32!=0x15338086)
        return Status;

    TempBuffer[0]++;    // Add LAN number.

    i = (TempBuffer[0] - 1) * 6;
    MemSet(TempBuffer + i + 1, 6, 0); // Clear MAC address

    // Get MAC Address
    Temp1 = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x04);
    *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x04) = 0x07;
    
    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x10);
    Data32 = (Data32 & 0xfffffff0) + 0x5400;
    gBS->CopyMem(TempBuffer + i + 1, (UINT8*)Data32, 6);
    
    *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x04) = Temp1;

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
    UINT16  BDF, Bus;


    // HardWareInfoData parameter data is used by 0x20 command
    // IPMI 0x20 command data:
    // Byte 0   : motherboard SSID  (bit[0:8])
    // Byte 1   : motherboard SSID  (bit[16:24])
    // Byte 2   : hardware monitor chip number
    // Byte 3   : NumLan M
    // Byte 4-9 : LAN port M+1 MAC address

    //
    // Clear Lan Num to 0. It will be update in "GetOnBoardLanMacAddress".
    //
    HardWareInfoData[3] = 0x00;
    BDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan1BDF));
    Bus = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x1A);
    BDF = Bus << 8;
    GetOnBoardLanMacAddress(BDF, (HardWareInfoData + 3));

    // LanMacInfoData parameter data is used by 0x9E command
    // IPMI 0x9E command data:
    // Byte 0    : LAN number N  (exclude NumLan of 0x30 0x20 command)
    // Byte 1-6  : LAN port M MAC address
    // Byte 7-12 : LAN port M+1 MAC address

    //
    // Clear Lan Num to 0. It will be update in "GetOnBoardLanMacAddress".
    //
    LanMacInfoData[0] = 0;
    BDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan2BDF));
    Bus = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x1A);
    BDF = Bus << 8;
    GetOnBoardLanMacAddress(BDF, LanMacInfoData);

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
// Procedure   :  X11SPLSmbiosUpdate
//
// Description :  Update SMBIOS information after ReadyToBootEvent
//
// Returns     :  none
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
X11SPLSmbiosUpdate()
{
    DEBUG((EFI_D_INFO, "X11SPM SmbiosUpdate entry.\n"));

    DEBUG((EFI_D_INFO, "X11SPM SmbiosUpdate end.\n"));
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
X11SPLReadyToBootCallBack (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    DEBUG((EFI_D_INFO, "X11SPLReadyToBootCallBack Entry.\n"));
    
    X11SPLSmbiosUpdate();
    
    gBS->CloseEvent(Event);
}

UINT8 CheckPresenceDetectState(UINT16 BDF)
{
    UINT8  Bus, Dev, Fun;
    UINT16 Data16 = 0;
    UINT32 i;

    Bus = (UINT8)(BDF >> 8);
    Dev = (UINT8)((BDF >> 3) & 0x1f);
    Fun = (UINT8)(BDF & 0x07);
    for(i=0; i<200; i++) {
        Data16 = *(UINT16*)MmPciAddress(0, Bus, Dev, Fun, 0xAA);
        if(Data16 & 0x40) {
            return 1;
        }
        gBS->Stall(200); // Delay 200 microseconds
    }

    return 0;
}

VOID
SmcChkX550Sts()
{
    UINT16 BDF;
    UINT8  Bus, Dev, Fun, TmpBus, RetryCount;
    UINT32 VidDid;


    BDF = SmcBDFTransfer(PcdGet32(PcdSmcSLOT3BDF));
    Bus = (UINT8)(BDF >> 8);
    Dev = (UINT8)((BDF >> 3) & 0x1f);
    Fun = (UINT8)(BDF & 0x07);

    // Assign temperate bus
    TmpBus = Bus;
    *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x18) = TmpBus;
    *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x19) = (TmpBus+4);
    *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x1A) = (TmpBus+4);
    
    // Check DidVid
    VidDid = *(UINT32*)MmPciAddress(0, (TmpBus+4), 0, 0, 0);
    if( VidDid==0xFFFFFFFF ) {
        // Check Presence_Detect_State
        if(CheckPresenceDetectState(BDF)) {
            RetryCount = GetCmosTokensValue(Q_X550WA);
            if(RetryCount<3) {
                SetCmosTokensValue(Q_X550WA, RetryCount+1);
                IoWrite8(0xCF9, 0x06);
            }
        }
    }
    SetCmosTokensValue(Q_X550WA, 0);

    // Clear temperate bus
    *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x18) = 0;
    *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x19) = 0;
    *(UINT8*)MmPciAddress(0, Bus, Dev, Fun, 0x1A) = 0;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : X11SPLDriverInit
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
X11SPLDxeDriverInit(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;
    EFI_EVENT	X11SPLReadyToBootEvent, HideItemEvent;
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
		    X11SPLReadyToBootCallBack,
		    NULL,
		    &guidReadyToBoot,
		    &X11SPLReadyToBootEvent);

    Status = gBS->CreateEvent(
                          EVT_NOTIFY_SIGNAL,
                          TPL_CALLBACK,
                          X11SPLHideSetupItem,
                          NULL,
                          &HideItemEvent);

    if(!Status){
    	Status = gBS->RegisterProtocolNotify(
                          &gSetupModifyProtocolGuid,
                          HideItemEvent,
                          &HideItemReg);
    }

    OVERRIDE_IOBP();

    SmcChkX550Sts();

    DEBUG((EFI_D_INFO, "Exit ProjectDriver Init\n"));

    return Status;
}
