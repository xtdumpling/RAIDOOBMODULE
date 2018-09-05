/**++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

Copyright (c) 2009-2015 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

    @file MeErrorInj.c

    This file contaisn the functionality for the ME Error Injection.

---------------------------------------------------------------------------**/

#include "WheaErrorInj.h"

extern EFI_CPU_CSR_ACCESS_PROTOCOL                    *mCpuCsrAccess;
extern BOOLEAN                                        mMESegInjEn;
extern BOOLEAN                                        mMESegIcvErrEn;
extern UINT32                                         mMESegInjAddr;
extern EFI_SMM_SYSTEM_TABLE2                          *mSmst;
extern EFI_RAS_SYSTEM_TOPOLOGY                        *mRasTopology;
extern WHEA_EINJ_TRIGGER_ACTION_TABLE                 *mEinjAction;
extern WHEA_EINJ_PARAM_BUFFER                         *mEinjParam;
extern EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL      *mSmmPeriodicTimerDispatch;
extern EFI_HANDLE                                     mPeriodicTimerHandle;
extern UINT64                                         mUmaErrorInjectionType;

/**
  Set MC9_CTL[bit 5] to 0 before ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PreMC9CtlSetupForMEErrInj (
  VOID
  )
{
  UINT64      Data;

  Data = AsmReadMsr64 ((EFI_IA32_MC0_CTL + (9*4)));
  Data &= (~BIT5);
  AsmWriteMsr64 ((EFI_IA32_MC0_CTL + (9*4)), Data);
}

/**
  Set MC9_CTL[bit 5] to 1 after ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PostMC9CtlSetupForMEErrInj (
  VOID
  )
{
  UINT64      Data;

  Data = AsmReadMsr64 ((EFI_IA32_MC0_CTL + (9*4)));
  Data |= BIT5;
  AsmWriteMsr64 ((EFI_IA32_MC0_CTL + (9*4)), Data);
}

/**
  Do the pre-setup for ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PreMESegErrInjSetup (
  VOID
  )
{
  UINT8                             Skt,Core;
  UINTN                             CpuNumber;
  MESEG_LIMIT_N0_CHABC_SAD_STRUCT   MeMemLimit;

  // Step 1:  set MC9[Bit5] = 0
  for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
    if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid != TRUE) continue;

    for (Core = 0; Core < MAX_CORE; Core++) {
      if (mRasTopology->SystemInfo.SocketInfo[Skt].CpuInfo[Core][0].Valid) {
        if (mRasTopology->SystemInfo.SocketInfo[Skt].CpuInfo[0][0].BspFlag != 0) {
          PreMC9CtlSetupForMEErrInj ();
        } else {
          CpuNumber = mRasTopology->SystemInfo.SocketInfo[Skt].CpuInfo[0][0].CpuNum;
          mSmst->SmmStartupThisAp ((EFI_AP_PROCEDURE)PreMC9CtlSetupForMEErrInj,
                CpuNumber,
                (VOID *)NULL
               );
        }
        break;      
      }
    }
  }
    
  // Step 2: MESEG_LIMIT.EN = 0
  for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
    if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid != TRUE) 
      continue;
      
    MeMemLimit.Data = mCpuCsrAccess->ReadCpuCsr (Skt, 0, MESEG_LIMIT_N0_CHABC_SAD_REG);
    MeMemLimit.Bits.en = 0;  
    mCpuCsrAccess->WriteCpuCsr (Skt, 0, MESEG_LIMIT_N0_CHABC_SAD_REG, MeMemLimit.Data);
  }
}

/**
  Do the post-setup for ME Error Injection
  
  @param NONE

  @retval NONE
**/
VOID
PostMESegErrInjSetup (
  VOID
  )
{
  UINT8                             Skt,Core;
  UINTN                             CpuNumber;
  MESEG_LIMIT_N0_CHABC_SAD_STRUCT   MeMemLimit;

  // Step 1: MESEG_LIMIT.EN = 1
  for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
    if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid != TRUE) 
      continue;
      
    MeMemLimit.Data = mCpuCsrAccess->ReadCpuCsr (Skt, 0, MESEG_LIMIT_N0_CHABC_SAD_REG);
    MeMemLimit.Bits.en = 1;  
    mCpuCsrAccess->WriteCpuCsr (Skt, 0, MESEG_LIMIT_N0_CHABC_SAD_REG, MeMemLimit.Data);
  }
  
  // Step 2:  Set MC9[Bit5] to 1
  for (Skt = 0; Skt < MAX_SOCKET; Skt++) {
    if (mRasTopology->SystemInfo.SocketInfo[Skt].Valid != TRUE) 
      continue;

    for (Core = 0; Core < MAX_CORE; Core++) {
      if (mRasTopology->SystemInfo.SocketInfo[Skt].CpuInfo[Core][0].Valid) {
        if (mRasTopology->SystemInfo.SocketInfo[Skt].CpuInfo[0][0].BspFlag != 0) {
          PostMC9CtlSetupForMEErrInj ();
        } else {
          CpuNumber = mRasTopology->SystemInfo.SocketInfo[Skt].CpuInfo[0][0].CpuNum;
          mSmst->SmmStartupThisAp ((EFI_AP_PROCEDURE)PostMC9CtlSetupForMEErrInj,
                CpuNumber,
                (VOID *)NULL
               );
        }
        break;      
      }
    }
  }

}

