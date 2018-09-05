/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++
  
  Copyright (c)  2001 - 2013 Intel Corporation.  All rights reserved.

  This software and associated documentation (if any) is furnished under 
  a license and may only be used or copied in accordance with the terms 
  of the license.  Except as permitted by such license, no part of this 
  software or documentation may be reproduced, stored in a retrieval system, 
  or transmitted in any form or by any means without the express written 
  consent of Intel Corporation.

  Module Name:
  
    PchGbe.asl
  
  Abstract:
  
    Gbe ASL methods. 
  
--*/

///
/// Gbe Ethernet ASL methods and structures 
///

  //
  // GPE bit 13 indicates wake from this device, can wakeup from S4 state
  //
  Method(_PRW, 0) {
    Return(Package() {13, 4})
  }