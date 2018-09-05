/*++

   Copyright (c) 2005 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.


   Module Name:

   TcgCommon.h

   Abstract:

   Header file for TcgCommon.c

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/TcgCommon.h 13    3/19/12 6:14p Fredericko $
//
// $Revision: 13 $
//
// $Date: 3/19/12 6:14p $
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TcgCommon.h
//
// Description:
//  Header file for TcgCommon.c
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _TCG_COMMON_H_
#define _TCG_COMMON_H_

#include <Uefi.h>
#include "AmiTcg/TcgPc.h"
#include "AmiTcg/TcgEfiTpm.h"
#include "AmiTcg/sha.h"
#include "AmiTcg/Tpm20.h"
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include "TCGMisc.h"

#define TcgCommonN2HS( v16 ) TcgCommonH2NS( v16 )
#define TcgCommonN2HL( v32 ) TcgCommonH2NL( v32 )
#define EFI_TPL_DRIVER                 6
#define EFI_TPL_APPLICATION         4
#define EFI_TPL_CALLBACK            8
#define EFI_TPL_NOTIFY              16
#define EFI_TPL_HIGH_LEVEL          31
//#define STRING_TOKEN( x ) x


#define TCGGET_HOB_TYPE( Hob )     ((Hob).Header->HobType)
#define TCGGET_HOB_LENGTH( Hob )   ((Hob).Header->HobLength)
#define TCGGET_NEXT_HOB( Hob )     ((Hob).Raw + TCGGET_HOB_LENGTH( Hob ))
#define TCGEND_OF_HOB_LIST( Hob )  (TCGGET_HOB_TYPE( Hob ) ==\
                                 EFI_HOB_TYPE_END_OF_HOB_LIST)

#define TCGPASSTHROUGH( cb, in, out ) \
    TcgCommonPassThrough(  \
        cb, \
        sizeof (in) / sizeof (*(in)), \
        (in), \
        sizeof (out) / sizeof (*(out)), \
        (out) \
        )

#define TCMPASSTHROUGH( cb, in, out ) \
    TcmCommonPassThrough(  \
        cb, \
        sizeof (in) / sizeof (*(in)), \
        (in), \
        sizeof (out) / sizeof (*(out)), \
        (out) \
        )

extern
UINT16
EFIAPI TcgCommonH2NS (
    IN UINT16 Val );

extern
UINT32
EFIAPI TcgCommonH2NL (
    IN UINT32 Val );

VOID* GetHob (
    IN UINT16 Type,
    IN VOID   *HobStart  );

EFI_STATUS TcgGetNextGuidHob(
    IN OUT VOID          **HobStart,
    IN EFI_GUID          * Guid,
    OUT VOID             **Buffer,
    OUT UINTN            *BufferSize OPTIONAL );

extern
EFI_STATUS
EFIAPI TcgCommonPassThrough (
    IN VOID                    *CallbackContext,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers );

extern
EFI_STATUS
EFIAPI TcmCommonPassThrough (
    IN VOID                    *CallbackContext,
    IN UINT32                  NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINT32                  NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers );

extern
VOID
EFIAPI TcgCommonCopyMem (
    IN VOID  *CallbackContext,
    OUT VOID *Dest,
    IN VOID  *Src,
    IN UINTN Len );

EFI_STATUS
EFIAPI TcgCommonLogEvent(
    IN VOID          *CallbackContext,
    IN TCG_PCR_EVENT *EvtLog,
    IN OUT UINT32    *TableSize,
    IN UINT32        MaxSize,
    IN TCG_PCR_EVENT *NewEntry,
    IN UINT8         HashAlgorithm );

extern
EFI_STATUS
EFIAPI TcmCommonLogEvent (
    IN VOID          *CallbackContext,
    IN TCM_PCR_EVENT *EvtLog,
    IN OUT UINT32    *TableSize,
    IN UINT32        MaxSize,
    IN TCM_PCR_EVENT *NewEntry );

extern
EFI_STATUS
EFIAPI TcgCommonSha1Start (
    IN VOID             *CallbackContext,
    IN TCG_ALGORITHM_ID AlgId,
    OUT UINT32          *MaxBytes );

extern
EFI_STATUS
EFIAPI TcgCommonSha1Update (
    IN VOID   *CallbackContext,
    IN VOID   *Data,
    IN UINT32 DataLen,
    IN UINT32 MaxBytes );

extern
EFI_STATUS
EFIAPI TcgCommonSha1CompleteExtend (
    IN VOID         *CallbackContext,
    IN VOID         *Data,
    IN UINT32       DataLen,
    IN TPM_PCRINDEX PCRIndex,
    OUT TCG_DIGEST  *Digest,
    OUT TCG_DIGEST  *NewPCRValue );


EFI_STATUS
EFIAPI TcmCommonSha1CompleteExtend(
    IN VOID         *CallbackContext,
    IN VOID         *Data,
    IN UINT32       DataLen,
    IN TPM_PCRINDEX PCRIndex,
    OUT TCM_DIGEST  *Digest,
    OUT TCM_DIGEST  *NewPCRValue );

extern
EFI_STATUS
EFIAPI TcgCommonExtend (
    IN VOID         *CallbackContext,
    IN TPM_PCRINDEX PCRIndex,
    IN TCG_DIGEST   *Digest,
    OUT TCG_DIGEST  *NewPCRValue );

EFI_STATUS
EFIAPI TcmCommonExtend(
    IN  VOID         *CallbackContext,
    IN  TPM_PCRINDEX PCRIndex,
    IN  TCM_DIGEST   *Digest,
    OUT TCM_DIGEST  *NewPCRValue );

extern
EFI_STATUS
EFIAPI SHA1HashAll (
    IN VOID            *CallbackContext,
    IN VOID            *HashData,
    IN UINTN           HashDataLen,
    OUT UINT8          *Digest
);

EFI_STATUS EfiLibGetSystemConfigurationTable(
    IN EFI_GUID *TableGuid,
    IN OUT VOID **Table );

extern
BOOLEAN
EFIAPI IsTcmSupportType (
);

#pragma pack(push,1)

typedef struct
{
    TPMI_DH_PCR              pcrHandle;
    TPML_DIGEST_VALUES       DigestValue;
} PCR_Extend_In;

typedef struct
{
    TPMI_ST_COMMAND_TAG       Tag;
    UINT32                    CommandSize;
    TPM_CC                    CommandCode;
    TPMI_DH_PCR               PcrHandle;
    UINT32                    AuthorizationSize;
    TPMS_AUTH_COMMAND         AuthSessionPcr;
    TPML_DIGEST_VALUES        DigestValues;
} TPM2_PCRExtend_cmd_t;

typedef struct
{
    TPMI_ST_COMMAND_TAG         Tag;
    UINT32                      RespondSize;
    TPM_RC                      ResponseCode;
    UINT32                      parameterSize;
    TPMS_AUTH_RESPONSE  pwapAuth;
} TPM2_PCRExtend_res_t;

EFI_STATUS
EFIAPI SHA2HashAll(
    IN  VOID            *CallbackContext,
    IN  VOID            *HashData,
    IN  UINTN           HashDataLen,
    OUT UINT8           *Digest
);


#pragma pack(pop)


#endif
