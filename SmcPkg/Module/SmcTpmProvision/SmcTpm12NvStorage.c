//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//
//  File History
//
//  Rev. 1.01
//    Bug Fix : Fixed TPM 1.2 command sending failed and SMC_IPMICmd30_68_1B 
//              data 2 bit 4 cannot be set after TPM 1.2 is provisioned.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jun/08/2017
//
//  Rev. 1.00
//    Bug Fix : N/A.
//    Reason  : Initialized source code from Intel.
//    Auditor : Joe Jhang
//    Date    : Jan/09/2017
//
//**************************************************************************//
/** @file
  Implement TPM1.2 Startup related command.

Copyright (c) 2012, Intel Corporation. All rights Reserved. <BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <IndustryStandard/Tpm12.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include "SmcTpmProvision.h"

#pragma pack(1)

typedef struct {
  TPM_RQU_COMMAND_HDR   Hdr;
  TPM_NV_INDEX          NvIndex;
  UINT32                Offset;
  UINT32                DataSize;
} TPM_CMD_NV_READ_VALUE;

typedef struct {
  TPM_RSP_COMMAND_HDR   Hdr;
  UINT32				        DataSize;
  UINT8					        Data[1024];
} TPM_RSP_NV_READ_VALUE;

typedef struct {
  TPM_RQU_COMMAND_HDR   Hdr;
  TPM_NV_INDEX          NvIndex;
  UINT32                Offset;
  UINT32                DataSize;
  UINT8					        Data[1024];
} TPM_CMD_NV_WRITE_VALUE;

typedef struct {
  TPM_RSP_COMMAND_HDR   Hdr;
} TPM_RSP_NV_WRITE_VALUE;

#pragma pack()

/**
  Send NV ReadValue command to TPM1.2.

  @param NvIndex           NvIndex.
  @param Offset            Offset to DataBuffer
  @param DataSize          Size of DataBuffer
  @param Data  	           Data Buffer

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm12NvReadValue (
  IN TPM_NV_INDEX   NvIndex,
  IN UINT32         Offset,
  IN UINT32         DataSize,
  OUT UINT8         *Data
  )
{
  EFI_STATUS                        Status;
  UINT32                            TpmRecvSize;
  UINT32                            TpmSendSize;
  TPM_CMD_NV_READ_VALUE             SendBuffer;
  TPM_RSP_NV_READ_VALUE             RecvBuffer;
  UINT32                            ReturnCode;

  //
  // send Tpm command TPM_ORD_Startup
  //
  TpmRecvSize               = sizeof (TPM_RSP_NV_READ_VALUE);
  TpmSendSize               = sizeof (TPM_CMD_NV_READ_VALUE);
  SendBuffer.Hdr.tag        = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.Hdr.paramSize  = SwapBytes32 (22);
  SendBuffer.Hdr.ordinal    = SwapBytes32 (TPM_ORD_NV_ReadValue);
  SendBuffer.NvIndex        = SwapBytes32 (NvIndex);
  SendBuffer.Offset         = SwapBytes32 (Offset);
  SendBuffer.DataSize       = SwapBytes32 (DataSize);

  Status = Tpm12SubmitCommand (TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, (UINT8 *)&RecvBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ReturnCode = SwapBytes32(RecvBuffer.Hdr.returnCode);
  DEBUG ((DEBUG_INFO, "Tpm12NvReadValue - ReturnCode = %x\n", ReturnCode));
  switch (ReturnCode) {
  case TPM_SUCCESS:
    break;
  default:
    return EFI_DEVICE_ERROR;
  }

  //
  // Return the response
  //
  CopyMem (Data, &RecvBuffer.Data, SwapBytes32(RecvBuffer.DataSize));

  return EFI_SUCCESS;
}

/**
  Send NV WriteValue command to TPM1.2.

  @param NvIndex           NvIndex.
  @param Offset            Offset to DataBuffer
  @param DataSize          Size of DataBuffer
  @param Data 		         Data Buffer

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm12NvWriteValue (
  IN TPM_NV_INDEX   NvIndex,
  IN UINT8          Offset,
  IN UINT32         DataSize,
  IN UINT8          *Data
  )
{
  EFI_STATUS                        Status;
  UINT32                            TpmRecvSize;
  UINT32                            TpmSendSize;
  TPM_CMD_NV_WRITE_VALUE            SendBuffer;
  TPM_RSP_NV_WRITE_VALUE            RecvBuffer;
  UINT32                            ReturnCode;

  //
  // send Tpm command TPM_ORD_Startup
  //
  TpmRecvSize               = sizeof (TPM_RSP_NV_WRITE_VALUE);
  TpmSendSize               = sizeof (TPM_CMD_NV_WRITE_VALUE);
  SendBuffer.Hdr.tag        = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.Hdr.paramSize  = SwapBytes32 (22);
  SendBuffer.Hdr.ordinal    = SwapBytes32 (TPM_ORD_NV_WriteValue);
  SendBuffer.NvIndex        = SwapBytes32 (NvIndex);
  SendBuffer.Offset         = SwapBytes32 (Offset);
  SendBuffer.DataSize       = SwapBytes32 (DataSize);
  CopyMem (SendBuffer.Data, Data, DataSize);

  Status = Tpm12SubmitCommand (TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, (UINT8 *)&RecvBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ReturnCode = SwapBytes32(RecvBuffer.Hdr.returnCode);
  DEBUG ((DEBUG_INFO, "Tpm12NvWritedValue - ReturnCode = %x\n", ReturnCode));
  switch (ReturnCode) {
  case TPM_SUCCESS:
    break;
  default:
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}
