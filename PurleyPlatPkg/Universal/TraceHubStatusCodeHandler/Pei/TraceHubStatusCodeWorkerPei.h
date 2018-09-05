/** @file
  Definitions for the TraceHubStatusCodeHandlerPei

@copyright
 Copyright (c) 2013 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains 'Framework Code' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may not be modified, except as allowed by
 additional terms of your license agreement.

@par Specification Reference:
**/

#ifndef __TRACE_HUB_STATUS_CODE_WORKER_PEI_H__
#define __TRACE_HUB_STATUS_CODE_WORKER_PEI_H__

#include <PiPei.h>
#include <Guid/StatusCodeDataTypeDebug.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseDebugPrintErrorLevelLib.h>
#include <Library/PcdLib.h>
// APTIOV_SERVER_OVERRIDE_RC_START : The file is from BpCommonPkg
//#include <Library/PlatformPostCodeMapLib.h>
// APTIOV_SERVER_OVERRIDE_RC_END : The file is from BpCommonPkg
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PrintLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/TraceHubDebugLib.h>
#include <Ppi/ReportStatusCodeHandler.h>

/**
  Convert status code value and extended data to readable ASCII string, send string to TraceHub device.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS      Status code reported to TraceHub successfully.

**/
EFI_STATUS
EFIAPI
TraceHubStatusCodeReportWorkerPei (
  IN CONST  EFI_PEI_SERVICES      **PeiServices,
  IN EFI_STATUS_CODE_TYPE         CodeType,
  IN EFI_STATUS_CODE_VALUE        Value,
  IN UINT32                       Instance,
  IN CONST EFI_GUID               *CallerId,
  IN CONST EFI_STATUS_CODE_DATA   *Data OPTIONAL
  );

#endif
