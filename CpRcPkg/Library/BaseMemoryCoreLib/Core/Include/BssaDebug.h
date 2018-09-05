/**
Defines BIOS SSA asserts and error printing.

@copyright
  Copyright (c) 2014 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  BssaDebug.h

@brief
  This file contains BIOS SSA error and assert support.
**/

#ifndef __BSSA_DEBUG_H__
#define __BSSA_DEBUG_H__

#ifdef SSA_FLAG
#include <DataTypes.h>
#ifndef __GNUC__
#include <printf.h>
#endif
#ifndef MINIBIOS_BUILD
#include <Library/DebugPrintErrorLevelLib.h>
#endif //MINIBIOS_BUILD

#define _BSSA_DEBUG_ERROR(host, errlevel, format, ...) \
  do{ \
    if ((errlevel & GetDebugPrintErrorLevel()) != 0) { \
      rcPrintf((host, format, __VA_ARGS__)); \
	  } \
  }while(0)


#define _BSSA_ASSERT(host, assert) \
  if(!(assert)) { \
    rcPrintf((host, "Assert failed: %s,  File: %s, Line: %u \n", #assert, __FILE__, __LINE__ )); \
  }


#ifdef SERIAL_DBG_MSG
  #define BSSA_DEBUG_ERROR(host, errlevel, format, ...) _BSSA_DEBUG_ERROR((host), (errlevel), (format), __VA_ARGS__) 
#else
  #define BSSA_DEBUG_ERROR(host, errlevel,format, ...) 
#endif // SERIAL_DBG_MSG


#ifdef SERIAL_DBG_MSG
  #define BSSA_ASSERT(host, assert) _BSSA_ASSERT((host), (assert))
#else
  #define BSSA_ASSERT(host, assert)
#endif // SERIAL_DBG_MSG
#else
//Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
#endif  // __BSSA_DEBUG_H__

