/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2009 - 2014 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license.  Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:
    SmbusRecovery.c

Abstract:
    This is an implementation for the Smbus Recovery

-----------------------------------------------------------------------------*/


#include "SmbusRecovery.h"


EFI_SMM_PERIODIC_TIMER_DISPATCH2_PROTOCOL     *mSmmPeriodicTimerDispatch = NULL;
EFI_CPU_CSR_ACCESS_PROTOCOL                   *mCpuCsrAccess = NULL;
EFI_HANDLE                                    mSemaphorePeriodicTimerHandle = NULL;
EFI_HANDLE                                    mRecoveryPeriodicTimerHandle = NULL;
UINT32                                        mSocketPresentBitMap;    
SMBUS_RECOVERY_DATA_STRUCT                    mSmBusRecoveryData[MAX_SOCKET * MAX_SMBUS_SEGMENT];
BOOLEAN                                       mSmbusRecoveryInProcess = FALSE;
UINT8                                         mSktBeingWorkedOn = 0xFF;
UINT8                                         mStateMachineState = 0xFF;
BOOLEAN                                       mBootTimeSmbusRecovery = FALSE;
UINT8                                         mSemaphorePollCount = 0;
IIO_UDS                                       *mIioUdsData;


SMBUS_REGISTER_ADDRS  mSmbRegAddrs[MAX_SMBUS_SEGMENT] = {
  {SMB_CMD_CFG_0_PCU_FUN5_REG,
   SMB_STATUS_CFG_0_PCU_FUN5_REG,
   SMB_PERIOD_CFG_0_PCU_FUN5_REG 
#if  MAX_SMBUS_SEGMENT > 1
  },
  {SMB_CMD_CFG_1_PCU_FUN5_REG,
   SMB_STATUS_CFG_1_PCU_FUN5_REG,
   SMB_PERIOD_CFG_1_PCU_FUN5_REG
#endif // MAX_SMBUS_SEGMENT > 1
  }};


EFI_STATUS
EFIAPI
InitializeSmbusRecovery (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description: 

  Entry point of the Smbus Error Recovery module. 

Arguments:  

  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table        

Returns:  

  EFI_SUCCESS:              Driver initialized successfully
  EFI_LOAD_ERROR:           Failed to Initialize or has been loaded 
  EFI_OUT_OF_RESOURCES:     Could not allocate needed resources

--*/
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;
  BOOLEAN                           InSmm;
  EFI_SMM_BASE2_PROTOCOL            *SmmBase = NULL;
  EFI_SMM_SYSTEM_TABLE2             *Smst = NULL;
  EFI_IIO_UDS_PROTOCOL              *IioUds;

  //
  // Retrieve SMM Base Protocol
  //
  Status = gBS->LocateProtocol (
          &gEfiSmmBase2ProtocolGuid,
          NULL,
          &SmmBase
          );
  ASSERT_EFI_ERROR (Status);

  //
  // Check to see if we are already in SMM
  //
  SmmBase->InSmm (SmmBase, &InSmm);

  if (InSmm) {
    Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &IioUds);
    ASSERT_EFI_ERROR (Status);
    mIioUdsData = (IIO_UDS *)IioUds->IioUdsPtr;    

    if (mIioUdsData->SystemStatus.SmbusErrorRecovery != 1) {
      DEBUG((DEBUG_ERROR, "\nSMBUS Error Recovery is not enabled in setup\n"));
      return EFI_SUCCESS;
    }

    //
    //Locate the required protocols to be used for registering SMM handler and also in runtime
    //
    SmmBase->GetSmstLocation (SmmBase, &Smst);

    Status = Smst->SmmLocateProtocol (&gEfiSmmPeriodicTimerDispatch2ProtocolGuid, NULL, (VOID **)&mSmmPeriodicTimerDispatch);
    ASSERT_EFI_ERROR (Status);

    Status = Smst->SmmLocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
    ASSERT_EFI_ERROR (Status);

    mSocketPresentBitMap = mIioUdsData->SystemStatus.socketPresentBitMap;
    
    DEBUG((DEBUG_ERROR, "\nIn SMBUS Recovery Module EntryPoint\n"));

    //
    // Check for Smbus Errors
    //
    CheckForSmbusErrors();

    //
    // Look for SMBUS errors, and if any, recover the SMBUS
    //
    if (mSmbusRecoveryInProcess) {
      mBootTimeSmbusRecovery = TRUE;
      ExecuteSmbusRecoveryStateMachine ();
      if (mSktBeingWorkedOn == 0xFF) {
        mSmbusRecoveryInProcess = FALSE;
        mBootTimeSmbusRecovery = FALSE;
      }
    }

    //
    //Configure the SMBUS to generate SMI when an error happens on SMBUS
    //
    ConfigureSmbusForErrorRecovery ();

    //
    // Register the SMI handler to recover the SMBUS from error
    //
    Status = Smst->SmiHandlerRegister (SmbusRecoverySmiHandler, NULL, &Handle);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}


