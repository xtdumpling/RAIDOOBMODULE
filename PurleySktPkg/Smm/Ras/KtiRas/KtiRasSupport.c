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
  RasMpLinkSupport.c

Abstract:

--*/
#include <SysFunc.h>
#include "KtiRas.h"
#include "KtiRasSupport.h"


RAS_MPLINK_STATUS
SearchQuiesceBuffer (
  IN  UINT64        Address,
  IN  UINT32        *Data32
  )
{
  QUIESCE_DATA_TABLE_ENTRY      *Ptr;

  Ptr = mNextEntry - 1;
  while (Ptr >= (QUIESCE_DATA_TABLE_ENTRY *)mQuiesceBuf) {
    if (Ptr->CsrAddress == Address) {
      *Data32 = Ptr->DataMask.Data;
      return RAS_MPLINK_SUCCESS;
    }
    Ptr--;
  }

  return RAS_MPLINK_FAILURE_GENERAL;
}

VOID
RasMpLinkClearKtiInfoBuffer (
  VOID
)
{
  MemSetLocal ((UINT8 *) mRasHost, 0, sizeof(SYSHOST));
  MemSetLocal ((UINT8 *) mRasSocketData, 0, sizeof(KTI_SOCKET_DATA));
  MemSetLocal ((UINT8 *) mRasSocketDataOrig, 0, sizeof(KTI_SOCKET_DATA));  
  MemSetLocal ((UINT8 *) mKtiInternalGlobal, 0, sizeof(KTI_HOST_INTERNAL_GLOBAL));

  return;
}

VOID
RasMpLinkClearCpuShadowBuffer (
  IN UINT8        StartCpu,
  IN UINT8        EndCpu
)
{
  UINT8   SocId;

  for (SocId = StartCpu; SocId <= EndCpu; ++SocId) {
    MemSetLocal ((UINT8 *) (mCpuShadowFlag + SocId * MAX_FLAG_BUF_PER_CPU), 0, MAX_FLAG_BUF_PER_CPU);      
  }

  return;
}

RAS_MPLINK_STATUS
RasMpLinkClearShadowCpuBufferInSteps (
  VOID
)
{
  STATIC UINT8    ClearBufType = 0;
  RAS_MPLINK_STATUS   Status = RAS_MPLINK_SUCCESS;

  switch (ClearBufType) {
    case 0:
      RasMpLinkClearCpuShadowBuffer(0, 1); // CPU 0, 1
      ClearBufType++; // Next two CPU sockets
      Status = RAS_MPLINK_PENDING;
      break;

    case 1:
      RasMpLinkClearCpuShadowBuffer(2, 3); // CPU 2, 3
      if (MAX_SOCKET <= 4) {
        ClearBufType = 0; // Done clearing
        Status = RAS_MPLINK_SUCCESS;
      } else {
        ClearBufType++; // Next two CPU sockets
        Status = RAS_MPLINK_PENDING;
      }
      break;

    case 2:
      RasMpLinkClearCpuShadowBuffer(4, 5); // CPU 4, 5
      ClearBufType++; // Next two CPU sockets
      Status = RAS_MPLINK_PENDING;
      break;

    case 3:
      RasMpLinkClearCpuShadowBuffer(6, 7); // CPU 6, 7
      ClearBufType = 0; // Done clearing
      Status = RAS_MPLINK_SUCCESS;
      break;

    default:
      ClearBufType = 0; // Done clearing
      Status = RAS_MPLINK_SUCCESS;
  }

  return Status;
}


VOID
RasMpLinkClearBuffer (
  VOID
)
{
  RasMpLinkClearKtiInfoBuffer ();
  while (RasMpLinkClearShadowCpuBufferInSteps() != RAS_MPLINK_SUCCESS);

  return;
}

