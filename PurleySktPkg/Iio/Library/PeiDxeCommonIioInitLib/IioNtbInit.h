//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IioNtbInit.h

  This file defines everything that's needed for IIO VData struct.

**/

#ifndef IIONTBINIT_H_
#define IIONTBINIT_H_


VOID
NtbInit (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8        IioIndex
  );

#ifndef IA32
VOID
NtbLateInit (
  IN  IIO_GLOBALS  *IioGlobalData,
  IN  UINT8         IioIndex
  );
#endif // IA32
#endif /* IIONTBINIT_H_*/
