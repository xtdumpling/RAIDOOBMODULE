//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
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
#include <Setup.h>
#include <PciBus.h>
#include <Library/MmPciBaseLib.h>
#include "SmcLib.h"
#include "SmcLibBmcPrivate.h"
#include <Protocol/PchReset.h>
#include <Library/SetupLib.h>
#include "KtiSetupDefinitions.h"
#include "MemDefaults.h"

EFI_GUID		mSetupGuid = SETUP_GUID;

UINT8 gAltEngPerfBIAS;

typedef struct {
//	UINT8 CpuPm;	// 0 - DISABLE; 1 - ENABLE; 2 - CUSTOM
//	UINT8 PwrPerfTuning;	// 0 - OS; 1 - BIOS
	UINT8 ProcessorHWPMEnable;
	UINT8 ProcessorVmxEnable;
	UINT8 QpiLinkSpeed;
	UINT8 SncEn;
	UINT8 ImcInterleaving;
	UINT8 LLCDeadLineAlloc;
	UINT8 StaleAtoSOptEn;
} SMC_OPTIMIZE_SPEC_POWER;


EFI_STATUS
EFIAPI
SmcOptimizePchResetCallback (
  IN     PCH_RESET_TYPE
  );


GLOBAL_REMOVE_IF_UNREFERENCED PCH_RESET_CALLBACK_PROTOCOL mPchResetCallbackProtocol = { SmcOptimizePchResetCallback };

VOID SetSpecPower(INTEL_SETUP_DATA* mSetupData)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	DEBUG((-1, "SpecPower:Max Power Efficient\n"));

	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorHyperThreadingDisable = 0;//Hyper-Threading: Enable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.ExecuteDisableBit = 0;//Execute Disable Bit: Disable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable = 1;//Intel Virtualization Technology: Enable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.PpinControl = 0;//PPIN Control: Unlock/Disable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.MlcStreamerPrefetcherEnable = 0;//Hardware Prefetcher: Disable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.MlcSpatialPrefetcherEnable = 0;//Adjacent cache Preferch: Disable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.DCUStreamerPrefetcherEnable = 0;//DCU Streamer Prefetch: Disable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.DCUIPPrefetcherEnable = 1;//DCU IP Prefetcher: Enable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.LlcPrefetchEnable = 1;//LLC Prefetch: Enable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorX2apic = 1;//Extended APIC: Enable
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.AesEnable = 1;//AES-NI: Enable

	mSetupData->SocketConfig.PowerManagementConfig.ProcessorEistEnable = 1;//SpeedStep (Pstates): Enable
	mSetupData->SocketConfig.PowerManagementConfig.ProcessorEistPsdFunc = 0;//EIST PSD Function: HW_ALL
	mSetupData->SocketConfig.PowerManagementConfig.TurboMode = 1;//Turbo mode: Enable
	mSetupData->SocketConfig.PowerManagementConfig.ProcessorHWPMEnable = 1;//Hardware P-States: Native Mode
	mSetupData->SocketConfig.PowerManagementConfig.ProcessorAutonomousCstateEnable = 0;//Autonomous Core C-state: Disable
	mSetupData->SocketConfig.PowerManagementConfig.C6Enable = 1;//CPU C6 report: Enable
	mSetupData->SocketConfig.PowerManagementConfig.ProcessorC1eEnable = 1;//Enhanced Halt State (C1E): Enable
	mSetupData->SocketConfig.PowerManagementConfig.PackageCState = 3;//Package C State: C6 (Retention) State
	mSetupData->SocketConfig.PowerManagementConfig.TStateEnable = 1;//Software Controlled T-state: Enable
	mSetupData->SocketConfig.PowerManagementConfig.OSCx = 1;//OS ACPI Cx: ACPI C2 change ACPI C3.
	mSetupData->SocketConfig.PowerManagementConfig.PkgCLatNeg = 1;//PKG C-state Lat Neg: Disable change Enable
	mSetupData->SocketConfig.PowerManagementConfig.CurrentLimit = 405;//Current Limitation -> 405
	mSetupData->SocketConfig.PowerManagementConfig.PowerLimit1Time = 56;//PL1 Time Window  [56]
	mSetupData->SocketConfig.PowerManagementConfig.PowerLimit2Time = 56;//PL2 Time Window  [56]  
	mSetupData->SocketConfig.PowerManagementConfig.PowerLimit1Power = 182;//PL1 Power Limit =182
	mSetupData->SocketConfig.PowerManagementConfig.PowerLimit2Power = 198;//PL2 Power Limit =198

	mSetupData->SocketConfig.CsiConfig.DegradePrecedence = 1;	//FEATURE_PRECEDENCE;//Degrade Precedence: Feature Precedence
	mSetupData->SocketConfig.CsiConfig.KtiLinkL0pEn = 0;	//STR_KTI_DISABLE;//Link L0p Enable: Disable
	mSetupData->SocketConfig.CsiConfig.KtiLinkL1En = 1;	//KTI_ENABLE;//Link L1 Enable: Enable
	mSetupData->SocketConfig.CsiConfig.IoDcMode = 3;	//IODC_EN_REM_INVITOM_ALLOCFLOW;//IO Directory Cache (IODC): InvItom AllocFlow
	mSetupData->SocketConfig.CsiConfig.QpiLinkSpeed = SPEED_REC_96GT;//UPI Link Speed = 9.6GT
	mSetupData->SocketConfig.CsiConfig.SncEn = KTI_ENABLE;//SNC:KTI_ENABLE
	mSetupData->SocketConfig.CsiConfig.KtiCrcMode = CRC_MODE_ROLLING_32BIT;//CRC mode: Auto change to 32 bit Rolling CRC
	mSetupData->SocketConfig.CsiConfig.KtiFailoverEn = KTI_ENABLE;//UPI Failover Support: Auto change to Enable   	

	mSetupData->SocketConfig.MemoryConfig.EnforcePOR = ENFORCE_POR_DIS;//Enforce POR: Disable
	mSetupData->SocketConfig.MemoryConfig.DdrFreqLimit = 11;//Memory Frequency: 2133
	mSetupData->SocketConfig.MemoryConfig.ScrambleEnDDRT = SCRAMBLE_DISABLE;//Data Scrambling for NVDIMM: Disable
	mSetupData->SocketConfig.MemoryConfig.ScrambleEn = SCRAMBLE_DISABLE;//Data Scrambling for DDR4: Disable
	mSetupData->SocketConfig.MemoryConfig.ADREn = 0;//Enable ADR: Disable
	mSetupData->SocketConfig.MemoryConfig.refreshMode = A2R_DISABLE;//2X REFRESH: Auto
	mSetupData->SocketConfig.MemoryConfig.PartialMirrorUefiPercent = 10;//ARM Mirror percentage: 10
	mSetupData->SocketConfig.MemoryConfig.PatrolScrub = PATROL_SCRUB_DIS;//Patrol Scrub: Disable
	mSetupData->SocketConfig.MemoryConfig.ImcInterleaving = IMC_1WAY;//IMC Interleaving = 1-way
	
	mSetupData->SocketConfig.CommonRcConfig.IsocEn = 0;//Isoc Mode: Disable
	mSetupData->SocketConfig.CommonRcConfig.LockStep = RAS_MODE_DISABLE;//Static Virtual Lockstep Mode: Disable
	mSetupData->SocketConfig.CommonRcConfig.MirrorMode = RAS_MODE_DISABLE;//Mirror mode: Disable
	
	mSetupData->SocketConfig.IioConfig.ATS = 0;//ATS: Disable
	mSetupData->SocketConfig.IioConfig.CoherencySupport = 0;//Coherency Support(Non-Isoch): Disable
	mSetupData->SocketConfig.IioConfig.PcieGlobalAspm = 2;//PCI-E ASPM Support (Global) : L1 Only
	
	mSetupData->SystemConfig.WheaSupportEn = 0;//WHEA Support: Disable
	mSetupData->SystemConfig.Hpet = 0;//High Precision Event Timer: Disable
	mSetupData->SystemConfig.PcieDmiAspm = 2;//PCH DMI ASPM : ASPM L1

	Status = SetEntireConfig (mSetupData);
	DEBUG((-1, "Status = %r\n", Status));
	ASSERT_EFI_ERROR (Status);

	// 3. Set Power cycle reset
	IoWrite32(0xcf8, 0x8000faac);
	IoWrite32(0xcfc, IoRead32(0xcfc)|(1<<20));
	DEBUG((-1, "0x8000faac = 0x%x\n", IoRead32(0xcfc)));
}

