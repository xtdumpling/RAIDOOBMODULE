/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2008 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CpuRasHandler.c

Abstract:



--*/

//BKL_PORTING - remove this when all macro re-definitions are solved
#pragma warning(disable :4005)
//BKL_PORTING

#include "RasInit.h"

extern  EFI_CPU_RAS_PROTOCOL                        *mCpuRas;
extern  BOOLEAN                                     mIsSoftSmiRequest;
extern  UINT32                                      mSmiParam[4];
extern  UINT8                                       FirstCpuRasEntry;
extern  UINT8                                       mHpRequest[6];
extern  BIOS_ACPI_PARAM                             *mAcpiParameter;
extern  BOOLEAN                                      mExitImmediately;
extern  EFI_ACPI_PCC_SHARED_MEMORY_REGION_RASF      *mRasfSharedMemoryRegion;
extern  UINT8                                       mQpiCpuSktHotPlugEn;
extern  IIO_UDS                                     *mIioUdsData;

UINT8   CpuOLRequest;
UINT8   TargetSocketIdList;

UINT8
CheckCpuOLRequest (
)
{
  UINT8       Counter;
  UINT8       Counter2;
  UINT8       SocketOccupied;
  UINT8       CurrentCpuStatus;
  UINT8       NumberOfActiveCpus;
  UINT8       CpuJumper;

  DEBUG     ((EFI_D_INFO, "\nCpu Ras Hanlder: CheckCpuOLRequest - Entry\n"));

  CpuOLRequest        = NoRequest;
  CurrentCpuStatus    = 0;
  NumberOfActiveCpus  = 0;

  //
  // Mask off only the CPU0-CPU3 bits for both socket occupied and jumper status.
  // This eliminates the IOH signals.
  //
  SocketOccupied = mHpRequest[1];
  CpuJumper      = mHpRequest[0];

  //
  // Get the current CPU status for all CPUs.  It is assumed that if a CPU gets
  // onlined or offlined during runtime, that the SocketBitMask will always reflect
  // the current state of the CPUs.
  //
  CurrentCpuStatus = (UINT8) (UINT8)mIioUdsData->SystemStatus.socketPresentBitMap;

  //
  // Sanity check against socket occupied.  That is, no online request from a socket
  // that isn't present.
  //
  CurrentCpuStatus &= SocketOccupied;
  CpuJumper        &= SocketOccupied;

  DEBUG     ((EFI_D_INFO, "\tCheckCpuOLRequest - SocketOccupied = %x\n",SocketOccupied));
  DEBUG     ((EFI_D_INFO, "\tCheckCpuOLRequest - CpuJumper = %x\n",CpuJumper));
  DEBUG     ((EFI_D_INFO, "\tCheckCpuOLRequest - CurrentCpuStatus = %x\n",CurrentCpuStatus));

  //
  // XORing will extract the change in status. Onlined CPU -> Offline request.  
  // Offline CPU -> Online request.
  //
  TargetSocketIdList =  CurrentCpuStatus ^ CpuJumper;
  DEBUG     ((EFI_D_INFO, "\tCheckCpuOLRequest - TargetSocketIdList = %x\n",TargetSocketIdList));

  //
  // Loop through all CPUs and look for online/offline requests for each.
  //
  for (Counter = 0; Counter < MAX_SOCKET; Counter++) {
    //
    // If the CPU has a pending online/offline request...
    //
    if (((TargetSocketIdList >> Counter) & 0x01) == 1) {

      //
      // Then if the CPU is currently online, it means the user
      // has requested to offline the CPU.
      //
      if (((CurrentCpuStatus >> Counter) & 0x01) == 1) {
        //
        // If we're requesting an offline, need to make sure that there are at 
        // least 2 processors installed in the system before offlining one of them.
        //
        for (Counter2 = 0; Counter2 < MAX_SOCKET; Counter2++) {
          NumberOfActiveCpus += ((((UINT8) mAcpiParameter->SocketBitMask) >> Counter2) & 0x01);
        }
        if (NumberOfActiveCpus > 1) {
          CpuOLRequest = CpuRasOfflineRequest;
          DEBUG     ((EFI_D_INFO, "\tCheckCpuOLRequest - It is OfflineRequest\n"));

        } else {
          CpuOLRequest = NoRequest;
          DEBUG     ((EFI_D_INFO, "\tCheckCpuOLRequest - NoRequest\n"));
        }
      } else {
        CpuOLRequest = CpuRasOnlineRequest;
        DEBUG     ((EFI_D_INFO, "\tCheckCpuOLRequest - It is OnlineRequest\n"));
      }

    }
  }

  return  CpuOLRequest;

}


