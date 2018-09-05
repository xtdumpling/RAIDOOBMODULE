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

/* Legacy SVEN DEVH Instrumentation API support */

#ifndef SVEN_DEVH_INCLUDED
#define SVEN_DEVH_INCLUDED

#ifndef SVEN_API_H_INCLUDED
#include "./api.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_sven_handle os_devhandle_t;

#if !defined(SVEN_DEVH_DISABLE_SVEN) && !defined(SVEN_DISABLE_SVEN)

#if defined( SVEN_PCFG_ENABLE_DEVH_API )
#define DEVH_DEBUG(handle, str) \
    sventx_WriteDebugString((handle), 0,\
             SVEN_DEBUGSTR_Generic, SVEN_SEVERITY_NONE,\
             0, (str))

#define DEVH_DELETE(h) \
    sventx_DeleteHandle(h)

#endif // defined( SVEN_PCFG_ENABLE_DEVH_API )

#endif /* !defined(SVEN_DEVH_DISABLE_SVEN) && !defined(SVEN_DISABLE_SVEN) */

#ifndef DEVH_DEBUG
#define DEVH_DEBUG(h,s)
#endif

#ifndef DEVH_DELETE
#define DEVH_DELETE(h)
#endif


#ifdef __cplusplus
} /* extern C */
#endif

#endif
