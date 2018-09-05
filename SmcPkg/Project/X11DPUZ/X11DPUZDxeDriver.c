//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.08
//    Bug Fix:  Patch UIO X550 drop issue
//    Reason:   X55O + Skylake may drop the link, speed or width in cburn test
//    Auditor:  Donald Han
//    Date:     May/12/2017
//
//  Rev. 1.07
//    Bug Fix:  Fix that MAC Address of AOC-URN4-m2TS was wrong on IPMI web.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Feb/09/2017
//
//  Rev. 1.06
//    Bug Fix:  Update NVMe hotplug settings.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Nov/10/2016
//
//  Rev. 1.05
//    Bug Fix:  Fix only two MAC addresses can be observed for 4 LAN ports riser card.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/14/2016
//
//  Rev. 1.04
//    Bug Fix:  Fix BMC can't get Lan MAC address from BIOS.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/24/2016
//
//     Rev. 1.03
//       Bug Fix:   Fix Onboard NVMe can't be detected.
//       Reason:
//       Auditor:   Hartmann Hsieh
//       Date:      Jul/14/2016
//
//  Rev. 1.02
//      Bug Fixed:  Add riser card name to IIO and OPROM control items.
//      Reason:
//      Auditor:    Kasber Chen
//      Date:       Jun/21/2016
//
//  Rev. 1.01
//      Bug Fixed:  Add PCIE bifurcation setting in PEI.
//      Reason:
//      Auditor:    Kasber Chen
//      Date:       Jun/06/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jacker Yeh
//    Date:     Feb/04/2016
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
#include <Library\UefiBootServicesTableLib.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <PciBus.h>
#include <Library/MmPciBaseLib.h>
#include "SmcLib.h"
#include "SmcSetupModify.h"
#include "HideSetupItem.h"
#include "X11DPUZDxeDriver.h"
#include "SmcLibBmcPrivate.h"

#include <Library/MmPciBaseLib.h>
extern VOID SmcGetMacFrVpd (UINT16 , UINT16 , UINT8*);

UINT32
GetPurleyCPUType();

EFI_GUID gProjectOwnProtocolGuid  = EFI_PROJECT_OWN_PROTOCOL_GUID;

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
    DEBUG((EFI_D_INFO, "OVERRIDE_IOBP entry.\n"));

    DEBUG((EFI_D_INFO, "OVERRIDE_IOBP end.\n"));
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

