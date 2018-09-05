/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2014 - 2016 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


  @file MemrasS3Save.c

  MemrasS3 Save support

**/

#include "MemRas.h"
#include <Library/mpsyncdatalib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/CompressLib.h>

extern EFI_GUID             gEfiMemoryConfigDataGuid;
extern MEMRAS_S3_PARAM      mMemrasS3Param;
extern SDDC_PLUSONE_STATUS  mSddcPlusOneStatus[MC_MAX_NODE][MAX_MC_CH][MAX_RANK_CH];

static EFI_SMM_VARIABLE_PROTOCOL   *mSmmVariable;
EFI_HANDLE                   mDecompressHandle;

EFI_DECOMPRESS_PROTOCOL  gEfiDecompress = {
  SmmRasUefiDecompressGetInfo,
  SmmRasUefiDecompress
};

/**
  Given a compressed source buffer, this function retrieves the size of the
  uncompressed buffer and the size of the scratch buffer required to decompress
  the compressed source buffer.

  The GetInfo() function retrieves the size of the uncompressed buffer and the
  temporary scratch buffer required to decompress the buffer specified by Source
  and SourceSize. If the size of the uncompressed buffer or the size of the
  scratch buffer cannot be determined from the compressed data specified by
  Source and SourceData, then EFI_INVALID_PARAMETER is returned. Otherwise, the
  size of the uncompressed buffer is returned in DestinationSize, the size of
  the scratch buffer is returned in ScratchSize, and EFI_SUCCESS is returned.
  The GetInfo() function does not have scratch buffer available to perform a
  thorough checking of the validity of the source data. It just retrieves the
  "Original Size" field from the beginning bytes of the source data and output
  it as DestinationSize. And ScratchSize is specific to the decompression
  implementation.

  @param  This               A pointer to the EFI_DECOMPRESS_PROTOCOL instance.
  @param  Source             The source buffer containing the compressed data.
  @param  SourceSize         The size, in bytes, of the source buffer.
  @param  DestinationSize    A pointer to the size, in bytes, of the
                             uncompressed buffer that will be generated when the
                             compressed buffer specified by Source and
                             SourceSize is decompressed.
  @param  ScratchSize        A pointer to the size, in bytes, of the scratch
                             buffer that is required to decompress the
                             compressed buffer specified by Source and
                             SourceSize.

  @retval EFI_SUCCESS        The size of the uncompressed data was returned in
                             DestinationSize and the size of the scratch buffer
                             was returned in ScratchSize.
  @retval EFI_INVALID_PARAMETER The size of the uncompressed data or the size of
                                the scratch buffer cannot be determined from the
                                compressed data specified by Source and
                                SourceSize.

**/
EFI_STATUS
EFIAPI
SmmRasUefiDecompressGetInfo (
  IN EFI_DECOMPRESS_PROTOCOL            *This,
  IN   VOID                             *Source,
  IN   UINT32                           SourceSize,
  OUT  UINT32                           *DestinationSize,
  OUT  UINT32                           *ScratchSize
  )
{
  if (Source == NULL || DestinationSize == NULL || ScratchSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return UefiDecompressGetInfo (Source, SourceSize, DestinationSize, ScratchSize);
}


/**
  Decompresses a compressed source buffer.

  The Decompress() function extracts decompressed data to its original form.
  This protocol is designed so that the decompression algorithm can be
  implemented without using any memory services. As a result, the Decompress()
  Function is not allowed to call AllocatePool() or AllocatePages() in its
  implementation. It is the caller's responsibility to allocate and free the
  Destination and Scratch buffers.
  If the compressed source data specified by Source and SourceSize is
  sucessfully decompressed into Destination, then EFI_SUCCESS is returned. If
  the compressed source data specified by Source and SourceSize is not in a
  valid compressed data format, then EFI_INVALID_PARAMETER is returned.

  @param  This                A pointer to the EFI_DECOMPRESS_PROTOCOL instance.
  @param  Source              The source buffer containing the compressed data.
  @param  SourceSize          SourceSizeThe size of source data.
  @param  Destination         On output, the destination buffer that contains
                              the uncompressed data.
  @param  DestinationSize     The size of the destination buffer.  The size of
                              the destination buffer needed is obtained from
                              EFI_DECOMPRESS_PROTOCOL.GetInfo().
  @param  Scratch             A temporary scratch buffer that is used to perform
                              the decompression.
  @param  ScratchSize         The size of scratch buffer. The size of the
                              scratch buffer needed is obtained from GetInfo().

  @retval EFI_SUCCESS         Decompression completed successfully, and the
                              uncompressed buffer is returned in Destination.
  @retval EFI_INVALID_PARAMETER  The source buffer specified by Source and
                                 SourceSize is corrupted (not in a valid
                                 compressed format).
**/
EFI_STATUS
EFIAPI
SmmRasUefiDecompress (
  IN     EFI_DECOMPRESS_PROTOCOL          *This,
  IN     VOID                             *Source,
  IN     UINT32                           SourceSize,
  IN OUT VOID                             *Destination,
  IN     UINT32                           DestinationSize,
  IN OUT VOID                             *Scratch,
  IN     UINT32                           ScratchSize
  )
{
  if (Source == NULL || Destination== NULL || Scratch == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return UefiDecompress (Source, Destination, Scratch);
}

INT16 MemRasCrc16 (char *ptr, int count)
{
  INT16 crc, i;
  crc = 0;
  while (--count >= 0)
  {
    crc = crc ^ (INT16)(int)*ptr++ << 8;
    for (i = 0; i < 8; ++i)
    {
      if (crc & 0x8000)
      {
        crc = crc << 1 ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    }
  }
  return (crc & 0xFFFF);
}

/*++

Routine Description:
    Get or update MemoryConfigX variables.
    This routine will not create the variables if they do not already exist.

Arguments:

  GetVars    - Flag is TRUE  to indicate routine is to get the variable data.
               Flag is FALSE to indicate routine is to update the variable data.
  *Buffer    - Pointer to caller-supplied buffer for the variable data.
  BufferSize - Size of the buffer.

Returns:

    Status   - EFI_SUCCESS if no failures
               non-zero error code for failures
--*/
EFI_STATUS
MemoryConfigVars (
  IN     BOOLEAN GetVars,
  IN     UINTN   BufferSize,
  IN OUT CHAR8   *Buffer
  )
{
  #define MEM_CFG_VARS_ATTRIBS (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

  EFI_STATUS     Status;
  UINTN          VariableSize;
  CHAR16         EfiMemoryConfigVariable[] = L"MemoryConfig0";

  UINTN                      CompressedVariableSize;
  UINT32                     ScratchSize;
  VOID                       *CompressedData = NULL;
  VOID                       *Scratch = NULL;
  EFI_DECOMPRESS_PROTOCOL    *Decompress = NULL;

  VOID                            *CompressedVariableData = NULL;
  UINTN                           CompressedBufferSize;

  Status = EFI_SUCCESS; // Satisfy compiler.

  if (Buffer == NULL) {
    BufferSize = 0;
  }

  if (BufferSize == 0) {
    Status = EFI_INVALID_PARAMETER;
  }

  Status = gSmst->SmmLocateProtocol (&gEfiDecompressProtocolGuid, NULL, (VOID **) &Decompress);
  if (EFI_ERROR (Status)) {
    return EFI_PROTOCOL_ERROR;
  }

  while (BufferSize) {

    //
    // Get the compressed variable size.
    //
    VariableSize = 0 ;
    CompressedVariableSize = 0;
    ScratchSize = 0;

    Status = mSmmVariable->SmmGetVariable (
                        EfiMemoryConfigVariable,
                        &gEfiMemoryConfigDataGuid,
                        NULL,
                        &CompressedVariableSize,
                        NULL
                        );

    if (Status == EFI_BUFFER_TOO_SMALL) {
      Status = gSmst->SmmAllocatePool (
                             EfiRuntimeServicesData,
                             CompressedVariableSize,
                             &CompressedData
                             );

      //
      // Get compressed variable.
      //
      if(!EFI_ERROR (Status)){
        Status = mSmmVariable->SmmGetVariable (
                    EfiMemoryConfigVariable,
                    &gEfiMemoryConfigDataGuid,
                    NULL,
                    &CompressedVariableSize,
                    CompressedData
                    );
      }

      //
      // Get info about buffers needed to compression and decompression
      //
      if(!EFI_ERROR (Status)) {
          Status = Decompress->GetInfo (
                         Decompress,
                         CompressedData,
                         (UINT32)CompressedVariableSize,
                         (UINT32*)&VariableSize,
                         &ScratchSize
                         );
      }

      if (VariableSize > BufferSize) {
        Status = EFI_BAD_BUFFER_SIZE;
      } else {

        if (GetVars) {
            //
            // Allocate memory for additional buffer needed to decompression
            //
            if(Status == EFI_SUCCESS) {
              Status = gSmst->SmmAllocatePool (
                     EfiRuntimeServicesData,
                     ScratchSize,
                     &Scratch
                     );
            }

            //
            // Do decompression
            //
            if(!EFI_ERROR (Status)) {
              Status = Decompress->Decompress (
                             Decompress,
                             CompressedData,
                             (UINT32)CompressedVariableSize,
                             Buffer,
                             (UINT32)VariableSize,
                             Scratch,
                             ScratchSize
                             );
            }

            if(Scratch != NULL) {
              gSmst->SmmFreePool (Scratch);
              Scratch = NULL;
            }
            if(CompressedData != NULL) {
              gSmst->SmmFreePool (CompressedData);
              CompressedData = NULL;
            }
          } else {
            if(CompressedData != NULL) {
              gSmst->SmmFreePool (CompressedData);
              CompressedData = NULL;
            }
            //
            // Update the variable.
            //

            //
            // Allocate memory for compressed data.
            //

            CompressedBufferSize = VariableSize;

            if(!EFI_ERROR (Status)) {
              Status = gSmst->SmmAllocatePool (
                            EfiRuntimeServicesData,
                            CompressedBufferSize,
                            &CompressedVariableData
                            );
            }

            if(!EFI_ERROR (Status)) {
              Status = Compress(Buffer, VariableSize, CompressedVariableData, &CompressedBufferSize);
            }

          if(!EFI_ERROR (Status)) {
            Status = mSmmVariable->SmmSetVariable (
                      EfiMemoryConfigVariable,
                      &gEfiMemoryConfigDataGuid,
                      MEM_CFG_VARS_ATTRIBS,
                      CompressedBufferSize,
                      CompressedVariableData
                      );
            if (Status == EFI_OUT_OF_RESOURCES) {
              //
              // Delete variable and try again.
              //
              Status = mSmmVariable->SmmSetVariable (
                        EfiMemoryConfigVariable,
                        &gEfiMemoryConfigDataGuid,
                        MEM_CFG_VARS_ATTRIBS,
                        0,
                        CompressedVariableData
                        );
              if (!EFI_ERROR (Status)) {
                Status = mSmmVariable->SmmSetVariable (
                          EfiMemoryConfigVariable,
                          &gEfiMemoryConfigDataGuid,
                          MEM_CFG_VARS_ATTRIBS,
                          CompressedBufferSize,
                          CompressedVariableData
                          );
              }
            }
          }
          if(CompressedVariableData != NULL) {
            gSmst->SmmFreePool (CompressedVariableData);
            CompressedVariableData = NULL;
          }
        }
      }
      if(CompressedData != NULL) { //for security in casae of some errors
        gSmst->SmmFreePool (CompressedData);
        CompressedData = NULL;
      }
    }

    if (!EFI_ERROR (Status)) {
      //
      // Move the buffer pointer.
      //
      Buffer += VariableSize;

      //
      // Update the remaining buffer size.
      //
      BufferSize -= VariableSize;

      //
      // Change to the next variable name.
      //
      EfiMemoryConfigVariable[12]++;
    } else {
      BufferSize = 0;
    }
  }

  return Status;
}

/**

  Saves all RAS settings that needs to be restored later during S3 resume boot

  @param DispatchHandle   -  The handle of this callback, obtained when registering.
  @param DispatchContext  -  The predefined context which contained sleep type and phase.

  @retval EFI_SUCCESS            -  Operation successfully performed.
  @retval EFI_INVALID_PARAMETER  -  Invalid parameter passed in.

**/
EFI_STATUS
MemRASS3EntryCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN CONST  EFI_SMM_SX_REGISTER_CONTEXT  *DispatchContext,
  IN OUT    VOID                         *CommBuffer,     OPTIONAL
  IN OUT    UINTN                        *CommBufferSize  OPTIONAL
  )
{
  UINT8   Socket;
  UINT8   Node;
  UINT8   Imc;
  UINT8   Ch;
  UINT8   SktCh;
  UINT8   Dimm;
  UINT8   Rank;
  UINT8   LogicalRank = 0;
  UINT8   spareInUse;
  UINTN   DataSize;
  INT16   data16;
  char    *memNvramPtr;
  UINT8   i;

  EFI_STATUS                      Status = EFI_SUCCESS;
  struct sysNvram                 *MemCfgData;
  SCRUBCTL_MC_MAIN_STRUCT         imcSCRUBCTL;
  SCRUBCTL_MC_MAIN_STRUCT         ScrubCtlReg;
  ECC_MODE_RANK0_MCDECS_STRUCT    EccModeMcDecs;
  ECC_MODE_RANK0_MCMAIN_STRUCT    EccModeMcMain;
  UINTN                           RegAddr;
  UINT32                          Data;
  UINT8                           Size;
  UINT32                          EccModeReg[]    = ECC_MODE_REG_LIST;
  UINT32                          EccModeRegExt[] = ECC_MODE_REG_EXT_LIST;

  //
  // Locate SmmVariableProtocol.
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&mSmmVariable);
  ASSERT_EFI_ERROR (Status);

  DataSize = sizeof (struct sysNvram);
  Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      DataSize,
                      &MemCfgData
                      );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) return EFI_OUT_OF_RESOURCES;

  //
  // Get the memory configuration variables.
  //
  Status = MemoryConfigVars (TRUE, DataSize, (UINT8 *) (MemCfgData));
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    gSmst->SmmFreePool (MemCfgData);
    return EFI_SUCCESS;
  }

  for(Node = 0; Node < MC_MAX_NODE; Node++) {
    Socket = NODE_TO_SKT(Node);
    Imc = Node % MAX_IMC;
    if(mSystemMemoryMap->Socket[Socket].imcEnabled[Imc] == 0) continue;

    // Save The Scrub address Low and High fields, if patrol scrub is on
    ScrubCtlReg.Data = mCpuCsrAccess->ReadMcCpuCsr( Socket, Imc, SCRUBCTL_MC_MAIN_REG);
    if( ScrubCtlReg.Bits.scrub_en == 1) {
      Data = mCpuCsrAccess->ReadMcCpuCsr( Socket, Imc, SCRUBADDRESSLO_MC_MAIN_REG);
      RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, SCRUBADDRESSLO_MC_MAIN_REG);
      RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite scrubAddrLo - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
      S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

      Data = mCpuCsrAccess->ReadMcCpuCsr( Socket, Imc, SCRUBADDRESSHI_MC_MAIN_REG);
      RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, SCRUBADDRESSHI_MC_MAIN_REG);
      RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite scrubAddrHi - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
      S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

      Data = mCpuCsrAccess->ReadMcCpuCsr( Socket, Imc, SCRUBADDRESS2LO_MC_MAIN_REG);
      RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, SCRUBADDRESS2LO_MC_MAIN_REG);
      RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite scrubAddr2Lo - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
      S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

      Data = mCpuCsrAccess->ReadMcCpuCsr( Socket, Imc, SCRUBADDRESS2HI_MC_MAIN_REG);
      RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, SCRUBADDRESS2HI_MC_MAIN_REG);
      RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite scrubAddr2Hi - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
      S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

      //Restore startscrub setting
	  ScrubCtlReg.Bits.startscrub = 1;
      Data = ScrubCtlReg.Data;
      RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, SCRUBCTL_MC_MAIN_REG);
      RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite ScrubCtlReg - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
      S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

      imcSCRUBCTL.Bits.scrub_en = 0;
      mCpuCsrAccess->WriteMcCpuCsr(Socket, Imc, SCRUBCTL_MC_MAIN_REG, imcSCRUBCTL.Data);
    }

    //
    // Leaky bucket node values
    //
    Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Imc, LEAKY_BUCKET_CFG_MC_MAIN_REG);
    RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, LEAKY_BUCKET_CFG_MC_MAIN_REG);
    RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite leakyBucketCfg - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

    Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Imc, LEAKY_BUCKET_CNTR_LO_MC_MAIN_REG);
    RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, LEAKY_BUCKET_CNTR_LO_MC_MAIN_REG);
    RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite leakyBucketCounterLo - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

    Data = mCpuCsrAccess->ReadMcCpuCsr (Socket, Imc, LEAKY_BUCKET_CNTR_HI_MC_MAIN_REG);
    RegAddr = mCpuCsrAccess->GetMcCpuCsrAddress (Socket, Imc, LEAKY_BUCKET_CNTR_HI_MC_MAIN_REG);
    RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite leakyBucketCounterHi - 0x%08x, 0x%08x\n", (UINTN)RegAddr, Data));
    S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );

    for(Ch = 0; Ch < MAX_MC_CH; Ch++) {
      SktCh = NODECH_TO_SKTCH(Node, Ch);
      if(mSystemMemoryMap->Socket[Socket].ChannelInfo[SktCh].Enabled == 0) continue;

      // save rank spare failover info
      spareInUse = mMemrasS3Param.spareInUse[Node][Ch];
      MemCfgData->mem.socket[Socket].channelList[SktCh].spareInUse = spareInUse;

      // save mirror failover info
      MemCfgData->mem.socket[Socket].channelList[SktCh].chFailed   = mMemrasS3Param.chFailed[Node][Ch];
      MemCfgData->mem.socket[Socket].channelList[SktCh].ngnChFailed   = mMemrasS3Param.ngnChFailed[Node][Ch];

      for (Dimm = 0; Dimm < MAX_DIMM; Dimm++) {
        if (MemCfgData->mem.socket[Socket].channelList[SktCh].dimmList[Dimm].dimmPresent == 0) continue;

        for (Rank = 0; Rank < MAX_RANK_DIMM; Rank++) {
          if (MemCfgData->mem.socket[Socket].channelList[SktCh].dimmList[Dimm].rankList[Rank].enabled == 0) continue;
          if (MemCfgData->mem.socket[Socket].channelList[SktCh].dimmList[Dimm].mapOut[Rank] == 1) continue;

          // save DRAM failure (SDDC) logical rank info
          if( mSddcPlusOneStatus[Node][Ch][Rank] == SddcPlusOneAvailable ) {
            LogicalRank = MemCfgData->mem.socket[Socket].channelList[SktCh].dimmList[Dimm].rankList[Rank].logicalRank;
            MemCfgData->mem.socket[Socket].channelList[SktCh].dimmList[Dimm].rankList[Rank].devTagInfo =
                mMemrasS3Param.devTag[Node][Ch][LogicalRank];
          }

          // Save physical rank id of old spared rank in nvram
          if (spareInUse == 1) {
            if (LogicalRank ==  mMemrasS3Param.oldLogicalRank[Node][Ch]) {
              MemCfgData->mem.socket[Socket].channelList[SktCh].oldSparePhysicalRank =
              MemCfgData->mem.socket[Socket].channelList[SktCh].dimmList[Dimm].rankList[Rank].phyRank;

              // Loop to add the oldSpare rank and New spare rank incase of multi rank support.
              //MemCfgData->nvram.mem.socket[Socket].channelList[SktCh].oldSparePhysicalRank[mMemrasS3Param.imc[NodeId].channelList[Ch].MultiSpareRankCnt] = SrcRank;
              //(mMemrasS3Param.MultiSpareRankCnt[Node][Ch])++;
              //MemCfgData->nvram.common.imc[Node].channelList[Ch].newSparePhysRank =
              //MemCfgData->nvram.common.imc[Node].channelList[Ch].dimmList[Dimm].rankList[Rank].phyRank;

            } // if (LogicalRank)
          } // if (SpareInUse)

        } // rank
      } // Dimm

      //
      // Per rank error counts and thresholds and status
      //
      MemCfgData->mem.socket[Socket].channelList[SktCh].rankErrCountStatus = mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, CORRERRORSTATUS_MCDDC_DP_REG);
      for (i = 0; i < MAX_RANK_CH/2; i++) {
        MemCfgData->mem.socket[Socket].channelList[SktCh].rankErrCountInfo[i] = mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, CORRERRCNT_0_MCDDC_DP_REG + (i*4));
        MemCfgData->mem.socket[Socket].channelList[SktCh].rankErrThresholdInfo[i] = mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, CORRERRTHRSHLD_0_MCDDC_DP_REG + (i*4));
      }
      for (i = 0; i < MAX_RANK_CH; i++) {
        // Save Plus 1 data
        MemCfgData->mem.socket[Socket].channelList[SktCh].Plus1Failover[i] = mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, PLUS1_RANK0_MCDDC_DP_REG + i);

        //Save ECC modes
        EccModeMcDecs.Data = (UINT8) mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, EccModeReg[i]);
        EccModeMcMain.Data = (UINT8) mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, EccModeRegExt[i]);
        MemCfgData->mem.socket[Socket].channelList[SktCh].EccModeMcDecs[i] = (UINT8)EccModeMcDecs.Bits.mode;
        MemCfgData->mem.socket[Socket].channelList[SktCh].EccModeMcMain[i] = (UINT8)EccModeMcMain.Bits.mode;

      } // for (i)
       
      MemCfgData->mem.socket[Socket].channelList[SktCh].LinkRetryErrLimits = mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, LINK_RETRY_ERR_LIMITS_MCDDC_DP_REG);
      MemCfgData->mem.socket[Socket].channelList[SktCh].LinkLinkFail = mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, LINK_LINK_FAIL_MCDDC_DP_REG);
            
      //
      // Leaky bucket
      //
      Data = mCpuCsrAccess->ReadCpuCsr (Socket, SktCh, LEAKY_BKT_2ND_CNTR_REG_MCDDC_DP_REG);
      RegAddr = mCpuCsrAccess->GetCpuCsrAddress (Socket, SktCh, LEAKY_BKT_2ND_CNTR_REG_MCDDC_DP_REG, &Size);
      RASDEBUG ((DEBUG_INFO, "S3BootScriptSaveMemWrite leakyBucket2ndCounter - 0x%08x, 0x%08x, 0x%x\n", (UINTN)RegAddr, Data, Size));
      S3BootScriptSaveMemWrite (S3BootScriptWidthUint32, RegAddr, 1, &Data );
    }  // for (Ch)


    //
    // SMI settings.
    //
    MemCfgData->mem.socket[Socket].smiCtrlUboxMisc = mCpuCsrAccess->ReadCpuCsr (Socket, 0, SMICTRL_UBOX_MISC_REG);
    MemCfgData->mem.socket[Socket].imc[Imc].EmcaLtCtlMcMainExt   = (UINT8) mCpuCsrAccess->ReadMcCpuCsr (Socket, Imc, EMCALTCTL_MC_MAIN_REG);
    MemCfgData->mem.socket[Socket].imc[Imc].ExRasConfigHaCfg     = mCpuCsrAccess->ReadMcCpuCsr (Socket, Imc, EXRAS_CONFIG_M2MEM_MAIN_REG);
    MemCfgData->mem.socket[Socket].imc[Imc].SmiSpareCtlMcMainExt = mCpuCsrAccess->ReadMcCpuCsr (Socket, Imc, SMISPARECTL_MC_MAIN_REG);
  } //Node

  // Update the CRC checksum of the memory nvram section
  MemCfgData->mem.crc16 = 0;
  memNvramPtr = (char *)(&(MemCfgData->mem));
  data16 = MemRasCrc16 (memNvramPtr, sizeof(MemCfgData->mem));
  MemCfgData->mem.crc16 = data16;

  //
  // Update the memory configuration variables.
  //
  Status = MemoryConfigVars (FALSE, DataSize, (UINT8 *) (MemCfgData));
  ASSERT_EFI_ERROR (Status);

  gSmst->SmmFreePool (MemCfgData);

  return EFI_SUCCESS;
}

