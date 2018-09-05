/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2007 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file Hecicore.c

  Heci driver core. For PEI/Dxe Phase, determines the HECI device and initializes it.

**/

#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Pci.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/PlatformHooksLib.h>
#include <MeChipset.h>
#include <Library/HeciCoreLib.h>

#include "MeAccess.h"
#include "HeciCore.h"
#include "MeState.h"

#ifndef NON_BLOCKING
#define NON_BLOCKING                  0
#define BLOCKING                      1
#endif

typedef enum _CLIENT_ID
{
  HECI_CLIENT = 0,
  DCMI_CLIENT = 0x1,
} CLIENT_ID;

#define NUM_ME_CLIENTS  2

typedef struct {
  UINT8   ClientId;
  UINT8   HostAddress;
  UINT32  MaxMessageLength;
  BOOLEAN Connected;
} ME_CLIENT_PROPERTIES;

ME_CLIENT_PROPERTIES  MeClients[NUM_ME_CLIENTS];


/*
 * Read HECI MBAR, assign default if not assigned.
 *
 * return 64-bit MBAR is returned, or NULL if HECI is not enabled.
 */
UINTN
HeciMbarRead (VOID)
{
  union
  {
    UINT64   QWord;
    struct
    {
      UINT32 DWordL;
      UINT32 DWordH;
    } Bits;
  } Mbar;
  
  Mbar.QWord = 0;
  Mbar.Bits.DWordL = HeciPciRead32(R_HECIMBAR);
  if (Mbar.Bits.DWordL == 0xFFFFFFFF)
  {
    DEBUG((DEBUG_ERROR, "[HECI] ERROR: HECI-1 device disabled\n"));
    Mbar.Bits.DWordL = 0;
    goto GetOut;
  }
  Mbar.Bits.DWordL &= 0xFFFFFFF0; // clear address type bits
  Mbar.Bits.DWordH = HeciPciRead32(R_HECIMBAR + 4);
  
  if (IsSimicsPlatform() && Mbar.Bits.DWordH != 0)
  {
    DEBUG((DEBUG_WARN, "[HECI] Detected 64-bit MBAR in HECI-1 under SIMICS, force 32-bit\n"));
    Mbar.QWord = 0;
  }
  if (Mbar.QWord == 0)
  {
    Mbar.QWord = HECI1_PEI_DEFAULT_MBAR;
    DEBUG((DEBUG_WARN, "[HECI] WARNING: MBAR not programmed, using default 0x%08X%08X\n",
           Mbar.Bits.DWordH, Mbar.Bits.DWordL));
    HeciPciWrite32(R_HECIMBAR + 4, Mbar.Bits.DWordH);
    HeciPciWrite32(R_HECIMBAR, Mbar.Bits.DWordL);
  }
 GetOut:
  return (UINTN)Mbar.QWord;
}


