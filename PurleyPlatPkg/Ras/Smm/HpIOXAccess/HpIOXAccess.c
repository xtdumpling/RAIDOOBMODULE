/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c) 2008 - 2016, Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

   HpIOXAccess.c

Abstract:

  Platform based component for IOX PLD Access.

--*/

#include "HpIOXAccess.h"

#pragma optimize("",off)

BIOS_ACPI_PARAM                *mAcpiParameter;
EFI_HP_IOX_ACCESS_PROTOCOL     *mHpIoxAccessProtocol;
EFI_CPU_CSR_ACCESS_PROTOCOL    *mCpuCsrAccess;
EFI_IIO_UDS_PROTOCOL           *mIioUds;
IIO_UDS                        *mIioUdsData;

SMBUS_VALUE mHpIoxValues[] = {{0x4E,0,0}, {0x4E,1,0},
                              {0xFF,0xFF,0xFF}};  // End of table

UINT8        mSlaveAddress[] = {0x4E, 0xFF}; 

//
// Driver entry point
//

EFI_STATUS
EFIAPI
HpIOXAccessStart (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  EFI_HANDLE                     Handle = NULL;
  EFI_GLOBAL_NVS_AREA_PROTOCOL   *AcpiNvsProtocol = NULL; 
  EFI_RAS_SYSTEM_TOPOLOGY               *RasTopology;
  SYSTEM_RAS_SETUP                      *RasSetup = NULL;
  EFI_PLATFORM_RAS_POLICY_PROTOCOL      *PlatformRasPolicyProtocol;


  Status = gSmst->SmmLocateProtocol (&gEfiPlatformRasPolicyProtocolGuid, NULL, &PlatformRasPolicyProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiIioUdsProtocolGuid, NULL, &mIioUds);
  ASSERT_EFI_ERROR (Status);
  mIioUdsData = (IIO_UDS *)mIioUds->IioUdsPtr;
  
  RasTopology = PlatformRasPolicyProtocol->EfiRasSystemTopology;
  RasSetup = PlatformRasPolicyProtocol->SystemRasSetup;

  if (RasSetup->Qpi.QpiCpuSktHotPlugEn != 1) {
    DEBUG ((DEBUG_INFO, "CPU Hot plug is not enabled. Exiting without doing HpIox Initialization\n"));
    return Status;
  }

  switch (RasTopology->SystemInfo.BoardId) {
    case TypeLightningRidgeEXRP:
    case TypeLightningRidgeEXECB1:
    case TypeLightningRidgeEXECB2:
    case TypeLightningRidgeEXECB3:
    case TypeLightningRidgeEXECB4:
    case TypeLightningRidgeEX8S1N:
    case TypeLightningRidgeEX8S2N:
      DEBUG ((DEBUG_INFO, "The Board identified as LightningRidge. Do HpIox Initialization\n"));
      break;

    default:
      DEBUG ((DEBUG_INFO, "The Board is not LightningRidge. Exiting without doing HpIox Initialization\n"));
      return Status;
  }


  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, &AcpiNvsProtocol);
  ASSERT_EFI_ERROR (Status);
  mAcpiParameter = AcpiNvsProtocol->Area;
  ASSERT (mAcpiParameter);

  Status = gBS->LocateProtocol (&gEfiCpuCsrAccessGuid, NULL, &mCpuCsrAccess);
  ASSERT_EFI_ERROR (Status);

  // Allocate the buffer for protocol data structure and install HP IOX ACCESS protocol
  mHpIoxAccessProtocol = AllocateZeroPool (sizeof (EFI_HP_IOX_ACCESS_PROTOCOL));
  if (mHpIoxAccessProtocol == NULL) {
    DEBUG ((EFI_D_ERROR, "Error!! HpIOXAccessStart () - AllocatePool failed!\n"));
    ASSERT (mHpIoxAccessProtocol != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  mHpIoxAccessProtocol->ReadIoxAndUpdateHpRequest = ReadIoxAndUpdateHpRequest;
  mHpIoxAccessProtocol->IssuePldCmdThroIox        = IssuePldCmdThroIox;
  mHpIoxAccessProtocol->ClearAttnLatch            = ClearAttnLatch;
  mHpIoxAccessProtocol->ClearInterrupt            = ClearInterrupt;
  mHpIoxAccessProtocol->HpSmbusReadWrite          = HpSmbusReadWrite;
  mHpIoxAccessProtocol->CheckOnlineOfflineRequest = CheckOnlineOfflineRequest;

  Status = gSmst->SmmInstallProtocolInterface (&Handle, &gEfiHpIoxAccessGuid, EFI_NATIVE_INTERFACE, mHpIoxAccessProtocol);
  ASSERT_EFI_ERROR (Status);

  // Init HP IOX  
  Status  = InitializationHPIOX();
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
InitializationHPIOX(
  VOID
  )
{

  UINT8        Index = 0;
  UINT8        Operation;
  UINT8        Command;
  UINT8        Buffer;
  EFI_STATUS   Status;
  
  Status      = EFI_SUCCESS;

  // Initialize the Mem, IOH and CPU HP IOX.
  while( mSlaveAddress[Index] != 0xFF )  {

    Buffer = 0xFF;
    Operation = SMB_WRITE;

    Command = 06;
    if (mSlaveAddress[Index] == 0x4E) {
      Buffer = 0xF0;
    }

    do{
      Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
    }while(Status == IN_PROGRESS);
    ASSERT_EFI_ERROR (Status);

    Command = 07;
    if (mSlaveAddress[Index] == 0x4E){
      Buffer = 0xFF;
    }

    do {     //Wait in loop until data is written
      Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
    }while(Status == IN_PROGRESS);
    ASSERT_EFI_ERROR (Status);
  
    Index++;
  }

  // Clear the interrupt
  ClearInterrupt(FALSE);
  
  return Status;
}


EFI_STATUS
ReadIoxAndUpdateHpRequest(
  IN UINT8*     HpRequest,
  IN BOOLEAN    TimeSliced
  )
{
  EFI_STATUS   Status;
  UINT8        Operation;
  STATIC UINT8 IoxIndex = 0;

  Operation   = SMB_READ;

  // Read the IOXs based on the array of structures ( mHpIoxValues) and fill the corresponding SmbusValues
  while(mHpIoxValues[IoxIndex].Addr != 0xFF) {
   if(!TimeSliced) {
     do {
       Status = SmbusReadWrite( mHpIoxValues[IoxIndex].Addr, Operation, mHpIoxValues[IoxIndex].PortNumber, &(mHpIoxValues[IoxIndex].Value)); 
     } while(Status == IN_PROGRESS);
   } else {
     Status = SmbusReadWrite( mHpIoxValues[IoxIndex].Addr, Operation, mHpIoxValues[IoxIndex].PortNumber, &(mHpIoxValues[IoxIndex].Value)); 
     if( Status == IN_PROGRESS)
      return IN_PROGRESS;
   }
    ASSERT_EFI_ERROR(Status);
    IoxIndex++;
  }
  IoxIndex = 0;

  UpdateHpRequest(HpRequest);

  return EFI_SUCCESS;
}

VOID
UpdateHpRequest(
  IN UINT8*     HpRequest
  )
/*
This runtime routine will get the Socket Occupied and Online/Offline Jumper status from the local data structure
and format it to be readable/useable by other RAS modules. It will store the formatted values into
a global variable mHpRequest.

HPIOX Configuration: 
0x4E Port0: Bit4 = CPU0 Socket Occupied
0x4E Port0: Bit5 = CPU1 Socket Occupied
0x4E Port0: Bit6 = CPU2 Socket Occupied
0x4E Port0: Bit7 = CPU3 Socket Occupied
0x4E Port1: Bit5 = CPU1 O*L Status
0x4E Port1: Bit6 = CPU2 O*L Status
0x4E Port1: Bit7 = CPU3 O*L Status


For the following array, a value of 1 indicates/signifies the Socket is either Online, or primed to be Onlined.
A value of 0 indicates the Socket is either Offline, or primed to be Offlined.
NOTE: Use this array, together with mAcpiParameter->CpuOnOffMask to determine whether the RAS Request is to be Online or Offline, and for which socket.
mHpRequest[0] Bit0 CPU0 O*L Status  //NOTE: THIS VALUE IS ALWAYS 1, and should be ignored.
              Bit1 CPU1 O*L Status
              Bit2 CPU2 O*L Status
              Bit3 CPU3 O*L Status

For the following array, a value of 1 indicates/signifies that the Socket/IIO is currently Populated/Occupied. (0 means it is not populated)
mHpRequest[1] Bit 0 CPU0 Socket Occupied //NOTE: THIS VALUE IS ALWAYS 1, and should be ignored.
              Bit 1 CPU1 Socket Occupied
              Bit 2 CPU2 Socket Occupied
              Bit 3 CPU3 Socket Occupied

*/
{
  UINT8   CpuOLStatus    = 00;
  UINT8   CpuPresent      = 00;
  UINT8   Address         = 00;
  UINT8   PortNumber      = 00;
  UINT8   Index           = 00;
  

  while(mHpIoxValues[Index].Addr != 0xFF) {
    Address     = mHpIoxValues[Index].Addr;
      PortNumber  = mHpIoxValues[Index].PortNumber;
    
    if(Address == 0x4E)  {
      if(PortNumber == 00)  {
        CpuPresent = (~mHpIoxValues[Index].Value) >> 4;  //One's Compliment so 1 = Occ and 0 = NotOcc.
		    CpuPresent &= 0x0F;
      } else  {
        CpuOLStatus = mHpIoxValues[Index].Value >> 4; //Bit Manipulation so that 1 = Socket Online, and 0 = Socket Offline.
		    CpuOLStatus |= 1; //Compensating for default Skt0 always online.
      }
    }
    Index++;
  }

  HpRequest[0] = CpuOLStatus; 
  HpRequest[1] = CpuPresent; 
}


EFI_STATUS
ClearAttnLatch(
  IN BOOLEAN    TimeSliced
  )
{

  UINT8        Operation;
  UINT8        Command;
  STATIC UINT8 Buffer;
  STATIC UINT8 Index = 0;
  STATIC UINT8 EntryFlag = 0;
  EFI_STATUS   Status;

  Status      = EFI_SUCCESS;
  
  //  Clear the attn latch
  while( mSlaveAddress[Index] != 0xFF )  {
   if(!EntryFlag) {
     Operation   = SMB_READ;
     Command     = 00;
     if(!TimeSliced) {
       do{
         Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
       } while(Status == IN_PROGRESS);
      } else {
       Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
       if (Status == IN_PROGRESS)
         return Status;
      }
      ASSERT_EFI_ERROR (Status); 
      EntryFlag++;
    }

   if (!(Buffer & BIT4)) {
      if(EntryFlag == 1){
        Operation   = SMB_WRITE;
        Command     = 02;
        Buffer      &= ~BIT1;
        if(!TimeSliced) {
          do{
            Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          } while(Status == IN_PROGRESS);
        } else {
          Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          if (Status == IN_PROGRESS)
            return Status;
        }
        //DelayFn(70);
        ASSERT_EFI_ERROR (Status);
        EntryFlag++;
      } 
      
      if(EntryFlag == 2){
        Operation   = SMB_WRITE;
        Command     = 02;
        Buffer      |= BIT1;
        if(!TimeSliced) {
          do{
            Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          } while(Status == IN_PROGRESS);
        } else {
          Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          if (Status == IN_PROGRESS)
            return Status;
        }
        //DelayFn(70);
        ASSERT_EFI_ERROR (Status);
        EntryFlag++;
      } 
    }
  
   if(EntryFlag == 3){
     Operation   = SMB_READ;
     Command     = 01;
     if(!TimeSliced) {
       do{
         Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
       } while(Status == IN_PROGRESS);
      } else {
       Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
       if (Status == IN_PROGRESS)
         return Status;
      }
      ASSERT_EFI_ERROR (Status); 
      EntryFlag++;
    }

   if (!(Buffer & BIT4)) {
      if(EntryFlag == 4){
        Operation   = SMB_WRITE;
        Command     = 03;
        Buffer      &= ~BIT1;
        if(!TimeSliced) {
          do{
            Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          } while(Status == IN_PROGRESS);
        } else {
          Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          if (Status == IN_PROGRESS)
            return Status;
        }
        //DelayFn(70);
        ASSERT_EFI_ERROR (Status);
        EntryFlag++;
      } 
      
      if(EntryFlag == 5){
        Operation   = SMB_WRITE;
        Command     = 03;
        Buffer      |= BIT1;
        if(!TimeSliced) {
          do{
            Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          } while(Status == IN_PROGRESS);
        } else {
          Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
          if (Status == IN_PROGRESS)
            return Status;
        }
        //DelayFn(70);
        ASSERT_EFI_ERROR (Status);
        EntryFlag++;
      } 
    }
    Index++;
    EntryFlag = 0;
  }

  Index = 0;
  return Status;
}

void
DelayFn(
  UINT32 dCount
  )
// Description: This routine delays for specified number of micro seconds
// Input:       wCount    Amount of delay (count in 15microsec)
// Output:      None
{
  UINT8    Reference;
  UINT8    Current;

  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, 0x61, 1, &Reference);
  Reference &= 0x10;

  while(dCount) {
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT8, 0x61, 1, &Current);
    Current &= 0x10;
    if (Reference != Current)
    {
      Reference = Current;
      dCount--;
    }
  }
}

