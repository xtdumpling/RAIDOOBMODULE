/**@file
  ACPI DSDT table for MEI1

@copyright
 Copyright (c) 2015 Intel Corporation. All rights reserved
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

Scope(\_SB.PCI0) {
  //
  // Management Engine Interface 1 - Device 22, Function 0
  //
  Device(HECI) {
    // APTIOV_SERVER_OVERRIDE_RC_START
    //Name(_ADR, 0x00160000)
    // APTIOV_SERVER_OVERRIDE_RC_END

    Method(_DSM, 0x4, NotSerialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj}) {
      if(PCIC(Arg0)) { return(PCID(Arg0,Arg1,Arg2,Arg3)) }
      //Fix warning: not all control paths return a value
      Return(0)
    } // End _DSM
  } // Device(HECI)
}

