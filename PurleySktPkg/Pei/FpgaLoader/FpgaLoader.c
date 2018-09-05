/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file FpgaLoader.c

**/

#include "FpgaLoader.h"

//
// Local variables
//
EFI_PEI_MP_SERVICES_PPI           *mMpServices;
EFI_PROCESSOR_INFORMATION         *mProcessorLocBuf;
UINTN                             mProcessorNum;
UINTN                             mBspIndex;
UINT32                            mBspSocketId;
BOOLEAN                           mFpgaLoadStatus = FALSE;
PSYSHOST                          host = NULL;
MRC_HOOKS_SERVICES_PPI            *mMrcHooksServices;
MRC_HOOKS_CHIP_SERVICES_PPI       *mMrcHooksChipServices;

//
// Local functions
//
/**
  FpgaMpServicesData, Load the FPGA Mp services structures.

  @param PeiServices                 General purpose services available to every PEIM.
  @param FpgaConfigurationPointer    Pointer to FPGA configuration struct.

  @retval EFI_SUCCESS                FPGA trigger completed successfully.

**/
EFI_STATUS
EFIAPI
FpgaMpServicesData (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN  FPGA_CONFIGURATION        *FpgaConfigurationPointer
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  UINTN                             EnabledProcessorNum;
  UINTN                             Index;

  Status = mMpServices->GetNumberOfProcessors (
                          PeiServices,
                          mMpServices,
                          &mProcessorNum,
                          &EnabledProcessorNum
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof(EFI_PROCESSOR_INFORMATION) * mProcessorNum,
                             (VOID **) &mProcessorLocBuf
                             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Get each processor Location info
  //
  for (Index = 0; Index < mProcessorNum; Index++) {
    Status = mMpServices->GetProcessorInfo (
                            PeiServices,
                            mMpServices,
                            Index,
                            &mProcessorLocBuf[Index]
                            );
    if (EFI_ERROR (Status)) {
      FreePool (mProcessorLocBuf);
      return Status;
    }
  }

  Status = mMpServices->WhoAmI (
                          PeiServices,
                          mMpServices,
                          &mBspIndex
                          );
  DEBUG ((EFI_D_ERROR, "BSP Index is: %X\n", mBspIndex));
  ASSERT_EFI_ERROR (Status);
  mBspSocketId = mProcessorLocBuf[mBspIndex].Location.Package;
  DEBUG ((EFI_D_ERROR, "BSP Socket is: %X\n", mBspSocketId));

  return Status;
}

/**
  The BIOS provide the location of the FPGA Loader module and payload by initialize FPGA_BBS_PARAM MSR.

  @param  ParmDirectory   Pointer to FPGA Param directory struct

  @retval None

**/
VOID
SetFpgaBbsParamMsr (
  IN FPGA_PARM_DIRECTORY  *ParmDirectory
  )
{
  AsmWriteMsr64 (MSR_FPGA_BBS_PARAM, (UINT64) ParmDirectory);
}

/**
  The BIOS issues WRMSR FPGA_BBS_TRIGGER to launch the module.

  @param  None

  @retval None

**/
VOID
SetFpgaBbsTriggerMsr (
  VOID
  )
{
  UINT32      Cr0;
  UINT32      Data32;

  // Save CR0
  Cr0 = AsmReadCr0 ();
  Data32 = Cr0;

  // Set NE bit
  Data32 |= 0x00000020;
  // Clear NW, CD bit
  Data32 &= 0x9FFFFFFF;
  // Write CR0
  AsmWriteCr0 (Data32);

  // Issue WRMSR FPGA_BBS_TRIGGER
  AsmWriteMsr64 (MSR_FPGA_BBS_TRIGGER, 0);

  // Restore CR0
  AsmWriteCr0(Cr0);
}

/**
  Read FPGA_BBS_PARAM to get the return value of the FPGA loader.

  @param  None

  @retval None

**/
VOID
ReadFpgaBbsParamMsr (
  VOID
  )
{
  UINT64                  MsrValue;

  MsrValue = AsmReadMsr64 (MSR_FPGA_BBS_PARAM);
  DEBUG ((EFI_D_ERROR, "PARAM MSR Read: %lX\n", MsrValue));

  if ((MsrValue & 0xFF) == 0) {  // Pass
    DEBUG ((EFI_D_ERROR, "FPGA BBS load passed.\n"));
    mFpgaLoadStatus = TRUE;
  } else {
    DEBUG ((EFI_D_ERROR, "FPGA BBS load failed.\n"));
    mFpgaLoadStatus = FALSE;
  }
}

/**
  FpgaLoaderLoadParmRegister, Load the FPGA Param register from the data

  @param PeiServices    General purpose services available to every PEIM.
  @param SocketIndex    BsGuidIndex indexed by socket.
  @param ParmDirectory  Pointer to FPGA Param directory struct

  @retval EFI_SUCCESS   FPGA load param register completed successfully.

**/
EFI_STATUS
EFIAPI
FpgaLoaderLoadParmRegister (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN UINTN                      SocketIndex,
  IN FPGA_PARM_DIRECTORY        *ParmDirectory
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  UINTN                 Index;

  DEBUG ((EFI_D_ERROR, "ParmDirectory:            %08X\n",  (UINTN)ParmDirectory));
  DEBUG ((EFI_D_ERROR, "FpgaParmLoaderAddress:    %08X\n", ParmDirectory->FpgaParmLoaderAddress));
  DEBUG ((EFI_D_ERROR, "FpgaParmStructureVersion: %08X\n", ParmDirectory->FpgaParmStructureVersion));
  DEBUG ((EFI_D_ERROR, "FpgaParmPayloadVersion:   %08X\n", ParmDirectory->FpgaParmPayloadVersion));
  DEBUG ((EFI_D_ERROR, "FpgaParmPayloadSize:      %08X\n", ParmDirectory->FpgaParmPayloadSize));
  DEBUG ((EFI_D_ERROR, "FpgaParmPayloadAddress:   %08X\n", ParmDirectory->FpgaParmPayloadAddress));

  for (Index = 0; Index < mProcessorNum; Index++) {
    if ((Index == mBspIndex) && (SocketIndex == mBspSocketId)) {
      SetFpgaBbsParamMsr(ParmDirectory);
      DEBUG ((EFI_D_ERROR, "FPGA BBS load PARAM on BSP.\n"));
      break;
    } else {
      if ((mProcessorLocBuf[Index].Location.Package == SocketIndex) && (SocketIndex != mBspSocketId)) {
        DEBUG ((EFI_D_ERROR, "FPGA BBS load PARAM on socket[%X], Index[%X]\n", SocketIndex, Index));
        mMpServices->StartupThisAP (
                       PeiServices,
                       mMpServices,
                       SetFpgaBbsParamMsr,
                       Index,
                       0,
                       ParmDirectory
                       );
        break;
      }
    }
  }
  return Status;
}


/**
  FpgaLoaderLoadTriggerRegister, Load the FPGA trigger register from the data

  @param PeiServices                 General purpose services available to every PEIM.
  @param FpgaConfigurationPointer    Pointer to FPGA configuration struct.

  @retval EFI_SUCCESS                FPGA trigger completed successfully.

**/
EFI_STATUS
EFIAPI
FpgaLoaderLoadTriggerRegister (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN  FPGA_CONFIGURATION        *FpgaConfigurationPointer
  )
{
  UINT8                            SocketIndex;
  UINTN                            Index;

  Index = 0;

  //
  // Set FPGA_BBS_TRIGGER trigger MSR register
  //
  for (SocketIndex = 0; SocketIndex < FPGA_MAX_SOCKET; SocketIndex++) {
    if ((FpgaConfigurationPointer->FpgaSktPresent & (1 << SocketIndex)) == 0) {
      continue;
    }
    // if no GUID, then skip as well
    if (FpgaConfigurationPointer->FpgaBitStreamGuid[SocketIndex] == FPGA_NULL_GUID_INDEX) {
      continue;
    }
    // If already programmed, then skip as well
    if ((FpgaConfigurationPointer->FpgaSktProgrammed & (0x01 << SocketIndex)) != 0) {
      continue;
    }

    //
    // Non-BSP socket
    //
    for (Index = 0; Index < mProcessorNum; Index++) {
      if (Index == mBspIndex) {
        continue;
      } else {
        if ((mProcessorLocBuf[Index].Location.Package == SocketIndex) && (SocketIndex != mBspSocketId)) {
          DEBUG ((EFI_D_ERROR, "FPGA BBS load trigger on socket[%X], Index[%X]\n", SocketIndex, Index));
          mMpServices->StartupThisAP (
                         PeiServices,
                         mMpServices,
                         (EFI_AP_PROCEDURE)SetFpgaBbsTriggerMsr,
                         Index,
                         0,
                         NULL
                         );
          break;
        }
      }
    }
  }

  //
  // Trigger the MSR on BSP socket
  //
  if (FpgaConfigurationPointer->FpgaSktPresent & (1 << mBspSocketId)) {
    if (FpgaConfigurationPointer->FpgaBitStreamGuid[mBspSocketId] != FPGA_NULL_GUID_INDEX) {
      DEBUG ((EFI_D_ERROR, "Trigger MSR for BSP\n"));
      SetFpgaBbsTriggerMsr();
    }
  }

  //
  // Read non-BSP FPGA_BBS_PARAM MSR register return value
  //
  for (SocketIndex = 0; SocketIndex < FPGA_MAX_SOCKET; SocketIndex++) {
    if ((FpgaConfigurationPointer->FpgaSktPresent  & (1 << SocketIndex)) == 0) {
      continue;
    }
    // If no GUID, then skip as well
    if (FpgaConfigurationPointer->FpgaBitStreamGuid[SocketIndex] == FPGA_NULL_GUID_INDEX) {
      continue;
    }

    for (Index = 0; Index < mProcessorNum; Index++) {
      if ((mProcessorLocBuf[Index].Location.Package == SocketIndex) && (SocketIndex != mBspSocketId)) {
        DEBUG ((EFI_D_ERROR, "FPGA read BBS load PARAM on socket[%X], Index[%X]\n", SocketIndex, Index));
        mMpServices->StartupThisAP (
                        PeiServices,
                        mMpServices,
                        (EFI_AP_PROCEDURE)ReadFpgaBbsParamMsr,
                        Index,
                        0,
                        NULL
                        );
        if (mFpgaLoadStatus) {
          FpgaConfigurationPointer->FpgaSktProgrammed |= (1 << SocketIndex);
          FpgaConfigurationPointer->FpgaBitStreamStatus[SocketIndex] = FPGA_LOAD_SUCCESS;
        } else {
          FpgaConfigurationPointer->FpgaSktProgrammed &= (~(1 << SocketIndex));
          FpgaConfigurationPointer->FpgaBitStreamStatus[SocketIndex] = FPGA_LOAD_STATUS_DEVICE_PARM_REGISTER_FAILED;
        }
        break;
      }
    }
  }

  //
  // Read BSP FPGA_BBS_PARAM MSR register return value
  //
  if (FpgaConfigurationPointer->FpgaSktPresent & (1 << mBspSocketId)) {
    if (FpgaConfigurationPointer->FpgaBitStreamGuid[mBspSocketId] != FPGA_NULL_GUID_INDEX) {
      DEBUG ((EFI_D_ERROR, "Read BSP PARAM MSR\n"));
      ReadFpgaBbsParamMsr();

      if (mFpgaLoadStatus) {
        FpgaConfigurationPointer->FpgaSktProgrammed |= 1;
        FpgaConfigurationPointer->FpgaBitStreamStatus[0] = FPGA_LOAD_SUCCESS;
      } else {
        FpgaConfigurationPointer->FpgaSktProgrammed &= (~1);
        FpgaConfigurationPointer->FpgaBitStreamStatus[0] = FPGA_LOAD_STATUS_DEVICE_PARM_REGISTER_FAILED;
      }
    }
  }

  if (FpgaConfigurationPointer->FpgaSktProgrammed != 0) {
    return EFI_SUCCESS;
  } else {
    return FPGA_LOAD_STATUS_DEVICE_PARM_REGISTER_FAILED;
  }
}


/**
  Disabled unused FPGA Socket.

  @param  FpgaConfigurationPointer    Pointer to FPGA configuration struct.

  @retval EFI_SUCCESS                 The function is executed successfully.
  @retval other                       Some error occurs when executing

**/
EFI_STATUS
EFIAPI
FpgaUnusedSocketDisable (
  IN  FPGA_CONFIGURATION        *FpgaConfigurationPointer
  )
{
  EFI_STATUS                     Status = EFI_UNSUPPORTED;
  UINT8                          Socket;
  UINT32                         PcodeMailboxStatus = 0;

  //
  // Disabled unused FPGA Socket
  //
  for (Socket = 0; Socket < FPGA_MAX_SOCKET; Socket++) {
    if ((FpgaConfigurationPointer->FpgaSktPresent & (1 << Socket)) == 0) {
      continue;
    }
    //
    // Check user disabled or have errors, only enable the disabled check now, will enable the errors check after BBS can be downloaded by bios
    //
    if ((FpgaConfigurationPointer->FpgaPlatformEnabled & (1 << Socket)) == 0) {
      DEBUG ((DEBUG_ERROR, "FpgaPlatformEnabled = 0x%X. FpgaBitStreamStatus = 0x%X.\n", FpgaConfigurationPointer->FpgaPlatformEnabled,FpgaConfigurationPointer->FpgaBitStreamStatus[Socket]));
      DEBUG ((DEBUG_ERROR, "Disable Fpga Socket[%x]!!!\n", Socket));

      //
      // Bios issues a b2p mailbox to notify pcode to stop fpga/fpga vr interaction
      //
      PcodeMailboxStatus = mMrcHooksChipServices->SendMailBoxCmdToPcode (host, Socket, MAILBOX_BIOS_CMD_REMOVE_MCP, 0);
      if (PcodeMailboxStatus != 0) {
        DEBUG ((DEBUG_ERROR, "Socket[%x ]mailbox command REMOVE_MCP fail return : 0x%x.\n", Socket, PcodeMailboxStatus));
      }
      //
      // Control the GPIO to power off FPGA
      //
      Status = FpgaPoweroff(Socket);
      if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR, "Socket[%x ] Fpga Power off fail return status = %r.\n", Socket, Status));
      }
    }
  }

  return Status;
}


