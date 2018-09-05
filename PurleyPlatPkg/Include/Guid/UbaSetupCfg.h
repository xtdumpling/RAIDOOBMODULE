//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    UbaSetupCfg.h

Abstract:

    Setup variable for Uba Support.

Revision History

--*/

#ifndef __UBA_SETUP_CONFIG_H__
#define __UBA_SETUP_CONFIG_H__

#define UBA_SETUP_CONFIG_SIGNATURE   SIGNATURE_32('M', 'O','S', 'C')
#define UBA_SETUP_CONFIG_VERSION     01

#define UBA_SETUP_CONFIG_VAR_NAME    L"UbaSetupCfg"


// {6426F33E-0950-470f-9EA7-AE157661973A}
#define UBA_SETUP_CONFIG_GUID \
  { \
    0x6426f33e, 0x950, 0x470f, 0x9e, 0xa7, 0xae, 0x15, 0x76, 0x61, 0x97, 0x3a \
  }


#pragma pack(1)

typedef 
struct _UBA_SETUP
{

  UINT32      Signature;
  UINT32      Version;
  UINT32      Size;
  UINT8       Type;
  UINT8       SystemUuid[16];

  //
  // Left for platform use.
  //

  UINT8       RESV[35];     // Default total size is 64 bytes, padding left bytes.

} UBA_SETUP;

#pragma pack()



#ifndef VFRCOMPILE

STATIC EFI_GUID gUbaSetupCfgGuid = UBA_SETUP_CONFIG_GUID;
#endif


#endif // #ifndef __UBA_SETUP_CONFIG_H__
