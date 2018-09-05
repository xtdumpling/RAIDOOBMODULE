/**@file

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


//
// _DSM : Device Specific Method supporting USB Sideband Deferring function
//
// Arg0: UUID Unique function identifier
// Arg1: Integer Revision Level
// Arg2: Integer Function Index
// Arg3: Package Parameters
//
Method (_DSM, 4, Serialized, 0, UnknownObj, {BuffObj, IntObj, IntObj, PkgObj})
{

  If (LEqual(Arg0, ToUUID ("A5FC708F-8775-4BA6-BD0C-BA90A1EC72F8")))
  {
    //
    // Switch by function index
    //
    Switch (ToInteger(Arg2))
    {
      //
      // Standard query - A bitmask of functions supported
      // Supports function 0-2
      //
      Case (0)
      {
        if (LEqual(Arg1, 1)){ // test Arg1 for the revision 
          Return (Buffer () {0x07})
        } else {
          Return (Buffer () {0})
        }
      }
      //
      // USB Sideband Deferring Support
      //   0: USB Sideband Deferring not supported on this device
      //   1: USB Sideband Deferring supported
      //
      Case (1)
      {
        if (LEqual(SDGV,0xFF)){ // check for valid GPE vector
          Return (0)
        } else {
          Return (1)
        }
      }
      //
      // GPE Vector
      //  Return the bit offset within the GPE block of the GPIO (HOST_ALERT) driven by this device
      //
      Case (2)
      {
        Return (SDGV)
      }
    }
  }

  Return (0)
}
