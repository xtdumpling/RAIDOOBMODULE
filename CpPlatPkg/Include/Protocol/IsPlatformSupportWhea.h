//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  IsPlatformSupportWhea Protocol.

  Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
**/

#ifndef _IS_PLATFORM_SUPPORT_WHEA_H_
#define _IS_PLATFORM_SUPPORT_WHEA_H_

typedef enum
{
	WheaNotSupported    = 0x00,
	WheaHestSupport     = 0x01,
	WheaBertSupport     = 0x02,
	WheaEinjSupport     = 0x04,
	WheaErstSupport     = 0x08,
	WheaSupported       = 0x0F  // WheaHestSupport|WheaBertFeature|WheaEinjFeature|WheaErstFeature
} WHEA_SUPPORT_LEVEL; 


typedef struct _EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL;

typedef
BOOLEAN
(EFIAPI *IS_PLATFORM_SUPPORT_WHEA)  (
IN EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL          *This
  );
/*++


Whea core calls this function to see if the specific platform support WHEA

--*/

typedef struct _EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL {
  WHEA_SUPPORT_LEVEL            WheaSupport;
  UINT8                         WheaAcpi5Support;
  IS_PLATFORM_SUPPORT_WHEA      IsPlatformSupportWhea;
} EFI_IS_PLATFORM_SUPPORT_WHEA_PROTOCOL;

extern EFI_GUID     gEfiIsPlatformSupportWheaProtocolGuid;

#endif /* _IS_PLATFORM_SUPPORT_WHEA_H_ */

