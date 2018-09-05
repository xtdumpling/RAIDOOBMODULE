//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiReportStatusConOut.h
    Header file for PeiReportStatusConOut module

**/

#ifndef _PEI_REPORT_STATUS_CON_OUT_H__
#define _PEI_REPORT_STATUS_CON_OUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <Library/PrintLib.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Ppi/AmiPeiTextOut.h>
#include <PeiReportStatusConOut.h>
#include <AmiProgressErrorCodeLib.h>

#define PANEL_DISPLAY   1 // 0-Top 1 -Bottom

EFI_STATUS
EFIAPI
PeiReportStatusConOutWorker (
    IN CONST  EFI_PEI_SERVICES        **PeiServices,
    IN EFI_STATUS_CODE_TYPE           CodeType,
    IN EFI_STATUS_CODE_VALUE          Value,
    IN UINT32                         Instance,
    IN CONST EFI_GUID                 *CallerId,
    IN CONST EFI_STATUS_CODE_DATA     *ErrorData OPTIONAL 
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