VOID SetCPU2006(INTEL_SETUP_DATA* mSetupData)
{
	EFI_STATUS Status = EFI_SUCCESS;
	SMC_OPTIMIZE_SPEC_POWER org_values;

	// 1. Save ORG values
	org_values.ImcInterleaving = mSetupData->SocketConfig.MemoryConfig.ImcInterleaving;
	org_values.LLCDeadLineAlloc = mSetupData->SocketConfig.CsiConfig.LLCDeadLineAlloc;
	org_values.ProcessorHWPMEnable = mSetupData->SocketConfig.PowerManagementConfig.ProcessorHWPMEnable;
	org_values.ProcessorVmxEnable = mSetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable;
	org_values.QpiLinkSpeed = mSetupData->SocketConfig.CsiConfig.QpiLinkSpeed;
	org_values.SncEn = mSetupData->SocketConfig.CsiConfig.SncEn;
	org_values.StaleAtoSOptEn = mSetupData->SocketConfig.CsiConfig.StaleAtoSOptEn;
	
	Status = pRS->SetVariable( L"SMC_CPU2006", 
			&mSetupGuid,
			EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
			sizeof(org_values),
			&org_values);
	ASSERT_EFI_ERROR(Status);
	
	// 2. Set new values
	DEBUG((-1, "Set new values\n"));
	mSetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable = 0;	// Disable
	
	mSetupData->SocketConfig.MemoryConfig.ImcInterleaving = 1;	// IMC_1WAY
	
	mSetupData->SocketConfig.CsiConfig.LLCDeadLineAlloc = 0;	// KTI_DISABLE
	mSetupData->SocketConfig.CsiConfig.QpiLinkSpeed = 1;	// SPEED_REC_104GT
	mSetupData->SocketConfig.CsiConfig.SncEn = 1;	// KTI_ENABLE
	mSetupData->SocketConfig.CsiConfig.StaleAtoSOptEn = 1;	// KTI_ENABLE

	mSetupData->SocketConfig.PowerManagementConfig.ProcessorHWPMEnable = 0;	// Disable

	DEBUG((-1, "SetupData.SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable = 0x%x.\n", mSetupData->SocketConfig.SocketProcessorCoreConfiguration.ProcessorVmxEnable));
	DEBUG((-1, "SetupData.SocketConfig.MemoryConfig.ImcInterleaving = 0x%x\n", mSetupData->SocketConfig.MemoryConfig.ImcInterleaving));
	DEBUG((-1, "SetupData.SocketConfig.CsiConfig.LLCDeadLineAlloc = 0x%x\n", mSetupData->SocketConfig.CsiConfig.LLCDeadLineAlloc));
	DEBUG((-1, "SetupData.SocketConfig.CsiConfig.QpiLinkSpeed = 0x%x\n", mSetupData->SocketConfig.CsiConfig.QpiLinkSpeed));
	DEBUG((-1, "SetupData.SocketConfig.CsiConfig.SncEn = 0x%x\n", mSetupData->SocketConfig.CsiConfig.SncEn));
	DEBUG((-1, "SetupData.SocketConfig.CsiConfig.StaleAtoSOptEn = 0x%x\n", mSetupData->SocketConfig.CsiConfig.StaleAtoSOptEn));	
	DEBUG((-1, "SetupData.SocketConfig.PowerManagementConfig.ProcessorHWPMEnable = 0x%x\n", mSetupData->SocketConfig.PowerManagementConfig.ProcessorHWPMEnable));
	DEBUG((-1, "SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS = 0x%x\n", mSetupData->SocketConfig.PowerManagementConfig.AltEngPerfBIAS));
	
	Status = SetEntireConfig (mSetupData);
	DEBUG((-1, "Status = %r\n", Status));
	ASSERT_EFI_ERROR (Status);

	// 3. Set Power cycle reset
	IoWrite32(0xcf8, 0x8000faac);
	IoWrite32(0xcfc, IoRead32(0xcfc)|(1<<20));
	DEBUG((-1, "0x8000faac = 0x%x\n", IoRead32(0xcfc)));
}