EFI_STATUS
CpuRasHandler (
    VOID
) 
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8       MultipleReqs = 0;
  UINT8       Count = 0;


  //
  // If the user has requested a soft SMI, it means the mHpRequest jumper values have been hard coded for a specific
  // type of RAS event.  We can only proceed with the CpuRasHandler if the user has requested a CPU event.
  //
  if (mIsSoftSmiRequest && ((mSmiParam[0] != SMI_CPU_ONLINE_OFFLINE) && (mSmiParam[0] != SMI_CPU_HOT_ADD_REMOVE))) {
    return NO_REQUEST;
  }

  //
  // Check the setup option for QpiCpuSktHotPlugEn.  If it is not enabled, don't service the Online/offline request
  //
  DEBUG((EFI_D_LOAD|EFI_D_INFO, "\n The value of QpiCpuSktHotPlugEn is %d\n",mQpiCpuSktHotPlugEn));
  if (mQpiCpuSktHotPlugEn != 1) {
    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nCPU RAS HANDLER: PLEASE ENABLE CPU HOTPLUG IN QPI SETUP TO PROCEED FURTHER WITH SKT ONLINE/OFFLINE\n"));
    return NO_REQUEST;
  }

  if (FirstCpuRasEntry) {

    DEBUG((EFI_D_LOAD|EFI_D_INFO, "\nFirst Time Entry to CPU Ras Handler\n"));
    FirstCpuRasEntry  = 0;
    mCpuRas->FirstCpuRasEntry = 1;
    
    //
    // Check the request corresponds to Online or Offline request
    //
    CpuOLRequest  = CheckCpuOLRequest();

    //
    // Return NO_REQUEST if the request is not for CPU.
    //
    if (CpuOLRequest == NoRequest) {
      mExitImmediately = TRUE;
      return NO_REQUEST;
    }


    //
    // Update the SocketID and Multiple online and Offline Reqs.
    //
    for (Count = 0; Count <= MAX_SOCKET; Count++)  {
      if ((TargetSocketIdList >> Count ) & 01)  {
        MultipleReqs++;
        mCpuRas->SocketToBeOl = Count;
      }
    }

    
    //
    // Return error if multiple requests come.
    //
    if (MultipleReqs > 01) {
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "    Detected more than 1 jumper change. No RAS Request will be processed.\n"));
      mExitImmediately = TRUE;
      return  ERR_CPU_MULTIPLE_REQ;
    }

    //
    // Assert error if there is no request.
    //
    if (MultipleReqs == 00) {
      ASSERT_EFI_ERROR (EFI_INVALID_PARAMETER);
    }

    mCpuRas->RASEvent = CpuOLRequest;
  }

  

  if (CpuOLRequest == CpuRasOnlineRequest) {
    Status = mCpuRas->CpuRasOnline();
    if (Status == ERROR_EXIT) {
      mExitImmediately = TRUE;
    }
  } else if (CpuOLRequest == CpuRasOfflineRequest) {
    Status = mCpuRas->CpuRasOffline();
    if (Status == NO_SUPPORT || Status == ERROR_EXIT || Status == STATUS_DONE) {
      mExitImmediately = TRUE;
    }
  }

  return Status;
}
