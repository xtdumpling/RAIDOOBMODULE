/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2009 - 2015, Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.



   @file QuiesceMain.c

    Quiesce Main service inplementation

Revision History

**/


#include "QuiesceMain.h"
#include <Library/PcdLib.h>
#include <Library/mpsyncdatalib.h>


//BKL_PORTING - need to be updated for Platform

#define THREAD_QUIESCE_STAKC_SIZE   0x400
#define QUIESCE_DATA_STACK_SIZE     (2 * THREAD_QUIESCE_STAKC_SIZE)

UINT32                      *mQuiesceStatusLocation;
UINT32                      mCpuFamilModel = 0;
UINT32                      mSocketLevelShift;
UINTN                       mCpuCounter;
SPIN_LOCK                   mApLock;      

UINT64                      Misc_feature_control;
UINT8                       mMirrorEnabled;

UINT32  mDoIioProgram[MAX_SOCKET];  //store thread ApicId

extern EFI_SMM_CPU_PROTOCOL         *mSmmCpu; 
extern EFI_SMM_CPU_SERVICE_PROTOCOL *mSmmCpuServiceProtocol;
extern IIO_UDS                      *mIioUdsData;


/**

    Main flow for monarch.

		This routine contains main Quiesce flow for Monarch execution 
		system reconfiguration. The routine is in Assembly.

    @param Cache Base, MMCFG Base, MiscFeatureControl Value, SMTFlag

    @retval Status - EFI_STATUS

**/
extern EFI_STATUS
DoQuiesce (
    IN UINT64 CacheBase,
    IN UINT64 MMCfgBase,
    IN UINT64 MiscFeatureControlValue,
    IN BOOLEAN  SMTFlag
);

/**

    Monarch set a flag to bring APs out of spin loop and enter Quiesce Lib.

    @param	None

    @retval	None

**/
VOID
BringInAps(
    VOID
);

/**

    This routine resets the Quiesce flag, Notifys APs that system re-config
		has finished, update CPU socket info and clean up the Quiesce Table.

    @param	CacheBase	address of the Quiesce code

    @retval	None

**/
VOID
PostUnQuiesce(
    IN UINT64 CacheBase
);

UINT64
GetTimerTick(
    VOID
);

/**

    Quiesce Main initialization routine

		This routine initializes the memory for Quiesce, including preparing
		quiesce page tables, copying quiesce code into Quiesce code area etc.

		@param	None

    @retval	None

**/
VOID
QuiesceMainInit(
    VOID
)
{
  UINT8     i;
  // Update each socket mmcfgbase 
  for(i = 0; i < MAX_SOCKET; i++) {
    mQuiesceCommData.MMCfgBase[i] = (UINT64)(mIioUdsData->PlatformData.CpuQpiInfo[i].SegMmcfgBase.hi);
    mQuiesceCommData.MMCfgBase[i] = mQuiesceCommData.MMCfgBase[i] << 32;
    mQuiesceCommData.MMCfgBase[i] |= (UINT64)(mIioUdsData->PlatformData.CpuQpiInfo[i].SegMmcfgBase.lo);
  }
	
	//
  // Clear Quiesce Status shared by Monach and APs
  //	1. get Quiesce Status location from bottom of Quiesce stack (i.e. bottom of Monarch AP stack)
	//
  mQuiesceStatusLocation = (UINT32 *)(UINTN)(mQuiesceCommData.DataBaseAddress + QUIESCE_CACHE_DATA_SIZE - QUIESCE_DATA_STACK_SIZE);
	//
  //	2. init Quiesce status with Monarch AP
	//
  *(volatile UINT32 *)mQuiesceStatusLocation = 0;
	//
  //	3. init Quiesce status with other APs
	//
  mQuiesceCommData.QuiesceStatus4APs = 0;

  mQuiesceCommData.MonarchApicId = (UINT32)-1;    //init to invalid Id

  AsmCpuid (EFI_CPUID_VERSION_INFO, &mCpuFamilModel, NULL, NULL, NULL);

  //
	//Assuming we have the same shift for all socket, so use SBSP's data
	//
   AsmCpuidEx (EFI_CPUID_CORE_TOPOLOGY, 1, &mSocketLevelShift, NULL, NULL, NULL);
   mSocketLevelShift &= 0x1F;

} // end of InitQuiesceMemory

