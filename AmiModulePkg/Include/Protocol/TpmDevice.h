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


   --*/
//*************************************************************************
// $Header: /Alaska/SOURCE/Modules/TCG/Protocol/TpmDevice_Protocol/TpmDevice/TpmDevice.h 7     3/29/11 3:10p Fredericko $
//
// $Revision: 7 $
//
// $Date: 3/29/11 3:10p $
//*************************************************************************
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  TpmDevice.h
//
// Description:
//  Header file for TpmDevice.c[Protocol]
//
//<AMI_FHDR_END>
//*************************************************************************
#ifndef _EFI_TPM_DEVICE_H_
#define _EFI_TPM_DEVICE_H_

#include "AmiTcg/TcgEfiTpm.h"
#include "AmiTcg/TcgEFI12.h"
#include "AmiTcg/TCGMisc.h"
#include "AmiTcg/TcgCommon12.h"

#define EFI_TPM_DEVICE_PROTOCOL_GUID \
    { 0xde161cfe, 0x1e60, 0x42a1, 0x8c, 0xc3, 0xee, 0x7e, 0xf0, 0x73, 0x52,\
      0x12 }

#define AMI_TCG_PLATFORM_PROTOCOL_GUID\
    {0x8c939604, 0x700, 0x4415, 0x9d, 0x62, 0x11, 0x61, 0xdb, 0x81, 0x64, 0xa6}



typedef struct _EFI_TPM_DEVICE_PROTOCOL EFI_TPM_DEVICE_PROTOCOL;


typedef
EFI_STATUS
(EFIAPI * EFI_TPM_MP_INIT)(
    IN EFI_TPM_DEVICE_PROTOCOL *This
);

/*++

   Routine Description:
    This service Open the TPM interface

   Arguments:
    This              - A pointer to the EFI_TPM_MP_DRIVER_PROTOCOL.

   Returns:
    EFI_SUCCESS       - Operation completed successfully
    EFI_DEVICE_ERROR  - The command was unsuccessful
    EFI_NOT_FOUND     - The component was not running

   --*/

typedef
EFI_STATUS
(EFIAPI * EFI_TPM_MP_CLOSE)(
    IN EFI_TPM_DEVICE_PROTOCOL *This
);
/*++

   Routine Description:
    This service close the TPM interface and deactivate TPM

   Arguments:
    This              - A pointer to the EFI_TPM_MP_DRIVER_PROTOCOL.

   Returns:
    EFI_SUCCESS       - Operation completed successfully
    EFI_DEVICE_ERROR  - The command was unsuccessful
    EFI_NOT_FOUND     - The component was not running

   --*/

typedef
EFI_STATUS
(EFIAPI * EFI_TPM_MP_GET_STATUS_INFO)(
    IN EFI_TPM_DEVICE_PROTOCOL *This
);
/*++

   Routine Description:
    This service get the current status infomation of TPM

   Arguments:
    This              - A pointer to the EFI_TPM_MP_DRIVER_PROTOCOL.
    ReqStatusType     - Requested type of status information, driver or device.
    Status            - Pointer to the returned status.

   Returns:
    EFI_SUCCESS           - Operation completed successfully
    EFI_DEVICE_ERROR      - The command was unsuccessful
    EFI_INVALID_PARAMETER - One or more of the parameters are incorrect
    EFI_BUFFER_TOO_SMALL  - The receive buffer is too small
    EFI_NOT_FOUND         - The component was not running

   --*/


typedef
EFI_STATUS
(EFIAPI * EFI_TPM_MP_TRANSMIT)(
    IN EFI_TPM_DEVICE_PROTOCOL *This,
    IN UINTN                   NoInBuffers,
    IN TPM_TRANSMIT_BUFFER     *InBuffers,
    IN UINTN                   NoOutBuffers,
    IN OUT TPM_TRANSMIT_BUFFER *OutBuffers
);
/*++

   Routine Description:
    This service transmit data to the TPM and get response from TPM

   Arguments:
    This            - A pointer to the EFI_TPM_MP_DRIVER_PROTOCOL.

   Returns:
    EFI_SUCCESS           - Operation completed successfully
    EFI_DEVICE_ERROR      - The command was unsuccessful
    EFI_INVALID_PARAMETER - One or more of the parameters are incorrect
    EFI_BUFFER_TOO_SMALL  - The receive buffer is too small
    EFI_NOT_FOUND         - The component was not running

   --*/


void Prepare2Thunkproc (
    UINT32 TcgOffset,
    UINT16 TcgSelector );

void TcmDxeCallMPDriver (
    IN UINTN               CFuncID,
    TPMTransmitEntryStruct *CData,
    UINT32* OUT            CRetVal );

void TpmDxeCallMPDriver (
    IN UINTN               CFuncID,
    TPMTransmitEntryStruct *CData,
    UINT32* OUT            CRetVal );


typedef struct _EFI_TPM_DEVICE_PROTOCOL
{
    EFI_TPM_MP_INIT            Init;
    EFI_TPM_MP_CLOSE           Close;
    EFI_TPM_MP_GET_STATUS_INFO GetStatusInfo;
    EFI_TPM_MP_TRANSMIT        Transmit;
};

extern EFI_GUID gEfiTpmDeviceProtocolGuid;
extern AmiTcgPlatformProtocolguid;

#endif
