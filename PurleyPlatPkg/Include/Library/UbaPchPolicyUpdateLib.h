/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UbaPchPolicyUpdateLib.h

Abstract:

--*/

#ifndef _PLATFORM_PCH_POLICY_UPDATE_LIB_H
#define _PLATFORM_PCH_POLICY_UPDATE_LIB_H

#include <Base.h>
#include <Uefi.h>

#include <Ppi/PchPolicy.h>


#define PLATFORM_PCH_POLICY_UPDATE_SIGNATURE  SIGNATURE_32 ('P', 'C', 'H', 'P')
#define PLATFORM_PCH_POLICY_UPDATE_VERSION    01

// {DA33A8DD-9C7C-4af9-BF21-D70909F05575}
#define   PLATFORM_PCH_POLICY_DATA_GUID \
{ 0xda33a8dd, 0x9c7c, 0x4af9, { 0xbf, 0x21, 0xd7, 0x9, 0x9, 0xf0, 0x55, 0x75 } }


typedef
EFI_STATUS
(*PCH_POLICY_UPDATE_CALLBACK) (
  // APTIOV_SERVER_OVERRIDE_RC_START : To avoid build error because of DXE_PCH_PLATFORM_POLICY_PROTOCOL declaration not available.
  //IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL  *DxePchPolicy
  IN  VOID  *DxePchPolicy
  // APTIOV_SERVER_OVERRIDE_RC_END : To avoid build error because of DXE_PCH_PLATFORM_POLICY_PROTOCOL declaration not available.
);

typedef struct 
{
  UINT32                          Signature;
  UINT32                          Version;
  
  PCH_POLICY_UPDATE_CALLBACK      CallPchPolicyUpdate;
  
} PLATFORM_PCH_POLICY_UPDATE_TABLE;

EFI_STATUS
PlatformUpdatePchPolicy(
  // APTIOV_SERVER_OVERRIDE_RC_START : To avoid build error because of DXE_PCH_PLATFORM_POLICY_PROTOCOL declaration not available.
  //IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL  *DxePchPolicy
  IN  VOID  *DxePchPolicy
  // APTIOV_SERVER_OVERRIDE_RC_END : To avoid build error because of DXE_PCH_PLATFORM_POLICY_PROTOCOL declaration not available.
);


STATIC  EFI_GUID gPlatformPchPolicygDataGuid = PLATFORM_PCH_POLICY_DATA_GUID;

#endif //_PLATFORM_PCH_POLICY_UPDATE_LIB_H
