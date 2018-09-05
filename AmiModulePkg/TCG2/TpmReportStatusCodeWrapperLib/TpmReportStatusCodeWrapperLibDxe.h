//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//*************************************************************************
// $Header:  $
//
// $Revision: $
//
// $Date: $
//*************************************************************************
// Revision History
// ----------------
// $Log: $
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:
//
// Description:
//
//<AMI_FHDR_END>
//**********************************************************************
#include <Uefi.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <AmiTcg/AmiTpmStatusCodes.h>

EFI_STATUS 
EFIAPI
TpmDxeReportStatusCode(  IN EFI_STATUS_CODE_TYPE   Type,
                         IN EFI_STATUS_CODE_VALUE  Value);

EFI_STATUS 
EFIAPI
TpmDxeReportStatusCodeEx( IN EFI_STATUS_CODE_TYPE   Type,
        IN EFI_STATUS_CODE_VALUE  Value,
        IN UINT32                 Instance,
        IN CONST EFI_GUID         *CallerId          OPTIONAL,
        IN CONST EFI_GUID         *ExtendedDataGuid  OPTIONAL,
        IN CONST VOID             *ExtendedData      OPTIONAL,
        IN UINTN                  ExtendedDataSize);

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**        5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093         **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
