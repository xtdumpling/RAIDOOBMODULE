//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2009, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


    @file EfiLegacyBootMarker.h
    
  GUID used to detmerine if we are supporting an EFI Optimized boot.

**/

#ifndef _EFI_LEGACY_BOOT_MARKER_GUID_H_
#define _EFI_LEGACY_BOOT_MARKER_GUID_H_

#define EFI_LEGACY_BOOT_MARKER_GUID \
  { \
 0x72e07da8, 0x63a9, 0x42d4,  0x83, 0xd5, 0xb, 0xb5, 0x18, 0x38, 0x44, 0x75 \
  }


#define EFI_WINDOWS_INT10_WORKAROUND \
  { \
  0x387f555, 0x20a8, 0x4fc2,  0xbb, 0x94, 0xcd, 0x30, 0xda, 0x1b, 0x40, 0x8  \
  } 


extern EFI_GUID gEfiLegacyBootMarkerGuid;
extern EFI_GUID gEfiWindowsInt10Workaround;
extern EFI_GUID gEfiUsbLegacyStack;

#endif
