/** @file

@copyright
 Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains a 'Sample Driver' and is licensed as such
 under the terms of your license agreement with Intel or your
 vendor. This file may be modified by the user, subject to
 the additional terms of the license agreement.

@par Specification Reference:
**/


// ITSS
// Define the needed ITSS registers used by ASL on Interrupt
// mapping.

scope(\_SB){
      OperationRegion(ITSS, SystemMemory, 0xfdc43100, 0x208)
      Field(ITSS, ByteAcc, NoLock, Preserve)
      {
        PARC, 8,
        PBRC, 8,
        PCRC, 8,
        PDRC, 8,
        PERC, 8,
        PFRC, 8,
        PGRC, 8,
        PHRC, 8,
       Offset(0x200),   // Offset 3300h ITSSPRC - ITSS Power Reduction Control
            , 1,
            , 1,
        SCGE, 1,        // ITSSPRC[2]: 8254 Static Clock Gating Enable (8254CGE)

      }
}


