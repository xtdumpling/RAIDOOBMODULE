/** @file
  Watchdog Timer PPI

@copyright
 Copyright (c) 2010 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _PEI_WDT_H_
#define _PEI_WDT_H_

#include <Protocol/Wdt.h>
//
// MRC takes a lot of time to execute in debug mode
//
#define WDT_TIMEOUT_BETWEEN_PEI_DXE 60

//
// Extern the GUID for PPI users.
//
extern EFI_GUID       gWdtPpiGuid;

///
/// Reuse WDT_PROTOCOL definition
///
typedef WDT_PROTOCOL  WDT_PPI;

#endif
