//****************************************************************************
//****************************************************************************
//**                                                                        **
//**            (C)Copyright 1993-2017 Supermicro Computer, Inc.            **
//**                                                                        **
//****************************************************************************
//****************************************************************************
//  File History
//
//  Rev. 1.00
//      Bug Fixed:  Log/Show MRC error/warning by major code (refer Intel Purley MRC Error Codes_draft_0.3.xlsx)
//      Reason:     
//      Auditor:    Jimmy Chiu
//      Date:       Jun/05/2017
//
//*****************************************************************************
/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/*************************************************************************
 *
 * Memory Reference Code
 *
 * ESS - Enterprise Silicon Software
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2014 - 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel
 * Corporation or its suppliers or licensors. Title to the Material
 * remains with Intel Corporation or its suppliers and licensors.
 * The Material contains trade secrets and proprietary and confidential
 * information of Intel or its suppliers and licensors. The Material
 * is protected by worldwide copyright and trade secret laws and treaty
 * provisions.  No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you
 * by disclosure or delivery of the Materials, either expressly,
 * by implication, inducement, estoppel or otherwise. Any license
 * under such intellectual property rights must be express and
 * approved by Intel in writing.
 *
 ************************************************************************/
#include "MemAddrMap.h"

//VOID DisplayBuffer( PSYSHOST host, UINT8 *buffer, UINT32 length )
//{
//  UINT32 i ;
//
//  for( i = 0 ; i < length ; i++ )  {
//
//    rcPrintf((host, "%02X ", *buffer++ ));;
//    if( ( ( ( i + 1 ) % 16 ) == 0 ) && ( i != 0 ) )  {
//      rcPrintf(( host, "\n" )) ;
//    }
//  }
//  rcPrintf(( host, "\n" )) ;
//
//  return ;
//}

/**

This routine computes the checksum of a table of given size.

@param TblStart     -  Start of the table for which checksum needs to be computed
@param BytesCount   -  Total size of the table for which checksum needs to be computed

@retval Checksum computed
@retval VOID

**/
UINT8
ChsumTbl(
   UINT8 *TblStart,
   UINT32 BytesCount
   ) {
  UINTN                          i;
  UINT8                          res = *TblStart;

  for (i = 1; i < BytesCount; i++) res = res + TblStart[i];

  return (res);
} // ChsumTbl


UINT16
EncodePartitionResponse(UINT8 status)
/**

  This routine translates the error codes related to the Partion requests to proper
  error responses defined in the spec.
  @param status - Status of the Partion Request

  @retval Value that needs to be programmed in the response field for this request
**/
{
  switch (status) {

  case PARTITIONING_SUCCESS:
    return NGN_PART_SIZE_STAT_SUCCESS;

  case INTERLEAVE_PARTICIPANT_UNAVAILABLE:
    return NGN_PART_SIZE_STAT_INTERLEAVE_PARTICIPANT_MISSING;

  case INTERLEAVE_PARAMETER_MISMATCH:
    return NGN_PART_SIZE_STAT_INTERLEAVE_SET_NOT_FOUND;

  case FNV_FW_ERROR:
    return NGN_PART_SIZE_STAT_AEP_FW_ERROR;

  case PARTITION_TOO_BIG:
    return NGN_PART_SIZE_STAT_PARTITION_SIZE_TOO_BIG;

  case  SAD_RULE_UNAVAILABLE:
    return NGN_PART_SIZE_STAT_INSUFFICIENT_DRAM_RULES;

  default:
    return NGN_PART_SIZE_STAT_UNDEFINED;
  }
}


UINT8
EncodeInterleaveResponse(UINT8 status)
/**

  This routine translates the error codes related to the Interleave  requests to proper
  error responses defined in the spec.
  @param status - Status of the Interleave Request

  @retval Value that needs to be programmed in the response field for this request
**/
{
  switch (status) {

  case OPERATION_SUCCESS:
    return NGN_INT_CH_STAT_SUCCESS;

  case INTERLEAVE_PARTICIPANT_UNAVAILABLE:
    return NGN_INT_CH_STAT_MATCHING_DIMM_MISSING;

  case INTERLEAVE_PARAMETER_MISMATCH:
   return NGN_INT_CH_STAT_INTERLEAVE_PARAMETERS_MISMATCH;

  case SAD_RULE_UNAVAILABLE:
    return NGN_INT_CH_STAT_INSUFFICIENT_DRAM_RULES;

  case PARTITIONING_REQUEST_FAILED:
    return NGN_INT_CH_STAT_PARTITIONING_FAILED;

  case MISSING_CFG_REQUEST:
    return NGN_INT_CH_STAT_CFG_REQ_MISSING;

  case CHANNEL_INTERLEAVE_MISMATCH:
    return NGN_INT_CH_STAT_CHANNEL_INTERLEAVE_MISMATCH;

  case NM_POPULATION_MISMATCH:
    return NGN_INT_CH_STAT_NEAR_MEMORY_POPULATION_MISMATCH;

  case MIRROR_MAPPING_FAILED:
    return NGN_INT_CH_STAT_MIRROR_MAPPING_FAILED;

  case INTERLEAVE_NOT_ALIGNED:
    return NGN_INT_CH_STAT_PARTITION_OFFSET_NOT_ALIGNED;

  default:
    return NGN_INT_CH_STAT_NOT_PROCESSED;
  }
}

UINT8
EncodeInterleaveGranularity( PSYSHOST host, UINT8 memType, UINT32 interleaveFormat )
/**

  This routine encodes the interelave granularity
  @param host             - Pointer to sysHost
  @param memType          - Memory type
  @param interleaveFormat - Interleave format

  @retval

**/
{
  UINT8 chIntSize, tgtIntSize ;

  tgtIntSize = (UINT8) (interleaveFormat >> 8);
  chIntSize = (UINT8) interleaveFormat;

  if(memType == MEM_TYPE_PMEM) {

    if( ( tgtIntSize == INTERLEAVE_FORMAT_4KB ) && ( chIntSize == INTERLEAVE_FORMAT_256B ) )  {
      return( MEM_INT_GRAN_PMEM_NUMA_4KB_256B ) ;
    }
    else if( ( tgtIntSize == INTERLEAVE_FORMAT_4KB ) && ( chIntSize == INTERLEAVE_FORMAT_4KB ) ) {
      return( MEM_INT_GRAN_PMEM_NUMA_4KB_4KB ) ;
    }
  }
  else if( memType == MEM_TYPE_PMEM_CACHE )  {

    if( ( tgtIntSize == INTERLEAVE_FORMAT_4KB ) && ( chIntSize == INTERLEAVE_FORMAT_64B ) )  {
      return( MEM_INT_GRAN_2LM_NM_4KB_64B_FM_4KB ) ;
    }
    else if( ( tgtIntSize == INTERLEAVE_FORMAT_4KB ) && ( chIntSize == INTERLEAVE_FORMAT_256B ) ) {
      return( MEM_INT_GRAN_2LM_NM_4KB_256B_FM_4KB ) ;
    }
  }

  return( 0xFF ) ;
}

void
ClearCfgCurInterleaveStruct(PSYSHOST host, UINT8 sckt, UINT8 ch, UINT8 dimm, UINT8 interleaveIndex)
/**

  This routine clears the interleave information structure in CCUR table
  @param host     - Pointer to sysHost
  @param sckt     - socket number
  @param ch       - channel number
  @param dimm     - dimm number
  @param interleaveIndex     - Index to the Interleave requests

  @retval
**/
{
  UINT8 j;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *InterleavePtr;
  NGN_DIMM_INTERLEAVE_ID_HOST  *InterleaveIdPtr;

  chdimmList = &host-> var.mem.socket[sckt].channelList[ch].dimmList;
  InterleavePtr = &(*chdimmList)[dimm].ngnCfgCur.interleave[interleaveIndex];
  InterleaveIdPtr = (*chdimmList)[dimm].ngnCfgCur.interleaveId[interleaveIndex];

  //clear the data in the struct
  InterleavePtr->InterleaveSetIndex =   0;
  InterleavePtr->NumOfDimmsInInterleaveSet =   0;
  InterleavePtr->InterleaveMemoryType =   0;
  InterleavePtr->InterleaveFormat =   0;
  InterleavePtr->MirrorEnable =   0;
  InterleavePtr->InterleaveChangeResponseStatus =   0;

  //Other variables that are not part of the spec
  InterleavePtr->RecPresent = 0;
  InterleavePtr->Valid = 0;
  InterleavePtr->Processed = 0;
  InterleavePtr->SadLimit = 0;

  //clear the interleave dimm id information.
  for(j=0; j<MAX_CH*MAX_SOCKET; j++){
    InterleaveIdPtr->socket = 0;
    InterleaveIdPtr->channel  = 0;
    InterleaveIdPtr->dimm = 0;
    InterleaveIdPtr->PartitionOffset = 0;
    InterleaveIdPtr->PartitionSize =0;
    InterleaveIdPtr++;
  }//j
  return;
}

UINT32
UpdateCfgCurForNewDimm(PSYSHOST host, UINT8 sckt, UINT8 ch, UINT8 dimm)
/**

  This routine clears the CCUR table and updates response parameters for new dimms.
  @param host     - Pointer to sysHost
  @param skt      - socket Number
  @param ch       - channel Number
  @param dimm     - Dimm Number


  @retval SUCCESS
**/
{
  UINT8  i;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  dimmNvList = &host->nvram.mem.socket[sckt].channelList[ch].dimmList;
  chdimmList = &host->var.mem.socket[sckt].channelList[ch].dimmList;

  (*chdimmList)[dimm].ngnCfgCurPresent = 1;
  (*chdimmList)[dimm].ngnCfgCurValid = 1;

  //Clear other variables.
  // in 1LM mode, VolatileMemSizeIntoSpa = 0; otherwise VolatileMemSizeIntoSpa = volCap
  if (host->var.mem.volMemMode == VOL_MEM_MODE_1LM) {
    (*chdimmList)[dimm].ngnCfgCur.body.VolatileMemSizeIntoSpa = 0;
  } else {
    (*chdimmList)[dimm].ngnCfgCur.body.VolatileMemSizeIntoSpa = (*dimmNvList)[dimm].volCap;
  }

  (*chdimmList)[dimm].ngnCfgCur.body.PersistentMemSizeIntoSpa = 0;
  (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_NEW_DIMM;

  //Update the Interleave records that they are not processed.
  for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
    if ((*chdimmList)[dimm].ngnCfgCur.interleave[i].RecPresent)
       (*chdimmList)[dimm].ngnCfgOut.interleave[i].InterleaveChangeResponseStatus = NGN_INT_CH_STAT_NOT_PROCESSED;
  }

  return SUCCESS;
}
void
UpdateCfgCurAfterMemmap(PSYSHOST host)
/**

  This routine is to update CCUR tables after memory mapping to reflect the updated dimm configuration wherever applicable.
  @param host     - Pointer to sysHost

  @retval
**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT8 i;
  UINT8 j;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *sourceInterleavePtr;
  NGN_DIMM_INTERLEAVE_ID_HOST  *sourceInterleaveIdPtr;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *destInterleavePtr;
  NGN_DIMM_INTERLEAVE_ID_HOST  *destInterleaveIdPtr;
  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;


  //If we are running in degraded memory map level, we will not honor any request from NVMDIMM mgmt driver.
  //So we can skip updating the current config records as they are not altered.
  if(host->var.mem.MemMapDegradeLevel) return;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //If this is a new dimm, clear the CUR record and update the error code as new dimm
        if ((*chdimmList)[dimm].newDimm == 1) {
          UpdateCfgCurForNewDimm(host, sckt, ch, dimm);
          continue;
        }

        if ( ((*chdimmList)[dimm].ngnCfgOutPresent != 0)  && ((*chdimmList)[dimm].ngnCfgOut.body.SequenceNumber == ((*chdimmList)[dimm].ngnCfgReq.body.SequenceNumber)) ) {
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "UpdateCfgCurAfterMemmap: Valid Cout found, ignoring CIN\n"));
          //Update the present and valid variables. This will be used by the DXE code
          (*chdimmList)[dimm].ngnCfgCurPresent = 1;
          (*chdimmList)[dimm].ngnCfgCurValid = 1;
          continue;
        }
        
        //Since this is a new request, the ammount of non-volatile memory mapped into the SPA by default is zero
        (*chdimmList)[dimm].ngnCfgCur.body.PersistentMemSizeIntoSpa = 0;
        //Update CCUR body with size of data mapped into the SPA. For volatile memory, assume that all is mapped into the SPA.
        //For non-volatile memory, need to examine the interleave request to understand the mapping.  (convert from 64MB gran
        // to byte gran).
        // Since this is a new request; in 1LM mode, VolatileMemSizeIntoSpa = 0; otherwise VolatileMemSizeIntoSpa = volCap
        if (host->var.mem.volMemMode == VOL_MEM_MODE_1LM) {
          (*chdimmList)[dimm].ngnCfgCur.body.VolatileMemSizeIntoSpa = 0;
        } else {
          (*chdimmList)[dimm].ngnCfgCur.body.VolatileMemSizeIntoSpa = (*dimmNvList)[dimm].volCap;
        }

        //Check if a CfgIn request is present.  If not skip this dimm
        if ( !(*chdimmList)[dimm].ngnCfgReqPresent ) continue ;

        // Check if a CfgIn request was not valid
        if ( !(*chdimmList)[dimm].ngnCfgReqValid )  {

          if ( (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus == NGN_CFGCUR_RESPONSE_SUCCESS ) {
            (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_CFGIN_ERROR_CCUR_USED;
          }
          else {
            (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_CFGIN_ERROR_DIMM_NOT_MAPPED;
          }
          continue;
        }

        //Update the present and valid variables. This will be used by the DXE code
        (*chdimmList)[dimm].ngnCfgCurPresent = 1;
        (*chdimmList)[dimm].ngnCfgCurValid = 1;

        //Update Partition information of CCUR if needed
        if ((*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent && (*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid ) {

          //check if the partition request was successfully implemented by looking at the response status.
          //If not successful, we can skip updating the CCUR complete;y, because, we will not honor interleave request if partition request fails.
          if ((*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus != NGN_PART_SIZE_STAT_SUCCESS) continue;

          //Update CCUR body with success response code
          (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_SUCCESS ;
        }

        //Update Interleave information of CCUR if needed
        for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
          //Skip if there is no interleave request or if the request failed our validation previously
          if ( !(*chdimmList)[dimm].ngnCfgReq.interleave[i].RecPresent  || !(*chdimmList)[dimm].ngnCfgReq.interleave[i].Valid ) {
            ClearCfgCurInterleaveStruct(host, sckt, ch, dimm, i);
            continue;
          }
          //Skip if this interleave is not processed.
          if ((*chdimmList)[dimm].ngnCfgOut.interleave[i].InterleaveChangeResponseStatus != NGN_INT_CH_STAT_SUCCESS)  {
            //Clear interleave information in the CCUR records because we haven honored an interleave request
            ClearCfgCurInterleaveStruct(host, sckt, ch, dimm, i);
            continue;
          }

          //Update CCUR body with success response code
          (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_SUCCESS ;

          sourceInterleavePtr = &(*chdimmList)[dimm].ngnCfgReq.interleave[i];
          sourceInterleaveIdPtr = (*chdimmList)[dimm].ngnCfgReq.interleaveId[i];
          destInterleavePtr = &(*chdimmList)[dimm].ngnCfgCur.interleave[i];
          destInterleaveIdPtr = (*chdimmList)[dimm].ngnCfgCur.interleaveId[i];

          //The non-volaitle memory contributed by this valid interleave request must be added to get the overall non-volatile memory size mapped into the SPA
          (*chdimmList)[dimm].ngnCfgCur.body.PersistentMemSizeIntoSpa += sourceInterleaveIdPtr->PartitionSize ;

          //Fill in the required data in the host structure
          destInterleavePtr->InterleaveSetIndex =   sourceInterleavePtr->InterleaveSetIndex;
          destInterleavePtr->NumOfDimmsInInterleaveSet =   sourceInterleavePtr->NumOfDimmsInInterleaveSet;
          destInterleavePtr->InterleaveMemoryType =   sourceInterleavePtr->InterleaveMemoryType;
          destInterleavePtr->InterleaveFormat =   sourceInterleavePtr->InterleaveFormat;
          destInterleavePtr->MirrorEnable =   sourceInterleavePtr->MirrorEnable;
          destInterleavePtr->InterleaveChangeResponseStatus =   sourceInterleavePtr->InterleaveChangeResponseStatus;

          //Other variables that are not part of the spec
          destInterleavePtr->RecPresent = sourceInterleavePtr->RecPresent;
          destInterleavePtr->Valid = sourceInterleavePtr->Valid;
          destInterleavePtr->Processed = sourceInterleavePtr->Processed;
          destInterleavePtr->SadLimit = sourceInterleavePtr->SadLimit;

          //Update th interleave dimm id information.
          for(j=0; j<destInterleavePtr->NumOfDimmsInInterleaveSet; j++){
            destInterleaveIdPtr->socket = sourceInterleaveIdPtr->socket;
            destInterleaveIdPtr->channel  = sourceInterleaveIdPtr->channel;
            destInterleaveIdPtr->dimm = sourceInterleaveIdPtr->dimm;
            destInterleaveIdPtr->PartitionOffset = sourceInterleaveIdPtr->PartitionOffset; //This data is in 64MB gran
            destInterleaveIdPtr->PartitionSize =sourceInterleaveIdPtr->PartitionSize;   //This data is in 64MB gran
            destInterleaveIdPtr++;
            sourceInterleaveIdPtr++;
          }//j
        }//i

      }//dimm
    }//ch
  }//sckt
  return;
}

UINT32
CopyInterleaveInformation(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8  interleaveCounter)
/**

  This routine copies interleave request details from CfgIn to CfgOut for a dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param interleaveCounter     -  Count of interleave PCAT tables read.

  @retval SUCCESS
**/
{
  UINT8  j;
  struct dimmDevice (*chdimmList)[MAX_DIMM];

  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *SourceInterleavePtr;
  NGN_DIMM_INTERLEAVE_ID_HOST  *SourceInterleaveIdPtr;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *destInterleavePtr;
  NGN_DIMM_INTERLEAVE_ID_HOST  *destInterleaveIdPtr;
  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;



  SourceInterleavePtr = &(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveCounter];
  SourceInterleaveIdPtr = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveCounter];
  destInterleavePtr = &(*chdimmList)[dimm].ngnCfgOut.interleave[interleaveCounter];
  destInterleaveIdPtr = (*chdimmList)[dimm].ngnCfgOut.interleaveId[interleaveCounter];

  //Fill in the required data in the host structure
  destInterleavePtr->RecPresent =   SourceInterleavePtr->RecPresent;
  destInterleavePtr->Valid =   SourceInterleavePtr->Valid;
  destInterleavePtr->Processed =   SourceInterleavePtr->Processed;
  destInterleavePtr->SadLimit =   SourceInterleavePtr->SadLimit;
  destInterleavePtr->InterleaveSetIndex =   SourceInterleavePtr->InterleaveSetIndex;
  destInterleavePtr->NumOfDimmsInInterleaveSet =   SourceInterleavePtr->NumOfDimmsInInterleaveSet;
  destInterleavePtr->InterleaveMemoryType =   SourceInterleavePtr->InterleaveMemoryType;
  destInterleavePtr->InterleaveFormat =   SourceInterleavePtr->InterleaveFormat;
  destInterleavePtr->MirrorEnable =   SourceInterleavePtr->MirrorEnable;

  //Scan for PCAT table and fill in the host structure.
  for(j=0; j<SourceInterleavePtr->NumOfDimmsInInterleaveSet; j++){

    destInterleaveIdPtr->socket = SourceInterleaveIdPtr->socket;
    destInterleaveIdPtr->channel = SourceInterleaveIdPtr->channel;
    destInterleaveIdPtr->dimm = SourceInterleaveIdPtr->dimm;
    destInterleaveIdPtr->PartitionOffset = SourceInterleaveIdPtr->PartitionOffset ;
    destInterleaveIdPtr->PartitionSize =SourceInterleaveIdPtr->PartitionSize;
    destInterleaveIdPtr++;
    SourceInterleaveIdPtr++;
  }

  return SUCCESS;
}

