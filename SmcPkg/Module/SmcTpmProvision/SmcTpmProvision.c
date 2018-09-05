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
//  Rev. 1.10
//    Bug Fix : 1. [Fixes] Fixed the system cannot enter to TXT Environment when
//              using TPM 1.2 on production platform.
//              2. [Fixes] Fixed TPM 2.0 BIOS-Based Provisioning table doesn't 
//              set PW flag to PS LCP Data for production platform.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jul/20/2017
//
//  Rev. 1.09
//    Bug Fix : Fixed TPM 1.2 command sending failed and SMC_IPMICmd30_68_1B 
//              data 2 bit 4 cannot be set after TPM 1.2 is provisioned.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jun/08/2017
//
//  Rev. 1.08
//    Bug Fix : N/A
//    Reason  : Implemented BIOS-based TPM provisioning finished flag for SUM.
//    Auditor : Joe Jhang
//    Date    : May/24/2017
//
//  Rev. 1.07
//    Bug Fix : Fixed memory leaks, the caller is responsible for freeing the 
//              buffer that is allocated by callee for ProvisionBuffer.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Feb/02/2017
//
//  Rev. 1.06
//    Bug Fix : N/A.
//    Reason  : Added the debug routine for dumping ME storage binary data.
//    Auditor : Joe Jhang
//    Date    : Jan/26/2017
//
//  Rev. 1.05
//    Bug Fix : Fixed HECI returns "EFI_DEVICE_ERROR".
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jan/25/2017
//
//  Rev. 1.04
//    Bug Fix : Fixed TPM 2.0 provisioning failed by built-in TPM provisioning table.
//    Reason  : Bug Fixed.
//    Auditor : Joe Jhang
//    Date    : Jan/23/2017
//
//  Rev. 1.03
//    Bug Fix : N/A.
//    Reason  : Added BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT token in SDL and
//              set to disabled by default for provisioning TPM remotely.
//    Auditor : Joe Jhang
//    Date    : Jan/20/2017
//
//  Rev. 1.02
//    Bug Fix : N/A.
//    Reason  : Implemented PPI-x OOB to get the complete binary table by 
//              walking through ME storage for provisioning TPM remotely.
//    Auditor : Joe Jhang
//    Date    : Jan/11/2017
//
//  Rev. 1.01
//    Bug Fix : N/A.
//    Reason  : Implemented PPI-x OOB to check TPM provisioning table signature.
//    Auditor : Joe Jhang
//    Date    : Jan/10/2017
//
//  Rev. 1.00
//    Bug Fix : N/A.
//    Reason  : Initialized source code from Intel.
//    Auditor : Joe Jhang
//    Date    : Jan/09/2017
//
//**************************************************************************//
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/** @file
  This file contains the algorithms and helper functions to 
  provision a TPM 1.2 or TPM 2.0

  Copyright (c) 2014, Intel Corporation. All rights Reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#include <Token.h>
#include <PiDxe.h>

#include <Protocol/TrEEProtocol.h>
#include <Protocol/TcgService.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadedImage.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/Tpm2CommandLib.h>
#include <SmcTpm12CommandLibEx.h>
#include <Library/MeStorageLib.h>
#include <SmcLib.h>
#include <Setup.h>

#include "SmcTpmProvision.h"

#define PROV_MAX_RESPONSE          250      //maximum size for a response (min = 30 +(2*MAX_DIGEST))

#pragma pack (1)

typedef struct {
  UINT16  tag;  
  UINT32  responseSize;  
  UINT32  responseCode;  
  UINT8   responseParams[PROV_MAX_RESPONSE-10];  
} TPM_RESPONSE;

#pragma pack ()

//
// Global Varibles
//
EFI_TREE_PROTOCOL  *gTrEEProtocol;
EFI_TCG_PROTOCOL   *gTcgProtocol;

EFI_GUID gTpm12ProvisionTableGuid = TPM12_PROVISION_TABLE_GUID;
EFI_GUID gTpm20ProvisionTableGuid = TPM20_PROVISION_TABLE_GUID;

/**

  @brief 
  Get an image from firmware volume

  @param[in] Fv                     The firmware volume protocol instance
  @param[in] NameGuid               GUID of file
  @param[in] SectionType            Type of section
  @param[out] Buffer                Image content buffer
  @param[out] Size                  Image size

  @retval EFI_SUCCESS               Successfully completed.
  @retval EFI_WARN_BUFFER_TOO_SMALL Too small a buffer is to contain image content
  @retval EFI_ACCESS_DENIED         Access firmware volume error.

**/
EFI_STATUS
GetImageFromFv (
  IN  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv,
  IN  EFI_GUID                      *NameGuid,
  IN  EFI_SECTION_TYPE              SectionType,
  OUT VOID                          **Buffer,
  OUT UINTN                         *Size
  )
{
  EFI_STATUS              Status;
  EFI_FV_FILETYPE         FileType;
  EFI_FV_FILE_ATTRIBUTES  Attributes;
  UINT32                  AuthenticationStatus;

  ///
  /// Read desired section content in NameGuid file
  ///
  *Buffer = NULL;
  *Size   = 0;
  Status = Fv->ReadSection (
                Fv,
                NameGuid,
                SectionType,
                0,
                Buffer,
                Size,
                &AuthenticationStatus
                );

  if (EFI_ERROR (Status) && (SectionType == EFI_SECTION_TE)) {
    ///
    /// Try reading PE32 section, since the TE section does not exist
    ///
    *Buffer = NULL;
    *Size   = 0;
    Status = Fv->ReadSection (
                  Fv,
                  NameGuid,
                  EFI_SECTION_PE32,
                  0,
                  Buffer,
                  Size,
                  &AuthenticationStatus
                  );
  }

  if (EFI_ERROR (Status) && ((SectionType == EFI_SECTION_TE) || (SectionType == EFI_SECTION_PE32))) {
    ///
    /// Try reading raw file, since the desired section does not exist
    ///
    *Buffer = NULL;
    *Size   = 0;
    Status = Fv->ReadFile (
                  Fv,
                  NameGuid,
                  Buffer,
                  Size,
                  &FileType,
                  &Attributes,
                  &AuthenticationStatus
                  );
  }

  return Status;
}