VOID
CheckForSmbusErrors (
  )
/*++

Routine Description: 

 This function will check if the error is happened on SMBUS, if so update the Attribute field that
 ERROR_RECOVERY is needed
 
Arguments:  
  NONE

Returns:  
 NONE

--*/
{
  SMB_STATUS_CFG_0_PCU_FUN5_STRUCT              SmbStat;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT                 SmbCmd;
  UINT32                                        SmbStatReg;
  UINT32                                        SmbCmdReg;
  UINT8                                         Index, SktId, SegIndex;

  if (mSmbusRecoveryInProcess)
    return;

  for(SktId = 0; SktId < MAX_SOCKET; SktId++ ) {
   if (mSocketPresentBitMap & (1 << SktId)) {
    for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {
      
      if (!(mIioUdsData->SystemStatus.imcEnabled[SktId][SegIndex]))
        continue;

      SmbCmdReg = mSmbRegAddrs[SegIndex].smbCmdReg;
      SmbStatReg = mSmbRegAddrs[SegIndex].smbStatReg;

      Index = (SktId * 2) + SegIndex; //(0 .. 7)
      SmbStat.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbStatReg);
      SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbCmdReg);

      if ((SmbStat.Bits.smb_sbe == 1) || (SmbCmd.Bits.smb_ckovrd == 0) ) {
        DEBUG((DEBUG_ERROR, "\nSmbus Error present on SktId 0x%x, SegmentId 0x%x", SktId, SegIndex));
        mSmBusRecoveryData[Index].Bits.Attribute |= ERROR_RECOVERY_NEEDED;
        mSmbusRecoveryInProcess = TRUE;
        mStateMachineState = FirstTimeEntry;
      }
    } //segIndex
   }// mSocketPresentBitMap
  }//Skt
}


EFI_STATUS
SmbusRecoverySmiHandler 
(
  IN  EFI_HANDLE    DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommunicationBuffer,
  IN OUT UINTN      *SourceSize
)
/*++

Routine Description: 

 This function is the entrypoint of the Smbus recovery SMM handler.  We get to this function for any 
 SMI happens in the system.  Here, We check if there are any errors in the  SMBUS and if so try to recover the
 SMBUS 
 
Arguments:  

SmmImageHandle          A unique value returned by the SMM infrastructure
DispatchContext         The pointer to the dispatch function's context.
CommunicationBuffer     An optional buffer that will be populated by the SMM infrastrcuture
SourceSize              If CommunicationBuffer is non-NULL, this field
                        indicates the size of the data payload in this buffer.

Returns:  

EFI_SUCCESS

**/
{

  CheckForSmbusErrors();
 
  if (mSmbusRecoveryInProcess)  {
    DEBUG((DEBUG_ERROR, "\nExecuting State Machine to recover SMBUS\n"));
    ExecuteSmbusRecoveryStateMachine ();

    if (mSktBeingWorkedOn == 0xFF) {
      mSmbusRecoveryInProcess = FALSE;
    }
       
  }

  return EFI_SUCCESS;
}

