/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
 **/
/*++

Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  IeHeciCore.c

Abstract:

  IE HECI core library. Used in PEI, DXE and SMM to implement IE HECI driver
  appropriate for given environment.

 **/
#include <PiPei.h>
#include <Uefi.h>

#include <Library/IoLib.h>
#include <Library/MmPciBaseLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/IeHeciCoreLib.h>


/*****************************************************************************
 * Local definitions.
 *****************************************************************************/
#ifndef VOLATILE
#define VOLATILE volatile
#endif

/*
 * HECI MBAR registers definition
 *
 * This structure defines registers mapped at HECI MBAR.
 */
typedef struct {
  VOLATILE UINT32 CB_WW;   // Circular Buffer Write Window
  VOLATILE UINT32 H_CSR;   // Host Control and Status Register
  VOLATILE UINT32 CB_RW;   // Circular Buffer Read Window
  VOLATILE UINT32 IE_CSR;  // IE Control and Status Register (read only)
} HECI_MBAR_REGS;


/*
 * HECI_MBAR_REGS::H_CSR - Host Control and Status Register
 */
typedef union {
  UINT32    DWord;
  struct {
    UINT32  H_IE    : 1,  // 0 - Host Interrupt Enable IE
            H_IS    : 1,  // 1 - Host Interrupt Status IE
            H_IG    : 1,  // 2 - Host Interrupt Generate
            H_RDY   : 1,  // 3 - Host Ready
            H_RST   : 1,  // 4 - Host Reset
            Reserved: 3,  // 7:5
            H_CBRP  : 8,  // 15:8 - Host CB Read Pointer
            H_CBWP  : 8,  // 23:16 - Host CB Write Pointer
            H_CBD   : 8;  // 31:24 - Host Circular Buffer Depth
  } Bits;
} HECI_HOST_CSR;

/*
 * HECI_MBAR_REGS::IE_CSR - IE Control and Status Register
 */
typedef union {
  UINT32   DWord;
  struct {
    UINT32 IE_IE   : 1,  // 0 - IE Interrupt Enable (Host Read Access)
           IE_IS   : 1,  // 1 - IE Interrupt Status (Host Read Access)
           IE_IG   : 1,  // 2 - IE Interrupt Generate (Host Read Access)
           IE_RDY  : 1,  // 3 - IE Ready (Host Read Access)
           IE_RST  : 1,  // 4 - IE Reset (Host Read Access)
           Reserved: 3,  // 7:5
           IE_CBRP : 8,  // 15:8 - IE CB Read Pointer (Host Read Access)
           IE_CBWP : 8,  // 23:16 - IE CB Write Pointer (Host Read Access)
           IE_CBD  : 8;  // 31:24 - IE Circular Buffer Depth (Host Read Access)
  } Bits;
} HECI_IE_CSR;


/*****************************************************************************
 * Local function prototypes.
 *****************************************************************************/
VOID* HeciMbarRead(IN IE_HECI_DEVICE *pThis);
VOID  HeciTrace(IN IE_HECI_DEVICE*, IN CHAR8*, IN HECI_MSG_HEADER*, IN INT32);


/*****************************************************************************
 * Public functions.
 *****************************************************************************/

/**
  Initialize HECI interface.
 
  This function initializes host side of HECI interface. If timeout is
  greater than zero it also waits until IE is ready to receive messages.
 
  @param[in,out] pThis     Pointer to HECI device structure
  @param[in,out] pTimeout  On input timeout in ms, on exit time left
 **/
