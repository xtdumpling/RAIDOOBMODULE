//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Declaration decorator to allow for proper lock or semaphore alignment.

  Copyright (c) 2015, Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#ifndef _LOCKALIGN_H_
#define _LOCKALIGN_H_

#if defined(__GNUC__)
#define ALIGN128        __attribute__((aligned(128)))
#elif defined(_MSC_VER)
#pragma warning(disable: 4324) // Disable warning about padding
#define ALIGN128        __declspec(align(128))
#else
#define ALIGN128
#endif

#endif  // _LOCKALIGN_H_