/**

  @brief 
  Get specified image from a firmware volume.

  @param[in] ImageHandle          Image handle for the loaded driver
  @param[in] NameGuid             File name GUID
  @param[in] SectionType          Section type
  @param[in] Buffer               Bufer to contain image
  @param[in] Size                 Image size
  @param[in] WithinImageFv        Whether or not in a firmware volume

  @retval EFI_INVALID_PARAMETER   Invalid parameter
  @retval EFI_NOT_FOUND           Can not find the file
  @retvalEFI_SUCCESS             Successfully completed

**/
EFI_STATUS
GetImageEx (
  IN  EFI_HANDLE         ImageHandle,
  IN  EFI_GUID           *NameGuid,
  IN  EFI_SECTION_TYPE   SectionType,
  OUT VOID               **Buffer,
  OUT UINTN              *Size,
  BOOLEAN                WithinImageFv
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *ImageFv;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;

  DEBUG ((DEBUG_INFO, "GetImageEx() Start\n"));

  if (ImageHandle == NULL && WithinImageFv) {
    return EFI_INVALID_PARAMETER;
  }

  Status  = EFI_NOT_FOUND;
  ImageFv = NULL;
  if (ImageHandle != NULL) {
    Status = gBS->HandleProtocol (
                    ImageHandle,
                    &gEfiLoadedImageProtocolGuid,
                    &LoadedImage
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->HandleProtocol (
                    LoadedImage->DeviceHandle,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    &ImageFv
                    );
    if (!EFI_ERROR (Status)) {
      Status = GetImageFromFv (ImageFv, NameGuid, SectionType, Buffer, Size);
    }
  }

  
  if (Status == EFI_SUCCESS || WithinImageFv) {
    return Status;
  }
  
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Find desired image in all Fvs
  ///
  
  for (Index = 0; Index < HandleCount; ++Index) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );

    if (EFI_ERROR (Status)) {
      (gBS->FreePool) (HandleBuffer);
      return Status;
    }

    if (ImageFv != NULL && Fv == ImageFv) {
      continue;
    }

    Status = GetImageFromFv (Fv, NameGuid, SectionType, Buffer, Size);

    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  
  (gBS->FreePool) (HandleBuffer);

  ///
  /// Not found image
  ///
  if (Index == HandleCount) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "GetImageEx() End\n"));

  return EFI_SUCCESS;
}

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
Tpm12SubmitCommand(
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
  EFI_STATUS                Status;
  TPM_RSP_COMMAND_HDR       *Header;

  if (gTcgProtocol != NULL) {
    //
    // Assumes when TCG Protocol is ready, RequestUseTpm already done.
    //
    Status = gTcgProtocol->PassThroughToTpm(
                             gTcgProtocol,
                             InputParameterBlockSize,
                             InputParameterBlock,
                             *OutputParameterBlockSize,
                             OutputParameterBlock
                             );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    Header = (TPM_RSP_COMMAND_HDR *)OutputParameterBlock;
    *OutputParameterBlockSize = SwapBytes32(Header->paramSize);
  }
  return EFI_SUCCESS;
}

/** 
  This function sends a command to a TPM 1.2 

  @param[in]      InputParameterBlockSize  Size of the TPM12 input parameter block.
  @param[in]      InputParameterBlock      Pointer to the TPM12 input parameter block.
  @param[out]     ResponseCode             The response code returned from TPM12 device.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
**/ 
EFI_STATUS
Tpm12SendCommand (
  IN UINT32  InputParameterBlockSize, 
  IN UINT8   *InputParameterBlock, 
  OUT UINT32 *ResponseCode
  ) 
{
  EFI_STATUS    Status;
  UINT32        ResponseBufferSize;
  TPM_RESPONSE  ResponseBuffer;
  
  ResponseBufferSize = sizeof(ResponseBuffer);
  Status = Tpm12SubmitCommand (
             InputParameterBlockSize,
             InputParameterBlock,
             &ResponseBufferSize,
             (UINT8 *)&ResponseBuffer
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Tpm12SubmitCommand - %r\n", Status));
    return EFI_DEVICE_ERROR;
  } else {
    *ResponseCode = SwapBytes32 (ResponseBuffer.responseCode);
    DEBUG ((EFI_D_INFO, "ResponseCode: 0x%04x \n", *ResponseCode));
    return EFI_SUCCESS;
  }  
}

