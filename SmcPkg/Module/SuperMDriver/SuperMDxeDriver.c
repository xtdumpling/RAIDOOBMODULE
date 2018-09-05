//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.06
//    Bug Fix : Add OnBoard LAN drop check function.
//    Reason  : Remove LAN DID check.
//    Auditor : Kasber Chen
//    Date    : Jul/31/2017
//
//  Rev. 1.05
//    Bug Fix : Stopped TCO timer under EFI shell to avoid the system being 
//              rebooted since some EFI shell applications need more time to 
//              finish specific task.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jul/06/2017
//
//  Rev. 1.04
//    Bug Fix : Added PC 0xB2 W/A for system stops at PXE option ROM.
//              (Refer to Grangeville Trunk revision #32.)
//    Reason  : Bug Fixed
//    Auditor : Joe Jhang
//    Date    : Jan/17/2017
//
//  Rev. 1.03
//    Bug Fix:  Send HideItem and RiserCard data to sum.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Sep/23/2016
//
//  Rev. 1.02
//    Bug Fix:  Add riser card name to IIO and OPROM control items.
//    Reason:     
//    Auditor:  Kasber Chen
//    Date:     Jun/21/2016
//
//  Rev. 1.01
//    Bug Fix:  Use AMIBCP to disable boot procedure messages displaying.
//    Reason:   
//    Auditor:  Hartmann Hsieh
//    Date:     Jun/21/2016
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/18/2014
//
//****************************************************************************
//****************************************************************************
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		SuperMDxeDriver.c
//
// Description:
//  This file contains code for SuperMDriver Dxe functions.  The
//  functions call Project Dxe driver to get board level information.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include <Setup.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Guid\HobList.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <SuperMDXEDriver.h>
#include "SuperMHob.h"
#if SmcRomHole_SUPPORT
#include "SmcRomHole.h"
#endif
#include "SmcLib.h"
#include <SmcDxeWorkaround.h>

#define NextHob(Hob,Type) ((Type*)((UINT8*)Hob + ((EFI_HOB_GENERIC_HEADER*)Hob)->HobLength))

EFI_GUID gHobListGuid = HOB_LIST_GUID;

extern EFI_GUID gSuperMDriverProtocolGuid;
extern EFI_GUID gProjectOwnProtocolGuid;

//----------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------
// Variable Declaration(s)

static PROJECTDRIVER_PROTOCOL	*mProjectInterface;
static SUPERMDRIVER_PROTOCOL 	*SuperMInterface;


