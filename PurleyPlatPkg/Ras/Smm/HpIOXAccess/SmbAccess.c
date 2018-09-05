
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

   SmbAccess.c

Abstract:

  Platform based component for SMB Access.

--*/

#include "HpIOXAccess.h"
#include "SmbAccess.h"
#include <PchAccess.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsSmbus.h>

#pragma optimize("", off)

UINT16      SmbBase;
UINT8       SmbusStateIndex;
UINT8       FirstSmbusEntry = 1;
UINT8       Count = 0;
UINT8       BusTries = 0;
UINT8       IOSESave;
UINT8       SmbSlaveAddress;
UINT8       SmbOperation;
UINT8       HostCmd;
UINT8       *CallBuffer;

UINTN SmbusStateMachine[4] = {

  (UINTN)&AcquireSmBus,
  (UINTN)&ReadWriteSmbus,
  (UINTN)&CheckForAccessComplete,
  (UINTN)-1,

};

NEXT_STATE_ENTRY  NSSmb;


EFI_STATUS
AcquireSmBus (
  VOID
  )
{
  UINT8              StsReg;
  static UINT8       BusAcquired;

  StsReg = 0;
  BusAcquired = 0;
 
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &StsReg);

  if ( StsReg & B_PCH_SMBUS_IUS ) //Check if the SmBus host controller in use.
     goto Error;

  BusAcquired = 1;
  
  // Check for errors
  if ( StsReg & (B_PCH_SMBUS_DERR + B_PCH_SMBUS_BERR + B_PCH_SMBUS_FAIL))
      return EFI_DEVICE_ERROR;
  
  //Clear Host busy and read it back to check the previous transaction is completed.
  StsReg |= B_PCH_SMBUS_HBSY;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &StsReg);

  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &StsReg);
  if ( StsReg & B_PCH_SMBUS_HBSY )
      goto Error;

  StsReg = B_PCH_SMBUS_HSTS_ALL;
  // Clear out any odd status information
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &StsReg);
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &StsReg);
  // Inuse status bit set when read.
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &StsReg);
  Count = 0;
  return EFI_SUCCESS;