VOID
RasMpLinkClearSocketData (
  VOID
)
{
  UINT8             Idx1;
  KTI_SOCKET_DATA  TempRasSocketData;

  MemSetLocal ((UINT8 *) &TempRasSocketData, 0, sizeof(TempRasSocketData));

  for (Idx1 = 0; Idx1 < MAX_SOCKET; ++Idx1) {
    TempRasSocketData.Cpu[Idx1].Valid = mRasSocketData->Cpu[Idx1].Valid;
    TempRasSocketData.Cpu[Idx1].SocId = mRasSocketData->Cpu[Idx1].SocId;
    MemCopy ((UINT8*)(UINTN)TempRasSocketData.Cpu[Idx1].LinkData, (UINT8*)(UINTN)(mRasSocketData->Cpu[Idx1].LinkData), sizeof(TempRasSocketData.Cpu[Idx1].LinkData));
  }

  MemCopy((VOID *)mRasSocketData, (VOID *)&TempRasSocketData, sizeof(KTI_SOCKET_DATA));

  return;
}

VOID
RasMpLinkAllocateBuffer (
  VOID
)
{
  EFI_STATUS                Status;

  // Buffer to store the KTI info produced when O*L event is in progress
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof(SYSHOST), &mRasHost);
  ASSERT_EFI_ERROR (Status);

  // Buffer to store the KTI related socket data gathered when O*L event is in progress
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof(KTI_SOCKET_DATA), &mRasSocketData);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof(KTI_SOCKET_DATA), &mRasSocketDataOrig);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, CPU_SHADOW_BUF_SIZE, &mCpuShadowBuf);
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, CPU_SHADOW_FLAG_SIZE, &mCpuShadowFlag);
  ASSERT_EFI_ERROR (Status);

  // Buffer for Host KTI internal global
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof(KTI_HOST_INTERNAL_GLOBAL), &mKtiInternalGlobal);
  ASSERT_EFI_ERROR (Status);

  // Clear all the internal buffers
  RasMpLinkClearBuffer ();

  return;   
}

#ifdef RAS_KTI_MEASURE_TIME

UINT32             mMsIndex;

VOID
GetExpiredTimeEntry (
  IN UINT32             MsIndex
  )
{
  UINT64_STRUCT count;
  ReadTsc64 (&count);
  mRasMpLink->Host->var.common.startTsc = count;
  mMsIndex = MsIndex;

  return;
}

VOID
GetExpiredTimeEntryPeriod (
  )
{
  
  UINT8              Temp;
  UINT64_STRUCT endTsc    = {0, 0};
  UINT32              Latency = 0;

  ReadTsc64 (&endTsc);
  Latency = TimeDiff (mRasMpLink->Host, mRasMpLink->Host->var.common.startTsc, endTsc, TDIFF_UNIT_US);
  Temp = mRasMpLink->Host->setup.kti.DebugPrintLevel;
  mRasMpLink->Host->setup.kti.DebugPrintLevel = 0xF;
 
  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n    MileStone: %02u   Time Period from last entry (uSecs):  %u \n",
             mMsIndex, Latency));

  mRasMpLink->Host->setup.kti.DebugPrintLevel = Temp;

  return;
}

VOID
GetExpiredTimeExit (
  )
{
  
  UINT8              Temp;
  UINT64_STRUCT endTsc    = {0, 0};
  UINT32              Latency = 0;

  ReadTsc64 (&endTsc);
  Latency = TimeDiff (mRasMpLink->Host, mRasMpLink->Host->var.common.startTsc, endTsc, TDIFF_UNIT_US);
  Temp = mRasMpLink->Host->setup.kti.DebugPrintLevel;
  mRasMpLink->Host->setup.kti.DebugPrintLevel = 0xF;
 
  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n    MileStone: %02u   Elapsed Time (uSecs): %u \n",
             mMsIndex, Latency));

  mRasMpLink->Host->setup.kti.DebugPrintLevel = Temp;

  return;
}
#endif

#ifdef RAS_KTI_DEBUG
VOID
CsrWriteCheckPoint (
  VOID
  )
{
  UINT8 Size = 4;
  mCsrWriteCtr++;
  if (mCsrWriteCtr % 16 == 0) {
    mNextEntry->Operation = EnumQuiesceWrite;
    mNextEntry->AccessWidth = 4;
    mNextEntry->CsrAddress = (UINT64) GetCpuPciCfgAddress (mRasHost, mSbspId, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG, &Size);
    mNextEntry->AndMask = 0;
    mNextEntry->DataMask.OrMask = mCsrWriteCtr;
    mNextEntry++;
    mCsrWriteCtr++;
  }
}

