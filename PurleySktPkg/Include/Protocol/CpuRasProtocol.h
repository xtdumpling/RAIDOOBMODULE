//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
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
  
    CpuiRasProtocol.h
    
Abstract:

  GUID for Cpu RAS protocol

--*/

#ifndef _CPU_RAS_PROTOCOL_H_
#define _CPU_RAS_PROTOCOL_H_


//
// Forward reference for pure ANSI compatability
//

#define EFI_CPU_RAS_PROTOCOL_GUID \
   { \
      0xf084ff45, 0xf9fa, 0x4e9e, 0x8d, 0xff, 0xe2, 0xd7, 0x80, 0xd2, 0x2c, 0xc2 \
   }


typedef 
EFI_STATUS 
(EFIAPI *EFI_CPU_RAS_ONLINE) (
);

typedef 
EFI_STATUS 
(EFIAPI *EFI_CPU_RAS_OFFLINE) (
);


typedef struct _EFI_CPU_RAS_PROTOCOL {
  UINT8                           FirstCpuRasEntry;
  UINT8                           SocketToBeOl;
  UINT8                           RASEvent; 
  EFI_CPU_RAS_ONLINE              CpuRasOnline;
  EFI_CPU_RAS_OFFLINE             CpuRasOffline;
} EFI_CPU_RAS_PROTOCOL;

extern EFI_GUID gEfiCpuRasProtocolGuid;


#endif