EFI_STATUS
FindNextHobByType(
    IN	UINT16	Type,
    IN OUT	VOID	**Hob
)
{
    EFI_HOB_GENERIC_HEADER	*ThisHob;

    if(Hob == NULL)	return EFI_INVALID_PARAMETER;
	ThisHob = *Hob;
    while(ThisHob->HobType != EFI_HOB_TYPE_END_OF_HOB_LIST){
	ThisHob=NextHob(ThisHob,EFI_HOB_GENERIC_HEADER);
	if(ThisHob->HobType==Type){
	    *Hob=ThisHob;
	    return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : GetSuperMBoardInfoCode
//
// Description : 1.Get SuperM Board Information from both SMCHOBINFO and SMC_PDR_DATA_HOB
//				 2.Get board level information from Board Dxe driver
//
// Parameters  :  Out: SuperMBoardInfoData
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
GetSuperMBoardInfoCode(
    OUT	SUPERMBOARDINFO	*SuperMBoardInfoData
)
{
    EFI_STATUS	Status;
    UINT8	i;
    UINT16	DID_Temp;
    VOID	*FirstHob;
    SMCHOBINFO	*pSHob;

    DEBUG((EFI_D_INFO, "GetSuperMBoardInfoCode.\n"));

    // Get HOB data sample code
    Status = EfiGetSystemConfigurationTable(&gHobListGuid, &FirstHob);
    if(!FirstHob){
	return Status = EFI_SUCCESS;
    }

    pSHob = (SMCHOBINFO*)FirstHob;

    //
    // Get SMC board info HOB entry.
    //

    while(!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION,&pSHob))){
	if(CompareGuid(&pSHob->EfiHobGuidType.Name,&gSmcBoardInfoHobGuid) == 1)
	    break;
    }

    if(EFI_ERROR(Status)){
	return Status = EFI_SUCCESS;
    }

    //
    // Copy PEI data to DXE board information
    //

    gBS->CopyMem(SuperMBoardInfoData->HardwareInfo, pSHob->HardwareInfo, sizeof(pSHob->HardwareInfo));
    gBS->CopyMem(SuperMBoardInfoData->SystemConfiguration, pSHob->SystemConfiguration, sizeof(pSHob->SystemConfiguration));
    SuperMBoardInfoData->MBSVID = pSHob->MBSVID;
    SuperMBoardInfoData->GPU_AdjustVID_Support = pSHob->GPU_AdjustVID_Support;
    SuperMBoardInfoData->BMC_Present = pSHob->BMC_Present;
    SuperMBoardInfoData->PCB_Revision = pSHob->PCB_Revision;

    //
    // Each Project get Board information routine
    //

    mProjectInterface->GetProjectHardwareInfo(SuperMBoardInfoData->HardwareInfo, SuperMBoardInfoData->LanMacInfo);

    if(mProjectInterface->CheckAdjustVID() && mProjectInterface->GetGPUDID != NULL){
	for(i = 0 ; i < 4 ; i ++){
	    DID_Temp = mProjectInterface->GetGPUDID(i);
	    SuperMBoardInfoData->BMCCMD70_SetGPU_DID[i][0] = 0x32;
	    SuperMBoardInfoData->BMCCMD70_SetGPU_DID[i][1] = i;
	    SuperMBoardInfoData->BMCCMD70_SetGPU_DID[i][2] = (UINT8) (DID_Temp & 0xFF);
	    SuperMBoardInfoData->BMCCMD70_SetGPU_DID[i][3] = (UINT8) ((DID_Temp >> 8) & 0xFF);
	}
    }

    DEBUG((EFI_D_INFO, "Exit GetSuperMBoardInfoCode.\n"));

    return EFI_SUCCESS;
}

/*-----------------12/23/2014 8:58PM----------------
 * Move to SuperMSetup
 * --------------------------------------------------*/

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SetSuperMSetupString
//
// Description : 1.Get HiiHandle, InitString function point fro outside.
//		 2.Set board name string token "STR_SMC_BOARD_NAME" for showing in setup.
//
// Parameters  : Intput: HiiHandle, InitString function point
//
// Returns     : EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS 
SetSuperMSetupString(
    IN EFI_HII_HANDLE   HiiHandle,
    IN UINT16           Class,
    IN InitStringSmc	InitString
)
{
    CHAR16 *PcdDefaultString=L"To be filled by O.E.M.To be filled by O.E.M.To be filled by O.E.M.";
    
    DEBUG((-1, "SetSuperMSetupString entry.\n"));

    if(StrCmp((CHAR16*)PcdGetPtr(PcdBaseBoardManufacturer), PcdDefaultString) == 0x00 ||
	    StrCmp((CHAR16*)PcdGetPtr(PcdBaseBoardProductName), PcdDefaultString) == 0x00)
    return EFI_INVALID_PARAMETER;
    	
    DEBUG((-1, "SetSuperMSetupString end.\n"));
    
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : GetSuperMPciEBifuracate
//
// Description : Get IIO PCIE Bifuracate setting
//
// Parameters  :  IN OUT IIO_GLOBALS *IioGlobalData
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
GetSuperMPciEBifuracate(
    IN OUT	IIO_CONFIG	*SetupData
)
{	
    if(mProjectInterface->GetProjectPciEBifurcate != NULL)
	mProjectInterface->GetProjectPciEBifurcate(SetupData);

    return EFI_SUCCESS;
}

/*-----------------12/23/2014 4:32PM----------------
 * Coding for every platform
 * --------------------------------------------------*/
/*
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SetSuperMPciECTLE
//
// Description : Set IIO PCIE CTLE setting
//
// Parameters  :  IN OUT IIO_GLOBALS *IioGlobalData
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS SetSuperMPciECTLE (
    IN OUT IIO_GLOBALS *IioGlobalData,
    IN UINT8	Iio,
    IN UINT8	BusNumber
)
{
//    Chipset relative coding	
    if(mProjectInterface->SetProjectPciECTLE != NULL)
	mProjectInterface->SetProjectPciECTLE(IioGlobalData, Iio, BusNumber);
    return EFI_SUCCESS;
}

*/		
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SuperMDxeDriverInit
//
// Description : SuperM Dxe Driver init
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
SuperMDxeDriverInit(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{

    EFI_STATUS	Status;
    EFI_HANDLE	Handle = NULL;

    DEBUG((EFI_D_INFO, "SuperMDriver Init.\n"));

    //
    // Display afu command data in NCB.
    //
#if 0
//#if defined DEBUG_MODE && DEBUG_MODE == 0x01
{
    UINT8	*TempData, StrBuff[17];
    UINT32	i = 0;

    gBS->SetMem(StrBuff, 17, 0);
    SmcRomHoleReadRegion(AFU_CMD_REGION, TempData);
    DEBUG((EFI_D_INFO, "The first 256 bytes afu command data in NCB : \n"));
    for(i = 0; i < 256; i++){
	if(i != 0 && i % 16 == 0){
	    DEBUG((EFI_D_INFO, "  %s\n", StrBuff));
	    MemSet (StrBuff, 17, 0);
	}
	StrBuff[i % 16] = TempData[i];
	DEBUG((EFI_D_INFO, "%02x ", TempData[i]));
    }
    DEBUG((-1, "\n"));
}
#endif

    Status = gBS->LocateProtocol(&gProjectOwnProtocolGuid , NULL, &mProjectInterface);
    ASSERT_EFI_ERROR(Status);

    if(Status == EFI_SUCCESS){
	DEBUG((EFI_D_INFO, "Locate protocol success\n"));

	Status = gBS->AllocatePool(
			EfiBootServicesData,
			sizeof(SUPERMDRIVER_PROTOCOL),
			&SuperMInterface);

	ASSERT_EFI_ERROR(Status);

	Status = gBS->InstallProtocolInterface(
			&Handle,
			&gSuperMDriverProtocolGuid,
			EFI_NATIVE_INTERFACE,
			SuperMInterface);

	SuperMInterface->GetSuperMBoardInfo = GetSuperMBoardInfoCode;
//	SuperMInterface->GetSuperMHiddenItem = NULL;
//#if defined SetupMenuHideItemNumber
//	SuperMInterface->GetSuperMHiddenItem = GetSuperMHiddenItemList;
//#endif
	SuperMInterface->GetSuperMPciEBifuracate = GetSuperMPciEBifuracate;
//	SuperMInterface->SetSuperMPciECTLE = SetSuperMPciECTLE;
	SuperMInterface->SetSuperMSetupString = SetSuperMSetupString;
	//SuperMInterface->SetIntelRCSetupString = SetIntelRCSetupString;

	ASSERT_EFI_ERROR( Status );
    }
#if B2_WORKAROUND_SUPPORT
    SmcDxeB2Workaround();
#endif
#if ONBORAD_LAN_DROP_CHECK
    SmcDxeOnboardLanWorkaround();
#endif
    DEBUG((EFI_D_INFO, "Exit SuperMDriver Init.\n"));
    return Status;
}
