/** @file
  Contains definitions relevant to the CPU Core Error Handler.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
	
#ifndef _WHEA_ERROR_LOG_H
#define _WHEA_ERROR_LOG_H

#define MAX_WHEA_ERR_REC_COUNT  8

// Function prototype declarations

/**
  The WHEA Error Logger. 

  This function is responsible for logging errors via the WHEA interface.

  @param[in] ErrorRecordHeader    A pointer to the error record

  @retval Status.

**/
EFI_STATUS
LogErrorToWhea (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  );

/**
  Notify the OS via WHEA mechanism

  This function takes the appropriate action for a particular error based on severity.

  @param[in] ErrorSeverity    The severity of the error to be handled
  @param[out] OsNotified      Returns whether the OS was notified

  @retval Status.

**/
EFI_STATUS
WheaNotifyOs (
  IN      UINT8     ErrorSeverity,
     OUT  BOOLEAN   *OsNotified
  );

#endif