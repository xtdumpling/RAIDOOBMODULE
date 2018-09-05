//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file

  Copyright (c) 2011 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef __USRA_TRACE_H__
#define __USRA_TRACE_H__


#include <UsraAccessType.h>

typedef enum {
  TraceRead = 0,
  TraceWrite = 1,
  TraceModify = 2,
  TraceGetAddr = 3
} TraceOperation;

/**
  This routine is for print register info to serial port.
  
  @param[in] Address              A pointer of the address of the USRA Address Structure
  @param[in] AlignedAddress       Physical address converted from USRA Address
  @param[in] Operation            Operation type in TraceOperation
  @param[in] Buffer1              A pointer of data buffer for TraceRead and TraceWrite
  @param[in] Buffer2              A pointer of data buffer for TraceModify

  @retval NONE
**/
VOID
USRATrace(
  IN USRA_ADDRESS             *Address,
  IN UINTN                    AlignedAddress,
  IN UINT8                    Operation,
  IN VOID                     *Buffer1,
  IN VOID                     *Buffer2
  );


#endif
