/** @file
  This file contains the tests for the MeasuredBootEnforcement BIT

@copyright
  INTEL CONFIDENTIAL
  Copyright 2015 - 2017 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains an 'Intel Peripheral Driver' and is uniquely identified as
  "Intel Reference Module" and is licensed for Intel CPUs and chipsets under
  the terms of your license agreement with Intel or your vendor. This file may
  be modified by the user, subject to additional terms of the license agreement.

@par Specification
**/

#include "HstiIhvProviderDxe.h"

TPM2B_AUTH             mNewPlatformAuth;

// APTIOV_SERVER_OVERRIDE_RC_START : Avoiding Intel TPM SecurityPkg changes
#pragma pack(1)

typedef struct {
  TPM2_COMMAND_HEADER       Header;
  TPML_PCR_SELECTION        PcrSelectionIn;
} TPM2_PCR_READ_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER      Header;
  UINT32                    PcrUpdateCounter;
  TPML_PCR_SELECTION        PcrSelectionOut;
  TPML_DIGEST               PcrValues;
} TPM2_PCR_READ_RESPONSE;

typedef struct {
  TPM2_COMMAND_HEADER       Header;
  TPM_CAP                   Capability;
  UINT32                    Property;
  UINT32                    PropertyCount;
} TPM2_GET_CAPABILITY_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER      Header;
  TPMI_YES_NO               MoreData;
  TPMS_CAPABILITY_DATA      CapabilityData;
} TPM2_GET_CAPABILITY_RESPONSE;

typedef struct {
  TPM2_COMMAND_HEADER       Header;
  TPMI_RH_HIERARCHY_AUTH    AuthHandle;
  UINT32                    AuthorizationSize;
  TPMS_AUTH_COMMAND         AuthSession;
  TPM2B_AUTH                NewAuth;
} TPM2_HIERARCHY_CHANGE_AUTH_COMMAND;

typedef struct {
  TPM2_RESPONSE_HEADER       Header;
  UINT32                     ParameterSize;
  TPMS_AUTH_RESPONSE         AuthSession;
} TPM2_HIERARCHY_CHANGE_AUTH_RESPONSE;

#pragma pack()
EFI_TCG2_PROTOCOL         *gTcg2Protocol = NULL;
// APTIOV_SERVER_OVERRIDE_RC_END : Avoiding Intel TPM SecurityPkg changes
/**
  Check if buffer is all zero.

  @param[in] Buffer      Buffer to be checked.
  @param[in] BufferSize  Size of buffer to be checked.

  @retval TRUE  Buffer is all zero.
  @retval FALSE Buffer is not all zero.
**/
BOOLEAN
IsZeroBuffer (
  IN VOID  *Buffer,
  IN UINTN BufferSize
  )
{
  UINT8 *BufferData;
  UINTN Index;

  BufferData = Buffer;
  for (Index = 0; Index < BufferSize; Index++) {
    if (BufferData[Index] != 0) {
      return FALSE;
    }
  }
  return TRUE;
}

// APTIOV_SERVER_OVERRIDE_RC_START : Avoiding Intel TPM SecurityPkg changes

/**
  This service enables the sending of commands to the TPM2.

  @param[in]      InputParameterBlockSize  Size of the TPM2 input parameter block.
  @param[in]      InputParameterBlock      Pointer to the TPM2 input parameter block.
  @param[in,out]  OutputParameterBlockSize Size of the TPM2 output parameter block.
  @param[in]      OutputParameterBlock     Pointer to the TPM2 output parameter block.

  @retval EFI_SUCCESS            The command byte stream was successfully sent to the device and a response was successfully received.
  @retval EFI_DEVICE_ERROR       The command was not successfully sent to the device or a response was not successfully received from the device.
  @retval EFI_BUFFER_TOO_SMALL   The output parameter block is too small. 
**/
EFI_STATUS
EFIAPI
Tpm2SubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
  EFI_STATUS                Status;
  TPM2_RESPONSE_HEADER      *Header;

  if (gTcg2Protocol == NULL) {
    Status = gBS->LocateProtocol (&gEfiTcg2ProtocolGuid, NULL, (VOID **) &gTcg2Protocol);
    if (EFI_ERROR (Status)) {
      //
      // Tcg2 protocol is not installed. So, TPM2 is not present.
      //
      DEBUG ((EFI_D_ERROR, "Tpm2SubmitCommand - Tcg2 - %r\n", Status));
      return EFI_NOT_FOUND;
    }
  }
  //
  // Assume when Tcg2 Protocol is ready, RequestUseTpm already done.
  //
  Status = gTcg2Protocol->SubmitCommand (
      gTcg2Protocol,
                            InputParameterBlockSize,
                            InputParameterBlock,
                            *OutputParameterBlockSize,
                            OutputParameterBlock
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Header = (TPM2_RESPONSE_HEADER *)OutputParameterBlock;
  *OutputParameterBlockSize = SwapBytes32 (Header->paramSize);

  return EFI_SUCCESS;
}

