//
// This file contains an 'Intel Peripheral Driver' and is
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may
// be modified by the user, subject to additional terms of the
// license agreement
//
/**

Copyright (c)  1999 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file VlsVariableGuid.h

  GUID used for VIRTUAL_LOCKSTEP_VARIABLE.

**/

#ifndef _VIRTUAL_LOCKSTEP_GUID_H_
#define _VIRTUAL_LOCKSTEP_GUID_H_

#define VIRTUAL_LOCKSTEP_VARIABLE_GUID { 0x9189541f, 0xac0c, 0x4368, 0x90, 0x62, 0x70, 0xe1, 0x95, 0x7c, 0x34, 0x45 }

#define VIRTUAL_LOCKSTEP_VARIABLE_NAME L"VLSEstablished"

#pragma pack(push,1)
typedef struct{
//
// VLS Happened in previous boot
//
// If there was a device failure that lead to a sparing operation and VLS
// then the next boot must be forced to a normal boot
// Save the information if VLS was established in a Boolean variable.
//

  BOOLEAN         VirtualLockstepEstablished;
} VIRTUAL_LOCKSTEP_VARIABLE_DATA;

extern EFI_GUID gEfiVirtualLockstepGuid;

#pragma pack(pop)
#endif
