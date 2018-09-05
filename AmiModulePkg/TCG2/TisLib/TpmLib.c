/*++

Copyright (c) 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  TpmLib.h

Abstract:

  Common code of TPM driver in both PEI and DXE
--*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TcgNext/Libraries/TisLib/TpmLib.c 1     10/08/13 11:58a Fredericko $
//
// $Revision: 1 $
//
// $Date: 10/08/13 11:58a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /Alaska/SOURCE/Modules/TcgNext/Libraries/TisLib/TpmLib.c $
// 
// 1     10/08/13 11:58a Fredericko
// Initial Check-In for Tpm-Next module
// 
// 2     10/03/13 12:33p Fredericko
// 
// 1     7/10/13 5:50p Fredericko
// [TAG]  		EIP120969
// [Category]  	New Feature
// [Description]  	TCG (TPM20)
// [Files]  		TisLib.cif
// TisLib.mak
// TcgTpm12.h
// TpmLib.h
// TcgCommon.h
// ZTEICTcmOrdinals.h
// TpmLib.c
// TcgCommon.c
// TisLib.sdl
// sha1.h
// INTTcgAcpi.h
// TcgPc.h
// TcmPc.h
// TcgEfiTpm.h
// TcgEFI12.h
// 
// 23    9/08/11 4:46p Fredericko
// [TAG]  		EIPEIP0000
// [Category]  	Improvement
// [Description]  	Added ability to modify Delay Amount for TPM driver
// [Files]  		Tcg.sdl
// Tpmlib.c
// 
// 22    8/26/11 12:31p Fredericko
// [TAG]  		EIP64300 
// [Category]  	Improvement
// [Description]  	Allow variations for TPM driver wait times. 
// [Files]  		Tpmlib.c
// Tcg.sd
// 
// 21    7/07/10 12:09p Fredericko
// Make Wait variable a #define.
// 
// 20    5/19/10 4:50p Fredericko
// Included File Header
// 
// 19    5/18/10 5:39p Fredericko
// Code Beautification and Enhancement
// 
// 18    5/18/10 11:32a Fredericko
// Header modification for AMI code standard.
// 
// 17    5/04/10 3:36p Fredericko
// Source Enhancement. EIP 22208
// 
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TpmLib.c
//
// Description: 
//  Contains low level TCG functions
//
//<AMI_FHDR_END>
//*************************************************************************
#include <Efi.h>
#include "AmiTcg/TpmLib.h"
#include <Library/BaseLib.h>
#include<Library/IoLib.h>
#include<Library/TimerLib.h>
#include <Token.h>

extern EFI_STATUS CountTime ( IN UINTN	DelayTime,  IN	UINT16	BaseAddr); // only needs to be 16 bit for I/O address)
#define Wait  TPM_DRIVER_WAIT 
#define ACCESS_WAITCOUNT    (750 * 1000 / 100)    // 750MS (750 * 10000 /1000)
#define ACCESS_WAITCOUNTB   (  3 * 1000000/100) // 3 seconds delay

#define NUM_BITS_IN_ACPI_TIMER  24  // Porting
#define MAX_ACPI_TIMER_BITS     32  // Porting

#define TIS_TIMEOUT_A   ACCESS_WAITCOUNT
#define TIS_TIMEOUT_B   ACCESS_WAITCOUNTB
#define TIS_TIMEOUT_C   ACCESS_WAITCOUNT
#define TIS_TIMEOUT_D   ACCESS_WAITCOUNT


EFI_STATUS TcgCountTime (
      IN  UINTN   DelayTime
  )
{
    MicroSecondDelay(DelayTime);
    return EFI_SUCCESS;
}





//**********************************************************************
//<AMI_PHDR_START>
//
// Procedure:   FixedDelay
//
// Description: This routine delays for specified number of micro seconds
//
// Input:       IN UINT32  dCount      Amount of delay (count in 20microsec)
//
// Output:      NONE
//
// Modified:
//
// Referrals:   Div64, CountTime
//
// Notes:       
//<AMI_PHDR_END>
//**********************************************************************

void
EFIAPI
FixedDelay(UINT32 dCount)
{
  UINTN  MicroSDelay = DELAY_AMOUNT;

  while(dCount) {
      TcgCountTime(MicroSDelay);  
      dCount--;
  }
}
// function parameter name changed
static
UINT8
EFIAPI
CheckAccessBit (
  IN      volatile UINT8    *Access,
  IN      UINT8             Bit,
  IN      UINT32            Timeout
  )
/*++
Routine Description:
  Function to check bits in TPM access register
Arguments:
  *Sts      - A Pointer to Status register
  Bit       - Bit position
  Timeout    - Timeout amount to wait till the specified bit
Returns:
  EFI_Status
--*/
{
    UINT32  AccessCount = Timeout;

    do {
        FixedDelay((UINT32)Wait);
        if((*Access & Bit) && (*Access & TPM_STS_VALID) ){ return *Access & Bit; }  // Improvement for ST TPM Hardware protocol.
        AccessCount--;
    } while (AccessCount);

    return 0;

}

