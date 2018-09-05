//****************************************************************************
//****************************************************************************
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.18
//    Bug Fix : N/A
//    Reason  : Remove guid that inform InBand prepare collect data, since the time sequence cannot immediately. 
//			  : Add SMC LSI Setup protocol to inform SMC LSI setup driver start. 
//    Auditor : Durant Lin
//    Date    : Aug/27/2018
//
//  Rev. 1.17
//    Bug Fix : N/A
//    Reason  : Add a guid to inform InBand prepare collect data.
//    Auditor : Durant Lin
//    Date    : Aug/14/2018
//
//  Rev. 1.16
//    Bug Fix:  
//    Reason:   Add a function hook before Update Bios Cfg to determine should update BIOS cfg.
//    Auditor:  Durant Lin
//    Date:     Jun/25/2018
//
//  Rev. 1.15
//    Bug Fix:  
//    Reason:   Modify Debug message.
//    Auditor:  Durant Lin
//    Date:     Jun/01/2018
//
//  Rev. 1.14
//    Bug Fix : N/A
//    Reason  : Use Oob Library Protocol to replace some library functions.
//    Auditor : Durant Lin
//    Date    : Feb/12/2018
//
//  Rev. 1.13
//    Bug Fix:  Remove unuse a7printflib.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Feb/07/2018
//
//  Rev. 1.12
//    Bug Fix:  Add a interface to hook updatevariable before.
//    Reason:
//    Auditor:  Durant Lin
//    Date:     Jan/17/2018
//
//  Rev. 1.11
//    Bug Fix : N/A
//    Reason  : Add a version control string for all OOB EFI files. "_SMCOOBV1.xx.xx_"
//    Auditor : Durant Lin
//    Date    : Jan/04/2018
//
//  Rev. 1.10
//    Bug Fix:  Fix HII data changing after sum load BIOS default. Move BIOS config combination later.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Mar/02/2017
//
//  Rev. 1.09
//    Bug Fix:  Remove module - SmcBuildTimeData.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Oct/13/2016
//
//  Rev. 1.08
//    Bug Fix:  Trigger a software SMI to flash ROM part after BIOS lock.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/30/2016
//
//  Rev. 1.07
//    Bug Fix:  Store Bios Config at SMRAM for InBand.
//    Reason:   BIOS Config binary is too big to save in SmcRomHole.
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/22/2016
//
//  Rev. 1.05
//    Bug Fix:  Calculate HII data checksum and fill to header.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/09/2016
//
//  Rev. 1.04
//    Bug Fix:  Add Full SmBios InBand support.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Aug/05/2016
//
//  Rev. 1.03
//    Bug Fix:  Add InBand HII data for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/21/2016
//
//  Rev. 1.02
//    Bug Fix:  Add BIOS config InBand function for sum.
//    Reason:
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/18/2016
//
//  Rev. 1.01
//    Bug Fix:  Fix system hangs at Sum InBand command.
//    Reason:   Because of using Boot Service in SMM. Separate SmcRomHole into two libraries - DXE and SMM.
//    Auditor:  Hartmann Hsieh
//    Date:     Jul/12/2016
//
//  Rev. 1.00
//    Bug Fix:  Initial revision.
//    Reason:
//    Auditor:  Kasber Chen
//    Date:     May/26/2016
//
//****************************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:        SmcInBand.c
//
// Description: SMC In-Band feature include BIOS config and SMBIOS.
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include "Protocol/AmiSmbios.h"
#include <Protocol/SmmControl2.h>
#include "SspTokens.h"
#include "TimeStamp.h"
#include "SmcLib.h"
#include "SmcInBand.h"
#include "SmcInBandHii.h"
#include "SmcInBandFullSM.h"
#include <Guid/SmcOobPlatformPolicy.h>
#include <Protocol/SmcRomHoleProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/SmcOobLibraryProtocol.h>
#include <Protocol/SmcLsiRaidOOBSetupDriver.h>

#include "SmcOobVersion.h"


AMI_SMBIOS_PROTOCOL	*SmbiosProtocol;
UINT8* pBuffer = NULL;

