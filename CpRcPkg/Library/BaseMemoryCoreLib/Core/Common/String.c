/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2015 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/

#include "SysFunc.h"

/**

  Copy string from source to destination

  @param dest  - Pointer to destination for string copy
  @param src   - Pointer to source of string to copy

  @retval N/A

**/
void
StrCpyLocal (
            INT8 *dest,
            INT8 *src
            )
{
  while (*src) *dest++ = *src++;
  *dest = 0;
}

/**

  Compares two strings

  @param src1  - first string to compare
  @param src2  - second string to compare

  @retval 0 - strings match
  @retval 1 - strings dont match

**/
UINT32
StrCmpLocal (
            INT8 *src1,
            INT8 *src2
            )
{
  UINT32 status = SUCCESS;

  while (*src1 && *src2 && (tolower_(*src1) == tolower_(*src2))) {
    src1++;
    src2++;
  }
  if (*src1 || *src1 != *src2) status = FAILURE;
  return status;
}

/**

  Determines the length of the string

  @param Str - Pointer to string to measure

  @retval Length of string

**/
UINT32
StrLenLocal (
            INT8 *Str
            )
{
  UINT32 i = 0;

  while (*(Str + i)) i++;
  return i;
}

/**

  Convert character to lower case

  @param c - character to convert

  @retval lower case character

**/
INT8
tolower_ (
         INT8 c
         )
{
  if (c >= 65 && c <= 90) c = c + 32;
  return c;
}
