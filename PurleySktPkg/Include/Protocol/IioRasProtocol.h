//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IioRasProtocol.h

Abstract:

  IIO Ras ProtocolCode

--*/

#ifndef _IIO_RAS_PROTOCOL_H_
#define _IIO_RAS_PROTOCOL_H_

#include <Guid/MemoryMapData.h>

#define EFI_IIO_RAS_PROTOCOL_GUID \
  {0x4c7e45bc, 0x8a23, 0x26cd, 0x94, 0xad, 0x5d, 0x2c, 0x26, 0x3f, 0x25, 0xfe}

#define EFI_FORWARD_DECLARATION(x) typedef struct _##x x
EFI_FORWARD_DECLARATION (EFI_IIO_RAS_PROTOCOL);

//
// Protocol Definition
//
typedef
EFI_STATUS
(EFIAPI *EFI_IIO_ONLINE) (
  IN UINT8 Socket
  );

typedef
EFI_STATUS
(EFIAPI *EFI_IIO_OFFLINE) (
  IN UINT8 Socket
  );

typedef struct _EFI_IIO_RAS_PROTOCOL {
  EFI_IIO_ONLINE  IioOnline;
  EFI_IIO_OFFLINE IioOffline;
} EFI_IIO_RAS_PROTOCOL;

extern EFI_GUID gEfiIioRasProtocolGuid;

#endif // _IIO_RAS_PROTOCOL_H_