SMC_OOB_PLATFORM_POLICY      		*mSmcOobPlatformPolicyPtr;
SMC_ROM_HOLE_PROTOCOL        		*mSmcRomHoleProtocol;
SMC_OOB_LIBRARY_PROTOCOL		  	*mSmcOobLibraryProtocol;
SMC_LSI_RAID_OOB_SETUP_DRIVER		*mSmcLsiRaidOOBSetupDriver = NULL;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  SmcInBandCallback
//
// Description:
//  Smbios callback for BDS_CONNECT_DRIVERS_PROTOCOL_GUID.
//
//  Steps :
//  1. Locate flash and Smbios protocol.
//  2. Update the BIOS configuration from user to system(InBandUpdateBiosCfg).
//  3. Read SmbiosData from ROM and update it(InBandUpdateSmbios).
//  4. Combine SmBios and update to ROM(SMBIOS default, current and previous).
//
// Input:
//      IN EFI_EVENT Event - Event.
//      IN VOID *Context - Context.
//
// Output:
//      None
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

VOID
SmcInBandCallback(
    IN	EFI_EVENT	Event,
    IN	VOID		*Context
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
    DEBUG((-1, "[SMC_OOB] :: SmcInBandCallback entry.\n"));

    pBS->CloseEvent(Event);

	Status = pBS->LocateProtocol(&gSmcLsiRaidOobSetupDriverGuid,NULL,&mSmcLsiRaidOOBSetupDriver);
	DEBUG((-1,"[SMC_OOB] :: LocateProtocol gSmcLsiRaidOobSetupDriver Status[%r]\n",Status));

	if(!!mSmcLsiRaidOOBSetupDriver){
		Status = mSmcLsiRaidOOBSetupDriver->SmcLsiSetupDriverStart(mSmcLsiRaidOOBSetupDriver);
		DEBUG((-1,"[SMCOOB] :: mSmcLsiRaidOOBSetupDriver SmcLsiSetupDriverStart Status[%r]\n",Status));
		if(!EFI_ERROR(Status)){
			Status = mSmcLsiRaidOOBSetupDriver->SmcLsiSetupDriverDownload(mSmcLsiRaidOOBSetupDriver);
			DEBUG((-1,"[SMCOOB] :: mSmcLsiRaidOOBSetupDriver SmcLsiSetupDriverDownload Status[%r]\n",Status));
		}
	}
	if((!!mSmcOobLibraryProtocol->Smc_OobDetermineUpdBiosCfg))
    	InBandUpdateBiosCfg();
    
	InBandUpdateSmbios();

    DEBUG((-1, "[SMC_OOB] :: SmcInBandCallback end.\n"));
}

