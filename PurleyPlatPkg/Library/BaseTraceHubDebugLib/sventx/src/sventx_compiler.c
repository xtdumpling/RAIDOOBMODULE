/*

  Copyright(c) 2012-2014 Intel Corporation. All rights reserved.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/* Compiler dependent code */

#include "sventx.h"

#if defined(_WIN32)     /* MSVC Compiler section*/

//#include <intrin.h>
#pragma intrinsic(_ReturnAddress)

#if defined(SVEN_PCFG_ENABLE_LOCATION_ADDRESS)
/**
 * Return the instruction pointer address of the instruction
 * that follows this function. It is used to compute
 * location information for SVEN instrumentation calls.
 * These are the calls that end with the _LOCADDR suffix.
 */
SVEN_EXPORT void * SVEN_CALLCONV sventx_ReturnAddress()
{
    return _ReturnAddress();
}
#endif /* defined(SVEN_PCFG_ENABLE_LOCATION_ADDRESS) */



#elif defined(__GNUC__) /* GNU-C Compiler section */



#if defined(SVEN_PCFG_ENABLE_LOCATION_ADDRESS)
/**
 * Return the instruction pointer address of the instruction
 * that follows this function. It is used to compute
 * location information for SVEN instrumentation calls.
 * These are the calls that end with the _LOCADDR suffix.
 */
SVEN_EXPORT void * SVEN_CALLCONV sventx_ReturnAddress()
{
    return __builtin_return_address(0);
}
#endif

#else
#error unknown compiler, copy and adapt one of the sections above
#endif
