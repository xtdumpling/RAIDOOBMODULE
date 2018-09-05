/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2007 - 2015 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file Hecicore.h

  Heci driver core. For Dxe Phase, determines the HECI device and initializes it.

**/
#ifndef _HECI_CORE_H
#define _HECI_CORE_H

#include <HeciRegs.h>

/**

  This function provides a standard way to verify the HECI cmd and MBAR regs
  in its PCI cfg space are setup properly and that the local mHeciContext
  variable matches this info.

  @param[in] HeciDev              The HECI device to be accessed.

  @retval HeciMBAR

**/
UINTN
CheckAndFixHeciForAccess (
  IN HECI_DEVICE HeciDev
  )
;

/**

    Calculate if the circular buffer has overflowed.
    Corresponds to HECI HPS (part of) section 4.2.1

    @param ReadPointer  - Location of the read pointer.
    @param WritePointer - Location of the write pointer.

    @retval Number of filled slots.

**/
UINT8
FilledSlots (
  IN      UINT32                    ReadPointer,
  IN      UINT32                    WritePointer
  )
;

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
  IN      UINT32                    ReadPointer,
  IN      UINT32                    WritePointer,
  IN      UINT32                    BufferDepth
  )
;

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
HeciPacketRead (
  IN      UINT32                    Blocking,
  OUT     HECI_MESSAGE_HEADER       *MessageHeader,
  OUT     UINT32                    *MessageData,
  IN OUT  UINT32                    *Length
  )
;

/**

   Function sends one message packet through the HECI circular buffer
   Corresponds to HECI HPS (part of) section 4.2.3

    @param MessageHeader - Pointer to the message header.
    @param MessageData   - Pointer to the actual message data.

    @retval EFI_STATUS

**/
EFI_STATUS
HeciPacketWrite (
  IN      HECI_MESSAGE_HEADER       *MessageHeader,
  IN      UINT32                    *MessageData
  )
;

#endif // _HECI_CORE_H