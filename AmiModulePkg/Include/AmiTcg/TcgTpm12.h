/*++

   Copyright (c) 2005 - 2006, Intel Corporation
   All rights reserved. This program and the accompanying materials
   are licensed and made available under the terms and conditions of the BSD License
   which accompanies this distribution.  The full text of the license may be found at
   http://opensource.org/licenses/bsd-license.php

   THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
   WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

   Module Name:

   Tpm12.h

   Abstract:

   TPM Specification data structures (TCG TPM Specification Version 1.2 Revision 94)

   See http://trustedcomputinggroup.org for latest specification updates

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TcgTpm12.h 6     3/29/11 12:25p Fredericko $
//
// $Revision: 6 $
//
// $Date: 3/29/11 12:25p $
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TcgTpm12.h
//
// Description: 
//  Contains Industry Standard #defines for TCG
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _TCG_TPM12_H_
#define _TCG_TPM12_H_

#include "Token.h"
#include <IndustryStandard/Tpm12.h>
#include <IndustryStandard/UefiTcgPlatform.h>
#include "AmiTcg/ZTEICTcmOrdinals.h"

//
// Structures are all packed on 1-byte alignment
//

#pragma pack (push)
#pragma pack (1)


//
// Part 2, section 5.4: TPM_DIGEST
//
#define TPM_SHA1_160_HASH_LEN       0x14
#define TPM_SHA1BASED_NONCE_LEN     TPM_SHA1_160_HASH_LEN

#define TPM_SHA2_256_HASH_LEN       0x20
#define TPM_SHA2BASED_NONCE_LEN     TPM_SHA2_256_HASH_LEN


//redefine TPM digest into a UNION so that we can support 
//sha1 and sha2 and other has functions at the same time
typedef union tdTPM2_DIGEST
{
    UINT8 digest[TPM_SHA1_160_HASH_LEN];
    UINT8 digestSha2[TPM_SHA2_256_HASH_LEN];
} TPM2_DIGEST;

typedef struct tdTCM_DIGEST
{
    UINT8 digest[TPM_SHA2BASED_NONCE_LEN];
} TCM_DIGEST;

#pragma pack (pop)

#endif  // _TPM12_H_
