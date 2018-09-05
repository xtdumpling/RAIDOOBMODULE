//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.03
//    Bug Fix:  Add pause BMC sensor reading before access SMBUS.
//    Reason:   
//    Auditor:  Leon Xu
//    Date:     Aug/15/2017
//
//  Rev. 1.02
//    Bug Fix:  Fixed access wrong device. 
//    Reason:   Fixed access wrong device.
//    Auditor:  Joshua Fan
//    Date:     Apr/14/2016
//
//  Rev. 1.01
//    Bug Fix:  Compiler error.
//    Reason:   Miss code and compiler error
//    Auditor:  Joshua Fan
//    Date:     Apr/13/2016
//
//  Rev. 1.00
//    Bug Fix:  System can boot normallu when system set .
//    Reason:   When enable CPLD_SUPPORT and set JPME2 to pin2-3, system will hang on 0x04.
//    Auditor:  Joshua Fan
//    Date:     Apr/13/2016
//
//**************************************************************************//

#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Dxe.h>
#include <PCI.h>
#include <AmiHobs.h>
#include <AmiCspLib.h>

// Consumed Protocols
#include <Protocol\PciIO.h>
#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\BootScriptSave.h>
#include <Protocol\LegacyBios.h>

#include "Protocol\SmbusHc.h"

#include <Protocol/IPMITransportProtocol.h>

#include "SmcCPLD.h"

#include <Library/PcdLib.h>
#include <Library/SetupLib.h>

#include "Library\GpioLib.h"
#include "SmcLibBmcPrivate.h"

EFI_STATUS _SetCPLDBYBMC(UINT32);

extern EFI_IPMI_TRANSPORT *mIpmiTransport;
extern EFI_STATUS SmcLibLocateIpmiProtocol(VOID);
STATIC EFI_EVENT   WatchdogEvt;
STATIC EFI_GUID gEfiEventReadyToBootGuid = EFI_EVENT_GROUP_READY_TO_BOOT;

EFI_GUID		mSetupGuid = SETUP_GUID;
SETUP_DATA		mSetupData;
UINTN			mSetupDataVarSize = sizeof(SETUP_DATA);
INTEL_SETUP_DATA	mIntelRCSetupData;

EFI_SMBUS_HC_PROTOCOL     *SmBus = NULL;

SMCCPLDFN mSMCCPLDFN;

STATIC EFI_STATUS SMCCPLDSmbusReadByte(UINT8 addr, UINT8 offset, UINT8* data)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINTN		Len = 1;
	UINT32		iCount = SMC_CPLD_SMBUS_RETRY;
	EFI_SMBUS_DEVICE_ADDRESS	Address;

	if ( NULL == SmBus )
		return EFI_NOT_FOUND;
	
	Address.SmbusDeviceAddress = addr >> 1;
	do {
		Status = SmBus->Execute( SmBus,	
						Address,
						offset,
						EfiSmbusReadByte,
						0,
						&Len,
						data
						);
		if ( !EFI_ERROR(Status) )
			break;
		else
			iCount--;
	} while (iCount);
	
	return Status;
}

STATIC EFI_STATUS SMCCPLDSmbusWriteByte(UINT8 addr, UINT8 offset, UINT8 data)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINTN		Len = 1;
	UINT32		iCount = SMC_CPLD_SMBUS_RETRY;
	EFI_SMBUS_DEVICE_ADDRESS	Address;

	if ( NULL == SmBus )
		return EFI_NOT_FOUND;
	
	Address.SmbusDeviceAddress = addr >> 1;
	do {
		Status = SmBus->Execute( SmBus,	
						Address,
						offset,
						EfiSmbusWriteByte,
						0,
						&Len,
						&data
						);
		if ( !EFI_ERROR(Status) )
			break;
		else
			iCount--;
	} while (iCount);
	
	return Status;
}

STATIC EFI_STATUS SmcCPLDReady2Boot(IN EFI_EVENT Event, IN VOID *Context)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8	iTemp = 0;

	TRACE((-1,"%s - start\n", __FUNCTION__));

	// Configure CPLD Watchdog
	if ( NULL != SmBus ) {

		TRACE((-1,"SmcCPLDWDT=0x%02x, SmcCPLDWDTSec=0x%02x\n", mSetupData.SmcCPLDWDT, mSetupData.SmcCPLDWDTSec));

		if ( SMC_PLD_WDT_OS == mSetupData.SmcCPLDWDT ) {	// WDT type is OS
			Status = _SetCPLDWatchdog(mSetupData.SmcCPLDWDTSec, mSetupData.SmcCPLDWDTNMIRESET);
			TRACE((-1, "SetWDT OS return status is %r.\n", Status));
		}
		else if ( SMC_PLD_WDT_POST == mSetupData.SmcCPLDWDT ||
			SMC_PLD_WDT_DISABLED == mSetupData.SmcCPLDWDT) {	// WDT type is POST or Disable		
			Status = _SetCPLDWatchdog(0, mSetupData.SmcCPLDWDTNMIRESET);
			TRACE((-1, "SetWDT POST return status is %r.\n", Status));
		}
	}
	
	TRACE((-1,"%s - end\n", __FUNCTION__));
	
	return EFI_SUCCESS;
}


