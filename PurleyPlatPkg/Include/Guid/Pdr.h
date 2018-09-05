//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Pdr.h

Abstract:
  

--*/

#ifndef _PDR_GUID_H_
#define _PDR_GUID_H_

#include <PiPei.h>

// {27E753F6-7CCF-46fd-AF01-75489D88D338}
#define PLATFORM_PDR_GUID \
{ 0x27e753f6, 0x7ccf, 0x46fd, { 0xaf, 0x1, 0x75, 0x48, 0x9d, 0x88, 0xd3, 0x38 } }

#pragma pack(1)

//
// PDR definition from Platform EAS
//
typedef struct _PLATFORM_PDR_TABLE {
  UINT8                       SystemUuid[16];     // 16 bytes
  UINT32                      Signature;          // "$PIT" 0x54495024
  UINT32                      Size;               // Size of the table
  UINT16                      Revision;           // Revision of the table
  //UINT16                      Checksum;         // Add in Grantley PDR definition, not enable now
  UINT16                      Flavor;             // 00- None, 01- Blade, 02- Telco
  UINT16                      Type;               // Platform Type
  UINT16                      PlatformRevision;   // Revision of the platform, Add in Grantley PDR definition

  } PLATFORM_PDR_TABLE;

#pragma pack()

STATIC EFI_GUID gPlatforPdrGuid = PLATFORM_PDR_GUID;

#endif
