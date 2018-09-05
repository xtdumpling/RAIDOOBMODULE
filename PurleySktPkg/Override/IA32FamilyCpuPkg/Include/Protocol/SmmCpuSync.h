//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file


  Smm CPU Sync protocol definition.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _SMM_CPU_SYNC_PROTOCOL_H_
#define _SMM_CPU_SYNC_PROTOCOL_H_

#define SMM_CPU_SYNC_PROTOCOL_GUID \
  { \
    0xd5950985, 0x8be3, 0x4b1c, { 0xb6, 0x3f, 0x95, 0xd1, 0x5a, 0xb3, 0xb6, 0x5f } \
  }

typedef struct _SMM_CPU_SYNC_PROTOCOL SMM_CPU_SYNC_PROTOCOL;

//
// Data structures
//

typedef enum {
  SmmCpuSyncModeTradition,
  SmmCpuSyncModeRelaxedAp,
  SmmCpuSyncModeMax
} SMM_CPU_SYNC_MODE;

//
// Protocol functions
//

/**
  Given timeout constraint, wait for all APs to arrive, and insure if this function returns EFI_SUCCESS, then
  no AP will execute normal mode code before entering SMM, so it is safe to access shared hardware resource 
  between SMM and normal mode. Note if there are SMI disabled APs, this function will return EFI_NOT_READY.


  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.

  @retval EFI_SUCCESS           No AP will execute normal mode code before entering SMM, so it is safe to access 
                                shared hardware resource between SMM and normal mode
  @retval EFI_NOT_READY         One or more CPUs may still execute normal mode code
  @retval EFI_DEVICE_ERROR      Error occured in obtaining CPU status.

**/
typedef
EFI_STATUS
(EFIAPI *SMM_CPU_SYNC_CHECK_AP_ARRIVAL)(
  IN    SMM_CPU_SYNC_PROTOCOL            *This
  );


/**
  Set SMM synchronization mode starting from the next SMI run.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              The SMM synchronization mode to be set and effective from the next SMI run.

  @retval EFI_SUCCESS           The function completes successfully.
  @retval EFI_INVALID_PARAMETER SynMode is not valid.

**/
typedef
EFI_STATUS
(EFIAPI *SMM_CPU_SYNC_SET_MODE) (
  IN    SMM_CPU_SYNC_PROTOCOL            *This,
  IN    SMM_CPU_SYNC_MODE                SyncMode
  );


/**
  Get current effective SMM synchronization mode.

  @param  This                  A pointer to the SMM_CPU_SYNC_PROTOCOL instance.
  @param  SyncMode              Output parameter. The current effective SMM synchronization mode.

  @retval EFI_SUCCESS           The SMM synchronization mode has been retrieved successfully.
  @retval EFI_INVALID_PARAMETER SyncMode is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *SMM_CPU_SYNC_GET_MODE) (
  IN    SMM_CPU_SYNC_PROTOCOL            *This,
  OUT   SMM_CPU_SYNC_MODE                *SyncMode
  );

/**
  This protocol provides services on SMM CPU syncrhonization related status and controls
**/
struct _SMM_CPU_SYNC_PROTOCOL {
  SMM_CPU_SYNC_CHECK_AP_ARRIVAL      CheckApArrival;
  SMM_CPU_SYNC_SET_MODE              SetMode;
  SMM_CPU_SYNC_GET_MODE              GetMode;
};

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gSmmCpuSyncProtocolGuid;


#endif
