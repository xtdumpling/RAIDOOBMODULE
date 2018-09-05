//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /AptioV/SRC/AMIDebugRx/DbgRxDebugSupportLib/DbgEndofPeiLib.c 1     1/05/16 7:11p Sudhirv $
//
// $Revision: 1 $
//
// $Date: 1/05/16 7:11p $
//
//**********************************************************************
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRx/DbgRxDebugSupportLib/DbgEndofPeiLib.c $
// 
// 1     1/05/16 7:11p Sudhirv
// Added for Apollolake support EIP 247001
//
//**********************************************************************

//**********************************************************************
//<AMI_FHDR_START>
//
// Name:	DbgEndOfPeiLib.c
//
// Description:	Handles the END of PEI for DebugRx\Debugger
//
//<AMI_FHDR_END>
//**********************************************************************

#include "Efi.h"
#include "Pei.h"
#include "Token.h"
#include <AmiPeiLib.h>
#include <AmiHobs.h>
#ifndef EFIx64
#include <Library/AMIpeidebug.h>
#else
#include <Library/AMIpeidebugX64.h>
#endif	
#include <Library/AMIPeiGUIDS.h>

EFI_STATUS
EFIAPI
DbgEndOfPEINotifyCallback (
  IN EFI_PEI_SERVICES       	**PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                   	*Ppi
  );

EFI_STATUS 
EFIAPI 
DbgMemoryDiscoveredCallback (
  IN CONST EFI_PEI_SERVICES 	**PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR 	*NotifyDescriptor,
  IN VOID 						*Ppi
  );

void DisableBrkptOnIO();

EFI_GUID mEndOfPeiGuid = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;