void
UpdateCfgOutAfterMemmap(PSYSHOST host)
/**

  This routine is to update CfgOut tables after memory mapping to update the validation status.
  It copies the contents of CFGIN requests (Partition and Interleave) into CfgOut and updates the
  response status in the cfgOut structure.
  @param host     - Pointer to sysHost

  @retval
**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT8 i;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];

  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  //If we are running in degraded memory map level, we will not honor any request from NVMDIMM mgmt driver.
  //We have handle this condition already in UpdateRequestforDegradedMemoryMapping
  if(host->var.mem.MemMapDegradeLevel) return;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        //Check if there are CfgIn request is present. If not skip this dimm.
        if ( !(*chdimmList)[dimm].ngnCfgReqPresent )  continue;

        if (((*chdimmList)[dimm].ngnCfgOutPresent != 0) && ((*chdimmList)[dimm].ngnCfgOut.body.SequenceNumber == ((*chdimmList)[dimm].ngnCfgReq.body.SequenceNumber))) {
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "UpdateCfgOutAfterMemmap: Valid Cout found, ignoring CIN\n"));
          continue;
        }

        (*chdimmList)[dimm].ngnCfgOutPresent = 1;
        
        //check if we have a partition req.
        if ((*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent) {
          //Copy CFGIN partition information into CFGOUT struct.
          (*chdimmList)[dimm].ngnCfgOut.partitionSize.RecPresent = (*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent;
          (*chdimmList)[dimm].ngnCfgOut.partitionSize.Valid = (*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid;
          (*chdimmList)[dimm].ngnCfgOut.partitionSize.PartitionSize = (*chdimmList)[dimm].ngnCfgReq.partitionSize.PartitionSize;

          //Note: Response status for the Partition has been updated while performing the paritioning.

          //check if the partition request is found invalid or if the request was failed to be implemented, if so update an error.
          if (( !(*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid) || (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus != NGN_PART_SIZE_STAT_SUCCESS)
            (*chdimmList)[dimm].ngnCfgOut.body.ValidationStatus = NGN_VLD_STAT_ERROR;
        }

        //check if we have an interleave request
        for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
          //Check if there is an interleave request present
          if ((*chdimmList)[dimm].ngnCfgReq.interleave[i].RecPresent) {
            //Update CfgOut with Interleaverequest details.
            CopyInterleaveInformation (host, sckt, ch, dimm, i);

            //Note: Response status for the Interleave request has been updated.

            //check if the partition request is found invalid or if the request was failed to be implemented, if so update an error.
            if ( !(*chdimmList)[dimm].ngnCfgReq.interleave[i].Valid  || ((*chdimmList)[dimm].ngnCfgOut.interleave[i].InterleaveChangeResponseStatus != NGN_INT_CH_STAT_SUCCESS) )
              (*chdimmList)[dimm].ngnCfgOut.body.ValidationStatus = NGN_VLD_STAT_ERROR;
          }
        }//i

      }//dimm
    }//ch
  }//sckt

  return;
}

UINT32
CopyCurrentCfgBody(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, NGN_DIMM_PLATFORM_CFG_CURRENT_BODY  *platformCurCfgBdy)
/**

  This routine is to parses through the buffer containing Current Configuration Body and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param platformCurCfgBdy     -  Pointer to the Platform Current configuration table body buffer

  @retval SUCCESS
**/
{
  struct dimmDevice (*chdimmList)[MAX_DIMM];

  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

  (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus =  platformCurCfgBdy->ConfigurationStatus ;
  (*chdimmList)[dimm].ngnCfgCur.body.VolatileMemSizeIntoSpa = (UINT16) ( platformCurCfgBdy->VolatileMemSizeIntoSpa >> CONVERT_B_TO_64MB );    //Total amount of 2LM size
  (*chdimmList)[dimm].ngnCfgCur.body.PersistentMemSizeIntoSpa = (UINT16) ( platformCurCfgBdy->PersistentMemSizeIntoSpa >> CONVERT_B_TO_64MB );  //Totsl amount of NV size.

  return SUCCESS;
}


UINT32
UpdateRequestforDegradedMemoryMapping(PSYSHOST host)
/**

  This routine is to update failure condition to all the request we have received from the NVMDIMM management software.
  This routine is called during memory mapping due to silicon resource deficit.
  MEM_MAP_LVL_NORMAL - NORMAL memory mapping
  MEM_MAP_LVL_IGNORE_CFGIN   - Will not honor requests from NVMDIMM management software.
  MEM_MAP_LVL_IGNORE_NEW_DIMMS   - MEM_MAP_LVL_IGNORE_CFGIN + Will not mapping volatile region of new dimms added to the system

  @param host     - Pointer to sysHost

  @retval SUCCESS
**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm, interleaveReqId;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];

  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //Skip if this dimm is new to the system
        if ((*chdimmList)[dimm].newDimm == 1) continue;

        //Update the configuration error variable in CCUr with proper error code
        (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_CFGIN_ERROR_CCUR_USED;

        //Check if there are partition request  present and valid for this dimm.
        if ( (*chdimmList)[dimm].ngnCfgReqPresent  && (*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent && (*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid )  {
          //If present update the CFGOUT for this partition request with proper response code
          //Update CfgOut for failure of partion request
          (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus = EncodePartitionResponse(SAD_RULE_UNAVAILABLE);
        }

        //Check if there are Intreleave request  present and valid for this dimm.
        for(interleaveReqId=0; interleaveReqId<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; interleaveReqId++) {
          //Break if the interve info is not valid.
          if(!(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].RecPresent || !(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].Valid) break;

          //Set the Valid bit to 0 for all the interleaves and update the response with proper response status
          (*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].Valid = 0;
          (*chdimmList)[dimm].ngnCfgOut.interleave[interleaveReqId].InterleaveChangeResponseStatus = EncodeInterleaveResponse(SAD_RULE_UNAVAILABLE);
        }
        (*chdimmList)[dimm].ngnCfgOut.body.ValidationStatus = NGN_VLD_STAT_ERROR;

      }//dimm
    }//ch
  }//socket
  return SUCCESS;
}

UINT32
UpdatePartitionRequest(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8  *PCATPtr, UINT8 RecordType)
/**

  This routine is to parses through the buffer containing Partition Size PCAT table and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param PCATPtr     -  Pointer to the PCAT table buffer
  @param RecordType     -  0-CFG_IN record  1- CFG_OUT record.

  @retval SUCCESS
**/
{
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT  *platformPCATPartition;
  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

  platformPCATPartition = (NGN_DIMM_PARTITION_SIZE_CHANGE_PCAT *)((UINT8*)PCATPtr + sizeof(NGN_PCAT_HEADER));
  if (RecordType == CFG_IN){
  (*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent = 1;
  //Update the Partition Size data(Non Vaolatile size in 64MB gran) in the host structure for this partition request
  (*chdimmList)[dimm].ngnCfgReq.partitionSize.PartitionSize = (UINT16) (platformPCATPartition->PartitionSize >> CONVERT_B_TO_64MB);
  } else if (RecordType == CFG_OUT){
  (*chdimmList)[dimm].ngnCfgOut.partitionSize.RecPresent = 1;
  //Update the Partition Size data(Non Vaolatile size in 64MB gran) in the host structure for this partition request
  (*chdimmList)[dimm].ngnCfgOut.partitionSize.PartitionSize = (UINT16) (platformPCATPartition->PartitionSize >> CONVERT_B_TO_64MB);
  }

  return SUCCESS;
}



UINT32
FillDimmLocation(PSYSHOST host, NGN_DIMM_INTERLEAVE_ID_HOST *destInterleaveIdPtr, UINT8 *DimmManufacturerId, UINT8 *DimmSerialNumber, UINT8 *DimmModelNumber)
/**

  This routine scans through the existing dimm information in the host
  NVRAm struct to find a match for the manufacturer id and serial id mentioned
  in the dimm interleave id table and fill the dimm lcoation in the host structs.

  @param host     - Pointer to sysHost
  @param destInterleaveIdPtr   - Pointer to the destination structure.
  @param DimmManufacturerId       - Manufacturer id mentiond in the dimm interleave id table.
  @param DimmSerialNumber     -  Serial number mentioned in the dimm interleave id table.

  @retval SUCCESS
**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct memNvram *nvramMem;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  nvramMem = &host->nvram.mem;

  for(sckt=0; sckt<MAX_SOCKET; sckt++){
    if (nvramMem->socket[sckt].enabled == 0) continue;
    channelNvList = &nvramMem->socket[sckt].channelList;
    for(ch=0; ch<MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if not an NVMDIMM
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        //If the Manufacturer id and serial number match, store the dimm location info. in the host struct.
        if( (!MemCompare ((UINT8 *)DimmManufacturerId, (UINT8 *)(*dimmNvList)[dimm].manufacturer, NGN_MAX_MANUFACTURER_STRLEN))  &&  (!MemCompare ((UINT8 *)DimmSerialNumber, (UINT8 *)(*dimmNvList)[dimm].serialNumber, NGN_MAX_SERIALNUMBER_STRLEN)) &&
             (!MemCompare ((UINT8 *)DimmModelNumber, (UINT8 *)(*dimmNvList) [dimm].modelNumber, NGN_MAX_MODELNUMBER_STRLEN)) ) {
          destInterleaveIdPtr->socket = sckt;
          destInterleaveIdPtr->channel = ch;
          destInterleaveIdPtr->dimm = dimm;
          return SUCCESS;
        }
      }
    }
  }

  //If we are here, we havent found a match for the given manufacturer id and serial number in the system.
  //so load some invalid values to let the validation code know that we didnt find a match.
  destInterleaveIdPtr->socket = 0xFF;
  destInterleaveIdPtr->channel = 0xFF;
  destInterleaveIdPtr->dimm = 0xFF;
  return FAILURE;
}

UINT32
UpdateInterleaveInformation(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8  *PCATPtr, UINT8 RecordType, UINT8 interleaveCounter)
/**

  This routine is to parses through the buffer containing Interleave information PCAT table and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param PCATPtr     -  Pointer to the PCAT table buffer
  @param RecordType     -  0- CFG_CUR record  1-CFG_IN record  2- CFG_OUT record.
  @param interleaveCounter     -  Count of interleave PCAT tables read.

  @retval SUCCESS
**/
{
  UINT8  j;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT  *platformPCATInterleave;
  NGN_DIMM_INTERLEAVE_ID  *platformPCATInterleaveId;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *destInterleavePtr;
  NGN_DIMM_INTERLEAVE_ID_HOST  *destInterleaveIdPtr;
  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

  platformPCATInterleave = (NGN_DIMM_INTERLEAVE_INFORMATION_PCAT *)((UINT8*)PCATPtr + sizeof(NGN_PCAT_HEADER));
  platformPCATInterleaveId = (NGN_DIMM_INTERLEAVE_ID *) ((UINT8*)platformPCATInterleave + sizeof(NGN_DIMM_INTERLEAVE_INFORMATION_PCAT));

  //Initialize the destination pointer to the corrct structure based on the record type.
  if(RecordType == CFG_CUR){
    destInterleavePtr = &(*chdimmList)[dimm].ngnCfgCur.interleave[interleaveCounter];
    destInterleaveIdPtr = (*chdimmList)[dimm].ngnCfgCur.interleaveId[interleaveCounter];
  } else if(RecordType == CFG_IN){
    destInterleavePtr = &(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveCounter];
    destInterleaveIdPtr = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveCounter];
  } else {
    destInterleavePtr = &(*chdimmList)[dimm].ngnCfgOut.interleave[interleaveCounter];
    destInterleaveIdPtr = (*chdimmList)[dimm].ngnCfgOut.interleaveId[interleaveCounter];
  }

  //Set the Interleave record preset bit
  destInterleavePtr->RecPresent = 1;

  //Fill in the required data in the host structure
  destInterleavePtr->InterleaveSetIndex =   platformPCATInterleave->InterleaveSetIndex;
  destInterleavePtr->NumOfDimmsInInterleaveSet =   platformPCATInterleave->NumOfDimmsInInterleaveSet;
  destInterleavePtr->InterleaveMemoryType =   platformPCATInterleave->InterleaveMemoryType;
  destInterleavePtr->InterleaveFormat =   platformPCATInterleave->InterleaveFormat;
  destInterleavePtr->MirrorEnable =   platformPCATInterleave->MirrorEnable;
  destInterleavePtr->InterleaveChangeResponseStatus =   platformPCATInterleave->InterleaveChangeResponseStatus;
  destInterleavePtr->MemorySpare =   platformPCATInterleave->MemorySpare;

  //Scan for PCAT table and fill in the host structure.
  for(j=0; j<platformPCATInterleave->NumOfDimmsInInterleaveSet; j++){
    FillDimmLocation(host, destInterleaveIdPtr, &platformPCATInterleaveId->DimmManufacturerId[0], &platformPCATInterleaveId->DimmSerialNumber[0], &platformPCATInterleaveId->DimmModelNumber[0]);
    destInterleaveIdPtr->PartitionOffset = (UINT16) (platformPCATInterleaveId->PartitionOffset >> CONVERT_B_TO_64MB);
    destInterleaveIdPtr->PartitionSize =(UINT16) (platformPCATInterleaveId->PartitionSize >> CONVERT_B_TO_64MB);
    destInterleaveIdPtr++;
    platformPCATInterleaveId++;
  }

  return SUCCESS;
}


UINT32
HandlePCATs(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8  *PCATPtr, UINT8 RecordType)
/**

  This routine to parses through the buffer looking for PCAT tables and updates the host structures accordingly
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param PCATPtr     -  Pointer to the PCAT table buffer
  @param partition     -  Partition from which the PCAT table is read from.
  @param RecordType     -  0- CFG_CUR record  1-CFG_IN record  2- CFG_OUT record.

  @retval SUCCESS
**/
{
  UINT8  interleaveCounter=0;
  NGN_PCAT_HEADER  *platformPCATHdr;
  struct dimmDevice (*chdimmList)[MAX_DIMM];

  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

  //Read the first PCAT table header
  platformPCATHdr = (NGN_PCAT_HEADER *)PCATPtr;

  //Loop through all the PCAT tables one by one.
  while(platformPCATHdr->Type == NGN_PCAT_TYPE_DIMM_INTERLEAVE_INFORMATION || platformPCATHdr->Type == NGN_PCAT_TYPE_DIMM_PARTITION_SIZE_CHANGE) {
    //Check if this is Interleave Request or Partition Request
    if(  platformPCATHdr->Type == NGN_PCAT_TYPE_DIMM_INTERLEAVE_INFORMATION){
      //If its is an Interleave Request/Info update the Interleave infor/Request struct in the host structure.
      if(interleaveCounter < MAX_UNIQUE_NGN_DIMM_INTERLEAVE)
        UpdateInterleaveInformation(host, socket, ch, dimm, (UINT8 *)platformPCATHdr, RecordType, interleaveCounter);

       //Log a warning and notify NVMDIMM driver if there are more than 2 interleave requests
      else {
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"Configuration Input interleave requests exceed two!\n"));
        OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_MAX_INTERLEAVE_SETS_EXCEEDED, socket, ch, dimm, 0xFF);
        (*chdimmList)[dimm].ngnCfgReqValid = 0;
        (*chdimmList)[dimm].ngnCfgOut.body.ValidationStatus = NGN_VLD_STAT_INTER_REQ_NUM_EXCEED_TWO;
      }
      interleaveCounter++;
    } else if(platformPCATHdr->Type == NGN_PCAT_TYPE_DIMM_PARTITION_SIZE_CHANGE) {
      //If its is an Partition request update  Parition request in the host structure.
      UpdatePartitionRequest(host, socket, ch, dimm, (UINT8 *)platformPCATHdr, RecordType);
    }
    platformPCATHdr = (NGN_PCAT_HEADER *) (((UINT8 *)platformPCATHdr) + platformPCATHdr->Length);
  }
  return SUCCESS;
}