VOID
ExecuteSmbusRecoveryStateMachine (
  VOID
) 
/*++

Routine Description: 

 This function is the entrypoint of the Smbus recovery SMM handler.  We get to this function for any 
 SMI happens in the system.  Here, We check if there are any errors in the  SMBUS and if so try to recover the
 SMBUS.

 The recovery process is implemented as a state machine.  After the POST, for the recovery, a semaphore (per socket) has to be
 acquired, and the agents are NOT allowed to acquire semaphores from multiple sockets at a time. 
 
Arguments:  
  NONE

Returns:
  NONE
**/
{
  UINT8                                     Index, SktId, SegIndex;
  SYSTEMSEMAPHORE0_UBOX_MISC_STRUCT         SystemSemaphore0;
  EFI_SMM_PERIODIC_TIMER_REGISTER_CONTEXT   PeriodicTimer;
  EFI_STATUS                                Status;
  BOOLEAN                                   SearchBack = FALSE;

  SktId = 0;
  while (SktId < MAX_SOCKET) {
  
    //  Skip the Socket that is not present
    if (!(mSocketPresentBitMap & (1 << SktId))) {
      SktId++;
      continue;
    }
      

    //
    // If the state machine hasn't found the socket, go to the first entry where we will 
    // find the socket to be worked on.  If we know the socket that is being worked on, go to that
    // socket directly
    //
    if (mSktBeingWorkedOn == 0xFF) {
      mStateMachineState = FirstTimeEntry;
    } else if (SktId != mSktBeingWorkedOn) {

      for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {
        Index = (SktId * 2) + SegIndex;
        if (mSmBusRecoveryData[Index].Bits.Attribute & ERROR_RECOVERY_NEEDED) {
          if (SktId < mSktBeingWorkedOn) {
            SearchBack = TRUE;
          }
        }
      }
      SktId++;
      continue;
    }


    //
    // First entry in the Statemachine, where will go through the attribute field in the mSmbusRecoveryDataStructure
    // to find the socket that needs to worked on
    //
    if (mStateMachineState == FirstTimeEntry) {
      DEBUG((DEBUG_ERROR, "\n\nStatemachine State is FirstTimeEntry"));

      for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {
        Index = (SktId * 2) + SegIndex;
        if (mSmBusRecoveryData[Index].Bits.Attribute & ERROR_RECOVERY_NEEDED) {
          
          mSktBeingWorkedOn = SktId;
          
          //
          // during boot time, there is no need to acquire semaphore
          //
          if (mBootTimeSmbusRecovery) {
            mStateMachineState = PrepareSmbusRecovery;
          } else {
            mStateMachineState = AcquireSemaphore;
          }

          DEBUG((DEBUG_ERROR, "\n\nThe socket being worked on is 0x%x", mSktBeingWorkedOn));
        }
      }
    }


    //
    // Second state:  AcquireSemaphore.  Here we will try to get the semaphore for access the
    // SMBUS.  If we don't get it right away, we will trigger the Periodic SMI Handler for polling
    // the register till the timeout (5 sec) and then override the semaphore
    //

    if (mStateMachineState == AcquireSemaphore) {
      DEBUG((DEBUG_ERROR, "\n\nStatemachine State is in AcquireSemaphore"));

      SystemSemaphore0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG);
      SystemSemaphore0.Bits.bus_aqu_release = 1;
      SystemSemaphore0.Bits.requesternode = 0;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG, SystemSemaphore0.Data);

      SystemSemaphore0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG);

      if ((SystemSemaphore0.Bits.bus_aqu_release == 1) && (SystemSemaphore0.Bits.currentnode == 0)) {
        mStateMachineState = PrepareSmbusRecovery;
      }
      else {
        // Trigger Periodic SMI Handler
        if(!mSemaphorePeriodicTimerHandle) {
          PeriodicTimer.Period = 50000000;  
          PeriodicTimer.SmiTickInterval = 640000;  
          Status = mSmmPeriodicTimerDispatch->Register(mSmmPeriodicTimerDispatch, SemaphoreAcquireCallBack, &PeriodicTimer, &mSemaphorePeriodicTimerHandle); 
          ASSERT_EFI_ERROR (Status);
        }
        mStateMachineState = SemaphoreAcquireInProgress;
        return;
      }
    }


    //
    // Third State: SemaphoreAcquireInProgress: This is where we come back while waiting for the semaphore
    // Go to next state once BIOS gets the semaphore
    //
    
    if (mStateMachineState == SemaphoreAcquireInProgress) {
      DEBUG((DEBUG_ERROR, "\n\nStatemachine State is in SemaphoreAcquireInProgress"));
      SystemSemaphore0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG);
      if ((SystemSemaphore0.Bits.bus_aqu_release == 1) && (SystemSemaphore0.Bits.currentnode == 0)) {
        mStateMachineState = PrepareSmbusRecovery;
        DEBUG((DEBUG_ERROR, "\n\nSemaphore Acquired"));
      } else {
        return;
      }
    }


    //
    // Fourth State:  Prepare for SmbusRecovery and trigger the SMBUS recovery process.
    // and also wait for the SMBUS Recovery to Complete
    //

    if (mStateMachineState == PrepareSmbusRecovery) {
      DEBUG((DEBUG_ERROR, "\n\nStatemachine State is in PrepareSmbusRecovery"));
      Status = InitiateSmbusRecovery();
      if (Status == EFI_SUCCESS) {
        if (mBootTimeSmbusRecovery) {
          DEBUG((DEBUG_ERROR, "\nwaiting after triggering the recovery"));
          MicroSecondDelay (KNL_SMBUS_RECOVERY_TIME_35_MS * 1000);
        } else {

          if (!mRecoveryPeriodicTimerHandle) {
            PeriodicTimer.Period = KNL_SMBUS_RECOVERY_TIME_35_MS * 1000 * 10;  // to convert to 100 nano seconds
            PeriodicTimer.SmiTickInterval = 160000;  
            Status = mSmmPeriodicTimerDispatch->Register(mSmmPeriodicTimerDispatch, SmbusRecoveryCallback, &PeriodicTimer, &mRecoveryPeriodicTimerHandle); 
            ASSERT_EFI_ERROR (Status);
            DEBUG((DEBUG_ERROR, "\nRegistered periodic handler"));
          }
        }
        mStateMachineState = IsSmbusRecoveryComplete;
        return;
       }
    }


    //
    // Fifth State:  IsSmbusRecoveryComplete,  In this state we check if the SMBUS is recovered,
    // restore the previous state, and release the semaphore
    //
    if (mStateMachineState == IsSmbusRecoveryComplete) {
      DEBUG((DEBUG_ERROR, "\n\nStatemachine State is in IsSmbusRecoveryComplete"));
      Status = CheckSmbusRecoveryComplete ();

      if (Status == EFI_SUCCESS) {
         RestoreStateAndEndSmbusRecovery();

        if (!(mBootTimeSmbusRecovery)) {
          ReleaseSemaphore ();
        }

        //
        // Need to update the global variables to let statemachine find other sockets with smbus errors
        //
        mSktBeingWorkedOn = 0xFF;
        mStateMachineState = FirstTimeEntry;

      } else {
        return;
      }
    }
    
    if (SearchBack == TRUE) {
      SktId = 0;
    } else {
      SktId++;
    }
  } // Socket
}


