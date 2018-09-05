/** @file
  Me Debug Lib implementation.

@copyright
 Copyright (c) 2013 - 2015 Intel Corporation. All rights reserved
 This software and associated documentation (if any) is furnished
 under a license and may only be used or copied in accordance
 with the terms of the license. Except as permitted by the
 license, no part of this software or documentation may be
 reproduced, stored in a retrieval system, or transmitted in any
 form or by any means without the express written consent of
 Intel Corporation.
 This file contains an 'Intel Peripheral Driver' and is uniquely
 identified as "Intel Reference Module" and is licensed for Intel
 CPUs and chipsets under the terms of your license agreement with
 Intel or your vendor. This file may be modified by the user, subject
 to additional terms of the license agreement.

@par Specification Reference:
**/
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>

#ifndef MDEPKG_NDEBUG

/**
  For serial debugger used, it will show the buffer message line by line to serial console.

  @param[in] Message              the address point of buffer message
  @param[in] Length               message length

**/
VOID
ShowBuffer (
  IN  UINT8                       *Message,
  IN  UINT32                      Length
  )
{
  UINT32                          Index;
  UINT32                          Offset;
  CHAR16                          Buffer[51]; // To construct a line needs 51 chars.

  Index  = 0;
  Offset = 0;
  ZeroMem (Buffer, sizeof (Buffer));

  while (Length-- > 0) {
    //
    // Get the corresponding offset value from the index of buffer message.
    //
    Offset = ((Index & 0x0F) > 7) ? (((Index & 0x0F) * 3) + 2) : ((Index & 0x0F) * 3);

    //
    // Print "- " at the half of a line increases the readability of debug message.
    //
    if ((Index & 0x0F) == 0x08) {
      UnicodeSPrint (&Buffer[24], 3 * sizeof (CHAR16), L"- ");
    }

    //
    // Collect the data of buffer message.
    //
    UnicodeSPrint (&Buffer[Offset], 4 * sizeof (CHAR16), L"%02x ", Message[Index]);

    //
    // A line contains 16 bytes of buffer message. If a line is complete, it will be shown through DEBUG macro.
    //
    if (Offset == 47) {
      DEBUG ((DEBUG_ERROR, "%02x: %s\n", (Index & 0xF0), Buffer));
    }

    Index++;
  }

  //
  // If a line isn't complete, show the remaining data.
  //
  if (Offset != 47) {
    DEBUG ((DEBUG_ERROR, "%02x: %s\n", (Index & 0xF0), Buffer));
  }
  return;
}

#endif // End Of MDEPKG_NDEBUG
