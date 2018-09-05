/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**
@copyright
  Copyright (c) 2014-2015 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  BitMask.c

@brief
  This file contains utility functions for manipulating bitmasks.
**/
#ifdef SSA_FLAG

#include "BitMask.h"

/**
@brief
  This helper function returns the number of 1's and location of the first 1 in
  the given 8-bit bitmask.

  @param[in]   Bitmask             Bitmask.
  @param[in]   MaxBitCount         Maximum bit count.  The value must be more than or equal to 1 and less than or equal to 32.
  @param[out]  pBitCnt             Pointer to the location where the count of 1's will be placed.
  @param[out]  pLowestBitLocation  Pointer to the location where the location of the lowest 1 will be placed or a value of MaxBitCount if there is no bit set.
**/
VOID GetBitSetCntInMask(
  IN UINT32 Bitmask,
  IN UINT8 MaxBitCount,
  OUT UINT8 *pBitCnt,
  OUT UINT8 *pLowestBitLocation)
{
  UINT8 Bit;
  UINT8 BitCnt = 0;

  for (Bit = 0; Bit < MaxBitCount; Bit++){
    if ((Bitmask & (1 << Bit)) != 0) {
      BitCnt += 1;
    }
  }

  for (Bit = 0; Bit < MaxBitCount; Bit++){
    if ((Bitmask & (1 << Bit)) != 0) {
      break;
    }
  }

  *pBitCnt = BitCnt;
  *pLowestBitLocation = Bit;
} // end function GetBitSetCntInMask

/**
@brief
  This function is used to check if the given bit is the highest bit set in the
  given mask.

  @param[in]   Bit       Bit to check.
  @param[in]   Bitmask   Bitmask.

  @retval  TRUE/FALSE.
**/
BOOLEAN IsHighestBitSetInMask(
  IN UINT8 Bit,
  IN UINT32 Bitmask)
{
  return ((Bitmask & ~(1 << Bit)) < ((UINT32) 1 << Bit));
} //  end function IsHighestBitSetInMask

/**
@brief
  This function is used to check if the given bit is the highest bit set in the
  given mask excluding the given special bit.

  @param[in]   Bit       Bit to check.
  @param[in]   Bitmask   Bitmask.
  @param[in]   ExclBit   Bit to exclude.

  @retval  TRUE/FALSE.
**/
BOOLEAN IsHighestBitSetInMaskExcl(
  IN UINT8 Bit,
  IN UINT32 Bitmask,
  IN UINT8 ExclBit)
{
  Bitmask &= ~(1 << ExclBit);

  return IsHighestBitSetInMask(Bit, Bitmask);
} // end function IsHighestBitSetInMaskExcl

/**
@brief
  This function is used to get the highest bit set in the given mask.

  @param[in]   Bitmask       Bitmask.
  @param[in]   MaxBitCount   Maximum bit count.  The value must be more than or equal to 1 and less than or equal to 32.

  @retval  Offset of highest bit set in mask or a value of 0 if there is no bit set.
**/
UINT8 GetHighestBitSetInMask(
  IN UINT32 Bitmask,
  IN UINT8 MaxBitCount)
{
  INT8 Bit;

  // FOR each bit (starting from the highest):
  for (Bit = (MaxBitCount - 1); Bit > 0; Bit--) {
    if (Bitmask & (1 << Bit)) {
      break;
    }
  }

  // IF there were no bits set THEN return 0
  if (Bit < 0) {
    Bit = 0;
  }

  return (UINT8) Bit;
} // end function GetHighestBitSetInMask

/**
@brief
  This function is used to get the lowest bit set in the given mask.

  @param[in]   Bitmask       Bitmask.
  @param[in]   MaxBitCount   Maximum bit count.  The value must be more than or equal to 1 and less than or equal to 32.

  @retval  Offset of lowest bit set in mask or a value of MaxBitCount if there is no bit set.
**/
UINT8 GetLowestBitSetInMask(
  IN UINT32 Bitmask,
  IN UINT8 MaxBitCount)
{
  UINT8 Bit;

  for (Bit = 0; Bit < MaxBitCount; Bit++) {
    if (Bitmask & (1 << Bit)) {
      break;
    }
  }

  return Bit;
} //  end function GetLowestBitSetInMask

/**
@brief
  This function is used to get the lowest bit set in the given mask that is
  also not equal to the given value.

  @param[in]   Bitmask       Bitmask.
  @param[in]   MaxBitCount   Maximum bit count.  The value must be more than or equal to 1 and less than or equal to 32.
  @param[in]   NotEqBit      Bit that the returned value must not be equal to.

  @retval  Offset of lowest bit set in mask that is also not equal to the given value or a value of MaxBitCount if there is no bit set.
**/
UINT8 GetLowestBitSetInMaskThatIsNotEq(
  IN UINT32 Bitmask,
  IN UINT8 MaxBitCount,
  IN UINT8 NotEqBit)
{
  Bitmask &= ~(1 << NotEqBit);

  return GetLowestBitSetInMask(Bitmask, MaxBitCount);
} //  end function GetLowestBitSetInMaskThatIsNotEq

/**
@brief
  This function is used to get the next higher bit set in the given mask.

  @param[in]   CrntBit       Current bit.
  @param[in]   Bitmask       Bitmask.
  @param[in]   MaxBitCount   Maximum bit count.  The value must be more than or equal to 1 and less than or equal to 32.

  @retval  Offset of next higher bit set in mask or a value of MaxBitCount if there is no next bit set.
**/
UINT8 GetNextHigherBitSetInMask(
  IN UINT8 CrntBit,
  IN UINT32 Bitmask,
  IN UINT8 MaxBitCount)
{
  UINT8 Bit = CrntBit + 1;

  for (Bit = CrntBit + 1; Bit < MaxBitCount; Bit++) {
    if (Bitmask & (1 << Bit)) {
      break;
    }
  }

  return Bit;
} //  end function GetNextHigherBitSetInMask

/**
@brief
  This function is used to get the next higher bit set in the given mask that
  is also not equal to the given value.

  @param[in]   CrntBit       Current bit.
  @param[in]   Bitmask       Bitmask.
  @param[in]   MaxBitCount   Maximum bit count.  The value must be more than or equal to 1 and less than or equal to 32.
  @param[in]   NotEqBit      Bit that the returned value must not be equal to.

  @retval  Offset of next higher bit set in mask that is also not equal to the given value or a value of MaxBitCount if there is no next bit set.
**/
UINT8 GetNextHigherBitSetInMaskThatIsNotEq(
  IN UINT8 CrntBit,
  IN UINT32 Bitmask,
  IN UINT8 MaxBitCount,
  IN UINT8 NotEqBit)
{
  Bitmask &= ~(1 << NotEqBit);

  return GetNextHigherBitSetInMask(CrntBit, Bitmask, MaxBitCount);
} //  end function GetNextBitSetInMaskThatIsNotEq

#endif  // SSA_FLAG

// end file BitMask.c