/**
  @brief
  This function provides a standard way to read and verify the HECI cmd and MBAR regs
  in its PCI cfg space are setup properly.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval UINTN                  HeciMar address

**/
UINTN
CheckAndFixHeciForAccess (
  IN HECI_DEVICE HeciDev
  )
{
  UINTN HeciMBAR;

  ASSERT(HeciDev == HECI1_DEVICE);

  //
  // Read HECI_MBAR in case it has changed
  // Check if Base register is 64 bits wide.
  //
  HeciMBAR = HeciMbarRead();

  //
  // Check if HECI_MBAR is disabled
  //
  if ((HeciPciRead8 (PCI_COMMAND_OFFSET) & (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)) !=
        (EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
        ) {
    //
    // If cmd reg in pci cfg space is not turned on turn it on.
    //
    HeciPciOr8 (
      PCI_COMMAND_OFFSET,
      EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER
      );
  }

  return HeciMBAR;
}

//
// //////////////////////////////////////////////////////////////////////////////////
// Macro definition for function used in Heci driver
////////////////////////////////////////////////////////////////////////////////////
//
/**

  The routing of MmIo Read Dword

  @param a - The address of Mmio

  @retval Return the value of MmIo Read

**/
UINT32
MmIoReadDword (
  UINTN a
  )
{
  VOLATILE HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;

  HeciRegHCsrPtr = (HECI_HOST_CONTROL_REGISTER *) a;
  return HeciRegHCsrPtr->ul;
}

/**

  The routing of MmIo Write Dword

  @param a - The address of Mmio
  @param b - Value revised

  @retval None

**/
VOID
MmIoWriteDword (
  UINTN  a,
  UINT32 b
  )
{
  VOLATILE HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;

  HeciRegHCsrPtr      = (HECI_HOST_CONTROL_REGISTER *) a;

  HeciRegHCsrPtr->ul  = b;
}

#define MMIOREADDWORD(a)      MmIoReadDword (a)
#define MMIOWRITEDWORD(a, b)  MmIoWriteDword (a, b)

//
// //////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////////
//
UINT8
FilledSlots (
  IN      UINT32                    ReadPointer,
  IN      UINT32                    WritePointer
  );

EFI_STATUS
OverflowCB (
  IN      UINT32                    ReadPointer,
  IN      UINT32                    WritePointer,
  IN      UINT32                    BufferDepth
  );

#if HECI_DUMP_ENABLE
/*****************************************************************************
 @brief
  Prints a message to the provided buffer

  @param  Buffer      Buffer for output string
  @param  Size        Buffer size
  @param  Format      Format string for the message to print.
  @param  ...         Variable argument list whose contents are accessed
                      based on the format string specified by Format.

**/
VOID
EFIAPI
HeciCreateTrace (
  IN OUT CHAR8     *Buffer,
  IN  UINTN        Size,
  IN  CONST CHAR8  *Format,
  ...
  )
{
  if (Buffer != NULL) {
    VA_LIST  Marker;

    //
    // Convert the message to an ASCII String
    //
    VA_START (Marker, Format);
    AsciiVSPrint (Buffer, Size, Format, Marker);
    VA_END (Marker);
  }
}
#endif // HECI_DUMP_ENABLE

/*****************************************************************************
 @brief
    Trace the message to the terminal.
    Normally just trace the header, if needed dump whole message to terminal.

 @param[in] pPrefix    Text prefix, whether it is receive or send
 @param[in] MsgHdr     The header of the message
 @param[in] pMsgBody   Pointer to the message body
 @param[in] MsgBodyLen The length of message body
**/
VOID HeciTrace(
  CONST CHAR8  *pPrefix,
  CONST UINT32 MsgHdr,
  CONST UINT8  *pMsgBody,
        UINT32 MsgBodyLen)
{
#if !HECI_DUMP_ENABLE
  if (MsgBodyLen > sizeof(UINT32))
  {
    DEBUG((DEBUG_INFO, "[HECI] %a%08X %08X ...\n", pPrefix, MsgHdr, *(UINT32*)pMsgBody));
  }
  else if (MsgBodyLen > 0)
  {
    DEBUG((DEBUG_INFO, "[HECI] %a%08X %08X\n", pPrefix, MsgHdr, *(UINT32*)pMsgBody));
  }
  else
  {
    DEBUG((DEBUG_INFO, "[HECI] %a%08X\n", pPrefix, MsgHdr));
  }
#else
#define ME_LINE_BREAK    16
#define ME_DEBUG_LINE    ME_LINE_BREAK*4
#define ME_DEBUG_CHAR    10

  UINT32  LineBreak = 0;
  UINT32  Index = 0;
  CHAR8   Line[ME_DEBUG_LINE];
  CHAR8   NextValue[ME_DEBUG_CHAR];
  
  DEBUG((EFI_D_ERROR, "[HECI] %a%08X\n", pPrefix, MsgHdr));
  *Line = 0;
  while (MsgBodyLen-- > 0)
  {
    if (LineBreak == 0)
    {
      HeciCreateTrace(Line, ME_DEBUG_CHAR, "%02x: ", (Index & 0xF0));
    }
    HeciCreateTrace(NextValue, ME_DEBUG_CHAR, "%02x ", pMsgBody[Index++]);
    AsciiStrCat(Line, NextValue);
    LineBreak++;
    if (LineBreak == ME_LINE_BREAK)
    {
      HeciCreateTrace(NextValue, ME_DEBUG_CHAR, "\n");
      AsciiStrCat(Line, NextValue);
      DEBUG((EFI_D_ERROR, Line));
      LineBreak = 0;
      *Line = 0;
    }
    if (LineBreak == ME_LINE_BREAK/2)
    {
      HeciCreateTrace(NextValue, ME_DEBUG_CHAR, "- ");
      AsciiStrCat(Line, NextValue);
    }
  }
  if (LineBreak != 0)
  {
      HeciCreateTrace(NextValue, ME_DEBUG_CHAR, "\n");
      AsciiStrCat(Line, NextValue);
      DEBUG((EFI_D_ERROR, Line));
  }
  DEBUG((EFI_D_ERROR, "\n"));

#undef ME_LINE_BREAK
#undef ME_DEBUG_LINE
#undef ME_DEBUG_CHAR

#endif
}

//
// //////////////////////////////////////////////////////////////////////////////////
// Heci driver function definitions
////////////////////////////////////////////////////////////////////////////////////
//

EFI_STATUS
WaitForMEReady(
  IN HECI_DEVICE HeciDev
  )
/**

    Waits for the ME to report that it is ready for communication over the HECI
    interface.

    @param[in] HeciDev              The HECI device to be accessed.

    @retval EFI_STATUS

**/
{
  UINT32                    Timeout = (HECI_INIT_TIMEOUT + HECI_WAIT_DELAY / 2) / HECI_WAIT_DELAY;
  HECI_ME_CONTROL_REGISTER  HeciRegMeCsrHa;
  UINTN                     HeciMBAR = CheckAndFixHeciForAccess(HECI1_DEVICE);

  ASSERT(HeciDev == HECI1_DEVICE);

  //
  //  Wait for ME ready
  //
  while (1)
  {
    HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
    if (HeciRegMeCsrHa.r.ME_RDY_HRA)
    {
      return EFI_SUCCESS;
    }
    //
    // If ME requests HECI reset do the reset
    //
    if (HeciRegMeCsrHa.r.ME_RST_HRA)
    {
      EFI_STATUS Status;

      Status = HeciResetInterface(HECI1_DEVICE);
      if (EFI_ERROR(Status))
      {
        return Status;
      }
      return EFI_SUCCESS;
    }
    if (Timeout-- == 0)
    {
      DEBUG((DEBUG_WARN, "[HECI] ME interface not ready (CSR: %08X/%08X, MEFS1:%08X)\n",
             MMIOREADDWORD (HeciMBAR + H_CSR), HeciRegMeCsrHa.ul, HeciPciRead32(R_FWSTATE)));
      return EFI_TIMEOUT;
    }
    MicroSecondDelay(HECI_WAIT_DELAY);
  }
}

/**

    Checks if HECI reset has occurred.

    @param None.

    @retval TRUE - HECI reset occurred
    @retval FALSE - No HECI reset occurred

**/
BOOLEAN
CheckForHeciReset (
  VOID
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER    HeciRegMeCsrHa;
  UINTN                       HeciMBAR = CheckAndFixHeciForAccess(HECI1_DEVICE);

  //
  // Init Host & ME CSR
  //
  HeciRegHCsr.ul    = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);

  if ((HeciRegMeCsrHa.r.ME_RDY_HRA == 0) || (HeciRegHCsr.r.H_RDY == 0)) {
    return TRUE;
  }

  return FALSE;
}