EFI_STATUS
SemaphoreAcquireCallBack (
  IN  EFI_HANDLE     Handle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  )

/*++

Routine Description: 

  This the routine attached to the periodic SMI handler to check if the BIOS gets the semaphore.
  If the timeout happens,  BIOS will override the semaphore.

  If BIOS gets the semaphore, unregister the periodic handler


Arguments:

  No Arguments

Returns:  

 EFI_SUCCESS : complete executing the function
 
--*/
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  SYSTEMSEMAPHORE0_UBOX_MISC_STRUCT     SystemSemaphore0;
  UINT8                                 SktId;

  SktId = mSktBeingWorkedOn;  

  mSemaphorePollCount++;
 
  //
  // Count is reached, override the seamphore
  //
  if (mSemaphorePollCount == 5) {
    SystemSemaphore0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG);
    if (!((SystemSemaphore0.Bits.bus_aqu_release == 1) && (SystemSemaphore0.Bits.currentnode == 0))) {
      SystemSemaphore0.Bits.bus_aqu_release = 0;
      SystemSemaphore0.Bits.requesternode = SystemSemaphore0.Bits.currentnode;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG, SystemSemaphore0.Data);


      // Now, do the acquire semaphore again
      SystemSemaphore0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG);
      SystemSemaphore0.Bits.bus_aqu_release = 1;
      SystemSemaphore0.Bits.requesternode = 0;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG, SystemSemaphore0.Data);
    }
  }
  
  SystemSemaphore0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG);

  //
  // If semaphore Acquired, reset the poll count and unregister the handler
  //
  if ((SystemSemaphore0.Bits.bus_aqu_release == 1) && (SystemSemaphore0.Bits.currentnode == 0)) {

    Status = mSmmPeriodicTimerDispatch->UnRegister(mSmmPeriodicTimerDispatch, mSemaphorePeriodicTimerHandle);
    ASSERT_EFI_ERROR (Status);

    mSemaphorePeriodicTimerHandle  = NULL;
    mSemaphorePollCount = 0;
  }

  return Status;
}