/**
  This command returns the values of all PCR specified in pcrSelect.

  @param[in]  PcrSelectionIn     The selection of PCR to read.
  @param[out] PcrUpdateCounter   The current value of the PCR update counter.
  @param[out] PcrSelectionOut    The PCR in the returned list.
  @param[out] PcrValues          The contents of the PCR indicated in pcrSelect.
  
  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
Tpm2PcrRead (
  IN      TPML_PCR_SELECTION        *PcrSelectionIn,
     OUT  UINT32                    *PcrUpdateCounter,
     OUT  TPML_PCR_SELECTION        *PcrSelectionOut,
     OUT  TPML_DIGEST               *PcrValues
  )
{
  EFI_STATUS                        Status;
  TPM2_PCR_READ_COMMAND             SendBuffer;
  TPM2_PCR_READ_RESPONSE            RecvBuffer;
  UINT32                            SendBufferSize;
  UINT32                            RecvBufferSize;
  UINTN                             Index;
  TPML_DIGEST                       *PcrValuesOut;
  TPM2B_DIGEST                      *Digests;

  //
  // Construct command
  //
  SendBuffer.Header.tag = SwapBytes16(TPM_ST_NO_SESSIONS);
  SendBuffer.Header.commandCode = SwapBytes32(TPM_CC_PCR_Read);
 
  SendBuffer.PcrSelectionIn.count = SwapBytes32(PcrSelectionIn->count);
  for (Index = 0; Index < PcrSelectionIn->count; Index++) {
    SendBuffer.PcrSelectionIn.pcrSelections[Index].hash = SwapBytes16(PcrSelectionIn->pcrSelections[Index].hash);
    SendBuffer.PcrSelectionIn.pcrSelections[Index].sizeofSelect = PcrSelectionIn->pcrSelections[Index].sizeofSelect;
    CopyMem (&SendBuffer.PcrSelectionIn.pcrSelections[Index].pcrSelect, &PcrSelectionIn->pcrSelections[Index].pcrSelect, SendBuffer.PcrSelectionIn.pcrSelections[Index].sizeofSelect);
  }

  SendBufferSize = sizeof(SendBuffer.Header) + sizeof(SendBuffer.PcrSelectionIn.count) + sizeof(SendBuffer.PcrSelectionIn.pcrSelections[0]) * PcrSelectionIn->count;
  SendBuffer.Header.paramSize = SwapBytes32 (SendBufferSize);

  //
  // send Tpm command
  //
  RecvBufferSize = sizeof (RecvBuffer);
  Status = Tpm2SubmitCommand (SendBufferSize, (UINT8 *)&SendBuffer, &RecvBufferSize, (UINT8 *)&RecvBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (RecvBufferSize < sizeof (TPM2_RESPONSE_HEADER)) {
    DEBUG ((EFI_D_ERROR, "Tpm2PcrRead - RecvBufferSize Error - %x\n", RecvBufferSize));
    return EFI_DEVICE_ERROR;
  }
  if (SwapBytes32(RecvBuffer.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "Tpm2PcrRead - responseCode - %x\n", SwapBytes32(RecvBuffer.Header.responseCode)));
    return EFI_NOT_FOUND;
  }

  //
  // Return the response
  //

  //
  // PcrUpdateCounter
  //
  if (RecvBufferSize < sizeof (TPM2_RESPONSE_HEADER) + sizeof(RecvBuffer.PcrUpdateCounter)) {
    DEBUG ((EFI_D_ERROR, "Tpm2PcrRead - RecvBufferSize Error - %x\n", RecvBufferSize));
    return EFI_DEVICE_ERROR;
  }
  *PcrUpdateCounter = SwapBytes32(RecvBuffer.PcrUpdateCounter);

  //
  // PcrSelectionOut
  //
  if (RecvBufferSize < sizeof (TPM2_RESPONSE_HEADER) + sizeof(RecvBuffer.PcrUpdateCounter) + sizeof(RecvBuffer.PcrSelectionOut.count)) {
    DEBUG ((EFI_D_ERROR, "Tpm2PcrRead - RecvBufferSize Error - %x\n", RecvBufferSize));
    return EFI_DEVICE_ERROR;
  }
  PcrSelectionOut->count = SwapBytes32(RecvBuffer.PcrSelectionOut.count);
  if (RecvBufferSize < sizeof (TPM2_RESPONSE_HEADER) + sizeof(RecvBuffer.PcrUpdateCounter) + sizeof(RecvBuffer.PcrSelectionOut.count) + sizeof(RecvBuffer.PcrSelectionOut.pcrSelections[0]) * PcrSelectionOut->count) {
    DEBUG ((EFI_D_ERROR, "Tpm2PcrRead - RecvBufferSize Error - %x\n", RecvBufferSize));
    return EFI_DEVICE_ERROR;
  }
  for (Index = 0; Index < PcrSelectionOut->count; Index++) {
    PcrSelectionOut->pcrSelections[Index].hash = SwapBytes16(RecvBuffer.PcrSelectionOut.pcrSelections[Index].hash);
    PcrSelectionOut->pcrSelections[Index].sizeofSelect = RecvBuffer.PcrSelectionOut.pcrSelections[Index].sizeofSelect;
    CopyMem (&PcrSelectionOut->pcrSelections[Index].pcrSelect, &RecvBuffer.PcrSelectionOut.pcrSelections[Index].pcrSelect, PcrSelectionOut->pcrSelections[Index].sizeofSelect);
  }

  //
  // PcrValues
  //
  PcrValuesOut = (TPML_DIGEST *)((UINT8 *)&RecvBuffer + sizeof (TPM2_RESPONSE_HEADER) + sizeof(RecvBuffer.PcrUpdateCounter) + sizeof(RecvBuffer.PcrSelectionOut.count) + sizeof(RecvBuffer.PcrSelectionOut.pcrSelections[0]) * PcrSelectionOut->count);
  PcrValues->count = SwapBytes32(PcrValuesOut->count);
  Digests = PcrValuesOut->digests;
  for (Index = 0; Index < PcrValues->count; Index++) {
    PcrValues->digests[Index].size = SwapBytes16(Digests->size);
    CopyMem (&PcrValues->digests[Index].buffer, &Digests->buffer, PcrValues->digests[Index].size);
    Digests = (TPM2B_DIGEST *)((UINT8 *)Digests + sizeof(Digests->size) + PcrValues->digests[Index].size);
  }

  return EFI_SUCCESS;
}

/**
  This command returns various information regarding the TPM and its current state.

  The capability parameter determines the category of data returned. The property parameter 
  selects the first value of the selected category to be returned. If there is no property 
  that corresponds to the value of property, the next higher value is returned, if it exists.
  The moreData parameter will have a value of YES if there are more values of the requested 
  type that were not returned.
  If no next capability exists, the TPM will return a zero-length list and moreData will have 
  a value of NO.

  NOTE: 
  To simplify this function, leave returned CapabilityData for caller to unpack since there are 
  many capability categories and only few categories will be used in firmware. It means the caller
  need swap the byte order for the feilds in CapabilityData.

  @param[in]  Capability         Group selection; determines the format of the response.
  @param[in]  Property           Further definition of information. 
  @param[in]  PropertyCount      Number of properties of the indicated type to return.
  @param[out] MoreData           Flag to indicate if there are more values of this type.
  @param[out] CapabilityData     The capability data.
  
  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
Tpm2GetCapability (
  IN      TPM_CAP                   Capability,
  IN      UINT32                    Property,
  IN      UINT32                    PropertyCount,
  OUT     TPMI_YES_NO               *MoreData,
  OUT     TPMS_CAPABILITY_DATA      *CapabilityData
  )
{
  EFI_STATUS                        Status;
  TPM2_GET_CAPABILITY_COMMAND       SendBuffer;
  TPM2_GET_CAPABILITY_RESPONSE      RecvBuffer;
  UINT32                            SendBufferSize;
  UINT32                            RecvBufferSize;

  //
  // Construct command
  //
  SendBuffer.Header.tag = SwapBytes16(TPM_ST_NO_SESSIONS);
  SendBuffer.Header.commandCode = SwapBytes32(TPM_CC_GetCapability);

  SendBuffer.Capability = SwapBytes32 (Capability);
  SendBuffer.Property = SwapBytes32 (Property);
  SendBuffer.PropertyCount = SwapBytes32 (PropertyCount);
 
  SendBufferSize = (UINT32) sizeof (SendBuffer);
  SendBuffer.Header.paramSize = SwapBytes32 (SendBufferSize);
    
  //
  // send Tpm command
  //
  RecvBufferSize = sizeof (RecvBuffer);
  Status = Tpm2SubmitCommand (SendBufferSize, (UINT8 *)&SendBuffer, &RecvBufferSize, (UINT8 *)&RecvBuffer );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (RecvBufferSize <= sizeof (TPM2_RESPONSE_HEADER) + sizeof (UINT8)) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Return the response
  //
  *MoreData = RecvBuffer.MoreData;
  //
  // Does not unpack all possiable property here, the caller should unpack it and note the byte order.
  //
  CopyMem (CapabilityData, &RecvBuffer.CapabilityData, RecvBufferSize - sizeof (TPM2_RESPONSE_HEADER) - sizeof (UINT8));
  
  return EFI_SUCCESS;
}
/**
  This command returns the information of TPM PCRs.

  This function parse the value got from TPM2_GetCapability and return the PcrSelection.

  @param[out] Pcrs    The Pcr Selection
  
  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_DEVICE_ERROR       The command was unsuccessful.
**/
EFI_STATUS
EFIAPI
Tpm2GetCapabilityPcrs (
  OUT TPML_PCR_SELECTION      *Pcrs
  )
{
  TPMS_CAPABILITY_DATA    TpmCap;
  TPMI_YES_NO             MoreData;
  EFI_STATUS              Status;
  UINTN                   Index;

  Status = Tpm2GetCapability (
             TPM_CAP_PCRS, 
             0, 
             1, 
             &MoreData, 
             &TpmCap
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Pcrs->count = SwapBytes32 (TpmCap.data.assignedPCR.count);
  for (Index = 0; Index < Pcrs->count; Index++) {
    Pcrs->pcrSelections[Index].hash = SwapBytes16 (TpmCap.data.assignedPCR.pcrSelections[Index].hash);
    Pcrs->pcrSelections[Index].sizeofSelect = TpmCap.data.assignedPCR.pcrSelections[Index].sizeofSelect;
    CopyMem (Pcrs->pcrSelections[Index].pcrSelect, TpmCap.data.assignedPCR.pcrSelections[Index].pcrSelect, Pcrs->pcrSelections[Index].sizeofSelect);
  }

  return EFI_SUCCESS;
}

/**

  @retval EFI_SUCCESS      Get the control of TPM2 chip.
  @retval EFI_NOT_FOUND    TPM2 not found.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2RequestUseTpm (
  VOID
  )
{
  EFI_STATUS   Status;

  if (gTcg2Protocol == NULL) {
    Status = gBS->LocateProtocol(&gEfiTcg2ProtocolGuid, NULL, (VOID **)&gTcg2Protocol);
     if (EFI_ERROR (Status)) {
      //
      // TCG2 protocol is not installed. So, TPM2 is not present.
      //
      DEBUG ((EFI_D_ERROR, "Tpm2RequestUseTpm - TrEE - %r\n", Status));
      return EFI_NOT_FOUND;
    }
  }
  //
  // Assume when TCG2 Protocol is ready, RequestUseTpm already done.
  //
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Tpm2GetCapabilityManufactureID (
    OUT     UINT32                    *ManufactureId
)
{
    TPMS_CAPABILITY_DATA    TpmCap;
    TPMI_YES_NO             MoreData;
    EFI_STATUS              Status;

    Status = Tpm2GetCapability (
                 TPM_CAP_TPM_PROPERTIES, 
                 TPM_PT_MANUFACTURER, 
                 1, 
                 &MoreData, 
                 &TpmCap
                 );
      if (EFI_ERROR (Status)) {
        return Status;
      }
   
    *ManufactureId = SwapBytes32 (TpmCap.data.tpmProperties.tpmProperty->value);
   
    // Intel Manufacturer ID (Intel TPM is always PTT) : INTC (494E5443)
    DEBUG((DEBUG_INFO, "\n ManufactureId = %x \n", *ManufactureId));

    return EFI_SUCCESS;
}

/**
  Copy AuthSessionIn to TPM2 command buffer.

  @param [in]  AuthSessionIn   Input AuthSession data
  @param [out] AuthSessionOut  Output AuthSession data in TPM2 command buffer

  @return AuthSession size
**/
UINT32
EFIAPI
CopyAuthSessionCommand (
  IN      TPMS_AUTH_COMMAND         *AuthSessionIn, OPTIONAL
  OUT     UINT8                     *AuthSessionOut
  )
{
  UINT8  *Buffer;

  Buffer = (UINT8 *)AuthSessionOut;
  
  //
  // Add in Auth session
  //
  if (AuthSessionIn != NULL) {
    //  sessionHandle
    WriteUnaligned32 ((UINT32 *)Buffer, SwapBytes32(AuthSessionIn->sessionHandle));
    Buffer += sizeof(UINT32);

    // nonce
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16 (AuthSessionIn->nonce.size));
    Buffer += sizeof(UINT16);

    CopyMem (Buffer, AuthSessionIn->nonce.buffer, AuthSessionIn->nonce.size);
    Buffer += AuthSessionIn->nonce.size;

    // sessionAttributes
    *(UINT8 *)Buffer = *(UINT8 *)&AuthSessionIn->sessionAttributes;
    Buffer++;

    // hmac
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16 (AuthSessionIn->hmac.size));
    Buffer += sizeof(UINT16);

    CopyMem (Buffer, AuthSessionIn->hmac.buffer, AuthSessionIn->hmac.size);
    Buffer += AuthSessionIn->hmac.size;
  } else {
    //  sessionHandle
    WriteUnaligned32 ((UINT32 *)Buffer, SwapBytes32(TPM_RS_PW));
    Buffer += sizeof(UINT32);

    // nonce = nullNonce
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16(0));
    Buffer += sizeof(UINT16);

    // sessionAttributes = 0
    *(UINT8 *)Buffer = 0x00;
    Buffer++;

    // hmac = nullAuth
    WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16(0));
    Buffer += sizeof(UINT16);
  }

  return (UINT32)(UINTN)(Buffer - (UINT8 *)AuthSessionOut);
}

