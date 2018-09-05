/** @file
  Implementation of the eMca Error Logger.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/

#include <Setup/IioUniversalData.h>
#include <Library/mpsyncdatalib.h>
#include <Protocol/ErrorHandlingProtocol.h>
#include <Protocol/PlatformErrorHandlingProtocol.h>
#include <Protocol/PlatformRasPolicyProtocol.h>
#include <Protocol/SmmBase2.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "EmcaErrorLog.h"
#include "ErrorRecords.h"
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/WheaDefs.h>
#include <Library/emcalib.h>
#include <Protocol\GlobalNvsArea.h>

// Module global data
EFI_PLATFORM_ERROR_HANDLING_PROTOCOL  *mPlatformErrorHandlingProtocol;
EFI_PLATFORM_RAS_POLICY_PROTOCOL      *mPlatformRasPolicyProtocol;
BIOS_ACPI_PARAM   *mAcpiParameter;

extern  EFI_GUID  gErrRecordProcGenErrGuid;
extern  EFI_GUID  gErrRecordPlatformMemErrGuid;

/**
  Entry point for the Core Handler initialization. 

  This function initializes the error handling and enables error
  sources for the CPU Core.
  

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval Status.

**/
EFI_STATUS
EFIAPI
InitializeEmcaErrorLogger (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS    Status;
  UINT8         *EmcaL1DirPtr;
  UINT8         *EmcaElogDirPtr;
  EFI_GLOBAL_NVS_AREA_PROTOCOL    *AcpiNvsProtocol = NULL;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiPlatformRasPolicyProtocolGuid,
                  NULL,
                  &mPlatformRasPolicyProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    RASDEBUG ((EFI_D_ERROR, "EmcaL1DirAddr Error (0x%08x) - Not installed\n", Status));
    return Status;
  }
  
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  &AcpiNvsProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    RASDEBUG ((EFI_D_ERROR, "EmcaL1DirAddr Error (0x%08x) - Not installed\n", Status));
    return Status;
  }
  mAcpiParameter = AcpiNvsProtocol->Area;
  if(mAcpiParameter == NULL) {
    Status = EFI_UNSUPPORTED;
    RASDEBUG ((EFI_D_ERROR, "EmcaL1DirAddr Error (0x%08x) - Not installed\n", Status));
    return EFI_UNSUPPORTED;
  }

  if ((mPlatformRasPolicyProtocol->SystemRasCapability->PlatformRasCapability.EmcaLogCap == 0) 
      || (mPlatformRasPolicyProtocol->SystemRasSetup->Emca.EmcaEn == 0)) {
    Status = EFI_UNSUPPORTED;
    RASDEBUG ((EFI_D_ERROR, "EmcaL1DirAddr Error (0x%08x) - Not installed\n", Status));
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiPlatformErrorHandlingProtocolGuid,
                    NULL,
                    &mPlatformErrorHandlingProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    RASDEBUG ((EFI_D_ERROR, "EmcaL1DirAddr Error (0x%08x) - Not installed\n", Status));
    return Status;
  }

  Status = InitEMCA(&EmcaL1DirPtr,&EmcaElogDirPtr);
  if (EFI_ERROR (Status)) {
    RASDEBUG ((EFI_D_ERROR, "EmcaL1DirAddr Error (0x%08x) - Not installed\n", Status));
    return Status;
  } else {
    mAcpiParameter->EmcaL1DirAddr = (UINT64)EmcaL1DirPtr;
    RASDEBUG ((EFI_D_ERROR, "EmcaL1DirAddr = 0x%08x\n", EmcaL1DirPtr));
  }
  
  Status = mPlatformErrorHandlingProtocol->RegisterErrorListener (LogErrorToElog);

  return Status;
}