//
// 'Setup' HPK is generated later. Move the code to ready to boot.
//
VOID
SmcInBandReadyToBoot (
    IN EFI_EVENT Event,
    IN VOID *Context
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 BufferSize = 0;
    EFI_SMM_CONTROL2_PROTOCOL *gSmmCtl = NULL;
    UINT8 SwSmiValue = mSmcOobPlatformPolicyPtr->OobConfigurations.InBandSwSmiE8;
    UINT8 DataPort = SWSMI_E8_SUBCMD_COMBINE_HII;
    EFI_HANDLE Handle = NULL;
    UINT32 Data32;


    pBS->CloseEvent(Event);

    DEBUG((-1, "[SMC_OOB] :: SmcInBandReadyToBoot entry.\n"));


    //
    // Combine BiosCfg data.
    //
    BufferSize = 0;
    CombineBiosCfg (pBuffer + SMC_OOB_SMRAM_BIOS_CFG_OFFSET, &BufferSize);
    if (BufferSize > SMC_OOB_SMRAM_BIOS_CFG_SIZE) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : BIOS Cfg data size exceeds map define.\n"));
    }
    DEBUG((-1, "[SMC_OOB] :: BIOS_CFG Total size = 0x%x\n", BufferSize));

    //
    // Trigger a software SMI to copy BIOS Cfg data to SMRAM.
    //
    Status = pBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, &gSmmCtl);
    if (!EFI_ERROR(Status)) {

        //
        // Save the buffer address to CMOS. The address can be passed to SMI by CMOS.
        //
        Data32 = (UINT32)pBuffer;
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrHH, (Data32 >> 24) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrH, (Data32 >> 16) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrL, (Data32 >> 8) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrLL, Data32 & 0xff);

        DataPort = SWSMI_E8_SUBCMD_COMBINE_BIOS_CFG;
        gSmmCtl->Trigger(gSmmCtl, &SwSmiValue, &DataPort, 0, 0);
    }

    //
    // Combine Smbios data.
    //
    CombineSmbios();

    //
    // Combine HII data.
    //
    BufferSize = 0;
	
	if(!!mSmcLsiRaidOOBSetupDriver){
		Status = mSmcLsiRaidOOBSetupDriver->SmcLsiSetupDriverLoadR(mSmcLsiRaidOOBSetupDriver);
		DEBUG((-1,"[SMC_OOB] :: mSmcLsiRaidOOBSetupDriver SmcLsiSetupDriverLoadR Status[%r]\n",Status));
	}


    CombineHiiData (pBuffer + SMC_OOB_SMRAM_HII_OFFSET, &BufferSize);
    if (BufferSize > SMC_OOB_SMRAM_HII_SIZE) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : HII data size exceeds map define.\n"));
    }
    DEBUG((-1, "[SMC_OOB] :: HII Total size = 0x%x\n", BufferSize));

    //
    // Combine FullSmBios data.
    //
    BufferSize = 0;
    CombineFullSmBios (pBuffer + SMC_OOB_SMRAM_FULL_SMBIOS_OFFSET, &BufferSize);
    if (BufferSize > SMC_OOB_SMRAM_FULL_SMBIOS_SIZE) {
        DEBUG((-1, "[SMC_OOB] :: ERROR : Full SmBios data size exceeds map define.\n"));
    }
    DEBUG((-1, "[SMC_OOB] :: FULL_SMBIOS Total size = 0x%x\n", BufferSize));

    //
    // Trigger a software SMI to copy HII data to SMRAM.
    //
    Status = pBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, &gSmmCtl);
    if (!EFI_ERROR(Status)) {

        //
        // Save the buffer address to CMOS. The address can be passed to SMI by CMOS.
        //
        Data32 = (UINT32)pBuffer;
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrHH, (Data32 >> 24) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrH, (Data32 >> 16) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrL, (Data32 >> 8) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrLL, Data32 & 0xff);

        DataPort = SWSMI_E8_SUBCMD_COMBINE_HII;
        gSmmCtl->Trigger(gSmmCtl, &SwSmiValue, &DataPort, 0, 0);
    }

    //
    // Trigger a software SMI to copy FULL SmBios data to SMRAM.
    //
    Status = pBS->LocateProtocol(&gEfiSmmControl2ProtocolGuid, NULL, &gSmmCtl);
    if (!EFI_ERROR(Status)) {

        //
        // Save the buffer address to CMOS. The address can be passed to SMI by CMOS.
        //
        Data32 = (UINT32)pBuffer;
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrHH, (Data32 >> 24) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrH, (Data32 >> 16) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrL, (Data32 >> 8) & 0xff);
		mSmcOobLibraryProtocol->Smc_OobSetCmosTokensValue(mSmcOobPlatformPolicyPtr->OobConfigurations.CmosSmcAddrLL, Data32 & 0xff);

        DataPort = SWSMI_E8_SUBCMD_COMBINE_FULL_SMBIOS;
        gSmmCtl->Trigger(gSmmCtl, &SwSmiValue, &DataPort, 0, 0);
    }

    //
    // Signaling gSmcOobDataReadyProtocolGuid Event
    //
    pBS->InstallProtocolInterface (
        &Handle,
        &gSmcOobDataReadyProtocolGuid,
        EFI_NATIVE_INTERFACE,
        NULL
    );

    DEBUG((-1, "[SMC_OOB] :: SmcInBandReadyToBoot end.\n"));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Name:  InitSmcInBand