UINT32
ReadCurrentCfgTable(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 CurCfgOffset, UINT32 CurCfgSize)
/**

  This routine to issue getplatformconfigdata command and read the current configuration data from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number


  @retval TRUE - if a new dimm is detected
  @retval FALSE - if it is the same dimm form the previous boot
**/
{
  UINT16  length = BYTES_128;
  UINT8 i, offsetAdjustment, mbStatus = 0;
  UINT32 offset = 0, counter=0,status = SUCCESS, CCurLength;
  UINT8  *PCATPtr, *CurCfgBuffer;
  UINT8 retrieve_option = RETRIEVE_PARTITION_DATA;
  struct fnvCISBuffer fnvBuffer ;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_PLATFORM_CFG_HEADER *platformCurCfgHeader;
  NGN_DIMM_PLATFORM_CFG_CURRENT_BODY *platformCurCfgBdy;
  UINT8 TempCurCfgBuffer[MAX_CUR_CFG_SIZE];
  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

  //Find the offset to the CurCfg table for the platform config body
  //GetPlatformConfigData only supports reads @ 128 byte boundary.
  //So if the offset is not in 128bytes boundary, we will have to adjust it and start at 128 byte boundary.
  offsetAdjustment = CurCfgOffset % BYTES_128;

  //Moving the offste pointer to 128byte boundary
  offset = CurCfgOffset - offsetAdjustment;
  //Increasing the data size to be read base on the adjustment value
  CurCfgSize = CurCfgSize + offsetAdjustment;

  //If the length of the current cfiguration is larger than 128 bytes, Calculate the number of times to read the PCD.
  if(CurCfgSize > BYTES_128) {
    counter =  CurCfgSize /  BYTES_128;
    if (CurCfgSize %  BYTES_128)
      counter ++;
  }

  //Read the complete data in PCD 1 to a temporary buffer to parse PCAT tables.
  for(i=0; i<=counter; i++) {
    MemDebugPrint(( host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
       "ReadCurrentCfgTable->GetPlatformConfigData, partition = %d, option = %d, offset = %d\n", BIOS_PARTITION, retrieve_option, offset ));

    status = GetPlatformConfigData(host, socket, ch, dimm, BIOS_PARTITION, retrieve_option, offset, &fnvBuffer, &mbStatus) ;

    if(status == FAILURE){
      MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
      //log data format:
      //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
      //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
      EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
      DisableChannel(host, socket, ch);
    } else if ((status == WARN_NVMCTRL_MEDIA_NOTREADY) || (status == WARN_NVMCTRL_MEDIA_INERROR) || (status == WARN_NVMCTRL_MEDIA_RESERVED)) {
      MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Problem with Media Access\n"));
      EwlOutputType1 (host, WARN_NVMCTRL_MEDIA_STATUS, (status == WARN_NVMCTRL_MEDIA_NOTREADY? WARN_NVMCTRL_MEDIA_NOTREADY:(status == WARN_NVMCTRL_MEDIA_INERROR? WARN_NVMCTRL_MEDIA_INERROR:WARN_NVMCTRL_MEDIA_RESERVED)), socket, ch, dimm, NO_RANK);
    }

    MemCopy(((UINT8 *)(&TempCurCfgBuffer)) + (i * length) , (UINT8 *)fnvBuffer.platformData, length);
    offset = offset + length;
  }

  //Adjust back the temporary Input buffer based on the offsetAdjustment value. (This is to negate the adjustment we made to read from 128byte boundary earlier.)
  CurCfgBuffer = ((UINT8 *)(&TempCurCfgBuffer)) + offsetAdjustment;
  //Locate and parse the CCUR table and update the host struct with proper values read
  platformCurCfgHeader = (NGN_DIMM_PLATFORM_CFG_HEADER *)CurCfgBuffer;
  CCurLength = platformCurCfgHeader->Length;

  //Check for 'CCUR' signature. Return if the signature doesnt match.
  if (platformCurCfgHeader->Signature != NGN_CURRENT_CONFIGURATION_SIGNATURE) {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "CCUR signature not found in the NGN DIMM Partition %d\n", BIOS_PARTITION));
    return FAILURE;
  }

  //Set the CfgCurPresent variable since we have located a CurCfg record.
  (*chdimmList)[dimm].ngnCfgCurPresent = 1;

  //Check if the checksum passes
  if(ChsumTbl((UINT8 *)CurCfgBuffer, CCurLength)) {
   (*chdimmList)[dimm].ngnCfgCurValid = 0;
   (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_CCUR_CHECKSUM_NOT_VALID;
   MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Current Configuration Record checksum failure.\n"));
   return FAILURE;
  }

  //check if the revision is compatable
  if (platformCurCfgHeader->Revision != NGN_CR_SW_FW_INTERFACE_REVISION)  {
    (*chdimmList)[dimm].ngnCfgCurValid= 0;
    (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_CCUR_REVISION_ERROR;
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "NVMDIMM Mgmt SW - BIOS interface revision mismatch.\n"));
    return FAILURE;
  }

  platformCurCfgBdy = (NGN_DIMM_PLATFORM_CFG_CURRENT_BODY *) ((UINT8 *)CurCfgBuffer + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER));
  //Copy the CUR Config body into the host struct
  CopyCurrentCfgBody(host, socket, ch, dimm, platformCurCfgBdy);

  //DisplayBuffer( host, (UINT8 *)fnvBuffer.platformData, BYTES_128 ) ;

  //Initialize PCAT table pointer and handle PCAT tables.
  PCATPtr =(UINT8 *) CurCfgBuffer + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER) + sizeof(NGN_DIMM_PLATFORM_CFG_CURRENT_BODY);
  HandlePCATs(host, socket, ch, dimm, PCATPtr, CFG_CUR);

  return SUCCESS;
}

UINT32
ReadBIOSPartition (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm)
/**

  This routine to issue getplatformconfigdata command and reads the configuration header data from BIOS partition
  to get the offset of the current configuration data
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number


  @retval TRUE - if a new dimm is detected
  @retval FALSE - if it is the same dimm form the previous boot
**/
{
  UINT8 retrieve_option = RETRIEVE_PARTITION_DATA, mbStatus = 0;
  struct fnvCISBuffer fnvBuffer ;
  NGN_DIMM_PLATFORM_CFG_HEADER  *platformCfgHdr;
  NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *platformCfgBdy;
  UINT32 offset = 0, status = SUCCESS;

  MemDebugPrint(( host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
    "ReadBIOSPartition->GetPlatformConfigData, partition = %d, option = %d, offset = %d\n", BIOS_PARTITION, retrieve_option, offset ));

  status = GetPlatformConfigData(host, socket, ch, dimm, BIOS_PARTITION, retrieve_option, offset, &fnvBuffer, &mbStatus) ;
  if(status == FAILURE){
    MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
    //log data format:
    //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
    //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
    EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
    DisableChannel(host, socket, ch);
  } else if ((status == WARN_NVMCTRL_MEDIA_NOTREADY) || (status == WARN_NVMCTRL_MEDIA_INERROR) || (status == WARN_NVMCTRL_MEDIA_RESERVED)) {
    MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Problem with Media Access\n"));
    EwlOutputType1 (host, WARN_NVMCTRL_MEDIA_STATUS, (status == WARN_NVMCTRL_MEDIA_NOTREADY? WARN_NVMCTRL_MEDIA_NOTREADY:(status == WARN_NVMCTRL_MEDIA_INERROR? WARN_NVMCTRL_MEDIA_INERROR:WARN_NVMCTRL_MEDIA_RESERVED)), socket, ch, dimm, NO_RANK);
  }
  //Initialize pointers to the header and body.
  platformCfgHdr = (NGN_DIMM_PLATFORM_CFG_HEADER *) &fnvBuffer.platformData;

  //Check for 'DMHD' signature. Return if the signature doesnt match.
  if (platformCfgHdr->Signature != NGN_CONFIGURATION_HEADER_SIGNATURE) {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "DMHD signature not found in the NGN DIMM Partition %d\n", BIOS_PARTITION));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return SUCCESS;
  }

  //check if the revision is compatable
  if (platformCfgHdr->Revision != NGN_CR_SW_FW_INTERFACE_REVISION)  {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "NVMDIMM Mgmt SW - BIOS interface revision mismatch.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }
  //Check if the checksum passes
  if(ChsumTbl((UINT8 *)&fnvBuffer.platformData, platformCfgHdr->Length)) {
   MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "BIOS Partition DMHD checksum failure.\n"));
   //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
   return FAILURE;
  }

  platformCfgBdy = (NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *) (((UINT8 *)&fnvBuffer.platformData) + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER));
  status = ReadCurrentCfgTable(host,socket,ch,dimm, platformCfgBdy->CurrentConfStartOffset, platformCfgBdy->CurrentConfDataSize);

  return status;
}


