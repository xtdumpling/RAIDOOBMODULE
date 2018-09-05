//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Memory Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

@copyright
  Copyright 2006 - 2016, Intel Corporation All Rights Reserved.

  The source code contained or described herein and all documents
  related to the source code ("Material") are owned by Intel
  Corporation or its suppliers or licensors. Title to the Material
  remains with Intel Corporation or its suppliers and licensors.
  The Material contains trade secrets and proprietary and confidential
  information of Intel or its suppliers and licensors. The Material
  is protected by worldwide copyright and trade secret laws and treaty
  provisions.  No part of the Material may be used, copied, reproduced,
  modified, published, uploaded, posted, transmitted, distributed, or
  disclosed in any way without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other
  intellectual property right is granted to or conferred upon you
  by disclosure or delivery of the Materials, either expressly,
  by implication, inducement, estoppel or otherwise. Any license
  under such intellectual property rights must be express and
  approved by Intel in writing.

@file
  Printf.h

@brief
  This module provides data and macros to support print and display.

**/

#ifndef _printf_h
#define _printf_h

#include "DataTypes.h"

#define PRINTF_CONTROL_OUTOF_SYNC_ERR_MAJOR   0xCF
#define PRINTF_CONTROL_OUTOF_SYNC_ERR_MINOR   0x01

#ifndef ASM_INC
#define TAB_STOP            4
#define LEFT_JUSTIFY        0x01
#define PREFIX_SIGN         0x02
#define PREFIX_BLANK        0x04
#define COMMON_PREFIX_ZERO  0x08
#define LONG_TYPE           0x10

#define INT_SIGNED          0x20
#define COMA_TYPE           0x40
#define LONG_LONG_TYPE      0x80
#define TO_UPPER            0x100

#define CHAR_CR             0x0d
#define CHAR_LF             0x0a

//
// ANSI Escape sequences for color
//
#define ANSI_FOREGROUND_BLACK      30
#define ANSI_FOREGROUND_RED        31
#define ANSI_FOREGROUND_GREEN      32
#define ANSI_FOREGROUND_YELLOW     33
#define ANSI_FOREGROUND_BLUE       34
#define ANSI_FOREGROUND_MAGENTA    35
#define ANSI_FOREGROUND_CYAN       36
#define ANSI_FOREGROUND_WHITE      37

#define ANSI_BACKGROUND_BLACK      40
#define ANSI_BACKGROUND_RED        41
#define ANSI_BACKGROUND_GREEN      42
#define ANSI_BACKGROUND_YELLOW     43
#define ANSI_BACKGROUND_BLUE       44
#define ANSI_BACKGROUND_MAGENTA    45
#define ANSI_BACKGROUND_CYAN       46
#define ANSI_BACKGROUND_WHITE      47

#define ANSI_ATTRIBUTE_OFF         0
#define ANSI_ATTRIBUTE_BOLD        1
#define ANSI_ATTRIBUTE_UNDERSCORE  4
#define ANSI_ATTRIBUTE_BLINK       5
#define ANSI_ATTRIBUTE_REVERSE     7
#define ANSI_ATTRIBUTE_CONCEAL     8

#ifndef INT32_MAX
#define INT32_MAX             0x7fffffffU
#endif

#ifndef va_start
typedef INT8 *  va_list;
#define _INTSIZEOF(n)   ((sizeof (n) + sizeof (UINT32) - 1) &~(sizeof (UINT32) - 1))
#define va_start(ap, v) (ap = (va_list) & v + _INTSIZEOF (v))
#define va_arg(ap, t)   (*(t *) ((ap += _INTSIZEOF (t)) - _INTSIZEOF (t)))
#define va_end(ap)      (ap = (va_list) 0)
#endif

#define ISDIGIT(_c)     (((_c) >= '0') && ((_c) <= '9'))
#define ISHEXDIGIT(_c)  (((_c) >= 'a') && ((_c) <= 'f'))
#endif // ASM_INC

#endif // _printf_h