EFI_STATUS EFIAPI
HeciInit(
  IN OUT IE_HECI_DEVICE   *pThis,
  IN OUT UINT32           *pTimeout)
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT32          Timeout = 0;
  HECI_HOST_CSR   HCsr;
  HECI_IE_CSR     IeCsr;
  HECI_MBAR_REGS *pMbarRegs;
  
  if (pThis == NULL || (pThis->Mbar & 0xF) != 0 || pThis->Hidm > HECI_HIDM_LAST)
  {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }
  if (pTimeout != NULL)
  {
    Timeout = *pTimeout;
  }
  //
  // Store HECI vendor and device information away
  //
  pThis->PciCfg = MmPciBase(pThis->Bus, pThis->Dev, pThis->Fun);
  pThis->Vid = MmioRead16(pThis->PciCfg + HECI_R_VENDORID);
  pThis->Did = MmioRead16(pThis->PciCfg + HECI_R_DEVICEID);
  DEBUG((EFI_D_INFO, "[IE_HECI-%d] VID-DID: %2X-%2X\n", pThis->Fun, pThis->Vid, pThis->Did));
  if (pThis->Vid != 0x8086)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Init failed, PCI device %d/%d/%d not valid HECI (%2X-%2X)\n",
           pThis->Bus, pThis->Bus, pThis->Dev, pThis->Fun, pThis->Vid, pThis->Did));
    return EFI_DEVICE_ERROR;
  }
  //
  // Check MBAR, whether it is configured. If not then check if user provided
  // some address to configure in such a case.
  //
  pMbarRegs = (HECI_MBAR_REGS*)HeciMbarRead(pThis);
  if (pMbarRegs == NULL)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Init failed (device disabled)\n", pThis->Fun));
    ASSERT(FALSE);
    Status = EFI_DEVICE_ERROR;
    goto GetOut;
  }
  //
  // Make sure HECI interrupts are disabled before configuring delivery mode.
  //
  HCsr.DWord = pMbarRegs->H_CSR;
  HCsr.Bits.H_IE = 0;
  pMbarRegs->H_CSR = HCsr.DWord;
  //
  // Set HECI interrupt delivery mode.
  //
  MmioWrite8(pThis->PciCfg + HECI_R_HIDM, pThis->Hidm);
  //
  // If HECI was in use reset it to clear queues.
  //
  IeCsr.DWord = pMbarRegs->IE_CSR;
  if (!IeCsr.Bits.IE_RDY)
  {
    Status = HeciQueReset(pThis, &Timeout);
  }
  else
  {
    if (!HCsr.Bits.H_RDY)
    {
      HCsr.Bits.H_IG = 1;
      HCsr.Bits.H_RDY = 1;
      HCsr.Bits.H_RST = 0;
      pMbarRegs->H_CSR = HCsr.DWord;
    }
    pThis->HMtu = HCsr.Bits.H_CBD * sizeof(UINT32) - sizeof(HECI_MSG_HEADER);
    pThis->IeMtu = IeCsr.Bits.IE_CBD * sizeof(UINT32) - sizeof(HECI_MSG_HEADER);
  }
  
 GetOut:
  if (pTimeout != NULL)
  {
    *pTimeout = Timeout;
  }
  pThis->Iefs1.DWord = IeHeciPciReadIefs1();
  return Status;
} // HeciInit()


/**
  Set HECI interrupt enable bit in HECI CSR.
 
  This function sets HECI interrupt enable bit. It may enable or disable HECI
  interrupt.
 
  NOTE: It is dedicated for SMM. DXE and PEI drivers must not use interrupt.
 
  @param[in,out] pThis      Pointer to HECI device structure
  @param[in]     IntEnabled TRUE enables HECI interrupt, FALSE disables
 **/