static EFI_PEI_NOTIFY_DESCRIPTOR mEndOfPeiNotifyList = {
	(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  	&mEndOfPeiGuid,
  	DbgEndOfPEINotifyCallback
};

static EFI_PEI_NOTIFY_DESCRIPTOR MemoryAvailableNotify[] = {
    {
        EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
        &gEfiPeiMemoryDiscoveredPpiGuid,
        DbgMemoryDiscoveredCallback
    }
};

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	UnhookIDT
//
// Description:	Function removes the Debugger IDT handlers. Repatched 
//				the DbgStatuscodeHob
//
// Input:		VOID
//
// Output:		VOID
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
VOID UnhookIDT (VOID)
{
  IA32_IDT_GATE_DESCRIPTOR   *IdtEntry;
  IA32_DESCRIPTOR            IdtDescriptor;
  UINTN                      Handler;

  AsmReadIdtr (&IdtDescriptor);
  IdtEntry = (IA32_IDT_GATE_DESCRIPTOR *) IdtDescriptor.Base;

  //get the IDT handler for IDT 0
  Handler = IdtEntry[0].Bits.OffsetLow + (IdtEntry[0].Bits.OffsetHigh << 16);
  
  //set that Handler for IDTs 1,3,32
  IdtEntry[1].Bits.OffsetLow  = (UINT16)(UINTN)Handler;
  IdtEntry[1].Bits.OffsetHigh = (UINT16)((UINTN)Handler >> 16);
  
  IdtEntry[3].Bits.OffsetLow  = (UINT16)(UINTN)Handler;
  IdtEntry[3].Bits.OffsetHigh = (UINT16)((UINTN)Handler >> 16);
  
  IdtEntry[32].Bits.OffsetLow  = (UINT16)(UINTN)Handler;
  IdtEntry[32].Bits.OffsetHigh = (UINT16)((UINTN)Handler >> 16);

  return;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	PatchupPeiDbgrReportStatusCodeHob()
//
// Description:	Patchs up the Pei Debugger Report Status Code Hob
//
// Input:		EFI_PEI_SERVICES       	**PeiServices,
//				VOID					*pPeiDbgrReportStatusCode
//
// Output:		void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PatchupPeiDbgrReportStatusCodeHob(
		IN EFI_PEI_SERVICES       	**PeiServices,
		IN VOID						*pPeiDbgrReportStatusCode,
		IN UINT8 					State)
{
	EFI_STATUS	Status;
	EFI_PEI_HOB_POINTERS	Hob;
	PEI_DBGR_REPORTSTATUSCODE	*Interface = NULL;
	EFI_GUID  PeiDbgReportStatuscodeGuid = PEI_DBGR_REPORTSTATUSCODE_GUID;

	(*PeiServices)->GetHobList(PeiServices,&Hob.Header);
	Status = FindNextHobByGuid(&PeiDbgReportStatuscodeGuid,&Hob.Header);
	if (EFI_ERROR(Status)) {
		return;
	}

  Interface  = (PEI_DBGR_REPORTSTATUSCODE *)Hob.Raw;
  (UINT64)Interface->ReportStatusCode = (UINT64)0;
  (UINT32)Interface->ReportStatusCode = (UINT32)pPeiDbgrReportStatusCode;
  Interface->State = State;

  return;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	DbgEndOfPEINotifyCallback
//
// Description:	This is the callback function of 
//				PPI EFI_PEI_END_OF_PEI_PHASE_PPI_GUID, when DXEIPL install to 
//				indicate that CPU mode switch from IA32 to x64 mode.
//
// Input:		EFI_PEI_SERVICES       	**PeiServices,
//				EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
//				VOID                   	*Ppi
//
// Output:		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
EFIAPI
DbgEndOfPEINotifyCallback (
  IN EFI_PEI_SERVICES       	**PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                   	*Ppi)
{
#ifndef EFIx64
	EFI_BOOT_MODE BootMode	= BOOT_WITH_FULL_CONFIGURATION;
	UINT32 Dr7Backup 		= 0;
	PEIDebugData_T 			*pData;
	SerXPortGblData_T 		*SerData;
	DbgGblData_T 			*DbgData;
	
	pData = (PEIDebugData_T *)GetDbgDataBaseAddress();
	SerData = (SerXPortGblData_T *)(pData->XportLocalVar);
	DbgData	= (DbgGblData_T *)(pData->DbgrLocalVar);
	
	// Do Nothing in S3 Resume, and clear Host.
	if(EFI_SUCCESS == (*PeiServices)->GetBootMode(PeiServices,&BootMode))
	{
		if(BootMode == BOOT_ON_S3_RESUME) {
			SerData->m_IsHostConnected = 0;
			return EFI_SUCCESS;
		}
	}
	
	if(!REDIRECTION_ONLY_MODE) 
	{
		DisableInterrupts();

		//Disable the DE bit of CR4 register to disable IO breakpoint on read/write
		_asm{
			_emit	0x0f		; mov		eax, cr4
			_emit	0x20		; mov		eax, cr4
			_emit	0xe0		; mov		eax, cr4
			and		eax,0xfffffff7
			//xor 	eax,8		//Clear DE bit
			_emit	0x0f		; mov		cr4, eax
			_emit	0x22		; mov		cr4, eax
			_emit	0xe0		; mov		cr4, eax
		}

		// Disable the debug feature in Dr7
		__asm {
			push eax
			mov eax,dr7
			mov Dr7Backup,eax
			mov eax,0x400
			mov dr7,eax
			pop eax
		};

		pData = (PEIDebugData_T *)GetDbgDataBaseAddress();
		SerData = (SerXPortGblData_T *)(pData->XportLocalVar);
		DbgData	= (DbgGblData_T *)(pData->DbgrLocalVar);
	
		DbgData->m_DR7 = Dr7Backup;
		//Unhook the IDTs
		UnhookIDT();	
	}
	//Unhook the PEIDebugger's Report statusCode HOB interface
	PatchupPeiDbgrReportStatusCodeHob(PeiServices,NULL,AMI_DBG_REPORTSTATUS_X64_PEI_XPORT);
#endif
	return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	DbgMemoryDiscoveredCallback ()
//
// Description:	Memory found ppi notification callback
//
// Input:		EFI_PEI_SERVICES       **PeiServices,
//				EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
//				VOID *Ppi
//
// Output: 		EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI DbgMemoryDiscoveredCallback(
    IN CONST EFI_PEI_SERVICES **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
    IN VOID *Ppi)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	// Register Callback on End Of PEI
	Status = ((**PeiServices).NotifyPpi (PeiServices, &mEndOfPeiNotifyList));
	
	return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:	SetupDebugCallback ()
//
// Description:	Function to set Callback on Memory discovered ppi 
//
// Input:		FI_PEI_SERVICES       **PeiServices,
//
// Output: 		EFI_STATUS
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
SetupDebugCallback (
		IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS Status = EFI_SUCCESS;
  
  // Register Callback for Memory Available
  Status = ((**PeiServices).NotifyPpi (PeiServices, MemoryAvailableNotify));
  
  return Status;
}

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