EFI_STATUS
IssuePldCmdThroIox (
  IN UINT8           Request,
  IN UINT8           SocketId,
  IN BOOLEAN         TimeSliced
  )
{
  UINT8        SlaveAddress = 0x4E;
  UINT8        Operation;
  UINT8        Command;
  UINT8        Buffer;
  EFI_STATUS   Status;

  Operation = SMB_WRITE;
  Buffer    = 0;
  Command   = 02;

  switch(Request) {  
  case ISSUE_ONLINE: //This command MUST be called *after* ISSUE_POWER_ON. That is, it will not work if the Socket/CPU is not at the the powergood-reset state.
    if (SocketId == 1){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S1 ONLINE being issued.\n"));	
      Buffer = 1;
    }else if (SocketId == 2){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S2 ONLINE being issued.\n"));
      Buffer = 3;
    }else if (SocketId == 3){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S3 ONLINE being issued.\n"));
      Buffer = 5;
    }
    break;        
  case ISSUE_OFFLINE: 
    if (SocketId == 1) {
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S1 OFFLINE being issued.\n"));
      Buffer = 0;
    }else if (SocketId == 2){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S2 OFFLINE being issued.\n"));
      Buffer = 2;
    }else if (SocketId == 3){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S3 OFFLINE being issued.\n"));
      Buffer = 4;
    }
    break;     
  case ISSUE_WARM_RESET: 
    if (SocketId == 1) {
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S1 WARM RESET being issued.\n"));
      Buffer = 6;
    }else if (SocketId == 2){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S2 WARM RESET being issued.\n"));
      Buffer = 7;
    }else if (SocketId == 3){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S3 WARM RESET being issued.\n"));
      Buffer = 8;
    }
    break;
  case ISSUE_POWER_ON: //This command MUST be called prior to ISSUE_ONLINE to power up the socket/cpu and hold it at powergood-reset.
    if (SocketId == 1) {
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S1 POWER ON being issued.\n"));
      Buffer = 9;
    }else if (SocketId == 2){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S2 POWER ON being issued.\n"));
      Buffer = 0xA;
    }else if (SocketId == 3){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S3 POWER ON being issued.\n"));
      Buffer = 0xB;
    }
    break;
  case ISSUE_BIOS_READY: 
  //This command is used during CPU ONLINE Flow. It creates a BIOS Syncpoint to enable/ensure any shared CPU error signals are added/enabled at precisely the right time during a Socket Online procedure.
  //BIOS asserts this command to enable shared error-related signals (e.g. CATERR and any other shared ERRs)
    if (SocketId == 1) {
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S1 BIOS READY being issued.\n"));
      Buffer = 0xC;
    }else if (SocketId == 2){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S2 BIOS READY being issued.\n"));
      Buffer = 0xD;
    }else if (SocketId == 3){
      DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: S3 BIOS READY being issued.\n"));
      Buffer = 0xE;
    }
    break;
  case ISSUE_PLD_IDLE:  //This should be the last command sent to the PLD after all RAS operations are complete; to bring the PLD to an idle state.
	DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: PLD Idle being issued.\n"));
	Buffer = 0xF;
    break;	
  default: 
    DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    PLD Command: Invalid PLD Command detected.\n"));
	return EFI_INVALID_PARAMETER;
  }

  if(!TimeSliced) {
    do{
       Status = SmbusReadWrite(SlaveAddress, Operation, Command, &Buffer); 
    }  while(Status == IN_PROGRESS);
  }
  else {
    Status = SmbusReadWrite(SlaveAddress, Operation, Command, &Buffer); 
    if (Status == IN_PROGRESS)
      return Status;     
  }
  ASSERT_EFI_ERROR (Status);
  return EFI_SUCCESS;
}

