/**
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/
/**

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file SetupFunctions.c

Revision History:

**/
#include "SetupPlatform.h"

/**

  Converts an ascii string to unicode string 16 chars at a time.

  @param AsciiString    -  Address of Ascii string.
  @param UnicodeString  -  Address of Unicode String.

  @retval None.

**/
VOID
AsciiToUnicode (
  IN CHAR8     *AsciiString,
  IN CHAR16    *UnicodeString
  )
{
  UINT8 Index;

  Index = 0;
  while (AsciiString[Index] != 0) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
    Index++;
  }
  UnicodeString[Index] = 0;
}

/**

  Swap order of nearby CHAR8 data.

  @param Data  -  The data to swap.
  @param Size  -  Size of data to swap.

  @retval None.
  
**/
VOID
SwapEntries (
  IN  CHAR8  *Data,
  IN  UINT16 Size
  )
{
  UINT16  Index;
  CHAR8   Temp8;

  for (Index = 0; (Index + 1) < Size; Index += 2) {
    Temp8           = Data[Index];
    Data[Index]     = Data[Index + 1];
    Data[Index + 1] = Temp8;
  }

  return ;
}

/**

  Convert data from base 10 to raw.

  @param Data  -  The data to convert.

  @retval Return converted data.

**/
UINT32
ConvertBase10ToRaw (
  IN  EFI_EXP_BASE10_DATA             *Data
  )
{
  UINTN   Index;
  UINT32  RawData;

  RawData = Data->Value;
  for (Index = 0; Index < (UINTN) Data->Exponent; Index++) {
    RawData *= 10;
  }

  return RawData;
}

/**

  Convert data from base 2 to raw.

  @param Data  -  The data to convert.

  @retval Return converted data.

**/
UINT32
ConvertBase2ToRaw (
  IN  EFI_EXP_BASE2_DATA             *Data
  )
{
  UINTN   Index;
  UINT32  RawData;

  RawData = Data->Value;
  for (Index = 0; Index < (UINTN) Data->Exponent; Index++) {
    RawData <<= 1;
  }

  return RawData;
}