VOID
IniMemRasS3 (VOID)
/**

    Memory ras s3 driver entry point

    @param None

    @retval None

**/
{

  EFI_STATUS                      Status;
  EFI_SMM_SX_REGISTER_CONTEXT     *EntryS3DispatchContext;
  EFI_SMM_SX_DISPATCH2_PROTOCOL   *SxDispatch;
  EFI_HANDLE                      SxDispatchHandle;
  UINTN                           Length;

  Length = sizeof ( EFI_SMM_SX_REGISTER_CONTEXT);
  Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Length,
                      &EntryS3DispatchContext
                      );
  if (EFI_ERROR (Status))  return ;

  ZeroMem (EntryS3DispatchContext, sizeof (EFI_SMM_SX_REGISTER_CONTEXT));

  Status = gSmst->SmmLocateProtocol (
                &gEfiSmmSxDispatch2ProtocolGuid,
                NULL,
                &SxDispatch
                );
  if (EFI_ERROR (Status))  return;
  //
  // Register the callback for S3 entry
  //
  EntryS3DispatchContext->Type  = SxS3;
  EntryS3DispatchContext->Phase = SxEntry;
  Status = SxDispatch->Register (
                                SxDispatch,
                                MemRASS3EntryCallback,
                                EntryS3DispatchContext,
                                &SxDispatchHandle
                                );
  ASSERT_EFI_ERROR (Status);

  mDecompressHandle = NULL;

  Status = gSmst->SmmInstallProtocolInterface (
                      &mDecompressHandle,
                      &gEfiDecompressProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &gEfiDecompress
                      );
  return;
}