EFI_STATUS _SetCPLDBYBMC(UINT32 byteFn)
{
	EFI_STATUS	Status = EFI_SUCCESS;

#if 0	// do not implement code now
	
	UINT32 	iIndex1;

#ifdef EFI_DEBUG
	UINT32	iIndex;
#endif

	UINT8	senddata1[]={0x03, 0x70, 0x01, 0x00};	// ipmitool raw 0x6 0x52 0x3(bus no) 0x70 0 0x80 0x2 	
	UINT8	senddata2[]={0x03, 0x70, 0x00, 0x00, 0x00};	// ipmitool raw 0x6 0x52 0x3(bus no 1) 0x70 0 0x80 0x2 

	UINT8	DataSize;
	UINT8	TempData[32];

	if ( EFI_ERROR(SmcLibLocateIpmiProtocol()) )
		return EFI_DEVICE_ERROR;

	for(iIndex1=0; iIndex1<30; iIndex1++) {
		pBS->SetMem(TempData, 32, 0);
		DataSize = 32;
		Status = mIpmiTransport->SendIpmiCommand (
										mIpmiTransport,
										0x06,	// APP
										BMC_LUN,
										0x52,	// master write-read
										senddata1,
										4,
										TempData,
										&DataSize);
		TRACE((-1, "[doudou]Status=%r\n", Status));
		if ( !EFI_ERROR(Status) ) {
			
#ifdef EFI_DEBUG
			TRACE((-1, "[doudou]DataSize=0x%x\r\n", DataSize));
			for(iIndex=0; iIndex<DataSize; iIndex++) {
				TRACE((-1, "[%d: 0x%x] ", iIndex, TempData[iIndex]));	
			}
			TRACE((-1, "\r\n"));
#endif

			senddata2[4] = TempData[0];
			break;
		}
		else {
			TRACE((-1, "again iIndex1=%d\n", iIndex1));
			pBS->Stall(500);
		}
	}

	if ( iIndex1 == 30 )
		return EFI_DEVICE_ERROR;

	if ( byteFn & BIT0 ) {
		senddata2[4] &= 0x3;
		senddata2[4] |= 0x1;
	}
	else {
		senddata2[4] &= 0x3;
	}

	for(iIndex1=0; iIndex1<30; iIndex1++) {
		pBS->SetMem(TempData, 32, 0);
		DataSize = 32;
		Status = mIpmiTransport->SendIpmiCommand (
										mIpmiTransport,
										0x06,	// APP
										BMC_LUN,
										0x52,	// master write-read
										senddata2,
										5,
										TempData,
										&DataSize);
		TRACE((-1, "[doudou]Status=%r\n", Status));
		if ( !EFI_ERROR(Status) ) {
			
#ifdef EFI_DEBUG
			TRACE((-1, "[doudou]DataSize=0x%x\r\n", DataSize));
			for(iIndex=0; iIndex<DataSize; iIndex++) {
				TRACE((-1, "[%d: 0x%x] ", iIndex, TempData[iIndex]));	
			}
			TRACE((-1, "\r\n"));
#endif

			break;
		}
		else {
			TRACE((-1, "again iIndex1=%d\n", iIndex1));
			pBS->Stall(500);
		}
	}

	if ( iIndex1 == 30 )
		return EFI_DEVICE_ERROR;
#endif

	return Status;
}

STATIC VOID PauseResumeBMC(BOOLEAN bWhat2do)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINT8 pause_ipmi[] = {0xdf, 0x00};	// 	Pause BMC SDR Reading
	UINT8 pause_ipmi_return[32];
	UINT8 pause_ipmi_return_size = 32;
	UINT8 iRetry = 5;

	pause_ipmi[1] = bWhat2do;
	do {
		TRACE((-1,"PauseResumeBMC %d, iRetry = %d\n", bWhat2do, iRetry));
		Status = SMC_IPMICmd(0x30, 0x00, 0x70, pause_ipmi, 2, pause_ipmi_return, &pause_ipmi_return_size);

		if ( EFI_NOT_READY == Status )
			break;
		
		iRetry--;
	} while ( iRetry && EFI_ERROR(Status) );
 
	if ( 0 == bWhat2do && !EFI_ERROR(Status) ) {
		pBS->Stall(5000);
	}
	
}