/**
  Corrupt the data the ME Seg Injection address offset
  
  @param NONE

  @retval NONE
**/
VOID
CorruptMESegInjOffset ( 
  VOID
)
{
  UINT32 Data32;

  Data32  = *( UINT32 *)(UINTN)(mMESegInjAddr);
  Data32 = Data32 ^ ((UINT32)-1);
  *( UINT32 *)(UINTN)(mMESegInjAddr) = Data32;
  return;
}

/**
 Set up Trigger Action Table for ME Seg ERROr Injection
  
  @param NONE

  @retval NONE
**/
VOID
SetupMESegTriggerActionTable (
  VOID
  )
{
  mEinjAction->Trigger0.Operation                  = WHEA_EINJ_TRIGGER_ERROR_ACTION; 
  mEinjAction->Trigger0.Instruction                = INSTRUCTION_WRITE_REGISTER_VALUE; 
  mEinjAction->Trigger0.Flags                      = FLAG_NOTHING; 
  mEinjAction->Trigger0.Reserved8                  = 00; 
  mEinjAction->Trigger0.Register.AddressSpaceId    = EFI_ACPI_3_0_SYSTEM_IO; 
  mEinjAction->Trigger0.Register.RegisterBitWidth  = 0x08; 
  mEinjAction->Trigger0.Register.RegisterBitOffset = 0x00; 
  mEinjAction->Trigger0.Register.AccessSize        = EFI_ACPI_3_0_BYTE; 
  mEinjAction->Trigger0.Register.Address           = 0xB2; 
  mEinjAction->Trigger0.Value                      = EFI_WHEA_ME_EINJ_CONSUME_SWSMI;
  mEinjAction->Trigger0.Mask                       = 0xffffffff;

  return;

}

/**

  Call ME interface to get a page offset for ME Error Injection
  
  @param NONE

  @retval EFI_SUCCESS:              ME Page allocated Successfully
  @retval EFI_OUT_OF_RESOURCES:     ME Page cannot be allocated
**/
EFI_STATUS
GetMEPageForErrorInj (
  VOID
  ) 
{
#if SPS_SUPPORT
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32       MeUmaBase;
  
  MeUmaBase     = MmioRead32 (MmPciBase(0, 5, 0) + R_MESEG_BASE);

  Status = MeUmaEinjPageOffset (&mMESegInjAddr);
  mMESegInjAddr = MeUmaBase + mMESegInjAddr;
  DEBUG ((DEBUG_ERROR, "The address of the ME PAGE  is 0x%lx\n", mMESegInjAddr));

  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "ME Error Inj: CANNOT get the ME Page offset\n"));
  }
#else
  EFI_STATUS Status = EFI_UNSUPPORTED;
#endif // SPS_SUPPORT

  return Status;

}

/**
  Periodic SMI handler to get the status from ME Seg Error consumption.
  ME will provide the status in around 5 ms after the error consumption is initiated

  @param Handle             - The unique handle assigned to this handler by SmiHandlerRegister().Type
                                            EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.1 Specification.
  @param mPeriodicTimer     -  Pointer to the DispatchContext

  @param CommBuffer         - A pointer to a collection of data in memory that will be conveyed from a non-SMM
                                           environment into an SMM environment. The buffer must be contiguous, physically mapped, and be a physical address.
  @param CommBufferSize     - The size of the CommBuffer. 

  @retval EFI_SUCCESS       - The interrupt was handled and quiesced. No other handlers should still be called.

**/


