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



   @file QuiesceMiscFuncs.c

    Quiesce AP and other service implementation

Revision History

**/
//BKL_PORTING - disable warning 4005; some MACROS are re-defined. Remove this after clean-up
#pragma warning(disable :4005)

#include "QuiesceMain.h"
#include "Cpu/CpuRegs.h"
#include <Library/mpsyncdatalib.h>



// don't want to have lower 12bit
#define PAGE_TABLE_2M_MASK          (0xFFEFF000)

#define SYNC_TICKS  ((UINT32) ((SMM_SYNC_TIMEOUT_US) * 3.579545))

#define MSR_IA32_APIC_BASE			              0x1B
#define MSR_EXT_XAPIC_LOGICAL_APIC_ID         0x00000802

#define APIC_REGISTER_LOCAL_ID_OFFSET         0x20

#define   B_EFI_MSR_IA32_APIC_BASE_G_XAPIC      BIT11
#define   B_EFI_MSR_IA32_APIC_BASE_M_XAPIC      BIT10
#define   B_EFI_MSR_IA32_APIC_BASE_BSP          BIT8

extern UINTN                       mCpuCounter;
extern SPIN_LOCK                   mApLock;      
extern EFI_SMM_CPU_PROTOCOL        *mSmmCpu; 

extern UINT32                      *mQuiesceStatusLocation;
extern UINT32  mDoIioProgram[MAX_SOCKET];  //store thread ApicId 


//
// Function Prototype
//

/**
	This routine contains main flow for Monarch AP execution when
	Monarch reconfiguring the syste. The routine is in assembly.

	@param	CacheBase		Base address of Quiesce code in cache
	@param  MMCFGBase
	@param	MiscFeatureControlValue
	@param	SMTFlag

	@return	None
**/
VOID
Monarch_Ap_DoQuiesce( 
    IN UINT64 CacheBase,
    IN EFI_PHYSICAL_ADDRESS MMCFGBase,
    IN UINT64 MiscFeatureControlValue,
    BOOLEAN   SMTFlag
);

//
// Function Implementation
//

/** 
	This routine contains main flow for Monarch AP and other 
	APs while Monarch re-config the system.
 ***********************************/
VOID
QuiesceApHandler(
    IN EFI_QUIESCE_SUPPORT_PROTOCOL *QuiesceSupport
)
{
  UINT32                ApicId;
  UINT32                SocketId;
  UINT32                MonarchApicId;
  UINTN                 OldPatMsr;
  UINT32                *QuiesceStatus;

  OldPatMsr             = 0;
  MonarchApicId         = mQuiesceCommData.MonarchApicId - 1;

  ApicId = GetApicID();   //get my ID
  SocketId = ApicId >> mSocketLevelShift; //get my socket id


    while (!AcquireSpinLockOrFail (&mApLock));
    mCpuCounter ++;
    if(mDoIioProgram[SocketId] == (UINT32)-1) {
      mDoIioProgram[SocketId] = ApicId;
    }
    ReleaseSpinLock (&mApLock);


  // change PAT MSR to default
  OldPatMsr  = AsmReadMsr64(MSR_IA32_CR_PAT);
  AsmWriteMsr64(MSR_IA32_CR_PAT, PAT_MSR_DEFAULT);           

  //check if I am Monarch AP
  if((ApicId ^ 0x1) == MonarchApicId) { //is Monarch AP
    mQuiesceCommData.MonarchApCheckInStatus = TRUE;
    QuiesceStatus = (UINT32 *)mQuiesceStatusLocation;
    //
    //step 4 - 10: setup page table, step 5: Read 64K Quiesce code
    Monarch_Ap_DoQuiesce(mQuiesceCommData.CodeBaseAddress,
        mQuiesceCommData.MMCfgBase[SocketId],
        mQuiesceCommData.MiscFeatureMsrVal,
        1
    );

  } else {
    QuiesceStatus = (UINT32 *)&(mQuiesceCommData.QuiesceStatus4APs);
    if(mDoIioProgram[SocketId] == ApicId) { 
      DisableIioPcieMsg(PROGRAM_SELF);    //disable my IIO's MSI/VLW DCA
    }

  }

  while(!(*(volatile UINT32 *)QuiesceStatus & B_UNQUIESCED));

  //restore PAT MSR
  AsmWriteMsr64(MSR_IA32_CR_PAT, OldPatMsr);            

  return;
}

/**

    This function check number of cpu threads enter SMM

    @param None

    @retval		Count Of Processors in SMM

**/
UINT64
GetNumberOfProcessorsinSmm (
    VOID
)
{
  UINT64		CpuIndex;
  UINT64		NumberOfProcessorsinSmm = 0;
  UINT64		ProcessorId;
  EFI_STATUS    Status;

  for (CpuIndex = 0; CpuIndex < gSmst->NumberOfCpus; CpuIndex++) {

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
        NumberOfProcessorsinSmm++;
	  } else {
		  ASSERT(FALSE);
	  }
  }

  return (NumberOfProcessorsinSmm);
}


