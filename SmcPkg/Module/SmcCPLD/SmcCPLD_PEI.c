//****************************************************************************
//**            (C)Copyright 1993-2016 Supermicro Computer, Inc.            **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  System can boot normally when system set .
//    Reason:   When enable CPLD_SUPPORT and set JPME2 to pin2-3, system will hang on 0x04.
//    Auditor:  Joshua Fan
//    Date:     Apr/13/2016
//
//**************************************************************************//

#include <UEfi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Hob.h>
#include <SBCspLib.h>
#include <Setup.h>
#include <AmiCspLib.h>
#include <StatusCodes.h>

#if defined(PI_SPECIFICATION_VERSION)&&(PI_SPECIFICATION_VERSION>=0x00010000)
#include <PPI\smbus2.h>
#else
#include <PPI\smbus.h>
#endif

#include <PPI\ReadOnlyVariable2.h>
#include "SmcCPLD.h"
#include <Library/PeiServicesTablePointerLib.h>
#include <Protocol/IPMITransportProtocol.h>

static EFI_GUID gSetupGuid = SETUP_GUID;


extern EFI_STATUS
SMC_IPMICmd(
  IN  UINT8          NetFunction,
  IN  UINT8          Lun,
  IN  UINT8          Command,
  IN  UINT8          *CommandData,
  IN  UINT8          CommandDataSize,
  OUT UINT8         *ResponseData,
  OUT UINT8         *ResponseDataSize
);	

STATIC EFI_STATUS SMCCPLDSmbusReadByte(UINT8 addr, UINT8 offset, UINT8* data)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINTN		Len1 = 1;
	EFI_SMBUS_DEVICE_ADDRESS	Address;
	UINT32		iCount = SMC_CPLD_SMBUS_RETRY;
	
	EFI_PEI_SMBUS2_PPI *SmBus = NULL;

	CONST EFI_PEI_SERVICES **PeiServices;
  	PeiServices = GetPeiServicesTablePointer ();
	
	// Get SMBUS protocol
	Status = (*PeiServices)->LocatePpi( PeiServices,
				&gEfiPeiSmbus2PpiGuid,
				0,
				NULL,
				&SmBus);
	DEBUG((-1, "Smbus Read Status = %r\n", Status));
	if ( !EFI_ERROR(Status) && SmBus != NULL ) {
		DEBUG((-1, "Smbus = 0x%x\n", SmBus));
		Address.SmbusDeviceAddress = addr >> 1;
		do {
			Status = SmBus->Execute( SmBus,	
							Address,
							offset,
							EfiSmbusReadByte,
							0,
							&Len1,
							data
							);
			DEBUG((-1, "Smbus Execute Status = %r\n", Status));
			if ( !EFI_ERROR(Status) )
				break;
			else
				iCount--;
		} while (iCount);
	}

	return Status;
}


STATIC EFI_STATUS SMCCPLDSmbusWriteByte(UINT8 addr, UINT8 offset, UINT8 data)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	UINTN		Len1 = 1;
	EFI_SMBUS_DEVICE_ADDRESS	Address;
	UINT32		iCount = SMC_CPLD_SMBUS_RETRY;
	
	EFI_PEI_SMBUS2_PPI *SmBus = NULL;
	
	CONST EFI_PEI_SERVICES **PeiServices;
  	PeiServices = GetPeiServicesTablePointer ();
	// Get SMBUS protocol
	Status = (*PeiServices)->LocatePpi( PeiServices,
				&gEfiPeiSmbus2PpiGuid,
				0,
				NULL,
				&SmBus);
	DEBUG((-1, "Smbus Write Status = %r\n", Status));
	if ( !EFI_ERROR(Status) && SmBus != NULL ) {
		DEBUG((-1, "Smbus = 0x%x\n", SmBus));
		Address.SmbusDeviceAddress = addr >> 1;
		do {
			Status = SmBus->Execute( SmBus,	
							Address,
							offset,
							EfiSmbusWriteByte,
							0,
							&Len1,
							&data
							);
			DEBUG((-1, "Smbus Execute Status = %r\n", Status));
			if ( !EFI_ERROR(Status) )
				break;
			else
				iCount--;
		} while (iCount);
	}
	return Status;
}

void printCPLDStatus()
{
	;
}


