/** @file
  This file contains definitions of IIO Policy HOB.

@copyright
 Copyright (c) 2016 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#ifndef _IIO_POLICY_HOB_H_
#define _IIO_POLICY_HOB_H_

extern EFI_GUID gIioPolicyHobGuid;
extern EFI_GUID gEfiOpaSocketMapHobGuid;

typedef struct {
    UINT8 Valid;
    UINT8 SocketId;
    UINT8 Segment;
    UINT8 BusBase;
    UINT8 BusLimit;
    UINT8 Reserved;
    UINT8 Reserved2;
    UINT8 Reserved3;
}  EFI_OPA_SOCKET_MAP;

typedef struct {
  EFI_HOB_GUID_TYPE   EfiHobGuidType; // mandatory field
  EFI_OPA_SOCKET_MAP  Info[MaxIIO];
} OPA_SOCKET_MAP_HOB;

#endif // _IIO_POLICY_HOB_H_

