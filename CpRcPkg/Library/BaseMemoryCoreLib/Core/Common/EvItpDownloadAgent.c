/** @file
  BIOS SSA loader ITP implementation.

@copyright
  Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include "SysHost.h"
#include "EvItpDownloadAgent.h"
#include "CoreApi.h"
#include "MemFunc.h"
#include "ChipApi.h"

#ifdef SSA_FLAG
#ifdef SSA_LOADER_FLAG

#ifndef MINIBIOS_BUILD
#ifdef _MSC_VER
#pragma warning(disable : 4100)
#endif
extern EFI_GUID  gSsaBiosServicesPpiGuid;

UINT32 flushBios(void)
{
  //
  // Workaround that is used to force DCU content into MLC
  //
  UINT32          temp = 0;
  UINT32          codeRegionStartAdr = 0xFFF00000;
  UINT32          codeSizeInDWords = 128 * 1024 / 4;   // 128K / 4
  volatile UINT32 *codeAdr;
  UINT32          DWordIdx;
  UINT32          repeatCnt = 2;
  UINT32          repeatIdx;

  //
  // Read the BIOS code region a few times to force the EV tool image to be
  // flushed into MLC
  //
  for (repeatIdx = 0 ; repeatIdx < repeatCnt; repeatIdx++) {
    for (DWordIdx = 0; DWordIdx < codeSizeInDWords; DWordIdx++) {
      codeAdr = (volatile UINT32 *) (codeRegionStartAdr + DWordIdx * 4);
      temp |= *codeAdr;
    }
  }

return temp;
}

/*++

Routine Description:

  Do EV commands while at a the RUN_EV_TOOL checkpoint

  @param pHandle         - Pointer to the system structure
  @param CheckpointData  - Pointer to the CheckpointData

  @retval 0 - successful
  @retval 1 - failure

--*/
VOID
EvItpDownloadCmd (PSYSHOST host, VOID *CheckpointData) {
  MRC_EV_AGENT_CMD_STRUCT cmdReg;
  UINT32                  toolSize;
  UINT32                  imageHandle = 0;
  UINT32                  inputNum = 0;
  UINT32                  entryPoint;
  UINT32                  inputBufferForITP = 0;
  UINT32                  memBlockHandle = 0;
  UINT16                  majorRev;
  UINT16                  minorRev;
  BOOLEAN                 exitCmdLoop = FALSE;
  SSA_STATUS              retStatus;
  EFI_PEI_SERVICES        **PeiServices;
  SSA_BIOS_SERVICES_PPI   *SsaServicesPpi;

  //
  // Memory buffer to separate downloaded code from other data.
  // basically the download image is sandwiched by these 4KB buffer.
  //
  UINT32 totalImageBuffer = 0;

  //
  // "EVCP" - EV Check Point
  //
  UINT32 checkPoint = ('E' << 24) | ('V' << 16) |('C' <<8) | 'P';

  DEBUG ((
      DEBUG_ERROR,
      "Inside EV agent ... \n"
      ));

  PeiServices = (EFI_PEI_SERVICES **) host->var.common.oemVariable;
  retStatus = (*PeiServices)->LocatePpi(PeiServices, &gSsaBiosServicesPpiGuid, 0, NULL, &SsaServicesPpi);
  ASSERT_EFI_ERROR (retStatus);

  //
  // First indicate we are in the checkpoint where running EV content
  //
  writeScratchPad_currentCheckPoint (host, checkPoint);

  do {
    //
    // Read scratchpad register to get the EV command
    //
    cmdReg.Data = readScratchPad_CMD(host);

    //
    // Execute cmd
    //
    if (cmdReg.Bits.command_pending_execution) {
      //
      // Update the cmd register to indicate the cmd is no more pending
      //
      cmdReg.Bits.command_pending_execution = MRC_EVAGENT_NO_PENDING_CMD;
      cmdReg.Bits.status                    = MRC_EVAGENT_STATUS_BUSY;
      CHIP_FUNC_CALL(host, writeScratchPad_CMD (host, cmdReg.Data));

      //
      // Execute
      //
      switch (cmdReg.Bits.command_opcode) {
        // PING
        case MRC_EVAGENT_CMD_PING:
          //
          // Update status
          //
          cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          break;

        //
        // Get the callback table revision
        //
        case MRC_EVAGENT_CMD_GET_CALLBACK_REV:
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_CALLBACK_REV  called ... \n"
            ));
          majorRev = *(((UINT16*)(&SsaServicesPpi->SsaCommonConfig->Revision)) + 1);
          minorRev = *((UINT16*)(&SsaServicesPpi->SsaCommonConfig->Revision));
          writeScratchPad_DATA0 (host, majorRev);
          writeScratchPad_DATA1 (host, minorRev);
          cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          break;

        case MRC_EVAGENT_CMD_GET_AGENT_VERSION:
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_AGENT_VERSION  called ... \n"
            ));
          writeScratchPad_DATA0 (host, (TOOL_MAJOR_VERSION << 16) | (TOOL_MINOR_VERSION));
          writeScratchPad_DATA1 (host, TOOL_REVISION);
          cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          break;

        case MRC_EVAGENT_CMD_GET_DLL_VERSION:
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_DLL_VERSION  called ... \n"
            ));
          if (imageHandle) {
            //
            // Call the PE module
            //
            entryPoint = GetPeEntrypoint(host, (UINT8 *)imageHandle);

            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          } else {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_INVALID_TOOL_HANDLE;
          }
          break;

        case MRC_EVAGENT_CMD_FLUSH_CACHE:
          DEBUG ((
          DEBUG_ERROR,
          "MRC_EVAGENT_CMD_FLUSH_CACHE  called ... \n"
          ));

        flushBios();
        cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
        DEBUG ((
          DEBUG_ERROR,
          "MRC_EVAGENT_CMD_FLUSH_CACHE  done ... \n"
          ));
        break;
        //
        // Get the memory buffer where the EV tool will be placed
        //
        case MRC_EVAGENT_CMD_GET_TOOL_BUFFER:
          //
          // Get the size of the tool image
          //
          toolSize = readScratchPad_DATA0 (host);

          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_TOOL_BUFFER before malloc ... \n"
            ));

          //
          // Reserve 4KB buffers below and above the tool image
          //
          totalImageBuffer = (UINT32) SsaServicesPpi->SsaCommonConfig->Malloc (SsaServicesPpi, (1024 * 4) + toolSize + (1024 * 4));
          imageHandle = totalImageBuffer + (1024 * 4);

          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_TOOL_BUFFER after malloc ... \n"
            ));

          //
          // Update the data0 with the imageHandle
          //
          writeScratchPad_DATA0 (host, imageHandle);

          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_TOOL_BUFFER after writeScratchPad_DATA0 ... \n"
            ));

          if (!totalImageBuffer)
          {
            DEBUG ((
              DEBUG_ERROR,
              "MRC_EVAGENT_CMD_GET_TOOL_BUFFER  !totalImageBuffer ... \n"
              ));
          }

          if (!totalImageBuffer) {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_MALLOC_FAIL;
          } else {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          }
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_TOOL_BUFFER  done ... \n"
            ));

          break;

        //
        // Load tool
        //
        case MRC_EVAGENT_CMD_LOAD_TOOL:
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_LOAD_TOOL  called ... \n"
            ));
          //
          // Call loader to locate the PE module
          //
          if (imageHandle) {
            PlacePeImage(host, (UINT8 *)imageHandle);
            flushBios ();
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          } else {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_INVALID_TOOL_HANDLE;
          }
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_LOAD_TOOL  done ... \n"
            ));

          break;
        //
        // Unload tool
        //
        case MRC_EVAGENT_CMD_UNLOAD_TOOL:
      DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_UNLOAD_TOOL  called ... \n"
            ));
          //
          // Get the imageHandle of the tool image from data0
          //
          memBlockHandle = readScratchPad_DATA0(host);
          if ((memBlockHandle == imageHandle) && (memBlockHandle != 0)) {
            //
            // Free the two memory buffers as well
            //
            SsaServicesPpi->SsaCommonConfig->Free (SsaServicesPpi, (void *) totalImageBuffer);
            //
            // Reset the imageHandle
            //
            imageHandle = 0;
            totalImageBuffer = 0;
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          } else {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_INVALID_TOOL_HANDLE;
          }
      DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_UNLOAD_TOOL  done ... \n"
            ));
          break;

        //
        // Get the location of input parameter and output data
        //
        case MRC_EVAGENT_CMD_GET_INPUT_BUFFER:
            DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_INPUT_BUFFER  called ... \n"
            ));
          // Get the number of input and output from data0 and data1
          //
          inputNum = readScratchPad_DATA0(host);
            DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_INPUT_BUFFER  readScratchPad_DATA0 done ... \n"
            ));

          // Input location
          //
          inputBufferForITP = (UINT32)SsaServicesPpi->SsaCommonConfig->Malloc(SsaServicesPpi, (inputNum * sizeof(UINT32)));

          //
          // Update the data0 and data1
          //
          CHIP_FUNC_CALL(host, writeScratchPad_DATA0(host, inputBufferForITP));

          if (!inputBufferForITP) {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_MALLOC_FAIL;
          } else {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          }

      DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_GET_INPUT_BUFFER  done ... \n"
            ));
          break;


        //
        //Free Results to prevent memory fragmentation
        //
        case MRC_EVAGENT_CMD_FREE_RESULTS:
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_FREE_RESULTS  called ... \n"
            ));
          if (SsaServicesPpi->SsaResultsConfig->ResultsData != NULL) {
            DEBUG ((
              DEBUG_ERROR,
              "Freeing result buffer ... \n"
              ));
            SsaServicesPpi->SsaCommonConfig->Free(SsaServicesPpi, SsaServicesPpi->SsaResultsConfig->ResultsData);
            SsaServicesPpi->SsaResultsConfig->ResultsData = NULL;
          }
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_FREE_RESULTS  done ... \n"
            ));
          break;

        //
        // Release the memory occupied by the input parameter and output data
        //
        case MRC_EVAGENT_CMD_FREE_MEMORY:
      DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_FREE_MEMORY  called ... \n"
            ));
          //
          //
          // Get the memory block handle(aka. address) from data0
          //
          memBlockHandle = readScratchPad_DATA0 (host);

          if (memBlockHandle) {
            SsaServicesPpi->SsaCommonConfig->Free(SsaServicesPpi,(void*)memBlockHandle);
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
          } else {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_INVALID_MEMORY_HANDLE;
          }
      DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_FREE_MEMORY  done ... \n"
            ));
          break;

        //
        // Run EV tool
        //
        case MRC_EVAGENT_CMD_RUN_EV_TOOL:
          DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_RUN_EV_TOOL  called ... \n"
            ));
          //
          // Check imageHandle first
          //
          if (imageHandle){
      
            // Clear the ResultsData
            SsaServicesPpi->SsaResultsConfig->ResultsData = NULL;
      
            //
            // Call the PE module
            //
            entryPoint = GetPeEntrypoint (host, (UINT8 *)imageHandle);

            ((EvToolEntryPoint*)entryPoint) (SsaServicesPpi, (UINT32 *)inputBufferForITP);
      
            // If there are results
            if(SsaServicesPpi->SsaResultsConfig->ResultsData != NULL){
              RESULTS_DATA_HDR *ResultsDataHdr ;
              UINT32 ResultsDataSize;

              ResultsDataHdr = (RESULTS_DATA_HDR *) SsaServicesPpi->SsaResultsConfig->ResultsData;

              ResultsDataSize = sizeof(RESULTS_DATA_HDR) + ResultsDataHdr->MdBlock.MetadataSize + ( ResultsDataHdr->RsBlock.ResultElementSize * ResultsDataHdr->RsBlock.ResultCapacity);

              // Set the results block memory position.
              writeScratchPad_DATA0 (host, (UINT32)SsaServicesPpi->SsaResultsConfig->ResultsData);
              // Set the results block size.
              writeScratchPad_DATA1 (host, ResultsDataSize);

              cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
            } else {  // No results to pass back

              // Set the results block memory position.
              CHIP_FUNC_CALL(host, writeScratchPad_DATA0 (host, 0));
              // Set the results block size.
              CHIP_FUNC_CALL(host, writeScratchPad_DATA1 (host, 0));

              cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
            }
          } else {
            cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_INVALID_TOOL_HANDLE;
          }
    
      DEBUG ((
            DEBUG_ERROR,
            "MRC_EVAGENT_CMD_RUN_EV_TOOL  done ... \n"
            ));
          break;

          //
          // EXIT
          //
        case MRC_EVAGENT_CMD_EXIT:
      DEBUG ((
      DEBUG_ERROR,
      "MRC_EVAGENT_CMD_EXIT  called ... \n"
      ));
      exitCmdLoop = TRUE;
      //
      // Update status
      //
      cmdReg.Bits.error_code = MRC_EVAGENT_CMD_NO_ERROR;
      
      DEBUG ((
      DEBUG_ERROR,
      "MRC_EVAGENT_CMD_EXIT  done ... \n"
      ));
      break;

        default:
      DEBUG ((
            DEBUG_ERROR,
            "Unknown command  called ... \n"
            ));
          //
          // Unknown command
          //
          cmdReg.Bits.error_code = MRC_EVAGENT_CMD_ERROR_UNKNOWN_CMD;
    
      DEBUG ((
            DEBUG_ERROR,
            "Unknown command  done ... \n"
            ));
          break;
      }
      //
      // Update status
      //
      cmdReg.Bits.status = MRC_EVAGENT_STATUS_READY;
      writeScratchPad_CMD(host, cmdReg.Data);
    } // cmdReg.Bits.command_pending_execution
  } while (!exitCmdLoop); // While still in the cmd loop

  //return mrcSuccess;
}
#endif //MINIBIOS_BUILD
#endif //SSA_LOADER_FLAG

#else
// Future placeholder: BSSA code intentionally left out for now
#endif //SSA_FLAG
