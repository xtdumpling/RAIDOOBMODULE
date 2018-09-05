//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file AmtWrapperLib.h

--*/

#ifndef _AMT_WRAPPER_LIB_H_
#define _AMT_WRAPPER_LIB_H_

/**
  Return default enable console device used by out-of-band.

  Now if user not select the device in the menu and the sol type console
  device is enabled, default select this device as out-of-band used device.

**/
EFI_DEVICE_PATH_PROTOCOL *
GetDefaultDeviceForSpcr (
  );

#endif // _PLATFORM_CMOS_LIB_H_
