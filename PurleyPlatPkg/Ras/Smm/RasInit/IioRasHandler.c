/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2008 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IioRasHandler.c

Abstract:

  This module implements the memory RAS flow control for memory hot
  adding/removal, spare migration and mirroring cutting off/resilvering
  through the interfaces provided by MemRas module.  

--*/
#include "RasInit.h"

extern BOOLEAN                 mExitImmediately;
EFI_CPU_RAS_PROTOCOL           *mCpuRas;
EFI_IIO_RAS_PROTOCOL           *mIioRasProtocol;
extern  UINT8                  mQpiCpuSktHotPlugEn;
extern  BOOLEAN                mIsSoftSmiRequest;
extern  UINT32                 mSmiParam[4];

EFI_STATUS
IioRasHandler(
  VOID
  )
{
  EFI_STATUS Status = EFI_INVALID_PARAMETER;

  if (mIsSoftSmiRequest && ((mSmiParam[0] != SMI_CPU_ONLINE_OFFLINE) && (mSmiParam[0] != SMI_CPU_HOT_ADD_REMOVE))) {
    return NO_REQUEST;
  }
  //
  // Check the setup option for QpiCpuSktHotPlugEn.  If it is not enabled, don't servie the Online/offline request
  //
  if (mQpiCpuSktHotPlugEn != 1) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nIIO RAS HANDLER: PLEASE ENABLE CPU HOTPLUG IN QPI SETUP TO PROCEED FURTHER WITH SKT ONLINE/OFFLINE\n"));
    return NO_REQUEST;
  }
  
  //Process the request
  if (mCpuRas->RASEvent == CpuRasOnlineRequest) {
    DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n\n  IIO %d Online Detected.\n", mCpuRas->SocketToBeOl));
	Status = mIioRasProtocol->IioOnline(mCpuRas->SocketToBeOl);

    if (Status == STATUS_DONE) {
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "    IIO Online is DONE.\n"));
    }

  } else if (mCpuRas->RASEvent == CpuRasOfflineRequest) {
    DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "    IIO %d Offline Detected.\n", mCpuRas->SocketToBeOl));
	Status = mIioRasProtocol->IioOffline(mCpuRas->SocketToBeOl);
    
    if (Status == STATUS_DONE) {
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "    IIO Offline is DONE.\n"));
    }
  }
  return Status;
}