EFI_STATUS
InitiateSmbusRecovery(
  VOID
)
/*++

Routine Description: 
  This function will save some SMBUS Features, disable TSOD and SMBUS Recovery Enabled and 
  activate the recovery process by setting sft_rst = 1.  Hardware will set sft_rst to '0' once the 
  recovery is done 
 
Arguments:  
  NONE

Returns:
  NONE
**/

{
  UINT8   SegIndex, Index, SktId;
  SMB_STATUS_CFG_0_PCU_FUN5_STRUCT              SmbStat;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT                 SmbCmd;
  UINT32                                        SmbStatReg;
  UINT32                                        SmbCmdReg;
  EFI_STATUS                                    Status;

  Status = EFI_SUCCESS;
  SktId = mSktBeingWorkedOn;
  DEBUG((DEBUG_ERROR, "\nInitiate SmbusRecovery on sktId 0x%x", SktId));
  for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {
    Index = (SktId * 2) + SegIndex;

    if (mSmBusRecoveryData[Index].Bits.Attribute & ERROR_RECOVERY_NEEDED) {

      DEBUG((DEBUG_ERROR, "\nInitiate SmbusRecovery on sktId 0x%x, SegIndex:0x%x, Index:0x%x", SktId, SegIndex, Index));

      SmbCmdReg = mSmbRegAddrs[SegIndex].smbCmdReg;
      SmbStatReg = mSmbRegAddrs[SegIndex].smbStatReg;

      SmbStat.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbStatReg);
      SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbCmdReg);

      //
      // Save Smbus features and disable TSOD polling
      //
      mSmBusRecoveryData[Index].Bits.TsodSA = (UINT8)SmbStat.Bits.tsod_sa;
      mSmBusRecoveryData[Index].Bits.TsodPollEn = (UINT8)SmbCmd.Bits.smb_tsod_poll_en;
      mSmBusRecoveryData[Index].Bits.SmbSbeEn = (UINT8)SmbCmd.Bits.smb_sbe_en;
        
      //
      // Disable TSOD polling and Smbus recvoery enable, so that error recovery can be done
      //
      SmbCmd.Bits.smb_tsod_poll_en = 0;
      SmbCmd.Bits.smb_sbe_en = 0;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, (UINT32)SmbCmdReg, (UINT32)SmbCmd.Data);
  
      //
      // Activate the Smbus recovery process by setting smb_soft_rst to 1 and smb_ckovrd to 0
      // When the recovery is complete, h/w will set smb_ckovrd to '1'
      // and it is BIOS responsibility to deactive smb_soft_rst (which is done is later steps)
      //
      SmbCmd.Bits.smb_ckovrd = 0;
      SmbCmd.Bits.smb_soft_rst = 1;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, (UINT32)SmbCmdReg, (UINT32)SmbCmd.Data);
      SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbCmdReg);

      DEBUG((DEBUG_ERROR, "\nTriggered SmbusRecovery on sktId 0x%x, SegIndex:0x%x, Index:0x%x", SktId, SegIndex, Index));
      DEBUG((DEBUG_ERROR, "\nThe value of SmbCmd.Bits.smb_ckovrd is 0x%x", SmbCmd.Bits.smb_ckovrd));

      Status = EFI_SUCCESS;
      
    }

  }

  return Status;
}