/** 
  This function sends a command to a TPM 2.0
 
  @param[in]       UINT32             msgSize           TPM message size.
  @param[in]       UINT8              *command          Pointer to command.
  @param[in, out]  UINT32             *RC               Pointer to TPM return code.
  
  @retval EFI_SUCCESS  Procedure returned successfully. 
  @retval EFI_DEVICE_ERROR   Procedure failed. 
**/ 
EFI_STATUS
Tpm20SendCommand ( 
  IN UINT32  InputParameterBlockSize, 
  IN UINT8   *InputParameterBlock, 
  OUT UINT32 *ResponseCode
  )
{
  EFI_STATUS    Status;
  UINT32        ResponseBufferSize;
  TPM_RESPONSE  ResponseBuffer;    

  ResponseBufferSize = sizeof(ResponseBuffer);
  Status = Tpm2SubmitCommand (
             InputParameterBlockSize,
             InputParameterBlock,
             &ResponseBufferSize,
             (UINT8 *)&ResponseBuffer
             );
  if (EFI_ERROR (Status)) {
    *ResponseCode = SwapBytes32 (ResponseBuffer.responseCode);
    DEBUG ((EFI_D_INFO, "ResponseCode: 0x%04x \n", *ResponseCode));
    DEBUG ((EFI_D_INFO, "Tpm2SubmitCommand returned %r\n", Status));
    return EFI_DEVICE_ERROR;
  } else {
    *ResponseCode = SwapBytes32 (ResponseBuffer.responseCode);
    DEBUG ((EFI_D_INFO, "ResponseCode: 0x%04x \n", *ResponseCode));
    return EFI_SUCCESS;
  }  
}


/** 
  Function to Detect if TPM2 Index already Exists

  It performs this operation by sending the read public command with the index value.
 
  @param[in]        NvIndex          Value of the NV index.
  
  @retval TRUE      Index exists. 
  @retval FALSE     Index does not exist. 
**/ 
BOOLEAN 
Tpm2IndexExist (
  IN UINT32 NvIndex
  ) 
{
  EFI_STATUS                Status;
  TPM2B_NV_PUBLIC           NvPublic;
  TPM2B_NAME                NvName;

  ZeroMem (&NvPublic, sizeof(NvPublic));
  ZeroMem (&NvName, sizeof(NvName));

  Status = Tpm2NvReadPublic (NvIndex, &NvPublic, &NvName);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    DEBUG ((EFI_D_ERROR, "ReadPublic - TPM NvIndex(0x%08x) device error\n", NvIndex));
    return FALSE;
  }
  if (Status == EFI_NOT_FOUND) {
    // TPM NvIndex not found
    DEBUG ((EFI_D_ERROR, "ReadPublic - TPM NvIndex(0x%08x) not found\n", NvIndex));
    return FALSE;
  } else {
    // TPM NvIndex found
    DEBUG ((EFI_D_ERROR, "ReadPublic - TPM NvIndex(0x%08x) found\n", NvIndex));
    return TRUE;
  }
}