/**

  This routine disables MSI/VLW from IOH PCIE/Internal devices and sets quiesce command.
  
  @param QuiesceFlag Bool indicating SYSTEM_QUIESCE 1, SYSTEM_UNQUIESCE  0
 
  @retval EFI_STATUS   

**/
EFI_STATUS
EFIAPI
SystemQuiesceUnQuiesce(
    BOOLEAN QuiesceFlag
)
{
  UINT64  MsrValue;
  EFI_STATUS  Status = EFI_SUCCESS;
  STATIC  UINT64  OldMiscMsr, OldPatMsr;

  UINT64		CpuIndex;
  UINT64		NumberOfAPsinSmm = 0;
  UINT64		ProcessorId;
  UINTN         BspIndex;
  EFI_STATUS    Status1 = EFI_SUCCESS;


  if(QuiesceFlag == SYSTEM_QUIESCE) { 
		//
    // Set Quiesce
		//
    MsrValue = AsmReadMsr64(MSR_QUIESCE_CTL1);
    if (!(MsrValue & 0x1)){ // System is not Quiesced yet      
      //
      // Clear Quiesce status flag so AP will wait there
      //
      mQuiesceCommData.QuiesceStatus4APs &= ~B_UNQUIESCED;        //communicate to APs

      //
      // Bring all AP into a spin loop for BSP completion
      // Wait for all AP to get into the spin loop
      //  
      mCpuCounter = 1;
      InitializeSpinLock (&mApLock);      

      Status1 = mSmmCpuServiceProtocol->WhoAmI (mSmmCpuServiceProtocol, &BspIndex);
      ASSERT_EFI_ERROR (Status1);
      
      for (CpuIndex = 0; CpuIndex < gSmst->NumberOfCpus; CpuIndex++) {

        if (CpuIndex == BspIndex) {
          continue;
        }

        Status1 = mSmmCpu->ReadSaveState (
                        mSmmCpu,
                        (UINTN)sizeof (UINT64),
                        EFI_SMM_SAVE_STATE_REGISTER_PROCESSOR_ID ,
                        CpuIndex,
                        &ProcessorId
                        );
        if (Status1 == EFI_NOT_FOUND) {
		      continue;
	      } else if (Status1 == EFI_SUCCESS) {
          NumberOfAPsinSmm++;
          gSmst->SmmStartupThisAp ((EFI_AP_PROCEDURE) QuiesceApPollingProc, CpuIndex, (VOID *)NULL);
	      } else {
		    ASSERT(FALSE);
	      }
      }

      while (mCpuCounter <= NumberOfAPsinSmm) {
        CpuPause ();
      }

      DisableIioPcieMsg(PROGRAM_ALL);   

      //
      // Disable prefetch
      //      
      OldMiscMsr = AsmReadMsr64(MSR_MISC_FEATURE_CONTROL);
      OldPatMsr  = AsmReadMsr64(MSR_IA32_CR_PAT);

      AsmWriteMsr64(MSR_MISC_FEATURE_CONTROL, OldMiscMsr | 0x000F);

			//
			// Set Page Attribute Table (PAT)defaults: 
			//	PA0 = 06h WriteBack (WB)
			//	PA1 = 04h Write Through (WT)
			//
      AsmWriteMsr64(MSR_IA32_CR_PAT, (OldPatMsr & ~(UINT64)0xFFFFFFFF) | 0x0406);            

      AsmWriteMsr64(MSR_QUIESCE_CTL1, B_QUIESCE_CTL1_QUIESCE);
			//wait till no evic pending
      while(AsmReadMsr64(MSR_QUIESCE_CTL1) & B_QUIESCE_CTL1_EVIC_PENDING);
    } else {
      Status = EFI_ALREADY_STARTED;
    }
  } else {
    MsrValue = AsmReadMsr64(MSR_QUIESCE_CTL1);
    if(MsrValue & 0x1) { //system already in Quiesce
      AsmWriteMsr64(MSR_QUIESCE_CTL1, B_QUIESCE_CTL1_UNQUIESCE); //unquiesce
      RestoreIioPcieMsg(PROGRAM_ALL);     

      //
      // Restore prefetch
      //      
      AsmWriteMsr64(MSR_MISC_FEATURE_CONTROL, OldMiscMsr);
      AsmWriteMsr64(MSR_IA32_CR_PAT, OldPatMsr);      

      // Release all APs
      mQuiesceCommData.QuiesceStatus4APs |= B_UNQUIESCED;         //communicate to APs
    } else {
      Status = EFI_NOT_STARTED;
    }
  }

  return Status;

}

