//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Intel CPU PCDs Set Done protocol should be installed after CPU related PCDs
  are set by platform driver. CPU driver only could get CPU PCDs after this 
  protocol installed.

Copyright (c) 2013, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _INTEL_CPU_PCDS_SET_DONE_PROTOCOL_H_
#define _INTEL_CPU_PCDS_SET_DONE_PROTOCOL_H_

#define INTEL_CPU_PCDS_SET_DONE_PROTOCOL_GUID \
  { \
    0xadb7b9e6, 0x70b7, 0x48d4, { 0xb6, 0xa5, 0x18, 0xfa, 0x15, 0xeb, 0xcd, 0x78 } \
  }

extern EFI_GUID gIntelCpuPcdsSetDoneProtocolGuid;

#endif
