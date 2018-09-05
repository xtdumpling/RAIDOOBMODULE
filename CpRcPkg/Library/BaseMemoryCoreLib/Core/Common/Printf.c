/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
/*************************************************************************
 *
 * Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2006 - 2016, Intel Corporation All Rights Reserved.
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
#include "SysHost.h"

#ifdef SERIAL_DBG_MSG

#define PUTCC(_c, _CharCount) \
  putchar_buffer (host, _c); \
  _CharCount++

//
// Local Prototypes
//
void EncodeUINT32 (UINT32 Value, INT8 *Str);
void EncodeUINT64 (UINT64_STRUCT Value, INT8 *Str);
UINT32 StrToNumber(INT8 **String);
void UintnToStr (UINT32 Value, INT8 *Str, UINT32 Width, UINT32 Flags, UINT32 Base);

void
UintnToStr (
           UINT32 Value,
           INT8   *Str,
           UINT32 Width,
           UINT32 Flags,
           UINT32 Base
           )
/*++

  Converts an unsigned integer to a text string

  @param Value - the integer to convert
  @param Str   - pointer to store string
  @param Width - width of integer (DWORD for example)
  @param Flags - bitwise flags
                 LEFT_JUSTIFY      0x01
                 PREFIX_SIGN       0x02
                 PREFIX_BLNK       0x04
                 PREFIX_ZERO       0x08
                 LONG_TYPE         0x10

          INT_SIGNED        0x20
          COMA_TYPE         0x40
          LONG_LONG_TYPE    0x80

  Base  - decimal (10), hex(16), etc

  @retval N/A

--*/
{
  UINT32      Negative;
  UINT32      Int;
  INT8 volatile *Ptr; //Use volatile to avoid intrinsic memset substitution by compiler.
  INT8        Prefix;
  INT8        c;
  UINT32      i;
  const INT8  Hex_upper[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  const INT8  Hex_lower[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  INT8 *Hex;

  if (Flags & TO_UPPER) {
    Hex = (INT8 *)&Hex_upper;
  } else {
    Hex = (INT8 *)&Hex_lower;
  }

  Ptr               = Str;
  if ((Value > INT32_MAX) && (Flags & INT_SIGNED)) {
    Int       = ~Value + 1; /* -Value */
    Negative  = 1;
  } else {
    Int       = Value;
    Negative  = 0;
  }

  i = 0;
  do {                      /* generate digits in reverse order */
    i++;
    *Ptr++ = Hex[Int % Base];
    if (Flags & COMA_TYPE) {
      if (Base == 16) {
        if (i % 4 == 0) {
          *Ptr++ = ',';
        }
      } else if (Base == 10) {
        if (i % 3 == 0) {
          *Ptr++ = ',';
        }
      }
    }
  } while ((Int /= Base) > 0);
  if (*(Ptr - 1) == ',') {
    Ptr--;
  }

  if (Negative) {
    *Ptr++ = '-';
  } else if (Flags & PREFIX_SIGN) {
    *Ptr++ = '+';
  }

  if (Flags & COMMON_PREFIX_ZERO) {
    Prefix = '0';
  } else if (!(Flags & LEFT_JUSTIFY)) {
    Prefix = ' ';
  } else {
    Prefix = 0x00;
  }

  if (Prefix != 0x00) {
    for (i = (int) (Ptr - Str); i < Width; i++) {
      *Ptr++ = Prefix;
    }
  }

  *Ptr = '\0';

  /* reverse string */
  while (Str < --Ptr) {
    c       = *Str;
    *Str++  = *Ptr;
    *Ptr    = c;
  }
}

UINT32
StrToNumber (
            INT8 **String
            )
/*++

  Converts a string to a number

  @param String  - pointer to string pointer

  @retval Number conversion of string

--*/
{
  UINT32  Sum;
  INT8    *Str;

  Str = *String;
  if (*Str == '0') {
    Str++;
  }

  Sum = 0;
  while (ISDIGIT (*Str)) {
    Sum = Sum * 10 + (*Str++ -'0');
  }

  *String = Str;
  return Sum;
}

void
EncodeUINT32 (
             UINT32 Value,
             INT8   *Str
             )
/*++

  encodes a UINT32 to a string

  @param Value - Value to encode
  @param Str   - pointer to string location

  @retval N/A

--*/
{
  INT8 volatile *Ptr; //Use volatile to avoid intrinsic memcpy substitution by compiler.
  UINT8 *vp;
  UINT8 i;
  UINT8 eb;
  UINT8 nb;

  Ptr = Str;
  vp  = (UINT8 *) &Value;

  if (Value < 0x10) {
    *Ptr++ = BIT7 | *vp;
  } else {
    if (Value < 0x100) {
      eb = nb = 1;
    } else if (Value < 0x10000) {
      eb = nb = 2;
    } else {
      eb  = 3;
      nb  = 4;
    }

    *Ptr++ = BIT7 | (eb << 4);
    for (i = 0; i < nb; i++) {
      *Ptr++ = *vp++;
    }
  }

  *Ptr = '\0';
}

void
EncodeUINT64 (
             UINT64_STRUCT Value,
             INT8          *Str
             )
/*++

  Encodes a UINT64 to a string

  @param Value - Value to encode
  @param Str   - Pointer to string

  @retval N/A

--*/
{
  INT8  *Ptr;
  UINT8 *vp;
  UINT8 i;
  UINT8 eb;
  UINT8 nb;

  Ptr = Str;
  vp  = (UINT8 *) &Value.lo;

  if (!Value.hi) {

    EncodeUINT32 (Value.lo, Str);
  } else {
    eb      = 4;
    nb      = 8;
    *Ptr++  = BIT7 | (eb << 4);
    for (i = 0; i < nb; i++) {
      *Ptr++ = *vp++;
    }
  }

  *Ptr = '\0';
}

#define ESC 27
void
setRed (
       PSYSHOST host
       )
/*++

  Sets color scheme for foreground red, background black

  @param host  - Pointer to the system host (root) structure

  @retval N/A

--*/
{
  setColor (host, 31, 40, 0);
}

void
setWhite (
         PSYSHOST host
         )
/*++

  Set color scheme to background black, forground white

  @param host  - Pointer to the system host (root) structure

  @retval N/A

--*/
{
  //
  // Set foreground white
  // Set background black
  //
  setColor (host, 37, 40, 0);
}

void
setColor (
         PSYSHOST host,
         INT8     foreGround,
         INT8     backGround,
         INT8     attribute
         )
/*++

  set screen color scheme

  @param host        - Pointer to the system host (root) structure
  @param foreGround  - color code for foreground
  @param backGround  - color code for background
  @param attribute   - attribute flag
                       ATTRIBUTE_OFF         0
                       ATTRIBUTE_BOLD        1
                       ATTRIBUTE_UNDERSCORE  4
                       ATTRIBUTE_BLINK       5
                       ATTRIBUTE_REVERSE     7
                       ATTRIBUTE_CONCEAL     8

  @retval N/A

--*/
{
  putchar_buffer (host, ESC);
  putchar_buffer (host, '[');
  putchar_buffer (host, '0');
  putchar_buffer (host, '0' + attribute);
  putchar_buffer (host, 'm');
  putchar_buffer (host, ESC);
  putchar_buffer (host, '[');
  putchar_buffer (host, '0' + (foreGround / 10));
  putchar_buffer (host, '0' + (foreGround % 10));
  putchar_buffer (host, 'm');
  putchar_buffer (host, ESC);
  putchar_buffer (host, '[');
  putchar_buffer (host, '0' + (backGround / 10));
  putchar_buffer (host, '0' + (backGround % 10));
  putchar_buffer (host, 'm');
}


UINT8
getPrintFControl (
                 PSYSHOST host
                 )
/*++

  Get control of a global semaphore for the calling socket BSP
  This should be used with a matching releasePrintFControl() 
  to gain exclusive ownership of a system semaphore for a section
  of code that blocks/yields to other socket BSPs.
  host->var.common.printfDepth is used to handle the nested calls to this function

  @param host  - Pointer to the system host (root) structure

  @retval 1 - success

--*/
{
   UINT8 haltOnError = 1;
   UINT8 *haltOnErrorPtr = &haltOnError;

  if (host->var.mem.varStructInit == 0) {
    host->var.mem.serialDebugMsgLvl = host->setup.common.serialDebugMsgLvl;
  }

  if (host->var.common.printfSync && host->var.mem.serialDebugMsgLvl) {
     if (host->var.common.printfDepth == 0) {
         AcquireReleaseGlobalSemaphore (host, SYSTEMSEMAPHORE0, ACQUIRE_SEMAPHORE);
     }
     // increment nested call depth
     host->var.common.printfDepth++;
     // if count rollover, fatal error
     if (host->var.common.printfDepth == 0)  {
       if (host->var.common.socketId == host->nvram.common.sbspSocketId) {
         PlatformFatalError( host, PRINTF_CONTROL_OUTOF_SYNC_ERR_MAJOR, PRINTF_CONTROL_OUTOF_SYNC_ERR_MINOR, haltOnErrorPtr);
       }
        HaltOnError(host, PRINTF_CONTROL_OUTOF_SYNC_ERR_MAJOR, PRINTF_CONTROL_OUTOF_SYNC_ERR_MINOR);
     }
  }
  return 1;
}

void
releasePrintFControl (
                     PSYSHOST host
                     )
/*++

  release ownership of semaphore.
  must be used to match getprintFControl()

  @param host  - Pointer to the system host (root) structure

  @retval N/A

--*/
{


  if (host->var.mem.varStructInit == 0) {
    host->var.mem.serialDebugMsgLvl = host->setup.common.serialDebugMsgLvl;
  }

  if (host->var.common.printfSync && host->var.mem.serialDebugMsgLvl) {
      if (host->var.common.printfDepth == 1) {
       AcquireReleaseGlobalSemaphore (host, SYSTEMSEMAPHORE0, RELEASE_SEMAPHORE);
      }
      // decrement nested call depth
      if (host->var.common.printfDepth != 0) {
       host->var.common.printfDepth--;  
      }
  }
}
#endif // SERIAL_DBG_MSG

int
rcVprintf (
          PSYSHOST   host,
          const INT8 *Format,
          va_list    Marker
          )
/*++

    rcPrintf with stdargs varargs stack frame in place of .... Limited
    support for sizes other than UINT32 to save code space

    @param host   - Pointer to the system host (root) structure
    @param Format - String containing characters to print and formating data.

    %[flags][width]type

    [flags] '-' left align
    [flags] '+' prefix with sign (+ or -)
    [flags] '0' zero pad numbers
    [flags] ' ' prefix black in front of postive numbers

    [width] non negative decimal integer that specifies
            the width to print a value.
    [width] '*' get the width from a int argument on the stack.

    type    'd'|'i' signed decimal integer
    type    'u' unsinged integer
    type    'x'|'X' usinged hex using "ABCDEF"
    type    'c' print character
    type    'p' print a pointer to void
    type    'r' output a raw binary encoding

    Marker - va_list that points to the arguments for Format that are on
                the stack.


    @retval Prints to putchar() - To save space we print every thing as UINT32 and
    @retval don't support type over rides like {h | I | I64 | L}

TODO: fix '%' char :-)

--*/
{
#ifdef SERIAL_DBG_MSG
  INT8    *p;
  UINT32  Width;
  UINT32  Flags;
  INT8    Str[160];
  INT8    *String;
  UINT8   release = 0;
  UINT32  CharCount=0;

  if (host == NULL) {
    return FAILURE;
  }


  if (!host->var.common.serialBufEnable) {
    if (host->var.common.printfSync) {
      release = getPrintFControl (host);
    }
  }

  for (p = (INT8 *) Format, CharCount = 0; *p; p++) {
    if (*p != '%') {
      if (*p == CHAR_LF) {
        //
        // Make LF into CR LF
        //
        PUTCC (CHAR_CR, CharCount);
      }

      PUTCC (*p, CharCount);
    } else {
      p++;
      //
      // Check for flags
      //
      Flags = 0;
      if (*p == '-') {
        Flags |= LEFT_JUSTIFY;
      } else if (*p == '+') {
        Flags |= PREFIX_SIGN;
      } else if (*p == ' ') {
        Flags |= PREFIX_BLANK;
      }

      if (Flags != 0) {
        p++;
      }
      //
      // Check for width
      //
      if (ISDIGIT (*p)) {
        if (*p == '0') {
          Flags |= COMMON_PREFIX_ZERO;
        }

        Width = StrToNumber (&p);
      } else if (*p == '*') {
        Width = va_arg (Marker, int);
        p++;
      } else {
        Width = 0;
      }

      if (*p == ',') {
        Flags |= COMA_TYPE;
        p++;
      }
      //
      // get type
      //
      String = Str;
      switch (*p) {
      case 'd':
      case 'i':
        //
        // always print as UINT32 will need extra code to print different widths
        //
        UintnToStr ((UINT32) va_arg (Marker, UINT32 *), Str, Width, Flags | INT_SIGNED, 10);
        break;

      case 'u':
        //
        // always print as UINT32 will need extra code to print different widths
        //
        UintnToStr ((UINT32) va_arg (Marker, UINT32 *), Str, Width, Flags, 10);
        break;

      case 'X':
        Flags |= TO_UPPER;

      case 'x':
        //
        // always print as UINT32 will need extra code to print different widths
        //
        UintnToStr ((UINT32) va_arg (Marker, UINT32 *), Str, Width, Flags, 16);
        break;

      case 'c':
        PUTCC((INT8) va_arg (Marker, int), CharCount);
        Str[0] = '\0';
        break;

      case 's':
        String = (INT8 *) va_arg (Marker, INT8 *);
        break;

      case 'p':
        UintnToStr ((UINT32) va_arg (Marker, UINT32 *), Str, Width, Flags, 16);
        break;

      case 'r':
        EncodeUINT32 ((UINT32) va_arg (Marker, UINT32 *), Str);
        break;

      case 'R':
        EncodeUINT64 (va_arg (Marker, UINT64_STRUCT), Str);
        break;
      }

      while (*String != '\0') {
        PUTCC (*String++, CharCount);
      }
    }
  }

  if (!host->var.common.serialBufEnable) {
    if (host->var.common.printfSync && release) {
      releasePrintFControl (host);
    }
  } else {
    SerialSendBuffer (host, 0);
  }

  return CharCount;
#else
  //
  // If serial debug is disabled, we simply return 0 characters printed
  //
  return 0;
#endif // SERIAL_DBG_MSG
} //rcVprintf

int
rcPrintfunction (
  PSYSHOST   host,
  const INT8 *Format,
  ...
  )
/*++

  Prints string to serial output

  @param host    - Pointer to the system host (root) structure
  @param Format  - format string for output
  @param ...     - values matching the variables declared in 
                   the Format parameter

    @retval 0 - Success
    @retval 1 - Failure

--*/
{
  //
  // If serial debug is disabled, we simply return 0 characters printed
  //
#ifdef SERIAL_DBG_MSG
#ifdef IA32
  va_list Marker;

  if (host->var.mem.varStructInit == 0) {
    host->var.mem.serialDebugMsgLvl = host->setup.common.serialDebugMsgLvl;
  }

  if (host->var.mem.serialDebugMsgLvl) {
    va_start (Marker, Format);
    return rcVprintf (host, Format, Marker);
  } else {
    return 0;
  }
#else
  // if we need RC print functionality in X64 build, then
  // enable it here
  return 0;
#endif // IA32
#else
  //
  // If serial debug is disabled, we simply return 0 characters printed
  //
  return 0;
#endif // SERIAL_DBG_MSG
}

void
DebugPrintRc (
              PSYSHOST  host,
              UINT32    dbgLevel,
              char*     Format,
              ...
              )
/**

  MRC specific print to serial output

  @param host      - Pointer to the system host (root) structure
  @param dbgLevel  - debug level
  @param Format    - string format 
  @param ...       - values matching the variables declared in 
                     the Format parameter

  @retval N/A

**/
{
#ifdef SERIAL_DBG_MSG
  va_list Marker;
  UINT8   release = 0;

  //
  // Check host for null
  //
  if (host != NULL) {
  
    //
    // Check the debug message level
    //
    if (!checkMsgLevel(host, dbgLevel)) return;

    //
    // Get control of rcPrintf
    //
    if (host->var.common.printfSync) {
      release = getPrintFControl (host);
    }

    va_start (Marker, Format);
    rcVprintf (host, Format, Marker);

    //
    // Release control of rcPrintf if necessary
    //
    if (host->var.common.printfSync && release) {
      releasePrintFControl (host);
    }
  }
#endif // SERIAL_DBG_MSG
  return;
}