/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_UNSUPPORTED         HECI MSG is unsupported
**/
EFI_STATUS
EFIAPI
HeciInitialize (
  IN HECI_DEVICE              HeciDev)
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  UINTN                       HeciMBAR;

  ASSERT(HeciDev == HECI1_DEVICE);
  //
  // Check for HECI-1 PCI device availability
  //
  if (HeciPciRead16(PCI_VENDOR_ID_OFFSET) != 0x8086)
  {
    DEBUG((EFI_D_ERROR, "[HECI] ERROR: Device not present\n"));
    return EFI_DEVICE_ERROR;
  }

  //
  // Make sure that HECI device BAR is correct and device is enabled.
  //
  HeciMBAR = CheckAndFixHeciForAccess (HECI1_DEVICE);

  //
  // Set HECI-1 interrupt delivery mode to MSI interrupt
  //
  HeciPciWrite8(R_HIDM, HECI_INTERRUPUT_GENERATE_LEGACY_MSI);
  //
  // Set HECI-1 interrupt delivery mode to MSI interrupt
  //
  HeciPciWrite8(R_HIDM, HECI_INTERRUPUT_GENERATE_LEGACY_MSI);
  //
  // Set HECI-1 interrupt delivery mode lock
  //
  HeciPciWrite8 (R_HIDM, HeciPciRead8 (R_HIDM) | HECI_INTERRUPUT_LOCK);

  //
  // Enable HECI BME and MSE
  //
  HeciPciWrite8(PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER);

  //
  // Need to do following on ME init:
  //
  //  1) wait for ME_CSR_HA reg ME_RDY bit set
  //
  if (WaitForMEReady (HECI1_DEVICE) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  //
  //  2) setup H_CSR reg as follows:
  //     a) Make sure H_RST is clear
  //     b) Set H_RDY
  //     c) Set H_IG
  //
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  if (HeciRegHCsr.r.H_RDY == 0) {
    HeciRegHCsr.r.H_RST = 0;
    HeciRegHCsr.r.H_RDY = 1;
    HeciRegHCsr.r.H_IE  = 0;
    HeciRegHCsr.r.H_IS  = 1;
    HeciRegHCsr.r.H_IG  = 1;
    MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);
  }

  return EFI_SUCCESS;
}


/**
  Heci Re-initializes it for Host

  @param[in] HeciDev          The HECI device to be accessed.

  @retval EFI_TIMEOUT         ME is not ready
  @retval EFI_STATUS          Status code returned by ResetHeciInterface
**/
EFI_STATUS
EFIAPI
HeciReInitialize(
  IN HECI_DEVICE              HeciDev)
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  EFI_STATUS                  Status;
  UINTN                       HeciMBAR;

  Status = EFI_SUCCESS;
  //
  // Need to do following on ME init:
  //
  //  1) wait for HOST_CSR_HA reg H_RDY bit set
  //
  //    if (WaitForHostReady() != EFI_SUCCESS) {
  //
  if (HeciMeResetWait(HeciDev, HECI_INIT_TIMEOUT) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }

  HeciMBAR = CheckAndFixHeciForAccess(HECI1_DEVICE);
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  if (HeciRegHCsr.r.H_RDY == 0) {
    Status = HeciResetInterface(HeciDev);
  }

  return Status;
}