static
UINT8
EFIAPI
CheckStsBit (
  IN      volatile UINT8            *Sts,
  IN      UINT8                     Bit
  )
/*++
Routine Description:
  Function to check status a specific TPM status bit 
Arguments:
  *Sts  - A Pointer to Status register
  Bit   - Bit position
Returns:
  EFI_Status
--*/
{
  UINT32  AccessCount =  TIS_TIMEOUT_B;

  do {
      FixedDelay((UINT32)Wait);
      AccessCount-=1;
      if(AccessCount == 0)return 0;
  }while (!(*Sts & TPM_STS_VALID));
  
  return *Sts & Bit;
}




static
UINT16
EFIAPI
ReadBurstCount (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
)
/*++
Routine Description:
  Gets the number of bytes (burstCount) that the TPM can return
  on reads or accept on writes without inserting LPC long wait 
  states on the LPC bus.
  burstCount is TPM_STS_x register bits 8..23
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  0                - Any Errors
  burstCount       - TPM_STS_x.burstCount
--*/  
{
  UINT16   burstCount;
  UINT64   Deadline = TIS_TIMEOUT_D;

  burstCount = 0;

  do {
    //
    // burstCount is little-endian bit ordering
    //
    FixedDelay((UINT32)Wait);
    Deadline--;
    burstCount = TpmReg->BurstCount;
  }while (!burstCount && (Deadline > 0));
  
  return burstCount;
}



EFI_STATUS
EFIAPI
TisRequestLocality (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Requests TPM locality 0
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/  
{
  if ((TpmReg->Access & TPM_ACC_ACTIVE_LOCALITY)==TPM_ACC_ACTIVE_LOCALITY){
    return EFI_SUCCESS;//EFI_ALREADY_STARTED;
  }
  TpmReg->Access = TPM_ACC_REQUEST_USE;
  if (CheckAccessBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY, (UINT32)TIS_TIMEOUT_B)) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_READY;
  }
}



#pragma optimize("",off)
EFI_STATUS
EFIAPI
TisReleaseLocality (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Release TPM locality 0
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{
  UINT32    AccessCount=ACCESS_WAITCOUNT;    
  if (!CheckStsBit (&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)) {
    return EFI_DEVICE_ERROR;
  }

  TpmReg->Access = TPM_ACC_ACTIVE_LOCALITY;
  if (CheckStsBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)) {
    do{/*give locality time to be released*/
     FixedDelay((UINT32)Wait); 
       AccessCount--;
    }while(((CheckStsBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)) && AccessCount));
    if(CheckStsBit(&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY)){return EFI_DEVICE_ERROR;}
  }


  return EFI_SUCCESS;
}
#pragma optimize("",on)




