//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IioInitDxeLib.h

Abstract:

  This file provides required function headers publish for IioInitLib

--*/

#ifndef _IIO_INIT_DXE_LIB_H_
#define _IIO_INIT_DXE_LIB_H_

VOID
IioIoatInitBootEvent (
  IN  IIO_GLOBALS                  *IioGlobalData,
  IN  UINT8                        IioIndex
  );

VOID
IioCommonInitAfterResources (
  IN  IIO_GLOBALS  *IioGlobalData
);

EFI_STATUS
IioPostLateInitialize (
  IIO_GLOBALS  *IioGlobalData
  );

EFI_STATUS
LockMsr (
  VOID
  );

EFI_STATUS
IioSecurePlatform (
  IIO_GLOBALS  *IioGlobalData
  );

#endif // _IIO_INIT_DXE_LIB_H_