EFI_STATUS EFIAPI
HeciIntEnable(
  IN OUT IE_HECI_DEVICE   *pThis,
     OUT BOOLEAN       IntEnabled)
{
  HECI_HOST_CSR   HCsr;
  HECI_MBAR_REGS *pMbarRegs;
  
  if (pThis == NULL)
  {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check for HECI availability on PCI
  //
  pMbarRegs = (HECI_MBAR_REGS*)HeciMbarRead(pThis);
  if (pMbarRegs == NULL)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Interrupt state read failed (device disabled)\n", pThis->Fun));
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  HCsr.DWord = pMbarRegs->H_CSR;
  HCsr.Bits.H_IE = (UINT32)IntEnabled;
  HCsr.Bits.H_IS = 0; // do not clear status
  pMbarRegs->H_CSR = HCsr.DWord;
  
  pThis->Iefs1.DWord = IeHeciPciReadIefs1();
  return EFI_SUCCESS;
} // HeciIntEnable()


/**
  Get HECI interrupt state.
 
  This function reads HECI interrupt state. Whether it is enabled and whether
  it is currently signalled.
 
  @param[in,out] pThis       Pointer to HECI device structure
  @param[out]    pIntEnabled Is HECI interrupt enabled
  @param[out]    pIntStatus  Is HECI interrupt signalled
 **/
EFI_STATUS EFIAPI
HeciIntState(
  IN OUT IE_HECI_DEVICE   *pThis,
     OUT BOOLEAN       *pIntEnabled,
     OUT BOOLEAN       *pIntStatus)
{
  HECI_HOST_CSR   HCsr;
  HECI_MBAR_REGS *pMbarRegs;
  
  if (pThis == NULL)
  {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check for HECI availability on PCI
  //
  pMbarRegs = (HECI_MBAR_REGS*)HeciMbarRead(pThis);
  if (pMbarRegs == NULL)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Interrupt state read failed (device disabled)\n", pThis->Fun));
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  HCsr.DWord = pMbarRegs->H_CSR;
  if (pIntEnabled != NULL)
  {
    *pIntEnabled = (BOOLEAN)HCsr.Bits.H_IE;
  }
  if (pIntStatus != NULL)
  {
    *pIntStatus = (BOOLEAN)HCsr.Bits.H_IS;
  }
  pThis->Iefs1.DWord = IeHeciPciReadIefs1();
  return EFI_SUCCESS;
} // HeciIntState()


/**
  Reset HECI interface.
 
  This function resets HECI queue. If timeout is greater than zero it also
  waits until IE is ready to receive messages.
 
  @param[in,out] pThis      Pointer to HECI device structure
  @param[in,out] pTimeout   On input timeout in ms, on exit time left
 **/
EFI_STATUS EFIAPI
HeciQueReset(
  IN OUT IE_HECI_DEVICE   *pThis,
  IN OUT UINT32        *pTimeout)
{
  EFI_STATUS      Status;
  UINT32          Timeout = 0;
  HECI_HOST_CSR   HCsr;
  HECI_IE_CSR     IeCsr;
  HECI_MBAR_REGS *pMbarRegs;
  
  if (pThis == NULL)
  {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check for HECI availability on PCI
  //
  pMbarRegs = (HECI_MBAR_REGS*)HeciMbarRead(pThis);
  if (pMbarRegs == NULL)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Reset failed (device disabled)\n", pThis->Fun));
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  if (pTimeout != NULL)
  {
    Timeout = *pTimeout;
  }
  DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Resetting HECI interface (CSR %X/%X)\n",
         pThis->Fun, pMbarRegs->H_CSR, pMbarRegs->IE_CSR));
  
  HCsr.DWord = pMbarRegs->H_CSR;
  if (!HCsr.Bits.H_RST)
  {
    HCsr.Bits.H_RST = 1;
    HCsr.Bits.H_IG = 1;
    pMbarRegs->H_CSR = HCsr.DWord;
  }
  //
  // Wait for H_RDY cleared to make sure that the reset started.
  //
  while (1)
  {
    HCsr.DWord = pMbarRegs->H_CSR;
    if (!HCsr.Bits.H_RDY)
    {
      break;
    }
    if (Timeout == 0)
    {
      DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Reset failed (timeout)(CSR %X/%X)\n",
             pThis->Fun, pMbarRegs->H_CSR, pMbarRegs->IE_CSR));
      Status = EFI_TIMEOUT;
      goto GetOut;
    }
    MicroSecondDelay(HECI_TIMEOUT_UNIT);
    Timeout--;
  }
  //
  // Wait for IE to perform reset and signal it is ready.
  //
  while (1)
  {
    IeCsr.DWord = pMbarRegs->IE_CSR;
    if (IeCsr.Bits.IE_RDY)
    {
      break;
    }
    if (Timeout == 0)
    {
      DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Reset failed (timeout)(CSR %X/%X)\n",
             pThis->Fun, pMbarRegs->H_CSR, pMbarRegs->IE_CSR));
      Status = EFI_TIMEOUT;
      goto GetOut;
    }
    MicroSecondDelay(HECI_TIMEOUT_UNIT);
    Timeout--;
  }
  //
  // IE side is ready, signal host side is ready too.
  //
  HCsr.DWord = pMbarRegs->H_CSR;
  HCsr.Bits.H_RST = 0;
  HCsr.Bits.H_RDY = 1;
  HCsr.Bits.H_IG = 1;
  pMbarRegs->H_CSR = HCsr.DWord;
  //
  // Update MTU, IE could change it during reset.
  //
  pThis->HMtu = HCsr.Bits.H_CBD * sizeof(UINT32) - sizeof(HECI_MSG_HEADER);
  pThis->IeMtu = IeCsr.Bits.IE_CBD * sizeof(UINT32) - sizeof(HECI_MSG_HEADER);
  Status = EFI_SUCCESS;
  
 GetOut:
  if (pTimeout != NULL)
  {
    *pTimeout = Timeout;
  }
  pThis->Iefs1.DWord = IeHeciPciReadIefs1();
  return Status;
} // HeciQueReset()


