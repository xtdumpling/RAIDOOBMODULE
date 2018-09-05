//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
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
//#include <Protocol/SmbusHc.h>
//#include <Guid/EventGroup.h>
#include <GpioConfig.h>
#include <Library\GpioLib.h>
#if SmcRiserCard_SUPPORT
#include "../Module/SmcRiserCard/SmcRiserCard.h"
EFI_GUID gSmcRiserCardProtocolGuid = EFI_SMC_RISERCARD_PROTOCOL_GUID;
SMCRISERCARD_PROTOCOL	*gSmcRiserCardProtocol;
#endif
#include "SmcLib.h"
#include "SmcSetupModify.h"
#include "HideSetupItem.h"
#include "X11DPTSGIDxeDriver.h"
#include <Protocol/AmiSmbios.h>
#include <Library/MmPciBaseLib.h>

#define IIO_BIFURCATE_x4x4x4x4  0
#define IIO_BIFURCATE_x4x4xxx8  1
#define IIO_BIFURCATE_xxx8x4x4  2
#define IIO_BIFURCATE_xxx8xxx8  3
#define IIO_BIFURCATE_xxxxxx16  4

EFI_GUID gProjectOwnProtocolGuid  = EFI_PROJECT_OWN_PROTOCOL_GUID;


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  IsA1B1Node
//
// Description :  determine A1 or B1 node.
//
// Parameters:  VOID
//
//
// Returns     :  BOOLEAN   True :  A1 or B1 side
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
BOOLEAN  IsA1B1Node(
VOID
)
{

   BOOLEAN             A1B1Node = FALSE;
   UINT32             Value = 0;
//    DEBUG ((DEBUG_INFO, "PeiProjectSystemConfiguration.\n"));
   DEBUG((EFI_D_INFO, "Check IsA1B1Node ent .\n"));

   GpioGetInputValue(SGI_NODE_1_PIN, &Value);
   //Check GPP_C_12     1 : A1/B1 side  
    if (Value) {
        //Disable NMI2SMI_EN for prevent NMI trigger again before service.
        DEBUG((EFI_D_INFO, "IsA1B1Node .\n"));
        A1B1Node = TRUE;
    }  

    return A1B1Node;
}
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

    
    DEBUG((-1, "GetOnBoardLanMacAddress start.\n"));
    
    MemSet (TempBuffer, 6, 0);
    
    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0);
    
    if(Data32 == 0xffffffff)	return Status;
    
    TempBuffer[0]++;	//add LAN number.

    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x10);

    Data32 = (Data32 & 0xfffffff0) + 0x5400;
    DEBUG((-1, "LanMac Addr=%p end.\n",(UINT8*)Data32));
    gBS->CopyMem(TempBuffer + 1, (UINT8*)Data32, 6);

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
        Bus = *(UINT8*)MmPciAddress(0,(UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                                     (UINT8)(BDF & 0x07), 0x1A);
        BDF = Bus << 8;

        GetOnBoardLanMacAddress(BDF, (HardWareInfoData + 3));
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

    DEBUG((-1, "X10DRTSGI Dxe GetProjectPciEBifurcate Start.\n"));
    DEBUG((-1, "Should not call this assert here\n"));
    ASSERT(FALSE);
    
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
//
// Procedure   :  X11SPWSmbiosUpdate
//
// Description :  Update SMBIOS information after ReadyToBootEvent
//
// Returns     :  none
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
X11DPTSGISmbiosUpdate()
{
    EFI_STATUS  Status;
    UINT8       *TempBuffer;
    UINT16	TempBufferSize;
    UINT16      Handle;
    AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;   


    TRACE((-1, "X11DPTSGI SmbiosUpdate entry.\n"));   
 
    Status = gBS->LocateProtocol(&gAmiSmbiosProtocolGuid,NULL,&SmbiosProtocol);
    if(EFI_ERROR(Status)) return;
    
    
    if( PcdGetBool(PcdSGINodeIsA1B1) ){
       Status = SmbiosProtocol->SmbiosReadStrucByType(9, 1, &TempBuffer, &TempBufferSize);  
       if (!EFI_ERROR(Status)) {
         Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
         TRACE((-1, "Del Type9 A0 node handle=0x%x\n",Handle));         
         SmbiosProtocol->SmbiosDeleteStructure(Handle); //delete Type9 A0 node x16 slot 
         gBS->FreePool(TempBuffer);
      }
    } else {
         Status = SmbiosProtocol->SmbiosReadStrucByType(9, 2, &TempBuffer, &TempBufferSize);  
       if (!EFI_ERROR(Status)) {
         Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
         TRACE((-1, "Del Type9 A1 node handle=0x%x\n",Handle));         
         SmbiosProtocol->SmbiosDeleteStructure(Handle); //delete Type9 A1 node x16 slot 
         gBS->FreePool(TempBuffer);
      }      
    }
        
    TRACE((-1, "X11DPTSGI SmbiosUpdate end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   X11DPTSGIReadyToBootCallBack
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
X11DPTSGIReadyToBootCallBack (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS	Status = EFI_SUCCESS;

    DEBUG((EFI_D_INFO, "X11DPTSGIReadyToBootCallBack Entry.\n"));
    
    X11DPTSGISmbiosUpdate();
    
    gBS->CloseEvent(Event);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : X11DPTSGIDriverInit
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
X11DPTSGIDxeDriverInit(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;
    EFI_EVENT	X11DPTSGIReadyToBootEvent;
    static EFI_GUID guidReadyToBoot = EFI_EVENT_GROUP_READY_TO_BOOT;
    EFI_EVENT HideItemEvent;
    VOID *HideItemReg;

    InitAmiLib(ImageHandle, SystemTable);
    DEBUG((EFI_D_INFO, "ProjectDriver Init\n"));

    Status = gBS->InstallProtocolInterface(
		    &Handle,
		    &gProjectOwnProtocolGuid ,
		    EFI_NATIVE_INTERFACE,
		    &ProjectInterface);

    ASSERT_EFI_ERROR(Status);

//    Status = CreateReadyToBootEvent(TPL_CALLBACK,
//                                    X11DPTSGIReadyToBootCallBack,
//                                    NULL,
//                                    &X11DPTSGIReadyToBootEvent);

        Status = CreateReadyToBootEvent(TPL_CALLBACK,
                                    X11DPTSGIReadyToBootCallBack,
                                    NULL,
                                    &X11DPTSGIReadyToBootEvent);
    DEBUG((EFI_D_INFO, "ReadyCallback Status=%r\n",Status));
                               
    Status = gBS->CreateEvent(
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  X11DPTSGIHideSetupItem,
                  NULL,
                  &HideItemEvent);
    if(!EFI_ERROR (Status)) {
    	Status = gBS->RegisterProtocolNotify(
                  &gSetupModifyProtocolGuid,
                  HideItemEvent,
                  &HideItemReg);
    }

    OVERRIDE_IOBP();
    
    DEBUG((EFI_D_INFO, "Exit ProjectDriver Init\n"));

    return Status;
}
