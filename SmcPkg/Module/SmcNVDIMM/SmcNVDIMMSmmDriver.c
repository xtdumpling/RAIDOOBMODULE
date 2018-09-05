//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//  File History
//
//  Rev. 1.00
//    Bug Fix:  Initialize revision
//    Reason:   Get base code from Grantley
//    Auditor:  Salmon Chen
//    Date:     Dec/22/2014
//
//****************************************************************************
//****************************************************************************
//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include "token.h"

#include <Setup.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmSwDispatch2.h>

//#include <Protocol/SmmIchnDispatch2.h>
#include "Protocol/IPMITransportProtocol.h"

#include <Setup.h>
//#include <Guid/SetupVariable.h>

#include <Library/PcdLib.h>
#include <Library/SetupLib.h>

// Constant Definition(s)

// Macro Definition(s)
#define ACPI_SHUTDOWN_TRIGGER_ADR_RETRY 5
#define DECS_VIDDID	0x20168086
#define SMB_TSOD_POLL_EN	BIT20
#define CPUBUSNO 0xCC
// Type Definition(s)

// Function Prototype(s)

// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)
extern EFI_GUID gProjectSmmProtocolGuid;

// Function Definition(s)

VOID
SmcSmmSxPrepareToSleep(
  VOID
  )
{
	EFI_STATUS	Status = EFI_SUCCESS;
	
	UINT8	TransmitData = 0;
	UINT8	TransmitDataSize = 1;
	UINT8	ResponseData[32];
	UINT8	ResponseDataSize = 32;
	EFI_IPMI_TRANSPORT		*mIpmiTransport;

	UINT32	iTemp = ACPI_SHUTDOWN_TRIGGER_ADR_RETRY, i=0;
	
	DEBUG((-1, "[SmcSmmSxPrepareToSleep] Start\n"));

	Status = gSmst->SmmLocateProtocol (
				&gEfiSmmIpmiTransportProtocolGuid,
				NULL,
				&mIpmiTransport);
	if ( EFI_ERROR(Status) ) {		
		DEBUG((-1, "[SmcSmmSxPrepareToSleep] Failed to SMM IPMI protocol.\n"));
	}
	else {
		DEBUG((-1, "[SmcSmmSxPrepareToSleep] Send command to BMC to do 4s shutdown.\n"));
		iTemp = ACPI_SHUTDOWN_TRIGGER_ADR_RETRY;
		do {
			Status = mIpmiTransport->SendIpmiCommand (	// ipmitool chassis power off
							mIpmiTransport,
							0,
							0,
							2,
							&TransmitData,
							TransmitDataSize,
							ResponseData,
							&ResponseDataSize
							);
			if ( !EFI_ERROR(Status) ) {
				break;
			}
			else {
				iTemp--;
				DEBUG((-1, "[SmcSmmSxPrepareToSleep] retry = %d.\n", iTemp));
			}
		} while ( iTemp );
	}

	DEBUG((-1, "[SmcSmmSxPrepareToSleep] Go to dead loop\n"));
	EFI_DEADLOOP();	// stop the cpu and let BMC power off the system
}

typedef struct {
	UINT32	DECS;
	UINT32	SMB_CMD_CFG_0;
	UINT32	OrgValue;
} CLTT_STRUCT;

CLTT_STRUCT sClttStruct[6] = {
	{0x80004200, 0x9C, 0}, 	// SMB_CMD_CFG_0	Bus: B(1), Device: 30, Function: 5 (PCU CR5)
	{0x80004200, 0xA0, 0}, 	// SMB_CMD_CFG_1
// It is for MCP	{0x80004200, 0xA4, 0},		// SMB_CMD_CFG_2

	{0x80804200, 0x9C, 0}, 	// SMB_CMD_CFG_0	Bus: B(1), Device: 30, Function: 5 (PCU CR5)
	{0x80804200, 0xA0, 0}		// SMB_CMD_CFG_1
// It is for MCP		{0x80804200, 0xA4, 0}		// SMB_CMD_CFG_2
};

