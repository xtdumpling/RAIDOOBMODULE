/**
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
**/
/**

Copyright (c) 2010 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PchPlatformResetPolicy.h

  PCH Platform Reset policy protocol produced by a platform driver providing
  interface to return extended PCH reset types. This protocol is consumed 
  by the PCH drivers.

**/
#ifndef _PCH_PLATFORM_RESET_POLICY_H_
#define _PCH_PLATFORM_RESET_POLICY_H_

extern EFI_GUID                                   gDxePlatformResetPolicyProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _DXE_PLATFORM_RESET_POLICY_PROTOCOL  DXE_PLATFORM_RESET_POLICY_PROTOCOL;

///
/// SPEICAL_RESET_TYPES must be aligned with PCH_EXTENDED_RESET_TYPES
///
typedef enum {
  PowerCycleResetReq  = 3,
  GlobalResetReq,
  GlobalResetWithEcReq,
  MaxRestReq
} PLT_SPECIAL_RESET_TYPES;


typedef
PLT_SPECIAL_RESET_TYPES
(EFIAPI *PLTFORM_RESET)(
  IN EFI_RESET_TYPE  Types
  )
/**

  Do the platform special reset.
  It doesn't return if platform decides to do special reset,
  otherwise the Types contains the PCH_EXTENDED_RESET TYPES for PCH to reset system.

  @param Type                  Pointer to PCH_EXTENDED_RESET_TYPES

**/
;


//
// ------------ General Platform Reset Policiy protocol definition ------------
//
struct _DXE_PLATFORM_RESET_POLICY_PROTOCOL {
  PLTFORM_RESET     PlatformReset;
};

#endif