/**

  Monarch set a flag to bring APs out of spin loop and enter Quiesce Lib 
	and sets quiesce command.
  
  @param	None
 
  @retval None   

**/
VOID
BringInAps(
    VOID
)
{

  EFI_STATUS    Status;
  UINT64		CpuIndex;
  UINT64		ProcessorId;
  UINT64        NumberOfAPsinSmm = 0;
  UINTN         BspIndex;

  mCpuCounter = 1;
  InitializeSpinLock (&mApLock);

  Status = mSmmCpuServiceProtocol->WhoAmI (mSmmCpuServiceProtocol, &BspIndex);
  ASSERT_EFI_ERROR (Status);

  for (CpuIndex = 0; CpuIndex < gSmst->NumberOfCpus; CpuIndex++) {

    if (CpuIndex == BspIndex) {
      continue;
    }

    Status = mSmmCpu->ReadSaveState (
                        mSmmCpu,
                        (UINTN)sizeof (UINT64),
                        EFI_SMM_SAVE_STATE_REGISTER_PROCESSOR_ID ,
                        CpuIndex,
                        &ProcessorId
                        );

    if (Status == EFI_NOT_FOUND) {
		continue;
	} else if (Status == EFI_SUCCESS) {
      NumberOfAPsinSmm++;
      gSmst->SmmStartupThisAp ((EFI_AP_PROCEDURE) QuiesceApProc, CpuIndex, (VOID *)NULL);
	} else {
		ASSERT(FALSE);
	}

  }

  while (mCpuCounter <= NumberOfAPsinSmm) {
    CpuPause ();
  }

  return; 
}

/**

  This routine resets the Quiesce flag, notifies APs to system re-config has finished, 
	updates CPU socket info and cleans up the Quiesce Table.
  
  @param QuiesceFlag Bool indicating SYSTEM_QUIESCE 1, SYSTEM_UNQUIESCE  0
 
  @retval EFI_STATUS   

**/
VOID
PostUnQuiesce(
    IN UINT64 QuiesceBase
)
{
  UINT32                    MySocketId;
  EFI_PHYSICAL_ADDRESS      *DataPtr;

  MySocketId = 0;
  DataPtr = NULL;

  mQuiesceCommData.AvailableBufferLegth = QUIESCE_DATA_SIZE - QUIESCE_STACK_SIZE - sizeof(QUIESCE_DATA_TABLE_ENTRY);
  *(UINT8 *)mQuiesceCommData.DataBaseAddress = EnumQuiesceOperationEnd;

  mQuiesceCommData.MonarchApicId = (UINT32)-1;    //init to invalid Id
  mQuiesceCommData.MonarchApCheckInStatus = FALSE;

} //end of PostUnQuiesce


