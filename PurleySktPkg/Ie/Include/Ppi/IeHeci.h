//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**

Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file IeHeci.h
    
  IeHeci PPI as defined in EFI 2.0

**/
#ifndef _PEI_IE_HECI_PPI_H_
#define _PEI_IE_HECI_PPI_H_

//
// PEI Timeout values
//
#define PEI_HECI_WAIT_DELAY    1000    // 1 ms
#define PEI_HECI_INIT_TIMEOUT  2000000 // 2 s
#define PEI_HECI_READ_TIMEOUT  500000  // 0.5 s
#define PEI_HECI_SEND_TIMEOUT  500000  // 0.5 s
#define PEI_HECI_RESET_TIMEOUT 2000000 // 2 sec


//
// Heci PPI definitions
//
#define PEI_IE_HECI_PPI_GUID \
  { \
     0x86d2df6a, 0x6f0d, 0x4d61, {0xbc, 0x3c, 0xb1, 0xbe, 0x2d, 0x2c, 0x1f, 0x61 }\
  }

typedef struct _PEI_IE_HECI_PPI PEI_IE_HECI_PPI;

typedef
EFI_STATUS
(EFIAPI *PEI_IE_HECI_SENDWACK) (
  IN      PEI_IE_HECI_PPI        *This,
  IN OUT  UINT32                 *Message,
  IN OUT  UINT32                 *Length,
  IN      UINT8                  HostAddress,
  IN      UINT8                  IeAddress
  );

typedef
EFI_STATUS
(EFIAPI *PEI_IE_HECI_READ_MESSAGE) (
  IN      PEI_IE_HECI_PPI        *This,
  IN      UINT32                 Blocking,
  IN      UINT32                 *MessageBody,
  IN OUT  UINT32                 *Length
  );

typedef
EFI_STATUS
(EFIAPI *PEI_IE_HECI_SEND_MESSAGE) (
  IN      PEI_IE_HECI_PPI        *This,
  IN      UINT32                 *Message,
  IN      UINT32                 Length,
  IN      UINT8                  HostAddress,
  IN      UINT8                  IeAddress
  );

typedef
EFI_STATUS
(EFIAPI *PEI_IE_HECI_INITIALIZE) (
  IN      PEI_IE_HECI_PPI           *This
  );

typedef
EFI_STATUS
(EFIAPI *PEI_IE_HECI_MEMORY_INIT_DONE_NOTIFY) (
  IN      PEI_IE_HECI_PPI        *This,
  IN      UINT32                 MrcStatus,
  IN OUT  UINT8                  *Action
  );


typedef struct _PEI_IE_HECI_PPI {

  PEI_IE_HECI_SENDWACK       SendwAck;
  PEI_IE_HECI_READ_MESSAGE   ReadMsg;
  PEI_IE_HECI_SEND_MESSAGE   SendMsg;
  PEI_IE_HECI_INITIALIZE     InitializeHeci;

  PEI_IE_HECI_MEMORY_INIT_DONE_NOTIFY MemoryInitDoneNotify;

} PEI_IE_HECI_PPI;

extern EFI_GUID gPeiIeHeciPpiGuid;


#endif