#ifdef EFI_DEBUG
/**
  DumpBinaryData(): Dump binary data.

  @param  Data IN: UINT8*, Ptr to data
  @param  Size IN: UINTN, Data size in bytes

  @retval None
**/
VOID
DumpBinaryData (
  IN UINT8 *Data,
  IN UINTN Size
  )
{
  UINTN  i, n;
  UINT8  b, *c, j, l;

  n = (Size >> 4);
  l = (UINT8) (Size & 0x0F);
  c = Data;
  DEBUG ((EFI_D_INFO, "----------------------------------------------------------------------\n"));
  DEBUG ((EFI_D_INFO, "Offset  00  01  02  03  04  05  06  07  08  09  0A  0B  0C  0D  0E  0F\n"));
  DEBUG ((EFI_D_INFO, "----------------------------------------------------------------------\n"));
  if (n != 0) {
    for (i = 0; i < n; i++) {
      DEBUG ((EFI_D_INFO, "%04X  ", (i << 4)));
      for (j = 0; j < 16; j++) {
        b = *c; c++;
        DEBUG ((EFI_D_INFO, "  %02X", b));
      }
      DEBUG ((EFI_D_INFO, "\n"));
    }
  }
  if (l != 0) {
    DEBUG ((EFI_D_INFO, "%04X  ", (n << 4)));
    for (j = 0; j < l; j++) {
      b = *c; c++;
      DEBUG ((EFI_D_INFO, "  %02X", b));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  }
  DEBUG ((EFI_D_INFO, "----------------------------------------------------------------------\n"));
}
#endif  //#ifdef EFI_DEBUG

/** 
  The function writelocks an index. 
 
  It performs this operation by sending the writelock command with the index value
 
  @param[in]       EFI_TREE_PROTOCOL  *TrEEProtocol     Handle for the TrEE TPM 2.0 protocol.    
  @param[in]       UINT32              NvIndex          Value of the NV index.
  
  @retval TRUE    Command returned successfully. 
  @retval FALSE     Command failed. 
**/ 

BOOLEAN 
Tpm2WriteLock (
  IN UINT32 NvIndex
  ) 
{
  EFI_STATUS                Status;

  Status = Tpm2NvWriteLock (NvIndex, NvIndex, NULL);
  if (Status == EFI_SUCCESS) {
    return TRUE;
  } else {
    return FALSE;
  }
}

#if (defined(BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT) && (BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT == 1))
/** 
  This function provisions a TPM 1.2 
 
  The function opens the TPM 1.2 provisioning table, reads and sends 
  the provisioning commands to create the Platform Supplier, Aux, and 
  SGX indexes. The algorithm also checks and writes the NV data for each
  index that requires it.
**/ 
VOID  
Tpm1Provision (
  VOID
  ) 
{
  UINT8                      Index;
  PROV1_TABLE                *Prov1Table;
  UINT32                     RespCode;
  EFI_STATUS                 Status;
  VOID                       *ProvisionBuffer;
  UINTN                      ProvisionBufferSize;
  BOOLEAN                    Provisioned = FALSE;
  BIOS_TPM_CAPABILITIES_REQ  CommandData = {0};

  DEBUG ((EFI_D_INFO, "TPM 1.2 Provisioning \n"));

  //
  // Get provision table
  //
  Status = GetImageEx (
               gImageHandle, 
               &gTpm12ProvisionTableGuid,
               EFI_SECTION_RAW,
               &ProvisionBuffer,
               &ProvisionBufferSize,
               FALSE
               );
  
  
  if (EFI_ERROR (Status)) {
    return ;
  }
 
  Prov1Table = ProvisionBuffer;
 
  if (Prov1Table->Major != 1) {
    DEBUG ((EFI_D_INFO, "Tpm1Provision() TPM1.2 Provision Table version incorrect.\n"));
    return;
  }
  
  for (Index = 0; Index < Prov1Table->NumIndexes; Index++)  {   
    
    if (0xCC == *((UINT8 *)ProvisionBuffer + Prov1Table->Params[Index].CreateCmdOffset + 0x9) ) {
      Status = Tpm12SendCommand (
                 Prov1Table->Params[Index].CreateCmdSize,
                 (UINT8 *)ProvisionBuffer + Prov1Table->Params[Index].CreateCmdOffset,
                 &RespCode
                 );
    }

    if (Prov1Table->Params[Index].bWrite) { 

      if ( (0xCD == *((UINT8 *)ProvisionBuffer + Prov1Table->Params[Index].WriteCmdOffset + 0x9)) && (0x00000150 != *((UINT32 *)((UINT8 *)ProvisionBuffer + Prov1Table->Params[Index].WriteCmdOffset + 0xA))) ) {
        Status = Tpm12SendCommand (
                   Prov1Table->Params[Index].WriteCmdSize,
                   (UINT8 *)ProvisionBuffer + Prov1Table->Params[Index].WriteCmdOffset,
                   &RespCode
                   );
        if (!EFI_ERROR(Status) && RespCode == 0) {
          Provisioned = TRUE;
        }
      }
      if (Prov1Table->Params[Index].bWriteProtect) { 
        Status = Tpm12NvWriteValue (Prov1Table->Params[Index].NvIndex, 0, 0, NULL);
      } 
    } 
  }

  if (Provisioned == TRUE) {
    SMC_IPMICmd30_68_1B (0, &CommandData);
    CommandData.StatusFlag |= BIT4;  // TPM provisioning finished.
    SMC_IPMICmd30_68_1B (1, &CommandData);
  }

  if (ProvisionBuffer != NULL) {
    FreePool (ProvisionBuffer);
    ProvisionBuffer = NULL;
  }

  return; 
} 

/** 
  This function provisions a TPM 2.0
 
  The function opens the TPM 2.0 provisioning table, reads and sends 
  the provisioning commands to create the Platform Supplier, Aux, and 
  SGX indexes. The algorithm also checks and writes the NV data for each
  index that requires it.
**/
VOID
Tpm2Provision (
  VOID
  )
{
  EFI_STATUS                 Status;
  UINT32                     RespCode;
  UINT8                      AlgIndex;
  UINT8                      Index;
  NV_INFO                    *IndexInfo;
  PROV2_TABLE                *Prov2Table;
  UINT32                     NvInfoOffset;
  VOID                       *ProvisionBuffer;
  UINTN                      ProvisionBufferSize;
  BOOLEAN                    Provisioned = FALSE;
  BIOS_TPM_CAPABILITIES_REQ  CommandData = {0};

  DEBUG ((EFI_D_INFO, "TPM 2.0 Provisioning \n"));

  //
  // Get provision table
  //
  Status = GetImageEx (
               gImageHandle, 
               &gTpm20ProvisionTableGuid,
               EFI_SECTION_RAW,
               &ProvisionBuffer,
               &ProvisionBufferSize,
               FALSE
               );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Can't get TPM2.0 Provision Table\n"));
    return ;
  }

  Prov2Table = ProvisionBuffer;

  if (Prov2Table->NumAlgs == 0) {
    return;
  }

  //
  // Finds an algorithm on disc and check to see if the TPM2 supports it.
  //
  for (AlgIndex = 0; AlgIndex < Prov2Table->NumAlgs; AlgIndex++) {

    Status = Tpm20SendCommand (
               Prov2Table->AlgParams[AlgIndex].SetPhPolSize,
               (UINT8 *)ProvisionBuffer + Prov2Table->AlgParams[AlgIndex].SetPhPolOffset,
               &RespCode
               );

    if ((Status == EFI_SUCCESS) && (RespCode == 0)) { // Then stop looking and use this algorithm.
      DEBUG ((EFI_D_INFO, "Found! Then stop looking and use this algorithm.\n"));
      break;
    }
  }

  if (AlgIndex == Prov2Table->NumAlgs) {
    return;
  }

  if (Prov2Table->AlgParams[AlgIndex].NumIndexes == 0) {
    return;
  }

  //
  // Gets a pointer to the begining of the Nv Info structures
  //

  NvInfoOffset = Prov2Table->AlgParams[AlgIndex].ParamsOffset;
  
  for (Index = 0; Index < Prov2Table->AlgParams[AlgIndex].NumIndexes; Index++) {
    IndexInfo = (VOID *)((UINT8 *)ProvisionBuffer + NvInfoOffset);

    if (!Tpm2IndexExist (IndexInfo->NvIndex)) {
           
      Status = Tpm20SendCommand (
                 IndexInfo->CreateCmdSize,
                 (UINT8 *)ProvisionBuffer + IndexInfo->CreateCmdOffset,
                 &RespCode
                 );

      if (IndexInfo->bWrite) {
               
        Status = Tpm20SendCommand (
                   IndexInfo->WriteCmdSize,
                   (UINT8 *)ProvisionBuffer + IndexInfo->WriteCmdOffset,
                   &RespCode
                   );
        if (!EFI_ERROR(Status) && RespCode == 0) {
          Provisioned = TRUE;
        }
        //      
        // Write-Protect the index if required, but only if write was successful.
        //
        if ((Status == EFI_SUCCESS) && (RespCode == 0) && (IndexInfo->bWriteProtect)) {
          Tpm2WriteLock (IndexInfo->NvIndex); 
        } 
      }
      
    } 
    NvInfoOffset += Prov2Table->AlgParams[AlgIndex].NvInfoSize;  
  }

  if (Provisioned == TRUE) {
    SMC_IPMICmd30_68_1B (0, &CommandData);
    CommandData.StatusFlag |= BIT4;  // TPM provisioning finished.
    SMC_IPMICmd30_68_1B (1, &CommandData);
  }

  if (ProvisionBuffer != NULL) {
    FreePool (ProvisionBuffer);
    ProvisionBuffer = NULL;
  }

  return; 
}