/**

    Function to pull one message packet off the HECI circular buffer.
    Corresponds to HECI HPS (part of) section 4.2.4


    @param Blocking      - Used to determine if the read is BLOCKING or NON_BLOCKING.
    @param MessageHeader - Pointer to a buffer for the message header.
    @param MessageData   - Pointer to a buffer to receive the message in.
    @param Length        - On input is the size of the callers buffer in bytes.  On
                           output this is the size of the packet in bytes.

    @retval EFI_STATUS

**/
EFI_STATUS
HECIPacketRead (
  IN      UINT32                    Blocking,
  OUT     HECI_MESSAGE_HEADER       *MessageHeader,
  OUT     UINT32                    *MessageData,
  IN OUT  UINT32                    *Length
  )
{
  BOOLEAN                     GotMessage;
  UINT32                      Timeout;
  UINT32                      Timeout1;
  UINT32                      i;
  UINT32                      LengthInDwords;
  HECI_ME_CONTROL_REGISTER    HeciRegMeCsrHa;
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  UINTN                       HeciMBAR = CheckAndFixHeciForAccess(HECI1_DEVICE);

  GotMessage = FALSE;
  //
  // Initialize memory mapped register pointers
  //
  //  VOLATILE HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr     = (VOID*)(HeciMBAR + H_CSR);
  //  VOLATILE HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr  = (VOID*)(HeciMBAR + ME_CSR_HA);
  //  VOLATILE UINT32                     *HeciRegMeCbrwPtr   = (VOID*)(HeciMBAR + ME_CB_RW);
  //
  // clear Interrupt Status bit
  //
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IS  = 1;

  //
  // test for circular buffer overflow
  //
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  if (OverflowCB (
        HeciRegMeCsrHa.r.ME_CBRP_HRA,
        HeciRegMeCsrHa.r.ME_CBWP_HRA,
        HeciRegMeCsrHa.r.ME_CBD_HRA
        ) != EFI_SUCCESS) {
    //
    // if we get here, the circular buffer is overflowed
    //
    *Length = 0;
    return EFI_DEVICE_ERROR;
  }
  //
  // If NON_BLOCKING, exit if the circular buffer is empty
  //
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);;
  if ((FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) == 0) && (Blocking == NON_BLOCKING)) {
    *Length = 0;
    return EFI_NO_RESPONSE;
  }
  //
  // Calculate timeout counter
  //
  Timeout = (HECI_SINGLE_READ_TIMEOUT + HECI_WAIT_DELAY / 2) / HECI_WAIT_DELAY;

  //
  // loop until we get a message packet
  //
  while (!GotMessage) {
    //
    // If 1 second timeout has expired, return fail as we have not yet received a full message.
    //
    if (Timeout-- == 0) {
      *Length = 0;
      return EFI_TIMEOUT;
    }
    //
    // Read one message from HECI buffer and advance read pointer.  Make sure
    // that we do not pass the write pointer.
    //
    HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);;
    if (FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) > 0) {
      //
      // Eat the HECI Message header
      //
      MessageHeader->Data = MMIOREADDWORD (HeciMBAR + ME_CB_RW);

      //
      // Compute required message length in DWORDS
      //
      LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

      //
      // Just return success if Length is 0
      //
      if (MessageHeader->Fields.Length == 0) {
        //
        // Set Interrupt Generate bit and return
        //
        MMIOREADDWORD (HeciMBAR + H_CSR);
        HeciRegHCsr.r.H_IG = 1;
        MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);
        *Length = 0;
        goto GotMessage;
      }
      //
      // Make sure that the message does not overflow the circular buffer.
      //
      HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
      if ((MessageHeader->Fields.Length + sizeof (HECI_MESSAGE_HEADER)) > (HeciRegMeCsrHa.r.ME_CBD_HRA * 4)) {
        *Length = 0;
        return EFI_DEVICE_ERROR;
      }
      //
      // Make sure that the callers buffer can hold the correct number of DWORDS
      //
      if ((MessageHeader->Fields.Length) <= *Length) {
        //
        // Calculate timeout counter for inner loop
        //
        Timeout1 = (HECI_SINGLE_READ_TIMEOUT + HECI_WAIT_DELAY / 2) / HECI_WAIT_DELAY;

        //
        // Wait here until entire message is present in circular buffer
        //
        HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
        while (LengthInDwords > FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA)) {
          //
          // If 1 second timeout has expired, return fail as we have not yet received a full message
          //
          if (Timeout1-- == 0) {
            *Length = 0;
            return EFI_TIMEOUT;
          }
          //
          // Wait before we read the register again
          //
          MicroSecondDelay(HECI_WAIT_DELAY);

          //
          // Read the register again
          //
          HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
        }
        //
        // copy rest of message
        //
        for (i = 0; i < LengthInDwords; i++) {
          MessageData[i] = MMIOREADDWORD (HeciMBAR + ME_CB_RW);
        }
        //
        // Update status and length
        //
        GotMessage  = TRUE;
        *Length     = MessageHeader->Fields.Length;

      } else {
        //
        // Message packet is larger than caller's buffer
        //
        *Length = 0;
        return EFI_BUFFER_TOO_SMALL;
      }
    }
    //
    // Wait before we try to get a message again
    //
    MicroSecondDelay(HECI_WAIT_DELAY);
  }
  //
  // Read ME_CSR_HA.  If the ME_RDY bit is 0, then an ME reset occurred during the
  // transaction and the message should be discarded as bad data may have been retrieved
  // from the host's circular buffer
  //
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  if (!HeciRegMeCsrHa.r.ME_RDY_HRA)
  {
    *Length = 0;
    //
    // Check if ME requested reset during the transaction
    //
    if (HeciRegMeCsrHa.r.ME_RST_HRA)
    {
      HeciResetInterface(HECI1_DEVICE);
    }
    return EFI_DEVICE_ERROR;
  }
  //
  // Set Interrupt Generate bit
  //
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IG  = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

 GotMessage:
  HeciTrace(" Got msg: ", MessageHeader->Data, (UINT8*)MessageData, *Length);

  return EFI_SUCCESS;
}