//
// The code is from X10DRU.
//
VOID GetMlxMacFrVpd (UINT16 BDF, UINT16 VpdOfs, UINT8 *LanMacAddressStart)
{
    UINT8 i, var8;

    for(i=0; i<12; i++) {
        *(UINT16*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                        (UINT8)(BDF & 0x07), 0x4a) = VpdOfs&0xfffc;
        var8 = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                        (UINT8)(BDF & 0x07), 0x4c+VpdOfs%4);
        if((var8>='0')&&(var8<='9'))
            var8 -= '0';
        else if((var8>='A')&&(var8<='F'))
            var8 -= 'A'-0xa;
        if(i%2==0)
            *LanMacAddressStart = (var8<<4);
        else
            *(LanMacAddressStart++) |= var8;
        VpdOfs++;
    }
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
    IN UINT8 Index, // 1 ~ 4
    IN UINT16 BDF,
    IN UINT8 *TempBuffer
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINT8	i, Is710 = 0, IsMellanox = 0;
    UINT32	Data32;
    UINT32	SVID_Tbl[] = {0x15728086, 0x15898086};

    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                    (UINT8)(BDF & 0x07), 0);

    if(Data32 == 0xffffffff)	return Status;

    TempBuffer[0]++;	//add LAN number.

    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                    (UINT8)(BDF & 0x07), 0);

    for(i = 0; i < (sizeof(SVID_Tbl)/sizeof(UINT32)); i++) {
        if(Data32 == SVID_Tbl[i])
            Is710 = 1;
    }

    if (Data32 == 0x101515b3) { // Mellanox 25G NIC?
        IsMellanox = 1;
    }

    i = (TempBuffer[0] - 1) * 6;

    MemSet (TempBuffer + i + 1, 6, 0); // Clear MAC address

    *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x04) |= 0x06;

    if(Is710) {
        Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                        (UINT8)(BDF & 0x07), 0x10);

        Data32 = (Data32 & 0xfffffff0) + 0x1E2120;
        gBS->CopyMem(TempBuffer + i + 1, (UINT8*)Data32, 4);

        Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                        (UINT8)(BDF & 0x07), 0x10);

        Data32 = (Data32 & 0xfffffff0) + 0x1E2140;
        gBS->CopyMem(TempBuffer + i + 5, (UINT8*)Data32, 2);
    }
    else if(IsMellanox) {
        if (Index == 1)
            SmcGetMacFrVpd (BDF, 1, TempBuffer + i + 1);	// GetMlxMacFrVpd (BDF, 0x6c, TempBuffer + i + 1);
        else
            SmcGetMacFrVpd (BDF, 2, TempBuffer + i + 1);
    }
    else {
        Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                        (UINT8)(BDF & 0x07), 0x10);

        Data32 = (Data32 & 0xfffffff0) + 0x5400;
        gBS->CopyMem(TempBuffer + i + 1, (UINT8*)Data32, 6);
    }

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
    UINT16	BDF, Bus, BusE;
	UINT8 	IsCPU0FPGA = FALSE;
	UINT32	iBus, iFn;

	IsCPU0FPGA = ( ( GetPurleyCPUType() & 0x3 ) == 0x1 ) ? TRUE: FALSE;

	DEBUG((-1, "IsCPU0FPGA = 0x%x\n", IsCPU0FPGA));

    if((PcdGet32(PcdSmcRiserID)&0x1f00)==0xB00)	//AOC-2UR6N4-i4XT?
    {
    	iBus = *(UINT32*)MmPciAddress(0, 0, 8, 2, 0xcc);
    	if ( IsCPU0FPGA ) {
    		iBus = (iBus>>24) & 0xff;
			iFn = 2;
    	}
		else {
			iBus = (iBus>>8) & 0xff;
			iFn = 0;
		}

		DEBUG((-1, "iBus = 0x%x, iFn = 0x%x, did = 0x%x, a2 = 0x%x\n", 
			iBus, 
			iFn, 
			*(UINT16*)MmPciAddress(0, iBus, iFn, 0, 0x02),
			*(UINT8*)MmPciAddress(0, iBus, iFn, 0, 0xa2)
			));
    	if ( (( (*(UINT16*)MmPciAddress(0, iBus, iFn, 0, 0x02)) == (0x2030|iFn))) &&
			(*(UINT8*)MmPciAddress(0, iBus, iFn, 0, 0xa2) != 0x43 ) ) {	// 1st X550 is gen3 x4?
			DEBUG((-1, "1st x550 cause reboot\n"));
			IoWrite8(0xcf9, 0x06);	// reset system
		}

		iFn++;

		DEBUG((-1, "iBus = 0x%x, iFn+1 = 0x%x, did = 0x%x, a2 = 0x%x\n", 
			iBus, 
			iFn, 
			*(UINT16*)MmPciAddress(0, iBus, iFn, 0, 0x02),
			*(UINT8*)MmPciAddress(0, iBus, iFn, 0, 0xa2) 
			));
		if ( (( (*(UINT16*)MmPciAddress(0, iBus, iFn, 0, 0x02)) == (0x2030|iFn))) &&
			(*(UINT8*)MmPciAddress(0, iBus, iFn, 0, 0xa2) != 0x43 ) ) {	// 2nd X550 is gen3 x4?
			DEBUG((-1, "2nd x550 cause reboot\n"));
			IoWrite8(0xcf9, 0x06);	// reset system
    	}
    }
    
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
    if(BDF != 0x8888) {
        BusE = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                     (UINT8)(BDF & 0x07), 0x1A);
        Bus = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                    (UINT8)(BDF & 0x07), 0x19);
        for( ; BusE > Bus; BusE--) {
            if(*(UINT8*)MmPciAddress(0, BusE, 0, 0, 0) != 0xff)
                break;
        }
        BDF = BusE << 8;
        GetOnBoardLanMacAddress(1, BDF, (HardWareInfoData + 3));
    }

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
    if(BDF != 0x8888) {
        BusE = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                     (UINT8)(BDF & 0x07), 0x1A);
        Bus = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                    (UINT8)(BDF & 0x07), 0x19);
        for( ; BusE > Bus; BusE--) {
            if(*(UINT8*)MmPciAddress(0, BusE, 0, 0, 0) != 0xff)
                break;
        }
        BDF = BusE << 8;
        if(PcdGet8(PcdSmcMultiFuncLanChip1) >= 0x02)
            BDF++;
        GetOnBoardLanMacAddress(2, BDF, LanMacInfoData);
    }

    BDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan3BDF));
    if(BDF != 0x8888) {
        BusE = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                     (UINT8)(BDF & 0x07), 0x1A);
        Bus = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                    (UINT8)(BDF & 0x07), 0x19);
        for( ; BusE > Bus; BusE--) {
            if(*(UINT8*)MmPciAddress(0, BusE, 0, 0, 0) != 0xff)
                break;
        }
        BDF = BusE << 8;
        if(PcdGet8(PcdSmcMultiFuncLanChip1) >= 0x04)
            BDF += 2;
        GetOnBoardLanMacAddress(3, BDF, LanMacInfoData);
    }

    BDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan4BDF));
    if(BDF != 0x8888) {
        BusE = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                     (UINT8)(BDF & 0x07), 0x1A);
        Bus = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                    (UINT8)(BDF & 0x07), 0x19);
        for( ; BusE > Bus; BusE--) {
            if(*(UINT8*)MmPciAddress(0, BusE, 0, 0, 0) != 0xff)
                break;
        }
        BDF = BusE << 8;
        if(PcdGet8(PcdSmcMultiFuncLanChip1) >= 0x04)
            BDF += 3;
        else if(PcdGet8(PcdSmcMultiFuncLanChip1) >= 0x02)
            BDF++;
        GetOnBoardLanMacAddress(4, BDF, LanMacInfoData);
    }

    pBS->Stall(1000); // delay 1000 microseconds

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
    EFI_STATUS	Status = EFI_SUCCESS;

    DEBUG((-1, "DXE GetProjectPciEBifurcate Start.\n"));

    DEBUG((EFI_D_INFO, "GetProjectPciEBifurcate End.\n"));
    return EFI_SUCCESS;
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
// Procedure:   X11DPUReadyToBootCallBack
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
X11DPUZReadyToBootCallBack (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
	EFI_STATUS	Status = EFI_SUCCESS;

	UINT8 pause_ipmi[] = {0xdf, 0x01};	// Resume BMC SDR Reading
	UINT8 pause_ipmi_return[32];
	UINT8 pause_ipmi_return_size = 32;
	
	DEBUG((-1, "X11DPUReadyToBootCallBack Entry.\n"));
	
	Status = SMC_IPMICmd(0x30, 0x00, 0x70, pause_ipmi, 2, pause_ipmi_return, &pause_ipmi_return_size);

	DEBUG((-1, "Sts=%r.\n", Status));
	
	gBS->CloseEvent(Event);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : X11DPUDriverInit
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
X11DPUZDxeDriverInit(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;
    EFI_EVENT	X11DPUReadyToBootEvent;
    static EFI_GUID guidReadyToBoot = EFI_EVENT_GROUP_READY_TO_BOOT;
    EFI_EVENT HideItemEvent;
    VOID *HideItemReg;

    InitAmiLib(ImageHandle, SystemTable);

    DEBUG((EFI_D_INFO, "X11DPUDxeDriverInit Init\n"));

    Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gProjectOwnProtocolGuid ,
                 EFI_NATIVE_INTERFACE,
                 &ProjectInterface);

    ASSERT_EFI_ERROR(Status);

    gBS->CreateEventEx(
        EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
        X11DPUZReadyToBootCallBack,
        NULL, &guidReadyToBoot,
        &X11DPUReadyToBootEvent);

    Status = gBS->CreateEvent(
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  X11DPUZHideSetupItem,
                  NULL,
                  &HideItemEvent);
    if(!EFI_ERROR (Status)) {
    	Status = gBS->RegisterProtocolNotify(
                  &gSetupModifyProtocolGuid,
                  HideItemEvent,
                  &HideItemReg);
    }

    OVERRIDE_IOBP();

    DEBUG((EFI_D_INFO, "Exit X11DPUDxeDriverInit Init\n"));

    return Status;
}