#else

/** 
  This function provisions a TPM 1.2 by PPI-x OOB.
 
  The function opens the TPM 1.2 provisioning table, reads and sends 
  the provisioning commands to create the Platform Supplier, Aux, and 
  SGX indexes. The algorithm also checks and writes the NV data for each
  index that requires it.
**/ 
VOID  
SmcTpm1ProvisionByPpiXOob (
  VOID
  ) 
{
  EFI_STATUS                  Status                    = EFI_SUCCESS;
  UINT8                       Index                     = 0;
  UINT8                       pStatus                   = 0;
  PROV1_TABLE                 *Prov1TblHeaderBuffer     = NULL;
  VOID                        *Prov1TblBuffer           = NULL;
  UINT32                      Prov1TblBufferSize        = 0;
  UINT32                      Prov1TblEntries           = 0;
  UINT8                       *Prov1TblWalkThrough      = NULL;
  UINT32                      MeStorageEntryMaxBuffSize = ME_STORAGE_MAX_BUFFER;

  PROV1_TABLE                 *Prov1Table;
  UINT32                      RespCode;

  DEBUG ((EFI_D_INFO, "SMC TPM 1.2 Provisioning by PPI-x OOB start\n"));

  //
  // Gets the signature of this binary table.
  //
  Prov1TblHeaderBuffer = AllocateZeroPool (MeStorageEntryMaxBuffSize);
  pStatus = 0;
  Status = MeStorageEntryRead (
             Index,
             &pStatus,
             Prov1TblHeaderBuffer,
             &MeStorageEntryMaxBuffSize
             );
  DEBUG ((EFI_D_INFO, "SmcTpm1ProvisionByPpiXOob MeStorageEntryRead Prov1TblHeader - %r\n", Status));

  //
  // Checks provision table signature.
  //
  DEBUG ((EFI_D_INFO, "SmcTpm1ProvisionByPpiXOob Prov1TblHeaderBuffer->Signature - %lX\n", Prov1TblHeaderBuffer->Signature));
  DEBUG((EFI_D_INFO, "SmcTpm1ProvisionByPpiXOob TPM12_PROVISION_TABLE_SIGNATURE - %lX\n", TPM12_PROVISION_TABLE_SIGNATURE));
  if (Prov1TblHeaderBuffer->Signature != TPM12_PROVISION_TABLE_SIGNATURE) {
    DEBUG ((EFI_D_INFO, "SmcTpm1ProvisionByPpiXOob: This is not a TPM 1.2 PPI-x OOB provisioning request.\n"));

    if (Prov1TblHeaderBuffer != NULL) {
      FreePool (Prov1TblHeaderBuffer);
      Prov1TblHeaderBuffer = NULL;
    }
    return;
  }

  //
  // Gets total length of this binary table.
  //
  Prov1TblBufferSize = Prov1TblHeaderBuffer->Length;
  Prov1TblBuffer = AllocateZeroPool (Prov1TblBufferSize);

  //
  // Calculates entry number that this binary table was divided to be stored in the ME storage.
  //
  Prov1TblEntries = Prov1TblBufferSize / ME_STORAGE_MAX_BUFFER;
  if ((Prov1TblBufferSize % ME_STORAGE_MAX_BUFFER) != 0) {
    Prov1TblEntries += 1;
  }
  DEBUG ((EFI_D_INFO, "SmcTpm1ProvisionByPpiXOob Prov1TblEntries: %x\n", Prov1TblEntries));

  //
  // Error handling for invalid entry number.
  //
  if (Prov1TblEntries > ME_STORAGE_MAX_INDEX + 1) {
    DEBUG ((EFI_D_INFO, "SmcTpm1ProvisionByPpiXOob: The length of TPM 1.2 PPI-x OOB provisioning table is too big to retrieve from ME storage.\n"));

    if (Prov1TblBuffer != NULL) {
      FreePool (Prov1TblBuffer);
      Prov1TblBuffer = NULL;
    }

    if (Prov1TblHeaderBuffer != NULL) {
      FreePool (Prov1TblHeaderBuffer);
      Prov1TblHeaderBuffer = NULL;
    }
    return;
  }
  ASSERT (Prov1TblEntries <= ME_STORAGE_MAX_INDEX + 1);

  //
  // Walks through ME storage to get the complete binary table.
  //
  if (Prov1TblEntries <= ME_STORAGE_MAX_INDEX + 1) {
    Prov1TblWalkThrough = (UINT8 *)Prov1TblBuffer;
    for (Index = 0; Index < Prov1TblEntries; Index++) {

      Status = MeStorageEntryRead (
                 Index,
                 &pStatus,
                 Prov1TblWalkThrough,
                 &MeStorageEntryMaxBuffSize
                 );
      DEBUG ((EFI_D_INFO, "SmcTpm1ProvisionByPpiXOob MeStorageEntryRead Prov1TblWalkThrough - %r, MeStorageEntry: 0x%x\n", Status, Index));
      Prov1TblWalkThrough += MeStorageEntryMaxBuffSize;
    }
  }
#ifdef EFI_DEBUG
  DumpBinaryData (Prov1TblBuffer, Prov1TblBufferSize);
#endif  //#ifdef EFI_DEBUG

  Prov1Table = Prov1TblBuffer;

  if (Prov1Table->Major != 1) {
    DEBUG ((EFI_D_INFO, "Tpm1Provision() TPM 1.2 Provision Table version incorrect.\n"));
    return;
  }

  for (Index = 0; Index < Prov1Table->NumIndexes; Index++)  {   

    if (0xCC == *((UINT8 *)Prov1TblBuffer + Prov1Table->Params[Index].CreateCmdOffset + 0x9) ) {
      Status = Tpm12SendCommand (
                 Prov1Table->Params[Index].CreateCmdSize,
                 (UINT8 *)Prov1TblBuffer + Prov1Table->Params[Index].CreateCmdOffset,
                 &RespCode
                 );
    }



    if (Prov1Table->Params[Index].bWrite) { 
     
      if ( (0xCD == *((UINT8 *)Prov1TblBuffer + Prov1Table->Params[Index].WriteCmdOffset + 0x9)) && (0x00000150 != *((UINT32 *)((UINT8 *)Prov1TblBuffer + Prov1Table->Params[Index].WriteCmdOffset + 0xA))) ) {
        Status = Tpm12SendCommand (
                   Prov1Table->Params[Index].WriteCmdSize,
                   (UINT8 *)Prov1TblBuffer + Prov1Table->Params[Index].WriteCmdOffset,
                   &RespCode
                   );
      }
      if (Prov1Table->Params[Index].bWriteProtect) { 
        Status = Tpm12NvWriteValue (Prov1Table->Params[Index].NvIndex, 0, 0, NULL);
      } 
    } 
  }

  if (Prov1TblBuffer != NULL) {
    FreePool (Prov1TblBuffer);
    Prov1TblBuffer = NULL;
  }
  if (Prov1TblHeaderBuffer != NULL) {
    FreePool (Prov1TblHeaderBuffer);
    Prov1TblHeaderBuffer = NULL;
  }

  DEBUG ((EFI_D_INFO, "SMC TPM 1.2 Provisioning by PPI-x OOB end\n"));
  return;
} 

