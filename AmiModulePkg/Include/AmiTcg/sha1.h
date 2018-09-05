/*++

   Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.


   Module Name:

   sha1.h

   Abstract:

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/sha1.h 8     3/29/11 12:24p Fredericko $
//
// $Revision: 8 $
//
// $Date: 3/29/11 12:24p $
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  Sha1.h
//
// Description: 
//  Header file for sha1.c
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _CAL_SHA1_H_
#define _CAL_SHA1_H_

#include "TcgTpm12.h"
#include "TcgEfiTpm.h"
#include "token.h"


typedef TPM_ALGORITHM_ID TCG_ALGORITHM_ID;
#define TCG_ALG_SHA          0x00000004      // The SHA1 algorithm

typedef struct __SHA1_CTX
{
    UINT8 M[64];
    TCG_DIGEST Digest;
    EFI_STATUS Status;
    UINT64     Length;
    UINTN      Count;
} SHA1_CTX;

EFI_STATUS
__stdcall SHA1_init (
    IN VOID     *CallbackContext,
    IN SHA1_CTX *Sha1Ctx );

EFI_STATUS
__stdcall SHA1_update (
    IN VOID     *CallbackContext,
    IN SHA1_CTX *Sha1Ctx,
    IN VOID     *Data,
    IN UINTN    DataLen );

EFI_STATUS
__stdcall SHA1_final (
    IN VOID        *CallbackContext,
    IN SHA1_CTX    *Sha1Ctx,
    OUT TCG_DIGEST **Digest
    );

#endif  // _CAL_SHA1_H_