UINT32
ReadCfgInTable(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 ConfRequestDataOffset, UINT32 ConfRequestDataSize)
/**

  This routine to issue getplatformconfigdata command and read the  configuration Input requests from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param ConfRequestDataOffset     -  Offset of CfgIn table in Platform Config Data Region 2.
  @param ConfRequestDataSize     -  Offset of CfgIn table

  @retval SUCCESS
**/
{
  UINT16  length;
  UINT8 i, offsetAdjustment, mbStatus = 0;
  UINT32 offset, counter=0, status = SUCCESS;
  UINT8 *PCATPtr, *CfgInBuffer;
  UINT8 retrieve_option = RETRIEVE_PARTITION_DATA;
  struct fnvCISBuffer fnvBuffer ;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_PLATFORM_CFG_HEADER  *platformCfgInputHeader;
  NGN_DIMM_PLATFORM_CFG_INPUT_BODY  *platformCfgInputBdy;
  UINT8 TempCfgInBuffer[MAX_CFG_IN_SIZE];

  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

  length = BYTES_128;
  //Find the offset to the CfgIn table for the platform config body
  //GetPlatformConfigData only supports reads @ 128 byte boundary.
  //So if the offset is not in 128bytes boundary, we will have to adjust it and start at 128 byte boundary.
  offsetAdjustment = ConfRequestDataOffset % 128;

  //Moving the offste pointer to 128byte boundary
  offset = ConfRequestDataOffset - offsetAdjustment;
  //Increasing the data size to be read base on the adjustment value
  ConfRequestDataSize = ConfRequestDataSize + offsetAdjustment;

  //Read the complete CfgIn table to a temporary buffer
  //If the length of the current configuration is larger than 128 bytes, Calculate the number of times to read the PCD.
  if(ConfRequestDataSize > BYTES_128) {
    counter =  ConfRequestDataSize /  BYTES_128;
    if (ConfRequestDataSize %  BYTES_128)
        counter ++;
  }
  //Read the complete CfgIn data to a temporary buffer to parse PCAT tables.
  for(i=0; i<=counter; i++) {
    MemDebugPrint(( host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
       "ReadCfgInTable->GetPlatformConfigData, partition = %d, option = %d, offset = %d\n", OS_PARTITION, retrieve_option, offset ));

    status = GetPlatformConfigData(host, socket, ch, dimm, OS_PARTITION, retrieve_option, offset, &fnvBuffer, &mbStatus) ;
    if(status == FAILURE){
      MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
      //log data format:
      //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
      //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
      EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
      DisableChannel(host, socket, ch);
    } else if ((status == WARN_NVMCTRL_MEDIA_NOTREADY) || (status == WARN_NVMCTRL_MEDIA_INERROR) || (status == WARN_NVMCTRL_MEDIA_RESERVED)) {
      MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Problem with Media Access\n"));
      EwlOutputType1 (host, WARN_NVMCTRL_MEDIA_STATUS, (status == WARN_NVMCTRL_MEDIA_NOTREADY? WARN_NVMCTRL_MEDIA_NOTREADY:(status == WARN_NVMCTRL_MEDIA_INERROR? WARN_NVMCTRL_MEDIA_INERROR:WARN_NVMCTRL_MEDIA_RESERVED)), socket, ch, dimm, NO_RANK);
    }
    MemCopy(((UINT8 *)(&TempCfgInBuffer)) + (i * length) , (UINT8 *)fnvBuffer.platformData, length);
    offset = offset + length;
  }

  /* platformCfgInputHeader->Length */
  //rcPrintf(( host, "Displaying TempCfgInBuffer\n" ));
  //DisplayBuffer( host, TempCfgInBuffer,  255 );

  //Adjust back the temporary Input buffer based on the offsetAdjustment value. (This is to negate the adjustment we made to read from 128byte boundary earlier.)
   CfgInBuffer = ((UINT8 *)(&TempCfgInBuffer)) + offsetAdjustment;

  //Initialize platform config input header
   platformCfgInputHeader = (NGN_DIMM_PLATFORM_CFG_HEADER *)CfgInBuffer;

  /* platformCfgInputHeader->Length */
  //rcPrintf(( host, "Displaying CfgInBuffer\n" ));
  //DisplayBuffer( host, CfgInBuffer,  255 );

  //Check for 'CIN_' signature. Return if the signature doesn't match.
  if (platformCfgInputHeader->Signature != NGN_CONFIGURATION_INPUT_SIGNATURE) {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "CIN_ signature not found in the NGN DIMM PCD 2.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }

  //Set the CfgReqPresent variable since we have located a CfgIn record.
  (*chdimmList)[dimm].ngnCfgReqPresent = 1;

  //Check if the checksum passes
  if(ChsumTbl((UINT8 *)CfgInBuffer, platformCfgInputHeader->Length)) {
    (*chdimmList)[dimm].ngnCfgReqValid = 0;
    (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_CFGIN_CHECKSUM_ERROR;
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Configuration Input Record checksum failure.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }


  //check if the revision is compatable
  if (platformCfgInputHeader->Revision != NGN_CR_SW_FW_INTERFACE_REVISION)  {
    (*chdimmList)[dimm].ngnCfgReqValid = 0;
    (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFGCUR_RESPONSE_CFGIN_REVISION_ERROR;
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "NVMDIMM Mgmt SW - BIOS interface revision mismatch.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }

  //Initialize platform Config input body
   platformCfgInputBdy = (NGN_DIMM_PLATFORM_CFG_INPUT_BODY *) ((UINT8 *)CfgInBuffer + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER));

  //Fill in the sequence number for this CfgIn record in the host struct
  (*chdimmList)[dimm].ngnCfgReq.body.SequenceNumber =  platformCfgInputBdy->SequenceNumber;

  //Fill in the PCAT table records for this CfgIn record in the host struct
  PCATPtr =(UINT8 *) CfgInBuffer + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER) + sizeof(NGN_DIMM_PLATFORM_CFG_INPUT_BODY);
  HandlePCATs(host, socket, ch, dimm, PCATPtr, CFG_IN);

  return SUCCESS;
}

UINT32
ReadCfgOutTable(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT32 ConfResponseDataOffset, UINT32 ConfResponseDataSize)
/**

  This routine to issue getplatformconfigdata command and read the  configuration Input requests from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param ConfResponseDataOffset     -  Offset of CfgOut table in Platform Config Data Region 2.
  @param ConfResponseDataSize     -  Offset of CfgOut table

  @retval SUCCESS
**/
{
  UINT16  length;
  UINT8 i, *CfgOutBuffer, offsetAdjustment, mbStatus = 0;
  UINT32 offset, counter=0, status = SUCCESS;
  struct fnvCISBuffer fnvBuffer ;
  UINT8 retrieve_option = RETRIEVE_PARTITION_DATA;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_PLATFORM_CFG_HEADER  *platformCfgOutputHeader;
  NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY  *platformCfgOutputBdy;
  UINT8 TempCfgOutBuffer[MAX_CFG_IN_SIZE];

  chdimmList = &host->var.mem.socket[socket].channelList[ch].dimmList;

  length = BYTES_128;
  //Find the offset to the CfgOut table for the platform config body
  //Find the offset to the CfgOut table for the platform config body
  //GetPlatformConfigData only supports reads @ 128 byte boundary.
  //So if the offset is not in 128bytes boundary, we will have to adjust it and start at 128 byte boundary.
  offsetAdjustment = ConfResponseDataOffset % 128;

  //Moving the offste pointer to 128byte boundary
  offset = ConfResponseDataOffset - offsetAdjustment;
  //Increasing the data size to be read base on the adjustment value
  ConfResponseDataSize = ConfResponseDataSize + offsetAdjustment;

  //Read the complete CfgOut table to a temporary buffer
  //If the length of the current cfiguration is larger than 128 bytes, Calculate the number of times to read the PCD.
  if(ConfResponseDataSize > BYTES_128) {
    counter =  ConfResponseDataSize /  BYTES_128;
    if (ConfResponseDataSize %  BYTES_128)
        counter ++;
  }
  //Read the complete CfgOut data to a temporary buffer to parse PCAT tables.
  for(i=0; i<counter; i++) {
    MemDebugPrint(( host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
       "ReadCfgOutTable->GetPlatformConfigData, partition = %d, option = %d, offset = %d\n", OS_PARTITION, retrieve_option, offset ));

    status = GetPlatformConfigData(host, socket, ch, dimm, OS_PARTITION, retrieve_option, offset, &fnvBuffer, &mbStatus) ;
    if(status == FAILURE){
      MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
      //log data format:
      //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
      //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
      EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
      DisableChannel(host, socket, ch);
    }else if ((status == WARN_NVMCTRL_MEDIA_NOTREADY) || (status == WARN_NVMCTRL_MEDIA_INERROR) || (status == WARN_NVMCTRL_MEDIA_RESERVED)) {
      MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Problem with Media Access\n"));
      EwlOutputType1 (host, WARN_NVMCTRL_MEDIA_STATUS, (status == WARN_NVMCTRL_MEDIA_NOTREADY? WARN_NVMCTRL_MEDIA_NOTREADY:(status == WARN_NVMCTRL_MEDIA_INERROR? WARN_NVMCTRL_MEDIA_INERROR:WARN_NVMCTRL_MEDIA_RESERVED)), socket, ch, dimm, NO_RANK);
    }
    MemCopy(((UINT8 *)(&TempCfgOutBuffer)) + (i * length) , (UINT8 *)fnvBuffer.platformData, length);
  offset = offset + length;
  }
  //Adjust back the temporary Input buffer based on the offsetAdjustment value. (This is to negate the adjustment we made to read from 128byte boundary earlier.)
   CfgOutBuffer = ((UINT8 *)(&TempCfgOutBuffer)) + offsetAdjustment;

  //Initialize platform Confug input header
   platformCfgOutputHeader = (NGN_DIMM_PLATFORM_CFG_HEADER *)CfgOutBuffer;

  //Check for 'COUT' signature. Return if the signature doesnt match.
  if (platformCfgOutputHeader->Signature != NGN_CONFIGURATION_OUTPUT_SIGNATURE) {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "COUT signature not found in the NGN DIMM PCD 2.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }

  //Set the CfgReqPresent variable since we have located a CfgIn record.
   (*chdimmList)[dimm].ngnCfgOutPresent = 1;
  //Check if the checksum passes
  if(ChsumTbl((UINT8 *)CfgOutBuffer, platformCfgOutputHeader->Length)) {
    (*chdimmList)[dimm].ngnCfgOutPresent = 0;
    //(*chdimmList)[dimm].ngnErrorBitmap|= NGN_CFG_REQ_CHECKSUM_FAILURE;
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Configuration Output Record checksum failure.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }

  //check if the revision is compatable
  if (platformCfgOutputHeader->Revision != NGN_CR_SW_FW_INTERFACE_REVISION)  {
    (*chdimmList)[dimm].ngnCfgOutPresent = 0;
    //(*chdimmList)[dimm].ngnErrorBitmap|= NGN_CFG_REQ_REVISION_MISMATCH;
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "NVMDIMM Mgmt SW - BIOS interface revision mismatch.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }

  //Initialize platform Confug Output body
   platformCfgOutputBdy = (NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY *) ((UINT8 *)CfgOutBuffer + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER));

  //Fill in the sequence number for this CfgOut record in the host struct
  (*chdimmList)[dimm].ngnCfgOut.body.SequenceNumber =  platformCfgOutputBdy->SequenceNumber;
  (*chdimmList)[dimm].ngnCfgOut.body.ValidationStatus =  platformCfgOutputBdy->ValidationStatus;

  //We dont need to read the PCAT tables like interleave infor and partition size for CFG_OUT records.
  //Fill in the PCAT table records for this CfgOut record in the host struct
  //PCATPtr =(UINT8 *) CfgOutBuffer + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER) + sizeof(NGN_DIMM_PLATFORM_CFG_OUTPUT_BODY);
  //HandlePCATs(host, socket, ch, dimm, PCATPtr, CFG_OUT);

  return SUCCESS;
}


UINT32
ReadOSPartition(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm)
/**

  This routine to issue getplatformconfigdata command and read the  configuration Input requests from the the dimm
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval SUCCESS
**/
{
  UINT32 offset, status = SUCCESS;
  struct fnvCISBuffer fnvBuffer ;
  UINT8 retrieve_option = RETRIEVE_PARTITION_DATA, mbStatus = 0;
  NGN_DIMM_PLATFORM_CFG_HEADER  *platformCfgHdr;
  NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *platformCfgBdy;

  offset = 0;

  MemDebugPrint(( host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
      "ReadOSPartition->GetPlatformConfigData, partition = %d, option = %d, offset = %d\n", OS_PARTITION, retrieve_option, offset ));

  status = GetPlatformConfigData(host, socket, ch, dimm, OS_PARTITION, retrieve_option , offset, &fnvBuffer, &mbStatus) ;
  if(status == FAILURE){
    MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
    //log data format:
    //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
    //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
    EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
    DisableChannel(host, socket, ch);
  } else if ((status == WARN_NVMCTRL_MEDIA_NOTREADY) || (status == WARN_NVMCTRL_MEDIA_INERROR) || (status == WARN_NVMCTRL_MEDIA_RESERVED)) {
    MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Problem with Media Access\n"));
    EwlOutputType1 (host, WARN_NVMCTRL_MEDIA_STATUS, (status == WARN_NVMCTRL_MEDIA_NOTREADY? WARN_NVMCTRL_MEDIA_NOTREADY:(status == WARN_NVMCTRL_MEDIA_INERROR? WARN_NVMCTRL_MEDIA_INERROR:WARN_NVMCTRL_MEDIA_RESERVED)), socket, ch, dimm, NO_RANK);
  }
  //Initialize pointers to the header and body.
  platformCfgHdr = (NGN_DIMM_PLATFORM_CFG_HEADER *) &fnvBuffer.platformData;

  //rcPrintf(( host, "ReadOSPartition\n" ));
  //DisplayBuffer( host, (UINT8 *) platformCfgHdr, 128 ) ;

  //Check for 'DMHD' signature. Return if the signature doesnt match.
  if (platformCfgHdr->Signature != NGN_CONFIGURATION_HEADER_SIGNATURE) {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "DMHD signature not found in the NGN DIMM Partition %d\n", OS_PARTITION));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return SUCCESS;
  }

  //check if the revision is compatable
  if (platformCfgHdr->Revision != NGN_CR_SW_FW_INTERFACE_REVISION)  {
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "NVMDIMM Mgmt SW - BIOS interface revision mismatch.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return FAILURE;
  }

  //Check if the checksum passes
  if(ChsumTbl((UINT8 *)&fnvBuffer.platformData, platformCfgHdr->Length)) {
   MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "OS Partition DMHD checksum failure.\n"));
   //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
   return FAILURE;
  }

  platformCfgBdy = (NGN_DIMM_PLATFORM_CFG_DESCRIPTION_BODY *) (((UINT8 *)&fnvBuffer.platformData) + sizeof(NGN_DIMM_PLATFORM_CFG_HEADER));
  if(platformCfgBdy->ConfRequestDataSize > 0) {
    status = ReadCfgInTable(host, socket, ch, dimm, platformCfgBdy->ConfRequestDataOffset, platformCfgBdy->ConfRequestDataSize);
    if(status == FAILURE){
      return FAILURE;
    }
  }

  if(platformCfgBdy->ConfResponseDataSize > 0) {
    status = ReadCfgOutTable(host, socket, ch, dimm, platformCfgBdy->ConfResponseDataOffset, platformCfgBdy->ConfResponseDataSize);
    if(status == FAILURE){
      return FAILURE;
    }
  }

  return status;
}
#ifndef MINIBIOS_BUILD
BOOLEAN
IsNewDimm(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm,   struct prevBootNGNDimmCfg  *prevBootNGNDimmCfg)

