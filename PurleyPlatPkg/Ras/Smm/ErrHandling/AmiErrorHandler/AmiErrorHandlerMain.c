//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//
//*****************************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		AmiErrorHandlerMain.c
//
// Description:	
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>


#include <Protocol\ErrorHandlingProtocol.h>
#include <Protocol\PlatformErrorHandlingProtocol.h>
#include <Protocol\SmmBase2.h>
#include <Library\DebugLib.h>
#include <Library\UefiBootServicesTableLib.h>
#include <Library\SmmServicesTableLib.h>
#include <Library\BaseMemoryLib.h>
#include "ErrorRecords.h"
#include "AmiErrorHandlerMain.h"
#include "RtErrorLog.h" 
#include "RtErrorLogBoard.h"

// Module global data
EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;


VOID
UnLockMSR790 (
  VOID *Buffer
  )
{
	UINT64 Data;

	Data = AsmReadMsr64(MC_ERR_INJ_LCK);
	Data &= ~(BIT0);
	AsmWriteMsr64 (MC_ERR_INJ_LCK, Data);
	return;
}

VOID
UnLockInjLogic()
{
	 UINTN       thread;
	 for (thread = 0; thread < gSmst->NumberOfCpus; thread++) {
	    gSmst->SmmStartupThisAp (UnLockMSR790,
	                             thread, 
	                             NULL);
	 }
	 UnLockMSR790(&thread);
}

/**
  AMI Wrapper function which calls Listeners based on the elinks.

  This function calls Listners like BMC , GPNV for Logging Errors.
  

  @param[in]  UEFI_ERROR_RECORD_HEADER        ErrorRecordHeader
  @retval Status.

**/
EFI_STATUS
AmiLogErrorWrapper (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  )
{
	EFI_STATUS          Status = EFI_SUCCESS;
	UEFI_ERROR_SECT_DESC  *ErrorRecordDescription;
	AMI_MEMORY_ERROR_SECTION      MemInfo;
	AMI_PCIE_ERROR_SECTION                PcieInfo;
	AMI_PROCESSOR_GENERIC_ERROR_SECTION   ProcInfo;

	ZeroMem (&MemInfo, sizeof (AMI_MEMORY_ERROR_SECTION));
	ZeroMem (&PcieInfo, sizeof (AMI_PCIE_ERROR_SECTION));
	ZeroMem (&ProcInfo, sizeof (AMI_PROCESSOR_GENERIC_ERROR_SECTION));
	  
	ErrorRecordDescription = (UEFI_ERROR_SECT_DESC *)((UINT8 *)ErrorRecordHeader + sizeof(UEFI_ERROR_RECORD_HEADER));
	
	RASDEBUG ((DEBUG_ERROR, "Inside AmiLogErrorWrapper() ...\n"));
	
	if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordPlatformMemErrGuid)) { 
		RASDEBUG ((DEBUG_ERROR, "Inside AmiLogErrorWrapper() Memory Error ...\n"));
	    MemInfo.Header.ErrorType = MemSource;
	    MemInfo.Header.ErrorSeverity = ErrorRecordHeader->Severity;
	    CopyMem (((UINT8 *)&MemInfo + sizeof(ERROR_HEADER)), ((UINT8 *)ErrorRecordHeader + sizeof(UEFI_ERROR_RECORD_HEADER) + sizeof(UEFI_ERROR_SECT_DESC)), sizeof(MEMORY_ERROR_SECTION));
	    OemCollectErrors((VOID *)&MemInfo); // Oem Hook to Collect Error Information before logging the Error
	    CommonErrorHandling((VOID *)&MemInfo);
	}
	else if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordPcieErrGuid)) {
		RASDEBUG ((DEBUG_ERROR, "Inside AmiLogErrorWrapper() PCIE Error ...\n"));
	    PcieInfo.Header.ErrorType = PciSource;
	    PcieInfo.Header.ErrorSeverity = ErrorRecordHeader->Severity;
	    CopyMem (((UINT8 *)&PcieInfo + sizeof(ERROR_HEADER)), ((UINT8 *)ErrorRecordHeader + sizeof(UEFI_ERROR_RECORD_HEADER) + sizeof(UEFI_ERROR_SECT_DESC)), sizeof(PCIE_ERROR_SECTION));
	    OemCollectErrors((VOID *)&PcieInfo); // Oem Hook to Collect Error Information before logging the Error
	    CommonErrorHandling((VOID *)&PcieInfo);
	} else if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordProcGenErrGuid)) {
		RASDEBUG ((DEBUG_ERROR, "Inside AmiLogErrorWrapper() Processor Error ...\n"));
	    ProcInfo.Header.ErrorType = ProcSource;
		ProcInfo.Header.ErrorSeverity = ErrorRecordHeader->Severity;
		CopyMem (((UINT8 *)&ProcInfo + sizeof(ERROR_HEADER)), ((UINT8 *)ErrorRecordHeader + sizeof(UEFI_ERROR_RECORD_HEADER) + sizeof(UEFI_ERROR_SECT_DESC)), sizeof(PROCESSOR_GENERIC_ERROR_SECTION));
		OemCollectErrors((VOID *)&ProcInfo); // Oem Hook to Collect Error Information before logging the Error
	    CommonErrorHandling((VOID *)&ProcInfo);
	}

	return Status;
}

/**
  Entry point for the AMI Error Handler initialization. 

  This function initializes AMI Error handling.
  

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
InitializeAmiErrorHandler (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS                Status;
 
  RASDEBUG ((DEBUG_INFO, "Inside AMIErrorHandler EntryPoint\n"));

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformErrorHandlingProtocolGuid,
                    NULL,
                    &mPlatformErrorHandlingProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformRasPolicyProtocolGuid,
                    NULL,
                    &mPlatformRasPolicyProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mPlatformRasPolicyProtocol->SystemRasSetup->UnlockMsr) {
	UnLockInjLogic();
  } 

  Status = mPlatformErrorHandlingProtocol->RegisterErrorListener (AmiLogErrorWrapper);
  ASSERT_EFI_ERROR (Status);

  // Call NULL OEM Hook function
  OemInitializeAmiErrorHandler(mPlatformRasPolicyProtocol);

  return Status;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**       5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093       **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
