//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/**

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AdminPasswordGuid.h

  GUIDs used for create HOB for prevent admin password resetting
  during CMOS clear. 

**/

#ifndef _ADMIN_PASSWORD_HOB_GUID_H_
#define _ADMIN_PASSWORD_HOB_GUID_H_

//
// Definitions for TCG-Defined Wake Event Data
//
#define EFI_ADMIN_PASSWORD_DATA_HOB_GUID \
    { 0x4E173307, 0x92DB, 0x4a4f, {0x9A, 0x8E, 0x86, 0xA8, 0x56, 0x49, 0xB4, 0x05 }}

extern EFI_GUID gEfiAdminPasswordHobGuid;

#endif // _ADMIN_PASSWORD_HOB_GUID_H_