/**

  This routine compare the manufacturer id and serial number of the current
  dimm in the slot with the dimm information stored in the NVRAM from previous boot.

  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param *prevBootNGNDimmCfg     -  Pointer to the struct that stores the NGN dimm cfg from the previous boot


  @retval TRUE - if a new dimm is detected
  @retval FALSE - if it is the same dimm form the previous boot


**/
{
  UINT8 i;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = &host->nvram.mem.socket[socket].channelList[ch].dimmList;

  for (i = 0; i < NGN_MAX_MANUFACTURER_STRLEN; i++) {
     if((*dimmNvList)[dimm].manufacturer[i] != prevBootNGNDimmCfg->socket[socket].channelList[ch].dimmList[dimm].manufacturer[i]) {
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "ManufacturerID Does Not Match!!!\n"));
      return TRUE;
     }
  }
  for (i = 0; i < NGN_MAX_SERIALNUMBER_STRLEN; i++) {
    if((*dimmNvList)[dimm].serialNumber[i] != prevBootNGNDimmCfg->socket[socket].channelList[ch].dimmList[dimm].serialNumber[i]) {
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "SerialNumber Does Not Match!!!\n"));
      return TRUE;
    }
  }
  for (i = 0; i < NGN_MAX_MODELNUMBER_STRLEN; i++) {
    if((*dimmNvList)[dimm].modelNumber[i] != prevBootNGNDimmCfg->socket[socket].channelList[ch].dimmList[dimm].modelNumber[i]) {
      MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "ModelNumber Does Not Match!!!\n"));
      return TRUE;
    }
  }
  return FALSE;

}
#endif

UINT32
CheckForIncompatibleDimms(PSYSHOST host)
/**

  This routine is to compare the SKU information of each dimm to see if we have mismatched dimm SKUs  in the system.
  We currently do not support dimms of different SKU.
  @param host     - Pointer to sysHost


  @retval SUCCESS - if the SKU information of all the NVMDIMMs in the system match.
  @retval FAILURE - if the SKU information of all the NVMDIMMs in the system donot match
**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT8 dimmSku = 0;
  struct memNvram *nvramMem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  nvramMem = &host->nvram.mem;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;
    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        //Scan through each dimm to see if the dimmSku information matched
        if (dimmSku == 0){
          //store DimmSku information - Exclude Bit4 - SoftPorgramableSKU. This is not used.
          dimmSku = (*dimmNvList)[dimm].dimmSku & ~BIT4;
        }
        else if (dimmSku != ((*dimmNvList)[dimm].dimmSku  & ~BIT4)) {
          MemDebugPrint ((host, 0xFFFF, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "DIMM SKU mismatch.\n"));
          if (host->setup.mem.dfxMemSetup.dfxOptions & CR_MIXED_SKU) {
#if SMCPKG_SUPPORT
            OutputError (host, ERR_NGN, NGN_SKU_MISMATCH, sckt, ch, dimm, 0xFF);
#else
            FatalError (host, ERR_NGN, NGN_SKU_MISMATCH);
#endif
          } else {
            EwlOutputType1 (host, WARN_DIMM_COMPAT, WARN_DIMM_SKU_MISMATCH, sckt, ch, dimm, NO_RANK);
          }
          return FAILURE;
        }
      } // dimm loop
    } // ch loop
  } // sckt loop
  return SUCCESS;

}




UINT32
GetDimmDeviceData(PSYSHOST host)
/**

  This routine is to issue identifydim and getplatformconfigdata command and read the current configuration data from all the NGN dimms in the system.
  @param host     - Pointer to sysHost


  @retval SUCCESS - if NGN DIMM PCD is read and the host struct is populated.
  @retval FAILURE - if dimm SKUs mismatch
**/
{

  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT8  i;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct dimmInformation dimmInfoList ={0};
  struct fnvCISBuffer fnvBuffer ;
  struct dimmPartition (*dimmPartitionList)[MAX_SOCKET][MAX_CH][MAX_DIMM];
#ifndef MINIBIOS_BUILD
#ifdef __GNUC__
  struct prevBootNGNDimmCfg  prevBootNGNDimmCfg = {};
#else
  struct prevBootNGNDimmCfg  prevBootNGNDimmCfg = {0};
#endif //__GNUC__
#endif
  UINT32 status = SUCCESS ;
#ifndef MINIBIOS_BUILD
  UINT32 prevBootNGNDimmCfgStatus = SUCCESS;
#endif
  UINT8  mbStatus = 0;
  UINT64_STRUCT tempData;

  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  //We donot want to run the below code in minibios environment
#ifndef MINIBIOS_BUILD
  //Update the prevBootNGNDimmCfg structure with data from previous boot
  prevBootNGNDimmCfgStatus = UpdatePrevBootNGNDimmCfg(host, &prevBootNGNDimmCfg);
  MemDebugPrint(( host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
     "Reading NVRAM to get NGN dimm data from previous boot: Status: %d\n", prevBootNGNDimmCfgStatus));
#endif
  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;
    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //Skip if this DIMM is disabled
        rankList = &nvramMem->socket[sckt].channelList[ch].dimmList[dimm].rankList;
        if((*dimmNvList)[dimm].aepDimmPresent) {
          if ((*dimmNvList)[dimm].mapOut[0] || (*rankList)[0].enabled == 0)  continue;
        }

        //1. IDENTIFY_DIMM
        //Issue identify dimm to the NVMDIMM to get the raw capacity, Manufacturer id, serial number, and model number.
        status = IdentifyDimm(host, sckt, ch, dimm, &dimmInfoList, &mbStatus);
        if (status == SUCCESS) {
          //Update dimm identification information in the NVRAM struct
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Manufacturer: " ));
          for (i = 0 ; i < NGN_MAX_MANUFACTURER_STRLEN ; i++ ) {
          (*dimmNvList)[dimm].manufacturer[i] = dimmInfoList.mf[i];
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%02X ", (*dimmNvList)[dimm].manufacturer[i]));
          }
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "SerialNumber: " ));
          for (i = 0 ; i < NGN_MAX_SERIALNUMBER_STRLEN ; i++ ) {
            (*dimmNvList)[dimm].serialNumber[i] = dimmInfoList.sn[i];
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%02X ", (*dimmNvList)[dimm].serialNumber[i]));
          }
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "ModellNumber: " ));
          for (i = 0 ; i < NGN_MAX_MODELNUMBER_STRLEN ; i++ ) {
            (*dimmNvList)[dimm].modelNumber[i] = dimmInfoList.mn[i];
            MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "%02X ", (*dimmNvList)[dimm].modelNumber[i]));
          }
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\n"));
          (*dimmNvList)[dimm].InterfaceFormatCode = dimmInfoList.ifc;
          (*dimmNvList)[dimm].dimmSku = dimmInfoList.dimmSku;
          MemDebugPrint(( host,SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"dimmSku: 0x%02X \n", (*dimmNvList)[dimm].dimmSku));
          (*dimmNvList)[dimm].DeviceID = dimmInfoList.deviceID;
          MemDebugPrint(( host,SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"NVM Controller: %s\n", ((*dimmNvList)[dimm].DeviceID == FNV ? "FNV": ((*dimmNvList)[dimm].DeviceID == EKV? "EKV":((*dimmNvList)[dimm].DeviceID == BWV ? "BWV": "???")))));
#ifndef MINIBIOS_BUILD
          //If there is previous boot information, determine if this DIMM should be considered new
          if( prevBootNGNDimmCfgStatus == SUCCESS )  {
            (*chdimmList)[dimm].newDimm = IsNewDimm(host, sckt, ch, dimm, &prevBootNGNDimmCfg);
          }
          else  {
            (*chdimmList)[dimm].newDimm = TRUE ;
          }
          MemDebugPrint(( host,SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"newDimm: %d \n", (*chdimmList)[dimm].newDimm));
#else
#ifndef MEMMAPSIM_BUILD
          //In minibios environment assume all NGN dimms are new since we cannot reboot in this environment
          (*chdimmList)[dimm].newDimm = 1;
#else
          //In memmap simulator environment assume all NGN dimms are not new for easy debugging
          (*chdimmList)[dimm].newDimm = 0;
#endif
#endif
          //Update raw capacity of the dimm, convert from 4kb granularity to 64MB granularity
          (*dimmNvList)[dimm].rawCap = (UINT16)(dimmInfoList.rc >>CONVERT_4KB_TO_64MB_GRAN);
          MemDebugPrint ((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
              "DIMM info RawCap:%d(64MB gran)  %d(4kb gran)\n",(*dimmNvList)[dimm].rawCap, dimmInfoList.rc));
        } else {
            MemDebugPrint((host, SDBG_MINMAX, sckt,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
            //log data format:
            //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
            //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
            EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, sckt, ch, dimm, NO_RANK);
            DisableChannel(host, sckt, ch);
        }
      }//dimm
    }//  ch
  } //sckt
  //Check for dimms with incompatible SKUs and if the dimms are not compatible donot map the volatile/persistent region.
  status = CheckForIncompatibleDimms(host);
  if(status != SUCCESS)
    return status;


  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;
    channelNvList = &nvramMem->socket[sckt].channelList;

    // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //If we are running in degraded memory mapping mode (MEM_MAP_LVL_IGNORE_CFGIN_IGNORE_NEW_DIMMS) and this is a new dimm,
        //skip updating the size of the new dimms in the host struct. This will make sure they are not mapped.
        if( (host->var.mem.MemMapDegradeLevel >= MEM_MAP_LVL_IGNORE_CFGIN_IGNORE_NEW_DIMMS) && ( (*chdimmList)[dimm].newDimm) ) continue;

        //2.GET_DIMM_PARTITION_INFO
        //call GetDimmPartitionInfo to get the volCap, nonVolCap and their DPAs.
        status = GetDimmPartitionInfo(host, sckt, ch, dimm, &fnvBuffer, &mbStatus);
        if (status == SUCCESS) {
          dimmPartitionList = GetFnvCisBufferDimmPartition(&fnvBuffer);
          (*dimmNvList)[dimm].volCap = (UINT16)((*dimmPartitionList)[sckt][ch][dimm].volatileCap  >> CONVERT_4KB_TO_64MB_GRAN );
          (*dimmNvList)[dimm].nonVolCap = (UINT16)((*dimmPartitionList)[sckt][ch][dimm].persistentCap  >> CONVERT_4KB_TO_64MB_GRAN );
          //Start addresses are in 64byte granularity
          tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].volatileStart, CONVERT_B_TO_64MB);
          (*dimmNvList)[dimm].volRegionDPA = tempData.lo;
          tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].persistentStart, CONVERT_B_TO_64MB);
          (*dimmNvList)[dimm].perRegionDPA = tempData.lo;
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
             "VolDPA: %d   PerDPA: %d \n",(*dimmNvList)[dimm].volRegionDPA, (*dimmNvList)[dimm].perRegionDPA));
        }  else  {
          MemDebugPrint((host, SDBG_MINMAX, sckt,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
          //log data format:
          //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
          //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
          EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, sckt, ch, dimm, NO_RANK);
          DisableChannel(host, sckt, ch);
        }
      }//dimm
    }//ch
  } //sckt

  return SUCCESS;
}

/**

Routine Description: SetDieSparingPolicy calls SetDieSparing per dimm.

  @param host           - Pointer to the system host (root) structure

  @retval 0 SUCCESS
  @retval 1 FAILURE

**/
UINT32
SetAepDieSparing (
                  PSYSHOST       host,
                  UINT8          socket
)
{
  UINT8  ch;
  UINT8  dimm;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];
  UINT8  Enable = host->setup.mem.DieSparing;
  UINT8  Aggressiveness = host->setup.mem.DieSparingAggressivenessLevel;
  UINT32 status = SUCCESS;
  UINT8 mbStatus = 0;

  channelNvList = GetChannelNvList(host, socket);
  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    // Get the dimmNvList to see if any of them are NVMDIMMs
    dimmNvList = GetDimmNvList(host, socket, ch);
    for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
      // Found an AepDimm in the channel, set die sparing on this dimm if it's Die Sparing Capable
      if ((*dimmNvList)[dimm].dimmSku & BIT3) {
        status = SetDieSparingPolicy(host, socket, ch, dimm, Enable, Aggressiveness, &mbStatus);
        if (status == FAILURE) {
          MemDebugPrint((host, SDBG_MINMAX, socket,ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
          //log data format:
          //Byte 0 (Bit 0 - DIMM Slot Number, Bits 1-3 - Channel Number, Bits 4-6 - Socket Number,Bit 7 - Reserved)
          //Byte 1 - FW MB Opcode,Byte 2 - FW MB Sub-Opcode,Byte 3 - FW MB Status Code
          EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, socket, ch, dimm, NO_RANK);
          DisableChannel(host, socket, ch);
        }
        return status;
      }
    }
  }
  return SUCCESS;
}