VOID
MarkEndOfCsrWrite (
  VOID
  )
{
  UINT8 Size = 4;
  mNextEntry->Operation = EnumQuiesceWrite;
  mNextEntry->AccessWidth = 4;
  mNextEntry->CsrAddress = (UINT64) GetCpuPciCfgAddress (mRasHost, mSbspId, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG, &Size);
  mNextEntry->AndMask = 0;
  mNextEntry->DataMask.OrMask = (0xE0F0 << 16) | mCsrWriteCtr;
  mNextEntry++;
  return;
}

VOID
DumpQuiesceBuffer (
  VOID
  )
{
  QUIESCE_DATA_TABLE_ENTRY     *TempPtr;

  if (mQuieseBufStart == NULL || mNextEntry == NULL) return;

  for (TempPtr = mQuieseBufStart; TempPtr < mNextEntry; ++TempPtr) {
    if ((TempPtr->Operation != EnumQuiesceWrite) || (TempPtr->AndMask!= 0) || 
        (TempPtr->AccessWidth != 1 && TempPtr->AccessWidth != 2 && TempPtr->AccessWidth != 4)) {
      ASSERT(FALSE);
    }
#ifdef RAS_KTI_DEBUG_REG_DETAIL
    KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n    WRITE    Addr: 0x%08X    Data:0x%08X    Size:%u    TempPtr/mQuieseBuf:0x%08X",
        (UINT32)TempPtr->CsrAddress, TempPtr->DataMask.OrMask, TempPtr->AccessWidth, TempPtr));
#endif
  }

  return;
}
#endif // RAS_KTI_DEBUG

#ifdef RAS_KTI_UNIT_TEST

EFI_SMM_SW_REGISTER_CONTEXT   KtiRasSwSmiContext;

EFI_STATUS
KtiRasDebugHandler (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                         *CommBuffer,     OPTIONAL
  IN OUT   UINTN                        *CommBufferSize  OPTIONAL
  )
{
  UINT32                Data32;
  UINT8                 Ctr1;
  INT8                  EventType = RAS_MPLINK_EVNT_RESET_SM;
  INT8                  SocketType = RAS_MPLINK_SOCKET_TYPE_NA;
  INT8                  SocketId = RAS_MPLINK_SOCKET_ID_NA;
  //INT8                  Size =4;
  RAS_MPLINK_STATUS         Status;

  //
  // Bits [7:0] - O*L event for CPU x. Only one bit can be set.
  // Bit [8]: 1 - Offline Event. 0 - Online Event
  //
  mDirectCommit = TRUE;

  Data32 = 0x02;   // online Socket 1 for unit test
  WriteCpuPciCfgRas(mRasHost, mSbspId, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG, Data32);

  Data32 = ReadCpuPciCfgRas(mRasHost, mSbspId, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG);

  //Data32 = *(volatile UINT32 *) ( GetCpuPciCfgAddress (mRasHost, mSbspId, 0, BIOSNONSTICKYSCRATCHPAD10_UBOX_MISC_REG, &Size));
  Data32 = Data32 & 0x1FF;

  if (Data32 & 0xFF) {
    EventType = (Data32 & 0x100) ? RAS_MPLINK_EVNT_OFFLINE: RAS_MPLINK_EVNT_ONLINE;
    for (Ctr1 = 0; Ctr1 < MAX_SOCKET; ++Ctr1) {
      if (Data32 & (1 << Ctr1)) { // CPU x needs to be on/offlined?
        SocketType = RAS_MPLINK_SOCKET_TYPE_CPU;
        SocketId = Ctr1;
        KtiDebugPrintInfo0((mRasHost, KTI_DEBUG_INFO0, "\n    CPU %u %s event ", Ctr1, (EventType == RAS_MPLINK_EVNT_OFFLINE) ? "OFFLINE": "ONLINE"));
        break;
      }
    }
  }

  if (EventType != RAS_MPLINK_EVNT_RESET_SM) {
    while ((Status = RasMpLinkCheckEvent (EventType, SocketId)) == RAS_MPLINK_PENDING);
    if (Status != RAS_MPLINK_SUCCESS) {
      KtiDebugPrintInfo0((mRasHost, KTI_DEBUG_INFO0, "\n\n    CSI RAS can not on/offline!!!\n "));
      CpuDeadLoop();
    }

    Status = RasMpLinkHandlerNonSliced(EventType, SocketId);
    if (Status != RAS_MPLINK_SUCCESS && Status != RAS_MPLINK_INT_DOMAIN_PENDING) {
      KtiDebugPrintInfo0((mRasHost, KTI_DEBUG_INFO0, "\n\n    CSI RAS handling failed!!!\n "));
      CpuDeadLoop();
      RasMpLinkHandlerNonSliced(RAS_MPLINK_EVNT_RESET_SM, RAS_MPLINK_SOCKET_INFO_NA);
    } else if (Status == RAS_MPLINK_INT_DOMAIN_PENDING) {
      Status = RasMpLinkHandlerNonSliced(EventType, SocketId);
      if (Status != RAS_MPLINK_SUCCESS) {
        KtiDebugPrintInfo0((mRasHost, KTI_DEBUG_INFO0, "\n\n    CSI RAS INT Domain init failed!!!\n "));
        CpuDeadLoop();
        RasMpLinkHandlerNonSliced(RAS_MPLINK_EVNT_RESET_SM, RAS_MPLINK_SOCKET_INFO_NA);
      }
    }
  }

  return EFI_SUCCESS;
}

