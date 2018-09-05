//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
  LpcPolicy.h

Abstract:
  This file defines platform policies for LPC chip driver.

--*/

#ifndef _LPC_POLICY_H_
#define _LPC_POLICY_H_

#define EFI_LPC_POLICY_PROTOCOL_COM_MAX 4

typedef struct {
  BOOLEAN Enable[EFI_LPC_POLICY_PROTOCOL_COM_MAX];
  UINT16  Address[EFI_LPC_POLICY_PROTOCOL_COM_MAX];
  UINT16  Irq[EFI_LPC_POLICY_PROTOCOL_COM_MAX];
} EFI_LPC_POLICY_PROTOCOL_COM;

#define EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX  4

typedef enum {
  EfiLpcPolicyParallelOutOnly,
  EfiLpcPolicyParallelBiDir,
  EfiLpcPolicyParallelEpp,
  EfiLpcPolicyParallelEcp,
  EfiLpcPolicyParallelLast
} EFI_LPC_POLICY_PROTOCOL_PARALLEL_MODE;

typedef struct {
  BOOLEAN                               Enable[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  UINT16                                Address[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  UINT16                                Irq[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  UINT16                                DmaChannel[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
  EFI_LPC_POLICY_PROTOCOL_PARALLEL_MODE Mode[EFI_LPC_POLICY_PROTOCOL_PARALLEL_MAX];
} EFI_LPC_POLICY_PROTOCOL_PARALLEL;

typedef struct {
  BOOLEAN Enable;
} EFI_LPC_POLICY_PROTOCOL_KEYBOARD;

typedef struct {
  BOOLEAN Enable;
} EFI_LPC_POLICY_PROTOCOL_MOUSE;

typedef enum {
  EfiLpcPolicyFloppy0M36,
  EfiLpcPolicyFloppy1M20,
  EfiLpcPolicyFloppy0M72,
  EfiLpcPolicyFloppy1M44,
  EfiLpcPolicyFloppy2M88,
  EfiLpcPolicyFloppyLast
} EFI_LPC_POLICY_PROTOCOL_FLOPPY_TYPE;

#define EFI_LPC_POLICY_PROTOCOL_FLOPPY_MAX  4

typedef struct {
  BOOLEAN                             Enable[EFI_LPC_POLICY_PROTOCOL_FLOPPY_MAX];
  EFI_LPC_POLICY_PROTOCOL_FLOPPY_TYPE Type[EFI_LPC_POLICY_PROTOCOL_FLOPPY_MAX];
} EFI_LPC_POLICY_PROTOCOL_FLOPPY;

typedef struct _EFI_LPC_POLICY_PROTOCOL {
  EFI_LPC_POLICY_PROTOCOL_COM       PolicyCom;
  EFI_LPC_POLICY_PROTOCOL_PARALLEL  PolicyParallel;
  EFI_LPC_POLICY_PROTOCOL_FLOPPY    PolicyFloppy;
  EFI_LPC_POLICY_PROTOCOL_KEYBOARD  PolicyKeyboard;
  EFI_LPC_POLICY_PROTOCOL_MOUSE     PolicyMouse;
} EFI_LPC_POLICY_PROTOCOL;

extern EFI_GUID gEfiLpcPolicyProtocolGuid;

#endif