UINT32
InitializeNGNDIMM(PSYSHOST host)
/**

  This routine initializes the memory size fields in the structures
  for all the NGN dimms in the system

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT32 Status;
  struct memNvram *nvramMem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];

  nvramMem = &host->nvram.mem;

  //Fill-in the dimm information returned from IdentifyDimm and GetDimmPartitionInfo commands.
  Status = GetDimmDeviceData(host);
  //If there is Failure because of SKU mismatch donot read PCDs or honor any interleaves.
  if(Status != FAILURE) {
    //If we have already calculated the resource requirements, return back.
    if(host->var.mem.MemMapState != MEM_MAP_STATE_RESOURCE_CALCULATION) return SUCCESS;
    // For each socket
    for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
      if (host->nvram.mem.socket[sckt].enabled == 0) continue;

      channelNvList = &nvramMem->socket[sckt].channelList;

     // For each channel
      for (ch = 0; ch < MAX_CH; ch++) {
        if ((*channelNvList)[ch].enabled == 0) continue;
        dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;

        // For each DIMM
        for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
          //Skip if this is not a NVMDIMM dimm
          if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
          rankList = &nvramMem->socket[sckt].channelList[ch].dimmList[dimm].rankList;
          if ((*dimmNvList)[dimm].mapOut[0] || (*rankList)[0].enabled == 0) {
             MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"\tDimm is disabled!!! PCD is not read!\n"));
             continue;
          }
          //Read the PCD partition 1, to get the Current configuration.
          Status = ReadBIOSPartition(host, sckt, ch, dimm);
          if(Status == FAILURE)
            continue;

          //Read the PCD partiion 2, to get the Cfgin & CfgOut records.
          ReadOSPartition(host, sckt, ch, dimm);

        }//dimm
      }//ch
    }//socket

    //Init variables for blk window size
    InitBlkCtrlRegion(host, MEM_TYPE_BLK_WINDOW);

#ifndef MEMMAPSIM_BUILD
    for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
        if (host->nvram.mem.socket[sckt].enabled == 0) continue;
        //
        // Set NVMDIMM die sparing
        //
      SetAepDieSparing (host, sckt);
    }
#endif
  } //Status=Failure(dimmSKU mismatch)

  //Init variables for controil region window size
  InitBlkCtrlRegion(host, MEM_TYPE_CTRL);

  return SUCCESS;
}


UINT8
ValidatePartitionRequest(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm)
/**

  This routine validates  the Partition request records read from PCD1
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval PARTITION_TOO_BIG - if the partition size is greater than dimm size.
  @retval VALIDATION_SUCCESS - If validation succeeds
**/
{
  struct memVar *mem;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct memNvram *nvramMem;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  nvramMem = &host->nvram.mem;
  dimmNvList = &nvramMem->socket[socket].channelList[ch].dimmList;
  mem = &host->var.mem;
  chdimmList = &mem->socket[socket].channelList[ch].dimmList;

  if((*chdimmList)[dimm].ngnCfgReq.partitionSize.PartitionSize > (*dimmNvList)[dimm].rawCap)
    return PARTITION_TOO_BIG;
  return VALIDATION_SUCCESS;
}

UINT32
CompareDimmInterleaveId(PSYSHOST host, NGN_DIMM_INTERLEAVE_ID_HOST *sourceInterleaveId, NGN_DIMM_INTERLEAVE_ID_HOST *destInterleaveId, UINT8 NumOfDimmsInInterleaveSet)
/**

  This routine validates  a specific Interleave request read from PCD1
  @param host     - Pointer to sysHost
  @param sourceInterleaveId   - Pointer to the InterleaveId array of the source
  @param destInterleaveId   - Pointer to the InterleaveId array of the destination
  @param NumOfDimmsInInterleaveSet     -  Number of Dimms in the interleave

  @retval status
**/

{
  UINT8 i ;

  NGN_DIMM_INTERLEAVE_ID_HOST *tempSourceInterleaveId, *tempDestInterleaveId ;


  tempSourceInterleaveId = sourceInterleaveId ;
  tempDestInterleaveId = destInterleaveId ;

  //Loop through the source InterleaveDimm set
  for(i=0; i<NumOfDimmsInInterleaveSet; i++){
    if (MemCompare((UINT8*)tempSourceInterleaveId, (UINT8*)tempDestInterleaveId, sizeof(NGN_DIMM_INTERLEAVE_ID_HOST))) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Ids don't match\n"));
      return FAILURE ;
    }
    tempSourceInterleaveId++;
    tempDestInterleaveId++;

  } //i
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Matching dimm ID\n"));
  return SUCCESS;
}

UINT8
ValidateInterleaveRequest(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 interleaveReqId, UINT8 recordType)
/**

  This routine validates  a specific Interleave request read from PCD1
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number
  @param interleaveReqId     -  Interleave request id
  @param recordType     -  Type of the record validated CFG_CUR/CFG_IN

  @retval status
**/
{
  UINT8 NumOfDimmsInInterleaveSet=0, i, j, socketNum,perviousDimmSktNum, channelNum, dimmNum, granularity = 0xff, memType ;
  UINT32  status = FAILURE;
  struct memVar *mem;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct memNvram *nvramMem;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *sourceInterleave=NULL, *destInterleave=NULL;
  NGN_DIMM_INTERLEAVE_ID_HOST  *sourceInterleaveId=NULL, *destInterleaveId=NULL;

  socketNum=0;
  perviousDimmSktNum = 0;
  channelNum=0;
  dimmNum=0;

  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;
  chdimmList = &mem->socket[socket].channelList[ch].dimmList;

   //Find the memory type based on perMemMode
  if(host->var.mem.dfxMemVars.dfxPerMemMode == PER_MEM_CACHE_MODE)
    memType = MEM_TYPE_PMEM_CACHE;
  else
    memType = MEM_TYPE_PMEM;

  if(recordType == CFG_IN) {
    granularity = EncodeInterleaveGranularity( host, memType, (*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].InterleaveFormat );
  } else if(recordType == CFG_CUR)  {
    granularity = EncodeInterleaveGranularity( host, memType, (*chdimmList)[dimm].ngnCfgCur.interleave[interleaveReqId].InterleaveFormat );
  }
  if( granularity == 0xff )  {
    return INTERLEAVE_FORMAT_NOT_VALID ;
  }

  if(recordType == CFG_IN) {
    NumOfDimmsInInterleaveSet = (*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].NumOfDimmsInInterleaveSet;
    sourceInterleave = &(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId];
  } else if(recordType == CFG_CUR) {
    NumOfDimmsInInterleaveSet = (*chdimmList)[dimm].ngnCfgCur.interleave[interleaveReqId].NumOfDimmsInInterleaveSet;
    sourceInterleave = &(*chdimmList)[dimm].ngnCfgCur.interleave[interleaveReqId];
  }

  for(i=0; i<NumOfDimmsInInterleaveSet; i++)
  {
    //Init temp variables based on dimm id struct
    if(recordType == CFG_IN) {
      socketNum = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][i].socket;
      channelNum = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][i].channel;
      dimmNum = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][i].dimm;
      sourceInterleaveId = &(*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][0];
    } else if(recordType == CFG_CUR) {
      socketNum = (*chdimmList)[dimm].ngnCfgCur.interleaveId[interleaveReqId][i].socket;
      channelNum = (*chdimmList)[dimm].ngnCfgCur.interleaveId[interleaveReqId][i].channel;
      dimmNum = (*chdimmList)[dimm].ngnCfgCur.interleaveId[interleaveReqId][i].dimm;
      sourceInterleaveId = &(*chdimmList)[dimm].ngnCfgCur.interleaveId[interleaveReqId][0];
    }

    //If the interleave set is one way, return success here.
    if ((NumOfDimmsInInterleaveSet ==1) && (socketNum == socket) &&(channelNum == ch) && (dimmNum == dimm) )
      return VALIDATION_SUCCESS;

    if (i == 0)
      perviousDimmSktNum = socketNum;
    else
      perviousDimmSktNum = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][i - 1].socket;

    //Check if MirrorEnable is cleared and the interleave set is with-in the current socket
    //skip if MirrorEnable is set and the interleave set spans across multiple sockets
    if( (sourceInterleave->MirrorEnable == 1) && (socketNum != perviousDimmSktNum) )
      return MIRROR_MAPPING_FAILED;

    //Check if the partition size contributed by this DIMM is aligned  to the Interleave Alignment Size
    //of the interleave capability information
    // skip if not aligned
    if ( (sourceInterleaveId->PartitionSize % ((BIT0 << INTERLEAVE_ALIGNMENT_SIZE) >> CONVERT_B_TO_64MB) ) != 0 )
      return INTERLEAVE_NOT_ALIGNED;

    //1. Check if the dimms in the interleave list are present in the system.
    //Skip if there if we have invalid socket, ch, dimm number
    if(socketNum == 0xff || channelNum == 0xff || dimmNum == 0xff)
      return INTERLEAVE_PARTICIPANT_UNAVAILABLE;

    //Skip if there is no NVMDIMM in the specified location.
    if(nvramMem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].aepDimmPresent ==  0)
      return INTERLEAVE_PARTICIPANT_UNAVAILABLE;

    //2. Check if the participating dimms also have similar interleave request.

    //Check if the participating dimm has an CfgIn table present.
    if( (recordType == CFG_CUR) && (mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgCurPresent == 0) )
        return MISSING_CFG_REQUEST;
    else if( (recordType == CFG_IN) && (mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReqPresent == 0))
        return MISSING_CFG_REQUEST;

    //Scan through the interleave requests of the participating dimm to find a matching request.
    for(j=0; j<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; j++) {
      if(recordType == CFG_IN) {
        destInterleave = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReq.interleave[j]);
        destInterleaveId = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReq.interleaveId[j][0]);
      } else if(recordType == CFG_CUR) {
        destInterleave = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgCur.interleave[j]);
        destInterleaveId = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgCur.interleaveId[j][0]);
      }

      //skip if there is no interleave record present
      if (destInterleave->RecPresent == 0)
        continue;

      //skip if basic Interleave information doesnt match
      if ((destInterleave->NumOfDimmsInInterleaveSet != sourceInterleave->NumOfDimmsInInterleaveSet)
          || (destInterleave->InterleaveMemoryType != sourceInterleave->InterleaveMemoryType)
          || (destInterleave->InterleaveFormat != sourceInterleave->InterleaveFormat)
          || (destInterleave->MirrorEnable != sourceInterleave->MirrorEnable))
        continue;

    //Compare the list of dimms participating in the interleave and see if they match
      status = CompareDimmInterleaveId(host, sourceInterleaveId, destInterleaveId, NumOfDimmsInInterleaveSet);
      if (status == SUCCESS)
        break;
    }
  //If we cannot find a matching interleave request, return error.
    if(status != SUCCESS)
      return INTERLEAVE_PARAMETER_MISMATCH;
  }

  return VALIDATION_SUCCESS;
}