EFI_STATUS
GetMeEInjConsumeStatus (
  IN  EFI_HANDLE     Handle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
 
  //
  // Place holder for the Get Status call from ME for Error consumption
  // Replace with the correct call after ME Provided the support
  //

  //
  // There is an error loading HeciSmm3 protocol, which is causing the WheaErrorInj driver not loading
  // So, not including the MESEG code below in the build.  This will be enabled once the HeciSmm3 protocol issue is fixed
  //
  //Status = MeUmaEinjConsume (MeEinjModeDeffered);
  
  mEinjParam->OpStatus = 0;
  
  if (Status == EFI_SUCCESS){
    mEinjParam->CmdStatus = WHEA_EINJ_CMD_SUCCESS;
  } else {
    mEinjParam->CmdStatus = WHEA_EINJ_CMD_UNKNOWN_FAILURE;
  }

  // Unregister periodic SMI handler
  Status = mSmmPeriodicTimerDispatch->UnRegister(mSmmPeriodicTimerDispatch, mPeriodicTimerHandle);
  mPeriodicTimerHandle  = NULL;
  ASSERT_EFI_ERROR (Status);
  
  return Status;

}


/**

    SW SMI handler registered to call the ME libray to initiate the Error consumption

    @param DispatchHandle     - The unique handle assigned to this handler by SmiHandlerRegister().Type
                                            EFI_HANDLE is defined in InstallProtocolInterface() in the UEFI 2.1 Specification.
    @param DispatchContext   - Points to the optional handler context which was specified when the handler was registered.
    @param CommBuffer         - A pointer to a collection of data in memory that will be conveyed from a non-SMM
                                           environment into an SMM environment. The buffer must be contiguous, physically mapped, and be a physical address.
    @param CommBufferSize    - The size of the CommBuffer. 

    @retval EFI_SUCCESS - The interrupt was handled and quiesced. No other handlers should still be called.
    
**/

EFI_STATUS
WheaMeEinjSwSmiHanlder (
  IN  EFI_HANDLE                          DispatchHandle,
  IN  CONST EFI_SMM_SW_REGISTER_CONTEXT   *DispatchContext, OPTIONAL
  IN OUT VOID                             *CommBuffer,     OPTIONAL
  IN OUT UINTN                            *CommBufferSize  OPTIONAL
  )
{
#if SPS_SUPPORT
  EFI_STATUS                                Status = EFI_SUCCESS;
  UINT8                                     MeEinjMode;
  MeEinjMode = 0;
  //EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT   PeriodicTimer;

  DEBUG ((DEBUG_ERROR, " In MeSeg Error Injection SWSMI Handler\n"));

  if (mUmaErrorInjectionType & (WHEA_MESEG_COR_ERR_INJ_CONSUME_NOW | WHEA_MESEG_COR_ERR_INJ_CONSUME_1MS
    | WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_NOW | WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_1MS 
    | WHEA_MESEG_ICV_ERR_INJ_CONSUME_NOW | WHEA_MESEG_ICV_ERR_INJ_CONSUME_1MS)) {
      MeEinjMode = MeEinjModeInstant;
  }
  else if (mUmaErrorInjectionType & (WHEA_MESEG_COR_ERR_INJ_CONSUME_G0_S1_RW | WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_G0_S1_RW
            | WHEA_MESEG_ICV_ERR_INJ_CONSUME_G0_S1_RW)) {
     MeEinjMode = MeEinjModeGoS1RW;
  }
  else if (mUmaErrorInjectionType & (WHEA_MESEG_COR_ERR_INJ_CONSUME_RESET_WARN | WHEA_MESEG_UNCOR_ERR_INJ_CONSUME_RESET_WARN
            | WHEA_MESEG_ICV_ERR_INJ_CONSUME_RESET_WARN)) {
    MeEinjMode = MeEinjModeOnReset;
  }
  
  DEBUG ((DEBUG_ERROR, " MeEinjMode = 0x%x\n",MeEinjMode));

  Status = MeUmaEinjConsume (MeEinjMode);

  // Need to enable the following code when we have a mechanism to get the status from ME about the error injection
  
  // register Periodic SMI handler to get the status from the ME
  /*
  if(!mPeriodicTimerHandle) {
    PeriodicTimer.Period = 5 * 1000 * 10;  // to convert to 100 nano seconds
    PeriodicTimer.SmiTickInterval = 15000;
    Status = mSmmPeriodicTimerDispatch->Register(mSmmPeriodicTimerDispatch, GetMeEInjConsumeStatus, &PeriodicTimer, &mPeriodicTimerHandle); 
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((DEBUG_ERROR, "Register the periodic timer\n"));

  mEinjParam->OpStatus = WHEA_EINJ_OPERATION_BUSY;
  */
#else
  EFI_STATUS Status = EFI_UNSUPPORTED;
#endif // SPS_SUPPORT

  return Status;

}

