/** @file
  Contains definitions relevant to the eMca Error Logger.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
	
#ifndef _EMCA_ERROR_LOG_H
#define _EMCA_ERROR_LOG_H

// Function prototype declarations

/**
  The Emca Error Logger. 

  This function is responsible for logging errors via the Emca interface.

  @param[in] ErrorRecordHeader    A pointer to the error record

  @retval Status.

**/
EFI_STATUS
LogErrorToElog (
  IN      UEFI_ERROR_RECORD_HEADER   *ErrorRecordHeader
  );


#endif