/**
  Get HECI queue state.
 
  This function reads HECI queue state. Whether it is ready for communication
  and whether there are any messages in the queue.
 
  @param[in,out] pThis     Pointer to HECI device structure
  @param[out]    pIsReady  Is HECI ready for communication
  @param[out]    pSendQue  Number of DWords in send queue
  @param[out]    pRecvQue  Number of DWords in receive queue
 **/
EFI_STATUS EFIAPI
HeciQueState(
  IN OUT IE_HECI_DEVICE   *pThis,
     OUT BOOLEAN       *pIsReady,
     OUT UINT32        *pSendQue,
     OUT UINT32        *pRecvQue)
{
  HECI_HOST_CSR   HCsr;
  HECI_IE_CSR     IeCsr;
  HECI_MBAR_REGS *pMbarRegs;
  
  if (pThis == NULL)
  {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check for HECI availability on PCI
  //
  pMbarRegs = (HECI_MBAR_REGS*)HeciMbarRead(pThis);
  if (pMbarRegs == NULL)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Queue state read failed (device disabled)\n", pThis->Fun));
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  HCsr.DWord = pMbarRegs->H_CSR;
  IeCsr.DWord = pMbarRegs->IE_CSR;
  if (pIsReady != NULL)
  {
    *pIsReady = HCsr.Bits.H_RDY && IeCsr.Bits.IE_RDY;
  }
  if (pSendQue != NULL)
  {
    *pSendQue = (UINT8)((INT8)HCsr.Bits.H_CBWP - (INT8)HCsr.Bits.H_CBRP);
  }
  if (pRecvQue != NULL)
  {
    *pRecvQue = (UINT8)((INT8)IeCsr.Bits.IE_CBWP - (INT8)IeCsr.Bits.IE_CBRP);
  }
  pThis->Iefs1.DWord = IeHeciPciReadIefs1();
  return EFI_SUCCESS;
} // HeciQueState()


/**
  Write one message to HECI queue.
 
  This function puts one message to HECI queue. If timeout is greater than
  zero the function may wait for space in the queue. This function handles
  only messages shorter than HECI queue length. Fragmentation of large
  messages must be done by upper layer protocol.
 
  @param[in,out] pThis      Pointer to HECI device structure
  @param[in,out] pTimeout   On input timeout in ms, on exit time left
  @param[in]     pMessage   The header of the message to send
 **/
EFI_STATUS EFIAPI
HeciMsgSend(
  IN OUT IE_HECI_DEVICE     *pThis,
  IN OUT UINT32          *pTimeout,
  IN     HECI_MSG_HEADER *pMessage)
{
  EFI_STATUS      Status;
  UINT32          Timeout = 0;
  UINT8           EmptySlots;
  UINT8           i, MsgDWordLen;
  HECI_HOST_CSR   HCsr;
  HECI_IE_CSR     IeCsr;
  HECI_MBAR_REGS *pMbarRegs;
  
  if (pThis == NULL || pMessage == NULL)
  {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }
  HeciTrace(pThis, "Send msg: ", pMessage, sizeof(HECI_MSG_HEADER) + pMessage->Bits.Length);
  //
  // Check for HECI availability on PCI
  //
  pMbarRegs = (HECI_MBAR_REGS*)HeciMbarRead(pThis);
  if (pMbarRegs == NULL)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Send failed (device disabled)\n", pThis->Fun));
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  if (pTimeout != NULL)
  {
    Timeout = *pTimeout;
  }
  //
  // Compute message length in double words (write window register size)
  //
  MsgDWordLen = (UINT8)((pMessage->Bits.Length + sizeof(UINT32) - 1) / sizeof(UINT32));
  MsgDWordLen++; // One more double word for message header
  while (1)
  {
    HCsr.DWord = pMbarRegs->H_CSR;
    IeCsr.DWord = pMbarRegs->IE_CSR;
    //
    // If message is bigger than queue length refuse it.
    //
    if (MsgDWordLen > HCsr.Bits.H_CBD)
    {
      DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Send failed (msg %d B, queue %d B only)\n",
            pThis->Fun, pMessage->Bits.Length, HCsr.Bits.H_CBD * sizeof(UINT32)));
      Status = EFI_BAD_BUFFER_SIZE;
      goto GetOut;
    }
    EmptySlots = (UINT8)HCsr.Bits.H_CBD -
                 (UINT8)((INT8)HCsr.Bits.H_CBWP - (INT8)HCsr.Bits.H_CBRP);
    //
    // If IE or host side is not ready for communication,
    // or buffer overflow occured reset the interface.
    //
    if (!IeCsr.Bits.IE_RDY || !HCsr.Bits.H_RDY || EmptySlots > HCsr.Bits.H_CBD)
    {
      Status = HeciQueReset(pThis, &Timeout);
      if (EFI_ERROR(Status))
      {
        goto GetOut;
      }
      continue;
    }
    //
    // If we got space in the queue for the whole message write it.
    //
    if (MsgDWordLen <= EmptySlots)
    {
      for (i = 0; i < MsgDWordLen; i++)
      {
        pMbarRegs->CB_WW = ((UINT32*)pMessage)[i];
      }
      //
      // Check if IE is still ready after writing message.
      // If not reset HECI and retry the message.
      //
      IeCsr.DWord = pMbarRegs->IE_CSR;
      if (!IeCsr.Bits.IE_RDY)
      {
        DEBUG((EFI_D_WARN, "[IE_HECI-%d] Queue has been reset while sending\n", pThis->Fun));
        continue;
      }
      //
      // If IE is still ready set interrupt generate bit and we are done.
      //
      HCsr.DWord = pMbarRegs->H_CSR;
      HCsr.Bits.H_IS = 0; // don't clear interrupt status if set
      HCsr.Bits.H_IG = 1;
      pMbarRegs->H_CSR = HCsr.DWord;
      Status = EFI_SUCCESS;
      goto GetOut;
    }
    //
    // Will wait until there is sufficient room in the circular buffer,
    // or timeout occures.
    //
    if (Timeout == 0)
    {
      DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Send failed (timeout)\n", pThis->Fun));
      Status = EFI_TIMEOUT;
      goto GetOut;
    }
    MicroSecondDelay(HECI_TIMEOUT_UNIT);
    Timeout--;
  }
 GetOut:
  if (pTimeout != NULL)
  {
    *pTimeout = Timeout;
  }
  pThis->Iefs1.DWord = IeHeciPciReadIefs1();
  return Status;
} // HeciMsgSend()