EFI_STATUS
ClearInterrupt(
  IN  BOOLEAN TimeSliced
  )
{

  UINT8        Operation;
  UINT8        Command;
  UINT8        Buffer;
  STATIC UINT8 Index = 0;
  STATIC UINT8 EntryFlag = 0;
  EFI_STATUS   Status;


  Status      = EFI_SUCCESS;
  Operation   = SMB_READ;
  //  Read all the ports to clear the interrupt
  while( mSlaveAddress[Index] != 0xFF )  {
    if(!EntryFlag) {
      Command     = 00;
      if(!TimeSliced) {
        do{
          Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
        }  while(Status == IN_PROGRESS);
      }
      else {
        Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
        if (Status == IN_PROGRESS)
        return Status;     
      }
    ASSERT_EFI_ERROR (Status);
    EntryFlag++;
    }

    Command     = 01;
    if(!TimeSliced) {
      do{
        Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
      }  while(Status == IN_PROGRESS);
    }
    else {
      Status = SmbusReadWrite(mSlaveAddress[Index], Operation, Command, &Buffer);
      if (Status == IN_PROGRESS)
      return Status;     
    }
    ASSERT_EFI_ERROR (Status);
    EntryFlag = 0;
    Index++;
  }

  Index = 0;

  return Status;
}


EFI_STATUS
HpSmbusReadWrite (
  IN      UINT8    SlaveAddress,
  IN      UINT8    Operation,
  IN      UINT8    Command,
  IN OUT  VOID     *Buffer,
  IN      BOOLEAN  TimeSliced
  )
{
  EFI_STATUS Status;

  if(!TimeSliced) {
    do{
      Status = SmbusReadWrite(SlaveAddress, Operation, Command, Buffer); 
    }  while(Status == IN_PROGRESS);
  }
  else {
    Status = SmbusReadWrite(SlaveAddress, Operation, Command, Buffer); 
    if (Status == IN_PROGRESS)
    return Status;     
  }
  return Status;
}

BOOLEAN
CheckOnlineOfflineRequest(
)
{
  UINT8       DevAddress;
  UINT8       Operation;
  UINT8       Command;
  UINT8       ProcIohRasEventData=00;
  EFI_STATUS  Status;

  //
  //Read Online/Offline request data
  //
  DevAddress  = 0x4E;
  Operation   = SMB_READ;
  Command     = 01;
  do{
    Status = SmbusReadWrite(DevAddress, Operation, Command, &ProcIohRasEventData); 
  }while(Status == IN_PROGRESS);
  ASSERT_EFI_ERROR (Status);
  // Check the request corrresponds to CPU
  if((((ProcIohRasEventData >> 4 ) & 0x0F) ^ (UINT8)mIioUdsData->SystemStatus.socketPresentBitMap) != 0x00) {
    return TRUE;
  }
 
  DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "    No CPU RAS event can be confirmed. Will exit, doing nothing.\n"));
  DEBUG     ((EFI_D_INFO | EFI_D_ERROR, "\n    RAS FLOW COMPLETE\n    RAS FLOW COMPLETE\n    RAS FLOW COMPLETE\n\n"));
  
  return  FALSE;
}