STATIC EFI_STATUS _DxeSetADR(BOOLEAN bADREn, BOOLEAN bRstBtnADR)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	PauseResumeBMC(0);
	Status = _SetADR(bADREn, bRstBtnADR);
	PauseResumeBMC(1);
	return Status;
}

STATIC EFI_STATUS _DxeSetCPLDWatchdog(UINT16 sec, SMC_WDT_TIMOUT_ACTION RstNMI)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	PauseResumeBMC(0);
	Status = _SetCPLDWatchdog(sec, RstNMI);
	PauseResumeBMC(1);
	return Status;
}

STATIC EFI_STATUS _DxeSetCPLDBYBMC(UINT32 byteFn)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	PauseResumeBMC(0);
	Status = _SetCPLDBYBMC(byteFn);
	PauseResumeBMC(1);
	return Status;
}

STATIC EFI_STATUS CPLD_Create_Protocol(IN EFI_EVENT Event, IN VOID *Context)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	EFI_HANDLE	Handle = NULL;

	TRACE((-1,"CPLD Create Protocol start\n"));
	
	Status = pBS->LocateProtocol (&gEfiSmbusHcProtocolGuid, NULL, (VOID**) &SmBus);
	ASSERT_EFI_ERROR(Status);

	mSMCCPLDFN.SetADR  = _DxeSetADR;
	mSMCCPLDFN.SetCPLDWatchdog = _DxeSetCPLDWatchdog;
	mSMCCPLDFN.SetCPLDBYBMC = _DxeSetCPLDBYBMC;
	
	Status = pBS->InstallProtocolInterface (
					 &Handle,
					 &gSmcCPLDProtocolGuid ,
					 EFI_NATIVE_INTERFACE,
					 &mSMCCPLDFN);
	ASSERT_EFI_ERROR(Status);

	TRACE((-1,"CPLD Create Protocol end\n"));
	
	return Status;
}


EFI_STATUS CPLDDXE_Init (
	IN EFI_HANDLE       ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable )
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_HANDLE Handle = NULL;

	EFI_EVENT	cpld_event;
	VOID*		cpld_event_handle;
	
	InitAmiLib(ImageHandle, SystemTable);

	// 0. Init Setup data
	Status = GetEntireConfig (&mIntelRCSetupData); 
	ASSERT_EFI_ERROR(Status);

	Status = pRS->GetVariable( L"Setup", 
			&mSetupGuid,
			NULL,
			&mSetupDataVarSize,
			&mSetupData );
	ASSERT_EFI_ERROR(Status);

	// 1. Install ready to boot event for WDT.
	TRACE((-1,"CPLD start\n"));
	Status = pBS->CreateEventEx (
                  EFI_EVENT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SmcCPLDReady2Boot,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &WatchdogEvt
                  );
	ASSERT_EFI_ERROR(Status);

	// 2. Create Callback Event for Protocol install
	Status = pBS->LocateProtocol (&gEfiSmbusHcProtocolGuid, NULL, (VOID**) &SmBus);
	if ( !EFI_ERROR(Status) && SmBus != NULL ) {
		mSMCCPLDFN.SetADR  = _DxeSetADR;
		mSMCCPLDFN.SetCPLDWatchdog = _DxeSetCPLDWatchdog;
		mSMCCPLDFN.SetCPLDBYBMC = _DxeSetCPLDBYBMC;
		
		Status = pBS->InstallProtocolInterface (
						 &Handle,
						 &gSmcCPLDProtocolGuid ,
						 EFI_NATIVE_INTERFACE,
						 &mSMCCPLDFN);
		ASSERT_EFI_ERROR(Status);
	}
	else {
		Status = pBS->CreateEvent(
						EVT_NOTIFY_SIGNAL,
						TPL_CALLBACK,
						CPLD_Create_Protocol,
						NULL,
						&cpld_event);
		ASSERT_EFI_ERROR(Status);
		Status = pBS->RegisterProtocolNotify(
						&gEfiSmbusHcProtocolGuid,
						cpld_event,
						&cpld_event_handle);
		ASSERT_EFI_ERROR(Status);
	}


	// 3. Set GPP_D2 to NMI if needed.
	if ( 1 == mSetupData.SmcCPLDWDTNMIRESET ) {
		// CPLD NMI setting
#if defined SMC_CPLD_NMI_GPIO && SMC_CPLD_NMI_GPIO != 0xFF
		TRACE((-1,"[SMC]Set gpp_d2 to NMI\n"));
		Status = GpioSetPadGpiNmiEnConfig(SMC_CPLD_NMI_GPIO, 1);
#endif
	}

	TRACE((-1,"CPLD end\n"));

	return Status;
}