UINT32
ValidateCfgRecords(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, UINT8 recordType)
/**

  This routine validates  the specific record read from PCD1 and stores the validation result in host struct
  @param host     - Pointer to sysHost
  @param socket   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval SUCCESS - Validation of the record successful
  @retval FAILURE - Validation of the record failed

**/
{
  UINT8 status, i;
  struct memVar *mem;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  BOOLEAN recPresent =0, *recValid = NULL;
  BOOLEAN validationFailureFlag = 0;
  mem = &host->var.mem;
  chdimmList = &mem->socket[socket].channelList[ch].dimmList;

  //Start updating the CfgOut records with proper status if validation fails.
  if(recordType == CFG_IN)  {
    (*chdimmList)[dimm].ngnCfgOut.body.SequenceNumber =  (*chdimmList)[dimm].ngnCfgReq.body.SequenceNumber;
    //Init validation status to 0 (Success), the failure conditions will be updated as necessary.
    (*chdimmList)[dimm].ngnCfgOut.body.ValidationStatus =  NGN_VLD_STAT_SUCCESS;
  }
  //Partition table is valid only for CFG_IN
  if(recordType == CFG_IN) {
    //Validate the Partition request.
    if( (*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent == 1) {

      //Update the CfgOut record
      (*chdimmList)[dimm].ngnCfgOut.partitionSize.RecPresent = 1;

      status = ValidatePartitionRequest (host, socket, ch, dimm);
      if(status != VALIDATION_SUCCESS) {
        validationFailureFlag = 1;
        (*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid = 0;
        (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus = EncodePartitionResponse(status);
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Partition Request Validation FAILED. - Partition requested too big.\n"));
      }
      else
        (*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid = 1;
    }//partitionSize.RecPresent
  }//CFG_IN

  //Validate the Interleave request one by one.
  for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
    if(recordType == CFG_IN) {
      recPresent = (*chdimmList)[dimm].ngnCfgReq.interleave[i].RecPresent;
      recValid = & ((*chdimmList)[dimm].ngnCfgReq.interleave[i].Valid);
      if(recPresent)
        //Update the CfgOut record
        (*chdimmList)[dimm].ngnCfgOut.interleave[i].RecPresent = 1;
    } else if (recordType == CFG_CUR) {
      recPresent = (*chdimmList)[dimm].ngnCfgCur.interleave[i].RecPresent;
      recValid = & ((*chdimmList)[dimm].ngnCfgCur.interleave[i].Valid);
    }
    if(recPresent == 1) {
      status = ValidateInterleaveRequest (host, socket, ch, dimm, i, recordType);
      if(status != VALIDATION_SUCCESS){

        *recValid = 0;
        validationFailureFlag = 1;
        MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Interleave Info/Request Validation FAILED. ERROR Code: %d \n", status));

        if(recordType == CFG_IN) {
          // Update the CgfOut record
          (*chdimmList)[dimm].ngnCfgOut.interleave[i].InterleaveChangeResponseStatus = EncodeInterleaveResponse(status);

          // If there is a Partition Size Change request, then update it so it is not applied to the system
          if( ((*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent == 1) && ((*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid == 1 ))  {
            (*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid = 0;

            if( status == INTERLEAVE_FORMAT_NOT_VALID)  {
              (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus = EncodePartitionResponse(NOT_PROCESSED);
              MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Partition Request Validation FAILED. ERROR Code: %d \n", NOT_PROCESSED));
            }
            else if (status == INTERLEAVE_PARTICIPANT_UNAVAILABLE) {
              (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus = EncodePartitionResponse(INTERLEAVE_PARTICIPANT_UNAVAILABLE);
               MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Partition Request Validation FAILED. ERROR Code: %d \n", INTERLEAVE_PARTICIPANT_UNAVAILABLE));
            }
            else if (status == INTERLEAVE_PARAMETER_MISMATCH) {
              (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus = EncodePartitionResponse(INTERLEAVE_PARAMETER_MISMATCH);
               MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Partition Request Validation FAILED. ERROR Code: %d \n", INTERLEAVE_PARAMETER_MISMATCH));
            }
          }
        } else if (recordType == CFG_CUR) {

          if ( status == INTERLEAVE_PARTICIPANT_UNAVAILABLE )
            (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFG_CUR_RESPONSE_ALL_DIMM_NOT_FOUND;
          else if ( status == INTERLEAVE_PARAMETER_MISMATCH)
            (*chdimmList)[dimm].ngnCfgCur.body.ConfigurationStatus = NGN_CFG_CUR_RESPONSE_MATCHING_INTERLEAVE_NOT_FOUND;

        }

      }
      else
        *recValid = 1;
    }//recordPresent
  }//for

  if(validationFailureFlag != 1)
    return SUCCESS;
  else
    return FAILURE;
}



UINT32
ValidateNGNDimmData(PSYSHOST host)
/**

  This routine goes through bothe CfgCur and CfgIn and validates the current config and config requests.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT32 status;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];

  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        rankList = &nvramMem->socket[sckt].channelList[ch].dimmList[dimm].rankList;
        if ((*dimmNvList)[dimm].mapOut[0] || (*rankList)[0].enabled == 0) {
           MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"\tDimm is disabled!!! CfgCur is not validated!\n"));
           continue;
        }

        //Initialize NGN dimm by populating the necessary fields in host struct
        //Skip if this dimm is new to the system
        if ((*chdimmList)[dimm].newDimm == 1) continue;

        //skip if there are no CfgIn records for this dimm or if the request is found invalid in previous checks
        //if ((*chdimmList)[dimm].ngnCfgReqPresent == 0) {
          //If we donot have a CfgIn record, check if there is a CfgCur record that is read.
          if((*chdimmList)[dimm].ngnCfgCurPresent == 1)  {
            //If found, validate the CfgCur record.
            status = ValidateCfgRecords(host, sckt, ch, dimm, CFG_CUR);
            if(status != SUCCESS) {
              (*chdimmList)[dimm].ngnCfgCurValid = 0;
              MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "CfgCur Validation FAILED. \n"));
            } else {
              (*chdimmList)[dimm].ngnCfgCurValid = 1;
              MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "CfgCur Validation SUCCESSFUL. \n"));
            }
          } else {
            MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Could not find a CfgCur record to validate. \n"));
          }
        //}

        //skip if there are no CfgIn records for this dimm
        if ((*chdimmList)[dimm].ngnCfgReqPresent == 1) {
          //Check if the request has been processed. If so, skip this dimm.
          if ( ((*chdimmList)[dimm].ngnCfgOutPresent != 0)  && ((*chdimmList)[dimm].ngnCfgOut.body.SequenceNumber == ((*chdimmList)[dimm].ngnCfgReq.body.SequenceNumber)) ) continue;

          //Validate the CfgReq records
          status = ValidateCfgRecords(host, sckt, ch, dimm, CFG_IN);
          if(status != SUCCESS) {
            (*chdimmList)[dimm].ngnCfgReqValid = 0;
            MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "CfgReq Validation FAILED. \n"));
          } else {
            (*chdimmList)[dimm].ngnCfgReqValid = 1;
            MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "CfgReq Validation SUCCESSFUL. \n"));
          }
        } else {
            MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Could not find a CfgReq record to validate. \n"));
        }
      }//dimm
    }//ch
  }//socket
  return SUCCESS;
}

UINT32
HandlePartitionFailure(PSYSHOST host, UINT8 sckt, UINT8 ch, UINT8 dimm, UINT32 nvmDimmStatusCode)
/**

  This routines clears the Interleaverequests related to a specific dimm

  @param host     - Pointer to sysHost
  @param sckt   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval SUCCESS

**/
{

  UINT8 NumOfDimmsInInterleaveSet=0, i, j, socketNum, channelNum, dimmNum, interleaveReqId;
  UINT32  status = SUCCESS;
  struct memVar *mem;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *sourceInterleave=NULL, *destInterleave=NULL;
  NGN_DIMM_INTERLEAVE_ID_HOST  *sourceInterleaveId=NULL, *destInterleaveId=NULL;

  socketNum=0;
  channelNum=0;
  dimmNum=0;


  mem = &host->var.mem;
  chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

  //Update CfgOut for failure of partion request
  (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus = ((nvmDimmStatusCode & 0xFF) << 8) | (EncodePartitionResponse(FNV_FW_ERROR ));
  (*chdimmList)[dimm].ngnCfgOut.body.ValidationStatus = NGN_VLD_STAT_ERROR;

  for(interleaveReqId=0; interleaveReqId<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; interleaveReqId++) {

    //Break if the interve info is not valid.
    if(!(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].RecPresent || !(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].Valid) break;

    //Set the Valid bit to 0 for all the interleaves.
    (*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].Valid = 0;
    (*chdimmList)[dimm].ngnCfgOut.interleave[interleaveReqId].InterleaveChangeResponseStatus = EncodeInterleaveResponse( PARTITIONING_REQUEST_FAILED );

    NumOfDimmsInInterleaveSet = (*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId].NumOfDimmsInInterleaveSet;
    sourceInterleave = &(*chdimmList)[dimm].ngnCfgReq.interleave[interleaveReqId];
    for(i=0; i<NumOfDimmsInInterleaveSet; i++)
    {
      socketNum = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][i].socket;
      channelNum = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][i].channel;
      dimmNum = (*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][i].dimm;
      sourceInterleaveId = &(*chdimmList)[dimm].ngnCfgReq.interleaveId[interleaveReqId][0];

      for(j=0; j<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; j++) {
        destInterleave = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReq.interleave[j]);
        destInterleaveId = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReq.interleaveId[j][0]);
        //skip if basic Interleave information doesnt match
        if ((destInterleave->NumOfDimmsInInterleaveSet != sourceInterleave->NumOfDimmsInInterleaveSet) && (destInterleave->InterleaveMemoryType != sourceInterleave->InterleaveMemoryType))
          continue;

        //Compare the list of dimms participating in the interleave and see if they match
        status = CompareDimmInterleaveId(host, sourceInterleaveId, destInterleaveId, NumOfDimmsInInterleaveSet);
        if (status == SUCCESS) {
          //We have found a matching interleave request in another dimm, clear the valid bit for this interleave set.
          mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReq.interleave[j].Valid = 0;
          mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgOut.interleave[interleaveReqId].InterleaveChangeResponseStatus = EncodeInterleaveResponse( NOT_PROCESSED );
        }
      }//j
    }//i
  }//interleaveReqId
  return SUCCESS;
}


UINT32
HandlePartitionRequests(PSYSHOST host)
/**

  This routine goes through CfgIn, gets at the paririon requests and applies them to the dimm.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  UINT32 status;
  UINT64_STRUCT tempData;
  UINT32 volSize=0, nonVolSize =0;
  UINT8 mbStatus = 0;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct fnvCISBuffer fnvBuffer;
  struct dimmPartition (*dimmPartitionList)[MAX_SOCKET][MAX_CH][MAX_DIMM];
  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;
        rankList = &nvramMem->socket[sckt].channelList[ch].dimmList[dimm].rankList;
        //Skip if DIMM is disabled
        if ((*dimmNvList)[dimm].mapOut[0] || (*rankList)[0].enabled == 0)  continue;
        //Skip if this dimm is new to the system
        if ((*chdimmList)[dimm].newDimm == 1) continue;

        //Check if there are partition request is present and valid for this dimm.
        if ( (*chdimmList)[dimm].ngnCfgReqPresent  && (*chdimmList)[dimm].ngnCfgReq.partitionSize.RecPresent && (*chdimmList)[dimm].ngnCfgReq.partitionSize.Valid )  {

          if(host->var.mem.MemMapState == MEM_MAP_STATE_RESOURCE_CALCULATION_DONE) {
            //Convert the sizes into 4kb granularity.
            nonVolSize = (*chdimmList)[dimm].ngnCfgReq.partitionSize.PartitionSize << CONVERT_64MB_TO_4KB_GRAN;
            volSize =  ((*dimmNvList)[dimm].rawCap - (*chdimmList)[dimm].ngnCfgReq.partitionSize.PartitionSize) << CONVERT_64MB_TO_4KB_GRAN;

            //Issue NVMCTLR command to partition the dimm. Sizes in 4KB granularity.
            status = SetDimmPartitionInfo(host, sckt, ch, dimm, volSize, nonVolSize, &fnvBuffer, &mbStatus);
            if (status == SUCCESS) {
              //Update CfgOut for success of partion request
              (*chdimmList)[dimm].ngnCfgOut.partitionSize.ResponseStatus = EncodePartitionResponse(PARTITIONING_SUCCESS );

              //Get the new dimm partition information and update the host structures.
              dimmPartitionList = GetFnvCisBufferDimmPartition(&fnvBuffer);
              (*dimmNvList)[dimm].volCap =  (UINT16)((*dimmPartitionList)[sckt][ch][dimm].volatileCap  >> CONVERT_4KB_TO_64MB_GRAN );
              (*dimmNvList)[dimm].nonVolCap = (UINT16)((*dimmPartitionList)[sckt][ch][dimm].persistentCap  >> CONVERT_4KB_TO_64MB_GRAN );
              //Start addresses are in byte granularity
              tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].volatileStart, CONVERT_B_TO_64MB);
              (*dimmNvList)[dimm].volRegionDPA = tempData.lo;
              tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].persistentStart, CONVERT_B_TO_64MB);
              (*dimmNvList)[dimm].perRegionDPA = tempData.lo;
            } else {
              //If the partition request failed, clear all the interleave requests from this dimm.
              HandlePartitionFailure(host, sckt, ch, dimm, mbStatus);
            }
          } else if(host->var.mem.MemMapState == MEM_MAP_STATE_RESOURCE_CALCULATION){
            //If this is the initial run, Update the size data in the internal structs without sending the command.
            //We will send the command in the enxt run.
            (*dimmNvList)[dimm].nonVolCap =  (UINT16)(*chdimmList)[dimm].ngnCfgReq.partitionSize.PartitionSize;
            (*dimmNvList)[dimm].volCap = (UINT16)((*dimmNvList)[dimm].rawCap - (*chdimmList)[dimm].ngnCfgReq.partitionSize.PartitionSize);
          }//if(host->var.mem.MemMapState)
        }
      }
    }
  }

  //Re-initialize variables for blk window size to handle any dimms that may have been updated with a paritioning request
  InitBlkCtrlRegion(host, MEM_TYPE_BLK_WINDOW);

  return SUCCESS;
}

UINT32
GetScktInterleaveInfo(PSYSHOST host, NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *interleavePtr, NGN_DIMM_INTERLEAVE_ID_HOST *interleaveIdPtr, UINT16 scktInterleave[MC_MAX_NODE], UINT8 *sktWays)
/**

  This gets the socket interleave information from the interleave request.

  @param host  - Pointer to sysHost
  @param interleavePtr  - Pointer to interleave table
  @param interleaveIdPtr  - pointer to the first dimm info struct for this interleave set
  @param scktInterleave  - pointer to the socket interleav structure
  @param sktWays  - pointer to the variable to store total IMC ways

  @retval SUCCESS

**/
{
  UINT8 i, socket, mc, NumOfDimmsInInterleaveSet;
  NumOfDimmsInInterleaveSet = interleavePtr->NumOfDimmsInInterleaveSet;

  //Loop through the source InterleaveDimm set
  for(i=0; i<NumOfDimmsInInterleaveSet; i++){

    //Find the socket number and MC number for this dimm
    socket = interleaveIdPtr->socket;
    mc = interleaveIdPtr->channel / MAX_MC_CH;

    if(scktInterleave[(socket * host->var.mem.maxIMC) + mc] ==0) {
      //Update the socket interleave structure for every MC that is part of the interleave.
      scktInterleave[(socket * host->var.mem.maxIMC) + mc] = 1;
      (*sktWays)++;
    }
    interleaveIdPtr++;
  }

  return SUCCESS;
}



UINT32
GetChInterleaveInfo(PSYSHOST host, UINT8 sckt, UINT8 ch, UINT8 dimm,  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *interleavePtr, NGN_DIMM_INTERLEAVE_ID_HOST *interleaveIdPtr, UINT8 *ChInterleaveBitMap)
/**

  This routine creates a DRAM rule for a specific interleave request and updates the SADTable struct in corresponding sockets.

  @param host  - Pointer to sysHost
  @param sckt  - socket number
  @param ch  - chennal number
  @param dimm  - dimm number
  @param interleavePtr  - pointer to the first interleave set
  @param interleaveIdPtr  - pointer to the first dimm info struct for this interleave set

  @retval SUCCESS

**/
{
  UINT8 i, j, k, counter, socketId, mcId, channelBitMap[MC_MAX_NODE]={0}, NumOfDimmsInInterleaveSet, mcParticipatedFlag;
  NGN_DIMM_INTERLEAVE_ID_HOST *tempInterleaveIdPtr;
  
  *ChInterleaveBitMap = 0;
  counter = 0;

  NumOfDimmsInInterleaveSet = interleavePtr->NumOfDimmsInInterleaveSet;
  //Loop for each socket and each mc
  for (i = 0; i < MAX_SOCKET; i++) {
    for (j = 0; j < host->var.mem.maxIMC; j++) {
      mcParticipatedFlag = 0;
      tempInterleaveIdPtr = interleaveIdPtr;
      //Loop through the source Interleave Dimm set
      for (k = 0; k < NumOfDimmsInInterleaveSet; k++){
        //Find the socket number and MC number for this dimm
        socketId = tempInterleaveIdPtr->socket;
        mcId = tempInterleaveIdPtr->channel / MAX_MC_CH;

        if (i==socketId && j==mcId) {
          mcParticipatedFlag = 1;
          //Update the channel interleave bit map for this MC
          if (mcId) {
            channelBitMap[counter] |= (BIT0 << (tempInterleaveIdPtr->channel - MAX_MC_CH));
          } else { 
            channelBitMap[counter] |= (BIT0 << tempInterleaveIdPtr->channel);
          }
        }
        tempInterleaveIdPtr++;
      }//k
      if (mcParticipatedFlag)
        counter++;
    }//j
  }//i

  if (counter > 1) {
    //Verify if channel interleave bitmap matches between all MCs in this interleave set, if not return error.
    for (i = 1; i < counter; i++) {
      if (channelBitMap[i] != channelBitMap[i-1]) {
        return FAILURE;
      }
    }
  } else if (counter == 0) {
    // s5331732: If a match can't be found, return fail to prevent default zero data from being treated as real
    return FAILURE;
  }

  //Initialise the return variable
  *ChInterleaveBitMap = channelBitMap[0];

  return SUCCESS;
}

UINT8
FindImcBitmap(PSYSHOST host, UINT16 scktInterleave[MC_MAX_NODE], UINT8 socket)
/**

  This routine helps to find the imc bitmap of MCs interleaveds for a SAD rule in a particular socket.

  @param host  - Pointer to sysHost
  @param scktInterleave  - structure with list of MCs that are part of this interleave
  @param socket  - Socket number

  @retval imcBitmap =- bit map of imcs interleaved in this SAD rule from thie socket.

**/
{
  UINT8 bitmap = 0;
  //create a bit map based on scktInterleave struct.
  if (socket < MAX_SOCKET) {  // Also makes sure we don't go out of bounds for scktInterleave
    bitmap = (UINT8) (scktInterleave[socket*MAX_IMC] | (scktInterleave[(socket*MAX_IMC) + 1] << 1));
  }
  return bitmap;
}


UINT32
AddSADRule(PSYSHOST host, UINT16 scktInterleave[MC_MAX_NODE], UINT8 sktWays, UINT8 FMChInterleaveBitMap, UINT8 NMBitmap, UINT16 InterSizePerCh, UINT8 memType, UINT8 mirrorEnable, UINT32 interleaveFormat, UINT32 *nodeLimit)
/**

  This routine creates a DRAM rule for a specific interleave request and updates the SADTable struct in corresponding sockets.

  @param host  - Pointer to sysHost
  @param scktInterleave  - structure with list of MCs that are part of this interleave
  @param sktWays  - Number of IMCs involved in this interleave
  @param FMChInterleaveBitMap  - FM Channel Interleave bitmap
  @param InterSizePerCh  - Interleave size form each channel
  @param memType  - PMEM / PMEM$
  @param mirrorEnable  - Enable/Disable mirroring for this SAD rule
  @param interleaveFormat - Interleave granularities for this SAD rule
  @param nodeLimit  - Pointer to return the limit of SAD created.

  @retval status

**/
{

  UINT8 i, socketNum, chWays=0, sadIndex, mc;
  UINT32 TotalInterleaveSize, status;
  struct Socket  *socket;
  *nodeLimit=0;
  //Find the channel ways for this interleave
  for(i=0; i<MAX_MC_CH ; i++){
    if (FMChInterleaveBitMap & (BIT0<<i))
      chWays ++;
  }

  TotalInterleaveSize = InterSizePerCh * sktWays * chWays;
  //Parse through each socket to create a SAD
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
    socket = &host->var.mem.socket[socketNum];
    //Skip the sockets that is not participating in the interleave
    if((scktInterleave[socketNum *  host->var.mem.maxIMC] == 0) && (scktInterleave[(socketNum *  host->var.mem.maxIMC)+1] == 0)) continue;

    //Find the index of next SAD to be filled for this socket
    sadIndex = FindSadIndex(host, socketNum);
    // Should not reach end of SAD rules before allocating all memory, Issue warning and break TAD loop.
    // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
    if (sadIndex >= MAX_SAD_RULES) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SAD rules exceeds %d\n", MAX_SAD_RULES));
      OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
      return ERROR_RESOURCE_CALCULATION_FAILURE;
    }
    // Make sure we don't go out of bounds of SAD array
    if ((sadIndex - 1) < 0) {
      return FAILURE;
    }
    //Add SAD details for the new SAD
    socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + TotalInterleaveSize;
    socket->SAD[sadIndex].Enable = 1;
    socket->SAD[sadIndex].ways  = sktWays;
    socket->SAD[sadIndex].type = memType;
    socket->SAD[sadIndex].imcInterBitmap =  FindImcBitmap(host, scktInterleave, socketNum);
    for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
      //skip mc that is not part of the interleave.
      if (socket->SAD[sadIndex].imcInterBitmap & (BIT0 << mc)){
        socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMChInterleaveBitMap;
        if (memType == MEM_TYPE_PMEM_CACHE) {
          socket->SAD[sadIndex].channelInterBitmap[mc] = NMBitmap;
        }
      }
    } 
    socket->SAD[sadIndex].local = 1;
    socket->SAD[sadIndex].mirrored = mirrorEnable;
    socket->SAD[sadIndex].granularity = EncodeInterleaveGranularity( host, memType, interleaveFormat ) ;
    socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType ) ;

    *nodeLimit = socket->SAD[sadIndex].Limit;

    //Update SadInterleaveList
    for(i=0; i<MAX_SOCKET* host->var.mem.maxIMC;i++) {
      //Update the SAD Interleave list
      if(scktInterleave[i]) socket->SADintList[sadIndex][i] = 1;
    }

  }//socket

  //create remote SAD entries for the other sockets
  status = AddRemoteSadEntry(host, scktInterleave, *nodeLimit, memType);
  if(status != SUCCESS) return status;
  return SUCCESS;

}

UINT32
CheckNMPopulation(PSYSHOST host, UINT16 scktInterleave[MC_MAX_NODE], UINT8 *NMBitmap)
/**

  This routine compare the NM population of all the MC involved in the SAD rule and returns error if they dont match.

  @param host  - Pointer to sysHost
  @param scktInterleave  - sckt interleave struct that holds list MC  involved in the request.
  @param NMBitmap  - Pointer to variable to store the bitmap


  @retval SUCCESS - if NM population matches
  @retval Failure - if NM population doesnt match


**/
{
  UINT8 socketNum, mc;
  UINT8 Bitmap;
  Bitmap = 0xff;

  //Parse through each socket to create a SAD
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
    if (host->nvram.mem.socket[socketNum].enabled == 0) continue;

    for(mc=0; mc<host->var.mem.maxIMC; mc++) {

      //Skip the sockets that is not participating in the interleave
      if(scktInterleave[(socketNum *  host->var.mem.maxIMC) + mc] == 0)  continue;

      if( (Bitmap != 0xFF) && (Bitmap  != FindNMBitMap(host, socketNum, mc)) )
        return FAILURE;
      else
        Bitmap = FindNMBitMap(host, socketNum, mc);

    }//mc
  }//socketnum

  *NMBitmap = Bitmap;
 return SUCCESS;
}


UINT32
ApplyInterleave(PSYSHOST host, UINT8 sckt, UINT8 ch, UINT8 dimm, NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *interleavePtr, NGN_DIMM_INTERLEAVE_ID_HOST *interleaveIdPtr, UINT32 *nodeLimit)
/**

  This routine creates a DRAM rule for a specific interleave request and updates the SADTable struct in corresponding sockets.

  @param host  - Pointer to sysHost
  @param sckt  - socket number
  @param ch  - Channel number
  @param dimm  - dimm number
  @param interleavePtr  - Pointer to interleave table
  @param interleaveIdPtr  - pointer to the first dimm info struct for this interleave set

  @retval SADLimit

**/
{
  UINT8 memType, sktWays=0,ChInterleaveBitMap=0, NMBitmap=0;
  UINT16 scktInterleave[MC_MAX_NODE] =  {0},  InterSizePerCh=0;
  UINT32 status;

  //Find the memory type based on perMemMode
  if(host->var.mem.dfxMemVars.dfxPerMemMode == PER_MEM_CACHE_MODE)
    memType = MEM_TYPE_PMEM_CACHE;
  else
    memType = MEM_TYPE_PMEM;

  //Get the MCs that are involved in this interleave
  status = GetScktInterleaveInfo(host, interleavePtr, interleaveIdPtr, scktInterleave, &sktWays);

  //Get the channel interleave for this interleave. (channel interleave should be the same in each MC involved. This is a restricion in SKX.
  //NVMDIMM mgmt driver is responsible for placing requests in which the channel interleave info matches between the MCs)
  status = GetChInterleaveInfo(host, sckt, ch, dimm, interleavePtr, interleaveIdPtr, &ChInterleaveBitMap);
  if(status==FAILURE)
  {
    *nodeLimit = 0;
    MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Channel interleave doesnt match between IMCs specied in the interleave Request.\n"));
    //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
    return CHANNEL_INTERLEAVE_MISMATCH;
  }

  //If the memptype is pmem$, make sure the NM population matches between imcs.
  if(memType == MEM_TYPE_PMEM_CACHE) {
    status = CheckNMPopulation(host, scktInterleave, &NMBitmap);
    if(status==FAILURE)
    {
      *nodeLimit = 0;
      MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "NM population doesnt match between IMCs specied in the interleave Request.\n"));
      //OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, socket, 0xFF, 0xFF, 0xFF);
      return NM_POPULATION_MISMATCH;
    }
  }

  //Determine the interleave size per channel for this interleave set
  InterSizePerCh = interleaveIdPtr->PartitionSize;

  status  = AddSADRule(host, scktInterleave, sktWays, ChInterleaveBitMap, NMBitmap, InterSizePerCh, memType, interleavePtr->MirrorEnable, interleavePtr->InterleaveFormat, nodeLimit);
  if(status != SUCCESS) return status;
  return OPERATION_SUCCESS;
}


