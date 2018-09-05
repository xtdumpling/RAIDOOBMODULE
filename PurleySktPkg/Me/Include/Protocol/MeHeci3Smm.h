/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

 MeHeci3Smm.h

Abstract:

 SMM HECI3 Protocol
 SMM HECI3 driver is using HECI-3.
 TODO: Currently this protoco is used by UMA error injection only.
 TODO: The plan is to make it common for all SMM communication with ME.

--*/
#ifndef _PROTOCOL_ME_HECI3_SMM_H_
#define _PROTOCOL_ME_HECI3_SMM_H_

#include <MeHeci3.h>

// 69735520-DA83-444A-93DC-BDDD59E59182
#define SMM_ME_HECI3_PROTOCOL_GUID  {0x69735520, 0xDA83, 0x444A, 0x93, 0xDC, 0xBD, 0xDD, 0x59, 0xE5, 0x91, 0x82}

extern EFI_GUID gSmmMeHeci3ProtocolGuid;

typedef struct _SMM_ME_HECI3_PROTOCOL_ SMM_ME_HECI3_PROTOCOL;


/**
 * HECI device (instance) data structure
 *
 * This structure describes HECI instance, i.e. PCI funcion on ME device.
 * First part of the structure is filled by driver that is initializing HECI
 * device. Second part is updated by HeciInit() function.
 */
typedef struct
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
  //
  // The below defined fields are set by Heciinit() function
  //
  UINTN      PciCfg;
  UINT16     Vid;     // PCI Device ID for this HECI instance
  UINT16     Did;     // PCI Vendor ID for this HECI instance
  UINT32     HMtu;    // Max transfer unit configured by ME minus header
  UINT32     MeMtu;   // Max transfer unit configured by ME minus header
  HECI_MEFS1 Mefs1;   // ME Firmware Status at recent operation
} ME_HECI_DEVICE;

/**
 * Initialize HECI interface.
 *
 * This function initializes host side of HECI interface. If timeout is
 * greater than zero it also waits until ME is ready to receive messages.
 *
 * @param[in,out] pThis     Pointer to protocol structure
 * @param[in,out] pTimeout  On input timeout in ms, on exit time left
 */
typedef EFI_STATUS (EFIAPI *SMM_ME_HECI3_INIT)(
  IN     SMM_ME_HECI3_PROTOCOL *pThis,
  IN OUT UINT32                *pTimeout);

/**
 * Reset HECI queue.
 *
 * This function resets HECI queue. If timeout is greater than zero it also
 * waits until ME is ready to receive messages.
 *
 * @param[in]     pThis     Pointer to protocol data
 * @param[in,out] pTimeout  On input timeout in ms, on exit time left
 */
typedef EFI_STATUS (EFIAPI *SMM_ME_HECI3_QUE_RESET)(
  IN     SMM_ME_HECI3_PROTOCOL *pThis,
  IN OUT UINT32                *pTimeout);

/**
 * Get HECI queue state.
 *
 * This function reads HECI queue state. It informs whether queue is ready for
 * communication, and whether there are some dwords in send or receive queue.
 * If SmmHeciRequest() is called and queue is not empty reset is done to clear
 * it. SmmHeciQueState() may be used to detect this situation and if possible
 * delay the SMM request, so that OS driver can finish its transaction.
 *
 * @param[in,out] pThis     Pointer to protocol data
 * @param[in,out] pTimeout  On input timeout in us, on exit time left
 */
typedef EFI_STATUS (EFIAPI *SMM_ME_HECI3_QUE_STATE)(
  IN OUT SMM_ME_HECI3_PROTOCOL *pThis,
     OUT BOOLEAN               *pIsReady,
     OUT UINT32                *pSendQue,
     OUT UINT32                *pRecvQue);

/**
 * Send request message to HECI queue, wait for response if needed.
 *
 * This function writes one message to HECI queue and - if receive buffer
 * was provided and timeout is greater than zero - waits for response message.
 * Fragmentation is not supported. Reqeust and response must be unfragmented.
 * Size of receive message buffer is given in bytes in (*pBufLen) on input.
 * On exit (*pBufLen) provides the number of bytes written to the message
 * buffer. If buffer is too short the message is truncated.
 *
 * @param[in]     pThis      Pointer to protocol data
 * @param[in,out] pTimeout   On input timeout in ms, on exit time left
 * @param[in]     pReqMsg    Request message
 * @param[out]    pMsgBuf    Buffer for the response message
 * @param[in,out] pBufLen    On input buffer size, on exit message, in bytes
 */
typedef EFI_STATUS (EFIAPI *SMM_ME_HECI3_REQUEST)(
  IN     SMM_ME_HECI3_PROTOCOL *pThis,
  IN OUT UINT32                *pTimeout,
  IN     HECI_MSG_HEADER       *pReqMsg,
     OUT HECI_MSG_HEADER       *pRspBuf,
  IN     UINT32                *pBufLen);

/**
 * EFI protocol for HECI communication in SMM.
 *
 * This protocol defines operations for communication with ME using HECI
 * (Host Embeded Controller Interface) in SMM.
 * Note that SMM uses HECI-1 potentially shared with OS driver.
 * SMM request may sometimes break OS driver reqeust.
 */
typedef struct _SMM_ME_HECI3_PROTOCOL_
{
  ME_HECI_DEVICE           HeciDev;
  
  SMM_ME_HECI3_INIT        HeciInit;
  SMM_ME_HECI3_QUE_RESET   HeciQueReset;
  SMM_ME_HECI3_QUE_STATE   HeciQueState;
  SMM_ME_HECI3_REQUEST     HeciRequest;
} SMM_ME_HECI3_PROTOCOL;

#endif // _PROTOCOL_HECI_SMM_H_

