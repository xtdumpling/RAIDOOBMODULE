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
//************************************************************************
#include "TpmReportStatusCodeWrapperLibDxe.h"
#include "Token.h"

static UINT8 isTpmReportStatusCodeEnabled = TCG2_REPORT_STATUS_CODE;  //set to TRUE for now, use setup to control in release

EFI_STATUS 
EFIAPI
TpmDxeReportStatusCode(  IN EFI_STATUS_CODE_TYPE   Type,
                         IN EFI_STATUS_CODE_VALUE  Value)
{
    if(isTpmReportStatusCodeEnabled){
            return (ReportStatusCode(Type, Value));
    }
    return RETURN_UNSUPPORTED;
}


EFI_STATUS 
EFIAPI
TpmDxeReportStatusCodeEx( IN EFI_STATUS_CODE_TYPE   Type,
        IN EFI_STATUS_CODE_VALUE  Value,
        IN UINT32                 Instance,
        IN CONST EFI_GUID         *CallerId          OPTIONAL,
        IN CONST EFI_GUID         *ExtendedDataGuid  OPTIONAL,
        IN CONST VOID             *ExtendedData      OPTIONAL,
        IN UINTN                  ExtendedDataSize)
{
    EFI_STATUS Status;
    
    if(!isTpmReportStatusCodeEnabled){
        return RETURN_UNSUPPORTED;
    }
    
    if(ExtendedDataGuid == NULL || ExtendedData == NULL || ExtendedDataSize == 0){
        Status = ReportStatusCode(Type, Value);
    }else{
        Status = ReportStatusCodeEx(Type, Value, Instance, CallerId, ExtendedDataGuid, ExtendedData, ExtendedDataSize );
    }
    
    return Status;
}
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