UINT32
UpdateProcessedFlag (PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *sourceInterleave, NGN_DIMM_INTERLEAVE_ID_HOST *sourceInterleaveId, UINT32 nodeLimit, UINT8 recordType, UINT32 responseStatus)
/**

  Description: This routine sets the "processedFlag" & SadLimit variable in the interleave request
  struct in dimm structs if the requests are processed and SADs were created.

  @param host  - Pointer to sysHost
  @param sckt  - Socket Id
  @param ch  - channel id
  @param Dimm  - Dimm num
  @param *sourceInterleave  - Pointer to the interleave struct
  @param *sourceInterleaveId  - Pointer to the dimm id struct.
  @param recordType  - specifies if this is CFGIN or CFG_CUR
  @param nodeLimit   - Limit of SAD Rule related to the request

  @retval SUCCESS - if sucessfull
  @retval Failure - if not sucessfull


**/
{
  UINT8 NumOfDimmsInInterleaveSet=0, i, j, socketNum, channelNum, dimmNum;
  UINT32  status = SUCCESS;
  struct memVar *mem;
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *destInterleave=NULL;
  NGN_DIMM_INTERLEAVE_ID_HOST  *destInterleaveId=NULL;
  NGN_DIMM_INTERLEAVE_ID_HOST *tempInterleaveId = sourceInterleaveId ;

  socketNum=0;
  channelNum=0;
  dimmNum=0;

  mem = &host->var.mem;

  NumOfDimmsInInterleaveSet = sourceInterleave->NumOfDimmsInInterleaveSet;
  for(i=0; i<NumOfDimmsInInterleaveSet; i++)
  {
    //Init temp variables based on dimm id struct
    socketNum = tempInterleaveId->socket;
    channelNum = tempInterleaveId->channel;
    dimmNum = tempInterleaveId->dimm;

    //Scan through the interleave requests of the participating dimm to find a matching request.
    for(j=0; j<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; j++) {
      if(recordType == CFG_IN) {
        destInterleave = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReq.interleave[j]);
        destInterleaveId = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgReq.interleaveId[j][0]);
      } else if(recordType == CFG_CUR) {
        destInterleave = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgCur.interleave[j]);
        destInterleaveId = &(mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgCur.interleaveId[j][0]);
      } else
        return FAILURE;

      //skip if basic Interleave information doesnt match
      if ((destInterleave->NumOfDimmsInInterleaveSet != sourceInterleave->NumOfDimmsInInterleaveSet) && (destInterleave->InterleaveMemoryType != sourceInterleave->InterleaveMemoryType))
        continue;

      //Compare the list of dimms participating in the interleave and see if they match
      status = CompareDimmInterleaveId(host, sourceInterleaveId, destInterleaveId, NumOfDimmsInInterleaveSet);
      if( status == SUCCESS ) break ;
    }

    if(status == SUCCESS)  {
      //If we are here, we have found a matching interleave request.
      destInterleave->Processed = 1;
      destInterleave->SadLimit = nodeLimit;

      // Make sure we don't go out of bounds for array "interleave"
      if (j >= MAX_UNIQUE_NGN_DIMM_INTERLEAVE) {
        return FAILURE;
      }
      //Update the cfgout that the operation is successfull.
      mem->socket[socketNum].channelList[channelNum].dimmList[dimmNum].ngnCfgOut.interleave[j].InterleaveChangeResponseStatus = EncodeInterleaveResponse((UINT8)responseStatus);
    }
    else  {
      return FAILURE;
    }
    tempInterleaveId++;
  }

 return SUCCESS;
}

UINT32
ApplyCCURRecords(PSYSHOST host)
/**

  This routine goes through each NVMDIMM in the system and if there are no CFGIn requests, it will apply the current config
  based on CCUR data retreived from the dimm.

  @param host  - Pointer to sysHost

  @retval status

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm, i;
  UINT32 nodeLimit,status ;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];


  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *interleavePtr = NULL;
  NGN_DIMM_INTERLEAVE_ID_HOST  *interleaveIdPtr= NULL;
  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //Skip if this dimm is new to the system
        if ((*chdimmList)[dimm].newDimm == 1) continue;

        //If we are in degraded memory mapping mode, we will not honor new requests, Donot look for its presence and validity.
        if(!host->var.mem.MemMapDegradeLevel) {
          //Check if there are CfgIn request is present and valid for this dimm. If present, skip this dimm.
          if ( (*chdimmList)[dimm].ngnCfgReqPresent && (*chdimmList)[dimm].ngnCfgReqValid )  continue;
        }

        //Check if we have CCUR presnet and valid for this dimm, if not, skip this dimm.
        if ( !(*chdimmList)[dimm].ngnCfgCurPresent || !(*chdimmList)[dimm].ngnCfgCurValid )  continue;

        for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
          //Skip if there is no interleave record or if the record failed our validation previously
          if ( !(*chdimmList)[dimm].ngnCfgCur.interleave[i].RecPresent  || !(*chdimmList)[dimm].ngnCfgCur.interleave[i].Valid ) continue;

          //Skip if this interleave is already processed.
          if ((*chdimmList)[dimm].ngnCfgCur.interleave[i].Processed == 1)  continue;

          interleavePtr = &(*chdimmList)[dimm].ngnCfgCur.interleave[i];
          interleaveIdPtr = &(*chdimmList)[dimm].ngnCfgCur.interleaveId[i][0];

          //init nodeLimit to 0.
          nodeLimit =0;

          //Call the helper function to create the SAD rule for this interleave.
          status = ApplyInterleave(host, sckt, ch, dimm, interleavePtr, interleaveIdPtr, &nodeLimit);
          if(status == ERROR_RESOURCE_CALCULATION_FAILURE) return status;
          //Update the interleave structure of all the participating dimms to denote the request has been implemented and update the response status.
          UpdateProcessedFlag(host, sckt, ch, dimm, interleavePtr, interleaveIdPtr, nodeLimit, CFG_CUR, status );

        }//i
      }//dimm
    }//ch

  }//socket


  return SUCCESS;
}


UINT32
ApplyInterleaveRequests(PSYSHOST host)
/**

  This routine goes through each NVMDIMM in the system and if there are  CFGIn requests, it will apply Interleave requests retrieved form the dimm

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm, i;
  UINT32 nodeLimit, status;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];


  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST  *interleavePtr= NULL;
  NGN_DIMM_INTERLEAVE_ID_HOST  *interleaveIdPtr= NULL;
  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelNvList = &nvramMem->socket[sckt].channelList;

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = &nvramMem->socket[sckt].channelList[ch].dimmList;
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //Skip if this dimm is new to the system
        if ((*chdimmList)[dimm].newDimm == 1) continue;

        //Check if there are CfgIn request is present and valid for this dimm. If not present, skip this dimm.
        if ( !(*chdimmList)[dimm].ngnCfgReqPresent || !(*chdimmList)[dimm].ngnCfgReqValid )  continue;


        for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
          //Skip if there is no interleave record or if the record failed our validation previously
          if ( !(*chdimmList)[dimm].ngnCfgReq.interleave[i].RecPresent  || !(*chdimmList)[dimm].ngnCfgReq.interleave[i].Valid ) continue;

          //Skip if this interleave is already processed.
          if ((*chdimmList)[dimm].ngnCfgReq.interleave[i].Processed == 1)  continue;

          interleavePtr = &(*chdimmList)[dimm].ngnCfgReq.interleave[i];
          interleaveIdPtr = &(*chdimmList)[dimm].ngnCfgReq.interleaveId[i][0];

          //Init nodLimit variable to 0
          nodeLimit = 0;
          //Call the helper function to create the SAD rule for this interleave.
          status = ApplyInterleave(host, sckt, ch, dimm, interleavePtr, interleaveIdPtr, &nodeLimit);
          if(status == ERROR_RESOURCE_CALCULATION_FAILURE) return status;
          //Update the interleave structure of all the participating dimms to denote the request has been implemented and update the response status.
          UpdateProcessedFlag(host, sckt, ch, dimm, interleavePtr, interleaveIdPtr, nodeLimit, CFG_IN, status );

        }//i
      }//dimm
    }//ch
  }//socket

  return SUCCESS;
}