Error:
  if(Count >= MAX_RETRY)
    return EFI_DEVICE_ERROR;
  else {
    Count++;
    return IN_PROGRESS;
  }
}

 
EFI_STATUS
ReadWriteSmbus (
  VOID
)
{
  EFI_STATUS                  Status;
  BOOLEAN                     PecCheck=FALSE;
  UINT8                       AuxcReg, Temp;
  UINT8                       SmbusOperation=0;
    
  AuxcReg = 0;
  if(SmbOperation == SMB_READ) SmbSlaveAddress |= 01;

  //
  // SmbOperation Specifics (pre-execution)
  //
  Status = EFI_SUCCESS;

  switch (SmbOperation) {

      case SMB_WRITE:
          gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HD0), 1, &CallBuffer[0]);
          //
          // The "break;" command is not present here to allow code execution
          // do drop into the next case, which contains common code to this case.
          //
     case SMB_READ:
          SmbusOperation = SMBUS_V_SMB_CMD_BYTE_DATA;
          break;
  
    };

  if (PecCheck == TRUE) {
    AuxcReg |= B_PCH_SMBUS_AAC;
  }
  //
  // Set Auxiliary Control register
  //
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_AUXC), 1, &AuxcReg);

  //
  // Reset the pointer of the internal buffer
  //
  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HCTL), 1, &Temp);

  //
  // Set SMBus slave address for the device to send/receive from
  //
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_TSA), 1, &SmbSlaveAddress);

  //  
  // Set Command register
  //
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HCMD), 1, &HostCmd);
 
  //
  // Set Control Register (Initiate Operation, Interrupt disabled)
  //
  SmbusOperation += B_PCH_SMBUS_START;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HCTL), 1, &SmbusOperation);
 
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
  /*  UINTN               StallIndex;
  UINTN                   StallTries;

  //
  // TickPeriod in 100ns so convert to us
  // 
  StallTries = STALL_TIME / 10000;

  StallIndex = 0;
  while ((StallIndex < StallTries)) {
      gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, StsReg);
      if (*StsReg & (B_PCH_SMBUS_INTR | B_PCH_SMBUS_BYTE_DONE_STS | B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR)) {
          return TRUE;
      } else {
          StallIndex++;
      }
  }
  return FALSE;*/

  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, StsReg);
  if (*StsReg & (B_PCH_SMBUS_INTR | B_PCH_SMBUS_BYTE_DONE_STS | B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

EFI_STATUS
CheckForAccessComplete (
  VOID
  )
{
  UINT8            AuxStsReg;
  UINT8            Temp;
  UINT8            StsReg;

  // Wait for IO to complete
  if (!(IoDone (&StsReg))) {
    return IN_PROGRESS;
  } else if (StsReg & B_PCH_SMBUS_DERR) {
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_AUXS), 1, &AuxStsReg);
    if (AuxStsReg & B_PCH_SMBUS_CRCE) {
      return EFI_CRC_ERROR;
    } else {
      return EFI_DEVICE_ERROR;
    }
  } else if (StsReg & B_PCH_SMBUS_BERR) {
    // Clear the Bus Error for another try
    Temp = B_PCH_SMBUS_BERR;
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &Temp);
    if(BusTries >= BUS_TRIES)
      return EFI_DEVICE_ERROR;
    else {
      BusTries++;
      return IN_PROGRESS;
    }
  }

  //
  // Clear Status Registers and exit
  //
  Temp = B_PCH_SMBUS_HSTS_ALL;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HSTS), 1, &Temp);
  Temp = B_PCH_SMBUS_CRCE;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_AUXS), 1, &Temp);
  Temp = 0;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_AUXC), 1, &Temp);

  if(!(IOSESave & B_PCH_SMBUS_PCICMD_IOSE)) {

    // Restore the value back to the original state before the function gets control
    PciWrite8 (PCI_LIB_ADDRESS(
      DEFAULT_PCI_BUS_NUMBER_PCH,
      PCI_DEVICE_NUMBER_PCH_LPC,
      PCI_FUNCTION_NUMBER_PCH_SMBUS,
      R_PCH_SMBUS_PCICMD), 
      IOSESave // Enable IO space
      );
  }

  //
  // successfull completion
  // Operation Specifics (post-execution)
  //
  switch (SmbOperation) {

  case SMB_READ:
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, (SmbBase + R_PCH_SMBUS_HD0), 1, &CallBuffer[0]);
    break;
            
  default:
    break;
  };

  BusTries = 0;
  return EFI_SUCCESS;
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

  SmbSlaveAddress = SlaveAddress;
  SmbOperation    = Operation;
  HostCmd         = Command;
  CallBuffer      = Buffer;

  //
  // Get the SMBUS base Address
  //

  SmbBase = PciRead16 (PCI_LIB_ADDRESS(
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LPC,
                       PCI_FUNCTION_NUMBER_PCH_SMBUS,
                       R_PCH_SMBUS_BASE)
            );

  SmbBase &= 0xFFE0;

  IOSESave = PciRead8 (PCI_LIB_ADDRESS(
                       DEFAULT_PCI_BUS_NUMBER_PCH,
                       PCI_DEVICE_NUMBER_PCH_LPC,
                       PCI_FUNCTION_NUMBER_PCH_SMBUS,
                       R_PCH_SMBUS_PCICMD)
             );

  if(!(IOSESave & B_PCH_SMBUS_PCICMD_IOSE)) {

    PciWrite8 (PCI_LIB_ADDRESS(
               DEFAULT_PCI_BUS_NUMBER_PCH,
               PCI_DEVICE_NUMBER_PCH_LPC,
               PCI_FUNCTION_NUMBER_PCH_SMBUS,
               R_PCH_SMBUS_PCICMD),
               (IOSESave |B_PCH_SMBUS_PCICMD_IOSE) // Enable IO space
              );

  }

  if(FirstSmbusEntry)  {
    //
    // First time call initialize with the start of the SmbusStateMachine.
    //
    SmbusStateIndex = 0;
    FirstSmbusEntry = 0;
    NSSmb.NextEntryPointer = (NEXT_ENTRY_POINTER)SmbusStateMachine[SmbusStateIndex];
  }

   // Call the entry in the SMBUS state machine table
  Status  = NSSmb.NextEntryPointer();
  if(Status == IN_PROGRESS)
    return Status;

  if(EFI_ERROR(Status)) {
    FirstSmbusEntry = 1;
    return Status;
  }
   
  SmbusStateIndex++;
  // Set FirstSmbusEntry to TRUE if it is last entry.
  if((UINTN)SmbusStateMachine[SmbusStateIndex] == (UINTN)-1){
    FirstSmbusEntry = 1;
    return EFI_SUCCESS;
  } else {
    NSSmb.NextEntryPointer = (NEXT_ENTRY_POINTER)SmbusStateMachine[SmbusStateIndex];
    return IN_PROGRESS; 
  }
}