EFIAPI CPLDIPMICallback(
	IN EFI_PEI_SERVICES 		 **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
	IN VOID 					 *Ppi)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	
	UINT32 	iIndex1;
#ifdef EFI_DEBUG
	UINT32	iIndex;
#endif

	EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable = NULL;
	UINTN	VariableSize = sizeof( SETUP_DATA);
	SETUP_DATA	SetupData;

	UINT8	NetFunction = 0x06;		//App
	UINT8	Lun = BMC_LUN;
	UINT8	Command = 0x52;		// master write-read
	UINT8	senddata1[] = {0x03, 0x70, 0x01, 0x00};
	UINT8	senddata2[] = {0x03, 0x70, 0x00, 0x00, 0x00};
	UINT8	TempData[32];
	UINT8	DataSize = 32;

	DEBUG((-1, "[SMC]CPLDIPMICallback Start\n"));

	// Get the SETUPDATA
	Status = (*PeiServices)->LocatePpi( PeiServices, \
	                                    &gEfiPeiReadOnlyVariable2PpiGuid, \
	                                    0, \
	                                    NULL, \
	                                    &ReadOnlyVariable );
	if ( !EFI_ERROR(Status) ) {
		Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                        L"Setup", \
                                        &gSetupGuid, \
                                        NULL, \
                                        &VariableSize, \
                                        &SetupData );
		if ( EFI_ERROR(Status) ) {
			SetupData.ThrottleOnPowerFail = 0;
		}
	}
	else {
		SetupData.ThrottleOnPowerFail = 0;
	}

	DEBUG((-1, "[SMC]SetupData.ThrottleOnPowerFail = 0x%x\n", SetupData.ThrottleOnPowerFail));


	for(iIndex1=0; iIndex1<SMC_CPLD_IPMI_RETRY; iIndex1++) {
		(*PeiServices)->SetMem(TempData, 32, 0);
		DataSize = 32;
		Status = SMC_IPMICmd(NetFunction, Lun, Command, senddata1, sizeof(senddata1), TempData, &DataSize);

		DEBUG((-1, "[SMC]Status=%r\n", Status));
		if ( !EFI_ERROR(Status) ) {
			
#ifdef EFI_DEBUG
			DEBUG((-1, "[SMC]DataSize=0x%x\r\n", DataSize));
			for(iIndex=0; iIndex<DataSize; iIndex++) {
				DEBUG((-1, "[%d: 0x%x] ", iIndex, TempData[iIndex]));	
			}
			DEBUG((-1, "\r\n"));
#endif

			senddata2[4] = TempData[0];
			break;
		}
		else {
			DEBUG((-1, "again iIndex1=%d\n", iIndex1));
		}
	}

	if ( iIndex1 == SMC_CPLD_IPMI_RETRY )
		return Status;

	senddata2[4] &= ~0x3;		// Set BMC_GPO0.Bit[1:0] to 0
	if ( SetupData.ThrottleOnPowerFail ) {	
		senddata2[4] |= 0x1;	// Set BMC_GPO0.Bit[1:0] to 1
	}

	for(iIndex1=0; iIndex1<SMC_CPLD_IPMI_RETRY; iIndex1++) {
		(*PeiServices)->SetMem(TempData, 32, 0);
		DataSize = 32;
		Status = SMC_IPMICmd(NetFunction, Lun, Command, senddata2, sizeof(senddata2), TempData, &DataSize);
		DEBUG((-1, "[SMC]Status=%r\n", Status));
		if ( !EFI_ERROR(Status) ) {
			
#ifdef EFI_DEBUG
			DEBUG((-1, "[SMC]DataSize=0x%x\r\n", DataSize));
			for(iIndex=0; iIndex<DataSize; iIndex++) {
				DEBUG((-1, "[%d: 0x%x] ", iIndex, TempData[iIndex]));	
			}
			DEBUG((-1, "\r\n"));
#endif

			break;
		}
		else {
			DEBUG((-1, "again iIndex1=%d\n", iIndex1));
		}
	}

	DEBUG((-1, "[SMC]CPLDIPMICallback End\n"));
	return Status;
}

