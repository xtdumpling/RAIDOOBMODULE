//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**@file
  
Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef __HII_SETUP_ENTER_H__
#define __HII_SETUP_ENTER_H__

///
/// This Guid is installed as protocol guid to notify system enter into Front Page for Setup configuration.
///
#define EFI_SETUP_ENTER_GUID \
  { 0x71202EEE, 0x5F53, 0x40d9, {0xAB, 0x3D, 0x9E, 0x0C, 0x26, 0xD9, 0x66, 0x57 }}

extern EFI_GUID gEfiSetupEnterGuid;

#endif