/**
  Read one message from HECI queue.

  This function reads one message from HECI queue. If timeout is greater than
  zero the function may wait for the message. Size of message buffer is given
  in bytes in (*pBufLen) on input. On exit (*pBufLen) provides the number of
  bytes written to the message buffer. If buffer is too short the message
  is truncated.
 
  @param[in,out] pThis      Pointer to HECI device structure
  @param[in,out] pTimeout   On input timeout in ms, on exit time left
  @param[out]    pMsgBuf    Buffer for the received message
  @param[in,out] pBufLen    On input buffer size, on exit message, in bytes
 **/
EFI_STATUS EFIAPI
HeciMsgRecv(
  IN OUT IE_HECI_DEVICE     *pThis,
  IN OUT UINT32          *pTimeout,
     OUT HECI_MSG_HEADER *pMsgBuf,
  IN     UINT32          *pBufLen)
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT32          Timeout = 0;
  UINT32          DWord, DWordReads, BufLen;
  UINT8           FilledSlots, MsgDWordLen = 0;
  HECI_HOST_CSR   HCsr;
  HECI_IE_CSR     IeCsr;
  HECI_MBAR_REGS  *pMbarRegs;
  
  if (pThis == NULL || pMsgBuf == NULL ||
      pBufLen == NULL || *pBufLen < sizeof(HECI_MSG_HEADER))
  {
    ASSERT(FALSE);
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check for HECI availability on PCI
  //
  pMbarRegs = (HECI_MBAR_REGS*)HeciMbarRead(pThis);
  if (pMbarRegs == NULL)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Receive failed (device disabled)\n", pThis->Fun));
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  if (pTimeout != NULL)
  {
    Timeout = *pTimeout;
  }
  //
  // BufLen is buffer size in bytes - value provided in *pBufLen on input
  // In the loop *pBufLen has number of bytes written to pMsgBuf.
  // DWordReads has number of double words read from HECI queue.
  //
  BufLen = *pBufLen;
  *pBufLen = DWordReads = 0;
  while (1)
  {
    HCsr.DWord = pMbarRegs->H_CSR;
    IeCsr.DWord = pMbarRegs->IE_CSR;
    
    FilledSlots = (UINT8)((INT8)IeCsr.Bits.IE_CBWP - (INT8)IeCsr.Bits.IE_CBRP);
    //
    // If IE or host side is not ready for communication, or buffer overflow occured
    // reset the HECI queue.
    //
    if (!IeCsr.Bits.IE_RDY || !HCsr.Bits.H_RDY || FilledSlots > HCsr.Bits.H_CBD)
    {
      Status = HeciQueReset(pThis, &Timeout);
      if (EFI_ERROR(Status))
      {
        goto GetOut;
      }
      continue;
    }
    //
    // Read HECI queue until we got full message or queue is empty.
    //
    while (FilledSlots-- > 0)
    {
      //
      // Read one double word from HECI queue. If we are within message buffer
      // store it in the buffer, otherwise drop it. But must read all
      // the message from HECI queue.
      //
      DWord = pMbarRegs->CB_RW;
      if (*pBufLen < BufLen)
      {
        //
        // Must handle the case when buffer size is not multiply of double word
        //
        if (DWordReads < BufLen / sizeof(UINT32))
        {
          ((UINT32*)pMsgBuf)[DWordReads] = DWord;
          *pBufLen += sizeof(UINT32);
        }
        else
        {
          switch (BufLen % sizeof(UINT32))
          {
            case 3: ((UINT8*)pMsgBuf)[*pBufLen + 2] = (UINT8)(DWord >> 16);
            case 2: ((UINT8*)pMsgBuf)[*pBufLen + 1] = (UINT8)(DWord >> 8);
            case 1: ((UINT8*)pMsgBuf)[*pBufLen + 0] = (UINT8)DWord;
          } // switch ()
          *pBufLen += BufLen % sizeof(UINT32);
        }
      }
      else
      {
        DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Message 0x%08X exceeds buffer size (%dB)\n",
               pThis->Fun, pMsgBuf[0].DWord, BufLen));
      }
      DWordReads++;
      //
      // If it is first double word it is message header and we can compute the message length.
      //
      if (MsgDWordLen == 0)
      {
        MsgDWordLen = (UINT8)((pMsgBuf[0].Bits.Length + sizeof(UINT32) - 1) / sizeof(UINT32));
        MsgDWordLen++; // One more double word for message header
        //
        // Sanity check. If message length exceeds queue length this is
        // not valid header. We are out of synch, let's reset the queue.
        //
        if (MsgDWordLen > IeCsr.Bits.IE_CBD)
        {
          DEBUG((EFI_D_ERROR, "[IE_HECI-%d] 0x%08X does not seem to be msg header, reseting...\n",
                 pThis->Fun, pMsgBuf[0].DWord));
          Status = HeciQueReset(pThis, &Timeout);
          if (EFI_ERROR(Status))
          {
            goto GetOut;
          }
          *pBufLen = DWordReads = MsgDWordLen = 0;
          break; // while (FilledSlots)
        }
      }
      //
      // If message is complete set interrupt to IE to let it know that next
      // message can be sent and exit.
      //
      if (DWordReads >= MsgDWordLen)
      {
        //
        // Check queue status before accepting the received message. If reset
        // occured while reading the message it may be corrupted. Reset queue
        // and return error in such case.
        //
        IeCsr.DWord = pMbarRegs->IE_CSR;
        HCsr.DWord = pMbarRegs->H_CSR;
        if (!IeCsr.Bits.IE_RDY)
        {
          HeciQueReset(pThis, &Timeout);
          Status = EFI_ABORTED;
        }
        else
        {
          HeciTrace(pThis, " Got msg: ", pMsgBuf, *pBufLen);
          HCsr.Bits.H_IG = 1;
          pMbarRegs->H_CSR = HCsr.DWord;
        }
        goto GetOut;
      }
    } // while (FilledSlots)
    if (Timeout == 0)
    {
      DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Receive failed (timeout)\n", pThis->Fun));
      Status = EFI_TIMEOUT;
      goto GetOut;
    }
    MicroSecondDelay(HECI_TIMEOUT_UNIT);
    Timeout--;
  }
 GetOut:
  if (pTimeout != NULL)
  {
    *pTimeout = Timeout;
  }
  pThis->Iefs1.DWord = IeHeciPciReadIefs1();
  return Status;
} // HeciMsgRecv()


