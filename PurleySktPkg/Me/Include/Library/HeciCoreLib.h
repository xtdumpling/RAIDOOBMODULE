/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2006 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file HeciCoreLib.h

  Definitions for HECI driver

**/
#ifndef _HECI_CORE_LIB_H
#define _HECI_CORE_LIB_H

#include <MeChipset.h>
#include "CoreBiosMsg.h"

/**
  Heci2 driver core (Server).
  For PEI Phase, determines the HECI2 device and initializes it.

**/

#ifndef _HECI_SERVER_H
#define _HECI_SERVER_H

EFI_STATUS
EFIAPI
Heci2GetMeStatus (
  IN UINT32                 *MeStatus
  );

EFI_STATUS
EFIAPI
Heci2Initialize(
  VOID
  );

#endif // _HECI_SERVER_H

//
// Prototypes
//
/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @exception EFI_UNSUPPORTED      HECI MSG is unsupported
**/
EFI_STATUS
EFIAPI
HeciInitialize (
  IN HECI_DEVICE                  HeciDev
  );

/**

  Heci Re-initializes it for Host

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_STATUS              Status code returned by ResetHeciInterface
**/
EFI_STATUS
EFIAPI
HeciReInitialize (
  IN HECI_DEVICE                  HeciDev
  );


/**
  Reads a message from the ME across HECI.

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
  IN      HECI_DEVICE             HeciDev,
  IN      UINT32                  Blocking,
  IN OUT  UINT32                 *MessageBody,
  IN OUT  UINT32                 *Length
  );

/**

  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSend (
  IN HECI_DEVICE                  HeciDev,
  IN UINT32                      *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  );

/**

  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in][out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the bufferbefore timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSendwAck (
  IN      HECI_DEVICE             HeciDev,
  IN OUT  UINT32                 *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                 *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  );

/**

  @param[in] Delay                The biggest waiting time

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Me is ready
**/
EFI_STATUS
EFIAPI
HeciMeResetWait (
  IN  HECI_DEVICE                 HeciDev,
  IN  UINT32                      Delay
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm
  in HPS 0.90 doc 4-17-06 njy

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
HeciResetInterface (
  IN  HECI_DEVICE                 HeciDev
  );

/**
  Get an abstract Intel ME State from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME
  functions

  @param[out] MeStatus            Pointer for status report
                                  see MeState.h - Abstract ME status definitions.

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeStatus is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  IN UINT32                     *MeStatus
  );

/**
  Return ME Mode

  @param[out] MeMode              Pointer for ME Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  IN UINT32                     *MeMode
  );

//
// Local/Private functions not part of EFIAPI for HECI
//

/**

    Function sends one messsage (of any length) through the HECI circular buffer.

    @param Message     - Pointer to the message data to be sent.
    @param Length      - Length of the message in bytes.
    @param HostAddress - The address of the host processor.
    @param MeAddress   - Address of the ME subsystem the message is being sent to.

    @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendMsg (
  IN      UINT32                    *Message,
  IN      UINT32                    Length,
  IN      UINT8                     HostAddress,
  IN      UINT8                     MeAddress
  );

/**
  Waits for the ME to report that it is ready for communication over the HECI
  interface.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_SUCCESS             ME is ready
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
WaitForMEReady (
    IN HECI_DEVICE HeciDev
  );

/**
  Function reads HECI mbar from PCI

  @param[in] None

  @retval HECI mbar
**/
UINTN
HeciMbarRead (
   VOID
   );

#endif // _HECI_CORE_H
