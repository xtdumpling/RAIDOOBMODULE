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
//  Rev. 1.02
//    Bug Fix : 1. [Fixes] Fixed the system cannot enter to TXT Environment when
//              using TPM 1.2 on production platform.
//              2. [Fixes] Fixed TPM 2.0 BIOS-Based Provisioning table doesn't 
//              set PW flag to PS LCP Data for production platform.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jul/20/2017
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
  This library is used by other modules to send TPM12 command.

Copyright (c) 2012, Intel Corporation. All rights Reserved. <BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _TPM12_COMMAND_LIB_EX_H_
#define _TPM12_COMMAND_LIB_EX_H_

#include <IndustryStandard/Tpm12.h>

/**
  This service enables the sending of commands to the TPM 1.2.

  @param[in]      InputParameterBlockSize  Size of the TPM12 input parameter block.
  @param[in]      InputParameterBlock      Pointer to the TPM12 input parameter block.
  @param[in,out]  OutputParameterBlockSize Size of the TPM12 output parameter block.
  @param[in]      OutputParameterBlock     Pointer to the TPM12 output parameter block.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
**/
EFI_STATUS
Tpm12SubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
);

/**
  Send NV ReadValue command to TPM 1.2.

  @param NvIndex           NvIndex.
  @param Offset            Offset to DataBuffer
  @param DataSize          Size of DataBuffer
  @param Buffer		         DataBuffer

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
  );

/**
  Send NV WriteValue command to TPM 1.2.

  @param NvIndex           NvIndex.
  @param Offset            Offset to DataBuffer
  @param DataSize          Size of DataBuffer
  @param Buffer		         DataBuffer

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
  );

#endif