EFI_STATUS
SmbusRecoveryCallback (
  IN  EFI_HANDLE     Handle,
  IN  CONST VOID     *mPeriodicTimer,
  IN  OUT VOID      *CommBuffer  OPTIONAL,
  IN  OUT UINTN     *CommBufferSize OPTIONAL
  )

  /*++

Routine Description: 

  This is the callback routine registered to PeriodicSMI handler.
  This routine will check if the SMBus recovery process is complete. 
  If so, will end the recovery process and unregister the periodic handler.
 

Arguments:

  No Arguments

Returns:  

 Success  after the SMBUS Recovery is triggered
 
--*/
{
  
  UINT8                             Index,SegIndex, SktId;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT     SmbCmd;
  EFI_STATUS                        Status = EFI_SUCCESS;

  SktId = mSktBeingWorkedOn;
  DEBUG((DEBUG_ERROR, "\nIn SmbusRecoveryCallback"));

  if (mStateMachineState == IsSmbusRecoveryComplete) {

  for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {
    Index = (SktId * 2) + SegIndex;

    if (mSmBusRecoveryData[Index].Bits.Attribute & ERROR_RECOVERY_NEEDED) {

      Status = 0xFF;

      SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)mSmbRegAddrs[SegIndex].smbCmdReg); 

      DEBUG((DEBUG_ERROR, "\nChecking Smbus Recovery on sktId 0x%x, SegIndex:0x%x, Index:0x%x", SktId, SegIndex, Index));
      DEBUG((DEBUG_ERROR, "\nThe value of SmbCmd.Bits.smb_ckovrd is 0x%x", SmbCmd.Bits.smb_ckovrd));

      //
      // h/w will set smb_ckovrd to '1' to indicate that the recovery is complete on the SMBUS
      //
      if (SmbCmd.Bits.smb_ckovrd == 1){
        mSmBusRecoveryData[Index].Bits.Attribute |= ERROR_RECOVERY_COMPLETED;
        Status = EFI_SUCCESS;
      }
    }
  }
  } else {
    Status = mSmmPeriodicTimerDispatch->UnRegister(mSmmPeriodicTimerDispatch, mRecoveryPeriodicTimerHandle);
    mRecoveryPeriodicTimerHandle  = NULL;
    ASSERT_EFI_ERROR (Status);
    DEBUG((DEBUG_ERROR, "\nUnregister periodic handler"));
  }


return EFI_SUCCESS;

}

EFI_STATUS
CheckSmbusRecoveryComplete(
  VOID
)
/*++

Routine Description: 

  This Function will check if the SMBUS is recovered by the h/w or not. 
   

Arguments:  

  None

Returns:  

  EFI_SUCCESS:SMBUS recovery completed successfully


--*/
{
  UINT8                             Index,SegIndex, SktId;
  EFI_STATUS                        Status = 0xFF;

  SktId = mSktBeingWorkedOn;

  DEBUG((DEBUG_ERROR, "\n\nChecking if recovery is complete or not"));

  for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {
    Index = (SktId * 2) + SegIndex; //(0 .. 7)

    if (mSmBusRecoveryData[Index].Bits.Attribute & ERROR_RECOVERY_NEEDED) {

      Status = 0xFF;

      if (mSmBusRecoveryData[Index].Bits.Attribute & ERROR_RECOVERY_COMPLETED){
        DEBUG((DEBUG_ERROR, "\n\nRecovery is completed on 0x%x, mc0x%x", SktId, SegIndex));
       
        Status = EFI_SUCCESS;
      }
    }
  } 
  return Status;
}



VOID
RestoreStateAndEndSmbusRecovery(
  VOID
)
/*++

Routine Description: 

  This function will restore the state that is saved before initiating the SMBUS error recovery.
  And also end the recovery process.
 
Arguments:  

  NONE

Returns:  

  NONE

--*/
{
  SMB_STATUS_CFG_0_PCU_FUN5_STRUCT        SmbStat;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT           SmbCmd;
  UINT32                                  SmbCmdReg;
  UINT32                                  SmbStatReg;
  UINT8                                   Count = 0;
  UINT8                                   Index, SktId, SegIndex;

  SktId = mSktBeingWorkedOn;

  for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {

    Index = (SktId * 2) + SegIndex; //(0 .. 7)

    if (mSmBusRecoveryData[Index].Bits.Attribute & ERROR_RECOVERY_COMPLETED) {
      SmbCmdReg = mSmbRegAddrs[SegIndex].smbCmdReg;
      SmbStatReg = mSmbRegAddrs[SegIndex].smbStatReg;

      //
      // Deactivate the Soft Reset
      //
      SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbCmdReg);
      SmbCmd.Bits.smb_soft_rst = 0;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, (UINT32)SmbCmdReg, (UINT32)SmbCmd.Data);

      //
      // To clear SMB_SBE, initate a SMBUS trasaction for TSOD temperature read to a populated DIMM
      // (H/W might provide a bit to do this, when that happens then code to do the SMBUS transaction can be removed
      //
      SmbCmd.Bits.smb_dti = DTI_TSOD;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, (UINT32)SmbCmdReg, (UINT32)SmbCmd.Data);

      SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbCmdReg);
      SmbCmd.Bits.smb_ba = 0x05;
      SmbCmd.Bits.smb_sa = mSmBusRecoveryData[Index].Bits.TsodSA;
      SmbCmd.Bits.smb_wrt = 0; //smb_wrt_cmd and smb_wrt_pntr has been combined in SKX
      SmbCmd.Bits.smb_cmd_trigger = 1;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, (UINT32)SmbCmdReg, (UINT32)SmbCmd.Data);

      // wait for the Smbus transaction to complete
      Count = 0;
      do {
        Count ++;
        MicroSecondDelay(5);
        SmbStat.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbStatReg);
        DEBUG((DEBUG_ERROR, "\nSetting Smb_sbe  on sktId 0x%x, SegIndex:0x%x, Index:0x%x", SktId, SegIndex, Index));
        DEBUG((DEBUG_ERROR, "\nThe value of SmbStat.Bits.smb_sbe is 0x%x", SmbStat.Bits.smb_sbe));

        if (Count >= 2)
          break;
      } while (SmbStat.Bits.smb_sbe);

      //
      // Restore the state of the SMBUS that is saved before doing the recocvery
      //
      SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbCmdReg);
      SmbCmd.Bits.smb_tsod_poll_en = mSmBusRecoveryData[Index].Bits.TsodPollEn;
      SmbCmd.Bits.smb_sbe_en = mSmBusRecoveryData[Index].Bits.SmbSbeEn;
      mCpuCsrAccess->WriteCpuCsr(SktId, 0, (UINT32)SmbCmdReg, (UINT32)SmbCmd.Data);

      mSmBusRecoveryData[Index].Bits.Attribute = 0;
    }
  } // SegIndex
}