/**

  This routine is main entry point for Monarch to do system Quiesce.
  It contains main flow for Monarch to Quiesce/UnQuiesce system.

  @param None

  @retval EFI_STATUS       

**/
EFI_STATUS 
QuiesceMain(
    VOID
)
{
  UINT32                      ApicId;
  UINT32                      SocketId;
  UINT64                      CodeBase;
  UINT64                      DataBase;
  EFI_PHYSICAL_ADDRESS        PageDirectoryBase;
  EFI_STATUS                  Status;

  EFI_PHYSICAL_ADDRESS        DataBufferAddr;

  UINTN                       OldPatMsr = 0;

  //  UINT64  StartTick;
  //  UINT64  EndTick;
  //  UINT64  TickDiff = 0;

  //  StartTick = GetTimerTick();


  Status = EFI_SUCCESS;
  CodeBase = 0;
  DataBase  = 0;

  if((AsmReadMsr64 (MSR_QUIESCE_CTL1) & B_QUIESCE_CTL1_QUIESCE_CAP) == 0) {
    Status = EFI_UNSUPPORTED;
    return Status;
  }

	// Inhibite TPD
  AsmWriteMsr64(MSR_QUIESCE_CTL2, 1);     
  
	//
	//wait til done
	//
  while(AsmReadMsr64 (MSR_QUIESCE_CTL2) & B_QUIESCE_CTL2_DISABLE_PSMI);
  //  POSTCODE(0xA0);  //ENTER_QUIESCEMAIN

  CodeBase       = mQuiesceCommData.CodeBaseAddress;
  DataBase        = mQuiesceCommData.DataBaseAddress;

  //Add End of Operation at end of operation
  AquireDataBuffer(&DataBufferAddr);
  *(UINT8 *)DataBufferAddr = EnumQuiesceOperationEnd;
  ReleaseDataBuffer(DataBufferAddr+sizeof(QUIESCE_DATA_TABLE_ENTRY));

  mMirrorEnabled = 0;

  ApicId = 0;

  //  POSTCODE(0xA1);  //QUIESCE_MEM_INIT

  PageDirectoryBase = mQuiesceCommData.CodeBaseAddress + QUIESCE_CACHE_DATA_SIZE;

  ApicId = GetApicID(); //get Monarch ID
  SocketId = ApicId >> mSocketLevelShift; 
  mQuiesceCommData.MonarchSocketId = SocketId;
  //write my APICID + 1 to avoid 0 value
  mQuiesceCommData.MonarchApicId = ApicId + 1;

  //
	// Clear Quiesce Status shared by Monach and Monarch AP - Runtime
	//
  *(volatile UINT32 *)mQuiesceStatusLocation = 0;  //comminucate to Monarch AP  
  mQuiesceCommData.QuiesceStatus4APs = 0;         //communicate to APs
  mQuiesceCommData.MonarchApCheckInStatus = FALSE;

  Save_Misc_Feature_Msr(&mQuiesceCommData.MiscFeatureMsrVal);

  //bring APs into QuiesceApHandler()
  //  POSTCODE(0xA2);  

      BringInAps();

  //
  // Wait til Monarch AP bring in new page table (set CR3)
  //

  //
  // STEP2 - 6, DisablePSMI, Disable Prefetch
  //wait Monarch AP setup memory attribute (wait MONARCH_AP_READY)
  DisableIioPcieMsg(PROGRAM_SELF);  

  if(mQuiesceCommData.MonarchApCheckInStatus) {
    while((*(volatile UINT32 *)mQuiesceStatusLocation & B_MONARCH_AP_READY) == 0);
  }

  // change PAT MSR to default
  OldPatMsr  = AsmReadMsr64(MSR_IA32_CR_PAT);
  AsmWriteMsr64(MSR_IA32_CR_PAT, PAT_MSR_DEFAULT);           

  //STEP 7 - 16
  //what DoQuiesce() will do:
  //a. copy Quiesce code and data to cache
  //b. Save CPU Registers 
  //c. switch to cache
  //d. run quiesce code in cache
  //e. restore CPU registers and original stack
  //f. return to main quiesce
  //ppsong: Adde QUIESCEMAIN_DOQUIESCE POST_CODE here
  //  POSTCODE(0xA4);  //READY_QUIESCE
  Status = DoQuiesce(CodeBase, 
      mQuiesceCommData.MMCfgBase[SocketId],
      mQuiesceCommData.MiscFeatureMsrVal,
      mQuiesceCommData.MonarchApCheckInStatus);

  //  POSTCODE(0xA5);  //QUIESCE_DONE
  //set UNQUIESCED flag
  *(volatile UINT32 *)mQuiesceStatusLocation |= B_UNQUIESCED;  //coomunicate to Monarch AP
  mQuiesceCommData.QuiesceStatus4APs |= B_UNQUIESCED;         //communicate to APs

  RestoreIioPcieMsg(PROGRAM_SELF);

  mDoIioProgram[SocketId] = (UINT32)-1;
  PostUnQuiesce(CodeBase);

  //restore PAT MSR
  AsmWriteMsr64(MSR_IA32_CR_PAT, OldPatMsr);            

  if((Status != EFI_SUCCESS)) { 
    Status = EFI_ACCESS_DENIED;
  }

  //  EndTick = GetTimerTick();
  //  TickDiff = EndTick - StartTick;
  //  Status = TickDiff;


  return Status;

} //end of QuiesceMain



