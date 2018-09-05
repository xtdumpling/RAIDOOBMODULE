//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PpmProtocol.h
    
Abstract:

  Protocol used for specifying Power Management information

--*/

#ifndef _CPU_PPM_PROTOCOL_H_
#define _CPU_PPM_PROTOCOL_H_

#include <Library/CpuPpmLib.h>

#define EFI_CPU_PPM_PROTOCOL_GUID \
  { \
    0x7e6a6cf5, 0xc89c, 0x492f, 0xac, 0x37, 0x23, 0x7, 0x84, 0x9c, 0x3a, 0xd5 \
  }

// {7E6A6CF5-C89C-492f-AC37-2307849C3AD5}


//
// Interface structure for the PPM Protocol
//
typedef struct _EFI_CPU_PPM_PROTOCOL {

  EFI_CPU_PM_STRUCT     *CpuPpmStruct;
  
} EFI_CPU_PPM_PROTOCOL;

extern EFI_GUID gEfiCpuPpmProtocolGuid;

#endif