VOID
ReleaseSemaphore (
  VOID
)
/*++

Routine Description: 

  This function will relese the semaphore after the recovery process is done

Arguments:  

  NONE

Returns:  

  NONE

--*/
{

  UINT8                                     SktId;
  SYSTEMSEMAPHORE0_UBOX_MISC_STRUCT        SystemSemaphore0;

  SktId = mSktBeingWorkedOn;
  SystemSemaphore0.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG);
  SystemSemaphore0.Bits.bus_aqu_release = 0;
  SystemSemaphore0.Bits.requesternode = 0;
  mCpuCsrAccess->WriteCpuCsr(SktId, 0, SYSTEMSEMAPHORE0_UBOX_MISC_REG, SystemSemaphore0.Data);
}

VOID
ConfigureSmbusForErrorRecovery(
VOID
)
/*++

Routine Description: 

  This function will configure SMBUS on all controllers to generate SMI when error happens on SMBUS
 
Arguments:

  None

Returns:  

  None

--*/
{
  SMB_STATUS_CFG_0_PCU_FUN5_STRUCT       SmbStat;
  SMB_CMD_CFG_0_PCU_FUN5_STRUCT             SmbCmd;
  UINT8                            Index, SktId, SegIndex;
  UINT32                            SmbStatReg;
  UINT32                            SmbCmdReg;

  
  for(SktId = 0; SktId < MAX_SOCKET; SktId++ ){
    if (mSocketPresentBitMap & (1 << SktId)){
      for (SegIndex = 0; SegIndex < MAX_SMBUS_SEGMENT; SegIndex++) {

        Index = (SktId * 2) + SegIndex; //(0 .. 7)

        SmbCmdReg = mSmbRegAddrs[SegIndex].smbCmdReg;
        SmbStatReg = mSmbRegAddrs[SegIndex].smbStatReg;

        SmbStat.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbStatReg);
        SmbCmd.Data = mCpuCsrAccess->ReadCpuCsr(SktId, 0, (UINT32)SmbCmdReg);

        if ((SmbStat.Bits.smb_sbe == 0) && (SmbCmd.Bits.smb_ckovrd == 1) ) {

          //
          // This bit will make the system generate SMI when there is an error on SMBUS
          //
          SmbCmd.Bits.smb_sbe_smi_en = 1;

          //
          // This bit will enable the error recovery and stops TSOD pollilng
          //(other wise, the error gets cleared in the next SMBus transaction)
          //
          SmbCmd.Bits.smb_sbe_en = 1;

          mCpuCsrAccess->WriteCpuCsr(SktId, 0, (UINT32)SmbCmdReg, (UINT32) SmbCmd.Data);
          DEBUG((DEBUG_ERROR, "\nConfigured SmbusErrorRecovery for Skt:0x%x, Mc:0x%x\n", SktId, SegIndex));
        }

        mSmBusRecoveryData[Index].Bits.Attribute = 0;
      }
    }
  }//SktId
}



