//
// This file contains an 'Intel Pre-EFI Module' and is licensed
// for Intel CPUs and Chipsets under the terms of your license
// agreement with Intel or your vendor.  This file may be
// modified by the user, subject to additional terms of the
// license agreement
//
/**

  Reference Code

  ESS - Enterprise Silicon Software

  INTEL CONFIDENTIAL

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
  DataTypes.h

@brief
  Common data types used in the Memory Reference Code

**/

#ifndef _datatypes_h
#define _datatypes_h

#ifndef ASM_INC
typedef unsigned char BOOLEAN;
//typedef signed char INT8;  // SKX TODO: All string/ASCII/ANSI character based functions need to be ported to use CHAR type due
// to GCC difference. After that porting occurs, then INT8 should be defined as signed type.  MS compiler treats char as signed value.
// GNU compiler treats char as unsigned value. This creates havoc when trying to make code compatible with runtime libraries. So...
// henceforth all strings usage will be of type CHAR and not INT8.
typedef char CHAR;
#if defined(__GNUC__) && !defined(MINIBIOS_BUILD)
typedef signed char INT8;
#else
typedef char INT8;
#endif
typedef char CHAR8;
typedef unsigned char UINT8;
typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned int UINT32;
typedef unsigned int MMRC_STATUS;
#define MMRC_SUCCESS     0
#define MMRC_FAILURE     0xFFFFFFFF
#ifndef CONST
#define CONST       const
#endif
#ifndef IN
#define             IN
#endif
#ifndef OUT
#define             OUT
#endif
#ifndef NULL
#define NULL        ((VOID *) 0)
#endif
#ifdef MINIBIOS_BUILD
typedef unsigned long long UINT64;
#endif
typedef unsigned char UCHAR8;
typedef unsigned short CHAR16;
//typedef signed long long SINT64;
//typedef signed long SINT32;
//typedef signed short SINT16;
//typedef signed char SINT8;
#define SINT8 INT8
#define SINT16 INT16
#define SINT32 INT32

#define CONST       const
#define STATIC      static
#define VOID        void
#define VOLATILE    volatile

#ifndef TRUE
#define TRUE  ((BOOLEAN) 1 == 1)
#endif
#ifndef FALSE
#define FALSE ((BOOLEAN) 0 == 1)
#endif

typedef UINT64 UINTX;

typedef struct u64_struct {
  UINT32  lo;
  UINT32  hi;
} UINT64_STRUCT, *PUINT64_STRUCT;

typedef struct u128_struct {
  UINT32  one;
  UINT32  two;
  UINT32  three;
  UINT32  four;
} UINT128;

typedef struct {
  UINT32  Data1;
  UINT16  Data2;
  UINT16  Data3;
  UINT8   Data4[8];
} GUID_RC;

#ifndef NT32_BUILD
#if defined  (RC_SIM) || defined (MINIBIOS_BUILD)
typedef GUID_RC EFI_GUID;
#endif // #if defined (RC_SIM) || defined (MINIBIOS_BUILD)

#ifdef MINIBIOS_BUILD
typedef INT32   INTN;
typedef UINT32  UINTN;
typedef UINTN RETURN_STATUS;
#define MAX_BIT (1 << ((sizeof (UINTN) << 3) - 1))
#define ENCODE_ERROR(StatusCode)     ((RETURN_STATUS)(MAX_BIT | (StatusCode)))
#define RETURN_SUCCESS               0
#define RETURN_UNSUPPORTED           ENCODE_ERROR (3)
#define EFIAPI
#define PcdUsraSupportS3 FALSE

#endif // #ifdef MINIBIOS_BUILD
#endif // #ifndef NT32_BUILD


#endif // ASM_INC

#define MAX_STRING_LENGTH 0x100

#endif // _datatypes_h