/**
  Reads a message from the ME across HECI. This function can only be used after invoking HeciSend() first.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in][out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN      HECI_DEVICE  HeciDev,
  IN      UINT32       Blocking,
  IN OUT  UINT32      *MessageBody,
  IN OUT  UINT32      *Length
  )
{
  HECI_MESSAGE_HEADER       PacketHeader = {0};
  UINT32                    CurrentLength;
  UINT32                    OriginalLength;
  UINT32                    MessageComplete;
  EFI_STATUS                ReadError;
  EFI_STATUS                Status;
  UINT32                    PacketBuffer;
  BOOLEAN                   QuitFlag;
  HECI_ME_CONTROL_REGISTER  HeciRegMeCsrHa;
  UINTN                     HeciMBAR = CheckAndFixHeciForAccess(HECI1_DEVICE);
#ifdef SPS_CMOS_WRITE
  UINT8                     CMOS_HaveFC;
#endif

  ASSERT(HeciDev == HECI1_DEVICE);
  
  CurrentLength     = 0;
  MessageComplete   = 0;
  Status            = EFI_SUCCESS;
  QuitFlag          = FALSE;
  OriginalLength    = *Length;

  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);

  do {
    //
    // Make sure that HECI device BAR is correct and device is enabled.
    //
    HeciMBAR = CheckAndFixHeciForAccess (HECI1_DEVICE);

    //
    // Make sure we do not have a HECI reset
    //
    if (CheckForHeciReset ()) {
      //
      // if HECI reset than try to re-init HECI
      //
      Status = HeciResetInterface(HeciDev);

      if (EFI_ERROR (Status)) {
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
    //
    // Make sure that HECI is ready for communication.
    //
    if (WaitForMEReady (HECI1_DEVICE) != EFI_SUCCESS) {
      Status = EFI_TIMEOUT;
      break;
    }
    while ((CurrentLength < *Length) && (MessageComplete == 0))
    {
      PacketBuffer = *Length - CurrentLength;
      ReadError = HECIPacketRead (
                    Blocking,
                    &PacketHeader,
                    (UINT32 *) &MessageBody[CurrentLength / 4],
                    &PacketBuffer
                    );

      //
      // Check for error condition on read
      //
      if (EFI_ERROR (ReadError)) {
        *Length   = 0;
        Status    = ReadError;
        QuitFlag  = TRUE;
        break;
      }
      //
      // Get completion status from the packet header
      //
      MessageComplete = PacketHeader.Fields.MessageComplete;

      //
      // Check for zero length messages
      //
      if (PacketBuffer == 0) {
        //
        // If we are not in the middle of a message, and we see Message Complete,
        // this is a valid zero-length message.
        //
        if ((CurrentLength == 0) && (MessageComplete == 1)) {
          *Length   = 0;
          Status    = EFI_SUCCESS;
          QuitFlag  = TRUE;
          break;
        } else {
          //
          // We should not expect a zero-length message packet except as described above.
          //
          *Length   = 0;
          Status    = EFI_DEVICE_ERROR;
          QuitFlag  = TRUE;
          break;
        }
      }
      //
      // Track the length of what we have read so far
      //
      CurrentLength += PacketBuffer;

    }

    if (QuitFlag == TRUE) {
      break;
    }
    //
    // If we get here the message should be complete, if it is not
    // the caller's buffer was not large enough.
    //
    if (MessageComplete == 0) {
      *Length = 0;
      Status  = EFI_BUFFER_TOO_SMALL;
      break;
    }
    *Length = CurrentLength;
  } while (EFI_ERROR (Status));

  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[HECI] Receive failed (%r)\n", Status));
  }
  return Status;
}


/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED      Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSend (
  IN HECI_DEVICE  HeciDev,
  IN UINT32      *Message,
  IN UINT32       Length,
  IN UINT8        HostAddress,
  IN UINT8        MeAddress
  )
{
  EFI_STATUS      Status;

  ASSERT(HeciDev == HECI1_DEVICE);
  
  Status = HeciSendMsg (Message, Length, HostAddress, MeAddress);

  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "[HECI] Send msg %02X -> %02X failed (%r)\n", HostAddress, MeAddress, Status));
  }
  return Status;
}


/**

    Function sends one message (of any length) through the HECI circular buffer.

    @param Message     - Pointer to the message data to be sent.
    @param Length      - Length of the message in bytes.
    @param HostAddress - The address of the host processor.
    @param MeAddress   - Address of the ME subsystem the message is being sent to.

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendMsg (
  IN UINT32                     *Message,
  IN UINT32                     Length,
  IN UINT8                      HostAddress,
  IN UINT8                      MeAddress
  )
{
  UINT32                      CBLength;
  UINT32                      SendLength;
  UINT32                      CurrentLength;
  HECI_MESSAGE_HEADER         MessageHeader;
  EFI_STATUS                  WriteStatus;
  EFI_STATUS                  Status;
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  UINTN                       HeciMBAR;

  CurrentLength = 0;
  Status        = EFI_SUCCESS;

  do {
    //
    // Make sure that HECI device BAR is correct and device is enabled.
    //
    HeciMBAR = CheckAndFixHeciForAccess (HECI1_DEVICE);

    //
    // Make sure we do not have a HECI reset
    //
    if (CheckForHeciReset ()) {
      //
      // if HECI reset than try to re-init HECI
      //
      Status = HeciResetInterface(HECI1_DEVICE);
      if (EFI_ERROR (Status)) {
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
    //
    // Make sure that HECI is ready for communication.
    //
    if (WaitForMEReady (HECI1_DEVICE) != EFI_SUCCESS) {
      Status = EFI_TIMEOUT;
      break;
    }
    //
    // Set up memory mapped registers
    //
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);

    //
    // Grab Circular Buffer length
    //
    CBLength = HeciRegHCsr.r.H_CBD;

    //
    // Prepare message header
    //
    MessageHeader.Data                = 0;
    MessageHeader.Fields.MeAddress    = MeAddress;
    MessageHeader.Fields.HostAddress  = HostAddress;

    //
    // Break message up into CB-sized packets and loop until completely sent
    //
    while (Length > CurrentLength) {
      //
      // Set the Message Complete bit if this is our last packet in the message.
      // Needs to be 'less than' to account for the header.
      //
      if ((((Length - CurrentLength) + 3) / 4) < CBLength) {
        MessageHeader.Fields.MessageComplete = 1;
      }
      //
      // Calculate length for Message Header
      //    header length == smaller of circular buffer or remaining message (both account for the size of the header)
      //
      SendLength = ((CBLength <= (((Length - CurrentLength) + 3) / 4)) ? ((CBLength - 1) * 4) : (Length - CurrentLength));
      MessageHeader.Fields.Length = SendLength;
      //
      // send the current packet (CurrentLength can be treated as the index into the message buffer)
      //
      WriteStatus = HeciPacketWrite (&MessageHeader, (UINT32 *) ((UINTN) Message + CurrentLength));
      if (EFI_ERROR (WriteStatus)) {
        Status = WriteStatus;
        break;
      }
      //
      // Update the length information
      //
      CurrentLength += SendLength;
    }

    if (EFI_ERROR (Status)) {
      break;
    }
  } while (EFI_ERROR (Status));

  return Status;
}

/**
   Function sends one message packet through the HECI circular buffer
   Corresponds to HECI HPS (part of) section 4.2.3
    @param MessageHeader - Pointer to the message header.
    @param MessageData   - Pointer to the actual message data.
    @retval EFI_STATUS
    @retval EFI_TIMEOUT - timeout waiting for ME
    @retval EFI_DEVICE_ERROR - too many resets occurred
**/
EFI_STATUS
HeciPacketWrite (
  IN  HECI_MESSAGE_HEADER       *MessageHeader,
  IN  UINT32                    *MessageData
  )
{
  UINT32                      Timeout;
  UINT32                      EmptySlots;
  UINT32                      Retries;
  UINT32                      i;
  UINT32                      LengthInDwords;
  HECI_HOST_CONTROL_REGISTER  HCsr;
  HECI_ME_CONTROL_REGISTER    MeCsr;
  UINTN                       HeciMBAR = CheckAndFixHeciForAccess(HECI1_DEVICE);

  HeciTrace("Send msg: ", MessageHeader->Data, (UINT8*)MessageData, MessageHeader->Fields.Length);
  //
  // Compute message length in DWORDS
  //
  LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);
  //
  // Set up timeout counter
  //
  Timeout = (HECI_SEND_TIMEOUT + HECI_WAIT_DELAY / 2) / HECI_WAIT_DELAY;
  Retries = HECI_MAX_RETRY + 1;
  while (Retries > 0) {
    MeCsr.ul    = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
    HCsr.ul     = MMIOREADDWORD (HeciMBAR + H_CSR);
    EmptySlots  = (UINT8) HCsr.r.H_CBD - (UINT8) ((INT8) HCsr.r.H_CBWP - (INT8) HCsr.r.H_CBRP);
    //
    // If HECI not ready or in buffer overrun reset it and try again.
    //
    if (!MeCsr.r.ME_RDY_HRA || !HCsr.r.H_RDY || EmptySlots > HCsr.r.H_CBD) {
      if (HeciResetInterface(HECI1_DEVICE) != EFI_SUCCESS) {
        return EFI_TIMEOUT;
      }

      Retries--;
      continue;
    }
    //
    // Wait until there is sufficient room in the circular buffer.
    // Must have room for message data and header.
    //
    if (LengthInDwords < EmptySlots) {
      //
      // Write message header first then data
      //
      MMIOWRITEDWORD (HeciMBAR + H_CB_WW, MessageHeader->Data);
      for (i = 0; i < LengthInDwords; i++) {
        MMIOWRITEDWORD (HeciMBAR + H_CB_WW, MessageData[i]);
      }
      //
      // Check if ME is not ready after writing reset HECI
      // and retry the message.
      //
      MeCsr.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
      if (!MeCsr.r.ME_RDY_HRA) {
        continue;
      }
      //
      // If ME is still ready set interrupt generate bit and we are done.
      //
      HCsr.ul     = MMIOREADDWORD (HeciMBAR + H_CSR);
      HCsr.r.H_IG = 1;
      MMIOWRITEDWORD (HeciMBAR + H_CSR, HCsr.ul);
      return EFI_SUCCESS;
    }

    if (Timeout-- == 0) {
      return EFI_TIMEOUT;
    }
    MicroSecondDelay(HECI_WAIT_DELAY);
  }
  //
  // If we got here to many retries happened.
  //
  return EFI_DEVICE_ERROR;
} // HeciPacketWrite()