/**
  WDT call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
SmcOptimizePchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
	EFI_STATUS Status = EFI_SUCCESS;
	INTEL_SETUP_DATA	IntelSetupData;
	
	DEBUG((-1, "SmcOptimizePchResetCallback\n"));
	
	Status = GetEntireConfig (&IntelSetupData);
	ASSERT_EFI_ERROR (Status);

	// if the item no change, do nothing.
	if ( IntelSetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == gAltEngPerfBIAS ) {
		return EFI_SUCCESS;
	}

	if ( IntelSetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_CPU2006 /*Max Performance*/ && 
		IntelSetupData.SocketConfig.PowerManagementConfig.CpuPm == 2 /*custom*/&&
		IntelSetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning == 1 /*BIOS*/ ) {
		SetCPU2006(&IntelSetupData);
	}
	else if (IntelSetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS == SMC_OPTIMIZE_SPECPOWER /*Max Power Efficient*/ && 
		IntelSetupData.SocketConfig.PowerManagementConfig.CpuPm == 2 /*custom*/&&
		IntelSetupData.SocketConfig.PowerManagementConfig.PwrPerfTuning == 1 /*BIOS*/ ) {
		SetSpecPower(&IntelSetupData);
	}
	else {
		// should we restore the old value?
	}

	DEBUG((-1, "SmcOptimizePchResetCallback End\n"));
	
	return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure   : SmcOptimizeDxeInit
//
// Description : 
//
// Parameters  : None
//
// Returns     :  EFI_SUCCESS
//
//----------------------------------------------------------------------------
//<AMI_PHDR_START>
EFI_STATUS
SmcOptimizeDxeInit(
    IN	EFI_HANDLE	ImageHandle,
    IN	EFI_SYSTEM_TABLE	*SystemTable
)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	INTEL_SETUP_DATA	SetupData;
	
	InitAmiLib(ImageHandle, SystemTable);

	DEBUG((-1, "SmcOptimizeDxeInit\n"));

	Status = GetEntireConfig(&SetupData);
	ASSERT_EFI_ERROR (Status);
	
	gAltEngPerfBIAS = SetupData.SocketConfig.PowerManagementConfig.AltEngPerfBIAS;
	DEBUG((-1, "gAltEngPerfBIAS = 0x%x\n", gAltEngPerfBIAS));
	
	Status = gBS->InstallProtocolInterface (
		&ImageHandle,
		&gPchResetCallbackProtocolGuid,
		EFI_NATIVE_INTERFACE,
		&mPchResetCallbackProtocol
		);
	ASSERT_EFI_ERROR (Status);
	
	DEBUG((-1, "Exit SmcOptimizeDxeInit\n"));

	return Status;
}