/**
  FpgaLoader, Load the FPGA from the data

  @param  PeiServices                 General purpose services available to every PEIM.
  @param  FpgaConfigurationPointer    Pointer to FPGA configuration struct.

  @retval EFI_SUCCESS                 FPGA loader completed successfully.
  @retval Others                      Internal error when load FPGA BBS.

**/
EFI_STATUS
EFIAPI
FpgaLoader (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN  FPGA_CONFIGURATION        *FpgaConfigurationPointer
  )
{
  EFI_STATUS              Status;
  UINT8                   SocketMask;
  UINTN                   SocketIndex;
  FPGA_LOADER_HEADER      *LoaderPtr;
  FPGA_PAYLOAD_HEADER     *BitStreamPtr;
  UINTN                   LoaderSize;
  UINT32                  BitStreamSize;
  FPGA_PARM_DIRECTORY     *ParmDirectory;
  VOID*                   FpgaLoaderBuffer;

  //
  // Get the N4PE binary from FV into the directory, as we only need to do this once.
  //
  Status = FpgaFvLibN4PeGet (&LoaderSize, &LoaderPtr);
  if (Status != EFI_SUCCESS) {
    //
    // N4PE error, quit with error
    //
    DEBUG ((EFI_D_ERROR, "Fpga Loader, FpgaFvLibN4PeGet returned error, %r\n", Status));
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "N4PE Date               : %x \n", LoaderPtr->Date));
  DEBUG ((EFI_D_ERROR, "N4PE BuildNumber        : %x \n", LoaderPtr->BuildNumber));
  DEBUG ((EFI_D_ERROR, "N4PE Size               : %x \n", LoaderPtr->LoaderSize));
  DEBUG ((EFI_D_ERROR, "N4PE MajorRevisionID    : %x \n", LoaderPtr->MajorRevisionID));
  DEBUG ((EFI_D_ERROR, "N4PE MinorHeaderVersion : %x \n", LoaderPtr->MinorHeaderVersion));

  if (LoaderSize != LoaderPtr->LoaderSize) {
    //
    // N4PE error, quit with error
    //
    DEBUG ((EFI_D_ERROR, "Fpga Loader, FpgaFvLibLoaderGet returned different size, header->%^04x, returned->%04x\n", LoaderPtr->LoaderSize, LoaderSize));
    return EFI_BAD_BUFFER_SIZE;
  }

  //
  // The uCode will execute the FPGA Loader module in the on-die memory (CRAM) in address range of 256KB
  // starting from the FPGA_LOADER_MODULE address provided by BIOS (guaranteed to be 256KB-aligned).
  //
  FpgaLoaderBuffer = AllocateAlignedPages (EFI_SIZE_TO_PAGES (SIZE_256KB), SIZE_256KB);
  if (FpgaLoaderBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (FpgaLoaderBuffer, LoaderPtr, LoaderSize);

  //
  // OK so loop through the sockets, and setup the load by programming the pointers
  //
  for (SocketIndex = 0; SocketIndex < FPGA_MAX_SOCKET; SocketIndex++) {
    SocketMask = 0x01 << SocketIndex;
    // If already programmed, skip this one, may happen due to retries.
    if ((FpgaConfigurationPointer->FpgaSktProgrammed & SocketMask) != 0) {
      continue;
    }

    if (FpgaConfigurationPointer->FpgaBitStreamGuid[SocketIndex] == FPGA_NULL_GUID_INDEX) {
      continue;
    }
    Status = FpgaFvLibBitStreamGet (FpgaConfigurationPointer->FpgaBitStreamGuid[SocketIndex], &BitStreamSize, &BitStreamPtr);

    if (Status != EFI_SUCCESS ) {
      //
      // Bit stream error, skip it after marking the error in configuration.
      //
      DEBUG ((EFI_D_ERROR, "Fpga Loader, FpgaFvLibBitStreamGet returned error, %r\n", Status));
      FpgaConfigurationPointer->FpgaBitStreamGuid[SocketIndex] = FPGA_NULL_GUID_INDEX; 
      FpgaConfigurationPointer->FpgaSktError |= SocketMask;
      FpgaConfigurationPointer->FpgaBitStreamStatus[SocketIndex] = FPGA_LOAD_STATUS_DEVICE_BITSTREAM_NOT_FOUND;
      continue;
    }
    
    DEBUG ((EFI_D_ERROR, "BBS ID                       : %g \n",   &(BitStreamPtr->FpgaBinaryGuid)));
    DEBUG ((EFI_D_ERROR, "BBS Size                     : %x \n",   BitStreamPtr->FpgaBinarySize));
    DEBUG ((EFI_D_ERROR, "BBS Data Structure Version   : %x \n",   BitStreamPtr->FpgaBinaryVersion));
    DEBUG ((EFI_D_ERROR, "BBS Flags                    : %x \n",   BitStreamPtr->FpgaBinaryFlags));

    if (BitStreamSize != BitStreamPtr->FpgaBinarySize) {
      //
      // Bit stream error, skip it after marking the error in configuration.
      //
      DEBUG ((EFI_D_ERROR, "Fpga Loader, FpgaFvLibBitStreamGet returned error, %r\n", Status));
      FpgaConfigurationPointer->FpgaBitStreamGuid[SocketIndex] = FPGA_NULL_GUID_INDEX;     
      FpgaConfigurationPointer->FpgaSktError |= SocketMask;
      FpgaConfigurationPointer->FpgaBitStreamStatus[SocketIndex] = FPGA_LOAD_STATUS_DEVICE_BINARY_SIZE_MISMATCH;
      continue;
    }

    //
    // Allocate Memory for ParmDirectory 
    //
    ParmDirectory = AllocateAlignedPages (EFI_SIZE_TO_PAGES (sizeof(FPGA_PARM_DIRECTORY)), 8);
    if (ParmDirectory == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    
    //
    // Fill in the ParmDirectory with loader info.
    //
    ParmDirectory->FpgaParmLoaderAddress    = (EFI_PHYSICAL_ADDRESS)FpgaLoaderBuffer;
    ParmDirectory->FpgaParmStructureVersion = FPGA_PRAM_DIRECTORY_VERSION;

    //
    // Fill in the ParmDirectory with BS info.
    //
    ParmDirectory->FpgaParmPayloadVersion  = BitStreamPtr->FpgaBinaryVersion;
    ParmDirectory->FpgaParmPayloadSize     = BitStreamSize;
    ParmDirectory->FpgaParmPayloadAddress  = (EFI_PHYSICAL_ADDRESS)(VOID*)BitStreamPtr;

    Status = FpgaLoaderLoadParmRegister (PeiServices, SocketIndex, ParmDirectory);
    if (Status != EFI_SUCCESS) {
      //
      // Bit stream error, skip it after marking the error in configuration.
      //
      DEBUG ((EFI_D_ERROR, "Fpga Loader, FpgaLoaderLoadParmRegister returned error, %r\n", Status));
      FpgaConfigurationPointer->FpgaBitStreamGuid[SocketIndex] = FPGA_NULL_GUID_INDEX;
      FpgaConfigurationPointer->FpgaSktError |= SocketMask;
      FpgaConfigurationPointer->FpgaBitStreamStatus[SocketIndex] = FPGA_LOAD_STATUS_DEVICE_PARM_REGISTER_FAILED;
      continue;
    }
  }

  //
  // Checkpoint to indicate FPGA Loader program is about to start.
  //
  mMrcHooksServices->OutputCheckpoint (host, STS_FPGA_LOADER_PROGRAM_START, 0, 0xFF);

  //
  // Load the FPGA trigger register from the data
  //
  Status = FpgaLoaderLoadTriggerRegister (PeiServices, FpgaConfigurationPointer);
  if (Status != EFI_SUCCESS ) {
    //
    // Trigger error.
    //
    mMrcHooksServices->OutputCheckpoint (host, STS_FPGA_LOADER_PROGRAM_FAIL, 0, 0xFF);
    DEBUG ((EFI_D_ERROR, "Fpga Loader, FpgaLoaderLoadTriggerRegister returned error, %r\n", Status));
  }

  return Status;
}


/**
  Driver entry point

  @param FileHandle             Pointer to image file handle.
  @param PeiServices            General purpose services available to every PEIM.

  @retval EFI_SUCCESS           FPGA loader task completed.
  @retval EFI_ALREADY_STARTED   FPGA already loaded, nothing to do (will happen after reset after load)
  @retval EFI_UNSUPPORTED       No FPGA is supported by BS in FV.
  @retval EFI_UNSUPPORTED       FPGA not active
  @retval Other                 Some status returned from sub function.

**/
EFI_STATUS
EFIAPI
PeimFpgaLoaderInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status;
  UINT8                            Socket;
  UINT8                            SocketMask;
  UINTN                            Retries;
  FPGA_CONFIGURATION               FpgaConfigurationPointer;
  BOOLEAN                          FpgaLoaderRequired;
  PCH_RESET_PPI                    *PchResetPpi;
  BIOSSCRATCHPAD1_UBOX_MISC_STRUCT BiosScratchPad1;

  //
  // Get the configuration data
  //
  Status = FpgaConfigurationGetValues (&FpgaConfigurationPointer);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader,FpgaConfigurationGetValues failed, return it's error!, %r\n", Status));
    return Status;
  }

  //
  // Check the configuration for global enable
  //
  if (FpgaConfigurationPointer.FpgaPcdValue == FALSE) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader,FpgaConfigurationGetValues global enable is False!\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Check the configuration for FPGA having any sockets present
  //
  if (FpgaConfigurationPointer.FpgaSktPresent == 0) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader,FpgaConfigurationGetValues no FPGA sockets present!\n"));
    return EFI_UNSUPPORTED;
  }
  //
  // Get the SYSHOST
  //
  host = (SYSHOST *)(UINTN) PcdGet64 (PcdSyshostMemoryAddress);

  //
  // Get the MRC Hooks Chip Services PPI
  //
  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gMrcHooksChipServicesPpiGuid,
                              0,
                              NULL,
                              &mMrcHooksChipServices
                              );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader, Find MRC Hooks Chip Services Ppi failed, return it's error!\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Disable FPGA if all the FPGA sockets are disabled.
  //
  if ((FpgaConfigurationPointer.FpgaPlatformEnabled == 0) || ((FpgaConfigurationPointer.FpgaPlatformEnabled & FpgaConfigurationPointer.FpgaSktPresent) == 0)) {
    FpgaUnusedSocketDisable (&FpgaConfigurationPointer);
    return EFI_UNSUPPORTED;
  }

  DEBUG ((EFI_D_ERROR, "FPGA Loader, FpgaSktActive       = %X! \n", FpgaConfigurationPointer.FpgaSktActive));
  DEBUG ((EFI_D_ERROR, "FPGA Loader, FpgaSktPresent      = %X! \n", FpgaConfigurationPointer.FpgaSktPresent));
  DEBUG ((EFI_D_ERROR, "FPGA Loader, FpgaPlatformEnabled = %X! \n", FpgaConfigurationPointer.FpgaPlatformEnabled));
  //
  // Get the MP services PPI
  //
  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gEfiPeiMpServicesPpiGuid,
                              0,
                              NULL,
                              &mMpServices
                              );
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader, find Mp services PPI failed, return it's error!\n"));
    return Status;
  }
  //
  // Set the MP CPU data
  //
  Status = FpgaMpServicesData (PeiServices, &FpgaConfigurationPointer);
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader, FpgaMpServicesData failed, return it's error!, %r\n", Status));
    return Status;
  }

  //
  // Get the MRC Hooks Services PPI
  //
  Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gMrcHooksServicesPpiGuid,
                              0,
                              NULL,
                              &mMrcHooksServices
                              );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader, Find MRC Hooks Core Services PPI failed, return it's error!\n"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Checkpoint to indicate FPGA Loader init is about to happen.
  //
  mMrcHooksServices->OutputCheckpoint (host, STS_FPGA_LOADER_INIT, 0, 0xFF);

  //
  // Clear the error indicator in configuration.
  //
  FpgaConfigurationPointer.FpgaSktError = 0;

  //
  // Check the configuration for FPGA already loaded, if so the active sockets will != 0
  //
  if ((mMrcHooksChipServices->ReadCpuPciCfgEx (host, (UINT8)mBspSocketId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG)) & BIT5) {
    DEBUG ((EFI_D_ERROR, "FPGA Loader, FpgaConfigurationGetValues FPGA already active!\n"));
    //
    // Error check the that all the present sockets are active.
    //
    for (Socket = 0; Socket < FPGA_MAX_SOCKET; Socket++) {
      SocketMask = 0x01 << Socket;
      // Mark load success if socket active
      if ((FpgaConfigurationPointer.FpgaSktActive & SocketMask) == SocketMask) {
        FpgaConfigurationPointer.FpgaBitStreamStatus[Socket] = FPGA_LOAD_SUCCESS;
      }
      // If the active bit and the present bit != each other, then mark them as having errors
      if ((FpgaConfigurationPointer.FpgaSktActive & SocketMask) != (FpgaConfigurationPointer.FpgaSktPresent & SocketMask)) {
        DEBUG ((EFI_D_ERROR, "FPGA Loader,FpgaConfigurationGetValues Sockets[%x] active !=  present \n",Socket));
        FpgaConfigurationPointer.FpgaBitStreamGuid[Socket] = FPGA_NULL_GUID_INDEX;
        FpgaConfigurationPointer.FpgaSktError |= SocketMask;
        FpgaConfigurationPointer.FpgaBitStreamStatus[Socket] = FPGA_LOAD_STATUS_DEVICE_PRESENT_NOT_ACTIVE;
      }
    }
    //
    // Update any configuration values that might have changed.
    //
    DEBUG ((EFI_D_ERROR, "FPGA Loader, Updating configuration values!\n"));
    FpgaConfigurationSetValues (&FpgaConfigurationPointer);

    return EFI_ALREADY_STARTED;
  }

  //
  // Loop to try and program each socket # of retries
  //
  for (Retries = 0; Retries < FPGA_MAX_LOADER_RETRIES; Retries++ ) {
    //
    // See if we need to run the loader function, start by assuming no
    //
    FpgaLoaderRequired       = FALSE;
    //
    // Loop through each socket, and validate it's configuration.
    //
    for (Socket = 0; Socket < FPGA_MAX_SOCKET; Socket++) {
      //
      // Set the bit mask for this socket
      //
      SocketMask = 0x01 << Socket;
      DEBUG ((EFI_D_ERROR, "FPGA Loader, Retry %d, Socket %d, Mask %02x, Started!\n",Retries, Socket, SocketMask ));
      //
      // If Socket not present, then continue but
      // Mark GUID as not programmed to be sure load is not attempted.
      //
      if ((FpgaConfigurationPointer.FpgaSktPresent & SocketMask) == 0) {
        FpgaConfigurationPointer.FpgaBitStreamGuid[Socket] = FPGA_NULL_GUID_INDEX;
        continue;
      }
      //
      // If Socket not enabled by setup, then continue but
      // Mark it as having an error
      // Save the load status
      //
      if ((FpgaConfigurationPointer.FpgaPlatformEnabled & SocketMask) == 0) {
        FpgaConfigurationPointer.FpgaBitStreamGuid[Socket] = FPGA_NULL_GUID_INDEX; 
        FpgaConfigurationPointer.FpgaSktError |= SocketMask;
        FpgaConfigurationPointer.FpgaBitStreamStatus[Socket] = FPGA_LOAD_STATUS_DEVICE_NOT_CONFIGURED;
        continue;
      }
      //
      // If GUID index is NOT NULL for the this socket and not programmed, set the FpgaLoaderRequired and continue to validate other sockets
      //
      if ((FpgaConfigurationPointer.FpgaBitStreamGuid[Socket] != FPGA_NULL_GUID_INDEX) && ((FpgaConfigurationPointer.FpgaSktProgrammed & SocketMask) == 0)) {   
        FpgaLoaderRequired = TRUE;
        DEBUG ((EFI_D_ERROR, "FPGA Loader,  Socket %d : BitStreamGuidIndex = %x !\n", Socket, FpgaConfigurationPointer.FpgaBitStreamGuid[Socket]));
        continue;
      }

      //
      // At this point no one assigned a GUID for this socket, or has already programmed, so just inform the user
      //
      DEBUG ((EFI_D_ERROR, "FPGA Loader, Socket %d has no GUID assigned or has already programmed,  skipping it!\n", Socket));

      //
      // Then Mark the error and set the status to the error value
      //
      FpgaConfigurationPointer.FpgaSktError |= SocketMask;
      FpgaConfigurationPointer.FpgaBitStreamStatus[Socket] = FPGA_LOAD_STATUS_GUID_NOT_FOUND_ERROR;
      FpgaConfigurationPointer.FpgaSktProgrammed |= (1 << Socket);

    }

    //
    // At this point someone has possibly set our data, so fire off the loading of the BS's
    //
    if (FpgaLoaderRequired == TRUE) {
     Status = FpgaLoader (PeiServices, &FpgaConfigurationPointer);
    }
    //
    // If we have all enabled sockets programmed, then no more retries are needed
    //
    if ((FpgaConfigurationPointer.FpgaSktProgrammed == FpgaConfigurationPointer.FpgaSktPresent) || (FpgaConfigurationPointer.FpgaSktProgrammed == FpgaConfigurationPointer.FpgaPlatformEnabled)) {
      DEBUG ((EFI_D_ERROR, "FPGA Loader,All enabled sockets programmed! Exiting retry loop\n"));
      Retries = FPGA_MAX_LOADER_RETRIES;
      break;
    }
  }

  //
  // Update any configuration values that might have changed.
  //
  DEBUG ((EFI_D_ERROR, "FPGA Loader, Updating configuration values!\n"));
  FpgaConfigurationSetValues (&FpgaConfigurationPointer);

  //
  // Disable unused FPGA Socket.
  //
  FpgaUnusedSocketDisable(&FpgaConfigurationPointer);

  //
  // Set the bitstream download attempted bit.
  //
  DEBUG ((EFI_D_ERROR, "Fpga Loader, Set the biosscratchpad1 bit5 to indicate bitstream download was attempted.\n"));
  BiosScratchPad1.Data = mMrcHooksChipServices->ReadCpuPciCfgEx (host, (UINT8)mBspSocketId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG);
  BiosScratchPad1.Data |= BIT5;
  mMrcHooksChipServices->WriteCpuPciCfgEx (host, (UINT8)mBspSocketId, 0, BIOSSCRATCHPAD1_UBOX_MISC_REG, BiosScratchPad1.Data);

  //
  // Warm Reset is required for KTI code to retrain the FPGA link.
  //
  DEBUG ((EFI_D_ERROR, "FPGA Loader, Warm Reset!!\n"));
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gPchResetPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PchResetPpi
                             );

  PchResetPpi->Reset (PchResetPpi, WarmReset);

  CpuDeadLoop();

  return Status;
}
