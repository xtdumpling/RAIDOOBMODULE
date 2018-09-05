/** @file
  Contains definitions relevant to the Platform Error Handler.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
	
#ifndef _PLATFORM_ERROR_HANDLER_H
#define _PLATFORM_ERROR_HANDLER_H

#include <Protocol/PlatformErrorHandlingProtocol.h>


// Function prototype declarations

/**
  Register an error listener. 

  This function allows a module to add its listener to the 
  list of listeners that will be executed to log reported errors.

  @param[in] FunctionPtr          A pointer to the listener

  @retval Status.

**/
EFI_STATUS
RegisterErrorListener (
  IN      EFI_ERROR_LISTENER_FUNCTION  FunctionPtr
  );

/**
  Register an OS notifier. 

  This function allows a module to add its notifier to the 
  list of OS Notifiers that will be executed to to notify 
  the OS that an error occurred.

  @param[in] FunctionPtr          A pointer to the notifier

  @retval Status.

**/
EFI_STATUS
RegisterNotifier (
  IN      EFI_NOTIFIER_FUNCTION  FunctionPtr,
  IN      UINT8                  Priority
  );
  
/**
  Log a reported error 

  This function logs an error.

  @param[in] ErrorRecordHeader		A pointer to the error record header

  @retval Status.

**/
EFI_STATUS
LogReportedError (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  );
  
/**
  Notify the OS

  This function takes the appropriate action for a particular error based on severity.

  @param[in] FunctionPtr          A pointer to the silicon module error handler

  @retval Status.

**/
EFI_STATUS
NotifyOs (
  IN      UINT8   ErrorSeverity
  );

  
#endif