VOID
RegisterSwSmi (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE         TempHandle;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SwDispatch;    

  Status = gBS->LocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
  ASSERT_EFI_ERROR (Status);
 
  KtiRasSwSmiContext.SwSmiInputValue = 0x76;
  //KtiRasSwSmiContext.Type  = 0x76;
  //KtiRasSwSmiContext.Phase = KtiRasDebugHandler;

  Status = SwDispatch->Register (SwDispatch, KtiRasDebugHandler, &KtiRasSwSmiContext, &TempHandle);
  ASSERT_EFI_ERROR (Status);

  return;
}
#endif  //RAS_KTI_UNIT_TEST

#if   0              //back up for HpIoxAccess modolue

void
DelayFn(
  UINT32 dCount
  )
{
  UINT8    Reference;
  UINT8    Current;

  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, 0, 0x61, 1, &Reference);
  Reference &= 0x10;

  while(dCount) {
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, 0, 0x61, 1, &Current);
    Current &= 0x10;
    if (Reference != Current) {
      Reference = Current;
      dCount--;
    }
  }
}
 
EFI_STATUS
AcquireBus (
  VOID
  )
{
  UINT8           StsReg;
  UINT8           Count=0x10;
  
  StsReg = 0;

  // Update SMMBASE every time since it may change.  
  SmbBase = PciRead16 (PCI_LIB_ADDRESS(
                       PCI_BUS_NUMBER_ICH,
                       PCI_DEVICE_NUMBER_ICH_LPC,
                       PCI_FUNCTION_NUMBER_ICH_SMBUS,
                       R_ICH_SMBUS_BASE)
            );

  SmbBase &= 0xFFE0;
  
  // wait til in use =0
  while(Count !=0)  {

    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &StsReg);

    if (!(StsReg & SMBUS_B_IUS))  break;
    DelayFn(10);
    Count--;
  }
  
  Count = 0x10;
  while(Count !=0)  {
    // wrtie to clear HOST_BUSY bit
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &StsReg);
    StsReg = StsReg | SMBUS_B_HBSY;
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &StsReg);
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &StsReg);

    if (!(StsReg & SMBUS_B_HBSY))  break;

    DelayFn(10);
    Count--;
  }

  if (StsReg & SMBUS_B_HBSY) return EFI_TIMEOUT;

  // Check for errors
  if (StsReg & (SMBUS_B_DERR + SMBUS_B_BERR + SMBUS_B_FAIL)) {
      return EFI_DEVICE_ERROR;
  } 

  StsReg = SMBUS_B_HSTS_ALL;
  // Clear out any odd status information
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &StsReg);
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &StsReg);
  // Inuse status bit set when read.
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &StsReg);

  return EFI_SUCCESS;

}


BOOLEAN
IoDone (
  IN      UINT8           *StsReg
  )