/*****************************************************************************
 * Local functions.
 *****************************************************************************/
/**
  Read HECI MBAR, enable it if needed. MBAR can be disabled by PCI Enumerator.
 
  @param[in,out] pThis       Pointer to HECI device structure
  @retval 64-bit MBAR is returned, or NULL if MBAR is not configured or not enabled.
 **/
VOID*
HeciMbarRead(
  IN OUT IE_HECI_DEVICE *pThis)
{
  UINT16 Cmd;
  union
  {
    UINT64   QWord;
    struct
    {
      UINT32 DWordL;
      UINT32 DWordH;
    } Bits;
  } Mbar;
  
  //
  // Read MBAR. Handle 64-bit case if bit 4 is set.
  // If MBAR register is not set use the value provided in pThis->Mbar.
  // Otherwise update pThis->Mbar to the value currently set in MBAR.
  // The later switches HECI driver to value assigned by PCI Enumerator.
  //
  Mbar.QWord = 0;
  Mbar.Bits.DWordL = MmioRead32(pThis->PciCfg + HECI_R_MBAR);
  if (Mbar.Bits.DWordL == 0xFFFFFFFF)
  {
    DEBUG((EFI_D_ERROR, "[IE_HECI-%d] Device disabled\n", pThis->Fun));
    Mbar.Bits.DWordL = 0;
    goto GetOut;
  }
  if (Mbar.Bits.DWordL & 0x4) // if 64-bit address add the upper half
  {
    Mbar.Bits.DWordH = MmioRead32(pThis->PciCfg + HECI_R_MBAR + 4);
  }
  Mbar.Bits.DWordL &= 0xFFFFFFF0; // clear address type bits
  if (Mbar.QWord == 0)
  {
    if (pThis->Mbar == 0)
    {
      DEBUG((EFI_D_ERROR, "[IE_HECI-%d] MBAR not programmed\n", pThis->Fun));
      goto GetOut;
    }
    else
    {
      Mbar.QWord = pThis->Mbar;
      DEBUG((EFI_D_WARN, "[IE_HECI-%d] MBAR not programmed, using default 0x%08X%08X\n",
             pThis->Fun, Mbar.Bits.DWordH, Mbar.Bits.DWordL));
      //
      // Programm the MBAR, set the 64-bit support bit regardless of the size
      // of the address currently used.
      //
      MmioWrite32(pThis->PciCfg + HECI_R_MBAR + 4, Mbar.Bits.DWordH);
      MmioWrite32(pThis->PciCfg + HECI_R_MBAR, Mbar.Bits.DWordL | 4);
    }
  }
  else
  {
    pThis->Mbar = Mbar.QWord;
  }
  //
  // Enable the MBAR if not enabled
  //
  Cmd = MmioRead16(pThis->PciCfg + HECI_R_COMMAND);
  if (!(Cmd & HECI_CMD_MSE))
  {
    MmioWrite16(pThis->PciCfg + HECI_R_COMMAND, Cmd | HECI_CMD_BME | HECI_CMD_MSE);
  }
 GetOut:
  return (VOID*)(INTN)Mbar.QWord;
} // HeciMbarRead()


