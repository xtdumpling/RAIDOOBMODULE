//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/**

Copyright (c)  1999 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file Heci.h
    
  Heci PPI as defined in EFI 2.0

**/
#ifndef _PEI_HECI_PPI_H_
#define _PEI_HECI_PPI_H_

#include <Protocol/HeciProtocol.h>

#if 0
//
// Ignore ME_FW_INIT_COMPLETE status Macro
//
#define ME_STATUS_IGNORE_FW_INIT_COMPLETE(a)  ((a) & (~ME_FW_INIT_COMPLETE))

//
// Macro to check if ME FW INIT is completed
//
#define ME_STATUS_IS_ME_FW_INIT_COMPLETE(a) (((a) & ME_FW_INIT_COMPLETE) == ME_FW_INIT_COMPLETE)

//
// Marco to combind the complete bit to status
//
#define ME_STATUS_WITH_ME_INIT_COMPLETE(a)  ((a) | ME_FW_INIT_COMPLETE)

//
// Abstract ME Status definitions
//   Bit0-3 ME Status
//   Bit6-4 Reserved
//   Bit7   ME FW Init Complete
//
#define ME_READY            0x00
#define ME_IN_RECOVERY_MODE 0x02
#define ME_NOT_READY        0x0F
#define ME_FW_INIT_COMPLETE 0x80

//
// Abstract ME Mode Definitions
//
#define ME_MODE_NORMAL        0x00
#define ME_MODE_ALT_DIS       0x02
#define ME_MODE_IGNITION      0x01
#define ME_MODE_TEMP_DISABLED 0x03
#define ME_MODE_SECOVER       0x04
#define ME_MODE_DFX_FW        0x05
#define ME_MODE_FAILED        0x06


extern EFI_GUID gPeiHeciPpiGuid;
#endif

///
/// HECI PPI GUID
/// This PPI provides an interface to communicate with Intel ME in PEI phase
///

extern EFI_GUID gHeciPpiGuid;

///
/// Revision
///
#define HECI_PPI_REVISION_1  1

typedef struct _HECI_PPI  HECI_PPI;

///
/// HECI PPI
/// The interface functions are for sending/receiving HECI messages between host and Intel ME subsystem
/// in PEI phase
///
struct _HECI_PPI {
  HECI_SENDWACK               SendwAck;       ///< Send HECI message and wait for respond
  HECI_READ_MESSAGE           ReadMsg;        ///< Read message from HECI
  HECI_SEND_MESSAGE           SendMsg;        ///< Send message to HECI
  HECI_INIT                   InitializeHeci; ///< Init HECI
  HECI_GET_ME_STATUS          GetMeStatus;    ///< Get Intel ME Status register
  HECI_GET_ME_MODE            GetMeMode;      ///< Get Intel ME mode
};



/******************************************************************************
 */
typedef
EFI_STATUS
(EFIAPI *PEI_HECI2_GET_ME_STATUS) (
  IN UINT32               *Status
  );

typedef 
EFI_STATUS
(EFIAPI *PEI_HECI2_INITIALIZE) (
  VOID
  );

typedef 
EFI_STATUS
(EFIAPI *PEI_HECI2_STALL_PPI) (
  IN UINTN                Microseconds
  );

typedef struct _PEI_HECI2_PPI {
  PEI_HECI2_INITIALIZE    Heci2Initialize;
  PEI_HECI2_STALL_PPI     Heci2Stall;
  PEI_HECI2_GET_ME_STATUS Heci2GetMeStatus;
} PEI_HECI2_PPI;

extern EFI_GUID gPeiHeci2PpiGuid;


#endif // _HECI_PPI_H_