/** 
  This function provisions a TPM 2.0 by PPI-x OOB.
 
  The function opens the TPM 2.0 provisioning table, reads and sends 
  the provisioning commands to create the Platform Supplier, Aux, and 
  SGX indexes. The algorithm also checks and writes the NV data for each
  index that requires it.
**/
VOID
SmcTpm2ProvisionByPpiXOob (
  VOID
  )
{
  EFI_STATUS                  Status                    = EFI_SUCCESS;
  UINT8                       Index                     = 0;
  UINT8                       pStatus                   = 0;
  PROV2_TABLE                 *Prov2TblHeaderBuffer     = NULL;
  VOID                        *Prov2TblBuffer           = NULL;
  UINT32                      Prov2TblBufferSize        = 0;
  UINT32                      Prov2TblEntries           = 0;
  UINT8                       *Prov2TblWalkThrough      = NULL;
  UINT32                      MeStorageEntryMaxBuffSize = ME_STORAGE_MAX_BUFFER;

  UINT32                      RespCode;
  UINT8                       AlgIndex;
  NV_INFO                     *IndexInfo;
  PROV2_TABLE                 *Prov2Table;
  UINT32                      NvInfoOffset;

  DEBUG ((EFI_D_INFO, "SMC TPM 2.0 Provisioning by PPI-x OOB start\n"));

  //
  // Gets the signature of this binary table.
  //
  Prov2TblHeaderBuffer = AllocateZeroPool (MeStorageEntryMaxBuffSize);
  Status = MeStorageEntryRead (
             Index,
             &pStatus,
             Prov2TblHeaderBuffer,
             &MeStorageEntryMaxBuffSize
             );
  DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob MeStorageEntryRead Prov2TblHeader - %r\n", Status));

  //
  // Checks provision table signature.
  //
  DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob Prov2TblHeaderBuffer->Signature - %lX\n", Prov2TblHeaderBuffer->Signature));
  DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob TPM20_PROVISION_TABLE_SIGNATURE - %lX\n", TPM20_PROVISION_TABLE_SIGNATURE));
  if (Prov2TblHeaderBuffer->Signature != TPM20_PROVISION_TABLE_SIGNATURE) {
    DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob: This is not a TPM 2.0 PPI-x OOB provisioning request.\n"));

    if (Prov2TblHeaderBuffer != NULL) {
      FreePool (Prov2TblHeaderBuffer);
      Prov2TblHeaderBuffer = NULL;
    }
    return;
  }

  //
  // Gets total length of this binary table.
  //
  Prov2TblBufferSize = Prov2TblHeaderBuffer->Length;
  Prov2TblBuffer = AllocateZeroPool (Prov2TblBufferSize);

  //
  // Calculates entry number that this binary table was divided to be stored in the ME storage.
  //
  Prov2TblEntries = Prov2TblBufferSize / ME_STORAGE_MAX_BUFFER;
  if ((Prov2TblBufferSize % ME_STORAGE_MAX_BUFFER) != 0) {
    Prov2TblEntries += 1;
  }
  DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob Prov2TblEntries: %x\n", Prov2TblEntries));

  //
  // Error handling for invalid entry number.
  //
  if (Prov2TblEntries > ME_STORAGE_MAX_INDEX + 1) {
    DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob: The length of TPM 2.0 PPI-x OOB provisioning table is too big to retrieve from ME storage.\n"));

    if (Prov2TblBuffer != NULL) {
      FreePool (Prov2TblBuffer);
      Prov2TblBuffer = NULL;
    }

    if (Prov2TblHeaderBuffer != NULL) {
      FreePool (Prov2TblHeaderBuffer);
      Prov2TblHeaderBuffer = NULL;
    }
    return;
  }
  ASSERT (Prov2TblEntries <= ME_STORAGE_MAX_INDEX + 1);

  //
  // Walks through ME storage to get the complete binary table.
  //
  if (Prov2TblEntries <= ME_STORAGE_MAX_INDEX + 1) {
    Prov2TblWalkThrough = Prov2TblBuffer;
    for (Index = 0; Index < Prov2TblEntries; Index++) {

      Status = MeStorageEntryRead (
                 Index,
                 &pStatus,
                 Prov2TblWalkThrough,
                 &MeStorageEntryMaxBuffSize
                 );
      DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob MeStorageEntryRead Prov2TblWalkThrough - %r, MeStorageEntry: 0x%x\n", Status, Index));
      Prov2TblWalkThrough += MeStorageEntryMaxBuffSize;
    }
  }
#ifdef EFI_DEBUG
  DumpBinaryData (Prov2TblBuffer, Prov2TblBufferSize);
#endif  //#ifdef EFI_DEBUG

  Prov2Table = Prov2TblBuffer;
  DEBUG ((EFI_D_INFO, "SmcTpm2ProvisionByPpiXOob Prov2Table->NumAlgs: 0x%x\n", Prov2Table->NumAlgs));
  if (Prov2Table->NumAlgs == 0) {
    return;
  }

  //
  // Finds an algorithm on disc and check to see if the TPM2 supports it.
  //
  for (AlgIndex = 0; AlgIndex < Prov2Table->NumAlgs; AlgIndex++) {

    Status = Tpm20SendCommand (
               Prov2Table->AlgParams[AlgIndex].SetPhPolSize,
               (UINT8 *)Prov2TblBuffer + Prov2Table->AlgParams[AlgIndex].SetPhPolOffset,
               &RespCode
               );

    if ((Status == EFI_SUCCESS) && (RespCode == 0)) { // Then stop looking and use this algorithm.
      DEBUG ((EFI_D_INFO, "Found! Then stop looking and use this algorithm.\n"));
      break;
    }
  }

  if (AlgIndex == Prov2Table->NumAlgs) {
    return;
  }

  if (Prov2Table->AlgParams[AlgIndex].NumIndexes == 0) {
    return;
  }

  //
  // Gets a pointer to the begining of the Nv Info structures.
  //
  NvInfoOffset = Prov2Table->AlgParams[AlgIndex].ParamsOffset;

  for (Index = 0; Index < Prov2Table->AlgParams[AlgIndex].NumIndexes; Index++) {
    IndexInfo = (VOID *)((UINT8 *)Prov2TblBuffer + NvInfoOffset);

    if (!Tpm2IndexExist (IndexInfo->NvIndex)) {

      Status = Tpm20SendCommand (
                 IndexInfo->CreateCmdSize,
                 (UINT8 *)Prov2TblBuffer + IndexInfo->CreateCmdOffset,
                 &RespCode
                 );

      if (IndexInfo->bWrite) {
               
        Status = Tpm20SendCommand (
                   IndexInfo->WriteCmdSize,
                   (UINT8 *)Prov2TblBuffer + IndexInfo->WriteCmdOffset,
                   &RespCode
                   );
        //      
        // Write-Protect the index if required, but only if write was successful.
        //
        if ((Status == EFI_SUCCESS) && (RespCode == 0) && (IndexInfo->bWriteProtect)) {
          Tpm2WriteLock (IndexInfo->NvIndex); 
        } 
      }
      
    } 
    NvInfoOffset += Prov2Table->AlgParams[AlgIndex].NvInfoSize;  
  }

  if (Prov2TblBuffer != NULL) {
    FreePool (Prov2TblBuffer);
    Prov2TblBuffer = NULL;
  }
  if (Prov2TblHeaderBuffer != NULL) {
    FreePool (Prov2TblHeaderBuffer);
    Prov2TblHeaderBuffer = NULL;
  }

  DEBUG ((EFI_D_INFO, "SMC TPM 2.0 Provisioning by PPI-x OOB end.\n"));
  return;
}