/**

   Function sends one message packet through the HECI circular buffer
   Corresponds to HECI HPS (part of) section 4.2.3

    @param MessageHeader - Pointer to the message header.
    @param MessageData   - Pointer to the actual message data.

    @retval EFI_STATUS

**/
EFI_STATUS
UnPatchHeciPacketWrite (
  IN  HECI_MESSAGE_HEADER       *MessageHeader,
  IN  UINT32                    *MessageData
  )
{
  UINT32                      Timeout;
  UINT32                      i;
  UINT32                      LengthInDwords;
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER    HeciRegMeCsrHa;
  UINTN                       HeciMBAR;

  //
  //  VOLATILE HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr     = (VOID*)(HeciMBAR + H_CSR);
  //  VOLATILE HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr  = (VOID*)(HeciMBAR + ME_CSR_HA);
  //  VOLATILE UINT32                     *HeciRegHCbwwPtr    = (VOID*)(HeciMBAR + H_CB_WW);
  //
  // Make sure that HECI is ready for communication.
  //
  if (WaitForMEReady (HECI1_DEVICE) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  //
  // Set up timeout counter
  //
  Timeout = (HECI_SEND_TIMEOUT + HECI_WAIT_DELAY / 2) / HECI_WAIT_DELAY;

  //
  // Compute message length in DWORDS
  //
  LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

  //
  // Wait until there is sufficient room in the circular buffer
  // Must have room for message and message header
  //
  HeciMBAR = CheckAndFixHeciForAccess(HECI1_DEVICE);
  while (1)
  {
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
    //
    // Check if we got enough empty slots for message
    //
    if (LengthInDwords <= (HeciRegHCsr.r.H_CBD - FilledSlots(HeciRegHCsr.r.H_CBRP, HeciRegHCsr.r.H_CBWP)))
    {
      break; // got space for the message
    }
    if (Timeout-- == 0)
    {
      return EFI_TIMEOUT;
    }
    //
    // Wait before we read the register again
    //
    MicroSecondDelay(HECI_WAIT_DELAY);
  }
  //
  // Write Message Header
  //
  MMIOWRITEDWORD (HeciMBAR + H_CB_WW, MessageHeader->Data);

  //
  // Write Message Body
  //
  for (i = 0; i < LengthInDwords; i++) {
    MMIOWRITEDWORD (HeciMBAR + H_CB_WW, MessageData[i]);
  }
  //
  // Set Interrupt Generate bit
  //
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IG  = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  //
  // Test if ME Ready bit is set to 1, if set to 0 a fatal error occurred during
  // the transmission of this message.
  //
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  if (HeciRegMeCsrHa.r.ME_RDY_HRA == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Function sends one message through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in][out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSendwAck (
  IN      HECI_DEVICE  HeciDev,
  IN OUT  UINT32      *Message,
  IN      UINT32       Length,
  IN OUT  UINT32      *RecLength,
  IN      UINT8        HostAddress,
  IN      UINT8        MeAddress
  )
{
  EFI_STATUS           Status;
  UINT16               RetryCount;
  UINT32               TempRecLength;
  UINTN                HeciMbar;
  HECI_ME_CONTROL_REGISTER MeCsr;

  Status = EFI_SUCCESS;
  TempRecLength = 0;
  if (RecLength == NULL) {
    RecLength = &Length;
  }
  HeciMbar = CheckAndFixHeciForAccess(HeciDev);
  //
  // We start a new transaction. If the receive queue is not empty, delete it because
  // the content was to be interpreted as a response.
  //
  MeCsr.ul = MMIOREADDWORD(HeciMbar + ME_CSR_HA);
  if (FilledSlots(MeCsr.r.ME_CBRP_HRA, MeCsr.r.ME_CBWP_HRA) > 0)
  {
    Status = HeciResetInterface(HeciDev);
    if (EFI_ERROR(Status))
    {
      return Status;
    }
  }

  for (RetryCount = 0; RetryCount < HECI_MAX_RETRY; RetryCount++) {
    ///
    /// Send the message
    ///
    Status = HeciSend (HeciDev, Message, Length, HostAddress, MeAddress);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ///
    /// Reload receive length as it has been modified by the read function
    ///
    TempRecLength = *RecLength;
    ///
    /// Read Message
    ///
    Status = HeciReceive (HeciDev, BLOCKING, Message, &TempRecLength);
    if (!EFI_ERROR (Status)) {
      break;
    }

    DEBUG ((
      DEBUG_ERROR,
      "HECI%d SendwAck: Retrying after %x failed attempt - Status = %r\n",
      HECI_NAME_MAP (HeciDev),
      RetryCount + 1,
      Status
      ));
  }
  ///
  /// Return read length and status
  ///
  *RecLength = TempRecLength;
  return Status;
}


/**
  Me reset and waiting for ready

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Delay                The biggest waiting time

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Me is ready
**/
EFI_STATUS
EFIAPI
HeciMeResetWait (
  IN  HECI_DEVICE             HeciDev,
  IN  UINT32                  Delay
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  UINTN                       HeciMBAR;

  ASSERT(HeciDev == HECI1_DEVICE);
  //
  // Make sure that HECI device BAR is correct and device is enabled.
  //
  HeciMBAR = CheckAndFixHeciForAccess (HECI1_DEVICE);

  //
  // Wait for the HOST Ready bit to be cleared to signal a reset
  //
  while (1)
  {
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
    if (!HeciRegHCsr.r.H_RDY)
    {
      break;
    }
    //
    // If timeout has expired, return fail
    //
    if (Delay-- == 0)
    {
      return EFI_TIMEOUT;
    }
    MicroSecondDelay(1);
  }

  return EFI_SUCCESS;
}


/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
HeciResetInterface(
  IN  HECI_DEVICE             HeciDev)
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER    HeciRegMeCsrHa;
  UINT32                      Timeout = (HECI_INIT_TIMEOUT + HECI_WAIT_DELAY / 2) / HECI_WAIT_DELAY;
  UINTN                       HeciMBAR;

  ASSERT(HeciDev == HECI1_DEVICE);
  //
  // Make sure that HECI device BAR is correct and device is enabled.
  //
  HeciMBAR = CheckAndFixHeciForAccess (HECI1_DEVICE);

  if (((HeciMBAR & 0xFFFFFFFF) == 0xFFFFFFFF) ||
       (HeciMBAR == 0)) {
    DEBUG((DEBUG_ERROR, "[HECI] Illegal MBAR 0x%x\n", HeciMBAR));
    return EFI_UNSUPPORTED;
  }

  HeciRegHCsr.ul = MMIOREADDWORD(HeciMBAR + H_CSR);
  HeciRegMeCsrHa.ul = MMIOREADDWORD(HeciMBAR + ME_CSR_HA);
  
  DEBUG((DEBUG_WARN, "[HECI] Resetting HECI interface (CSR: %08X/%08X, MEFS1:%08X)\n",
         HeciRegHCsr.ul, HeciRegMeCsrHa.ul, HeciPciRead32(R_FWSTATE)));
  //
  // Enable reset bit in host CSR, unless it is already set. This function can be called
  // after timeout in previews call. Then it should just continue, not triggering new reset.
  //
  if (!HeciRegHCsr.r.H_RST)
  {
    HeciRegHCsr.r.H_RST = 1;
    HeciRegHCsr.r.H_IG  = 1;
    MMIOWRITEDWORD(HeciMBAR + H_CSR, HeciRegHCsr.ul);
  }
  //
  // Wait for host ready bit cleared by HECI hardware
  //
  while (1)
  {
    HeciRegHCsr.ul = MMIOREADDWORD(HeciMBAR + H_CSR);
    if (!HeciRegHCsr.r.H_RDY)
    {
      break;
    }
    if (Timeout-- == 0)
    {
      DEBUG((DEBUG_ERROR, "[HECI] HECI reset failed on host side (CSR: %08X/%08X, MEFS1: %08X)\n",
            HeciRegHCsr.ul, MMIOREADDWORD(HeciMBAR + ME_CSR_HA), HeciPciRead32(R_FWSTATE)));
      return EFI_TIMEOUT;
    }
    MicroSecondDelay(HECI_WAIT_DELAY);
  }
  //
  // Wait for ME to get ready after reset procedure and signal interrupt to host
  //
  while (1)
  {
    HeciRegMeCsrHa.ul = MMIOREADDWORD(HeciMBAR + ME_CSR_HA);
    if (HeciRegMeCsrHa.r.ME_RDY_HRA)
    {
      break;
    }
    if (Timeout-- == 0)
    {
      DEBUG((DEBUG_ERROR, "[HECI] HECI reset failed on ME side (CSR: %08X/%08X, MEFS1: %08X)\n",
             MMIOREADDWORD(HeciMBAR + H_CSR), HeciRegMeCsrHa.ul, HeciPciRead32(R_FWSTATE)));
      return EFI_TIMEOUT;
    }
    MicroSecondDelay(HECI_WAIT_DELAY);
  }

  //
  // Enable host side interface
  //
  HeciRegHCsr.ul      = MMIOREADDWORD(HeciMBAR + H_CSR);;
  HeciRegHCsr.r.H_RST = 0;
  HeciRegHCsr.r.H_IG  = 1;
  HeciRegHCsr.r.H_RDY = 1;
  MMIOWRITEDWORD(HeciMBAR + H_CSR, HeciRegHCsr.ul);

  return EFI_SUCCESS;
}


/**

    Calculate if the circular buffer has overflowed.
    Corresponds to HECI HPS (part of) section 4.2.1

    @param ReadPointer  - Location of the read pointer.
    @param WritePointer - Location of the write pointer.

    @retval Number of filled slots.

**/
UINT8
FilledSlots (
  IN  UINT32 ReadPointer,
  IN  UINT32 WritePointer
  )
{
  UINT8 FilledSlots;

  //
  // Calculation documented in HECI HPS 0.68 section 4.2.1
  //
  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  return FilledSlots;
}

/**

    Calculate if the circular buffer has overflowed
    Corresponds to HECI HPS (part of) section 4.2.1

    @param ReadPointer - Value read from host/me read pointer
    @param WritePointer - Value read from host/me write pointer
    @param BufferDepth - Value read from buffer depth register

    @retval EFI_STATUS

**/
EFI_STATUS
OverflowCB (
  IN  UINT32 ReadPointer,
  IN  UINT32 WritePointer,
  IN  UINT32 BufferDepth
  )
{
  UINT8 FilledSlots;

  //
  // Calculation documented in HECI HPS 0.68 section 4.2.1
  //
  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  //
  // test for overflow
  //
  if (FilledSlots > ((UINT8) BufferDepth)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**

    Return ME Status

    @param MeStatus pointer for status report

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  IN UINT32                     *MeStatus
  )
{
  HECI_FWS_REGISTER MeFirmwareStatus;

  if (MeStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);

  switch (MeFirmwareStatus.r.CurrentState) {
  case ME_STATE_NORMAL:
    if (MeFirmwareStatus.r.ErrorCode == ME_ERROR_CODE_NO_ERROR) {
      *MeStatus = ME_READY;
    }
    break;
  case ME_STATE_RECOVERY:
    *MeStatus = ME_IN_RECOVERY_MODE;
    break;
  case ME_STATE_INIT:
    *MeStatus = ME_INITIALIZING;
    break;
  case ME_STATE_DISABLE_WAIT:
    *MeStatus = ME_DISABLE_WAIT;
    break;
  case ME_STATE_TRANSITION:
    *MeStatus = ME_TRANSITION;
    break;
  case ME_STATE_DFX_FW:
    *MeStatus = ME_INVALID;
    break;
  default:
    *MeStatus = ME_NOT_READY;
    break;
  }

  if (MeFirmwareStatus.r.FwUpdateInprogress) {
    *MeStatus |= ME_FW_UPDATES_IN_PROGRESS;
  }

  if (MeFirmwareStatus.r.FwInitComplete == ME_FIRMWARE_COMPLETED) {
    *MeStatus |= ME_FW_INIT_COMPLETE;
  }

  if (MeFirmwareStatus.r.MeBootOptionsPresent == ME_BOOT_OPTIONS_PRESENT) {
    *MeStatus |= ME_FW_BOOT_OPTIONS_PRESENT;
  }

  DEBUG((EFI_D_INFO, "[HECI] MEFS1 %08X -> MeStatus %X\n", MeFirmwareStatus.ul, *MeStatus));

  return EFI_SUCCESS;
}


/**
  Return ME Mode

  @param[out] MeMode              Pointer for ME Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32                    *MeMode
  )
{
  HECI_FWS_REGISTER MeFirmwareStatus;

  if (MeMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);
  //
  // Check if it's DFX ME FW first
  //
  if (MeFirmwareStatus.r.CurrentState == ME_STATE_DFX_FW) {
    *MeMode = ME_MODE_DFX_FW;
  } else {
    switch (MeFirmwareStatus.r.MeOperationMode) {
      case ME_OPERATION_MODE_NORMAL:
    case ME_OPERATION_MODE_SPS:
        *MeMode = ME_MODE_NORMAL;
        break;

      case ME_OPERATION_MODE_DEBUG:
        *MeMode = ME_MODE_DEBUG;
        break;

      case ME_OPERATION_MODE_SOFT_TEMP_DISABLE:
        *MeMode = ME_MODE_TEMP_DISABLED;
        break;

      case ME_OPERATION_MODE_SECOVR_JMPR:
      case ME_OPERATION_MODE_SECOVR_HECI_MSG:
        *MeMode = ME_MODE_SECOVER;
        break;

      default:
        *MeMode = ME_MODE_FAILED;
    }
  }
  DEBUG((EFI_D_INFO, "[HECI] MEFS1: %08X -> MeMode %d\n", MeFirmwareStatus.ul, *MeMode));

  return EFI_SUCCESS;
}