/**
  Print HECI message to the terminal.
  Normally just trace the header, if needed dump whole message to terminal.
 
  @param[in] pPrefix Text prefix, whether it is receive or send
  @param[in] pMsg    Pointer to the message body
  @param[in] MsgLen  The length of message body
 **/
VOID HeciTrace(
  IN     IE_HECI_DEVICE     *pThis,
  IN     CHAR8           *pPrefix,
  IN     HECI_MSG_HEADER *pMsg,
  IN     INT32            MsgLen)
{
  DEBUG((EFI_D_INFO, "[IE_HECI-%d] %a%08X\n", pThis->Fun, pPrefix, pMsg->DWord));
#if HECI_DUMP_ENABLE
  if (MsgLen > 4)
  {
    UINT32  LineBreak = 0;
    UINT32  Index = 0;
    UINT8   *pMsgBody = (UINT8*)&pMsg[1];
    
    MsgLen -= 4;
    while (MsgLen-- > 0)
    {
      if (LineBreak == 0)
      {
        DEBUG((EFI_D_ERROR, "%02x: ", (Index & 0xF0)));
      }
      DEBUG ((EFI_D_ERROR, "%02x ", pMsgBody[Index++]));
      LineBreak++;
      if (LineBreak == 16)
      {
        DEBUG((EFI_D_ERROR, "\n"));
        LineBreak = 0;
      }
      if (LineBreak == 8)
      {
        DEBUG((EFI_D_ERROR, "- "));
      }
    }
    DEBUG((EFI_D_ERROR, "\n"));
  }
#endif
} // HeciTrace()


/**
  Checks if onBoard IE FW is Dfx Type

  param                     None

  retval TRUE                  IE FW is Dfx Type
  retval FALSE                 IE FW is not Dfx Type
 **/
BOOLEAN IeTypeIsDfx(VOID)
{
  IEFS1 Iefs;

  Iefs.DWord = IeHeciPciReadIefs1();

  if (Iefs.DWord != (UINT32)-1 && Iefs.Bits.CurrentState == IEFS1_CURSTATE_DFXFW) {
    return TRUE;
  } else {
    return FALSE;
  }
} // IeTypeIsDfx()

