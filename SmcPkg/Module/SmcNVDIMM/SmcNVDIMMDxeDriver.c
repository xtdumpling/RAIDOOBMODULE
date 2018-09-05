//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.01
//    Bug Fix:  Improve code.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Aug/16/2017
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/19/2014
//
//****************************************************************************
//****************************************************************************
#include <Token.h>
#include <AmiDxeLib.h>
#include <Library\UefiBootServicesTableLib.h>
#include <SmcLib.h>
#include <Library/DebugLib.h>
#include <Guid\EventGroup.h>
#include <SspTokens.h>
#include <SmcLibCommon.h>
#include "Library\GpioLib.h"
#include "Setup.h"
#include <AmiCompatibilityPkg\Include\AcpiRes.h>

#include <Library/PcdLib.h>
#include <Library/SetupLib.h>

#if SMC_CPLD_SUPPORT
#include "SmcCPLDInterface.h"
#endif

EFI_GUID	SetupGuid = SETUP_GUID;
UINTN		VarSize = sizeof(SETUP_DATA);
SETUP_DATA	mSetupData;
INTEL_SETUP_DATA	mIntelRCSetupData;

#if SMC_CPLD_SUPPORT
VOID SetCPLD (
    IN	EFI_EVENT	Event,
    IN	VOID		*Context
    )
{
	EFI_STATUS Status = EFI_SUCCESS;
	SMCCPLDFN* cSmcCpldFn = NULL;
	
	//UINT32	byteFn;
	
	Status = pBS->LocateProtocol (&gSmcCPLDProtocolGuid, NULL, (VOID**) &cSmcCpldFn); ASSERT_EFI_ERROR(Status);
	
	if ( PcdGet64(PcdSmcNVDIMM) & BIT0 ) {
		TRACE(( -1, "SetCPLD\n"));
		Status = cSmcCpldFn->SetADR(TRUE, mIntelRCSetupData.SocketConfig.MemoryConfig.SmcGlobalResetEn);
		TRACE(( -1, "Set ADR status %r\n", Status));
	}
	else {
		TRACE(( -1, "SetCPLD\n"));
		Status = cSmcCpldFn->SetADR(FALSE, FALSE);
		TRACE(( -1, "Set ADR status %r\n", Status));
	}

	/*
	byteFn = 0;
	byteFn = mSetupData.ThrottleOnPowerFail ? ( byteFn | (1<<0) ) : ( byteFn | (0<<0) );
	TRACE(( -1, "ThrottleOnPowerFail = 0x%x, byteFn = 0x%x\n", mSetupData.ThrottleOnPowerFail, byteFn));
	Status = cSmcCpldFn->SetCPLDBYBMC(byteFn);
	*/
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   ProjectReadyToBootCallBack
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
SmcNVDIMMReadyToBootCallBack (
    IN	EFI_EVENT	Event,
    IN	VOID		*Context
)
{
	UINT64 iPcdNVDIMM = PcdGet64(PcdSmcNVDIMM);

	SetCmosTokensValue(SMC_NVDIMM_PCD_0, (UINT8)iPcdNVDIMM);
	SetCmosTokensValue(SMC_NVDIMM_PCD_1, (UINT8)(iPcdNVDIMM>>8));
	SetCmosTokensValue(SMC_NVDIMM_PCD_2, (UINT8)(iPcdNVDIMM>>16));
	SetCmosTokensValue(SMC_NVDIMM_PCD_3, (UINT8)(iPcdNVDIMM>>24));

/*
#if SMC_CPLD_SUPPORT
	SetCPLD();
#endif
*/

}

//<SMC_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : ProjectDxeDriverInit
//
// Description : Setting the power status functio
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<SMC_PHDR_START>
EFI_STATUS
SmcNVDIMMDxeDriverInit(
    IN	EFI_HANDLE			ImageHandle,
    IN	EFI_SYSTEM_TABLE		*SystemTable
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_EVENT 	oEvent;
#if SMC_CPLD_SUPPORT
	EFI_EVENT 	cpld_event;
	EFI_HANDLE	cpld_event_handle = NULL;
	SMCCPLDFN* cSmcCpldFn = NULL;
#endif

	static EFI_GUID guidReadyToBoot = EFI_EVENT_GROUP_READY_TO_BOOT;

	InitAmiLib(ImageHandle, SystemTable);

	DEBUG((-1, "Enter SmcNVDIMMDxeDriverInit\n"));

	Status = GetEntireConfig (&mIntelRCSetupData); 
	ASSERT_EFI_ERROR(Status);

	Status = pRS->GetVariable(
					L"Setup",
					&SetupGuid,
					NULL,
					&VarSize,
					&mSetupData);   
	ASSERT_EFI_ERROR(Status);
	
	Status = CreateReadyToBootEvent(TPL_CALLBACK,
                                    SmcNVDIMMReadyToBootCallBack,
                                    NULL,
                                    &oEvent);
	ASSERT_EFI_ERROR(Status);

#if SMC_CPLD_SUPPORT
	Status = pBS->CreateEvent(
					EVT_NOTIFY_SIGNAL,
					TPL_CALLBACK,
					SetCPLD,
					NULL,
					&cpld_event);
	ASSERT_EFI_ERROR(Status);
	Status = pBS->RegisterProtocolNotify(
					&gSmcCPLDProtocolGuid,
					cpld_event,
					&cpld_event_handle);
	ASSERT_EFI_ERROR(Status);
	pBS->SignalEvent(cpld_event);
#endif

	DEBUG((-1, "Exit SmcNVDIMMDxeDriverInit\n"));
	
	return Status;
}
