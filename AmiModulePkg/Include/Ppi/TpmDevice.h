/*++

   Copyright (c)  2005 Intel Corporation. All rights reserved
   This software and associated documentation (if any) is furnished
   under a license and may only be used or copied in accordance
   with the terms of the license. Except as permitted by such
   license, no part of this software or documentation may be
   reproduced, stored in a retrieval system, or transmitted in any
   form or by any means without the express written consent of
   Intel Corporation.

   Abstract:

   TPM PPI as defined in EFI 2.0

   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/PPI/TpmDevice_PPI/TpmDevice/TpmDevice.h 7     3/29/11 3:09p Fredericko $
//
// $Revision: 7 $
//
// $Date: 3/29/11 3:09p $
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TpmDevice.h
//
// Description:
//  Header file for TpmDevice.c[Ppi]
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _PEI_TPMDEVICE_H_
#define _PEI_TPMDEVICE_H_

#include  <AmiTcg/TcgEfiTpm.h>
#include  <AmiTcg/TcgEFI12.h>
#include  "TcgPlatformSetupPeiPolicy.h"

#define PEI_TPM_PPI_GUID \
    {0xca4853f4, 0xe94b, 0x42b4, 0x86, 0x42, 0xcd, 0xe2, 0x8a, 0x7f, 0xac, 0x2d}

#define TPM_DEACTIVATED_VARIABLE_NAME \
    L"IsTpmDeactivated"

struct _PEI_TPM_PPI;
typedef struct _PEI_TPM_PPI PEI_TPM_PPI;

typedef
EFI_STATUS
(EFIAPI * PEI_TPM_INIT)(
    IN PEI_TPM_PPI      *This,
    IN EFI_PEI_SERVICES **PeiServices
);

typedef
EFI_STATUS
(EFIAPI * PEI_TPM_CLOSE)(
    IN PEI_TPM_PPI      *This,
    IN EFI_PEI_SERVICES **PeiServices
);

typedef
EFI_STATUS
(EFIAPI * PEI_TPM_GET_STATUS_INFO)(
    IN PEI_TPM_PPI      *This,
    IN EFI_PEI_SERVICES **PeiServices
);


VOID FillDriverLoc (
    UINT32           * Offset,
    EFI_PEI_SERVICES **ps,
    EFI_GUID         *Driveguid );

VOID MAFillDriverLoc (
    MASTRUCT         *Data,
    EFI_PEI_SERVICES **ps,
    EFI_GUID         *Driveguid );

EFI_STATUS FillDriverLocByFile (
    UINT32           * Offset,
    EFI_PEI_SERVICES **ps,
    EFI_GUID         *Driveguid,
    void             **MAStart,
    UINTN            *MASize );


EFI_STATUS
EFIAPI TpmPeiEntry (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES    **PeiServices );


typedef
EFI_STATUS
(EFIAPI * PEI_TPM_TRANSMIT)(
    IN PEI_TPM_PPI             *This,
    IN EFI_PEI_SERVICES        **PeiServices,
    IN UINTN                   NoInputBuffers,
    IN TPM_TRANSMIT_BUFFER     *InputBuffers,
    IN UINTN                   NoOutputBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutputBuffers
);

typedef struct _PEI_TPM_PPI
{
    PEI_TPM_INIT            Init;
    PEI_TPM_CLOSE           Close;
    PEI_TPM_GET_STATUS_INFO GetStatusInfo;
    PEI_TPM_TRANSMIT        Transmit;
} PEI_TPM_PPI;

extern EFI_GUID gPeiTpmPpiGuid;

#endif
