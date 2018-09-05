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

/** @file PeiSelStatusCode.h
    Header file for PeiSelStatusCode module

**/

#ifndef _PEI_SEL_STATUS_CODE_H_
#define _PEI_SEL_STATUS_CODE_H_

//
// Statements that include other files
//
#include <PiPei.h>
#include <Pi/PiStatusCode.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Include/IpmiNetFnSensorDefinitions.h>
#include <Include/StatusCodeConversionLib.h>

#include <Ppi/ReportStatusCodeHandler.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/IPMITransportPpi.h>
#include <Ppi/PeiSelPpi.h>
#include <Ppi/ReadOnlyVariable2.h>


//
// Sel logging constant
//
#define SEL_LOGGING_DISABLED                  0x00
#define SEL_LOGGING_ENABLED                   0x01

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
#define SEL_OVERFLOW_FLAG_SET               0x80

//
// Function Prototypes
//
EFI_STATUS
EFIAPI
UpdatedPeiSelPpi (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN  VOID                        *Ppi
  );

//
// Produced PPIs
// PPI interface definition
//
EFI_STATUS
EFIAPI
EfiSelReportStatusCode(
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN PEI_SEL_STATUS_CODE_PPI        * This,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
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