EFIAPI CPLDSmbusCallback(
	IN EFI_PEI_SERVICES 		 **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
	IN VOID 					 *Ppi)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	
	EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadOnlyVariable = NULL;
	UINTN	VariableSize = sizeof( SETUP_DATA);
	SETUP_DATA	SetupData;
	
	EFI_PEI_SMBUS2_PPI          *SmBus = NULL;

	DEBUG ((-1,  "CPLDSumbusCallback Start.\n"));

	// 1. Check SMBUS protocol
	Status = (*PeiServices)->LocatePpi( PeiServices,
				&gEfiPeiSmbus2PpiGuid,
				0,
				NULL,
				&SmBus);
	if ( EFI_ERROR(Status) || SmBus == NULL ) {
		DEBUG ((-1,  "Can't get SMBUS protocol.\n"));
		return Status;
	}


	// 2. Get the SETUPDATA
	Status = (*PeiServices)->LocatePpi( PeiServices, \
	                                    &gEfiPeiReadOnlyVariable2PpiGuid, \
	                                    0, \
	                                    NULL, \
	                                    &ReadOnlyVariable );
	if ( !EFI_ERROR(Status) ) {
		Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable, \
                                        L"Setup", \
                                        &gSetupGuid, \
                                        NULL, \
                                        &VariableSize, \
                                        &SetupData );
		if ( EFI_ERROR(Status) ) {
			SetupData.SmcCPLDWDT = 0;
			SetupData.SmcCPLDWDTSec = SMC_CPLD_RESET;
		}
	}
	else {
		SetupData.SmcCPLDWDT = 0;
		SetupData.SmcCPLDWDTSec = SMC_CPLD_RESET;
	}
	
	DEBUG((-1, "SetupData.SmcCPLDWDT=0x%x, SetupData.SmcCPLDWDTSec=0x%x.\n", SetupData.SmcCPLDWDT, SetupData.SmcCPLDWDTSec));

	
	// 3. Enable SMBUS
	IoWrite32(0xcf8, 0x8000FC40);	// 00:1F.4.40
	DEBUG((-1, "offset 0x40=0x%x.\n", IoRead32(0xcfc)));
	IoWrite32(0xcfc, IoRead32(0xcfc) & (~(BIT1+BIT2)) | BIT0);


	// 4. Set Watchdog
	switch(SetupData.SmcCPLDWDT) {
		case SMC_PLD_WDT_POST:
		case SMC_PLD_WDT_POWERON:
			Status = _SetCPLDWatchdog(SetupData.SmcCPLDWDTSec, SetupData.SmcCPLDWDTNMIRESET);
			break;
		default:
			Status = _SetCPLDWatchdog(0, SetupData.SmcCPLDWDTNMIRESET);
			break;
	}

	DEBUG ((-1,  "Set CPLD WDT status %r.\n", Status));

	// 5. Print CPLD status
	printCPLDStatus();
	
	DEBUG ((-1,  "CPLDSumbusCallback End.\n"));

	return Status;
}


EFI_STATUS EFIAPI CPLDPEI_Init (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices )
{
	EFI_STATUS Status;
	EFI_PEI_NOTIFY_DESCRIPTOR *pSmbusCallback;
	EFI_PEI_NOTIFY_DESCRIPTOR *pIPMICallback;

	DEBUG((-1, "CPLDPEI_Init Start.\n"));

	Status = (**PeiServices).AllocatePool(
			       PeiServices,
			       sizeof (EFI_PEI_NOTIFY_DESCRIPTOR),
			       &pSmbusCallback);
	if ( !EFI_ERROR(Status) ) {
		pSmbusCallback->Flags = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
							| EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
		pSmbusCallback->Guid   = &gEfiPeiSmbus2PpiGuid;
		pSmbusCallback->Notify = CPLDSmbusCallback;
		(*PeiServices)->NotifyPpi( PeiServices, pSmbusCallback);
	}

	Status = (**PeiServices).AllocatePool(
			       PeiServices,
			       sizeof (EFI_PEI_NOTIFY_DESCRIPTOR),
			       &pIPMICallback);
	if ( !EFI_ERROR(Status) ) {
		pIPMICallback->Flags = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK
							| EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
		pIPMICallback->Guid   = &gEfiPeiIpmiTransportPpiGuid;
		pIPMICallback->Notify = CPLDIPMICallback;
		(*PeiServices)->NotifyPpi( PeiServices, pIPMICallback);
	}
	
	DEBUG((-1, "CPLDPEI_Init End.\n"));

	return EFI_SUCCESS;
}