//
// Description:
//  Init SmcInBand.
//  Define a SMC software SMI to handle SMC SW (SUM) for date transfer butween
//  BIOS and SUM in Linux.
//
// Input:
//      IN EFI_HANDLE ImageHandle
//                  - Image handle
//      IN EFI_SYSTEM_TABLE *SystemTable
//                  - System table pointer
//
// Output:
//      EFI_STATUS
//          EFI_SUCCESS - Success
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
InitSmcInBand(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS	Status;
    EFI_EVENT	SmEvent;
    VOID        *SmReg;
    EFI_EVENT ReadyToBootEvent;
    SMC_BOARD_INFO BoardInfo;
    UINTN VariableSize;
    SMC_BOARD_INFO OldBoardInfo;

    DEBUG((-1, "[SMC_OOB] :: InitSmcInBand entry.\n"));

    InitAmiLib(ImageHandle, SystemTable);

    Status = pBS->LocateProtocol (&gSmcRomHoleProtocolGuid, NULL, &mSmcRomHoleProtocol);
    if (EFI_ERROR (Status)) {
		DEBUG((-1,"[SMC_OOB] :: Locate SmcRomHoleProtocol %g failed! Status = %r\n",gSmcRomHoleProtocolGuid,Status));
		return EFI_NOT_READY; 
    }

    Status = pBS->LocateProtocol (&gSmcOobPlatformPolicyGuid, NULL, &mSmcOobPlatformPolicyPtr);
    if (EFI_ERROR (Status)) {
		DEBUG((-1,"[SMC_OOB] :: Locate SmcOobPlatformPolicy %g failed! Status = %r\n",gSmcOobPlatformPolicyGuid,Status));
      return EFI_NOT_READY; 
    }

	Status = pBS->LocateProtocol (&gSmcOobLibraryProtocolGuid, NULL, &mSmcOobLibraryProtocol);
    if (EFI_ERROR (Status)) {
      DEBUG((-1, "[SMC_OOB] :: OOB_DEBUG : SMC_OOB_LIBRARY_PROTOCOL is not ready.\n"));
      return EFI_NOT_READY; 
    }

    //
    // Update variable - SMC_BOARD_INFO_NAME
    //
    *(UINT32*)BoardInfo.SVID = mSmcOobPlatformPolicyPtr->OobConfigurations.BoardInfoSvid;
    *(UINT32*)BoardInfo.Date = mSmcOobPlatformPolicyPtr->OobConfigurations.BoardInfoDate;
    *(UINT16*)BoardInfo.Time = mSmcOobPlatformPolicyPtr->OobConfigurations.BoardInfoTime;

    VariableSize = sizeof (SMC_BOARD_INFO);
    Status = pRS->GetVariable(
                 L"SMC_BOARD_INFO_NAME",
                 &gSmcBoardInfoVariableGuid,
                 NULL,
                 &VariableSize,
                 (UINT8*)(&OldBoardInfo));
    if( EFI_ERROR (Status) ||
        MemCmp ((UINT8*)(&OldBoardInfo), (UINT8*)(&BoardInfo), sizeof (SMC_BOARD_INFO)) != 0)
    {
        VariableSize = sizeof (SMC_BOARD_INFO);
        Status = pRS->SetVariable(
                     L"SMC_BOARD_INFO_NAME",
                     &gSmcBoardInfoVariableGuid,
                     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                     sizeof(SMC_BOARD_INFO),
                     &BoardInfo);

		DEBUG((-1,"[SMC_OOB] :: Set BoardInfo SVID %x, Date %x, Time %x Status = %r\n",*(UINT32*)BoardInfo.SVID,*(UINT32*)BoardInfo.Date,*(UINT16*)BoardInfo.Time,Status));
    }

    Status = pBS->LocateProtocol(
                 &gAmiSmbiosProtocolGuid,
                 NULL,
                 &SmbiosProtocol);

	if(EFI_ERROR(Status)){
		DEBUG((-1,"[SMC_OOB] :: Locate gAmiSmbiosProtocolGuid %g failed! Status = %r\n",gAmiSmbiosProtocolGuid,Status));
		return EFI_NOT_READY;
	}

    Status = pBS->CreateEvent(
                 EVT_NOTIFY_SIGNAL,
                 TPL_CALLBACK,
                 SmcInBandCallback,
                 NULL,
                 &SmEvent);
    if(!EFI_ERROR (Status)) {
        Status = pBS->RegisterProtocolNotify(
                     &gSmcOobPlatformPolicyCallbackGuid,
                     SmEvent,
                     &SmReg);
		DEBUG((-1,"[SMC_OOB] :: Register SmcInBandCallback in Protocol Notify gSmcOobPlatformPolicyCallbackGuid %g Status = %r\n",gSmcOobPlatformPolicyCallbackGuid,Status));
    }

    Status = CreateReadyToBootEvent (
                 TPL_CALLBACK,
                 SmcInBandReadyToBoot,
                 NULL,
                 &ReadyToBootEvent);

    //
    // Create a buffer for SMI and DXE communication.
    //
    Status = pBS->AllocatePool (EfiBootServicesData, SMC_OOB_SMRAM_TOTAL_SIZE, &pBuffer);

    //
    // Clear the buffer
    //
    MemSet (pBuffer, SMC_OOB_SMRAM_TOTAL_SIZE, 0);

    DEBUG((-1, "[SMC_OOB] :: InitSmcInBand End.\n"));
    return EFI_SUCCESS;
}