EFI_STATUS
PauseResumeCLTT(
   IN EFI_HANDLE			DispatchHandle,
   IN CONST EFI_SMM_SW_REGISTER_CONTEXT			*DispatchContext,
   IN OUT   VOID			*CommBuffer,     OPTIONAL
   IN OUT   UINTN			*CommBufferSize  OPTIONAL
   )
{
	UINT32 CPUBUSNO1 = 0;
	//UINT32 CPUBUS1[2] = {0x80004200, 0x80804200};	// Bus: B(0), Device: 8, Function: 2 (DECS) Offset 0xCC
	
	UINT8 i;
	
	TRACE((-1,"\nSMI Debug:  %s: Entering... 0x%x\n",__FUNCTION__, DispatchContext->SwSmiInputValue));
	
	switch(DispatchContext->SwSmiInputValue){
		case SMC_PAUSE_CLTT:
		case SMC_RESUME_CLTT:
			break;
		default:
			return EFI_NOT_FOUND;
	}
	
	for(i=0; i<(sizeof(sClttStruct)/sizeof(CLTT_STRUCT)); i++) {
		IoWrite32(0xcf8, sClttStruct[i].DECS);
		TRACE((-1,"0x%x\n",IoRead32(0xcfc)));
		
		if ( DECS_VIDDID == IoRead32(0xcfc) ) {
			IoWrite32(0xcf8, sClttStruct[i].DECS+CPUBUSNO);
			CPUBUSNO1 = ( IoRead32(0xcfc) >> 8 ) & 0xff;
			
			IoWrite32(0xcf8, BIT31+(CPUBUSNO1<<16)+(30<<11)+(5<<8)+sClttStruct[i].SMB_CMD_CFG_0);
			TRACE((-1,"before bus1,dev30,fn5,off 0x%x = 0x%x\n", sClttStruct[i].SMB_CMD_CFG_0, IoRead32(0xcfc)));
			
			if ( DispatchContext->SwSmiInputValue ==  SMC_PAUSE_CLTT ) {
				sClttStruct[i].OrgValue = IoRead32(0xcfc) & SMB_TSOD_POLL_EN;
				IoWrite32(0xcfc, IoRead32(0xcfc)&(~SMB_TSOD_POLL_EN));
			}
			else {
				IoWrite32(0xcfc, IoRead32(0xcfc)|sClttStruct[i].OrgValue);
			}
			TRACE((-1," after bus1,dev30,fn5,off 0x%x = 0x%x\n", sClttStruct[i].SMB_CMD_CFG_0, IoRead32(0xcfc)));
		
		}
	}
	
	TRACE((-1,"\nSMI Debug:  %s: Exiting... \n",__FUNCTION__));

	return EFI_SUCCESS;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InSmmFunction
//
// Description: Installs SuperM SMM Child Dispatcher Handler.
//
// Input:       ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//                  EFI_NOT_FOUND - The SMM Base protocol is not found.
//                  EFI_SUCCESS   - Installs SuperM SMM Child Dispatcher Handler
//                                  successfully.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS 
InSmmFunction (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_SMM_SW_REGISTER_CONTEXT       SwContext;
	EFI_SMM_SW_DISPATCH2_PROTOCOL     *SwDispatch  = 0;
	EFI_HANDLE                        SwHandle;
	
	INTEL_SETUP_DATA	mSetupData;
	EFI_HANDLE			NewHandle = NULL;

	// 1. Install software SMI for CLTT pause/resume.
	Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
	SwContext.SwSmiInputValue = 0x13;
	Status = SwDispatch->Register (SwDispatch, PauseResumeCLTT, &SwContext, &SwHandle);
	SwContext.SwSmiInputValue = 0x14;
	Status = SwDispatch->Register (SwDispatch, PauseResumeCLTT, &SwContext, &SwHandle);
	
	// 2. Hook sleep handler for graceful shutdown trigger ADR.
	Status = GetEntireConfig (&mSetupData);
	if ( mSetupData.SocketConfig.MemoryConfig.SmcGracefulShutdownTriggerADR && 
		( PcdGet64(PcdSmcNVDIMM) & BIT0 ) ) {
		DEBUG((-1, "[SMC]Hook sleep handler for graceful shutdown trigger ADR.\n"));
		Status = gSmst->SmmInstallProtocolInterface (
			                &NewHandle,
			                &gSmcNVDIMMSmmProtocolGuid,
			                EFI_NATIVE_INTERFACE,
			                SmcSmmSxPrepareToSleep);
	}
	
	return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SmcNVDIMMSmmInit
//
// Description: Installs SuperM SMM Child Dispatcher Handler.
//
// Input:       ImageHandle - Image handle
//              *SystemTable - Pointer to the system table
//
// Output:      EFI_STATUS
//                  EFI_NOT_FOUND - The SMM Base protocol is not found.
//                  EFI_SUCCESS   - Installs SuperM SMM Child Dispatcher Handler
//                                  successfully.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS SmcNVDIMMSmmInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable )
{
    InitAmiLib(ImageHandle, SystemTable);
    return InitSmmHandler(ImageHandle, SystemTable, InSmmFunction, NULL);
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2011, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