/**
  The Elog Error Logger. 

  This function is responsible for logging errors via the Elog interface.

  @param[in] ErrorRecordHeader    A pointer to the error record header

  @retval Status.

**/
EFI_STATUS
LogErrorToElog (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  )
{
  UEFI_ERROR_SECT_DESC            *ErrorRecordDescription;
  VOID                            *ErrorRecord;
  UINT8                           GenErrRecord[SIZE_4KB];
  GENERIC_ERROR_STATUS            *GenErrSts;
  GENERIC_ERROR_DATA_ENTRY        *GenErrData;
  VOID                            *ErrorSection;
  BOOLEAN                         ValidEntry = FALSE;
  UINT32                          McBank = 0;
  EMCA_MC_SIGNATURE_ERR_RECORD    McErrSig;
  
  if (isEmcaOptInFlagSet () || (mPlatformRasPolicyProtocol->SystemRasSetup->Emca.EmcaIgnOptin == 1)) {
    GenErrSts = (GENERIC_ERROR_STATUS *)&GenErrRecord;
    GenErrData = (GENERIC_ERROR_DATA_ENTRY *)((UINT8 *)GenErrSts + sizeof (GENERIC_ERROR_STATUS));
    ErrorSection = (VOID *) ((UINT8 *)GenErrData + sizeof(GENERIC_ERROR_DATA_ENTRY));

    ErrorRecordDescription = (UEFI_ERROR_SECT_DESC *)((UINT8 *)ErrorRecordHeader + sizeof (UEFI_ERROR_RECORD_HEADER));
    ErrorRecord = (UINT8 *)ErrorRecordDescription + sizeof (UEFI_ERROR_SECT_DESC);

    if ((ErrorRecordDescription->Severity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTED) 
        && (mPlatformRasPolicyProtocol->SystemRasSetup->Emca.ElogCorrErrEn == 0)) {
      return  EFI_SUCCESS;
    }
    
    SetMem (GenErrSts, sizeof (GENERIC_ERROR_STATUS), 0);
    SetMem (GenErrData, sizeof (GENERIC_ERROR_DATA_ENTRY), 0);

    if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordPlatformMemErrGuid)) {
      if ((mPlatformRasPolicyProtocol->SystemRasCapability->PlatformRasCapability.EmcaMemLogCap == 1)
          && (mPlatformRasPolicyProtocol->SystemRasSetup->Emca.ElogMemErrEn == 1)) {
        //RASDEBUG ((DEBUG_INFO, "ELOG: Detected Memory Error\n"));
        ValidEntry = ((MEMORY_ERROR_SECTION *)ErrorRecord)->McaBankInfo.Valid;
        GenErrData->DataSize = sizeof (MEMORY_ERROR_SECTION) - sizeof (MCA_BANK_INFO);
        CopyMem (ErrorSection, ErrorRecord, sizeof(MEMORY_ERROR_SECTION) - sizeof (MCA_BANK_INFO));
        
        McBank = ((MEMORY_ERROR_SECTION *)ErrorRecord)->McaBankInfo.BankNum;
        McErrSig.ApicId = ((MEMORY_ERROR_SECTION *)ErrorRecord)->McaBankInfo.ApicId;
        McErrSig.Signature.McSts = ((MEMORY_ERROR_SECTION *)ErrorRecord)->McaBankInfo.McaStatus;
        McErrSig.Signature.McAddr = ((MEMORY_ERROR_SECTION *)ErrorRecord)->McaBankInfo.McaAddress;
        McErrSig.Signature.McMisc = ((MEMORY_ERROR_SECTION *)ErrorRecord)->McaBankInfo.McaMisc;
      }
    } else if (CompareGuid (&ErrorRecordDescription->SectionType, &gErrRecordProcGenErrGuid)) { 
      if ((mPlatformRasPolicyProtocol->SystemRasCapability->PlatformRasCapability.EmcaProcLogCap == 1)
          && (mPlatformRasPolicyProtocol->SystemRasSetup->Emca.ElogProcErrEn == 1)) {
        //RASDEBUG ((DEBUG_INFO, "ELOG: Detected Processor Error\n"));
        ValidEntry = ((PROCESSOR_GENERIC_ERROR_SECTION *)ErrorRecord)->McaBankInfo.Valid;
        GenErrData->DataSize = sizeof (PROCESSOR_GENERIC_ERROR_SECTION) - sizeof (MCA_BANK_INFO);
        CopyMem (ErrorSection, ErrorRecord, sizeof (PROCESSOR_GENERIC_ERROR_SECTION) - sizeof (MCA_BANK_INFO));
        
        McBank = ((PROCESSOR_GENERIC_ERROR_SECTION *)ErrorRecord)->McaBankInfo.BankNum;
        McErrSig.ApicId = ((PROCESSOR_GENERIC_ERROR_SECTION *)ErrorRecord)->McaBankInfo.ApicId;
        McErrSig.Signature.McSts = ((PROCESSOR_GENERIC_ERROR_SECTION *)ErrorRecord)->McaBankInfo.McaStatus;
        McErrSig.Signature.McAddr = ((PROCESSOR_GENERIC_ERROR_SECTION *)ErrorRecord)->McaBankInfo.McaAddress;
        McErrSig.Signature.McMisc = ((PROCESSOR_GENERIC_ERROR_SECTION *)ErrorRecord)->McaBankInfo.McaMisc;
      }
    }

    if (ValidEntry) {
      //RASDEBUG ((DEBUG_INFO, " --Logging Error to EMCA\n"));
      GenErrSts->Severity = ErrorRecordDescription->Severity;
      GenErrSts->BlockStatus.NumErrorDataEntry = 1;
      if ((ErrorRecordDescription->Severity == EFI_ACPI_5_0_ERROR_SEVERITY_FATAL)
          || (ErrorRecordDescription->Severity == EFI_ACPI_5_0_ERROR_SEVERITY_CORRECTABLE)) {
        GenErrSts->BlockStatus.UeValid = 1;
      } else {
        GenErrSts->BlockStatus.CeValid = 1;
      }
      GenErrSts->RawDataSize = 1;
      CopyMem (&GenErrData->SectionType, &ErrorRecordDescription->SectionType, sizeof (GUID));
      GenErrData->Severity = ErrorRecordDescription->Severity;
      GenErrData->Revision = ErrorRecordDescription->Revision;
      GenErrData->SectionFlags.Primary = 1;
      if (ErrorRecordDescription->ValidBits & BIT0) {
        GenErrData->SecValidMask.FruIdValid = 1;
        CopyMem (&GenErrData->FruId, &ErrorRecordDescription->FruId, sizeof (GUID));
      }
      if (ErrorRecordDescription->ValidBits & BIT1) {
        GenErrData->SecValidMask.FruStringValid = 1;
        CopyMem (&GenErrData->FruString[0], &ErrorRecordDescription->FruText[0], 20);
      }

      GenErrSts->ErrDataSize = GenErrData->DataSize + sizeof (GENERIC_ERROR_DATA_ENTRY);
      GenErrSts->RawDataOffset = GenErrSts->ErrDataSize + sizeof (GENERIC_ERROR_STATUS);

      LogEmcaRecord (McBank, &McErrSig, GenErrSts);
    }
  }
    
  return EFI_SUCCESS;
}