EFI_STATUS
EFIAPI
TisPrepareSendCommand (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Helper function to prepare to send a TPM command
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{
    UINTN   AccessCount = TIS_TIMEOUT_B;
    UINTN   RetryCmdReadyCoount;

    if( !CheckStsBit (&TpmReg->Access, TPM_ACC_ACTIVE_LOCALITY) )
    {
        return EFI_NOT_STARTED;
    }

    for( RetryCmdReadyCoount = 2; RetryCmdReadyCoount > 0; --RetryCmdReadyCoount )
    {
        // Initial AccessCount.
        AccessCount = TIS_TIMEOUT_B;

        TpmReg->Sts = TPM_STS_READY;
        do {
          --AccessCount;
          FixedDelay((UINT32)Wait);
  } while ( !(TpmReg->Sts & TPM_STS_READY) && AccessCount );

        if( AccessCount > 0 )
        {
            break;
        }
    }

    if( 0 == AccessCount )
    {
        return EFI_DEVICE_ERROR;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
TisSendCommand (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg,
  IN      const VOID                *CmdStream,
  IN      UINTN                     Size,
  IN      BOOLEAN                   Last
  )
/*++
Routine Description:
  Function to send TPM command
Arguments:
  TpmReg       - A Pointer to the TPM Register Space
  *CmdStream   - A Pointer to the command stream to be sent to TPM Fifo
  Size         - Size in bytes of the command stream
  Last         - Boolean to signify the last byte?
Returns:
  EFI_Status
--*/
{
  UINT8                             *Ptr;
  UINT16                            burstCount;

  if (Size == 0) {
    return Last ? EFI_INVALID_PARAMETER : EFI_SUCCESS;
  }

  if ((TpmReg->Access & TPM_ACC_ACTIVE_LOCALITY)!=TPM_ACC_ACTIVE_LOCALITY) {
    return EFI_ABORTED;
  }

  Ptr = (UINT8*)CmdStream;

  if (Last) {
    Size--;
  }

  while (Size > 0) {
      burstCount = ReadBurstCount(TpmReg);
    if (burstCount == 0) {
      // Cannot get the correct burstCount value
      return EFI_TIMEOUT;
    }

    for (; burstCount > 0 && Size > 0; burstCount--) {
     *(UINT8*)&TpmReg->DataFifo = *Ptr;
      Ptr++;
      Size--;
    }
   }

  if (Last) {
    if (!CheckStsBit (&TpmReg->Sts, TPM_STS_EXPECT)) {
      return EFI_ABORTED;
    }

    *(UINT8*)&TpmReg->DataFifo = *Ptr;

    if (CheckStsBit (&TpmReg->Sts, TPM_STS_EXPECT)) {
      return EFI_ABORTED;
    }

    TpmReg->Sts = TPM_STS_GO;
            
  }
  return EFI_SUCCESS;
}




EFI_STATUS
EFIAPI
TisWaitForResponse (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Waits till TPM result is available
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{

  while (!CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE));
  return EFI_SUCCESS;
}




EFI_STATUS
EFIAPI
TisReceiveResponse (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg,
  OUT     VOID                      *Buffer,
  OUT     UINTN                     *Size
  )
/*++
Routine Description:
  Function to recieve TPM command results
Arguments:
  TpmReg       - A Pointer to the TPM Register Space
  *Buffer      - A Pointer to buffer for recieving result data
  Size         - buffer size
Returns:
  EFI_Status
--*/
{
  UINT8                             *Ptr, *BufEnd;
  UINT16                             burstCount;
  Ptr = (UINT8*)Buffer;
  BufEnd = Ptr + *Size;
  while (Ptr < BufEnd &&
         CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE)) {
      
        burstCount = ReadBurstCount(TpmReg);
          if (burstCount == 0) {
              return EFI_TIMEOUT;
        }            
        while(burstCount && Ptr < BufEnd  && CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE )){
            *Ptr++ = *(UINT8*)&TpmReg->DataFifo;
            burstCount--;    }
  }

  *Size -= BufEnd - Ptr;
  if (CheckStsBit (&TpmReg->Sts, TPM_STS_DATA_AVAILABLE)) {
    return EFI_BUFFER_TOO_SMALL;
  } else {
    return EFI_SUCCESS;
  }
}



VOID
EFIAPI
TisResendResponse (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
/*++
Routine Description:
  Sets Bit to resend TPM command
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
Returns:
  EFI_Status
--*/
{
  TpmReg->Sts = TPM_STS_RESPONSE_RETRY;
}

EFI_STATUS
EFIAPI
IsTpmPresent (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg
  )
{
  if ((TpmReg->Access == 0xff) || (TpmReg->Access == 0x00)) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}




EFI_STATUS
EFIAPI
TpmLibPassThrough (
  IN      TPM_1_2_REGISTERS_PTR     TpmReg,
  IN      UINTN                     NoInputBuffers,
  IN      TPM_TRANSMIT_BUFFER       *InputBuffers,
  IN      UINTN                     NoOutputBuffers,
  IN OUT  TPM_TRANSMIT_BUFFER       *OutputBuffers
  )
/*++
Routine Description:
  Higher level function to send a recieve commands to the TPM
Arguments:
  TpmReg           - A Pointer to the TPM Register Space
  NoInputBuffers   - Number count of Input buffers
  *InputBuffers    - Pointer to InputBuffers[0]
  NoOutputBuffers  - Number count of Output buffers
  *OutputBuffers   - Pointer to OutputBuffers[0]
Returns:
  EFI_Status
--*/
{
  EFI_STATUS                        Status;
  UINTN                             i;
  UINTN                             count=NoInputBuffers;

  if (NoInputBuffers == 0 
          || InputBuffers == NULL
          || OutputBuffers == NULL
          || NoOutputBuffers == 0) {
    return EFI_INVALID_PARAMETER;
  }

  Status = TisPrepareSendCommand (TpmReg);
  if (EFI_ERROR (Status)){
    return EFI_DEVICE_ERROR;
  }

  for(i=0;  !EFI_ERROR (Status) &&  i< NoInputBuffers; i++)
  {
     ///------------------- Send DATA -------------------------------
     if((count - 1) == 0){
         Status = TisSendCommand(
                 TpmReg,
                 InputBuffers[i].Buffer,
                 InputBuffers[i].Size,
                 TRUE
                 );
     }else{
         Status = TisSendCommand(
                             TpmReg,
                             InputBuffers[i].Buffer,
                             InputBuffers[i].Size,
                             FALSE
                             );
     }
     count-=1;
  }

  if (!EFI_ERROR (Status)) {
    Status = TisWaitForResponse (TpmReg);
  }

  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = EFI_BUFFER_TOO_SMALL;
  for (i = 0; Status == EFI_BUFFER_TOO_SMALL && i < NoOutputBuffers; i++) {
    Status = TisReceiveResponse (
      TpmReg,
      OutputBuffers[i].Buffer,
      &OutputBuffers[i].Size
      );
  }
  TpmReg->Sts = TPM_STS_READY;
Exit:
  return Status;
}
