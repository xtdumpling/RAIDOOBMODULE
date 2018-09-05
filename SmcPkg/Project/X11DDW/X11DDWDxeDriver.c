//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Update GetOnBoardLanMacAddress method for 1G(Marvell 88E1512).
//    Reason:   Change the MAC address to PRTPM_SAL(0x001e4440) and PRTPM_SAH(0x001e44C0)
//    Auditor:  Jimmy Chiu
//    Date:     Jan/10/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Clone from Sample folder.
//    Auditor:  Jimmy Chiu
//    Date:     Nov/22/2016
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
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <PciBus.h>
#include "X11DDWDxeDriver.h"
#include <Guid/SocketPowermanagementVariable.h>
#include <Protocol/IPMITransportProtocol.h>
#include "SmcLib.h"
#include "SmcSetupModify.h"
#include "HideSetupItem.h"
#include <Protocol/AmiSmbios.h>
#include <Library/MmPciBaseLib.h>
#include <Library/PchPcieRpLib.h>

UINT8
EFIAPI
GetPchMaxPciePortNum (
  VOID
  );
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
    UINT16	Cmd, OldCmd = 0;

    DEBUG((-1, "GetOnBoardLanMacAddress start.\n"));
    
    MemSet (TempBuffer, 6, 0);

    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
		    (UINT8)(BDF & 0x07), 0);
    
    if(Data32 == 0xffffffff)	return Status;
    
    TempBuffer[0]++;	//add LAN number.

    // Enable Memory Access and mastering for cmd if needed
    Cmd = *(UINT16*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x4);
    if( (Cmd&0x06)==0 ) {
        OldCmd = Cmd;
        *(UINT16*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f), (UINT8)(BDF & 0x07), 0x4) = (Cmd|0x06);
    }

    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
		    (UINT8)(BDF & 0x07), 0x10);

    Data32 = (Data32 & 0xfffffff0) + 0x001E4440; //PRTPM_SAL  0x001e4440
    gBS->CopyMem(TempBuffer + 1, (UINT8*)Data32, 4);

    Data32 = *(UINT32*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
    		    (UINT8)(BDF & 0x07), 0x10);

    Data32 = (Data32 & 0xfffffff0) + 0x001E44C0; //PRTPM_SAH  0x001e44c0
    gBS->CopyMem(TempBuffer + 5, (UINT8*)Data32, 2);

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
    UINT16	BDF, Bus, BusE;

    HardWareInfoData[3] = 0x00;
    BDF = SmcBDFTransfer(PcdGet32(PcdSmcOBLan1BDF));
    BusE = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                    (UINT8)(BDF & 0x07), 0x1A);
    Bus = *(UINT8*)MmPciAddress(0, (UINT8)(BDF >> 8), (UINT8)((BDF >> 3) & 0x1f),
                    (UINT8)(BDF & 0x07), 0x19);
    for( ; BusE > Bus; BusE--){
        if(*(UINT8*)MmPciAddress(0, BusE, 0, 0, 0) != 0xff)
            break;
    }
    BDF = BusE << 8;
    GetOnBoardLanMacAddress(BDF, (HardWareInfoData + 3));

    // LanMacInfoData parameter data is used by 0x9E command
    // IPMI 0x9E command data:
    // Byte 0    : LAN number N  (exclude NumLan of 0x30 0x20 command)
    // Byte 1-6  : LAN port M MAC address
    // Byte 7-12 : LAN port M+1 MAC address
    LanMacInfoData[0] = 0;

    BDF = (BusE << 8) + 1;
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
    DEBUG((-1, "DXE GetProjectPciEBifurcate Start.\n"));

    DEBUG((EFI_D_INFO, "DXE GetProjectPciEBifurcate End.\n"));
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   :  X11DDWSmbiosUpdate
//
// Description :  Update SMBIOS information after ReadyToBootEvent
//
// Returns     :  none
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
VOID
X11DDWSmbiosUpdate()
{
	DEBUG((EFI_D_INFO, "X11DDW SmbiosUpdate entry.\n"));
	DEBUG((EFI_D_INFO, "X11DDW SmbiosUpdate end.\n"));     
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
// Procedure:   X11DDWReadyToBootCallBack
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
X11DDWReadyToBootCallBack (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS	Status = EFI_SUCCESS;
    UINTN       VariableSize;
    SOCKET_POWERMANAGEMENT_CONFIGURATION SocketPowermanagmentConfiguration;
    EFI_IPMI_TRANSPORT  *mIpmiTransport;
    UINTN  RpDev, RpFun, RpBase;
    UINT8       i, count = 0, CommandRetry = 20;
    UINT8       Data, DataSize = 1;
    UINT8       MAX_POWER_EFFICIENT_VRM[][20] = {
                    {4, 0x30, 0x70, 0xdf, 0x00},                            //Disable sensor scan
                    {7, 0x06, 0x52, 0x01, 0xb0, 0x00, 0x00, 0x00},          //set page(CPU1)
                    {7, 0x06, 0x52, 0x01, 0xb0, 0x00, 0xd5, 0xe0},          //CPU_vout_offset = -60mv
                    {7, 0x06, 0x52, 0x01, 0xc0, 0x00, 0x00, 0x00},          //set page(CPU2)
                    {7, 0x06, 0x52, 0x01, 0xc0, 0x00, 0xd5, 0xe0},          //CPU_vout_offset = -60mv                   
                    {7, 0x06, 0x52, 0x01, 0xb8, 0x00, 0x00, 0x00},          //set page(MEM CHA)
                    {7, 0x06, 0x52, 0x01, 0xb8, 0x00, 0xd5, 0xe0},          //MEM_vout_offset = -60mv
                    {7, 0x06, 0x52, 0x01, 0xbc, 0x00, 0x00, 0x00},          //set page(MEM CHB)
                    {7, 0x06, 0x52, 0x01, 0xbc, 0x00, 0xd5, 0xe0},          //MEM_vout_offset = -60mv
                    {7, 0x06, 0x52, 0x01, 0xc8, 0x00, 0x00, 0x00},          //set page(MEM CHC)
                    {7, 0x06, 0x52, 0x01, 0xc8, 0x00, 0xd5, 0xe0},          //MEM_vout_offset = -60mv
                    {7, 0x06, 0x52, 0x01, 0xcc, 0x00, 0x00, 0x00},          //set page(MEM CHD)
                    {7, 0x06, 0x52, 0x01, 0xcc, 0x00, 0xd5, 0xe0},          //MEM_vout_offset = -60mv
                    {4, 0x30, 0x70, 0xdf, 0x01},                            //Enable sensor scan
                    {0}};
    UINT8       Original_VRM[][20] = {   
                    {4, 0x30, 0x70, 0xdf, 0x00},                            //Disable sensor scan
                    {7, 0x06, 0x52, 0x01, 0xb0, 0x00, 0x00, 0x00},          //set page(CPU1)
                    {7, 0x06, 0x52, 0x01, 0xb0, 0x00, 0xd5, 0x00},          //CPU_vout_offset = 0mv
                    {7, 0x06, 0x52, 0x01, 0xc0, 0x00, 0x00, 0x00},          //set page(CPU2)
                    {7, 0x06, 0x52, 0x01, 0xc0, 0x00, 0xd5, 0x00},          //CPU_vout_offset = 0mv                   
                    {7, 0x06, 0x52, 0x01, 0xb8, 0x00, 0x00, 0x00},          //set page(MEM CHA)
                    {7, 0x06, 0x52, 0x01, 0xb8, 0x00, 0xd5, 0x00},          //MEM_vout_offset = 0mv
                    {7, 0x06, 0x52, 0x01, 0xbc, 0x00, 0x00, 0x00},          //set page(MEM CHB)
                    {7, 0x06, 0x52, 0x01, 0xbc, 0x00, 0xd5, 0x00},          //MEM_vout_offset = 0mv
                    {7, 0x06, 0x52, 0x01, 0xc8, 0x00, 0x00, 0x00},          //set page(MEM CHC)
                    {7, 0x06, 0x52, 0x01, 0xc8, 0x00, 0xd5, 0x00},          //MEM_vout_offset = 0mv
                    {7, 0x06, 0x52, 0x01, 0xcc, 0x00, 0x00, 0x00},          //set page(MEM CHD)
                    {7, 0x06, 0x52, 0x01, 0xcc, 0x00, 0xd5, 0x00},          //MEM_vout_offset = 0mv
                    {4, 0x30, 0x70, 0xdf, 0x01},                            //Enable sensor scan
                    {0}};                       

    DEBUG((EFI_D_INFO, "X11DDWReadyToBootCallBack Entry.\n"));
    
    X11DDWSmbiosUpdate();

    Status = gBS->LocateProtocol(
                        &gEfiDxeIpmiTransportProtocolGuid, 
                        NULL,
                        &mIpmiTransport);

    if(Status)  return;

    VariableSize = sizeof(SOCKET_POWERMANAGEMENT_CONFIGURATION);
    Status = gRT->GetVariable(
                    SOCKET_POWERMANAGEMENT_CONFIGURATION_NAME,
                    &gEfiSocketPowermanagementVarGuid,
                    NULL,
                    &VariableSize,
                    &SocketPowermanagmentConfiguration);
    DEBUG((-1, "GetVariable Status = %r.\n", Status));
    if(Status)  return;

    if(SocketPowermanagmentConfiguration.AltEngPerfBIAS == 0x0e){
        DEBUG((-1, "Set MAX POWER EFFICIENT VRM.\n"));
        count = 0;
        while(MAX_POWER_EFFICIENT_VRM[count][0]){
            count++;
        }

        for(i = 0; i < count; i++){
            while(CommandRetry){
                Status = mIpmiTransport->SendIpmiCommand(
                                mIpmiTransport,
                                MAX_POWER_EFFICIENT_VRM[i][1],
                                0,
                                MAX_POWER_EFFICIENT_VRM[i][2],
                                &MAX_POWER_EFFICIENT_VRM[i][3],
                                MAX_POWER_EFFICIENT_VRM[i][0] - 2,
                                &Data,
                                &DataSize);    
                DEBUG((-1, "SendIpmiCommand = %r.\n", Status));
                if(!Status)     //return success
                    break;
                else
                    DEBUG((-1, "SendIpmiCommand retry.\n"));
                gBS->Stall(100000);     // delay 100000 microseconds
                CommandRetry--;
            }
        }
        for (i = 0; i < GetPchMaxPciePortNum (); i++) {
          if (GetPchPcieRpDevFun (i, &RpDev, &RpFun) == EFI_SUCCESS)
          {
        	  RpBase = MmPciBase (0, (UINT32)RpDev, (UINT32)RpFun);
        	  MmioOr32 (RpBase + 0x324, 8);  //R_PCH_PCIE_PCIEDBG 0x324   B_PCH_PCIE_DMIL1EDM BIT3 	  
          }
        }
    } else {
        DEBUG((-1, "Set Original VRM.\n"));
        count = 0;
        while(Original_VRM[count][0]){
            count++;
        }

        for(i = 0; i < count; i++){
            while(CommandRetry){
                Status = mIpmiTransport->SendIpmiCommand(
                                mIpmiTransport,
                                Original_VRM[i][1],
                                0,
                                Original_VRM[i][2],
                                &Original_VRM[i][3],
                                Original_VRM[i][0] - 2,
                                &Data,
                                &DataSize);    
                DEBUG((-1, "SendIpmiCommand = %r.\n", Status));
                if(!Status)     //return success
                    break;
                else
                    DEBUG((-1, "SendIpmiCommand retry.\n"));
                gBS->Stall(100000);     // delay 100000 microseconds
                CommandRetry--;
            }
        }
        for (i = 0; i < GetPchMaxPciePortNum (); i++) {
          if (GetPchPcieRpDevFun (i, &RpDev, &RpFun) == EFI_SUCCESS)
          {
        	  RpBase = MmPciBase (0, (UINT32)RpDev, (UINT32)RpFun);
        	  MmioAnd32 (RpBase + 0x324, 0xFFFFFFF7);  //R_PCH_PCIE_PCIEDBG 0x324   B_PCH_PCIE_DMIL1EDM BIT3 	  
          }
        }
    }
    gBS->CloseEvent(Event);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : X11DDWDriverInit
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
X11DDWDxeDriverInit(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;
    EFI_EVENT	X11DDWReadyToBootEvent, HideItemEvent;
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
		    X11DDWReadyToBootCallBack,
		    NULL,
		    &guidReadyToBoot,
		    &X11DDWReadyToBootEvent);

    Status = gBS->CreateEvent(
                          EVT_NOTIFY_SIGNAL,
                          TPL_CALLBACK,
                          X11DDWHideSetupItem,
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
