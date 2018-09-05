/**

Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file PlatformLtDxeLib.h
  

**/

#ifndef __PLATFORM_LT_DXE_LIB__
#define __PLATFORM_LT_DXE_LIB__

/**
  Platform hook for oem to disable Ltsx.

**/
VOID
EFIAPI
PlatformHookForDisableLtsx (
  VOID
  );
  
#endif

