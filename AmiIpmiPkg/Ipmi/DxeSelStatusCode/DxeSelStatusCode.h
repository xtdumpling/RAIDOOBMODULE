//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file DxeSelStatusCode.h
    Header file for DxeSelStatusCode module

**/

#ifndef _DXE_SEL_STATUS_CODE_H_
#define _DXE_SEL_STATUS_CODE_H_

//---------------------------------------------------------------------

#include <Pi/PiStatusCode.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/DataHubStatusCodeRecord.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Include/IpmiNetFnSensorDefinitions.h>
#include <Include/StatusCodeConversionLib.h>

#include <Protocol/ReportStatusCodeHandler.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/SelProtocol.h>

//---------------------------------------------------------------------

//
// SEL logging constant
//
#define SEL_LOGGING_DISABLED                0x00
#define SEL_LOGGING_ENABLED                 0x01

//
// SelErase Constants
//
#define DO_NOT_ERASE                        0x00
#define ERASE_NEXT_BOOT                     0x01
#define ERASE_EVERY_BOOT                    0x02

//
// SEL Full constants
//
#define DO_NOTHING_WHEN_SEL_FULL            0x00
#define ERASE_WHEN_SEL_FULL                 0x01

//
// LogSelStatusCodes Constants
//
#define LOG_SEL_STATUSCODE_DISABLED         0x00
#define LOG_BOTH_PROGRESS_AND_ERROR_CODES   0x01
#define LOG_ERROR_CODE_ONLY                 0x02
#define LOG_PROGRESS_CODE_ONLY              0x03

//
// SEL Flags Macro
//
#define SEL_OVERFLOW_FLAG_SET 0x80

EFI_STATUS
GetReservationId(
  UINT8* ReserveId
  );

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