/**
  This command allows the authorization secret for a hierarchy or lockout to be changed using the current
  authorization value as the command authorization.

  @param[in] AuthHandle        TPM_RH_LOCKOUT, TPM_RH_ENDORSEMENT, TPM_RH_OWNER or TPM_RH_PLATFORM+{PP}
  @param[in] AuthSession       Auth Session context
  @param[in] NewAuth           New authorization secret

  @retval EFI_SUCCESS      Operation completed successfully.
  @retval EFI_DEVICE_ERROR Unexpected device behavior.
**/
EFI_STATUS
EFIAPI
Tpm2HierarchyChangeAuth (
  IN TPMI_RH_HIERARCHY_AUTH    AuthHandle,
  IN TPMS_AUTH_COMMAND         *AuthSession,
  IN TPM2B_AUTH                *NewAuth
  )
{
  EFI_STATUS                           Status;
  TPM2_HIERARCHY_CHANGE_AUTH_COMMAND   Cmd;
  TPM2_HIERARCHY_CHANGE_AUTH_RESPONSE  Res;
  UINT32                               CmdSize;
  UINT32                               RespSize;
  UINT8                                *Buffer;
  UINT32                               SessionInfoSize;
  UINT8                                *ResultBuf;
  UINT32                               ResultBufSize;

  //
  // Construct command
  //
  Cmd.Header.tag          = SwapBytes16(TPM_ST_SESSIONS);
  Cmd.Header.paramSize    = SwapBytes32(sizeof(Cmd));
  Cmd.Header.commandCode  = SwapBytes32(TPM_CC_HierarchyChangeAuth);
  Cmd.AuthHandle          = SwapBytes32(AuthHandle);

  //
  // Add in Auth session
  //
  Buffer = (UINT8 *)&Cmd.AuthSession;

  // sessionInfoSize
  SessionInfoSize = CopyAuthSessionCommand (AuthSession, Buffer);
  Buffer += SessionInfoSize;
  Cmd.AuthorizationSize = SwapBytes32(SessionInfoSize);

  // New Authorization size
  WriteUnaligned16 ((UINT16 *)Buffer, SwapBytes16(NewAuth->size));
  Buffer += sizeof(UINT16);

  // New Authorizeation
  CopyMem(Buffer, NewAuth->buffer, NewAuth->size);
  Buffer += NewAuth->size;

  CmdSize = (UINT32)(Buffer - (UINT8 *)&Cmd);
  Cmd.Header.paramSize = SwapBytes32(CmdSize);

  ResultBuf     = (UINT8 *) &Res;
  ResultBufSize = sizeof(Res);

  //
  // Call the TPM
  //
  Status = Tpm2SubmitCommand (
             CmdSize, 
             (UINT8 *)&Cmd, 
             &ResultBufSize,
             ResultBuf
             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  if (ResultBufSize > sizeof(Res)) {
    DEBUG ((EFI_D_ERROR, "HierarchyChangeAuth: Failed ExecuteCommand: Buffer Too Small\r\n"));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Validate response headers
  //
  RespSize = SwapBytes32(Res.Header.paramSize);
  if (RespSize > sizeof(Res)) {
    DEBUG ((EFI_D_ERROR, "HierarchyChangeAuth: Response size too large! %d\r\n", RespSize));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Done;
  }

  //
  // Fail if command failed
  //
  if (SwapBytes32(Res.Header.responseCode) != TPM_RC_SUCCESS) {
    DEBUG((EFI_D_ERROR,"HierarchyChangeAuth: Response Code error! 0x%08x\r\n", SwapBytes32(Res.Header.responseCode)));
    Status = EFI_DEVICE_ERROR;
    goto Done;
  }

Done:
  //
  // Clear AuthSession Content
  //
  ZeroMem (&Cmd, sizeof(Cmd));
  ZeroMem (&Res, sizeof(Res));
  return Status;
}
// APTIOV_SERVER_OVERRIDE_RC_END : Avoiding Intel TPM SecurityPkg changes
/**
  Run tests for MeasuredBootEnforcement bit
**/
VOID
CheckMeasuredBootEnforcement (
  VOID
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   Result;
  BOOLEAN                   TpmPresent;
  CHAR16                    *HstiErrorString;
  TPML_PCR_SELECTION        PcrSelectionIn;
  UINT32                    PcrUpdateCounter;
  TPML_PCR_SELECTION        PcrSelectionOut;
  TPML_DIGEST               PcrValues;
  TPML_PCR_SELECTION        Pcrs;
  UINT32                    Index;
  BOOLEAN                   Pcr7Initialized;
  UINTN                     Index3;
  EFI_PHYSICAL_ADDRESS      EventLogLocation;
  EFI_PHYSICAL_ADDRESS      EventLogLastEntry;
  BOOLEAN                   EventLogTruncated;
  TPMS_AUTH_COMMAND         LocalAuthSession;
  UINT16                    AuthSize;
// APTIOV_SERVER_OVERRIDE_RC_START : Avoiding Intel TPM SecurityPkg changes
/*
  UINT32                    PttPchStrap;

  EFI_TCG2_PROTOCOL         *Tcg2Protocol = NULL;
  EFI_SPI_PROTOCOL          *SpiProtocol = NULL;
*/
  UINT32              TpmManufacturer=0;
// APTIOV_SERVER_OVERRIDE_RC_END : Avoiding Intel TPM SecurityPkg changes

  if ((mFeatureImplemented[0] & HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT) == 0) {
    return;
  }

  // APTIOV_SERVER_OVERRIDE_RC_START : Avoiding Intel TPM SecurityPkg changes
  /*
  Status = gBS->LocateProtocol (&gEfiSpiProtocolGuid, NULL, (VOID **) &SpiProtocol);
  ASSERT_EFI_ERROR (Status);
  */
  // APTIOV_SERVER_OVERRIDE_RC_END : Avoiding Intel TPM SecurityPkg changes
  
  Result     = TRUE;
  TpmPresent = TRUE;
  Status     = EFI_SUCCESS;

  DEBUG ((DEBUG_INFO, "MeasuredBoot Enforcement\n"));

  DEBUG ((DEBUG_INFO, "    1. If chipset supports FW TPM it must be enabled or the platform must have a dTPM\n"));

  
// APTIOV_SERVER_OVERRIDE_RC_START : Avoiding Intel TPM SecurityPkg changes
/*
  Status = SpiProtocol->ReadPchSoftStrap (SpiProtocol, R_PCH_SPI_STRP_DSCR_0, sizeof (PttPchStrap), &PttPchStrap);

  if (((PttPchStrap & BIT22) == 0) &&
      (*(UINT8 *) (UINTN) TPM_BASE_ADDRESS == 0xFF)) {
*/
  // Intel Manufacturer ID (Intel TPM is always PTT) : INTC (494E5443)
  Status = Tpm2GetCapabilityManufactureID(&TpmManufacturer);
  if (EFI_ERROR(Status)) {
	  DEBUG ((DEBUG_INFO, "Fail:  No TPM present on system\n"));
	  HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_CODE_1,HSTI_MEASURED_BOOT_CONFIGURATION, HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_STRING_1);
	      Status = HstiLibAppendErrorString (
	                 PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
	                 NULL,
	                 HstiErrorString
	                 );
	      ASSERT_EFI_ERROR (Status);
	      Result = FALSE;
	      FreePool (HstiErrorString);
	  TpmPresent = FALSE;
  }
  if (!EFI_ERROR(Status) && TpmManufacturer == 0x494E5443 && (*(UINT8 *) (UINTN) TPM_BASE_ADDRESS == 0xFF)) {
// APTIOV_SERVER_OVERRIDE_RC_END : Avoiding Intel TPM SecurityPkg changes
  
    DEBUG ((DEBUG_INFO, "Fail: FW TPM supported but no TPM present on system\n"));

    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_CODE_1,HSTI_MEASURED_BOOT_CONFIGURATION, HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_STRING_1);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
    TpmPresent = FALSE;
  }

  DEBUG ((DEBUG_INFO, "    2.PCR[7] Must be initialized\n"));

  ZeroMem (&PcrSelectionIn, sizeof (PcrSelectionIn));
  ZeroMem (&PcrUpdateCounter, sizeof (UINT32));
  ZeroMem (&PcrSelectionOut, sizeof (PcrSelectionOut));
  ZeroMem (&PcrValues, sizeof (PcrValues));
  ZeroMem (&Pcrs, sizeof (TPML_PCR_SELECTION));
  Pcr7Initialized = FALSE;

  if (TpmPresent == TRUE) {
    Tpm2RequestUseTpm();

    Status = Tpm2GetCapabilityPcrs (&Pcrs);

    if (!EFI_ERROR(Status)) {
      for (Index3 = 0; Index3 < Pcrs.count; Index3++) {
        PcrSelectionIn.pcrSelections[PcrSelectionIn.count].hash = Pcrs.pcrSelections[Index3].hash;
        PcrSelectionIn.pcrSelections[PcrSelectionIn.count].sizeofSelect = PCR_SELECT_MAX;
        PcrSelectionIn.pcrSelections[PcrSelectionIn.count].pcrSelect[0] = 0x80;
        PcrSelectionIn.pcrSelections[PcrSelectionIn.count].pcrSelect[1] = 0x00;
        PcrSelectionIn.pcrSelections[PcrSelectionIn.count].pcrSelect[2] = 0x00;
        PcrSelectionIn.count++;
      }

      Tpm2PcrRead (&PcrSelectionIn,&PcrUpdateCounter,&PcrSelectionOut,&PcrValues);

      for (Index = 0; Index < PcrValues.count; Index++) {
        Pcr7Initialized = !IsZeroBuffer (PcrValues.digests[Index].buffer,PcrValues.digests[Index].size);
        if ( !Pcr7Initialized ) {
          DEBUG ((DEBUG_INFO, "Fail: Pcr 7 not initialized\n"));
          break;
        }
      }
    } else {
      DEBUG ((DEBUG_INFO, "Fail: Not able to get PCRs from TPM\n"));
    }
  }

  if ( !Pcr7Initialized || (TpmPresent != TRUE)) {
    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_CODE_2,HSTI_MEASURED_BOOT_CONFIGURATION, HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_STRING_2);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    3.Event log must be published\n"));

  EventLogLocation = (EFI_PHYSICAL_ADDRESS) NULL;
  if (TpmPresent == TRUE) {

// APTIOV_SERVER_OVERRIDE_RC_START : Avoiding Intel TPM SecurityPkg changes
//    Status = gBS->LocateProtocol(&gEfiTcg2ProtocolGuid, NULL, (VOID **)&Tcg2Protocol);
// APTIOV_SERVER_OVERRIDE_RC_END : Avoiding Intel TPM SecurityPkg changes

    if (!EFI_ERROR (Status)) {
// APTIOV_SERVER_OVERRIDE_RC_START : Avoiding Intel TPM SecurityPkg changes
      Status = gTcg2Protocol->GetEventLog(gTcg2Protocol,
// APTIOV_SERVER_OVERRIDE_RC_END : Avoiding Intel TPM SecurityPkg changes
                               EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2,
                               &EventLogLocation,
                               &EventLogLastEntry,
                               &EventLogTruncated
                               );
    }
    DEBUG ((DEBUG_INFO, "EventLogLocation - EventLogLocation - %x\n", EventLogLocation));

  }

  if (EFI_ERROR (Status) || (EventLogLocation == (EFI_PHYSICAL_ADDRESS) NULL) || (TpmPresent != TRUE)) {
    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_CODE_3,HSTI_MEASURED_BOOT_CONFIGURATION, HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_STRING_3);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  DEBUG ((DEBUG_INFO, "    4. Platform Auth value must not be all zero\n"));

  if ( TpmPresent == TRUE ) {
    AuthSize = MAX_NEW_AUTHORIZATION_SIZE;
    for (Index = 0; Index < Pcrs.count; Index++) {
      switch (Pcrs.pcrSelections[Index].hash) {
        case TPM_ALG_SHA1:
          AuthSize = SHA1_DIGEST_SIZE;
          break;
        case TPM_ALG_SHA256:
          AuthSize = SHA256_DIGEST_SIZE;
          break;
        case TPM_ALG_SHA384:
          AuthSize = SHA384_DIGEST_SIZE;
          break;
        case TPM_ALG_SHA512:
          AuthSize = SHA512_DIGEST_SIZE;
          break;
        case TPM_ALG_SM3_256:
          AuthSize = SM3_256_DIGEST_SIZE;
          break;
      }
    }

    ZeroMem (mNewPlatformAuth.buffer, AuthSize);
    ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = AuthSize;
    CopyMem (LocalAuthSession.hmac.buffer, mNewPlatformAuth.buffer, AuthSize);

    Status = Tpm2HierarchyChangeAuth (TPM_RH_PLATFORM, NULL, &mNewPlatformAuth);
    DEBUG ((DEBUG_INFO, "Tpm2HierarchyChangeAuth with NULL auth Result: - %r\n", Status));
  }

  if (!EFI_ERROR (Status) || (TpmPresent != TRUE)) {
    HstiErrorString = BuildHstiErrorString (HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_CODE_4,HSTI_MEASURED_BOOT_CONFIGURATION, HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT_ERROR_STRING_4);
    Status = HstiLibAppendErrorString (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               HstiErrorString
               );
    ASSERT_EFI_ERROR (Status);
    Result = FALSE;
    FreePool (HstiErrorString);
  }

  //
  // ALL PASS
  //
  if (Result) {

    Status = HstiLibSetFeaturesVerified (
               PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE,
               NULL,
               0,
               HSTI_BYTE0_MEASURED_BOOT_ENFORCEMENT
               );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}