#endif  //#if (defined(BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT) && (BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT == 1))

EFI_STATUS
EFIAPI
SmcTpmProvisionDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  SETUP_DATA                  SetupDataBuffer;
  UINTN                       SetupVariableSize = sizeof(SETUP_DATA);
  UINT32                      SetupVariableAttributes;

  Status = gRT->GetVariable (
                 L"Setup",
                 &gSetupVariableGuid,
                 &SetupVariableAttributes,
                 &SetupVariableSize,
                 &SetupDataBuffer
                 );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (SetupDataBuffer.SmcBiosBasedTpmProvisionSupport == TRUE) {
    DEBUG((EFI_D_INFO, "SMC BIOS-Based TPM Provision Support is enabled in BIOS setup menu.\n"));
    Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **) &gTcgProtocol);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "TCG Protocol - %r\n", Status));
      //
      // Try TPM2.0
      //
    } else {
#if (defined(BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT) && (BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT == 1))
      Tpm1Provision ();
#else
      SmcTpm1ProvisionByPpiXOob ();
#endif

      return EFI_SUCCESS;
    }

    Status = gBS->LocateProtocol (&gEfiTrEEProtocolGuid, NULL, (VOID **) &gTrEEProtocol);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, "TrEE Protocol - %r\n", Status));
      return EFI_NOT_FOUND;
    } else {
#if (defined(BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT) && (BUILT_IN_TPM_PROVISIONING_TABLE_SUPPORT == 1))
      Tpm2Provision ();
#else
      SmcTpm2ProvisionByPpiXOob ();
#endif

      return EFI_SUCCESS;
    }
  }  //if (SetupDataBuffer.SmcBiosBasedTpmProvisionSupport == TRUE) {

  return EFI_ABORTED;
}