/*++

Routine Description:

  This function provides a standard way to check if an SMBus transaction has
  completed.

Arguments:

  StsReg:   Not used for input.

Returns:

  StsReg:   On return, contains the value of the SMBus status register.
  Returns TRUE if transaction is complete, FALSE otherwise.

--*/
{
  // Wait for IO to complete
  UINTN                   StallIndex;
  UINTN                   StallTries;

  //
  // TickPeriod in 100ns so convert to us
  // 
  StallTries = STALL_TIME / 10000;

  StallIndex = 0;
  while ((StallIndex < StallTries)) {
      gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, StsReg);
      if (*StsReg & (SMBUS_B_INTR | SMBUS_B_BYTE_DONE_STS | SMBUS_B_DERR | SMBUS_B_BERR)) {
          return TRUE;
      } else {
          MicroSecondDelay(10000);
          StallIndex++;
      }
  }

  return FALSE;
}

EFI_STATUS
SmbusReadWrite (
  IN      UINT8    SlaveAddress,
  IN      UINT8    Operation,
  IN      UINT8    Command,
  IN OUT  VOID     *Buffer
  )
/*++

Routine Description:

  This function provides a standard way to execute Smbus protocols
  as defined in the SMBus Specification.
  
Arguments:

  Slave Address: Smbus Slave device the command is directed at
  Operation: Which SMBus protocol will be used
    
Returns:

  EFI_SUCCESS           The operation completed successfully.
    Length              Contain the actual number of bytes read/written.
    Buffer              Contain the data read/written.
  EFI_UNSUPPORTED 
  EFI_INVALID_PARAMETER Length or Buffer is NULL for any operation besides
                        quick read or quick write.
  EFI_TIMEOUT           The transaction did not complete within an internally
                        specified timeout period, or the controller is not
                        available for use.
  EFI_DEVICE_ERROR      There was an Smbus error (NACK) during the operation.
                        This could indicate the slave device is not present
                        or is in a hung condition.

--*/
{
  EFI_STATUS                  Status;
  UINT8                       AuxcReg, Temp;
  UINT8                       AuxStsReg;
  UINT8                       SmbusOperation=0;
  UINT8                       StsReg;
  UINT8                       HostCmdReg;
  UINTN                       BusIndex;
  UINT8                       *CallBuffer;
  BOOLEAN                     PecCheck=FALSE;
  UINT8                       IOSESave;

  IOSESave = PciRead8 (PCI_LIB_ADDRESS(
                       PCI_BUS_NUMBER_ICH,
                       PCI_DEVICE_NUMBER_ICH_LPC,
                       PCI_FUNCTION_NUMBER_ICH_SMBUS,
                       R_ICH_SMBUS_PCICMD)
             );

  if(!(IOSESave & B_ICH_SMBUS_PCICMD_IOSE)) {
    PciWrite8 (PCI_LIB_ADDRESS(
               PCI_BUS_NUMBER_ICH,
               PCI_DEVICE_NUMBER_ICH_LPC,
               PCI_FUNCTION_NUMBER_ICH_SMBUS,
               R_ICH_SMBUS_PCICMD),
               (IOSESave |B_ICH_SMBUS_PCICMD_IOSE) // Enable IO space
              );
  }

  CallBuffer = Buffer;
  HostCmdReg = Command;
  AuxcReg = 0;
  //
  // See if its ok to use the bus based upon INUSE_STS bit.
  //
  Status = AcquireBus ();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if(Operation == SMB_READ) SlaveAddress |= 01;

  //
  // This is the main operation loop.  If the operation results in a Smbus
  // collision with another master on the bus, it attempts the requested
  // transaction again at least BUS_TRIES attempts.
  //
  for (BusIndex = 0; BusIndex < BUS_TRIES; BusIndex++) {

    //
    // Operation Specifics (pre-execution)
    //
    Status = EFI_SUCCESS;

    switch (Operation) {

        case SMB_WRITE:
            gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HD0), 1, &CallBuffer[0]);
            //
            // The "break;" command is not present here to allow code execution
            // do drop into the next case, which contains common code to this case.
            //

        case SMB_READ:
            SmbusOperation = SMBUS_V_SMB_CMD_BYTE_DATA;
            break;
    
        default:
            Status = EFI_INVALID_PARAMETER;
            break;
    };

    if (EFI_ERROR(Status)) {
      break;
    }

    if (PecCheck == TRUE) {
      AuxcReg |= SMBUS_B_AAC;
    }
    //
    // Set Auxiliary Control register
    //
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_AUXC), 1, &AuxcReg);

    //
    // Reset the pointer of the internal buffer
    //
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HCTL), 1, &Temp);

    //
    // Set SMBus slave address for the device to send/receive from
    //
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_TSA), 1, &SlaveAddress);

    //  
    // Set Command register
    //
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HCMD), 1, &HostCmdReg);

    //
    // Set Control Register (Initiate Operation, Interrupt disabled)
    //
    SmbusOperation += SMBUS_B_START;
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HCTL), 1, &SmbusOperation);

    // Wait for IO to complete
    if (!(IoDone (&StsReg))) {
        Status = EFI_TIMEOUT;
        break;
    } else if (StsReg & SMBUS_B_DERR) {
        gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_AUXS), 1, &AuxStsReg);
        if (AuxStsReg & SMBUS_B_CRCE) {
            Status = EFI_CRC_ERROR;
        } else {
            Status = EFI_DEVICE_ERROR;
        }
        break;
    } else if (StsReg & SMBUS_B_BERR) {
        // Clear the Bus Error for another try
        Status = EFI_DEVICE_ERROR;
        Temp = SMBUS_B_BERR;
        gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &Temp);
        continue;
    }

    //
    // successfull completion
    // Operation Specifics (post-execution)
    //
    switch (Operation) {

        case SMB_READ:
            gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HD0), 1, &CallBuffer[0]);
            break;
            
        default:
            break;
    };

    if ((StsReg & SMBUS_B_BERR) && (Status != EFI_BUFFER_TOO_SMALL)) {
        // Clear the Bus Error for another try
        Status = EFI_DEVICE_ERROR;
        Temp = SMBUS_B_BERR;
        gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &Temp);
        continue;
    } else {
        break;
    }
  }

  //
  // Clear Status Registers and exit
  //
  Temp = SMBUS_B_HSTS_ALL;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_HSTS), 1, &Temp);
  Temp = SMBUS_B_CRCE;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_AUXS), 1, &Temp);
  Temp = 0;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + SMBUS_R_AUXC), 1, &Temp);

  // Restore the value back to the original state before the function gets control
  if(!(IOSESave & B_ICH_SMBUS_PCICMD_IOSE)) {
    // Restore the value back to the original state before the function gets control
    PciWrite8 (PCI_LIB_ADDRESS(
      PCI_BUS_NUMBER_ICH,
      PCI_DEVICE_NUMBER_ICH_LPC,
      PCI_FUNCTION_NUMBER_ICH_SMBUS,
      R_ICH_SMBUS_PCICMD), 
      IOSESave // Enable IO space
      );
  }

  return Status;
}

RAS_MPLINK_STATUS
IssuePowerOnOffCommand (				// ???check later, this function should be in HpIOXAccess driver
  IN UINT8          EventType,
  IN INT32          SocketType,
  IN INT32          SocketId
  )
{
  UINT8       DevAddress;
  UINT8       Operation;
  UINT8       Command;
  UINT8       PowerOnData;

  KtiDebugPrintInfo0((mRasMpLink->Host, KTI_DEBUG_INFO0, "\n    Issuing CPU Power %s Command to PLD logic", (EventType == RAS_MPLINK_EVNT_ONLINE) ? "ON" : "OFF"));

  DevAddress  = 0x4A;
  Operation   = SMB_WRITE;
  Command     = 0x2;
  if (EventType == RAS_MPLINK_EVNT_ONLINE) {
    PowerOnData = (UINT8)(0x10 |(SocketId << 5));       // online command
  } else {  
    PowerOnData = (UINT8)(SocketId << 5);               // offline command 
  }
  SmbusReadWrite(DevAddress, Operation, Command, &PowerOnData);
  DelayFn(100000);

  return RAS_MPLINK_SUCCESS;
}

#endif

