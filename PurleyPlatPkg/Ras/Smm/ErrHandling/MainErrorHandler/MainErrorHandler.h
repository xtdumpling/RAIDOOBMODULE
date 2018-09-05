/** @file
  Contains definitions relevant to the RAS Module Main Error Handler.

  Copyright (c) 2009-2014 Intel Corporation.
  All rights reserved. This software and associated documentation
  (if any) is furnished under a license and may only be used or
  copied in accordance with the terms of the license.  Except as
  permitted by such license, no part of this software or
  documentation may be reproduced, stored in a retrieval system, or
  transmitted in any form or by any means without the express
  written consent of Intel Corporation.

**/
	
#ifndef _MAIN_ERROR_HANDLER_H
#define _MAIN_ERROR_HANDLER_H

#include <Protocol/ErrorHandlingProtocol.h>

// Definitions

// IIO_DFX spare_swwest function(BoxInst) 7
#define SPARESWWEST_BOXINST     7

// Function prototype declarations
/**
  Register a silicon error Handler. 

  This function allows a silicon module to add its handler to the main
  list of handlers that will be executed on each SMI.

  @param[in] FunctionPtr    A pointer to the silicon module error handler
  @param[in]  Priority            The priority of the error handler

  @retval Status.

**/
EFI_STATUS
RegisterSiliconErrorHandler (
  IN      REPORTER_PROTOCOL  *Reporter,
  IN      UINT8              Priority
  );

#endif
