/*++
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
 **/
/*++

Copyright (c) 2014 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  IeHeciCoreLib.h

Abstract:

  The HECI core library implements HECI operations shared by drivers.

 **/
#ifndef _IE_HECI_CORE_LIB_H_
#define _IE_HECI_CORE_LIB_H_

#include <IeHeciRegs.h>

#define HECI_TIMEOUT_UNIT 1000  // Timeout unit in us for the HECI functions


/**
  HECI device (instance) data structure

  This structure describes HECI instance, i.e. PCI function on IE device.
  First part of the structure is filled by driver that is initializing HECI
  device. Second part is updated by HeciInit() function.
 */
typedef struct _IE_HECI_DEVICE_
{ //
  // The below defined fields must be set before calling HeciInit()
  //
  UINT8      Bus;     // PCI bus where this HECI instance is located
  UINT8      Dev;     // PCI device where this HECI instance is located
  UINT8      Fun;     // PCI function number of this HECI instance
  UINT8      Hidm;    // HECI interrupt mode for this HECI instance
  //
  // The MBAR field is in/out for HeciInit(). Non zero value provided on input
  // is an address to set in HECI MBAR if it is not configured yet.
  // On exit this field contains address currently used in HECI MBAR.
  // This field is not used by functions other than HeciInit().
  // MBAR register is used by send/receive/reset functions.
  //
  UINT64     Mbar;
  //BOOLEAN    IsReady; // Is HECI interface ready
  //
  // The below defined fields are set by Heciinit() function
  //
  UINTN      PciCfg;
  UINT16     Vid;     // PCI Device ID for this HECI instance
  UINT16     Did;     // PCI Vendor ID for this HECI instance
  UINT32     HMtu;    // Max transfer unit configured by IE minus header
  UINT32     IeMtu;   // Max transfer unit configured by IE minus header
  IEFS1      Iefs1;   // IE Firmware Status at recent operation
} IE_HECI_DEVICE;


/**
  Initialize HECI interface.

  This function initializes host side of HECI interface. If timeout is
  greater than zero it also waits until IE is ready to receive messages.

  @param[in,out] pThis     Pointer to HECI device structure
  @param[in,out] pTimeout  On input timeout in ms, on exit time left
 */
EFI_STATUS EFIAPI
HeciInit(
  IN OUT IE_HECI_DEVICE   *pThis,
  IN OUT UINT32           *pTimeout);

/**
  Set HECI interrupt enable bit in HECI CSR.

  This function sets HECI interrupt enable bit. It may enable or disable HECI
  interrupt.

  NOTE: It is dedicated for SMM. DXE and PEI drivers must not use interrupt.

  @param[in,out] pThis      Pointer to HECI device structure
  @param[in]     IntEnabled TRUE enables HECI interrupt, FALSE disables
 */
EFI_STATUS EFIAPI
HeciIntEnable(
  IN OUT IE_HECI_DEVICE   *pThis,
     OUT BOOLEAN          IntEnabled);

/**
  Get HECI interrupt state.

  This function reads HECI interrupt state. Whether it is enabled and whether
  it is currently signalled.

  @param[in,out] pThis       Pointer to HECI device structure
  @param[out]    pIntEnabled Is HECI interrupt enabled
  @param[out]    pIntStatus  Is HECI interrupt signalled
 */
EFI_STATUS EFIAPI
HeciIntState(
  IN OUT IE_HECI_DEVICE   *pThis,
     OUT BOOLEAN          *pIntEnabled,
     OUT BOOLEAN          *pIntStatus);

/**
  Reset HECI interface.

  This function resets HECI queue. If timeout is greater than zero it also
  waits until IE is ready to receive messages.

  @param[in,out] pThis      Pointer to HECI device structure
  @param[in,out] pTimeout   On input timeout in ms, on exit time left
 */
EFI_STATUS EFIAPI
HeciQueReset(
  IN OUT IE_HECI_DEVICE   *pThis,
  IN OUT UINT32           *pTimeout);

/**
  Get HECI queue state.

  This function reads HECI queue state. Whether it is ready for communication
  and whether there are any messages in the queue.

  @param[in,out] pThis     Pointer to HECI device structure
  @param[out]    pIsReady  Is HECI ready for communication
  @param[out]    pSendQue  Number of DWords in send queue
  @param[out]    pRecvQue  Number of DWords in receive queue
 */
EFI_STATUS EFIAPI
HeciQueState(
  IN OUT IE_HECI_DEVICE   *pThis,
     OUT BOOLEAN          *pIsReady,
     OUT UINT32           *pSendQue,
     OUT UINT32           *pRecvQue);

/**
  Write one message to HECI queue.

  This function puts one message to HECI queue. If timeout is greater than
  zero the function may wait for space in the queue. This function handles
  only messages shorter than HECI queue length. Fragmentation of large
  messages must be done by upper layer protocol.

  @param[in,out] pThis      Pointer to HECI device structure
  @param[in,out] pTimeout   On input timeout in ms, on exit time left
  @param[in]     pMessage   The header of the message to send
 */
EFI_STATUS EFIAPI
HeciMsgSend(
  IN OUT IE_HECI_DEVICE  *pThis,
  IN OUT UINT32          *pTimeout,
  IN     HECI_MSG_HEADER *pMessage);

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
 */
EFI_STATUS EFIAPI
HeciMsgRecv(
  IN OUT IE_HECI_DEVICE  *pThis,
  IN OUT UINT32          *pTimeout,
     OUT HECI_MSG_HEADER *pMsgBuf,
  IN     UINT32          *pBufLen);

/**
  Checks if onBoard IE FW is Dfx Type

  param                     None

  retval TRUE                  IE FW is Dfx Type
  retval FALSE                 IE FW is not Dfx Type
 **/
BOOLEAN
IeTypeIsDfx(
  VOID);

#endif // _IE_HECI_CORE_LIB_H_

