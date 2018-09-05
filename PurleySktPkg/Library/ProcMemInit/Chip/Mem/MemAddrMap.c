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
 * Copyright 2006 - 2017 Intel Corporation All Rights Reserved.
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
#include "Token.h" //SMCPKG_SUPPORT
#include "MemAddrMap.h"

// Module variables

// Function definitions

#ifdef NVMEM_FEATURE_EN
#define SIZE_4GB_64MB 0x40
#endif

VOID
ClearMemMapStructs (PSYSHOST host)
/**

  This routine clears host structures used for memory mapping so that memory mapping algorithm can be run again afresh.

  @param host  - Pointer to sysHost

  @retval VOID

**/
{
  UINT8 sadIndex, sckt, mc, i, tad, ch, dimm ;
  UINT8 maxSADRules = MAX_SAD_RULES;
  struct memVar *mem;
  struct Socket *socket;
  struct socketNvram (*socketNvram)[MAX_SOCKET];
  struct IMC *imc;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];

  socketNvram = &host->nvram.mem.socket;
  mem = &host->var.mem;

  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if ((*socketNvram)[sckt].enabled == 0) continue;

    socket = &mem->socket[sckt];
    channelNvList = GetChannelNvList(host, sckt);

    //Clear all the SAD entries from the internal structs
    for (sadIndex = 0; sadIndex < maxSADRules; sadIndex++) {
      socket->SAD[sadIndex].Enable = 0;
      socket->SAD[sadIndex].ways  = 0;
      socket->SAD[sadIndex].type = 0;
      socket->SAD[sadIndex].imcInterBitmap =  0;
      socket->SAD[sadIndex].Limit =    0;
      socket->SAD[sadIndex].local = 0;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].granularity = 0;
      for (mc = 0; mc < MAX_MC; mc++) {
        socket->SAD[sadIndex].FMchannelInterBitmap[mc] = 0;
        socket->SAD[sadIndex].channelInterBitmap[mc] = 0;
      }
      socket->SAD[sadIndex].tgtGranularity = 0;
      //clear the SAD Interleave list
      for(i=0; i<MAX_SOCKET * MAX_IMC;i++)
        socket->SADintList[sadIndex][i] = 0;
    }

    //Clear all the TAD entries from the internal structs
    for(mc=0; mc< host->var.mem.maxIMC; mc++){
      imc = &host->var.mem.socket[sckt].imc[mc];
      for (tad=0; tad<TAD_RULES; tad++) {
        imc->TAD[tad].Enable = 0;
        imc->TAD[tad].SADId = 0;
        imc->TAD[tad].mode = 0;
        imc->TAD[tad].socketWays = 0;
        imc->TAD[tad].channelWays = 0;
        imc->TAD[tad].Limit = 0;
        imc->TAD[tad].ChannelAddressLow = 0;
        imc->TAD[tad].ChannelAddressHigh = 0;
        //Parse through each channel in the IMC and clear the TAD offset and Intlist for this TAD
        for (i = 0; i < MAX_MC_CH; i++) {
          imc->TADOffset[tad][i]= 0;
          imc->TADChnIndex[tad][i]= 0;
          imc->TADintList[tad][i]= 0;
        }
      }//tad
    }//mc

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      dimmNvList = GetDimmNvList(host, sckt, ch);
      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        //Skip if this dimm is new to the system
        if ((*chdimmList)[dimm].newDimm == 1) continue;

        for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
          //clear processed flag and the sadlimit variable if we have a valid intreleave request present in CFGIN
          if ( (*chdimmList)[dimm].ngnCfgReq.interleave[i].RecPresent  && (*chdimmList)[dimm].ngnCfgReq.interleave[i].Valid ) {
            (*chdimmList)[dimm].ngnCfgReq.interleave[i].Processed = 0;
            (*chdimmList)[dimm].ngnCfgReq.interleave[i].SadLimit = 0;
          }
          //clear processed flag and the sadlimit variable if we have a valid intreleave resord  present in CCUR
          if ( (*chdimmList)[dimm].ngnCfgCur.interleave[i].RecPresent  && (*chdimmList)[dimm].ngnCfgCur.interleave[i].Valid ) {
            (*chdimmList)[dimm].ngnCfgCur.interleave[i].Processed = 0;
            (*chdimmList)[dimm].ngnCfgCur.interleave[i].SadLimit = 0;
          }
        }//i
      }//dimm
    }//ch
  }//sckt
}


UINT32
DegradeMemoryMapLevel (PSYSHOST host)
/**

  This degrades the memory mapping level and reruns the memory mapping algorithm.


  @param host  - Pointer to sysHost

  @retval ERROR_RESOURCE_CALCULATION_FAILURE

**/
{
  struct memVar *mem;
  mem = &host->var.mem;
  //If CR mgmt driver is not responsible, then we halt.
  if(host->var.mem.dfxMemVars.dfxDimmManagement != DIMM_MGMT_CR_MGMT_DRIVER) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
             "NVMDIMM Management not controlling NVMDIMMs which prevents us going into any degraded memory mapping mode. So Halting the system due to this failure.\n"));
#if SMCPKG_SUPPORT
    OutputError (host, ERR_NGN, NGN_DRIVER_NOT_RESPONSIBLE, 0xFF, 0xFF, 0xFF, 0xFF);
#else
    FatalError(host, ERR_NGN, NGN_DRIVER_NOT_RESPONSIBLE);
#endif
  }
  //Degrade the memory mapping level
  mem->MemMapDegradeLevel +=  1;

  //Clear Memory map structures.
  ClearMemMapStructs(host);

  //If we are here it means, that we have completed initial run of memmap and found that we DONOT have all silicon resources
  //So reset the resourceCalcDone variable and rerun the memory mapping algorithm.
  //host->var.mem.ResourceCalculationDone = 0;
  host->var.mem.MemMapState = MEM_MAP_STATE_RESOURCE_CALCULATION_FAILURE;

  return ERROR_RESOURCE_CALCULATION_FAILURE;
}

/**

  Gets printable SAD type.

  @param type  - type of SAD record
  @retval string with SAD type

**/
static const CHAR8 *GetSADTableTypeString(UINT8 type){
    switch(type){
    case MEM_TYPE_1LM :
      return "1LM       ";
    case MEM_TYPE_2LM:
      return "2LM       ";
    case MEM_TYPE_PMEM_CACHE :
      return "PMEM_CACHE";
    case MEM_TYPE_BLK_WINDOW :
      return "BLK_WINDOW";
    case MEM_TYPE_CTRL :
      return "NVDIM_CTRL";
    case MEM_TYPE_PMEM :
      return "PMEM      ";
    default:
      return "??????????";
    }
}

/**

Displays the SADTable data structure in the host structure

@param host  - Pointer to sysHost

**/
VOID DisplaySADTable(PSYSHOST host) {
  UINT8  socketNum, sadIndex, i;
  //Print SAD teble for each socket
  for (socketNum = 0; socketNum < MAX_SOCKET; socketNum++) {
    // skip empty SAD tables
    if (host->var.mem.socket[socketNum].SAD[0].Enable == 0) continue;

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n********SAD table for socket %d*******\n", socketNum));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Enable  Type        Limit    Ways  MC0_ChIntBitmap  MC1_ChIntBitmap  MC0_FMchIntBitmap  MC1_FMchIntBitmap  MC_IntBitmap  Local\n"));

    for(sadIndex=0; sadIndex<MAX_SAD_RULES; sadIndex++){
      if(host->var.mem.socket[socketNum].SAD[sadIndex].Enable == 0) continue;
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
          "%6d  %10s  0x%05x  %4d  %15d  %15x  %17x  %17x  %12x  %5d\n",
          host->var.mem.socket[socketNum].SAD[sadIndex].Enable,
          GetSADTableTypeString(host->var.mem.socket[socketNum].SAD[sadIndex].type),
          host->var.mem.socket[socketNum].SAD[sadIndex].Limit,
          host->var.mem.socket[socketNum].SAD[sadIndex].ways,
          host->var.mem.socket[socketNum].SAD[sadIndex].channelInterBitmap[IMC0],
          host->var.mem.socket[socketNum].SAD[sadIndex].channelInterBitmap[IMC1],
          host->var.mem.socket[socketNum].SAD[sadIndex].FMchannelInterBitmap[IMC0],
          host->var.mem.socket[socketNum].SAD[sadIndex].FMchannelInterBitmap[IMC1],
          host->var.mem.socket[socketNum].SAD[sadIndex].imcInterBitmap,
          host->var.mem.socket[socketNum].SAD[sadIndex].local));
    }
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"<<SAD Interleave List>>"));
    for(sadIndex=0; sadIndex<MAX_SAD_RULES; sadIndex++){
      if(host->var.mem.socket[socketNum].SAD[sadIndex].Enable == 0) continue;
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
      for(i=0; i<MAX_SAD_WAYS; i++)
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%d\t", host->var.mem.socket[socketNum].SADintList[sadIndex][i]));
    }
  }
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n</SADTable>\n"));
}

/**

Displays the TADTable data structure in the host structure

@param host  - Pointer to sysHost

**/
VOID DisplayTADTable(PSYSHOST host) {
  UINT8   mcIndex;
  UINT8   i;
  UINT8   sckt;
  struct IMC *imc;

  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    for(mcIndex=0; mcIndex< host->var.mem.maxIMC; mcIndex++) {
      imc = &host->var.mem.socket[sckt].imc[mcIndex];
      if (!imc->TAD[0].Enable) continue;
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n*******TAD Table for socket %d Mc %d*******\n", sckt,mcIndex ));
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Enable    Limit  SktWays  ChWays  Mode  SadID\n" ));
      for (i = 0; i < TAD_RULES; i++) {
        if (!imc->TAD[i].Enable) break;
        MemDebugPrint(
       (host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%6d  0x%05x  %7d  %6d  %4d  %5d\n",imc->TAD[i].Enable ,imc->TAD[i].Limit,imc->TAD[i].socketWays,imc->TAD[i].channelWays, imc->TAD[i].mode ,imc->TAD[i].SADId ));
      }
      MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n</TADTable>\n"));
    }
  }
}

UINT8
FindNMBitMap(
  PSYSHOST  host,
  UINT8 sckt,
  UINT8 mc)
  /**

  Description: Find the NM population in the specified MC and fill the bitmap accordingly

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param mcIndex - mc id

  @retval Bit map of NM population in the given MC

**/
{
  struct ddrChannel *channelList;
  UINT8 ch, channelInterBitmap = 0;
  channelList = &host->var.mem.socket[sckt].channelList[0];
  for(ch = 0 ; ch < host->var.mem.numChPerMC ; ch++) {
    if(channelList[(mc*MAX_MC_CH)+ch].memSize != 0)
      channelInterBitmap |= (BIT0<<ch);
  }
  return channelInterBitmap;
}


BOOLEAN IsDDRTPresent(PSYSHOST host, UINT8 socketBitMap)
/**

  Check NVMDIMM presence in the socket

  @param host           - Pointer to sysHost
  @param socketBitMap   - BitMap of the sockets that need to be investigated.

  @retval TRUE /FALSE based on NVMDIMM presense

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    //Skip the sockets that are not requested to be verified.
    if (((BIT0<<sckt) & socketBitMap) == 0) continue;
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;
  channelNvList = GetChannelNvList(host, sckt);
    // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        dimmNvList = GetDimmNvList(host, sckt, ch);
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if((*dimmNvList)[dimm].aepDimmPresent) return TRUE;
      }
    }
  }
  return FALSE;
}

UINT8
FindTadIndex(
  PSYSHOST  host,
  UINT8 sckt,
  UINT8 mcIndex)
  /**

  Description: Find the next Tad entry that needs to be filled for this Socket-IMC.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param mcIndex - mc id

  @retval Index of Next available Tad entry

**/
{
  struct IMC *imc;
  UINT8 i, tadIndex;

 //scan through the TAD table and find the next tad entry with Enable =0
  imc = &host->var.mem.socket[sckt].imc[mcIndex];
  for( i=0; i<TAD_RULES; i++) {
    if (imc->TAD[i].Enable ==0){
      tadIndex = i;
      return tadIndex;
    }
  }
  return 0xff;
}

UINT8
GetTargetInterleaveGranularity(
  PSYSHOST  host,
  UINT8     memType
)
/**

  Description: Deteremine the target interleave granularity based on the memType

  @param host  - Pointer to sysHost
  @param memType - Memory type

  @retval Target interleave granularity
**/
{
  UINT8 tgtInterleaveGranularity = MEM_CTRL_TGT_INTERLEAVE_64B ;

  switch(memType) {
    case MEM_TYPE_1LM :
      // Supported 1LM interleave granularity options have either 64B or 256B target interleave
      if( host->var.mem.memInterleaveGran1LM == MEM_INT_GRAN_1LM_64B_64B )  {
        tgtInterleaveGranularity = MEM_CTRL_TGT_INTERLEAVE_64B ;
      }
      else {
        tgtInterleaveGranularity = MEM_CTRL_TGT_INTERLEAVE_256B ;
      }
      break;

    case MEM_TYPE_2LM :
    case MEM_TYPE_PMEM_CACHE :
    case MEM_TYPE_BLK_WINDOW :
    case MEM_TYPE_CTRL :
      // All supported 2LM, PMEM$, BLK Window and CTRL region interleave granularity options have 4KB target interleave
      tgtInterleaveGranularity = MEM_CTRL_TGT_INTERLEAVE_4KB ;
      break;

    case MEM_TYPE_PMEM :
      // Supported PMEM interleave granularity options have either 4KB or 1GB interleave depending on the NUMA flag
      if(host->setup.mem.options & NUMA_AWARE)  {
        tgtInterleaveGranularity = MEM_CTRL_TGT_INTERLEAVE_4KB ;
      }
      else {
        tgtInterleaveGranularity = MEM_CTRL_TGT_INTERLEAVE_1GB;
      }
      break;
  }
  return( tgtInterleaveGranularity ) ;
}

INT8
GetFirstSad(
  PSYSHOST  host,
  UINT8 sckt,
  UINT8 mcIndex,
  UINT8 regionType)
/**

  Description: Get the first SAD rule of the specified type that the MC is part of.

  @param host  - Pointer to sysHost
  @param sckt  - Socket Id
  @param mcIndex  - IMC Id
  @param regionType  - Type of the SAD


  @retval SAD ID of the first SAD rule of the specified type that the MC is part of.

**/
{

  UINT8 i;
  struct  Socket *socket;
  socket = &host->var.mem.socket[sckt];

  for (i = 0; i < MAX_SAD_RULES; i++) {
    //Break if Sad is not enabled
    if(socket->SAD[i].Enable == 0) break;
    //Skip this SAD if the SAD is not local
    if(socket->SAD[i].local == 0) continue;

    //Check if it is a SAD of required type and the MC is part of this SAD
    if((socket->SAD[i].type == regionType) && (socket->SADintList[i][(sckt * host->var.mem.maxIMC)+ mcIndex]))
      return( i );
  }
  return( -1 );
}


UINT32
GetInterReqOffset(PSYSHOST host, UINT8 sckt, UINT8 mc, UINT8 ch, UINT8 dimm, UINT32 nodeLimit)
/**

  Description: Get the first SAD rule of the specified type that the MC is part of.

  @param host  - Pointer to sysHost
  @param sckt  - Socket Id
  @param mc  - IMC Id
  @param ch  - Chennal id
  @param Dimm  - Dimm num
  @param nodeLimit   - Limit of SAD Rule related to the request

  @retval Partition offset value for the dimm int he Interleave request corresponding to the SAD rule.

**/
{
  UINT8 i, j, NumOfDimmsInInterleaveSet,chIndex;
  struct memVar *mem;
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  NGN_DIMM_INTERLEAVE_INFORMATION_PCAT_HOST *interleavePtr;
  NGN_DIMM_INTERLEAVE_ID_HOST *interleaveIdPtr;
  mem = &host->var.mem;

  chIndex = (mc * MAX_MC_CH) + ch;
  chdimmList = &mem->socket[sckt].channelList[chIndex].dimmList;
  for(i=0; i<MAX_UNIQUE_NGN_DIMM_INTERLEAVE; i++) {
    //Skip if there is no interleave record or if the record failed our validation previously
    if ( !(*chdimmList)[dimm].ngnCfgReq.interleave[i].RecPresent  || !(*chdimmList)[dimm].ngnCfgReq.interleave[i].Valid ) continue;

    //Skip if this interleave is not processed or nodelimit doesnt match
    if ( (!(*chdimmList)[dimm].ngnCfgReq.interleave[i].Processed == 1) || (!((*chdimmList)[dimm].ngnCfgReq.interleave[i].SadLimit == nodeLimit)) ) continue;

    interleavePtr = &(*chdimmList)[dimm].ngnCfgReq.interleave[i];


    NumOfDimmsInInterleaveSet = interleavePtr->NumOfDimmsInInterleaveSet;
    //scan through the dimm info list to find the matching dimm
    for(j=0; j<NumOfDimmsInInterleaveSet; j++)
    {
      //Initialize Interleave id pointer to a dimm set
      interleaveIdPtr = &(*chdimmList)[dimm].ngnCfgReq.interleaveId[i][j];

      if( (sckt == interleaveIdPtr->socket) && (chIndex == interleaveIdPtr->channel) && (dimm == interleaveIdPtr->dimm) )
        //If the scket, channel and dimm infor matches, return the corresponding offset.
        return interleaveIdPtr->PartitionOffset;

    }//j

  }//i
  //IF we are here, it is an error condition.
  return 0;
}

//-----------------------------------------------------------------------------
// TAD (Channel) Interleave
//
// Description: Interleaves memory on the channel level within a socket. This
// routine figures out the mapping and fills in the structures TAD
// table.  It does not program any registers.
// assumed all SAD entries are sorted correctly at this point
//
//-----------------------------------------------------------------------------
UINT32 TADInterleave(PSYSHOST host, UINT16 TOLM)
{
  UINT8 sckt, sadIndex, i, mcIndex, channelWays, tadIndex, imcWays, dimm, tadLimitAdjustment = 0, totalWays;
  UINT32  sadInterleaveSize, interSizePerMc, interSizePerCh, channelMemUsed=0, tadLimit, TotalMemUsed;
  struct memVar *mem;
  struct Socket *socket;
  struct socketNvram *socketNvram;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct IMC *imc;
  UINT8   chInter = 0;
  UINT32  dpaOffset = 0xFFFFFFFF, interReqOffset =0;

  mem = &host->var.mem;
  tadIndex = 0;

  //Loop through each socket to look at the SADs of that socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {

    socket = &mem->socket[sckt];
    socketNvram = &host->nvram.mem.socket[sckt];

    // Skip socket if not enabled
    if (socketNvram->enabled == 0) continue;
    // Go to next socket if memsize is 0
    if (socket->memSize == 0) {
	  continue;
    }

    for (sadIndex = 0; sadIndex < MAX_SAD_RULES; sadIndex++) {
      //Break if Sad is not enabled
      if(socket->SAD[sadIndex].Enable == 0) break;
      //Skip this SAD if the SAD is not local
      if(socket->SAD[sadIndex].local == 0) continue;

      if(socket->SAD[sadIndex].Attr == SAD_NXM_ATTR) continue;

      // Determine which IMC channel bitmap to use
      mcIndex = GetMcIndexFromBitmap (socket->SAD[sadIndex].imcInterBitmap);

      switch(socket->SAD[sadIndex].type) {
        case MEM_TYPE_1LM :
          chInter = socket->SAD[sadIndex].channelInterBitmap[mcIndex];
          break;
        case MEM_TYPE_PMEM :
        case MEM_TYPE_2LM :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
          chInter = socket->SAD[sadIndex].FMchannelInterBitmap[mcIndex] ;
          break;
      }

      if (chInter == 0) {
        continue;
      }

      //Initialize local variables
      channelWays = 0;
      imcWays = socket->SAD[sadIndex].ways;
      if(sadIndex!=0)
        sadInterleaveSize = socket->SAD[sadIndex].Limit - socket->SAD[sadIndex-1].Limit;
      else
        sadInterleaveSize = socket->SAD[sadIndex].Limit;

      //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
      channelWays = CalculateWaysfromBitmap(chInter);

      //Adjust sadInterleaveSize for Memory hole
      if(socket->SAD[sadIndex].Limit >=TOLM) {
        if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0))  {

          sadInterleaveSize -= host->setup.common.lowGap;
          tadLimitAdjustment = 0 ;

          if( channelWays == 3 )  {

            // If TOLM boundary is not evenly divisible, then we need to adjust the sadInterleaveSize so that it divides evenly
            totalWays = imcWays * channelWays ;
            if( TOLM % totalWays ) {

              //5332821: Partial Mirroring with 3Chway population causes Memicals data miscompare
              if ((host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM)||(host->nvram.mem.RASmode & PARTIAL_MIRROR_2LM)) {
                if(imcWays == 1) {
                  totalWays = totalWays*2;
                }
              }

              sadInterleaveSize = sadInterleaveSize + (totalWays - ( TOLM % totalWays ) );
              tadLimitAdjustment = totalWays - ( TOLM % totalWays );

            }
          }
        }
      }

      //Calculate true interleaved sizes for each Channel
      interSizePerMc = sadInterleaveSize / imcWays;
      interSizePerCh = interSizePerMc / channelWays;

      if(socket->SAD[sadIndex].mirrored) {
        interSizePerCh = interSizePerCh*2;
      }

      //Fill the TAD table for each Mc in the socket
      for(mcIndex=0; mcIndex<host->var.mem.maxIMC; mcIndex++) {
        //If the current IMC is not a part of interleave, skip.
        if((socket->SAD[sadIndex].imcInterBitmap & (BIT0<<mcIndex)) == 0) continue;
        imc = &mem->socket[sckt].imc[mcIndex];
        //Get the index of next TAD to be filled.
        tadIndex = FindTadIndex(host, sckt, mcIndex );
        // Issue warning if all TAD rules used before all memory allocated
        if (tadIndex >= TAD_RULES) {
          MemDebugPrint((host, SDBG_MINMAX, sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "TAD rules exeeds %d\n", TAD_RULES));
          OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_TAD_RULES_EXCEEDED, sckt, 0xFF, 0xFF, 0xFF);
          return ERROR_RESOURCE_CALCULATION_FAILURE;
        }

        // Adjust TAD limit as needed for memory hole
        tadLimit = socket->SAD[sadIndex].Limit ;
        if( socket->SAD[sadIndex].Limit >= TOLM )  {
          if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0))  {
            tadLimit = socket->SAD[sadIndex].Limit - host->setup.common.lowGap + tadLimitAdjustment ;
          }
        }

        //Create a TAD entry
        imc->TAD[tadIndex].Enable = 1;
        imc->TAD[tadIndex].SADId = sadIndex;
        imc->TAD[tadIndex].Limit = tadLimit ;
        imc->TAD[tadIndex].socketWays = socket->SAD[sadIndex].ways;
        imc->TAD[tadIndex].channelWays = channelWays;
        imc->TAD[tadIndex].mode = socket->SAD[sadIndex].granularity;


        //Update the Offsets for different memory regions for the first SAD of that type.
        for (i = 0; i < MAX_MC_CH; i++) {
         //Reset dpa offset for this channel
         dpaOffset = 0xFFFFFFFF;
         if (chInter & (BIT0 << i)) {
            if(tadIndex < TAD_RULES) {
              dimmNvList = GetDimmNvList(host, sckt, i+(mcIndex*MAX_MC_CH));
              //Parse through the dimms in the channel to find the NVM dimm and fill its Vol/Per Region DPA
              for (dimm = 0; dimm < MAX_DIMM; dimm++) {
                //Skip if DPAOffset is already calculated for this channel
                if(dpaOffset != 0xFFFFFFFF) continue;
                //Skip if dimm slot not populated
                if(((*dimmNvList)[dimm].dimmPresent == 0)) continue;
                //Update the proper DPA of this dimm as TAD offset for TAD 0 in that channel based on memory type.
                switch(socket->SAD[sadIndex].type) {

                  case MEM_TYPE_1LM :
                    if ((*dimmNvList)[dimm].aepDimmPresent)
                      break;
                    dpaOffset = 0;
#ifdef NVMEM_FEATURE_EN
                    if ((socket->NvMemSadIdxBitMap >> sadIndex) & 1) {
                      MemDebugPrint ((host, SDBG_MAX, sckt, (i + (mcIndex * MAX_MC_CH)), dimm, NO_RANK, NO_STROBE,
                                      NO_BIT,"SadIndex %d is NVDIMM Sad...\n", sadIndex));
                      //
                      // If this is the first time this NVDIMM SAD is being mapped, make sure ddr4RemSize is reset.
                      // This is required as some of the 3 way interleaving of RDIMMs will not be equally distributed 
                      // among channels. So when NVDIMM SAD region is decoded, it could wrongly point to RDIMM on the 
                      // channel instead of NVDIMM.Resetting ddr4Remsize to NVmemSize will make sure that TADOffsets
                      // are computed correctly to point to NVDIMMs on the channel.
                      //
                      if (socket->channelList[i + (mcIndex * MAX_MC_CH)].ddr4RemSize >
                           socket->channelList[i + (mcIndex * MAX_MC_CH)].NVmemSize) {
                        socket->channelList[i + (mcIndex * MAX_MC_CH)].ddr4RemSize = 
                         socket->channelList[i + (mcIndex * MAX_MC_CH)].NVmemSize;
                        MemDebugPrint ((host, SDBG_MAX, sckt, (i + (mcIndex * MAX_MC_CH)), dimm, 
                                        NO_RANK, NO_STROBE, NO_BIT,"Adjusted ddr4RemSize = 0x%x.\n", 
                                        socket->channelList[i + (mcIndex * MAX_MC_CH)].ddr4RemSize));
                      }
                    }
#endif // NVMEM-FEATURE_EN
                    channelMemUsed = (socket->channelList[i + (mcIndex * MAX_MC_CH)].memSize - socket->channelList[i + (mcIndex * MAX_MC_CH)].ddr4RemSize) + interSizePerCh + dpaOffset;
                    // Update remaining size on channel
                    socket->channelList[i+(mcIndex*MAX_MC_CH)].ddr4RemSize -= (UINT16) interSizePerCh ;
                    break;

                  case MEM_TYPE_2LM :
                    if ((*dimmNvList)[dimm].aepDimmPresent == 0)
                      break;
                    dpaOffset = (*dimmNvList)[dimm].volRegionDPA;
                    channelMemUsed = ( socket->channelList[i+(mcIndex*MAX_MC_CH)].volSize - socket->channelList[i+(mcIndex*MAX_MC_CH)].volRemSize ) + interSizePerCh + dpaOffset;
                    // Update remaining size on channel
                    socket->channelList[i+(mcIndex*MAX_MC_CH)].volRemSize -= (UINT16) interSizePerCh ;
                    break;

                  case MEM_TYPE_PMEM :
                    if ((*dimmNvList)[dimm].aepDimmPresent == 0)
                      break;
                    //Get the offset (within Persistant range) specified in the interleave request.
                    interReqOffset = GetInterReqOffset(host, sckt, mcIndex, i, dimm, socket->SAD[sadIndex].Limit);
                    dpaOffset = (*dimmNvList)[dimm].perRegionDPA + interReqOffset;
                    //If the system is running in 1LM+PMEM
                    //Find if there is another dimm (only DDR4 in SKX) in this channel that has already been mapped.
                    //If so, find the size of the memory that is already mapped from this channel and add it to the offset
                    //This will ensure that the channel address decoded using the Tad Offset will be correct.
                    if((host->var.mem.volMemMode == VOL_MEM_MODE_1LM) && (socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize > 0) ) {
                      MemDebugPrint((host, SDBG_MAX, sckt, i+(mcIndex*MAX_MC_CH), NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                     " ddr4 memsize: %x   \n", socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize));
                      dpaOffset += socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize;
                      MemDebugPrint((host, SDBG_MAX, sckt, i+(mcIndex*MAX_MC_CH), NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                     " DPA offset: %x  \n", dpaOffset));
                    }
                    channelMemUsed = ( socket->channelList[i+(mcIndex*MAX_MC_CH)].perSize - socket->channelList[i+(mcIndex*MAX_MC_CH)].perRemSize ) + interSizePerCh + dpaOffset;
                    // Update remaining size on channel
                    socket->channelList[i+(mcIndex*MAX_MC_CH)].perRemSize = socket->channelList[i+(mcIndex*MAX_MC_CH)].perRemSize - (UINT16) interSizePerCh ;
                    break;

                  case MEM_TYPE_PMEM_CACHE :
                    if ((*dimmNvList)[dimm].aepDimmPresent == 0)
                      break;
                    //Get the offset (within Persistant range) specified in the interleave request.
                    interReqOffset = GetInterReqOffset(host, sckt, mcIndex, i, dimm, socket->SAD[sadIndex].Limit);
                    dpaOffset = (*dimmNvList)[dimm].perRegionDPA + interReqOffset;
                    channelMemUsed = ( socket->channelList[i+(mcIndex*MAX_MC_CH)].perSize - socket->channelList[i+(mcIndex*MAX_MC_CH)].perRemSize ) + interSizePerCh + dpaOffset;
                    // Update remaining size on channel
                    socket->channelList[i+(mcIndex*MAX_MC_CH)].perRemSize -= (UINT16) interSizePerCh ;
                    break;

                  case MEM_TYPE_BLK_WINDOW :
                    if ((*dimmNvList)[dimm].aepDimmPresent == 0)
                      break;
                    dpaOffset =  DDRT_BLK_WINDOW_DPA_START;
                    //If the system is running in 1LM+PMEM
                    //Find if there is another dimm (only DDR4 in SKX) in this channel that has already been mapped.
                    //If so, find the size of the memory that is already mapped from this channel and add it to the offset
                    //This will ensure that the channel address decoded using the Tad Offset will be correct.
                    if((host->var.mem.volMemMode == VOL_MEM_MODE_1LM) && (socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize > 0) ) {
                      MemDebugPrint((host, SDBG_MAX, sckt, i+(mcIndex*MAX_MC_CH), NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                     " ddr4 memsize: %x   \n", socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize));
                      dpaOffset += socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize;
                      MemDebugPrint((host, SDBG_MAX, sckt, i+(mcIndex*MAX_MC_CH), NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                     " DPA offset: %x  \n", dpaOffset));
                    }
                    channelMemUsed = ( socket->channelList[i+(mcIndex*MAX_MC_CH)].blkWinSize - socket->channelList[i+(mcIndex*MAX_MC_CH)].blkWinRemSize ) + interSizePerCh + dpaOffset;
                    // Update remaining size on channel
                    socket->channelList[i+(mcIndex*MAX_MC_CH)].blkWinRemSize -= (UINT16) interSizePerCh ;
                    break;

                  case MEM_TYPE_CTRL :
                    if ((*dimmNvList)[dimm].aepDimmPresent == 0)
                      break;
                    dpaOffset = DDRT_CTRL_DPA_START;
                    //If the system is running in 1LM+PMEM
                    //Find if there is another dimm (only DDR4 in SKX) in this channel that has already been mapped.
                    //If so, find the size of the memory that is already mapped from this channel and add it to the offset
                    //This will ensure that the channel address decoded using the Tad Offset will be correct.
                    if((host->var.mem.volMemMode == VOL_MEM_MODE_1LM) && (socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize > 0) ) {
                      MemDebugPrint((host, SDBG_MAX, sckt, i+(mcIndex*MAX_MC_CH), NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                     " ddr4 memsize: %x   \n", socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize));
                      dpaOffset += socket->channelList[i+(mcIndex*MAX_MC_CH)].memSize;
                      MemDebugPrint((host, SDBG_MAX, sckt, i+(mcIndex*MAX_MC_CH), NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                     " DPA offset: %x  \n", dpaOffset));
                    }
                    channelMemUsed = ( socket->channelList[i+(mcIndex*MAX_MC_CH)].ctrlSize - socket->channelList[i+(mcIndex*MAX_MC_CH)].ctrlRemSize ) + interSizePerCh + dpaOffset;
                    // Update remaining size on channel
                    socket->channelList[i+(mcIndex*MAX_MC_CH)].ctrlRemSize -= (UINT16) interSizePerCh ;
                    break;
                }
              }
              //Update the TadInterleave list
              imc->TADintList[tadIndex][i] = 1;

              if(socket->SAD[sadIndex].mirrored){
                //Update the TadOffset value
                TotalMemUsed = imc->TAD[tadIndex].socketWays * imc->TAD[tadIndex].channelWays * channelMemUsed;
                if(imc->TAD[tadIndex].Limit > ((TotalMemUsed )/2 ) ) {
                  imc->TADOffset[tadIndex][i] = imc->TAD[tadIndex].Limit - ((TotalMemUsed)/2);
                } else {
                  //There are scenarios in which this value could be negative.
                  imc->TADOffset[tadIndex][i] = ((TotalMemUsed)/2) - imc->TAD[tadIndex].Limit ;

                  //Program 2's compliment of the result if we have DDR4 memory Bit31 not supported in tadchnilvoffset register
                  if (socket->SAD[sadIndex].type == MEM_TYPE_1LM) {
                    imc->TADOffset[tadIndex][i] = (~(imc->TADOffset[tadIndex][i]) + 1) ;
                  } else {
                    imc->TADOffset[tadIndex][i]|= BIT0 << 31;
                  }
                }
              }
              else{
                //Update the TadOffset value
                if(imc->TAD[tadIndex].Limit >= (imc->TAD[tadIndex].socketWays * imc->TAD[tadIndex].channelWays * channelMemUsed ) ) {
                  imc->TADOffset[tadIndex][i] = imc->TAD[tadIndex].Limit - (imc->TAD[tadIndex].socketWays * imc->TAD[tadIndex].channelWays * channelMemUsed );
                } else {
                  //There are scenarios in which this value could be negative.
                  imc->TADOffset[tadIndex][i] = (imc->TAD[tadIndex].socketWays * imc->TAD[tadIndex].channelWays * channelMemUsed ) - imc->TAD[tadIndex].Limit ;
                  if (socket->SAD[sadIndex].type == MEM_TYPE_1LM) {
                    //Program 2's compliment of the result if we have DDR4 memory Bit31 not supported in tadchnilvoffset register
                    imc->TADOffset[tadIndex][i] = (~(imc->TADOffset[tadIndex][i]) + 1) ;
                  } else {
                    //Set BIT31 if this is a negative value.
                    imc->TADOffset[tadIndex][i]|= BIT0 << 31;
                  }

                  imc->TADOffset[tadIndex][i]|= BIT0 << 31;
                }
              }
              //Calculate the Ch_idx_offset for each of the channel
              //Ch_idx_offset = (TAD[N].BASE / TAD[N].TAD_SKT_WAY) % TAD[N].TAD_CH_WAY
              if(sadIndex > 0)
                imc->TADChnIndex[tadIndex][i] = ((socket->SAD[sadIndex-1].Limit / imc->TAD[tadIndex].socketWays) % imc->TAD[tadIndex].channelWays);
              else
                imc->TADChnIndex[tadIndex][i] = 0;
            }
            else  {
              MemDebugPrint((host, SDBG_MINMAX, sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "TAD rules exeeds %d\n", TAD_RULES));
              OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_TAD_RULES_EXCEEDED, sckt, 0xFF, 0xFF, 0xFF);
              return ERROR_RESOURCE_CALCULATION_FAILURE;
            }
          } else {
              imc->TADOffset[tadIndex][i] = 0;
              imc->TADChnIndex[tadIndex][i] = 0;
          }
        }//i
        tadIndex++;
      }//mc
    }//SAD
  }//socket

  return SUCCESS;
} // TADInterleave

UINT8
FindSadIndex(
  PSYSHOST  host,
  UINT8 sckt)
/**

  Description: Find the next SAD entry that needs to be filled for this Socket.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id


  @retval Index of Next available SAD entry

**/
{
  struct  Socket *socket;
  UINT8 i, sadIndex;
  UINT8 maxSADRules = MAX_SAD_RULES;

 //scan through the SAD table and find the next SAD entry with Enable =0
  socket = &host->var.mem.socket[sckt];
  for( i=0; i<maxSADRules; i++) {
    if (socket->SAD[i].Enable ==0){
      sadIndex = i;
      return sadIndex;
    }
  }
  return 0xff;
}

/**

  Description: sort the channels in the socket based on the remsize value in descending order.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id
  @param chIndex - Index of first channel in the IMC that needs to be sorted
  @param channelSortList - structure to hold the sorted entries

  @retval sortnum - number of channels left with unallocated memory in the IMC

**/
UINT8
ChannelSort(
  PSYSHOST host,
  UINT8 socketNum,
  UINT8 chIndex,
  struct channelSort channelSortList[]
  )
{
  UINT8 i;
  UINT8 ch;
  UINT8 si;
  UINT8 sortNum;
  UINT16 remSize;
  UINT8 lowMemChannelCheckedFlag;
  UINT8 SortedListStartIndex;

  sortNum = 0;
  lowMemChannelCheckedFlag = 0;


 //
 // Special DFX case: if ch interleave = 1, allocate specified channel first
 //
  if((host->var.mem.chInter == 1) && (host->setup.mem.dfxMemSetup.dfxLowMemChannel != 0)) {
    ch = host->setup.mem.dfxMemSetup.dfxLowMemChannel - 1;
    remSize = host->var.mem.socket[socketNum].channelList[ch].remSize;

    if (remSize != 0){

      // Insert channel specified by lowMemChannel at index 0 in sorted list
      channelSortList[0].chNum = ch - chIndex;
      channelSortList[0].socketNum = socketNum;
      channelSortList[0].chSize = remSize;
      lowMemChannelCheckedFlag = 1;
      sortNum++;
    }
  }

  for (ch=chIndex; ch<(chIndex + MAX_MC_CH); ch++)
  {
    if ((lowMemChannelCheckedFlag == 1) && (ch == host->setup.mem.dfxMemSetup.dfxLowMemChannel - 1)) continue;

    remSize = host->var.mem.socket[socketNum].channelList[ch].remSize;
    if (remSize == 0) continue;

    if(lowMemChannelCheckedFlag == 1) SortedListStartIndex = 1;
    else SortedListStartIndex = 0;

    // Determine sort list position
    for (si = SortedListStartIndex; si < sortNum; si++) {
      if (channelSortList[si].chSize < remSize)
        break;
    }

    // Shift smaller channels towards end of list
    if (si < sortNum) {
      for (i = sortNum; i > si; i--) channelSortList[i] = channelSortList[i - 1];
    }

    // Insert current channel
    channelSortList[si].chNum = ch - chIndex;
    channelSortList[si].socketNum = socketNum;
    channelSortList[si].chSize = remSize;

    sortNum++;
  } // Channel Loop

  return sortNum;
}


/**

  Description: Add a SAD entry in  socket that are not involved in the SAD interleave.

  @param host  - Pointer to sysHost
  @param map  - map of all the IMCs that are part of the interleave.
  @param nodeLimit - Limit value for the SAD entry

  @retval status

**/
UINT32
AddRemoteSadEntry (
              PSYSHOST  host,
              UINT16  map[MAX_SOCKET * MAX_IMC],
              UINT32  nodeLimit,
              UINT8   type
)

{
  struct Socket  *socket;
  UINT8  socketNum, sadIndex, i, socketsInInterleaveList, tgtGranularity, SADMerged = 0, mc;
  BOOLEAN merge = 0;
  UINT16 socketFlag, mapFlag;
  struct  socketNvram  *socketNv;
  UINT8   maxSADRules = MAX_SAD_RULES;

  mapFlag = 0;
  socketFlag = 0;
  socketsInInterleaveList = 0 ;

  //create remote SAD entries for the other sockets
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
    socketNv = &host->nvram.mem.socket[socketNum];
    // Skip if socket not enabled
    if (socketNv->enabled == 0) continue;
    socket = &host->var.mem.socket[socketNum];

    if (map[socketNum* host->var.mem.maxIMC] || map[(socketNum* host->var.mem.maxIMC) +1])   continue;

    //Find the Index for next available SAD entry
    sadIndex = FindSadIndex(host, socketNum);
    // Should not reach end of SAD rules before allocating all memory, Issue warning and break TAD loop.
    // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
    if (sadIndex >= maxSADRules) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "SAD rules exceeds %d\n", maxSADRules));
      OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
      return ERROR_RESOURCE_CALCULATION_FAILURE;
    }
    merge = 1;
    //Check if the previous entry has the same interleave target list
    for(i=0; i<MAX_SOCKET* host->var.mem.maxIMC; i=i+ host->var.mem.maxIMC) {
      if(sadIndex == 0) break;

      // Does previous SAD entry include socket[i]?
      if( socket->SADintList[sadIndex-1][i] || socket->SADintList[sadIndex-1][i+1] )
        socketFlag = TRUE ;
      else
        socketFlag = FALSE ;

      // Does new SAD entry include socket[i]?
      if( map[i] || map[i+1] )
        mapFlag= TRUE ;
      else
        mapFlag = FALSE ;

      // If the previous SAD entry and the new SAD entry do not match, then cannot merge
      if( !(((socketFlag==0) && (mapFlag==0)) || ((socketFlag!=0) && (mapFlag!=0))) ){
        merge = 0;
        break;
      }
      // Keep track of the numebr of sockets in the interleave list
      if( socketFlag == TRUE )  {
        socketsInInterleaveList++;
      }
    }

    // Determine target granularity of new entry
    tgtGranularity = GetTargetInterleaveGranularity( host, type ) ;

    //Check if the previous entry has the same interleave target granularity if there is more than one socket in the interleave list
    if( ( merge == 1 ) && ( socketsInInterleaveList > 1 ) && ( sadIndex !=0 ) )  {
      if( tgtGranularity != socket->SAD[sadIndex-1].tgtGranularity )  {
        merge = 0 ;
      }
    }

    //merge the entry with the previous SAD entry if the previous entry is also a remote entry
    if(sadIndex > 0) {
      if (merge == 1 && socket->SAD[sadIndex-1].local == 0) {
        socket->SAD[sadIndex-1].Limit = nodeLimit;
        SADMerged = 1;
      }
    }
    if (SADMerged == 0) {
      //create remote SAD entry
      socket->SAD[sadIndex].Enable = 1;
      socket->SAD[sadIndex].Limit = nodeLimit;
      socket->SAD[sadIndex].ways  = 0;
      for (mc = 0; mc < host->var.mem.maxIMC; mc++){
        socket->SAD[sadIndex].FMchannelInterBitmap[mc] = 0;
        socket->SAD[sadIndex].channelInterBitmap[mc] = 0;
      }
      socket->SAD[sadIndex].imcInterBitmap =  0;
      socket->SAD[sadIndex].local =  0;
      socket->SAD[sadIndex].type = 0;
      socket->SAD[sadIndex].granularity = 0;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].tgtGranularity = tgtGranularity ;


      //Create SADINTlist for this SAD entry
      for(i=0; i<MAX_SOCKET* host->var.mem.maxIMC;i++)
        //Update the SAD Interleave list
        if(map[i]) socket->SADintList[sadIndex][i] = 1; //host->nvram.mem.socket[i].imc[0].haNodeId;
    }
  }//socket
  return SUCCESS;
}


UINT32
DoChannelInterleave(
  PSYSHOST         host,
  MEMORY_MAP_DATA  *MemMapData,
  UINT8            socketNum,
  UINT16           TOLM,
  UINT8            memType,
  UINT32           *nodeLimit,
  UINT8            below4GBInterFlag
  )
/**

  Description: Interleave the memory between the channels within an IMC of a socket

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum  - socket id
  @param TOLM - Top Of Low Memory 
  @param memType   - Memory type  
  @param nodeLimit - Limit value for the SAD entry  
  @param below4GBInterFlag - Flag to check whether we are interleaving above or
  below TOLM

  @retval status
**/

{

  UINT8  i, mc, channelIndex, sadIndex, remainingEntries, interleaveWays, channelInterBitmap[MAX_MC], FMchannelInterBitmap[MAX_MC];
  UINT16 interleaveSize;
  UINT32  status = SUCCESS;
  UINT16  map[MAX_SOCKET * MAX_IMC] ={0};
  struct ddrChannel *channelList;
  struct Socket  *socket;
  struct channelSort channelSortList[MAX_MC_CH] = {{0}};
  UINT8   maxSADRules = MAX_SAD_RULES;
  UINT32 MemoryToBeAllocated = 0;
  UINT8 McList[MAX_SOCKET*MAX_IMC] = {0};

  sadIndex = 0;
  socket = &host->var.mem.socket[socketNum];
  channelList = &host->var.mem.socket[socketNum].channelList[0];

  for(mc=0; mc< host->var.mem.maxIMC; mc++){
    *nodeLimit = 0;
    channelIndex = mc * MAX_MC_CH ;
    interleaveWays = host->var.mem.chInter;
    do {
      channelInterBitmap[mc] =0;
      FMchannelInterBitmap[mc] =0;
      //Sort channels based on the remsize
      remainingEntries = ChannelSort(host, socketNum, channelIndex, channelSortList);
      if (remainingEntries) {
        //Calculate the proper Interleave ways.
        while (interleaveWays > remainingEntries) {
          interleaveWays -= 1;
        }
        //Calculate the interleave size.
        if ((interleaveWays < 1) || (interleaveWays > MAX_MC_CH)){
          RC_ASSERT(interleaveWays >= 1 || interleaveWays <= MAX_MC_CH, ERR_RC_INTERNAL, RC_ASSERT_MINOR_CODE_36);
          continue;
        }
        if ((host->setup.mem.options & SPLIT_BELOW_4GB_EN) &&(below4GBInterFlag == 1)){
          if(TOLM % interleaveWays){
            interleaveSize = (TOLM / interleaveWays);
            interleaveSize = interleaveSize + (interleaveWays - (TOLM % interleaveWays));
          }
          else
            interleaveSize = (TOLM / interleaveWays);
        }
        else
        {
          if ((interleaveWays -1) >= MAX_MC_CH) {
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                          "\n\n\n!!!!FATAL ERROR -- channelSortList[interleaveWays -1 ]Array out of bounds!! (interleaveWays -1) with value of: 0x%X\n\n\n", (interleaveWays -1)));
#if SMCPKG_SUPPORT
            OutputError (host, ERR_NGN, NGN_ARRAY_OUT_OF_BOUNDS, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
            FatalError(host, ERR_NGN, NGN_ARRAY_OUT_OF_BOUNDS);
#endif
          }
          interleaveSize = channelSortList[interleaveWays -1 ].chSize;
        }

        //Find the Index for next available SAD entry
        sadIndex = FindSadIndex(host, socketNum);
        // Should not reach end of SAD rules before allocating all memory, Issue warning and break TAD loop.
        // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
        if (sadIndex >= maxSADRules) {
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
            "SAD rules exceeds %d\n", maxSADRules));
          OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
          return ERROR_RESOURCE_CALCULATION_FAILURE;
        }

        //Subtract the interleave size from each channel
        for(i=0;  i<interleaveWays; i++){
          channelSortList[i].chSize -= interleaveSize;
          switch(memType) {
            case MEM_TYPE_1LM :
              channelInterBitmap[mc] = channelInterBitmap[mc] | (BIT0<<channelSortList[i].chNum);
              break;
            case MEM_TYPE_2LM :
            case MEM_TYPE_PMEM :
            case MEM_TYPE_PMEM_CACHE :
            case MEM_TYPE_BLK_WINDOW :
            case MEM_TYPE_CTRL :
              FMchannelInterBitmap[mc] = FMchannelInterBitmap[mc] | (BIT0<<channelSortList[i].chNum);
              break;
          }
          channelList[channelSortList[i].chNum + channelIndex].remSize-=interleaveSize;
        }

        //Calculate the Channel interleave bitmap based on Near Memory population for 2LM mode.
        if((memType == MEM_TYPE_2LM) ||  (memType == MEM_TYPE_PMEM_CACHE)){
          channelInterBitmap[mc] = FindNMBitMap(host, socketNum, mc);
        }

        //Create a SAD entry in the host structure
        if(sadIndex)
          socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + (interleaveWays * interleaveSize);
        else

          if ((host->setup.mem.options & SPLIT_BELOW_4GB_EN) &&(below4GBInterFlag == 1))
            socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
          else
          {
            socket->SAD[sadIndex].Limit = interleaveWays * interleaveSize;
          }

        *nodeLimit = socket->SAD[sadIndex].Limit;

        if (*nodeLimit != 0) {
          // find out what all MC are participating/populated in the socket
          McList[(socketNum *  host->var.mem.maxIMC)+mc] = 1;
        }

        //Adjust SAD Limit for Memory hole
        if(socket->SAD[sadIndex].Limit >=TOLM) {
          if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0)){
            socket->SAD[sadIndex].Enable = 1;
            socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
            socket->SAD[sadIndex].ways  = 1;
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap[mc];
            socket->SAD[sadIndex].imcInterBitmap =  mc + 1;
            socket->SAD[sadIndex].type = memType;
            socket->SAD[sadIndex].local = 1;
            socket->SAD[sadIndex].mirrored = 0;
            socket->SAD[sadIndex].Attr = 0;
            socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

            if (IsSkuLimitViolation(host, MemMapData, TOLM, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
              OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
              FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
            }
            switch(memType) {
              case MEM_TYPE_1LM :
                socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
                break;
              case MEM_TYPE_2LM :
                socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap[mc];
                break;
              case MEM_TYPE_PMEM :
              case MEM_TYPE_PMEM_CACHE :
              case MEM_TYPE_BLK_WINDOW :
              case MEM_TYPE_CTRL :
                MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                               "INVALID CONFIGURATION. The first SAD rule cannot be PMEM, PMEM$, BLK or CTRL region\n"));
#if SMCPKG_SUPPORT
                OutputError (host, ERR_NGN, NGN_PMEM_CONFIG_ERROR, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
                FatalError(host, ERR_NGN, NGN_PMEM_CONFIG_ERROR);
#endif
                break;
            }
            socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + mc] = 1;

            //if SPLIT_BELOW_4GB_EN is set then stop interleaving across sockets at 4GB
            if (host->setup.mem.options & SPLIT_BELOW_4GB_EN){
              return SUCCESS;
            }

            *nodeLimit += FOUR_GB_MEM - TOLM;

            // If interleaving across three channels, then we need to check the TOLM boundary
            if( interleaveWays == 3 )  {

              // If TOLM boundary is not evenly divisible when interleaving across three channels, then we need to adjust the node limit so that it divides evenly
              if( TOLM % interleaveWays ) {

                //5332821: Partial Mirroring with 3Chway population causes Memicals data miscompare
                if ((host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM)||(host->nvram.mem.RASmode & PARTIAL_MIRROR_2LM)) {
                    interleaveWays = interleaveWays*2;
                }

                // TOLM boundary is a multiple of 64MB less than a boundary evenly divisible by totalWays, so adjust nodeLimit by that multiple in 64MB units, so that it divides evenly
                *nodeLimit -=  interleaveWays - ( TOLM % interleaveWays ) ;

              }
            }

            sadIndex++;
          }
        }

        if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN ) {
          if ((sadIndex > 0) && (sadIndex < MAX_SAD_RULES)) {
            if(socket->SAD[sadIndex -1].Limit == FOUR_GB_MEM) {
              // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
              socket->SAD[sadIndex].Limit = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
              socket->SAD[sadIndex].Enable = 1;
              socket->SAD[sadIndex].Attr = SAD_NXM_ATTR;
              socket->SAD[sadIndex].local = 0;
              sadIndex++;
              *nodeLimit = (*nodeLimit + socket->SAD[sadIndex-1].Limit) - socket->SAD[sadIndex-2].Limit;
            }
          }
        }

        if ( sadIndex < MAX_SAD_RULES)
          socket->SAD[sadIndex].Enable = 1;

        if ( *nodeLimit > host->var.mem.mmiohBase ) {
           if ((sadIndex > 0) && (socket->SAD[sadIndex - 1].Limit == host->var.mem.mmiohBase )) {
              MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                               "WARNING!!! Failure to create Sad%d! Sad%d limit is MMIOHBase...\n", sadIndex, sadIndex - 1 ));
              return FAILURE;
           }
           *nodeLimit = host->var.mem.mmiohBase;
           MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                   "WARNING!!! SAD%d limit exceeds MMIOHBase! Sad%d is truncated to MMIOHBase...\n", sadIndex, sadIndex));
        }
        if ( sadIndex < MAX_SAD_RULES) {
          //OutSncGbAlignRequired = 1(enabled) AND 1LM or 2LM
          if ((host->var.kti.OutSncGbAlignRequired == 1) && ((memType == MEM_TYPE_1LM) || (memType == MEM_TYPE_2LM)) ) {
            *nodeLimit = (*nodeLimit & GB_BOUNDARY_ALIGN); //align to GB boundary

            // If 3-way interleave, then this SAD limit may need to be rounded
            if ( interleaveWays == 3 ) {

              // Check size of memory mapped by this entry to insure that all three channels contributes equally
              while ((*nodeLimit - socket->SAD[sadIndex - 1].Limit) % 3) {
                // Each channel did not have an equal contribution, so try next GB boundary
                *nodeLimit = *nodeLimit - 0x10;
              }
            }
          }
          
          if (sadIndex) {
            MemoryToBeAllocated = *nodeLimit - socket->SAD[sadIndex - 1].Limit;
          }

          if ( IsSkuLimitViolation(host, MemMapData, MemoryToBeAllocated, memType, McList) == TRUE) {
            // DRAM rule not created, so clear the enable flag
            socket->SAD[sadIndex].Enable = 0;
#if SMCPKG_SUPPORT
            OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
            FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
          }

          socket->SAD[sadIndex].Limit = *nodeLimit;
          socket->SAD[sadIndex].ways  = 1;
          socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap[mc];
          socket->SAD[sadIndex].type = memType;
          socket->SAD[sadIndex].imcInterBitmap =  mc + 1;
          socket->SAD[sadIndex].local = 1;
          socket->SAD[sadIndex].mirrored = 0;
          socket->SAD[sadIndex].Attr = 0;
          socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );
          switch(memType) {
            case MEM_TYPE_1LM :
              socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = 0;
              break;
            case MEM_TYPE_2LM :
              socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap[mc];
              break;
            case MEM_TYPE_PMEM :
              socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap[mc];
              break;
            case MEM_TYPE_PMEM_CACHE :
              socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap[mc];
              break;
            case MEM_TYPE_BLK_WINDOW :
            case MEM_TYPE_CTRL :
              socket->SAD[sadIndex].granularity = 0;
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap[mc];
              break;
          }
          // Create Interleave List for this MC
          socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + mc] = 1;
        }
      }
    }while(remainingEntries);

    if(*nodeLimit !=0){
      //Create the proper imc target map
      //For IMCs belonging to the current socket, set the target
      map[(socketNum * host->var.mem.maxIMC)+mc] = 1;

      //create remote SAD entries for the other sockets
      status = AddRemoteSadEntry(host, map, *nodeLimit, memType);
      if(status != SUCCESS) return status;
    }
  }//mc
  return SUCCESS;
}


/**

  Description: This routine check validaty of the imc ways and adjusts it to the nearest valid IMC ways.

  @param host  - Pointer to sysHost
  @param map   -  structure to hold the interleaveable size in each participating imc
  @param counter  -  Number of imcs capable of participating in the interleave.
  @retval counter - number of imcs that can participate in this interleave

**/
UINT8
AdjustSADWays(
  PSYSHOST host,
  UINT16 map[MAX_IMC * MAX_SOCKET],
  UINT8 counter
  )
{
  UINT8  mc, diffCount, socketNum, imcIndex, sockIndex;
  struct  socketNvram  *socketNv;
  UINT16  imcInterSize;
  imcInterSize = 0xffff;
  imcIndex = 0;
  sockIndex = 0;
  diffCount=0;
  //Steps:
  //1. Adjust the counter variable to meet the nearest valid IMC interleave ways (2,4,8)
  //2. Calculate the interleave size based on the smallest interleaveable size of every IMC
  //3. Create a new sad in the participating sockets
  //4. Update the SAD interleave list

  //Check for valid Interleave ways and store the difference in diffCount
  while((counter != 0) && (! IsPowerOfTwo(counter))){
    counter --;
    diffCount++;
  };

  //Adjust the map based on the variable diffCount
  //If there is a socket with both the IMCs participating in the interleave, remove the imc with lowest possible Interleave size.
  for(mc=0; mc< (MAX_SOCKET* host->var.mem.maxIMC); mc+= host->var.mem.maxIMC) {
    if(diffCount==0) break;
    if(map[mc] && map[mc+1]){
      if(map[mc] < map[mc+1])
        map[mc] = 0;
      else
        map[mc+1] = 0;
      diffCount--;
    }
  }
  //Adjust the map based on the variable diffCount
  //Next, remove the imc from the whole map with the lowest possible interleave size.
  while(diffCount){
    for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
      socketNv = &host->nvram.mem.socket[socketNum];
      // Skip if socket not enabled
      if (socketNv->enabled == 0) continue;

      for(mc =0; mc< host->var.mem.maxIMC; mc++) {
        //skip mc that is not populated in all the channels.
        if(map[(socketNum* host->var.mem.maxIMC)+mc] == 0) continue;

        if(imcInterSize > map[(socketNum* host->var.mem.maxIMC)+mc] ){
          imcInterSize = map[(socketNum* host->var.mem.maxIMC)+mc];
          imcIndex = mc;
          sockIndex = socketNum;
        }
      }//mc
    }//socket
    map[(sockIndex *  host->var.mem.maxIMC)+imcIndex] = 0;
    diffCount--;
  };
  return counter;
}


/**

  Description: This routine fills the 2LM matrix below based on the near memory population

---------------------------------------------------------------------------------------------------------
  NM population/IMC  ||   IMC0 |  IMC1 |  IMC2 |  IMC3 |  IMC4 |  IMC5 |  IMC6 |  IMC7  ||   Counter
---------------------------------------------------------------------------------------------------------
                     ||        |       |       |       |       |       |       |        ||
  0 0 1              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  0 1 0              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  0 1 1              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 0 0              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 0 1              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 1 0              ||        |       |       |       |       |       |       |        ||
                     ||        |       |       |       |       |       |       |        ||
  1 1 1              ||        |       |       |       |       |       |       |        ||
---------------------------------------------------------------------------------------------------------

  @param host  - Pointer to sysHost
  @param matrix   -  structure to hold the above matrix
  @param counter   - Structure that holds the number of imcs that could participate in the interleave
  @param ddrtChInterleave  -  NVMDIMM channel interleave that is being verified
  @retval counter - number of imcs that can participate in this interleave

**/
UINT8
FillMatrix_2LM(
  PSYSHOST host,
  UINT16 matrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8 counter[MAX_TWO_LM_COMBINATION],
  UINT8 ddrtChInterleave
  )
{
  struct ddrChannel *channelList;
  UINT8 NM_Population, ch, socketNum, mc,channelIndex, optimalInterIndex, i, skipMC=0;
  UINT16  interleavableSize, totalInterleaveSize[MAX_TWO_LM_COMBINATION];
  UINT8 ChInterleave[3]={0};

  for(ch =0; ch<MAX_MC_CH; ch++) {
  ChInterleave[ch] = (ddrtChInterleave >> ch) & 0x01;
  }
   //STEP 1:  Check if FM interleave Ways is valid.
  //Parse through each socket/each imc and increase the counter for every IMC with all channels populated
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
    channelList = &host->var.mem.socket[socketNum].channelList[0];
    for(mc =0; mc< MAX_IMC; mc++) {
      interleavableSize = 0xffff;
      channelIndex = mc * MAX_MC_CH ;

      skipMC = 0;
      // skip if  channels are not populated as expected in ChInterleave[] for NVMDIMM.
      for(ch=0; ch< MAX_MC_CH; ch++){
      if(ChInterleave[ch])//{
      if(channelList[channelIndex + ch].remSize ==0){ skipMC = 1;}
      //}else{
      //  if(channelList[channelIndex + ch].remSize !=0){ skipMC = 1;}
      //}
      }
      if (skipMC == 1)
        continue;

      //STEP2: for each MC with NVM population successful, look at the NM population and update the table accordingly
      //Check for NM population and fill the interleaveable size in the map accordingly
      NM_Population = FindNMBitMap(host, socketNum, mc);
      //If NM population is zero skip the IMC
      if (!NM_Population) continue;

      // calculate the interleave size
      for(ch=channelIndex; ch<(channelIndex + MAX_MC_CH); ch++){
        // Make sure we don't skipMc out of bounds of array in for loop
        if(ch > (mc * MAX_MC_CH)){
          //Skip the channels that has memory but are not part of the interleave combination
          if(!ChInterleave[ch-(mc * MAX_MC_CH)]) continue;

          if ((channelList[ch].remSize != 0) && (channelList[ch].remSize < interleavableSize))
              interleavableSize = channelList[ch].remSize;
        }
      }
      //Fill the interleavable size for this imc in the corresponding place in the Matrix above based on the NM population
      matrix[NM_Population-1][(socketNum *  host->var.mem.maxIMC) + mc] = interleavableSize;
      counter[NM_Population-1]++;
    }  //mc
  } //socketNum

  //Print out the structure
  //for(i=0; i< MAX_TWO_LM_COMBINATION; i++){
  //  for(mc =0; mc< (MAX_SOCKET * host->var.mem.maxIMC); mc++) {
  //    MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%d\t", matrix[i][mc]));
  //  }
  //MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Counter: %d", counter[i]));
  //    MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  //}

  //STEP 3: Adjust the matrix generated based on valid SAD ways
  for(i=0; i< MAX_TWO_LM_COMBINATION; i++)
    counter[i] = AdjustSADWays(host,matrix[i] , counter[i]);

    //Print out the structure
  //for(i=0; i< MAX_TWO_LM_COMBINATION; i++){
  //  for(mc =0; mc< (MAX_SOCKET * host->var.mem.maxIMC); mc++) {
  //    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%d\t", matrix[i][mc]));
  //  }
  //MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Counter: %d", counter[i]));
  //    MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  //}

  //STEP:4 Find the optimal interleave index in the matrix that gives the NM population, that results in maximum memory interleave
  optimalInterIndex = 0;
  for(i=0; i<MAX_TWO_LM_COMBINATION; i++) {
    totalInterleaveSize[i] = 0;
    // skip if there are no MCs in this NM population
    if(!counter[i]) continue;

    interleavableSize = 0xffff;

    //Find the smallest interleavable size for this combination.
    for(mc =0; mc< (MAX_SOCKET * host->var.mem.maxIMC); mc++) {
       if ((matrix[i][mc] != 0) && (matrix[i][mc] < interleavableSize))
          interleavableSize = matrix[i][mc];
    }
    totalInterleaveSize[i] = interleavableSize * counter[i];

    //Find if this combination interleaves more memory than the previous combinations
  if(totalInterleaveSize[i] > totalInterleaveSize[optimalInterIndex])
      optimalInterIndex = i;
  //If they interleave equal amount of memory choose the one with maximum imc ways.
  if(totalInterleaveSize[i] == totalInterleaveSize[optimalInterIndex]){
      if(counter[i] > counter[optimalInterIndex])
    optimalInterIndex = i;
  }
  }


  return optimalInterIndex;
}

/**

  Description: This routine check if three way UMA interleave is possible

  @param host  - Pointer to sysHost
  @param matrix   -  structure to hold the interleaveable size in each participating imc

  @retval counter - number of imcs that can participate in this interleave

**/
UINT8
IsThreeWayInterleave(
  PSYSHOST host,
  UINT8 memType,
  UINT16 matrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8 *Index
  )
{
  UINT8  socketNum, channelIndex, mc, ch,  i;
  UINT16 interleavableSize = 0xffff;
  struct ddrChannel *channelList;
  //struct  socketNvram  *socketNv;
  UINT8  twoLMCounter[MAX_TWO_LM_COMBINATION]={0}, counter = 0, ddrtChInterleave;

  //diffCount = 0;
  //imcInterSize = 0xffff;
  //sockIndex =0;
 // imcIndex=0;
  ddrtChInterleave = 0x7;
  //Clear the matrix array
  for(i=0; i< MAX_TWO_LM_COMBINATION; i++){
    for(mc =0; mc< (MAX_SOCKET * host->var.mem.maxIMC); mc++) {
      matrix[i][mc] = 0;
    }
  }
  //In case of 2lm check if near memory population matches between IMCs
  if(memType == MEM_TYPE_2LM || memType == MEM_TYPE_PMEM_CACHE){
    *Index = FillMatrix_2LM(host, matrix, twoLMCounter, ddrtChInterleave);
    counter = twoLMCounter[*Index];
  }
  else {
    *Index=0;
    //Fill the map variable with the smallest possible interleave size in each imc (for every IMC that has all the channels populated)
    //Increment the counter variable for every IMC with three channels populated with memory.

    //Parse through each socket/each imc and increase the counter for every IMC with all channels populated
    for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
      channelList = &host->var.mem.socket[socketNum].channelList[0];
      for(mc =0; mc< host->var.mem.maxIMC; mc++) {
        interleavableSize = 0xffff;
        channelIndex = mc * MAX_MC_CH ;
        matrix[*Index][(socketNum *  host->var.mem.maxIMC) + mc] = 0;

        // skip if all three channels are not populated.
        if((channelList[channelIndex].remSize == 0) || (channelList[channelIndex+1].remSize == 0)  || (channelList[channelIndex+2].remSize == 0) )  continue;

        //Calculate the interleave size
        for(ch=channelIndex; ch<(channelIndex + MAX_MC_CH); ch++){
           if (channelList[ch].remSize < interleavableSize)
              interleavableSize = channelList[ch].remSize;
        }
        matrix[*Index][(socketNum *  host->var.mem.maxIMC) + mc] = interleavableSize;
        counter++;
      }  //mc
    } //socketNum

    counter = AdjustSADWays(host, matrix[*Index], counter);
  }

  return counter;
}

UINT32
DoThreeWayUMAInterleave(
  PSYSHOST         host,
  MEMORY_MAP_DATA  *MemMapData,
  UINT16           TOLM,
  UINT8            memType,
  UINT16           matrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8            Index,
  UINT8            counter,
  UINT32           *nodeLimit
  )
/**

  Description: This routine check if three way UMA interleave is possible

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM  - Top of Low Memory 
  @param memType  - Type of memory region to be mapped
  @param matrix   -  structure that holds the interleaveable size in each participating imc
  @param counter - number of imcs that can participate in this interleave
  @param nodeLimit - Pointer to return the limit of SAd entry created

  @retval status

**/
{
  UINT8  channelIndex, sadIndex, socketNum, mc, chnum,i, channelInterBitmap, totalWays;
  UINT8  imcMap[MAX_SOCKET] = {0};
  UINT16 interleaveSize;
  UINT32 TotalInterleaveSize;
  struct ddrChannel *channelList;
  struct Socket  *socket;
  struct  socketNvram  *socketNv;
  UINT8 McList[MAX_SOCKET*MAX_IMC] = {0};

  channelInterBitmap = BITMAP_CH0_CH1_CH2;
  interleaveSize = 0xffff;
  *nodeLimit = 0;
  //Calculate the Interleave size
  //Parse through each socket/each imc to find the interleave size
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
      //clear the imc bit matrix for this socket
    imcMap[socketNum] = 0;

    socketNv = &host->nvram.mem.socket[socketNum];
    // Skip if socket not enabled
    if (socketNv->enabled == 0) continue;

    channelList = &host->var.mem.socket[socketNum].channelList[0];

    for(mc =0; mc< host->var.mem.maxIMC; mc++) {

      //skip mc that is not part of the interleave.
      if(matrix[Index][(socketNum* host->var.mem.maxIMC)+mc] == 0) continue;
      channelIndex = mc * MAX_MC_CH ;

      for (chnum=channelIndex; chnum<channelIndex + MAX_MC_CH; chnum++){
        if(channelList[chnum].remSize ==0) continue;

        if (host->setup.mem.options & SPLIT_BELOW_4GB_EN){
          if(TOLM %(counter * THREE_WAY)){
           interleaveSize = (TOLM / (counter * THREE_WAY));
           interleaveSize = interleaveSize + (THREE_WAY - (TOLM %(THREE_WAY)));
          }
          else
            interleaveSize = (TOLM / (counter * THREE_WAY));
        }
        else
        {
          if(interleaveSize >  channelList[chnum].remSize)
            interleaveSize = channelList[chnum].remSize;
        }
      }//ch

      //Update imcMap structure for this socket
      imcMap[socketNum]|=(BIT0<<mc);
    }//mc
  }//socketNum


  TotalInterleaveSize = interleaveSize * counter * THREE_WAY;

  // find out what all MC are participating/populated in all the sockets
  for (i = 0; i<MAX_SOCKET; i++){
	if (imcMap[i] & BIT0)
      McList[i *  host->var.mem.maxIMC] = 1;
	if (imcMap[i] & BIT1)
	  McList[(i *  host->var.mem.maxIMC) + MC1] = 1;
  }

  if (IsSkuLimitViolation(host, MemMapData, TotalInterleaveSize, memType, McList) == TRUE ) {
#if SMCPKG_SUPPORT
    OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
#else
    FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);      
#endif
  }

  //Parse through each socket to create a SAD
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
    socket = &host->var.mem.socket[socketNum];
    channelList = &host->var.mem.socket[socketNum].channelList[0];

    //Create the imc bit map for this socket and subtract the interleave size from the channel size
    for(mc =0; mc< host->var.mem.maxIMC; mc++) {
      //skip mc that is not part of the interleave.
      if(matrix[Index][(socketNum* host->var.mem.maxIMC)+mc] == 0) continue;
      channelIndex = mc * MAX_MC_CH ;
      for (chnum=channelIndex; chnum<channelIndex + MAX_MC_CH; chnum++){
          channelList[chnum].remSize -= interleaveSize;
      }

      //Calculate the Channel interleave bitmap based on Near Memory population for 2LM mode.
      if(memType == MEM_TYPE_2LM  || memType == MEM_TYPE_PMEM_CACHE) {
        channelInterBitmap = Index + 1;
      }
    }//mc

    //Skip the sockets that is not participating in the interleave
    if((matrix[Index][socketNum *  host->var.mem.maxIMC] == 0) && (matrix[Index][(socketNum *  host->var.mem.maxIMC)+1] == 0)) continue;

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

    //Fill the SAD entry
    if(sadIndex)
      socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + TotalInterleaveSize;
    else{
      //If SPLIT_BELOW_4GB_EN bit is set and sadIndex is 0 then set SAD limit to 4GB
      //Instead of max interleave size
      if (host->setup.mem.options & SPLIT_BELOW_4GB_EN)
        socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
      else
      {
        socket->SAD[sadIndex].Limit = TotalInterleaveSize;
      }
    }

    *nodeLimit = socket->SAD[sadIndex].Limit;

    //Create a new SAD if TOLM is breached
    if(socket->SAD[sadIndex].Limit >=TOLM) {
      if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0)){
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].ways  = counter;
        socket->SAD[sadIndex].type = memType;
        socket->SAD[sadIndex].imcInterBitmap =  imcMap[socketNum];
        socket->SAD[sadIndex].local = 1;
        socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
        socket->SAD[sadIndex].mirrored = 0;
        socket->SAD[sadIndex].Attr = 0;
        socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

        switch(memType) {
          case MEM_TYPE_1LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].channelInterBitmap[mc] =BITMAP_CH0_CH1_CH2 ;
              }
            }
            break;
          case MEM_TYPE_2LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
                socket->SAD[sadIndex].channelInterBitmap[mc] =channelInterBitmap ;
              }
            }
            break;
          case MEM_TYPE_PMEM :
          case MEM_TYPE_PMEM_CACHE :
          case MEM_TYPE_BLK_WINDOW :
          case MEM_TYPE_CTRL :
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "INVALID CONFIGURATION. The first SAD rule cannot be PMEM, PMEM$, BLK or CTRL region\n"));
#if SMCPKG_SUPPORT
            OutputError (host, ERR_NGN, NGN_PMEM_CONFIG_ERROR, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
            FatalError(host, ERR_NGN, NGN_PMEM_CONFIG_ERROR);
#endif
            break;
        }

        for(i=0; i<MAX_SOCKET;i++){
          //Update the SAD Interleave list
          if((imcMap[i] & BIT0))
          socket->SADintList[sadIndex][i *  host->var.mem.maxIMC] = 1; //host->nvram.mem.socket[i].imc[0].haNodeId;
          if((imcMap[i] & BIT1))
          socket->SADintList[sadIndex][(i *  host->var.mem.maxIMC)+1] = 1; //host->nvram.mem.socket[i].imc[1].haNodeId;
        }

        //if SPLIT_BELOW_4GB_EN is set then stop interleaving across sockets at 4GB
        if (host->setup.mem.options & SPLIT_BELOW_4GB_EN){
          if(socketNum < MAX_SOCKET){
            continue;
          }
        }

        //Adjust the Limit variable with the memhole size
        *nodeLimit += FOUR_GB_MEM - TOLM;

        // If TOLM boundary is not evenly divisible when interleaving across three channels of two memory controllers, then we need to adjust the node limit so that it divides evenly
        totalWays = counter * 3 ;
        if( TOLM % totalWays ) {

          //5332821: Partial Mirroring with 3Chway population causes Memicals data miscompare
          if ((host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM)||(host->nvram.mem.RASmode & PARTIAL_MIRROR_2LM)) {
            if(counter == 1) {
              totalWays = totalWays*2;
            }
          }

          // TOLM boundary is a multiple of 64MB less than a boundary evenly divisible by totalWays, so adjust nodeLimit by that multiple in 64MB units, so that it divides evenly
          *nodeLimit = *nodeLimit - ( totalWays - ( TOLM % totalWays )) ;
        }

        sadIndex++;
      }
    }

    if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN ) {
      if((sadIndex > 0) && (sadIndex < MAX_SAD_RULES)) {
        if(socket->SAD[sadIndex -1].Limit == FOUR_GB_MEM) {
          // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
          socket->SAD[sadIndex].Limit = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
          socket->SAD[sadIndex].Enable = 1;
          socket->SAD[sadIndex].Attr = SAD_NXM_ATTR;
          socket->SAD[sadIndex].local = 0;
          sadIndex++;
          *nodeLimit = (*nodeLimit + socket->SAD[sadIndex-1].Limit) - socket->SAD[sadIndex-2].Limit;
        }
      }
    }
    if (sadIndex < MAX_SAD_RULES) {
        //Create a SAD entry for the actual liomit value
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].ways  = counter;
        socket->SAD[sadIndex].type = memType;
        socket->SAD[sadIndex].imcInterBitmap =  imcMap[socketNum];

        if ( *nodeLimit > host->var.mem.mmiohBase) {
          if ((sadIndex > 0) && (socket->SAD[sadIndex - 1].Limit == host->var.mem.mmiohBase)) {
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "WARNING!!! Failure to create Sad%d! Sad%d limit is MMIOHBase...\n", sadIndex, sadIndex - 1 ));
            return FAILURE;
          }
          *nodeLimit = host->var.mem.mmiohBase;
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "WARNING!!! SAD%d limit exceeds MMIOHBase! Sad%d is truncated to MMIOHBase...\n", sadIndex, sadIndex));
        }
        socket->SAD[sadIndex].Limit =    *nodeLimit;
        socket->SAD[sadIndex].local = 1;
        socket->SAD[sadIndex].mirrored = 0;
        socket->SAD[sadIndex].Attr = 0;
        socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );
        switch(memType) {
          case MEM_TYPE_1LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].channelInterBitmap[mc] =BITMAP_CH0_CH1_CH2 ;
              }
            }
            break;
          case MEM_TYPE_2LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
                socket->SAD[sadIndex].channelInterBitmap[mc] =channelInterBitmap ;
              }
            }
            break;
          case MEM_TYPE_PMEM :
            socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] =BITMAP_CH0_CH1_CH2 ;
              }
            }
            break;
          case MEM_TYPE_PMEM_CACHE :
            socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
                socket->SAD[sadIndex].channelInterBitmap[mc] =channelInterBitmap ;
              }
            }
            break;
          case MEM_TYPE_BLK_WINDOW :
          case MEM_TYPE_CTRL :
            socket->SAD[sadIndex].granularity = 0;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
              }
            }
            break;
        }

        for(i=0; i<MAX_SOCKET;i++){
          //Update the SAD Interleave list
          if((imcMap[i] & BIT0))
            socket->SADintList[sadIndex][i *  host->var.mem.maxIMC] = 1; //host->nvram.mem.socket[i].imc[0].haNodeId;
          if((imcMap[i] & BIT1))
            socket->SADintList[sadIndex][(i *  host->var.mem.maxIMC)+1] = 1; //host->nvram.mem.socket[i].imc[1].haNodeId;
        }
    }
  }//socketNum
  return SUCCESS;
}

/**

  Description: This rountine fill the two way interleave table

  @param host  - Pointer to sysHost
  @param TwoWayMap   -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param TwoWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillTwoWayInterTable_2lm(
  PSYSHOST host,
  UINT16  TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT8   nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8   TwoWayCombnationIndex
  )
{
  UINT16 twoLMMatrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET] = {{0}};
  UINT8  twoLMCounter[MAX_TWO_LM_COMBINATION]={0}, ddrtChInterleave=0, Index, mc;

  switch(TwoWayCombnationIndex)
  {
    case 0:
    ddrtChInterleave = 0x03;
    break;

  case 1:
    ddrtChInterleave = 0x05;
    break;

  case 2:
    ddrtChInterleave = 0x06;
    break;
  }

  Index = FillMatrix_2LM(host, twoLMMatrix,  twoLMCounter, ddrtChInterleave);
  TwoWayCount[TwoWayCombnationIndex] = twoLMCounter[Index];
  nmInterleaveBitMap[TwoWayCombnationIndex] = Index + 1;

  for(mc =0; mc< host->var.mem.maxIMC * MAX_SOCKET; mc++) {
    TwoWayMap[TwoWayCombnationIndex][mc] = twoLMMatrix[Index][mc];
  if((TwoWayMap[TwoWayCombnationIndex][mc] != 0) && (interleavableSize[TwoWayCombnationIndex] >  TwoWayMap[TwoWayCombnationIndex][mc]))
    interleavableSize[TwoWayCombnationIndex] = TwoWayMap[TwoWayCombnationIndex][mc];

  }
}

/**

  Description: This rountine fill the two way interleave table

  @param host  - Pointer to sysHost
  @param TwoWayMap   -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param TwoWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillTwoWayInterTable(
  PSYSHOST host,
  UINT16  TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8   TwoWayCombnationIndex
  )
{
  UINT8  socketNum, channelIndex, mc ;
  struct ddrChannel *channelList;
  struct  socketNvram  *socketNv;

   for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
         socketNv = &host->nvram.mem.socket[socketNum];
      // Skip if socket not enabled
      if (socketNv->enabled == 0) continue;

      channelList = &host->var.mem.socket[socketNum].channelList[0];
      for(mc =0; mc< host->var.mem.maxIMC; mc++) {
        channelIndex = mc * MAX_MC_CH ;
        switch(TwoWayCombnationIndex) {
          case 0: //Combination 011
              if( (channelList[channelIndex].remSize !=0) &&  ( channelList[channelIndex+1].remSize !=0)){
                TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc] = (channelList[channelIndex].remSize < channelList[channelIndex+1].remSize)? channelList[channelIndex].remSize : channelList[channelIndex+1].remSize;
                TwoWayCount[TwoWayCombnationIndex]++;
                if(interleavableSize[TwoWayCombnationIndex] >  TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc])
                  interleavableSize[TwoWayCombnationIndex] = TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc];
              }
              break;
          case 1://Combination 101
             if( (channelList[channelIndex].remSize !=0) &&  ( channelList[channelIndex+2].remSize !=0)){
                TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc] = (channelList[channelIndex].remSize < channelList[channelIndex+2].remSize)? channelList[channelIndex].remSize : channelList[channelIndex+2].remSize;
                TwoWayCount[TwoWayCombnationIndex]++;
                if(interleavableSize[TwoWayCombnationIndex] >  TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc])
                  interleavableSize[TwoWayCombnationIndex] = TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc];
              }
              break;
          case 2:////Combination 110
               if( (channelList[channelIndex+1].remSize !=0) &&  ( channelList[channelIndex+2].remSize !=0)){
                TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc] = (channelList[channelIndex+1].remSize < channelList[channelIndex+2].remSize)? channelList[channelIndex+1].remSize : channelList[channelIndex+2].remSize;
                TwoWayCount[TwoWayCombnationIndex]++;
                if(interleavableSize[TwoWayCombnationIndex] >  TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc])
                  interleavableSize[TwoWayCombnationIndex] = TwoWayMap[TwoWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc];
              }
              break;
        }
      }//mc
    }//sock
}

/**

  Description: This routine check if two way UMA interleave is possible

  @param Host  - Pointer to sysHost
  @param MemType - memory type
  @param TwoWayMap   -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param InterleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param NmInterleaveBitMap  - structure that holds the near memory interleave information

  @retval Counter - number of imcs participating in the interleave combination that yield maximum interleave.

**/
UINT8
IsTwoWayInterleave (
  SYSHOST 	*Host,
  UINT8   	MemType,
  UINT16  	TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   	TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT16  	InterleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8   	NmInterleaveBitMap[MAX_TWO_WAY_COMBINATION]
  )

{
  UINT8                         SocketNum;
  UINT8                         Mc;
  UINT8                         i;
  UINT8                         Counter;
  UINT8                         DiffCount;
  UINT8                         ImcIndex;
  UINT8                         SocketIndex;
  UINT8                         TargetGranularity;
  UINT16                        ImcInterSize;
  SOCKET_NVRAM                  *SocketNv;
  EncodedInterleaveGranularity  EncodedGranularity;

  ImcInterSize = 0xffff;
  Counter = 0;
  ImcIndex = 0;
  SocketIndex = 0;
  DiffCount = 0;
  
  TargetGranularity = GetTargetInterleaveGranularity(Host, MemType);
  EncodedGranularity = SADEncodeInterleaveGranularity(Host, MemType, TargetGranularity);

  //Steps:
  //Update three input varibale in this funtion:
  //TwoWayMap -  two dimensional array with possible interleavable size of every IMc under a specific channel interleave combination
  //Two way count - one dimensional array with number of IMCs participating in the interleave if a particular 2-way channel interleave is selected
  //interleavableSize - one dimensional array with the interleave size for every possible 2-way channel interleave
  //  IMC/Ch. Interleave   |     imc0  imc1  imc2  imc3  imc4  imc5  imc6  imc7       | TwoWayCount   | interleavableSize |
  //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   |-  - - - - - - |-  - - - - - - - --|
  //        011            |                                                          |               |                   |
  //        101            |                                                          |               |                   |
  //        110            |                                                          |               |                   |

  for (i = 0; i < MAX_TWO_WAY_COMBINATION; i++) {
    for (SocketNum = 0; SocketNum < MAX_SOCKET; SocketNum++) {
      for (Mc = 0; Mc < Host->var.mem.maxIMC; Mc++) {
        TwoWayMap[i][(SocketNum * Host->var.mem.maxIMC) + Mc] = 0;
      }
    }
    TwoWayCount[i] = 0;
    NmInterleaveBitMap[i] = 0;
  }

  for (i = 0; i < MAX_TWO_WAY_COMBINATION; i++) {
    //Fill the two way interleave table for each of the Two way Interleave combinations possible
    //index 0 : Channel Interleave 011
    //index 1 : Channel Interleave 101
    //index 2 : Channel Interleave 110

    if (MemType == MEM_TYPE_2LM  || MemType == MEM_TYPE_PMEM_CACHE) {
      FillTwoWayInterTable_2lm(Host, TwoWayMap, TwoWayCount, NmInterleaveBitMap, InterleavableSize, i);
    } else {
      FillTwoWayInterTable(Host, TwoWayMap, TwoWayCount, InterleavableSize, i);
    }
    //adjust the SAD ways to the nearest valid one
    //Check for valid Interleave ways and store the difference in diffCount
    if ((MemType == MEM_TYPE_1LM) || (MemType == MEM_TYPE_PMEM)) {
      if (EncodedGranularity.chaCh == EncodedGranularity.chaTgt) {
        // Check if enough target entries in the interleave list for this SAD entry
        while ((TwoWayCount[i] * 2) > MAX_SAD_WAYS) {
          TwoWayCount[i]--;
          DiffCount++;
        }
      }
    } else if ((MemType == MEM_TYPE_2LM) || (MemType == MEM_TYPE_PMEM_CACHE)) {
      if (EncodedGranularity.fmCh == EncodedGranularity.chaTgt)  {
        // Check if enough target entries in the interleave list for this SAD entry
        while ((TwoWayCount[i] * 2) > MAX_SAD_WAYS) {
          TwoWayCount[i]--;
          DiffCount++;
        }
      }
    }

    while((TwoWayCount[i] != 0) && (!IsPowerOfTwo(TwoWayCount[i]))) {
      TwoWayCount[i]--;
      DiffCount++;
    };

    //Adjust the map based on the variable diffCount
    //If there is a socket with both the IMCs participating in the interleave, remove the imc with lowet possible Interleave size.
    for (Mc = 0; Mc < (MAX_SOCKET * Host->var.mem.maxIMC); Mc += Host->var.mem.maxIMC) {
      if (DiffCount == 0) {
        break;
      }
      if (TwoWayMap[i][Mc] && TwoWayMap[i][Mc + 1]) {
        if (TwoWayMap[i][Mc] < TwoWayMap[i][Mc + 1]) {
          TwoWayMap[i][Mc] = 0;
        } else {
          TwoWayMap[i][Mc + 1] = 0;
        }
        DiffCount--;
      }
    }
    //Adjust the map based on the variable diffCount
    //Next, remove the imc from the whole map with the lowest possible interleave size.
    while (DiffCount) {
      for (SocketNum = 0; SocketNum < MAX_SOCKET; SocketNum++) {
        SocketNv = &Host->nvram.mem.socket[SocketNum];
        // Skip if socket not enabled
        if (SocketNv->enabled == 0) {
          continue;
        }

        for (Mc = 0; Mc < Host->var.mem.maxIMC; Mc++) {
          //skip mc that is not populated in both the channels.
          if(TwoWayMap[i][(SocketNum* Host->var.mem.maxIMC) + Mc] == 0) {
            continue;
          }

          if(ImcInterSize > TwoWayMap[i][(SocketNum * Host->var.mem.maxIMC) + Mc]) {
            ImcInterSize = TwoWayMap[i][(SocketNum * Host->var.mem.maxIMC) + Mc];
            ImcIndex = Mc;
            SocketIndex = SocketNum;
          }
        } // Mc
      } // Socket
      TwoWayMap[i][(SocketIndex * Host->var.mem.maxIMC) + ImcIndex] = 0;
      DiffCount--;
    };

    //Recalculate the interleaveSize and Interleave cout values after adjusting the sadways
    InterleavableSize[i] = 0xffff;
    TwoWayCount[i] = 0;
    for (Mc = 0; Mc < Host->var.mem.maxIMC * MAX_SOCKET; Mc++) {
      if (TwoWayMap[i][Mc] != 0) {
        TwoWayCount[i]++;
        if (InterleavableSize[i] > TwoWayMap[i][Mc]) {
          InterleavableSize[i] = TwoWayMap[i][Mc];
        }
      }
    }
  }//i

  for (i = 0; i < MAX_TWO_WAY_COMBINATION; i++) {
    if(TwoWayCount[i] > Counter) {
      Counter = TwoWayCount[i];
    }
  }

  return Counter;
}


/**

  Description: Do two way UMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM  - top of low memory 
  @param memType  - Type of memory region to be mapped  
  @param TwoWayMap   -  structure to hold the interleaveable size in each participating imc
  @param TwoWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param interleaveOption - structure to hold the interleaveble size in each of the possible imc combination
  @param nodeLimit - Pointer to return Limit of the sad entry created.

  @retval status
**/
UINT32
DoTwoWayUMAInterleave(
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16          TOLM,
  UINT8           memType,
  UINT16          TwoWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8           TwoWayCount[MAX_TWO_WAY_COMBINATION],
  UINT16          interleavableSize[MAX_TWO_WAY_COMBINATION],
  UINT8           nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION],
  UINT8           *interleaveOption,
  UINT32          *nodeLimit
  )
{
  UINT8  channelIndex, sadIndex, socketNum, mc;
  UINT8  imcMap[MAX_SOCKET];
  UINT8  i, channelInterBitmap, FMchannelInterBitmap;
  UINT32 maxInterleaveSize;
  struct ddrChannel *channelList;
  struct Socket  *socket;
  struct  socketNvram  *socketNv;
  UINT8 McList[MAX_SOCKET*MAX_IMC] = {0};

  *nodeLimit = 0;
  sadIndex = 0;
  maxInterleaveSize = 0;
  *interleaveOption = 0;
  channelInterBitmap = 0;
  FMchannelInterBitmap = 0;

  //find the optimal channel interleave that would yield maximum memory to be interleaved.
  //interleavableSize[i] * TwoWayCount[i] - gives out the maximum interleaveble memory for each combnation.
  for(i=0; i<MAX_TWO_WAY_COMBINATION; i++){
    if ((UINT32) (interleavableSize[i] * TwoWayCount[i] * TWO_WAY) > maxInterleaveSize){
      maxInterleaveSize = interleavableSize[i] * TwoWayCount[i] * TWO_WAY;
      *interleaveOption = i;
    }
  }

  if (host->setup.mem.options & SPLIT_BELOW_4GB_EN){
    interleavableSize[*interleaveOption] = (TOLM / (TwoWayCount[*interleaveOption] * TWO_WAY));
  }

  //Calculate the imcMap for each socket
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
      //clear the imc bit map for this socket
    imcMap[socketNum] = 0;
    socketNv = &host->nvram.mem.socket[socketNum];
    // Skip if socket not enabled
    if (socketNv->enabled == 0) continue;

    for(mc =0; mc< host->var.mem.maxIMC; mc++) {
      //skip mc that is not populated in all the channels.
      if(TwoWayMap[*interleaveOption][(socketNum* host->var.mem.maxIMC)+mc] == 0) continue;
      //Update imcMap structure for this socket
      imcMap[socketNum]|=(BIT0<<mc);
    }
  }

  // find out what all MC are participating/populated in all the sockets
  for (i = 0; i<MAX_SOCKET; i++){
    if (imcMap[i] & BIT0)
      McList[i *  host->var.mem.maxIMC] = 1;
	if (imcMap[i] & BIT1)
	  McList[(i *  host->var.mem.maxIMC) + MC1] = 1;
  }

  if (IsSkuLimitViolation(host, MemMapData, maxInterleaveSize, memType, McList) == TRUE ) {
#if SMCPKG_SUPPORT
    OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
#else
    FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
  }

  //Parse through each socket to create a SAD
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
    socket = &host->var.mem.socket[socketNum];
    channelList = &host->var.mem.socket[socketNum].channelList[0];
    //Parse through each imc in the socket to subtract the interleave size for the channel size variable.
    //Create the imc bit map for this socket
    for(mc =0; mc< host->var.mem.maxIMC; mc++) {
      //Skip the IMCs that are not part of the interleave
      if(TwoWayMap[*interleaveOption][(socketNum *  host->var.mem.maxIMC) + mc] == 0) continue;
      channelIndex = mc * MAX_MC_CH ;
      switch(*interleaveOption) {
        case 0:
          channelList[channelIndex].remSize -=interleavableSize[*interleaveOption];
          channelList[channelIndex+1].remSize -=interleavableSize[*interleaveOption];
          if( ( memType == MEM_TYPE_2LM ) || ( memType == MEM_TYPE_CTRL ) || (memType == MEM_TYPE_PMEM) || (memType == MEM_TYPE_PMEM_CACHE) || (memType == MEM_TYPE_BLK_WINDOW) )
            FMchannelInterBitmap = BITMAP_CH0_CH1;
          else
            channelInterBitmap = BITMAP_CH0_CH1;
          break;

        case 1:
          channelList[channelIndex].remSize -=interleavableSize[*interleaveOption];
          channelList[channelIndex+2].remSize -=interleavableSize[*interleaveOption];
          if( ( memType == MEM_TYPE_2LM ) || ( memType == MEM_TYPE_CTRL ) || (memType == MEM_TYPE_PMEM) ||  (memType == MEM_TYPE_PMEM_CACHE) || (memType == MEM_TYPE_BLK_WINDOW)  )
            FMchannelInterBitmap = BITMAP_CH0_CH2;
          else
            channelInterBitmap = BITMAP_CH0_CH2;
          break;

        case 2:
          channelList[channelIndex+1].remSize -=interleavableSize[*interleaveOption];
          channelList[channelIndex+2].remSize -=interleavableSize[*interleaveOption];
          if( ( memType == MEM_TYPE_2LM ) || ( memType == MEM_TYPE_CTRL )  || (memType == MEM_TYPE_PMEM) ||  (memType == MEM_TYPE_PMEM_CACHE) || (memType == MEM_TYPE_BLK_WINDOW) )
            FMchannelInterBitmap = BITMAP_CH1_CH2;
          else
            channelInterBitmap = BITMAP_CH1_CH2;
          break;
      }
      if(memType == MEM_TYPE_2LM  || memType == MEM_TYPE_PMEM_CACHE)
        //Calculate the Channel interleave bitmap based on Near Memory population for 2LM mode.
        channelInterBitmap = nmInterleaveBitMap[*interleaveOption];

    }//mc

    //Skip the socket thet dont have atleast one IMC participating in the interleave
    if((TwoWayMap[*interleaveOption][(socketNum *  host->var.mem.maxIMC)] == 0) && (TwoWayMap[*interleaveOption][(socketNum *  host->var.mem.maxIMC)+1]==0) ) continue;



    sadIndex = FindSadIndex(host, socketNum);
    // Should not reach end of SAD rules before allocating all memory, Issue warning and break TAD loop.
    // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
    if (sadIndex >= MAX_SAD_RULES) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SAD rules exceeds %d\n", MAX_SAD_RULES));
      OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
     return ERROR_RESOURCE_CALCULATION_FAILURE;
    }

    //Fill the SAD entry
    socket->SAD[sadIndex].Enable = 1;
    if(sadIndex)
      socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + maxInterleaveSize;
    else{

      if (host->setup.mem.options & SPLIT_BELOW_4GB_EN)
        socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
      else
      {
        socket->SAD[sadIndex].Limit = maxInterleaveSize;
      }
    }

    *nodeLimit = socket->SAD[sadIndex].Limit;
    //Create a new SAD if TOLM is breached
    if(socket->SAD[sadIndex].Limit >=TOLM) {
      if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0)){
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].ways  = TwoWayCount[*interleaveOption];
        for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
          if (imcMap[socketNum] & (BIT0 << mc)) {
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
        }
        socket->SAD[sadIndex].type = memType;
        socket->SAD[sadIndex].imcInterBitmap =  imcMap[socketNum];
        socket->SAD[sadIndex].local = 1;
        socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
        socket->SAD[sadIndex].mirrored = 0;
        socket->SAD[sadIndex].Attr = 0;
        socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

        switch(memType) {
          case MEM_TYPE_1LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
            break;
          case MEM_TYPE_2LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
              }
            }
            break;
          case MEM_TYPE_PMEM :
          case MEM_TYPE_PMEM_CACHE :
          case MEM_TYPE_BLK_WINDOW :
          case MEM_TYPE_CTRL :
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "INVALID CONFIGURATION. The first SAD rule cannot be PMEM, PMEM$, BLK or CTRL region\n"));
#if SMCPKG_SUPPORT
            OutputError (host, ERR_NGN, NGN_PMEM_CONFIG_ERROR, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
            FatalError(host, ERR_NGN, NGN_PMEM_CONFIG_ERROR);
#endif
        }

        for(i=0; i<MAX_SOCKET;i++){
          //Update the SAD Interleave list
          if(imcMap[i] & BIT0)
          socket->SADintList[sadIndex][i *  host->var.mem.maxIMC] = 1; //host->nvram.mem.socket[i].imc[0].haNodeId;
          if(imcMap[i] & BIT1)
            socket->SADintList[sadIndex][(i *  host->var.mem.maxIMC)+1] = 1; //host->nvram.mem.socket[i].imc[1].haNodeId;
        }

        //if SPLIT_BELOW_4GB_EN is set then stop interleaving across sockets at 4GB
        if (host->setup.mem.options & SPLIT_BELOW_4GB_EN){
          if(socketNum < MAX_SOCKET){
            continue;
          }
        }

        *nodeLimit += FOUR_GB_MEM - TOLM;
        sadIndex++;
      }
    }

    if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN ) {
      if((sadIndex > 0) && (sadIndex < MAX_SAD_RULES)) {
        if( socket->SAD[sadIndex -1].Limit == FOUR_GB_MEM) {
          // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
          socket->SAD[sadIndex].Limit = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
          socket->SAD[sadIndex].Enable = 1;
          socket->SAD[sadIndex].Attr = SAD_NXM_ATTR;
          socket->SAD[sadIndex].local = 0;
          sadIndex++;
          *nodeLimit = (*nodeLimit + socket->SAD[sadIndex-1].Limit) - socket->SAD[sadIndex-2].Limit;
        }
      }
    }

    if (sadIndex < MAX_SAD_RULES) {
      socket->SAD[sadIndex].Enable = 1;
      socket->SAD[sadIndex].ways  = TwoWayCount[*interleaveOption];
      for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
        if (imcMap[socketNum] & (BIT0 << mc)) {
          socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
        }
      }
      socket->SAD[sadIndex].type = memType;
      socket->SAD[sadIndex].imcInterBitmap =  imcMap[socketNum];

      if ( *nodeLimit > host->var.mem.mmiohBase) {
         if ((sadIndex > 0) && (socket->SAD[sadIndex - 1].Limit == host->var.mem.mmiohBase)) {
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "WARNING!!! Failure to create Sad%d! Sad%d limit is MMIOHBase...\n", sadIndex, sadIndex - 1 ));
            return FAILURE;
         }
         *nodeLimit = host->var.mem.mmiohBase;
         MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                 "WARNING!!! SAD%d limit exceeds MMIOHBase! Sad%d is truncated to MMIOHBase...\n", sadIndex, sadIndex));
      }
      socket->SAD[sadIndex].Limit =   *nodeLimit;
      socket->SAD[sadIndex].local = 1;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].Attr = 0;
      socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );
      switch(memType) {
        case MEM_TYPE_1LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
          break;
        case MEM_TYPE_2LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            if (imcMap[socketNum] & (BIT0 << mc)) {
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
            }
          }
          break;
        case MEM_TYPE_PMEM :
          socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            if (imcMap[socketNum] & (BIT0 << mc)) {
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
            }
          }
          break;
        case MEM_TYPE_PMEM_CACHE :
          socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            if (imcMap[socketNum] & (BIT0 << mc)) {
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
            }
          }
          break;
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
          socket->SAD[sadIndex].granularity = 0;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            if (imcMap[socketNum] & (BIT0 << mc)) {
              socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
            }
          }
          break;
      }

      for(i=0; i<MAX_SOCKET;i++){

        //Update the SAD Interleave list
        if(imcMap[i] & BIT0)
          socket->SADintList[sadIndex][i *  host->var.mem.maxIMC] = 1; //host->nvram.mem.socket[i].imc[0].haNodeId;
        if(imcMap[i] & BIT1)
          socket->SADintList[sadIndex][(i *  host->var.mem.maxIMC)+1] = 1; //host->nvram.mem.socket[i].imc[1].haNodeId;
      }
    }
  }//socket
  return   SUCCESS ;
}



/**

  Description: This rountine fill the one way interleave table for 2lm

  @param host  - Pointer to sysHost
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param oneWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillOneWayInterTable_2lm(
  PSYSHOST host,
  UINT16  oneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   oneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8  nmInterleaveBitMap[MAX_ONE_WAY_COMBINATION],
  UINT8   oneWayCombnationIndex
  )
{
  UINT16 twoLMMatrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET] = {{0}};
  UINT8  twoLMCounter[MAX_TWO_LM_COMBINATION]={0}, ddrtChInterleave=0, Index, mc;

  switch(oneWayCombnationIndex)
  {
    case 0:
    ddrtChInterleave = BITMAP_CH0;
    break;

  case 1:
    ddrtChInterleave = BITMAP_CH1;
    break;

  case 2:
    ddrtChInterleave = BITMAP_CH2;
    break;
  }

  Index = FillMatrix_2LM(host, twoLMMatrix, twoLMCounter, ddrtChInterleave);
  oneWayCount[oneWayCombnationIndex] = twoLMCounter[Index];
  nmInterleaveBitMap[oneWayCombnationIndex] = Index + 1;

  for(mc =0; mc< host->var.mem.maxIMC * MAX_SOCKET; mc++) {
    oneWayMap[oneWayCombnationIndex][mc] = twoLMMatrix[Index][mc];
  if((oneWayMap[oneWayCombnationIndex][mc] != 0) && (interleavableSize[oneWayCombnationIndex] >  oneWayMap[oneWayCombnationIndex][mc]))
    interleavableSize[oneWayCombnationIndex] = oneWayMap[oneWayCombnationIndex][mc];

  }
}

/**

  Description: This rountine fill the one way interleave table

  @param host  - Pointer to sysHost
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param OneWayCombnationIndex - The index to the combination that is most efficient


  @retval VOID

**/
VOID
FillOneWayInterTable(
  PSYSHOST host,
  UINT16  OneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   OneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8   OneWayCombnationIndex
  )
{
  UINT8  socketNum, channelIndex, mc ;
  struct ddrChannel *channelList;
  struct  socketNvram  *socketNv;

    for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
         socketNv = &host->nvram.mem.socket[socketNum];
      // Skip if socket not enabled
      if (socketNv->enabled == 0) continue;

      channelList = &host->var.mem.socket[socketNum].channelList[0];
      for(mc =0; mc< host->var.mem.maxIMC; mc++) {
        channelIndex = mc * MAX_MC_CH ;
        switch(OneWayCombnationIndex) {
          case 0:
              if( (channelList[channelIndex].remSize !=0)){
                OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc] = channelList[channelIndex].remSize ;
                OneWayCount[OneWayCombnationIndex]++;
                if(interleavableSize[OneWayCombnationIndex] >  OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc])
                  interleavableSize[OneWayCombnationIndex] = OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc];

              }
              break;
          case 1:
              if( (channelList[channelIndex+1].remSize !=0)){
                OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc] = channelList[channelIndex+1].remSize;
                OneWayCount[OneWayCombnationIndex]++;
                if(interleavableSize[OneWayCombnationIndex] >  OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc])
                  interleavableSize[OneWayCombnationIndex] = OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc];
              }
              break;
          case 2:
              if( channelList[channelIndex+2].remSize !=0){
                OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc] = channelList[channelIndex+2].remSize;
                OneWayCount[OneWayCombnationIndex]++;
                if(interleavableSize[OneWayCombnationIndex] >  OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc])
                  interleavableSize[OneWayCombnationIndex] = OneWayMap[OneWayCombnationIndex][(socketNum *  host->var.mem.maxIMC) + mc];
              }
              break;
        }
      }//mc
    }//socket
}

/**

  Description: This routine check if one way UMA interleave is possible

  @param host  - Pointer to sysHost
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information

  @retval counter - number of imcs participating in the interleave combination that yield maximum interleave.

**/
UINT8
IsOneWayInterleave(
  PSYSHOST host,
  UINT8   memType,
  UINT16  OneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8   OneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16  interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8   nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION]
  )
{
  UINT8  socketNum,  mc, i, counter, imcIndex, sockIndex, diffCount ;
  UINT16  imcInterSize;

  struct  socketNvram  *socketNv;


  imcInterSize = 0xffff;
  counter=0;
  imcIndex = 0;
  sockIndex=0;
  diffCount=0;

  //Steps:
  //Update three input varibale in this funtion:
  //OneWayMap -  two dimensional array with possible interleavable size of every IMc under a specific channel interleave combination
  //OneWayCount - one dimensional array with number of IMCs participating in the interleave if a particular 1-way channel interleave is selected
  //interleavableSize - one dimensional array with the interleave size for every possible 1-way channel interleave
  //  IMC/Ch. Interleave   |     imc0  imc1  imc2  imc3  imc4  imc5  imc6  imc7       | OneWayCount   | interleavableSize |
  //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -   |-  - - - - - - |-  - - - - - - - --|
  //        001            |                                                          |               |                   |
  //        010            |                                                          |               |                   |
  //        100            |                                                          |               |                   |

  //clear the onewaymap, onewaycount structures
  for(i=0; i<MAX_ONE_WAY_COMBINATION; i++)
  {
    for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
      for(mc =0; mc< host->var.mem.maxIMC; mc++) {
       OneWayMap[i][(socketNum *  host->var.mem.maxIMC) + mc] = 0;
      }
    }
  OneWayCount[i] = 0;
  nmInterleaveBitMap[i] = 0;
  interleavableSize[i] = 0xffff;
  }
  //Loop through each possible channel interleave combination
  for(i=0; i<MAX_ONE_WAY_COMBINATION; i++)
  {
   //Fill the one way interleave table for each of the one way Interleave combinations possible
    //index 0 : Channel Interleave 001
    //index 1 : Channel Interleave 010
    //index 2 : Channel Interleave 100

    if(memType == MEM_TYPE_2LM  || memType == MEM_TYPE_PMEM_CACHE){
      FillOneWayInterTable_2lm(host, OneWayMap, OneWayCount, interleavableSize, nmInterleaveBitMap, i);
    } else {
      FillOneWayInterTable(host, OneWayMap, OneWayCount, interleavableSize, i);
    }

    //adjust the SAD ways to the nearest valid one
    //Check for valid Interleave ways and store the difference in diffCount
    while((OneWayCount[i] != 0) && (! IsPowerOfTwo(OneWayCount[i]))){
      OneWayCount[i] --;
      diffCount++;
    };

    //Adjust the map based on the variable diffCount
    //If there is a socket with both the IMCs participating in the interleave, remove the imc with lowet possible Interleave size.
    for(mc=0; mc< (MAX_SOCKET* host->var.mem.maxIMC); mc+= host->var.mem.maxIMC) {
      if(diffCount==0) break;
      if(OneWayMap[i][mc] && OneWayMap[i][mc+1]){
        if(OneWayMap[i][mc] < OneWayMap[i][mc+1])
          OneWayMap[i][mc] = 0;
        else
          OneWayMap[i][mc+1] = 0;
        diffCount--;
      }
    }
    //Adjust the map based on the variable diffCount
    //Next, remove the imc from the whole map with the lowest possible interleave size.
    while(diffCount){
      for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
        socketNv = &host->nvram.mem.socket[socketNum];
        // Skip if socket not enabled
        if (socketNv->enabled == 0) continue;

        for(mc =0; mc< host->var.mem.maxIMC; mc++) {
          //skip mc that is not populated in all the channels.
          if(OneWayMap[i][(socketNum* host->var.mem.maxIMC)+mc] == 0) continue;

          if(imcInterSize > OneWayMap[i][(socketNum* host->var.mem.maxIMC)+mc] ){
            imcInterSize = OneWayMap[i][(socketNum* host->var.mem.maxIMC)+mc];
            imcIndex = mc;
            sockIndex = socketNum;
            }
        }//mc
      }//socket
      OneWayMap[i][(sockIndex *  host->var.mem.maxIMC)+imcIndex] = 0;
      diffCount--;
    };

    //Recalculate the interleaveSize and Interleave cout values after adjusting the sadways
    interleavableSize[i] = 0xffff;
    OneWayCount[i]=0;
    for(mc =0; mc< host->var.mem.maxIMC*MAX_SOCKET; mc++) {
      if(OneWayMap[i][mc] != 0){
        OneWayCount[i]++;
          if(interleavableSize[i] >  OneWayMap[i][mc])
            interleavableSize[i] = OneWayMap[i][mc];
      }
    }
  }//i

  //find the combination with the maximum IMC participation
  for(i=0; i<MAX_ONE_WAY_COMBINATION; i++)
  {
    if(OneWayCount[i]>counter)
      counter = OneWayCount[i];
  }

  return counter;
}

/**

  Description: Do one way UMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM  - top of low memory 
  @param memType  - Type of memory region to be mapped  
  @param OneWayMap   -  structure to hold the interleaveable size in each participating imc
  @param OneWayCount - structure to hold the number of participating imc in each of the possible imc combination
  @param nmInterleaveBitMap  - structure that holds the near memory interleave information
  @param interleavableSize - structure to hold the interleaveble size in each of the possible imc combination
  @param nodeLimit - Pointer to return Limit of the sad entry created.

  @retval status
**/
UINT32
DoOneWayUMAInterleave(
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16          TOLM,
  UINT8           memType,
  UINT16          OneWayMap[MAX_ONE_WAY_COMBINATION][MAX_IMC * MAX_SOCKET],
  UINT8           OneWayCount[MAX_ONE_WAY_COMBINATION],
  UINT16          interleavableSize[MAX_ONE_WAY_COMBINATION],
  UINT8           nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION],
  UINT8           *interleaveOption,
  UINT32          *nodeLimit
)
{
  UINT8  channelIndex, sadIndex, socketNum, mc;
  UINT8  i, channelInterBitmap, FMchannelInterBitmap;
  UINT8  imcMap[MAX_SOCKET];
  UINT32 maxInterleaveSize;
  UINT8 McList[MAX_SOCKET*MAX_IMC] = {0};
  struct ddrChannel *channelList;
  struct Socket  *socket;
  struct  socketNvram  *socketNv;
  UINT8 Index;

  sadIndex = 0;
  maxInterleaveSize = 0;
  *interleaveOption = 0;
  channelInterBitmap = 0;
  FMchannelInterBitmap = 0;
  *nodeLimit = 0;

  //Parse through the map to find the one way ch interleave combination wihich wiill result in maximum memory to be interleaved.
  for(i=0; i<MAX_ONE_WAY_COMBINATION; i++){
    if ((UINT32) (interleavableSize[i] * OneWayCount[i]) > maxInterleaveSize){
      maxInterleaveSize = interleavableSize[i] * OneWayCount[i];
      *interleaveOption = i;
    }
  }

  if (host->setup.mem.options & SPLIT_BELOW_4GB_EN){
    interleavableSize[*interleaveOption] = (TOLM /OneWayCount[*interleaveOption]);
  }

  //Calculate the imcMap for each socket
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
      //clear the socket bit map for this socket
    imcMap[socketNum] = 0;
    socketNv = &host->nvram.mem.socket[socketNum];
    // Skip if socket not enabled
    if (socketNv->enabled == 0) continue;

    for(mc =0; mc< host->var.mem.maxIMC; mc++) {
      //skip mc that is not populated in all the channels.
      if(OneWayMap[*interleaveOption][(socketNum* host->var.mem.maxIMC)+mc] == 0) continue;
      //Update imcMap structure for this socket
      imcMap[socketNum]|=(BIT0<<mc);
    }
  }
  // find out what all MC are participating/populated in all the sockets
  for (Index = 0; Index < MAX_SOCKET; Index++) {
    if (imcMap[Index] & BIT0)
      McList[Index *  host->var.mem.maxIMC] = 1;
    if (imcMap[Index] & BIT1)
      McList[(Index *  host->var.mem.maxIMC) + MC1] = 1;
  }

  if (IsSkuLimitViolation(host, MemMapData, maxInterleaveSize, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
    OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK);
#else
    FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
  }
  //Parse through each socket to create a SAD
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++) {
    socket = &host->var.mem.socket[socketNum];
    channelList = &host->var.mem.socket[socketNum].channelList[0];
    //Parse through each IMC in the socket to create the IMC bitmap for this socket and subtract the intreleave size form the channel mmemory size in participating channels.
    for(mc =0; mc< host->var.mem.maxIMC; mc++) {
      if(OneWayMap[*interleaveOption][(socketNum *  host->var.mem.maxIMC) + mc] == 0) continue;
      channelIndex = mc * MAX_MC_CH ;
      switch(*interleaveOption) {
              case 0:
                channelList[channelIndex].remSize -=interleavableSize[*interleaveOption];
                channelInterBitmap = BITMAP_CH0;
                if( ( memType == MEM_TYPE_2LM ) || ( memType == MEM_TYPE_CTRL ) || (memType == MEM_TYPE_PMEM)  || (memType == MEM_TYPE_PMEM_CACHE) || (memType == MEM_TYPE_BLK_WINDOW))
                  FMchannelInterBitmap = BITMAP_CH0;
                break;

              case 1:
                channelList[channelIndex+1].remSize -=interleavableSize[*interleaveOption];
                channelInterBitmap = BITMAP_CH1;
                if( ( memType == MEM_TYPE_2LM ) || ( memType == MEM_TYPE_CTRL ) || (memType == MEM_TYPE_PMEM) || (memType == MEM_TYPE_PMEM_CACHE) || (memType == MEM_TYPE_BLK_WINDOW))
                  FMchannelInterBitmap = BITMAP_CH1;
                break;

              case 2:
                channelList[channelIndex+2].remSize -=interleavableSize[*interleaveOption];
                channelInterBitmap = BITMAP_CH2;
                if( ( memType == MEM_TYPE_2LM ) || ( memType == MEM_TYPE_CTRL ) || (memType == MEM_TYPE_PMEM) || (memType == MEM_TYPE_PMEM_CACHE) || (memType == MEM_TYPE_BLK_WINDOW))
                  FMchannelInterBitmap = BITMAP_CH2;
                break;
      }
      if(memType == MEM_TYPE_2LM  || memType == MEM_TYPE_PMEM_CACHE )
        //Calculate the Channel interleave bitmap based on Near Memory population for 2LM mode.
        channelInterBitmap = nmInterleaveBitMap[*interleaveOption];
      else if (memType == MEM_TYPE_CTRL )
        channelInterBitmap = 0 ;

    }//mc

    //Skip the sockets that are not participating in the interleave
    if((OneWayMap[*interleaveOption][(socketNum *  host->var.mem.maxIMC)] == 0) && (OneWayMap[*interleaveOption][(socketNum *  host->var.mem.maxIMC)+1]==0) ) continue;
    sadIndex = FindSadIndex(host, socketNum);
    // Should not reach end of SAD rules before allocating all memory, Issue warning and break TAD loop.
    // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
    if (sadIndex >= MAX_SAD_RULES) {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "SAD rules exceeds %d\n", MAX_SAD_RULES));
      OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
      return ERROR_RESOURCE_CALCULATION_FAILURE;
    }

    //Fill the SAD entry
    if(sadIndex)
      socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + maxInterleaveSize;
    else
      if (host->setup.mem.options & SPLIT_BELOW_4GB_EN)
        socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
      else
      {
        socket->SAD[sadIndex].Limit = maxInterleaveSize;
      }

    *nodeLimit = socket->SAD[sadIndex].Limit;

    //Create a new SAD if TOLM is breached
    if(socket->SAD[sadIndex].Limit >=TOLM) {
      if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0)){
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].ways  = OneWayCount[*interleaveOption];
        for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
          if (imcMap[socketNum] & (BIT0 << mc)) {
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
        }
        socket->SAD[sadIndex].type = memType;
        socket->SAD[sadIndex].imcInterBitmap =  imcMap[socketNum];
        socket->SAD[sadIndex].local = 1;
        socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
        socket->SAD[sadIndex].mirrored = 0;
        socket->SAD[sadIndex].Attr = 0;
        socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

        switch(memType) {
          case MEM_TYPE_1LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
            break;
          case MEM_TYPE_2LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
             for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
              }
            }
            break;
          case MEM_TYPE_PMEM :
          case MEM_TYPE_PMEM_CACHE :
          case MEM_TYPE_BLK_WINDOW :
          case MEM_TYPE_CTRL :
            MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                           "INVALID CONFIGURATION. The first SAD rule cannot be PMEM, PMEM$, BLK or CTRL region\n"));
#if SMCPKG_SUPPORT
            OutputError (host, ERR_NGN, NGN_PMEM_CONFIG_ERROR, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
            FatalError(host, ERR_NGN, NGN_PMEM_CONFIG_ERROR);
#endif
            break;
        }

        for(i=0; i<MAX_SOCKET;i++){
          //Update the SAD Interleave list
          if(imcMap[i] & BIT0)
            socket->SADintList[sadIndex][i *  host->var.mem.maxIMC] = 1;
          if(imcMap[i] & BIT1)
          socket->SADintList[sadIndex][(i *  host->var.mem.maxIMC)+1] = 1;
        }

        //if SPLIT_BELOW_4GB_EN is set then stop interleaving across sockets at 4GB
        if (host->setup.mem.options & SPLIT_BELOW_4GB_EN){
          if(socketNum < MAX_SOCKET){
            continue;
          }
        }

        *nodeLimit += FOUR_GB_MEM - TOLM;
        sadIndex++;
      }
    }

    if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN ) {
      if ((sadIndex > 0) && (sadIndex < MAX_SAD_RULES)) {
        if (socket->SAD[sadIndex -1].Limit == FOUR_GB_MEM) {
          // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
          socket->SAD[sadIndex].Limit = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
          socket->SAD[sadIndex].Enable = 1;
          socket->SAD[sadIndex].Attr = SAD_NXM_ATTR;
          socket->SAD[sadIndex].local = 0;
          sadIndex++;
          *nodeLimit = (*nodeLimit + socket->SAD[sadIndex-1].Limit) - socket->SAD[sadIndex-2].Limit;
        }
      }
    }

    if(sadIndex < MAX_SAD_RULES) {
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].ways  = OneWayCount[*interleaveOption];
        for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
        if (imcMap[socketNum] & (BIT0 << mc)) {
          socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
        }
        socket->SAD[sadIndex].type = memType;
        socket->SAD[sadIndex].imcInterBitmap =  imcMap[socketNum];

        if ( *nodeLimit > host->var.mem.mmiohBase) {
           if ((sadIndex > 0) && (socket->SAD[sadIndex - 1].Limit == host->var.mem.mmiohBase)) {
              MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                               "WARNING!!! Failure to create Sad%d! Sad%d limit is MMIOHBase...\n", sadIndex, sadIndex - 1 ));
              return FAILURE;
           }
           *nodeLimit = host->var.mem.mmiohBase;
           MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                                   "WARNING!!! SAD%d limit exceeds MMIOHBase! Sad%d is truncated to MMIOHBase...\n", sadIndex, sadIndex));
        }
        socket->SAD[sadIndex].Limit = *nodeLimit;
        socket->SAD[sadIndex].local = 1;
        socket->SAD[sadIndex].mirrored = 0;
        socket->SAD[sadIndex].Attr = 0;
        socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );
        switch(memType) {
          case MEM_TYPE_1LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
            break;
          case MEM_TYPE_2LM :
            socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
             for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
              }
            }
            break;
          case MEM_TYPE_PMEM :
            socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
             for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
              }
            }
            break;
          case MEM_TYPE_PMEM_CACHE :
            socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
             for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
              }
            }
            break;
          case MEM_TYPE_BLK_WINDOW :
          case MEM_TYPE_CTRL :
            socket->SAD[sadIndex].granularity = 0;
            for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
              if (imcMap[socketNum] & (BIT0 << mc)) {
                socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
              }
            }
            break;
        }

        for(i=0; i<MAX_SOCKET;i++){

          //Update the SAD Interleave list
          if(imcMap[i] & BIT0)
            socket->SADintList[sadIndex][i *  host->var.mem.maxIMC] = 1;
          if(imcMap[i] & BIT1)
            socket->SADintList[sadIndex][(i *  host->var.mem.maxIMC)+1] = 1;
        }
    }
  }//socket
  return SUCCESS;
}

/**

  Interleaves memory across sockets. This routine figures out the mapping and
  fills in the structures for the SAD tables.  It does not program any
  registers.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM      - Top of low memory
  @param memType   - Memory type

  @retval status


**/
UINT32
SADInterleave (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16          TOLM,
  UINT8           memType
)
{
  UINT8     counter, interleaveOption, Index;
  UINT32    nodeLimit, status = SUCCESS;
  UINT16  NWayMap[MAX_TWO_WAY_COMBINATION][MAX_IMC * MAX_SOCKET] = {{0}}, twoLMMatrix[MAX_TWO_LM_COMBINATION][MAX_IMC * MAX_SOCKET] = {{0}};
  UINT8   NWayCount[MAX_TWO_WAY_COMBINATION]={0}, nmInterleaveBitMap[MAX_TWO_WAY_COMBINATION]={0};
  UINT16  interleavableSize[MAX_TWO_WAY_COMBINATION] = {0xffff, 0xffff, 0xffff};
  UINT8  socketNum, ch;
  UINT8  Below4GBEnabledFlag;

  nodeLimit = 0;
  interleaveOption = 0;
  if((host->var.mem.chInter == 3) || (memType == MEM_TYPE_BLK_WINDOW) || (memType == MEM_TYPE_CTRL)){
    //Do three channel way interleave until possible
    do{
      counter = IsThreeWayInterleave(host, memType, twoLMMatrix, &Index) ;
      if(counter>1){
        status = DoThreeWayUMAInterleave(host, MemMapData, TOLM, memType, twoLMMatrix, Index, counter, &nodeLimit );
        if(status != SUCCESS) return status;
        //create remote SAD entries for the other sockets
        status = AddRemoteSadEntry(host, twoLMMatrix[Index], nodeLimit, memType);
        if(status != SUCCESS) return status;

        if((host->setup.mem.options & SPLIT_BELOW_4GB_EN) && (nodeLimit == FOUR_GB_MEM)){
          return SUCCESS;
        }

      }
    } while(counter>1);

    //Reset the counter
    counter =0;
  }
  if((host->var.mem.chInter >= 2)|| (memType == MEM_TYPE_BLK_WINDOW) || (memType == MEM_TYPE_CTRL)){
    //Do two channel way interleave until possible
    do{
      counter = IsTwoWayInterleave(host, memType, NWayMap, NWayCount, interleavableSize, nmInterleaveBitMap ) ;

      //If counter is more than one, do 2 ch way interleave
      if(counter>1){
        status = DoTwoWayUMAInterleave(host, MemMapData, TOLM, memType, NWayMap, NWayCount, interleavableSize, nmInterleaveBitMap, &interleaveOption, &nodeLimit);
        if(status != SUCCESS) return status;
        //create remote SAD entries for the other sockets
        status = AddRemoteSadEntry(host, NWayMap[interleaveOption], nodeLimit, memType);
        if(status != SUCCESS) return status;

       if((host->setup.mem.options & SPLIT_BELOW_4GB_EN) && (nodeLimit == FOUR_GB_MEM)){
         return SUCCESS;
       }

      }
    }while(counter>1);

    //Reset the counter
    counter =0;
  }

  //Do one channel way interleave until possible
  do{
    counter = IsOneWayInterleave(host, memType, NWayMap, NWayCount, interleavableSize, nmInterleaveBitMap) ;
    if(counter>1) {
      status = DoOneWayUMAInterleave(host, MemMapData, TOLM, memType, NWayMap, NWayCount, interleavableSize, nmInterleaveBitMap, &interleaveOption, &nodeLimit );
      if(status != SUCCESS) return status;
      //create remote SAD entries for the other sockets
      status = AddRemoteSadEntry(host, NWayMap[interleaveOption], nodeLimit, memType);
        if(status != SUCCESS) return status;

      if((host->setup.mem.options & SPLIT_BELOW_4GB_EN) && (nodeLimit == FOUR_GB_MEM)){
        return SUCCESS;
      }

    }
  }while(counter>1);

  //For each socket do Channel interleave (within every IMC) untill all the memory is allocated.
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++){
    Below4GBEnabledFlag = INTER_BELOW_4GB_DIS;
    if(host->setup.mem.options & SPLIT_BELOW_4GB_EN){
      status = DoChannelInterleave(host, MemMapData, socketNum, TOLM, memType, &nodeLimit, INTER_BELOW_4GB_EN);
        if(status != SUCCESS) return status;
      Below4GBEnabledFlag = INTER_BELOW_4GB_EN;
      if (nodeLimit == FOUR_GB_MEM){
        return SUCCESS;
      }
    }

    if(Below4GBEnabledFlag == INTER_BELOW_4GB_DIS){
      status = DoChannelInterleave(host, MemMapData, socketNum, TOLM, memType,  &nodeLimit, INTER_BELOW_4GB_DIS);
        if(status != SUCCESS) return status;
    }

  }//socketNum

  // Reload the channel size in the structure
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++){
    for (ch=0; ch <( host->var.mem.maxIMC* MAX_MC_CH); ch++)  {
#ifdef NVMEM_FEATURE_EN
      host->var.mem.socket[socketNum].channelList[ch].remSize = host->var.mem.socket[socketNum].channelList[ch].memSize - host->var.mem.socket[socketNum].channelList[ch].NVmemSize;
#else
      host->var.mem.socket[socketNum].channelList[ch].remSize = host->var.mem.socket[socketNum].channelList[ch].memSize;
#endif
    }
  }
  return SUCCESS;
}

BOOLEAN
IsPmemPossible(
  PSYSHOST  host,
  UINT8     socketBitMap)
/**

  This routine looks for NVM dimms with PMEM partition

  @param host         - Pointer to sysHost
  @param socketBitMap - BitMap of sockets to process

  @retval SUCCESS

**/
{
  UINT8   ch, sockId;
  struct  Socket *socket;

  for (sockId = 0; sockId < MAX_SOCKET; sockId++) {
    //Skip the sockets that are not requested. (FOR NUMA)
    if (((BIT0<<sockId) & socketBitMap) == 0) continue;
    socket = &host->var.mem.socket[sockId];
    for (ch=0; ch < ( host->var.mem.maxIMC* MAX_MC_CH); ch++) {
      if (socket->channelList[ch].perSize) return TRUE;
    }
  }
  return FALSE;
}

VOID
UpdateHostStructForPmem(
  PSYSHOST   host,
  UINT8     socketBitMap)
/**

  This routine copies the pmem size into the remSize field of each channel in the sockets specified

  @param host         - Pointer to sysHost
  @param socketBitMap - BitMap of sockets to process

  @retval void

**/
{
  UINT8   ch, sockId;
  for (sockId = 0; sockId < MAX_SOCKET; sockId++) {
    //Skip the sockets that are not requested. (FOR NUMA)
    if (((BIT0<<sockId) & socketBitMap) == 0) continue;
    for (ch=0; ch < ( host->var.mem.maxIMC* MAX_MC_CH); ch++) {
      host->var.mem.socket[sockId].channelList[ch].remSize = host->var.mem.socket[sockId].channelList[ch].perSize;
    }
  }
}

VOID
UpdateHostStructForVolMem(
  PSYSHOST   host,
  UINT8     socketBitMap)
/**

  This routine copies the volSize/DDR4 size into the remSize field of each channel in the sockets specified based on the volatile memory mode

  @param host         - Pointer to sysHost
  @param socketBitMap - BitMap of sockets to process

  @retval SUCCESS

**/
{
  UINT8   ch, sockId;
  for (sockId = 0; sockId < MAX_SOCKET; sockId++) {
    //Skip the sockets that are not requested. (FOR NUMA)
    if (((BIT0<<sockId) & socketBitMap) == 0) continue;
    for (ch=0; ch < ( host->var.mem.maxIMC* MAX_MC_CH); ch++) {
      if(host->var.mem.volMemMode == VOL_MEM_MODE_2LM)
        host->var.mem.socket[sockId].channelList[ch].remSize =  host->var.mem.socket[sockId].channelList[ch].volSize;
      else
#ifdef NVMEM_FEATURE_EN
        host->var.mem.socket[sockId].channelList[ch].remSize =  host->var.mem.socket[sockId].channelList[ch].memSize - host->var.mem.socket[sockId].channelList[ch].NVmemSize;
#else
        host->var.mem.socket[sockId].channelList[ch].remSize =  host->var.mem.socket[sockId].channelList[ch].memSize;
#endif
    }
  }
}

#ifdef NVMEM_FEATURE_EN
UINT32 GetSadMemLimit(PSYSHOST host, UINT8 sckt)
{
  UINT32  SadLimit = 0;
  struct  Socket *socket;
  UINT8 i;

  //scan through the SAD table and find the next SAD entry with Enable =0
  socket = &host->var.mem.socket[sckt];
  for (i = 0; i<MAX_SAD_RULES; i++) {
    if (socket->SAD[i].Enable == 0){
      break;
    }
    SadLimit = socket->SAD[i].Limit;
  }
  return SadLimit;
}

UINT32 SocketInterleaveNormalMem (
  PSYSHOST        host, 
  MEMORY_MAP_DATA *MemMapData, 
  UINT16          TOLM, 
  UINT8           memType
)
/**
Interleaves normal DDR4 memory (Non NVMem) on the channel level across sockets. This routine figures out if there is any normal DDR4 memory present in the system and then calls SAD interleave function.
This function works for cases where normal dimms are mixed with Type 01 NVDIMMs and also when AEP dimms are present.
This function does not intereleave memory if the only memory presetn in the system is NVMem.

@param host      - Pointer to sysHost
@param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
@param TOLM      - Top of low memory
@param memType   - Type of memory - 1LM or 2LM

@retval status

**/
{
  UINT32 status = SUCCESS;
  UINT32 CurrentSadLimit = 0;

  host->var.mem.TotalInterleavedMemSize = 0;
  if (memType == MEM_TYPE_1LM) {
    if (host->var.mem.memSize == host->var.mem.NVmemSize) // If only NVMem is present in the system do not interleave here.
      return SUCCESS;
  }
  CurrentSadLimit = GetSadMemLimit(host, 0);
  if (CurrentSadLimit == 0) // There are no entries in the SAD yet;
    CurrentSadLimit = SIZE_4GB_64MB;

  status = SADInterleave(host, MemMapData, TOLM, memType); // Interleave all volatile memory (or 2LM memory)
  // Check if all the normal memory were interleaved. In some cases (like 3 way channel Interleave), not all the memory can be evenly interleaved.
  // In that case there may be some memory left out.
  host->var.mem.TotalInterleavedMemSize = GetSadMemLimit(host,0);
  host->var.mem.TotalInterleavedMemSize = host->var.mem.TotalInterleavedMemSize - CurrentSadLimit;
  return status;
}

UINT32 SocketInterleaveNVMem (
  PSYSHOST        host, 
  MEMORY_MAP_DATA *MemMapData, 
  UINT16          TOLM, 
  UINT8           memType
)
/**

Interleaves NVMem (Type01) on the channel level across sockets. This
routine figures out if there is any NVMem and then calls SAD interleave function.
If there no Type01 NVMEM in the system this function does nothing.

@param host      - Pointer to sysHost
@param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
@param TOLM      - Top of low memory
@param memType   - Type of memory - 1LM or 2LM

@retval status

**/

{
  UINT32 status = SUCCESS;
  UINT8 socketNum, ch = 0;
  UINT8 OrigchInter = 0;
  UINT32 CurrentSadLimit = 0;
  UINT8 NvMemSadIndexStart = 0;
  UINT8 NvMemSadIndexEnd = 0;
  UINT8 i=0;
  UINT8 j=0;

  host->var.mem.TotalInterleavedNVMemSize = 0;
  if (host->var.mem.NVmemSize == 0) // No NVMem in the system. Nothing to interleave.
    return SUCCESS;
  // Load NVMem into remSize on each channel
  for (socketNum = 0; socketNum<MAX_SOCKET; socketNum++) {
    for (ch = 0; ch <(host->var.mem.maxIMC* MAX_MC_CH); ch++) {
      host->var.mem.socket[socketNum].channelList[ch].remSize = host->var.mem.socket[socketNum].channelList[ch].NVmemSize;
    }
  }
  OrigchInter = host->var.mem.chInter;
#ifdef MEM_NVDIMM_EN
  if (host->setup.mem.interNVDIMMS == 0 && host->setup.mem.ADRDataSaveMode == ADR_NVDIMM) {
     host->var.mem.chInter = 1;
  }
#endif
  CurrentSadLimit = GetSadMemLimit(host, 0);
  if (CurrentSadLimit == 0) // There are no entries in the SAD yet;
    CurrentSadLimit = SIZE_4GB_64MB;
  NvMemSadIndexStart = FindSadIndex(host, socketNum);
  status = SADInterleave(host, MemMapData, TOLM, memType);
  NvMemSadIndexEnd = FindSadIndex(host, socketNum);
  for (i = NvMemSadIndexStart; i<NvMemSadIndexEnd; i++) {
   for (j=0; j < MAX_SOCKET; j++) {
    host->var.mem.socket[j].NvMemSadIdxBitMap |= (1 << i);
   }
  }
  host->var.mem.chInter = OrigchInter;
  host->var.mem.TotalInterleavedNVMemSize = GetSadMemLimit(host, 0);
  host->var.mem.TotalInterleavedNVMemSize = host->var.mem.TotalInterleavedNVMemSize - CurrentSadLimit;
  return status;
}

UINT32 SocketNonInterleaveNormalMem (
  PSYSHOST        host, 
  MEMORY_MAP_DATA *MemMapData, 
  UINT8           socketNum, 
  UINT16          TOLM, 
  UINT8           memType 
)
/**

Description: Interleaves normal DDR4 memory if present on the channel level but not across sockets.
This routine figures out the mapping and fills in the structures for the SAD, TAD,
tables.  It does not program any registers.

@param host      - Pointer to sysHost
@param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
@param socketNum   - number of the socket  
@param TOLM      - Top of low memory
@param memType   - Type of memory - 1LM or 2LM

@retval status

**/
{
  UINT32 status = SUCCESS;
  UINT32 CurrentSadLimit = 0;
  struct  Socket *socket;
  socket = &host->var.mem.socket[socketNum];
  socket->TotalInterleavedMemSize = 0;
  if (memType == MEM_TYPE_1LM) {
    if (host->var.mem.socket[socketNum].memSize == host->var.mem.socket[socketNum].NVmemSize) //Only NVMEM is in this socket, do not interleave here.
      return SUCCESS;
  }
  CurrentSadLimit = GetSadMemLimit(host, socketNum);
  if (CurrentSadLimit == 0) { // There are no entries in the SAD yet;
    CurrentSadLimit = SIZE_4GB_64MB;
  }

  status = SADNuma(host, MemMapData, socketNum, TOLM, memType);
  socket->TotalInterleavedMemSize = GetSadMemLimit(host, socketNum) - CurrentSadLimit;
  return status;
}


UINT32 SocketNonInterleaveNVMem (
  PSYSHOST        host, 
  MEMORY_MAP_DATA *MemMapData, 
  UINT8           socketNum, 
  UINT16          TOLM, 
  UINT8           memType
)
/**

Description: Interleaves Type01 NVMEM if present on the channel level but not across sockets.
This routine figures out the mapping and fills in the structures for the SAD, TAD,
tables.  It does not program any registers.

@param host      - Pointer to sysHost
@param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
@param socketNum   - number of the socket    
@param TOLM      - Top of low memory
@param memType   - Type of memory - 1LM or 2LM

@retval status

**/

{
  UINT32 status = SUCCESS;
  UINT8 ch = 0;
  UINT8 OrigImcInter = 0;
  UINT8 OrigChInter = 0;
  UINT32 CurrentSadLimit = 0;
  UINT8 NvMemSadIndexStart = 0;
  UINT8 NvMemSadIndexEnd = 0;
  UINT8 i=0;


  struct  Socket *socket;
  socket = &host->var.mem.socket[socketNum];
  socket->TotalInterleavedNVMemSize = 0;

  if (host->var.mem.socket[socketNum].NVmemSize == 0) // No NVMem in the system. Nothing to interleave.
    return SUCCESS;

  for (ch = 0; ch < (host->var.mem.maxIMC* MAX_MC_CH); ch++) {
    host->var.mem.socket[socketNum].channelList[ch].remSize = host->var.mem.socket[socketNum].channelList[ch].NVmemSize;
  }
  OrigImcInter = host->var.mem.imcInter;
  OrigChInter = host->var.mem.chInter;
#ifdef MEM_NVDIMM_EN
  if (host->setup.mem.interNVDIMMS == 0 && host->setup.mem.ADRDataSaveMode == ADR_NVDIMM) {
      host->var.mem.imcInter = 1;
      host->var.mem.chInter = 1;
  }
#endif
  CurrentSadLimit = GetSadMemLimit(host, 0);
  if (CurrentSadLimit == 0) // There are no entries in the SAD yet;
    CurrentSadLimit = SIZE_4GB_64MB;

  NvMemSadIndexStart = FindSadIndex(host, socketNum);
  status = SADNuma(host, MemMapData, socketNum, TOLM, memType);
  host->var.mem.imcInter = OrigImcInter;
  host->var.mem.chInter = OrigChInter;
  socket->TotalInterleavedNVMemSize = GetSadMemLimit(host, socketNum);
  socket->TotalInterleavedNVMemSize = socket->TotalInterleavedNVMemSize - CurrentSadLimit;
  NvMemSadIndexEnd = FindSadIndex(host, socketNum);
  for (i = NvMemSadIndexStart; i<NvMemSadIndexEnd; i++) {
    host->var.mem.socket[socketNum].NvMemSadIdxBitMap |= (1 << i);
  }
  MemDebugPrint ((host, SDBG_MAX, socketNum, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                 "NvMemSadIndexStart = %d, NvMemSadIndexEnd = %d,NvMemSadIdxBitMap = 0x%x\n",
                 NvMemSadIndexStart, NvMemSadIndexEnd,host->var.mem.socket[socketNum].NvMemSadIdxBitMap ));
  return status;
}
#endif

UINT32 SocketInterleave (
  PSYSHOST        host, 
  MEMORY_MAP_DATA *MemMapData, 
  UINT16          TOLM
)
/**

  Interleaves memory on the channel level across sockets. This

  routine figures out the mapping and fills in the structures for the SAD, TAD,
  and SAG tables.  It does not program any registers.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @param TOLM      - Top of low memory

  @retval status

**/
{
  UINT8  memType = 0;
  UINT32 status = SUCCESS;
  // Determine volatile memory mode
  if(host->var.mem.volMemMode == VOL_MEM_MODE_2LM)
    memType = MEM_TYPE_2LM;
  else
    memType = MEM_TYPE_1LM;

  UpdateHostStructForVolMem(host, SOCKET_BITMAP_ALL);

  // Map the volatile memory
  OutputExtendedCheckpoint((host, STS_DDR_MEMMAP, SUB_SAD_INTERLEAVE, 0));

#ifdef NVMEM_FEATURE_EN
  // Interleave Normal memory dimms first and then NvMem
  status = SocketInterleaveNormalMem(host, MemMapData, TOLM, memType);
  if (status != SUCCESS) return status;

  status = SocketInterleaveNVMem(host, MemMapData, TOLM, memType);
  if (status != SUCCESS) return status;

#else
  status = SADInterleave(host, MemMapData, TOLM, memType);
  if(status != SUCCESS) return status;
#endif
  //Interleave PMEM/PMEM$ only if the dimms are controlled by SV only setup options not the CR Management driver.
  if(host->var.mem.dfxMemVars.dfxDimmManagement != DIMM_MGMT_CR_MGMT_DRIVER) {

    // Check if Pmem is possible and that Pmem mapping is enabled
    if(IsPmemPossible(host, SOCKET_BITMAP_ALL) && !( host->var.mem.dfxMemVars.dfxPerMemMode == NON_PER_MEM_MODE ) ) {

      if(host->var.mem.dfxMemVars.dfxPerMemMode == PER_MEM_MODE )  {
        memType = MEM_TYPE_PMEM;
      }
      else if(host->var.mem.dfxMemVars.dfxPerMemMode == PER_MEM_CACHE_MODE )  {
        memType = MEM_TYPE_PMEM_CACHE;
      }

      //Update the remsize information in the host struct for this socket before calling SADNuma() to create PMEM SADs.
      UpdateHostStructForPmem(host, SOCKET_BITMAP_ALL);

      status = SADInterleave(host, MemMapData, TOLM, memType);
      if(status != SUCCESS) return status;
    }
  }
  //DisplaySADTable(host);

  return SUCCESS;
}

/**

  Description: Check if near memory population matches between IMCs

  @param host  - Pointer to sysHost
  @param socketNum - socket id

  @retval True -  Near Memory population matches between IMCs
  @retval False - Near Memory population does not match between IMCs

**/
BOOLEAN
checkNMPopulation(
  PSYSHOST host,
  UINT8 socketNum
  )

{
  UINT8  mc;
  UINT8  ch;
  UINT8  channelbitmap[MAX_IMC] = { 0 };
  UINT8  numberofchannels[MAX_IMC] = { 0 };
  struct ddrChannel *channelList = NULL;

  channelList = &host->var.mem.socket[socketNum].channelList[0];

  // Find the number of channels with ddr4 memory populated and their bit map in each iMC
  for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
    for (ch = 0; ch < MAX_MC_CH; ch++) {
      if (channelList[ch + (mc * MAX_MC_CH)].memSize != 0) {
        channelbitmap[mc] |= BIT0 << ch;
        numberofchannels[mc]++;
      }
    }
  }

  // check number of channels populated on both imc is the same and for skylake processor additionally check channel bit maps are the same
  for (mc = 0; mc < host->var.mem.maxIMC; mc = mc + host->var.mem.maxIMC) {
    if (numberofchannels[mc] != numberofchannels[mc + 1] || (host->var.common.cpuType == CPU_SKX && channelbitmap[mc] != channelbitmap[mc + 1])) {
      return FALSE;
    }
  }
  return TRUE;
}


/**

  Description: Check if NUMA interleave is possible between the imcs in a socket

  @param host  - Pointer to sysHost
  @param socketNum - socket id
  @param way   - two way/threeway/one way interleave


  @retval True -  Interleave possible
  @retval False -  Interleave possible

**/
BOOLEAN
IsInterleavePossible(
  PSYSHOST host,
  UINT8 memType,
  UINT8 socketNum,
  UINT8 way
  )
{
  UINT8  mcIndex;
  UINT8  chIndex;
  UINT8  numberofchannels[MAX_IMC] = { 0 };
  UINT8  counter = 0;
  struct ddrChannel *channelList = NULL;

  if ( memType == MEM_TYPE_2LM || memType == MEM_TYPE_PMEM_CACHE ){
    if ( !checkNMPopulation( host, socketNum ) )
      return FALSE;
  }

  channelList = &host->var.mem.socket[socketNum].channelList[0];

  // Find the number of channels with memory populated in each iMC
  for ( mcIndex = 0; mcIndex < host->var.mem.maxIMC; mcIndex++ ) {
    for ( chIndex = 0; chIndex < MAX_MC_CH; chIndex++ ) {
      if (channelList[chIndex + (mcIndex * MAX_MC_CH)].remSize != 0) {
        numberofchannels[mcIndex]++;
      }
    }
  }

  // check number of channels populated on both imc is >= input channel ways
  for (mcIndex = 0; mcIndex < host->var.mem.maxIMC && mcIndex < MAX_MC; mcIndex ++) {
    if (numberofchannels[mcIndex] < way) {
      return FALSE;
    }
  }
  // for skylake processor additionally check channel numbers are the same between iMCs
  if (host->var.common.cpuType == CPU_SKX) {
    for ( chIndex = 0; chIndex < MAX_MC_CH; chIndex++ ) {
      if (channelList[chIndex].remSize != 0 && channelList[chIndex + MAX_MC_CH].remSize != 0) {
        counter++;
      }
    }
    if (counter < way) {
      return FALSE;
    }
  }

  return TRUE;
}

UINT32
DoThreeWayInterleave(
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8           socketNum,
  UINT16          TOLM,
  UINT8           memType
)
/**

  Description: Do three way NUMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum - socket id
  @param TOLM  - Top of Low Memory
  @param memType   - Type of memory - 1LM or 2LM

    @retval status
**/
{
  UINT8  channelIndex, sadIndex, imcWays, chWays, channelInterBitmap, totalWays, mc=0;
  UINT16 interleaveSize;
  UINT32 nodeLimit =0;
  struct ddrChannel *channelList;
  struct Socket  *socket;
  UINT32 MemoryToBeAllocated = 0;
  UINT8 McList[MAX_SOCKET*MAX_IMC] = {0};

  sadIndex = 0;
  channelInterBitmap = 0;
  imcWays = TWO_WAY;
  chWays = THREE_WAY;
  socket = &host->var.mem.socket[socketNum];
  channelList = &host->var.mem.socket[socketNum].channelList[0];
  interleaveSize = channelList[0].remSize;


  //Calculate the interleave size for 3 way interleave
  for (channelIndex=0; channelIndex < ( host->var.mem.maxIMC* MAX_MC_CH); channelIndex++){
    if (channelList[channelIndex].remSize < interleaveSize)
      interleaveSize = channelList[channelIndex].remSize;
  }
  if(memType == MEM_TYPE_2LM || memType == MEM_TYPE_PMEM_CACHE){
    //Calculate the Channel interleave bitmap based on Near Memory population for 2LM mode.
    channelInterBitmap = FindNMBitMap(host, socketNum, mc);
  }
  //Find the Index for next available SAD entry
  sadIndex = FindSadIndex(host, socketNum);
  // Should not reach end of SAD rules before allocating all memory, Issue warning and break  loop.
  // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
  if (sadIndex >= MAX_SAD_RULES) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD rules exceeds %d\n", MAX_SAD_RULES));
    OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
     return ERROR_RESOURCE_CALCULATION_FAILURE;
  }

  //Create a SAD entry in the host structure
  if(sadIndex)
    socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + (imcWays * chWays * interleaveSize);
  else
    socket->SAD[sadIndex].Limit = imcWays * chWays * interleaveSize;

  nodeLimit = socket->SAD[sadIndex].Limit;
  if (nodeLimit != 0) {
    // fill out what all MC are participating/populated in the socket
	McList[socketNum *  host->var.mem.maxIMC] = 1;
	McList[(socketNum *  host->var.mem.maxIMC)+MC1] = 1;
  }
  //Adjust SAD Limit for Memory hole
  if(socket->SAD[sadIndex].Limit >=TOLM) {
    if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0)){
      socket->SAD[sadIndex].Enable = 1;
      socket->SAD[sadIndex].ways  = TWO_WAY;
      socket->SAD[sadIndex].type = memType;
      socket->SAD[sadIndex].imcInterBitmap =  BIT1 | BIT0;
      socket->SAD[sadIndex].local = 1;
      socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].Attr = 0;
      socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

      if (IsSkuLimitViolation(host, MemMapData, TOLM, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
        OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
        FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
      }
      nodeLimit += FOUR_GB_MEM - TOLM;

      // If TOLM boundary is not evenly divisible when interleaving across three channels of two memory controllers, then we need to adjust the node limit so that it divides evenly
      totalWays = socket->SAD[sadIndex].ways * 3 ;
      if( TOLM % totalWays ) {

        //5332821: Partial Mirroring with 3Chway population causes Memicals data miscompare
        if ((host->nvram.mem.RASmode & PARTIAL_MIRROR_1LM)||(host->nvram.mem.RASmode & PARTIAL_MIRROR_2LM)) {
          if(socket->SAD[sadIndex].ways == 1) {
            totalWays = totalWays*2;
          }
        }

        // TOLM boundary is a multiple of 64MB less than a boundary evenly divisible by totalWays, so adjust nodeLimit by that multiple in 64MB units, so that it divides evenly
        nodeLimit = nodeLimit - ( totalWays - ( TOLM % totalWays )) ;

      }

      switch(memType) {
        case MEM_TYPE_1LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].channelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
          }
          break;
        case MEM_TYPE_2LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
          break;
        case MEM_TYPE_PMEM :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "INVALID CONFIGURATION. The first SAD rule cannot be PMEM, PMEM$, BLK or CTRL region\n"));
#if SMCPKG_SUPPORT
          OutputError (host, ERR_NGN, NGN_PMEM_CONFIG_ERROR, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
          FatalError(host, ERR_NGN, NGN_PMEM_CONFIG_ERROR);
#endif
          break;
      }

      // Create Interleave List for 2 HAs
      socket->SADintList[sadIndex][socketNum *  host->var.mem.maxIMC] = 1;
      socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + 1] = 1;

      //Increment the sadIndex
      sadIndex++;
    }
  }

  if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN) {
    if ((sadIndex > 0) && (sadIndex < MAX_SAD_RULES)) {
      if(socket->SAD[sadIndex -1].Limit == FOUR_GB_MEM) {
        // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
        socket->SAD[sadIndex].Limit = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].Attr = SAD_NXM_ATTR;
        socket->SAD[sadIndex].local = 0;
        sadIndex++;
        nodeLimit = (nodeLimit + socket->SAD[sadIndex-1].Limit) - socket->SAD[sadIndex-2].Limit;
      }
    }
  }

  if ( nodeLimit > host->var.mem.mmiohBase) {
     if ((sadIndex > 0) && (socket->SAD[sadIndex - 1].Limit == host->var.mem.mmiohBase)) {
        MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "WARNING!!! Failure to create Sad%d! Sad%d limit is MMIOHBase...\n", sadIndex, sadIndex - 1 ));
        return FAILURE;
     }
     nodeLimit = host->var.mem.mmiohBase;
     MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "WARNING!!! SAD%d limit exceeds MMIOHBase! Sad%d is truncated to MMIOHBase...\n", sadIndex, sadIndex));
  }
  if(sadIndex < MAX_SAD_RULES) {
    // OutSncGbAlignRequired = 1(enabled) AND 1LM or 2LM
    if ((host->var.kti.OutSncGbAlignRequired == 1) && (memType & (MEM_TYPE_1LM | MEM_TYPE_2LM))) {
      nodeLimit = (nodeLimit & GB_BOUNDARY_ALIGN); //align to GB boundary
      // Check size of memory mapped by this entry to insure that all six channels contributes equally
      while ((sadIndex > 0) && ((nodeLimit - socket->SAD[sadIndex-1].Limit) % 6)) {
        // Each channel did not have an equal contribution, so try next GB boundary
        nodeLimit = nodeLimit - 0x10;
      }
    }
      if ( sadIndex ) {
        MemoryToBeAllocated = nodeLimit - socket->SAD[sadIndex - 1].Limit;
      }

      if (IsSkuLimitViolation(host, MemMapData, MemoryToBeAllocated, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
        OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
        FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
      }
      socket->SAD[sadIndex].Limit = nodeLimit;
      socket->SAD[sadIndex].Enable = 1;
      socket->SAD[sadIndex].ways  = TWO_WAY;
      socket->SAD[sadIndex].type = memType;
      socket->SAD[sadIndex].imcInterBitmap =  BIT1 | BIT0;
      socket->SAD[sadIndex].local = 1;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].Attr = 0;
      socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

      switch(memType) {
        case MEM_TYPE_1LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].channelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
          }
          break;
        case MEM_TYPE_2LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
          break;
        case MEM_TYPE_PMEM :
          socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
          }
          break;
        case MEM_TYPE_PMEM_CACHE :
          socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BITMAP_CH0_CH1_CH2;
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
          break;
      }
      // Create Interleave List for 2 HAs
      socket->SADintList[sadIndex][socketNum *  host->var.mem.maxIMC] = 1;
      socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + 1] = 1;
    }
  //Subtract the interleave size from the remsize variable
  for (channelIndex=0; channelIndex<MAX_MC_CH *  host->var.mem.maxIMC; channelIndex++){
    channelList[channelIndex].remSize -= interleaveSize;
  }
  return SUCCESS;
}


UINT32
DoTwoWayInterleave(
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8           socketNum,
  UINT16          TOLM,
  UINT8           memType
  )
/**

  Description: Do two way NUMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum - socket id
  @param TOLM  - Top of Low Memory
  @param memType  - Type of memory - 1LM or 2LM

  @retval status
**/
{
  UINT8  channelIndex, sadIndex, imcWays, chWays, skipChannelIndex, channelInterBitmap, FMchannelInterBitmap, mc=0;
  UINT16 interleaveSize;
  UINT32 nodeLimit = 0;
  struct ddrChannel *channelList;
  struct Socket  *socket;
  UINT32 MemoryToBeAllocated = 0;
  UINT8 McList[MAX_SOCKET*MAX_IMC] = {0};
  sadIndex = 0;
  skipChannelIndex = 0;
  channelInterBitmap =0;
  FMchannelInterBitmap = 0;
  imcWays = TWO_WAY;
  chWays = TWO_WAY;
  socket = &host->var.mem.socket[socketNum];
  channelList = &host->var.mem.socket[socketNum].channelList[0];

  //Find the Channels to skip in each imc - the one with no memory or its equivalent channel in the other imc
  for (channelIndex=0; channelIndex<MAX_MC_CH; channelIndex++){
    if((channelList[channelIndex].remSize ==0) || (channelList[channelIndex + MAX_MC_CH].remSize ==0)){
      skipChannelIndex = channelIndex;
      break;
    }
  }

  // Select any other channel than the skippped channel for beginning interleave computation
  interleaveSize =channelList[(skipChannelIndex + 1) % MAX_MC_CH].remSize;

  //Calculate the interleave size for 2 way interleave
  for (channelIndex=0; channelIndex < MAX_MC_CH *  host->var.mem.maxIMC; channelIndex++){
    if((channelIndex == skipChannelIndex) || (channelIndex == skipChannelIndex+ MAX_MC_CH)) continue;
    if (channelList[channelIndex].remSize < interleaveSize)
      interleaveSize = channelList[channelIndex].remSize;
  }

  //Find the Index for next available SAD entry
  sadIndex = FindSadIndex(host, socketNum);
  // Should not reach end of SAD rules before allocating all memory, Issue warning and break TAD loop.
  // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
  if (sadIndex >= MAX_SAD_RULES) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD rules exceeds %d\n", MAX_SAD_RULES));
    OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
    return ERROR_RESOURCE_CALCULATION_FAILURE;
  }

  //Subtract the interleave size in the host structure variables
  for (channelIndex=0; channelIndex < MAX_MC_CH * MAX_IMC; channelIndex++) {
    if ((channelIndex == skipChannelIndex) || (channelIndex == skipChannelIndex + MAX_MC_CH)) {
      continue;
    }
    channelList[channelIndex].remSize -= interleaveSize;
    if(channelIndex < MAX_MC_CH)  {
      if (memType == MEM_TYPE_2LM || memType == MEM_TYPE_PMEM_CACHE || memType == MEM_TYPE_PMEM)
        FMchannelInterBitmap = FMchannelInterBitmap | (BIT0<<channelIndex);
      else
        channelInterBitmap = channelInterBitmap | (BIT0<<channelIndex);
    }
  }

  if(memType == MEM_TYPE_2LM || memType == MEM_TYPE_PMEM_CACHE){
    //Calculate the Channel interleave bitmap based on Near Memory population for 2LM mode.
    channelInterBitmap = FindNMBitMap(host, socketNum, mc);
  }

  //Create a SAD entry in the host structure
  if(sadIndex)
    socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + (imcWays * chWays * interleaveSize);
  else
    socket->SAD[sadIndex].Limit = imcWays * chWays * interleaveSize;

  nodeLimit = socket->SAD[sadIndex].Limit;

  if (nodeLimit != 0) {
    // fill out what all MC are participating/populated in the socket
	McList[socketNum *  host->var.mem.maxIMC] = 1;
	McList[(socketNum *  host->var.mem.maxIMC)+MC1] = 1;
  }

  //Adjust SAD Limit for Memory hole
  if(socket->SAD[sadIndex].Limit >=TOLM) {
    if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0)){
      socket->SAD[sadIndex].Enable = 1;
      socket->SAD[sadIndex].ways  = TWO_WAY;
      socket->SAD[sadIndex].type = memType;
      for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
        socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
      }
      socket->SAD[sadIndex].imcInterBitmap =  BIT1 | BIT0;
      socket->SAD[sadIndex].local = 1;
      socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].Attr = 0;
      socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

      if (IsSkuLimitViolation(host, MemMapData, TOLM, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
        OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
        FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
      }
      nodeLimit += FOUR_GB_MEM - TOLM;
      switch(memType) {
        case MEM_TYPE_1LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
          break;
        case MEM_TYPE_2LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
          }
          break;
        case MEM_TYPE_PMEM :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_CTRL :
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "INVALID CONFIGURATION. The first SAD rule cannot be PMEM, PMEM$, BLK or CTRL region\n"));
#if SMCPKG_SUPPORT
          OutputError (host, ERR_NGN, NGN_PMEM_CONFIG_ERROR, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
          FatalError(host, ERR_NGN, NGN_PMEM_CONFIG_ERROR);
#endif
          break;
      }
      // Create Interleave List for 2 HAs
      socket->SADintList[sadIndex][socketNum *  host->var.mem.maxIMC] = 1;
      socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + 1] = 1;

      //Increment the Sad Index
      sadIndex++;
    }
  }

  if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN ) {
   if ((sadIndex > 0) && (sadIndex < MAX_SAD_RULES)) {
      if(socket->SAD[sadIndex -1].Limit == FOUR_GB_MEM) {
        // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
        socket->SAD[sadIndex].Limit = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].Attr = SAD_NXM_ATTR;
        socket->SAD[sadIndex].local = 0;
        sadIndex++;
        nodeLimit = (nodeLimit + socket->SAD[sadIndex-1].Limit) - socket->SAD[sadIndex-2].Limit;
      }
    }
  }

  if ( nodeLimit > host->var.mem.mmiohBase) {
     if ((sadIndex > 0) && (socket->SAD[sadIndex - 1].Limit == host->var.mem.mmiohBase)) {
        MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "WARNING!!! Failure to create Sad%d! Sad%d limit is MMIOHBase...\n", sadIndex, sadIndex - 1 ));
        return FAILURE;
     }
     nodeLimit = host->var.mem.mmiohBase;
     MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "WARNING!!! SAD%d limit exceeds MMIOHBase! Sad%d is truncated to MMIOHBase...\n", sadIndex, sadIndex));
  }

  if ( sadIndex < MAX_SAD_RULES) {

    if ( sadIndex ) {
      MemoryToBeAllocated = nodeLimit - socket->SAD[sadIndex - 1].Limit;
    }
    if (IsSkuLimitViolation(host, MemMapData, MemoryToBeAllocated, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
      OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
      FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
    }

    socket->SAD[sadIndex].Limit = nodeLimit;
    socket->SAD[sadIndex].Enable = 1;
    socket->SAD[sadIndex].ways  = TWO_WAY;
    socket->SAD[sadIndex].type = memType;
    for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
      socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
    }
    socket->SAD[sadIndex].imcInterBitmap =  BIT1 | BIT0;
    socket->SAD[sadIndex].local = 1;
    socket->SAD[sadIndex].mirrored = 0;
    socket->SAD[sadIndex].Attr = 0;
    socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

    switch(memType) {
      case MEM_TYPE_1LM :
        socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
        break;
      case MEM_TYPE_2LM :
        socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
        for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
          socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
        }
        break;
      case MEM_TYPE_PMEM :
        socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
        for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
          socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
        }
        break;
      case MEM_TYPE_PMEM_CACHE :
        socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
        for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
          socket->SAD[sadIndex].FMchannelInterBitmap[mc] = FMchannelInterBitmap;
        }
        break;
    }

    // Create Interleave List for 2 HAs
    socket->SADintList[sadIndex][socketNum *  host->var.mem.maxIMC] = 1;
    socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + 1] = 1;

  }

  return SUCCESS;
}

UINT32
DoOneWayInterleave(
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8           socketNum,
  UINT16          TOLM,
  UINT8           memType
  )
/**

  Description: Do one way NUMA interleave

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)    
  @param socketNum - socket id
  @param TOLM  - Top of Low Memory
  @param memType  - Type of memory - 1LM or 2LM


    @retval status
**/
{
  UINT8  channelIndex, sadIndex, imcWays, chWays, targetChannel, channelInterBitmap, mc=0;
  UINT16 interleaveSize;
  UINT32 nodeLimit=0;
  struct ddrChannel *channelList;
  struct Socket  *socket;
  UINT32 MemoryToBeAllocated = 0;
  UINT8 McList[MAX_SOCKET*MAX_IMC] = {0};

  sadIndex = 0;
  targetChannel=0;
  channelInterBitmap = 0;
  imcWays = TWO_WAY;
  chWays = ONE_WAY;
  socket = &host->var.mem.socket[socketNum];
  channelList = &host->var.mem.socket[socketNum].channelList[0];

  //Find the Channels to interleave in each imc
  for (channelIndex=0; channelIndex<MAX_MC_CH; channelIndex++){
    if((channelList[channelIndex].remSize != 0) && (channelList[channelIndex + MAX_MC_CH].remSize !=0)) {
      targetChannel = channelIndex; break;
      }
  }
  if(memType == MEM_TYPE_2LM || memType == MEM_TYPE_PMEM_CACHE){
    //Calculate the Channel interleave bitmap based on Near Memory population for 2LM mode.
    channelInterBitmap = FindNMBitMap(host, socketNum, mc);
  }

  //Find the interleave size for the oneway interleave
  if(channelList[targetChannel].remSize < channelList[targetChannel + MAX_MC_CH].remSize)
    interleaveSize = channelList[targetChannel].remSize;
  else
    interleaveSize = channelList[targetChannel + MAX_MC_CH].remSize;


  //Find the Index for next available SAD entry
  sadIndex = FindSadIndex(host, socketNum);
  // Should not reach end of SAD rules before allocating all memory, Issue warning and break TAD loop.
  // Only check if non-zero amount of memory left to be allocated as indicated by sortNum != 0.
  if (sadIndex >= MAX_SAD_RULES) {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "SAD rules exceeds %d\n", MAX_SAD_RULES));
    OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_SAD_RULES_EXCEEDED, socketNum, 0xFF, 0xFF, 0xFF);
    return ERROR_RESOURCE_CALCULATION_FAILURE;
  }

  //Create a SAD entry in the host structure
  if(sadIndex)
    socket->SAD[sadIndex].Limit = socket->SAD[sadIndex-1].Limit + (imcWays * chWays * interleaveSize);
  else
    socket->SAD[sadIndex].Limit = imcWays * chWays * interleaveSize;

  nodeLimit = socket->SAD[sadIndex].Limit;
  if (nodeLimit != 0) {
    // fill out what all MC are participating/populated in the socket
	McList[socketNum *  host->var.mem.maxIMC] = 1;
	McList[(socketNum *  host->var.mem.maxIMC)+MC1] = 1;
  }

  //Adjust SAD Limit for Memory hole
  if(socket->SAD[sadIndex].Limit >=TOLM) {
    if(((sadIndex != 0) && (socket->SAD[sadIndex-1].Limit<TOLM)) || (sadIndex == 0)){
      socket->SAD[sadIndex].Enable = 1;
      socket->SAD[sadIndex].ways  = TWO_WAY;
      socket->SAD[sadIndex].type = memType;
      socket->SAD[sadIndex].imcInterBitmap =  BIT1 | BIT0;
      socket->SAD[sadIndex].local = 1;
      socket->SAD[sadIndex].Limit = FOUR_GB_MEM;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].Attr = 0;
      socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );

      if (IsSkuLimitViolation(host, MemMapData, TOLM, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
        OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
        FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
      }
      nodeLimit += FOUR_GB_MEM - TOLM;
      switch(memType) {
        case MEM_TYPE_1LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].channelInterBitmap[mc] = BIT0<<targetChannel;
          }
          break;
        case MEM_TYPE_2LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BIT0<<targetChannel;
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
          break;
        case MEM_TYPE_PMEM :
        case MEM_TYPE_PMEM_CACHE :
        case MEM_TYPE_BLK_WINDOW :
        case MEM_TYPE_CTRL :
          MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "INVALID CONFIGURATION. The first SAD rule cannot be PMEM, PMEM$, BLK or CTRL region\n"));
#if SMCPKG_SUPPORT
          OutputError (host, ERR_NGN, NGN_PMEM_CONFIG_ERROR, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
          FatalError(host, ERR_NGN, NGN_PMEM_CONFIG_ERROR);
#endif
          break;
      }

      // Create Interleave List for 2 HAs
      socket->SADintList[sadIndex][socketNum *  host->var.mem.maxIMC] = 1;
      socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + 1] = 1;

      //Increment the SAD Index
      sadIndex++;
    }
  }

  if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN ) {
    if ((sadIndex > 0) && (sadIndex < MAX_SAD_RULES)) {
      if (socket->SAD[sadIndex -1].Limit == FOUR_GB_MEM) {
        // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
        socket->SAD[sadIndex].Limit = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
        socket->SAD[sadIndex].Enable = 1;
        socket->SAD[sadIndex].Attr = SAD_NXM_ATTR;
        socket->SAD[sadIndex].local = 0;
        sadIndex++;
        nodeLimit = (nodeLimit + socket->SAD[sadIndex-1].Limit) - socket->SAD[sadIndex-2].Limit;
      }
    }
  }

  if ( nodeLimit > host->var.mem.mmiohBase) {
     if ((sadIndex > 0) && (socket->SAD[sadIndex - 1].Limit == host->var.mem.mmiohBase)) {
        MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                         "WARNING!!! Failure to create Sad%d! Sad%d limit is MMIOHBase...\n", sadIndex, sadIndex - 1 ));
        return FAILURE;
     }
     nodeLimit = host->var.mem.mmiohBase;
     MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                             "WARNING!!! SAD%d limit exceeds MMIOHBase! Sad%d is truncated to MMIOHBase...\n", sadIndex, sadIndex));
  }
  if ( sadIndex < MAX_SAD_RULES) {
      if ( sadIndex ) {
        MemoryToBeAllocated = nodeLimit - socket->SAD[sadIndex - 1].Limit;
      }

      if (IsSkuLimitViolation(host, MemMapData, MemoryToBeAllocated, memType, McList) == TRUE) {
#if SMCPKG_SUPPORT
        OutputError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION, socketNum, NO_CH, NO_DIMM, NO_RANK);
#else
        FatalError (host, ERR_SKU_LIMIT, ERR_SKU_LIMIT_VIOLATION);
#endif
      }

      socket->SAD[sadIndex].Limit = nodeLimit;
      socket->SAD[sadIndex].Enable = 1;
      socket->SAD[sadIndex].ways  = TWO_WAY;
      socket->SAD[sadIndex].type = memType;
      socket->SAD[sadIndex].imcInterBitmap =  BIT1 | BIT0;
      socket->SAD[sadIndex].local = 1;
      socket->SAD[sadIndex].mirrored = 0;
      socket->SAD[sadIndex].Attr = 0;
      socket->SAD[sadIndex].tgtGranularity = GetTargetInterleaveGranularity( host, memType );
      switch(memType) {
        case MEM_TYPE_1LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran1LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].channelInterBitmap[mc] = BIT0<<targetChannel;
          }
          break;
        case MEM_TYPE_2LM :
          socket->SAD[sadIndex].granularity = host->var.mem.memInterleaveGran2LM;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BIT0<<targetChannel;
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
          break;
        case MEM_TYPE_PMEM :
          socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BIT0<<targetChannel;
          }
          break;
        case MEM_TYPE_PMEM_CACHE :
          socket->SAD[sadIndex].granularity = host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA;
          for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
            socket->SAD[sadIndex].FMchannelInterBitmap[mc] = BIT0<<targetChannel;
            socket->SAD[sadIndex].channelInterBitmap[mc] = channelInterBitmap;
          }
          break;
      }

      // Create Interleave List for 2 HAs
      socket->SADintList[sadIndex][socketNum *  host->var.mem.maxIMC] = 1;
      socket->SADintList[sadIndex][(socketNum *  host->var.mem.maxIMC) + 1] = 1;
  }

  //Subtract the remsize variable based on the interleave size in corresponding channels.
  channelList[targetChannel].remSize -= interleaveSize;
  channelList[targetChannel + MAX_MC_CH].remSize -= interleaveSize;

  return SUCCESS;
}

UINT32 SADNuma (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8           socketNum,
  UINT16          TOLM,
  UINT8           memType
)
/**

  Interleaves memory on the mc and channel level but not across sockets for NUMA configuration.
  This routine figures out the mapping and fills in the structures for the SAD and TAD
  tables.  It does not program any registers.

  @param host                - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum           - Socket number
  @param memType             - Type of memory being interleaved
  @param TOLM                - Top of low memory
  @param memType     - Type of memory - 1LM or 2LM
  
  @retval status

**/
{
  UINT32  status = SUCCESS, nodeLimit = 0;
  UINT8   i;
  UINT8   sadIndex;
  UINT16  map[MAX_SOCKET * MAX_IMC];
  UINT8   maxSADRules = MAX_SAD_RULES;

  //
  //Steps:
  //1. For each socket check if threeway interleave is possible between the IMCs
  //2. If possible, Do three(channel) way interleave between IMCs within the socket.
  //3. If possible, Do a two way interleave, keep repeating until no more 2 (ch)way interleave is possible between IMCs
  //4. If possible do 1 way interleave between imcs in the socket, keep repeating until no more 1 (ch) way interleave is possible.
  //5. With the remeaining memory try 3 way, 2 way and one way channel interleave within each imc until all the memory is allocated.
  //


  //interleave between IMCs if ImcInterleave 2 and both iMCs are enabled
  if ((host->var.mem.imcInter == 2) && (host->var.mem.maxIMC > 1)) {
    if(host->var.mem.chInter == 3){
      //Do 3 way interleave between IMCs of the socket if possible
      if(IsInterleavePossible(host, memType, socketNum, THREE_WAY)){
        status = DoThreeWayInterleave(host, MemMapData, socketNum, TOLM, memType);
        if(status != SUCCESS) return status;
      }
    }
    if(host->var.mem.chInter >= 2){
      //Do 2 way interleave between IMCs of the socket if possible and repeat
      while(IsInterleavePossible(host, memType, socketNum, TWO_WAY)) {
        status = DoTwoWayInterleave(host, MemMapData, socketNum, TOLM, memType);
        if(status != SUCCESS) return status;
      }
    }

    //Do 1 way interleave between IMCs of the socket if possible and repeat
    while(IsInterleavePossible(host, memType, socketNum, ONE_WAY)) {
      status =DoOneWayInterleave(host, MemMapData, socketNum, TOLM, memType);
      if(status != SUCCESS) return status;
    }

    //Add remote SAD entry here since the IMC targets are the same until now
    //Get the Limit value for this node to create remote sad entry in other nodes
    sadIndex = FindSadIndex(host, socketNum);
    if(sadIndex > 0 && sadIndex < maxSADRules) {
      nodeLimit = host->var.mem.socket[socketNum].SAD[sadIndex-1].Limit;
      //Create the proper imc target map
      for(i=0; i<MAX_SOCKET *  host->var.mem.maxIMC; i++) {
        //Clear the map
        map[i]=0;
        //For IMCs belonging to the current socket, set the target
        if((i/ host->var.mem.maxIMC)==socketNum) map[i] = 1;
      }
      //create remote SAD entries for the other sockets
      status = AddRemoteSadEntry(host, map, nodeLimit, memType);
      if(status != SUCCESS) return status;
    }
  }
  //Do Channel interleave within IMC
  status = DoChannelInterleave(host, MemMapData, socketNum, TOLM, memType, &nodeLimit, INTER_BELOW_4GB_DIS);
  if(status != SUCCESS) return status;
  //
  // Call BIOS/VCU Mailbox API; sending to SetIotMemoryBuffer() the Socket ID on which each IOT rule exists
  // The addresses of the IOT rules need to be reported to a VCU via a newly defined BIOS/VCU mailbox interface
  //
  //for (sadIndex = 0; sadIndex < maxSADRules; sadIndex++) {
  //  if (mem->SAD[sadIndex].IotEnabled == 0) continue;

  ///  nodeID = mem->SADintList[sadIndex][0];
  //  Socket = (nodeID & 3);
  //  SetIotMemoryBuffer(host, Socket); // Calling the function which reports the addresses of the IOT buffers to the VCU
  //}

  return SUCCESS;

} // SAD NonInterleave

UINT32
SocketNonInterleavePerMem (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8           socketNum,
  UINT16          TOLM
  )
/**

  Interleave persistent memory on IMC and channel level but not across sockets for NUMA configuration.

  @param host                - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum           - Socket number
  @param TOLM                - Top of low memory

  @retval status

**/
{
  UINT8   memType = NON_PER_MEM_MODE;
  UINT8   socketBitMap;

  //Interleave PMEM/PMEM$ only if the dimms are controlled by SV only setup options not the CR Management driver.
  if (host->var.mem.dfxMemVars.dfxDimmManagement != DIMM_MGMT_CR_MGMT_DRIVER) {
    socketBitMap = BIT0 << socketNum;

    // Check if Pmem is possible and that Pmem mapping is enabled
    if (IsPmemPossible(host, socketBitMap) && host->var.mem.dfxMemVars.dfxPerMemMode != NON_PER_MEM_MODE) {

      if (host->var.mem.dfxMemVars.dfxPerMemMode == PER_MEM_MODE)  {
        memType = MEM_TYPE_PMEM;
      } else if (host->var.mem.dfxMemVars.dfxPerMemMode == PER_MEM_CACHE_MODE)  {
        memType = MEM_TYPE_PMEM_CACHE;
      }

      // Skip if memType is PMEM/PMEM$ and there is no NVMDIMM persistent memory on this socket
      if (((memType == MEM_TYPE_PMEM) || (memType == MEM_TYPE_PMEM_CACHE)) && (host->var.mem.socket[socketNum].perSize == 0)) {
        return RETRY;
      }

      //Update the remsize information in the host struct for this socket before calling SADNuma() to create PMEM SADs.
      UpdateHostStructForPmem (host, socketBitMap);

      //Call SADNuma() to create SADs for the PMEM in this socket
      return SADNuma (host, MemMapData, socketNum, TOLM, memType);
    }
  }
  return SUCCESS;
}

UINT32
SocketNonInterleaveVolMem (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT8           socketNum,
  UINT16          TOLM
  )
/**

  Interleave volatile (DDR4/HBM) memory on IMC and channel level but not across sockets for NUMA configuration.

  @param host                - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param socketNum           - Socket number
  @param TOLM                - Top of low memory

  @retval status

**/
{
  UINT8   memType;
  UINT32  status = SUCCESS;

  if (host->var.mem.volMemMode == VOL_MEM_MODE_2LM) {
    memType = MEM_TYPE_2LM;
  } else {
    memType = MEM_TYPE_1LM;
  }

  // Skip if memType is 1LM and there is no DDR4 memory on this socket
  if ((memType == MEM_TYPE_1LM) && (host->var.mem.socket[socketNum].memSize == 0)) {
    return RETRY;
  }

  // Skip if memType is 2LM and there is no NVMDIMM volatile memory on this socket
  if ((memType == MEM_TYPE_2LM) && (host->var.mem.socket[socketNum].volSize == 0)) {
    return RETRY;
  }

#ifdef NVMEM_FEATURE_EN
  // Interleave Normal memory dimms first and then NvMem
  status = SocketNonInterleaveNormalMem (host, MemMapData, socketNum, TOLM, memType );
  if (status != SUCCESS) {
    return status;
  }

  status = SocketNonInterleaveNVMem (host, MemMapData, socketNum, TOLM, memType);
  if (status != SUCCESS) {
    return status;
  }
#else
  status = SADNuma (host, MemMapData, socketNum, TOLM, memType);
#endif

  return status;
}

/**

  Description: Interleaves memory on the channel level but not across sockets.
  This routine figures out the mapping and fills in the structures for the SAD, TAD,
  tables.  It does not program any registers.

  @param host      - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param TOLM      - Top of low memory

  @retval status

**/
UINT32
SocketNonInterleave (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT16          TOLM
  )
{
  UINT8   socketNum;
  struct  socketNvram  *socketNv;
  UINT8   ch;
  UINT32 status = SUCCESS;

  UpdateHostStructForVolMem(host, SOCKET_BITMAP_ALL);

  // Map the volatile and persistent memory
  OutputExtendedCheckpoint((host, STS_DDR_MEMMAP, SUB_SAD_NONINTER, 0));

  //
  //HSD-927607: BIOS Setup shall support socket interleaving for memory below 4GB
  //
  if (host->setup.mem.options & SPLIT_BELOW_4GB_EN) {
    status = SocketInterleave(host, MemMapData, TOLM);
    if(status != SUCCESS) return status;
  }

  for (socketNum = 0; socketNum < MAX_SOCKET; socketNum++) {

    // Skip if socket not enabled
    socketNv = &host->nvram.mem.socket[socketNum];
    if (socketNv->enabled == 0) continue;

    //Steps:
    //1. Loop for every socket present in the system
    //2. Find the volatile memory mode(1lm/2lm)
    //3. Create SAD rules for volatile memory in this socket
    //4. If PMEM is enabled, fill the Ch level host struct with PMEM related values
    //5. Create SAD rules for PMEM in this socket

    status = SocketNonInterleaveVolMem (host, MemMapData, socketNum, TOLM);
    if(status == RETRY) continue;
    if (status != SUCCESS) return status;

    status = SocketNonInterleavePerMem (host, MemMapData, socketNum, TOLM);
    if (status == RETRY) continue;
    if(status != SUCCESS) return status;

    // If this socket NUMA interleaved successfully then reload rem channel size in the structure
    for (ch = 0; ch <(host->var.mem.maxIMC* MAX_MC_CH); ch++)  {
#if NVMEM_FEATURE_EN
      host->var.mem.socket[socketNum].channelList[ch].remSize = host->var.mem.socket[socketNum].channelList[ch].memSize - host->var.mem.socket[socketNum].channelList[ch].NVmemSize;
#else
      host->var.mem.socket[socketNum].channelList[ch].remSize = host->var.mem.socket[socketNum].channelList[ch].memSize;
#endif
    }
  }

  //DisplaySADTable(host);
  return SUCCESS;
}

UINT8
RankSort(PSYSHOST host,
         struct rankSort rkSortList[],
         UINT8 sckt,
         UINT8 ch)
/**

  Computes a list of ranks sorted by size, largest to smallest.

  @param host        - Pointer to sysHost
  @param rkSortList  - Rank sort list
  @param sckt      - Socket Id
  @param ch          - Channel number (0-based)

  @retval sortNum - Number of ranks with memory remaining to be allocated
            @retval (sortNum may be 0 indicating all memory allocated)

**/
{
  UINT8 dimm;
  UINT8 rank;
  UINT8 si;
  UINT8 i;
  UINT8 sortNum;
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, sckt, ch);

  // Sort ranks by size, largest first
  sortNum = 0;
  for (rank = 0; rank < MAX_RANK_DIMM; rank++) {
    for (dimm = 0; dimm < MAX_DIMM; dimm++) {

      // Skip if this DIMM is not present
      if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

      // Skip if this DIMM is an NVM DIMM
      if ((*dimmNvList)[dimm].aepDimmPresent == 1) continue;

      rankList = GetRankNvList(host, sckt, ch, dimm);

      if (CheckRank(host, sckt, ch, dimm, rank, CHECK_MAPOUT)) continue;

      // Search for ranks with remaining size
      if ((*rankList)[rank].remSize == 0) continue;

      // Determine sort list position
      for (si = 0; si < sortNum; si++) {
        if (rkSortList[si].rankSize < (*rankList)[rank].remSize) break;
      }
      // Shift smaller ranks towards end of list
      if (si < sortNum) {
        for (i = sortNum; i > si; i--) rkSortList[i] = rkSortList[i - 1];
      }
      // Insert current rank
      rkSortList[si].socketNum = sckt;
      rkSortList[si].chNum = ch;
      rkSortList[si].dimmNum = dimm;
      rkSortList[si].rankNum = rank;
      rkSortList[si].rankID = (*rankList)[rank].phyRank;
      rkSortList[si].rankSize = (*rankList)[rank].remSize;
      sortNum++;
    } // DIMM loop
  } // Rank loop

  return sortNum;
}

UINT32 RankInterleave(PSYSHOST host)
/**

  Interleaves memory on the rank level for DDR and NVM memory.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  RankInterleaveDDR( host ) ;
  RankInterleaveDDRT( host ) ;

  return SUCCESS ;
}

UINT32 RankInterleaveDDR(PSYSHOST host)
/**

  Interleaves memory on the rank level. This routine figures out the
   mapping and fills in the structures for the RIR tables for DDR.
  It does not program any registers.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 rank;
  UINT8 si;
  UINT8 i;
  UINT8 j;
  UINT8 interNum;
  UINT8 sortNum;
#ifdef NVMEM_FEATURE_EN
  int NVPass;
  struct ddrRank (*rankListTemp)[MAX_RANK_DIMM];
  UINT8 rankTemp;
  UINT8 dimmTemp;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
#endif  //NVMEM_FEATURE_EN
  UINT8 dimmNum;
  UINT16 interSize;
  UINT16 rirLimit;
  UINT8 rirIndex;
  UINT16 rankMemUsed;
  UINT16 previousRIRLimit;
  struct socketNvram (*socketNvram)[MAX_SOCKET];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct rankSort rkSortList[MAX_RANK_CH];
  struct rankSort rkSortItem;
  struct ddrChannel *channelList;

  socketNvram = &host->nvram.mem.socket;
  // Loop for each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if ((*socketNvram)[sckt].enabled == 0) continue;

    // Loop for each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*socketNvram)[sckt].channelList[ch].enabled == 0) continue;

      channelList = &host->var.mem.socket[sckt].channelList[ch];

      if (channelList->memSize == 0) continue;

#ifdef NVMEM_FEATURE_EN 
      dimmNvList = GetDimmNvList(host, sckt, ch);
#endif

      // Get desired interleave width
      interNum = host->var.mem.rankInter;

      // Start the rirLimit off at 0 for each channel
      rirLimit = 0;
      previousRIRLimit = 0;
#ifdef NVMEM_FEATURE_EN
      NVPass = 0;
#endif  //NVMEM_FEATURE_EN

      // 5 Rank Level Interleaves can be defined
      for (rirIndex = 0; rirIndex < MAX_RIR; rirIndex++) {

        // Check for exit condition: all channel memory allocated in RIR
        if (rirLimit == channelList->memSize) {
          break;
        }

        // Sort ranks by size, largest first
        sortNum = RankSort (host, rkSortList, sckt, ch);
        // Determine largest N ranks, where N:1 is the rank interleave
        if (sortNum) {

          // Compare N to remaining ranks
          while (interNum > sortNum) interNum >>= 1;

          // Get size common to N ranks
          interSize = rkSortList[interNum - 1].rankSize;

          // Update the size common to N ranks
          if (interSize > rkSortList[interNum - 1].rankSize)
            interSize = rkSortList[interNum - 1].rankSize;

          // optimize sortList to eliminate adjacent
          if (interNum > 2) {
            for (i = 0; i < interNum - 1; i++) {
              // Check if these ranks are adjacent
              if (rkSortList[i].dimmNum == rkSortList[i+1].dimmNum) {

                // Point to a entry to trade with
                j = i + 2;

                // Break if we can't find an entry to trade with
                if (j >= interNum) break;

                // Find an entry that is different
                while (rkSortList[i].dimmNum == rkSortList[j].dimmNum) {
                  j++;
                  // Break if we can't find an entry to trade with
                  if (j >= interNum) break;
                }

                // Swap entries if this entry is valid
                if (j >= interNum) break;

                // Exchange sortList entries 1 and 2
                rkSortItem = rkSortList[i];
                rkSortList[i] = rkSortList[j];
                rkSortList[j] = rkSortItem;
              }
            }
          }

          // Increment RIR.Limit by interNum * interSize
          rirLimit += (interNum * interSize);

          // Save RIR Limit / ways
          channelList->rirLimit[rirIndex] = rirLimit;
          channelList->rirWay[rirIndex] = interNum;
          channelList->rirValid[rirIndex] = 1;

          // Initialize RIR Interleave
          for (i = 0; i < interNum; i++) {
            rank = rkSortList[i].rankNum;

            // Fill in rank ID
            channelList->rirIntList[rirIndex][i] = rkSortList[i].rankID;

            // Compute RIR offset = (channel_address_base / RIRways) - Rank_Address_base
            dimmNum = rkSortList[i].dimmNum;

            rankList = GetRankNvList(host, sckt, ch, dimmNum);
#ifdef NVMEM_FEATURE_EN
            if (NVPass == 0) {
              rankMemUsed = (*rankList)[rank].rankSize - ((*rankList)[rank].remSize + (*rankList)[rank].NVrankSize);
            }
            else
#endif // NVMEM_FEATURE_EN
            rankMemUsed = (*rankList)[rank].rankSize - (*rankList)[rank].remSize;
            channelList->rirOffset[rirIndex][i] = (previousRIRLimit / interNum) - rankMemUsed;
          }

          // Fill in the rest of the rir registers for this interleave
          // i index was carried over from previous loop
          for (; i < MAX_RIR_WAYS; i++) {
            channelList->rirIntList[rirIndex][i] = 0;
            channelList->rirOffset[rirIndex][i] = 0;
          }

          // Subtract common size from N ranks
          for (si = 0; si < interNum; si++) {
            (*socketNvram)[sckt].channelList[ch].dimmList[rkSortList[si].dimmNum].rankList[rkSortList[si].rankNum].remSize = (*socketNvram)[sckt].channelList[ch].dimmList[rkSortList[si].dimmNum].rankList[rkSortList[si].rankNum].remSize - interSize;
          }

          // Save current rirLimit to use while creating the next entry
          previousRIRLimit = rirLimit;

#ifdef NVMEM_FEATURE_EN
        }
        else {
          if (NVPass != 0) break;
          if (!channelList->NVmemSize) break;
          NVPass = 1;
          rirIndex--;
          channelList->remSize = channelList->NVmemSize;
#ifdef MEM_NVDIMM_EN
          if (host->setup.mem.interNVDIMMS == 0) interNum = 1;
#endif

          for (dimmTemp = 0; dimmTemp < MAX_DIMM; dimmTemp++) {
            if ((*dimmNvList)[dimmTemp].dimmPresent == 0) continue;

            rankListTemp = &host->nvram.mem.socket[sckt].channelList[ch].dimmList[dimmTemp].rankList;

            for (rankTemp = 0; rankTemp < MAX_RANK_DIMM; rankTemp++) {
              if ((*rankListTemp)[rankTemp].enabled == 0) continue;
              (*rankListTemp)[rankTemp].remSize = (*rankListTemp)[rankTemp].NVrankSize;
            } // rankTemp loop
          } // dimmTemp loop
        }
#else
      }
        else break;// if sortNum
#endif // NVMEM_FEATURE_EN
      } // rirIndex loop

      // Issue Fatal error if all RIR rules used before all memory allocated
      if (rirIndex > MAX_RIR) {
        MemDebugPrint((host, SDBG_MINMAX, sckt, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "RIR rules exceeds %d\n", MAX_RIR));
#if SMCPKG_SUPPORT
        OutputError (host, ERR_INTERLEAVE_FAILURE, ERR_RIR_RULES_EXCEEDED, sckt, ch, 0xFF, 0xFF);
#else
        FatalError (host, ERR_INTERLEAVE_FAILURE, ERR_RIR_RULES_EXCEEDED);
#endif
      }

      // Issue warning if all all the memory did not get mapped
      if (rirLimit < channelList->memSize) {
        MemDebugPrint((host, SDBG_MINMAX, sckt, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "RIR rules exceeds %d\n", MAX_RIR));

        channelList->memSize = rirLimit;

        OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_RIR_RULES_EXCEEDED, sckt, ch, 0xFF, 0xFF);
      }

      // Initialize unused RIR rules for the current channel

      // rirIndex set by the for loop above
      for (; rirIndex < MAX_RIR; rirIndex++) {
        channelList->rirLimit[rirIndex] = rirLimit;
        channelList->rirValid[rirIndex] = 0;
        channelList->rirWay[rirIndex] = 0;

        for (i = 0; i < MAX_RIR_WAYS; i++) {
          channelList->rirIntList[rirIndex][i] = 0;
          channelList->rirOffset[rirIndex][i] = 0;
        }
      }

    } // Ch loop
  } // socket loop

  return SUCCESS;
}

UINT32 RankInterleaveDDRT(PSYSHOST host)
/**

  Interleaves memory on the rank level. This routine figures out the
  mapping and fills in the structures for the RIR tables for DDRT.
  It does not program any registers.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 i;
  UINT16 rirLimit;
  UINT8 rirIndex;
  struct socketNvram (*socketNvram)[MAX_SOCKET];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct ddrChannel *channelList;
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  UINT8  aepPresent;
  UINT8  dimm;

  socketNvram = &host->nvram.mem.socket;

  // Loop for each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if ((*socketNvram)[sckt].enabled == 0) continue;

    // Loop for each channel
    for (ch = 0; ch < MAX_CH; ch++) {

      // Check if channel is enabled
      if ((*socketNvram)[sckt].channelList[ch].enabled == 0) continue;

      // Find NVM DIMM on this channel - only a single NVM DIMM per channel is supported
      aepPresent = 0 ;
      dimmNvList = GetDimmNvList(host, sckt, ch);
      for (dimm = 0; dimm < (*socketNvram)[sckt].channelList[ch].maxDimm; dimm++) {
        if((*dimmNvList)[dimm].dimmPresent == 0) continue;
        if((*dimmNvList)[dimm].aepDimmPresent)  {
          aepPresent = 1 ;
          break;
        }
      }

      // Skip this channel if NVM DIMM is not present
      if( aepPresent == 0 ) continue ;

      channelList = &host->var.mem.socket[sckt].channelList[ch];

      // For 1LM, the RIR limit needs to include the DDR4 memory in the channel.  For 2LM, it does not
      if (host->var.mem.volMemMode == VOL_MEM_MODE_2LM)  {
        rirLimit = (*dimmNvList)[dimm].volCap + (*dimmNvList)[dimm].nonVolCap + DDRT_VOL_DPA_START;
      } else {
        rirLimit = channelList->memSize + (*dimmNvList)[dimm].volCap + (*dimmNvList)[dimm].nonVolCap + DDRT_VOL_DPA_START;
      }

      // The rirLimit calculated above is in SAD_GRAN units, but it is eventually stored in RIR_GRAN units.  So, we need to round the rirLimit to the next RIR_GRAN boundary or it will be too small
      if (rirLimit % (RIR_GRAN / SAD_GRAN))  {
        rirLimit += (RIR_GRAN / SAD_GRAN) - (rirLimit % (RIR_GRAN / SAD_GRAN) );
      }
      rankList = GetRankNvList(host, sckt, ch, dimm);

      // Since NVMCTLR DIMMs have a single rank and since there is only one NVMCTLR DIMM per channel, there is only a single entry
      channelList->rirDDRTLimit[0] = rirLimit;
      channelList->rirDDRTWay[0] = 1;
      channelList->rirDDRTValid[0] = 1;
      channelList->rirDDRTIntList[0][0] = (*rankList)[0].phyRank;

      //For 1LM, update the DDRT offset needs to include the DDR4 memory  in the channel.  For 2LM, it does not
      if (host->var.mem.volMemMode == VOL_MEM_MODE_2LM)  {
        channelList->rirDDRTOffset[0][0] = 0 ;
      }
      else {
        channelList->rirDDRTOffset[0][0] = channelList->memSize;
      }

      // Fill in the rest of the rir registers for this interleave
      for( i = 1 ; i < MAX_RIR_DDRT_WAYS ; i++ ) {
        channelList->rirDDRTIntList[0][i] = 0;
        channelList->rirDDRTOffset[0][i] = 0;
      }

      // Initialize unused RIR rules for the current channel

      // rirIndex set by the for loop above
      for ( rirIndex = 1; rirIndex < MAX_RIR_DDRT ; rirIndex++) {
        channelList->rirDDRTLimit[rirIndex] = rirLimit;
        channelList->rirDDRTValid[rirIndex] = 0;
        channelList->rirDDRTWay[rirIndex] = 0;

        for (i = 0; i < MAX_RIR_DDRT_WAYS ; i++) {
          channelList->rirDDRTIntList[rirIndex][i] = 0;
          channelList->rirDDRTOffset[rirIndex][i] = 0;
        }
      }
    } // Ch loop
  } // socket loop

  return SUCCESS;
}



UINT8
InitBlkCtrlRegion(PSYSHOST host, UINT8 memType)
/**

  Init memory size variables based on the memtype

  @param host  - Pointer to sysHost
  @param memType  - - Type of memory region to be mapped

  @retval N/A

**/
{
  UINT8 sckt, ch, dimm, ddrtCount, regionSize;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrChannel (*channelList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct memVar *mem;


  mem = &host->var.mem;

  //Init the memory region size per dimm based on the mem type
  if(memType == MEM_TYPE_BLK_WINDOW){
    regionSize = DDRT_BLK_WINDOW_SIZE;
  }
  else {
    regionSize = DDRT_CTRL_SIZE;
  }

  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {

    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelList = &mem->socket[sckt].channelList;
    channelNvList = GetChannelNvList(host, sckt);

    // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      ddrtCount = 0;
      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        dimmNvList = GetDimmNvList(host, sckt, ch);
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, sckt, ch, dimm);
        //Skip if DIMM is disabled
        if ((*dimmNvList)[dimm].mapOut[0] || (*rankList)[0].enabled == 0)  continue;
        if(memType == MEM_TYPE_BLK_WINDOW){
          if(((*dimmNvList)[dimm].aepDimmPresent) && ((*dimmNvList)[dimm].nonVolCap > 0 ))
            ddrtCount++;
        } else {
          if((*dimmNvList)[dimm].aepDimmPresent)
            ddrtCount++;
        }
      }

      if(memType == MEM_TYPE_BLK_WINDOW)
        (*channelList)[ch].blkWinSize = ddrtCount * regionSize;
      else
        (*channelList)[ch].ctrlSize = ddrtCount * regionSize;
    }
  }
  return SUCCESS;
}

UINT32
PartitionDDRT(PSYSHOST host, UINT8 sckt, UINT8 ch, UINT8 dimm)
/**

  This routine checks if partitioning is requested form setup
  or BIOS knobs and partitions the dimm as per the requested ration.

  @param host     - Pointer to sysHost
  @param sckt   - socket number
  @param ch       - Channel number
  @param dimm     -  dimm number

  @retval status

  **/
{
  struct dimmNvram(*dimmNvList)[MAX_DIMM];
  UINT8  volRatio = 0, pmemRatio = 0, blkRatio = 0, mcIndex = 0;
  UINT32 volSize = 0, nonVolSize = 0;
  UINT64_STRUCT tempData;
  struct fnvCISBuffer fnvBuffer;
  UINT32 status = SUCCESS;
  UINT8 mbStatus = 0;
  struct dimmPartition(*dimmPartitionList)[MAX_SOCKET][MAX_CH][MAX_DIMM];

  dimmNvList = GetDimmNvList(host, sckt, ch);

  MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\tIn PartitionDDRT dimm\n"));

  //
  // Get the partition ratio for this dimm from the setup options
  //
  mcIndex = (sckt * host->var.mem.maxIMC) + (ch / MAX_MC_CH);

  MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\tmcIndex: %d\n", mcIndex));

  switch (host->var.mem.dfxMemVars.dfxPartitionRatio[mcIndex]){
  case RATIO_20_80_0:
    volRatio = 20;
    pmemRatio = 80;
    blkRatio = 0;
    break;
  case RATIO_80_20_0:
    volRatio = 80;
    pmemRatio = 20;
    blkRatio = 0;
    break;
  case RATIO_50_50_0:
    volRatio = 50;
    pmemRatio = 50;
    blkRatio = 0;
    break;
  case RATIO_60_0_40:
    volRatio = 60;
    pmemRatio = 0;
    blkRatio = 40;
    break;
  case RATIO_40_10_50:
    volRatio = 40;
    pmemRatio = 10;
    blkRatio = 50;
    break;
  case RATIO_0_40_60:
    volRatio = 0;
    pmemRatio = 40;
    blkRatio = 60;
    break;
  case RATIO_0_30_70:
    volRatio = 0;
    pmemRatio = 30;
    blkRatio = 70;
    break;
  case RATIO_100_0_0:
    volRatio = 100;
    pmemRatio = 0;
    blkRatio = 0;
    break;
  case RATIO_0_100_0:
    volRatio = 0;
    pmemRatio = 100;
    blkRatio = 0;
    break;
  case RATIO_0_0_100:
    volRatio = 0;
    pmemRatio = 0;
    blkRatio = 100;
    break;
  case RATIO_30_60_10:
    volRatio = 30;
    pmemRatio = 60;
    blkRatio = 10;
    break;
  case RATIO_70_0_30:
    volRatio = 70;
    pmemRatio = 0;
    blkRatio = 30;
    break;
  }

  MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\t option chosen: %d\n", host->var.mem.dfxMemVars.dfxPartitionRatio[mcIndex]));
  MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\t %d %d %d\n", volRatio, pmemRatio, blkRatio));
  volSize = ((((UINT32)(*dimmNvList)[dimm].rawCap) * volRatio) / 100) << CONVERT_64MB_TO_4KB_GRAN;
  nonVolSize = ((((UINT32)(*dimmNvList)[dimm].rawCap) * (pmemRatio + blkRatio)) / 100) << CONVERT_64MB_TO_4KB_GRAN;
  MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\t Requesting partitioning for volCap(4kb gran): %d nonVolCap(4kb gran):%d  \n", volSize, nonVolSize));

  status = SetDimmPartitionInfo(host, sckt, ch, dimm, volSize, nonVolSize, &fnvBuffer, &mbStatus);
  if (status == SUCCESS) {
    dimmPartitionList = GetFnvCisBufferDimmPartition(&fnvBuffer);
    (*dimmNvList)[dimm].volCap = (UINT16)((*dimmPartitionList)[sckt][ch][dimm].volatileCap >> CONVERT_4KB_TO_64MB_GRAN);
    (*dimmNvList)[dimm].nonVolCap = (UINT16)((*dimmPartitionList)[sckt][ch][dimm].persistentCap >> CONVERT_4KB_TO_64MB_GRAN);
    MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\t Partitioing returns the values: volCap(4kb): %d nonVolCap(4kb):%d \n", (*dimmPartitionList)[sckt][ch][dimm].volatileCap, (*dimmPartitionList)[sckt][ch][dimm].persistentCap));

    //HSD-169493: Division by zero error when using ratio 100% vol, 0% PMEM, 0% BLK to partition NVMDIMM dimm
    if ((*dimmNvList)[dimm].nonVolCap == 0) {
      (*dimmNvList)[dimm].perCap = 0;
      (*dimmNvList)[dimm].blkCap = 0;
    }
    else {
      //HSD-4928795: When partitionDDRTDimm is enabled persistent, blk memory sizes are incorrect
      (*dimmNvList)[dimm].perCap = (UINT16)((((UINT32)(*dimmNvList)[dimm].nonVolCap) * pmemRatio) / (pmemRatio + blkRatio));
      (*dimmNvList)[dimm].blkCap = (UINT16)((((UINT32)(*dimmNvList)[dimm].nonVolCap) * blkRatio) / (pmemRatio + blkRatio));
    }

    //Start addresses are in byte granularity
    tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].volatileStart, CONVERT_B_TO_64MB);
    (*dimmNvList)[dimm].volRegionDPA = tempData.lo;
    tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].persistentStart, CONVERT_B_TO_64MB);
    (*dimmNvList)[dimm].perRegionDPA = tempData.lo;
    MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "VolDPA: %d   PerDPA: %d \n", (*dimmNvList)[dimm].volRegionDPA, (*dimmNvList)[dimm].perRegionDPA));
  } else {
    //If the partition request failed, clear all the interleave requests from this dimm.
    HandlePartitionFailure(host, sckt, ch, dimm, mbStatus);
  }
  return status;
}

void
GetDimmInfoFromSPD(PSYSHOST host, UINT8 socket, UINT8 ch, UINT8 dimm, DIMMINFORMATION  dimmInformation )
/**

  This routine initializes the dimm NVRAM struct for the given dimm with data read from the SPD of the dimm.

  @param host  - Pointer to sysHost

  @retval void

**/
{
  struct dimmNvram (*dimmNvList)[MAX_DIMM];

  dimmNvList = GetDimmNvList(host, socket, ch);
  //Update the raw capacity from the SPD data obtained, store it in 4kb granularity
  dimmInformation->rc = rankSizeAEP[(*dimmNvList)[dimm].aepTechIndex] << CONVERT_64MB_TO_4KB_GRAN;

  MemDebugPrint ((host, SDBG_MAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"raw cap : %d(4kb)    %d (64mb)\n",dimmInformation->rc, dimmInformation->rc >> CONVERT_4KB_TO_64MB_GRAN));
  return;
}


void   AlignVolCaptoGB(PSYSHOST host)
/**

  This routine aligns the volCap of all the channels in every NVMCTLR dimm in the system to the nearest GB.
  This is to suffice a silicon requirement when SNC is enabled.

  @param host  - Pointer to sysHost

  @retval void

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  struct memNvram *nvramMem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  nvramMem = &host->nvram.mem;

 // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (nvramMem->socket[sckt].enabled == 0) continue;
  channelNvList = GetChannelNvList(host, sckt);

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        dimmNvList = GetDimmNvList(host, sckt, ch);
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        if((*dimmNvList)[dimm].volCap) {
          //Round off the volcap to the GB.
          (*dimmNvList)[dimm].volCap &= GB_ALIGN_NVM;
    }

        if(host->var.mem.dfxMemVars.dfxDimmManagement != DIMM_MGMT_CR_MGMT_DRIVER) {
      if((*dimmNvList)[dimm].perCap) {
            //Round off the perCap to the GB.
            (*dimmNvList)[dimm].perCap &= GB_ALIGN_NVM;
      }
    }
      }//dimm
    }//ch
  }//sckt
  return;
}



void UpdateDdrtFields(PSYSHOST host)
/**

  This routine initializes the memory size fields in the structures
  for DIMMs, Channels, and Nodes.  The sizes are calculated from the sizes in
  the rank structures.

  @param host  - Pointer to sysHost

  @retval void

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 dimm;
  struct memNvram *nvramMem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  UINT8 i=0;
  struct dimmInformation dimmInfoList ={0};
  UINT8  security, mbStatus = 0, secMbStatus = 0;
  UINT8 passphrase[PASSPHRASE_LENGTH];
  struct fnvCISBuffer fnvBuffer ;
  UINT64_STRUCT tempData;
  struct dimmPartition (*dimmPartitionList)[MAX_SOCKET][MAX_CH][MAX_DIMM];
  UINT32 status = SUCCESS, secStatus = SUCCESS;

  nvramMem = &host->nvram.mem;
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Gather NVMDIMM information:\n"));
  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (nvramMem->socket[sckt].enabled == 0) continue;
  channelNvList = GetChannelNvList(host, sckt);

   // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        dimmNvList = GetDimmNvList(host, sckt, ch);
        //Skip if this is not a NVMDIMM dimm
        if ((*dimmNvList)[dimm].aepDimmPresent == 0) continue;

        rankList = GetRankNvList(host, sckt, ch, dimm);
        //Skip if DIMM is disabled
        if ((*dimmNvList)[dimm].mapOut[0] || (*rankList)[0].enabled == 0)  continue;
#ifdef MEMMAPSIM_BUILD
        if (MMS_UseNVMDIMMFWInterface == TRUE)  {
#endif
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\nIssuing IdentifyDimm\n"));
          //Issue identify dimm to all the NVMCTLR dimms present to get the Manufacturer id and serial number
          status = IdentifyDimm(host, sckt, ch, dimm, &dimmInfoList, &mbStatus);
          if (status == SUCCESS) {
            //Update dimm identification information in the NVRAM struct
            (*dimmNvList)[dimm].firmwareRevision.majorVersion = dimmInfoList.fwr.majorVersion;
            (*dimmNvList)[dimm].firmwareRevision.minorVersion = dimmInfoList.fwr.minorVersion;
            (*dimmNvList)[dimm].firmwareRevision.hotfixVersion = dimmInfoList.fwr.hotfixVersion;
            (*dimmNvList)[dimm].firmwareRevision.buildVersion = dimmInfoList.fwr.buildVersion;

            for (i = 0; i < NGN_MAX_MANUFACTURER_STRLEN; i++) {
              (*dimmNvList)[dimm].manufacturer[i] = dimmInfoList.mf[i];
            }

            for (i = 0; i < NGN_MAX_SERIALNUMBER_STRLEN; i++) {
              (*dimmNvList)[dimm].serialNumber[i] = dimmInfoList.sn[i];
            }

            for (i = 0; i < NGN_MAX_MODELNUMBER_STRLEN; i++) {
              (*dimmNvList)[dimm].modelNumber[i] = dimmInfoList.mn[i];
            }

            (*dimmNvList)[dimm].InterfaceFormatCode = dimmInfoList.ifc;
//APTIOV_SERVER_OVERRIDE_RC_START
            //RSD type 197 Memory Ext Info          
            (*dimmNvList)[dimm].ApiVersion = dimmInfoList.api;
//APTIOV_SERVER_OVERRIDE_RC_END               
            (*dimmNvList)[dimm].dimmSku = dimmInfoList.dimmSku;
          }
          else {
            MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
            //log data format:
            //Byte 0 (Bit 0 – DIMM Slot Number, Bits 1-3 – Channel Number, Bits 4-6 – Socket Number,Bit 7 - Reserved)
            //Byte 1 – FW MB Opcode,Byte 2 – FW MB Sub-Opcode,Byte 3 – FW MB Status Code
            EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, sckt, ch, dimm, NO_RANK);
          }
          (*dimmNvList)[dimm].rawCap = (UINT16)(dimmInfoList.rc >> CONVERT_4KB_TO_64MB_GRAN);
          MemDebugPrint((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "\tRaw Capacity retrieved from Identifydimm : %d\n", (*dimmNvList)[dimm].rawCap));
#ifdef MEMMAPSIM_BUILD
        }
#endif
        //Update raw capacity of the dimm convert from 4kb granularity to 64MB granularity
        if( (*dimmNvList)[dimm].rawCap == 0 )  {
          (*dimmNvList)[dimm].rawCap = (UINT16)(dimmInfoList.rc >> CONVERT_4KB_TO_64MB_GRAN);
        }

#ifndef MEMMAPSIM_BUILD
        //Set the default NVMDIMM DIMM partitioning to be 100%VOL 0%PMEM 0%BLK
        (*dimmNvList)[dimm].volCap = (*dimmNvList)[dimm].rawCap;
        (*dimmNvList)[dimm].nonVolCap = 0 ;
        (*dimmNvList)[dimm].perCap = 0 ;
        (*dimmNvList)[dimm].blkCap = 0 ;
        (*dimmNvList)[dimm].volRegionDPA = DDRT_VOL_DPA_START ;
        (*dimmNvList)[dimm].perRegionDPA = 0 ;
#endif
        //DIMM partitioning code.
        if(host->var.mem.dfxMemVars.dfxPartitionDDRTDimm == PARTITION_DDRT_DIMM_EN ) {
          MemDebugPrint ((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"Partitioning NVMDIMM dimms based on setup option.\n"));

          PartitionDDRT(host, sckt, ch, dimm);

        } else {
#ifdef MEMMAPSIM_BUILD
          if (MMS_UseNVMDIMMFWInterface == TRUE)  {
#endif
            //If no partition request is found for this dimm, read the current partition information and update the host structs.
            status = GetDimmPartitionInfo(host, sckt, ch, dimm, &fnvBuffer, &mbStatus);
            if (status == SUCCESS) {
              dimmPartitionList = GetFnvCisBufferDimmPartition(&fnvBuffer);
              (*dimmNvList)[dimm].volCap = (UINT16)((*dimmPartitionList)[sckt][ch][dimm].volatileCap >> CONVERT_4KB_TO_64MB_GRAN);
              (*dimmNvList)[dimm].nonVolCap = (UINT16)((*dimmPartitionList)[sckt][ch][dimm].persistentCap >> CONVERT_4KB_TO_64MB_GRAN);
              (*dimmNvList)[dimm].perCap = (*dimmNvList)[dimm].nonVolCap / 2;
              (*dimmNvList)[dimm].blkCap = (*dimmNvList)[dimm].nonVolCap / 2;
              //Start addresses are in 64byte granularity
              tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].volatileStart, CONVERT_B_TO_64MB);
              (*dimmNvList)[dimm].volRegionDPA = tempData.lo;
              tempData = RShiftUINT64((*dimmPartitionList)[sckt][ch][dimm].persistentStart, CONVERT_B_TO_64MB);
              (*dimmNvList)[dimm].perRegionDPA = tempData.lo;
            }
            else {
              MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
              //log data format:
              //Byte 0 (Bit 0 – DIMM Slot Number, Bits 1-3 – Channel Number, Bits 4-6 – Socket Number,Bit 7 - Reserved)
              //Byte 1 – FW MB Opcode,Byte 2 – FW MB Sub-Opcode,Byte 3 – FW MB Status Code
              EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, sckt, ch, dimm, NO_RANK);
            }
#ifdef MEMMAPSIM_BUILD
          }
#endif
        }

        MemDebugPrint ((host, SDBG_MAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,"\tRaw: %d  Vol:%d  NonVol:%d  Per:%d  Blk:%d\n", (*dimmNvList)[dimm].rawCap, (*dimmNvList)[dimm].volCap, (*dimmNvList)[dimm].nonVolCap, (*dimmNvList)[dimm].perCap, (*dimmNvList)[dimm].blkCap));
#ifdef MEMMAPSIM_BUILD
        if (MMS_UseNVMDIMMFWInterface == TRUE)  {
#endif
          //
          // Set System Time
          //
          status = SetFnvSystemTime(host, sckt, ch, dimm, &mbStatus);
          if (status != SUCCESS) {
            MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
            //log data format:
            //Byte 0 (Bit 0 – DIMM Slot Number, Bits 1-3 – Channel Number, Bits 4-6 – Socket Number,Bit 7 - Reserved)
            //Byte 1 – FW MB Opcode,Byte 2 – FW MB Sub-Opcode,Byte 3 – FW MB Status Code
            EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, sckt, ch, dimm, NO_RANK);
          }
          //call SecureEraseUnit
          if (host->var.mem.setSecureEraseAllDIMMs == ENABLE_SECURE_ERASE || host->var.mem.setSecureEraseSktCh[sckt][ch] == ENABLE_SECURE_ERASE) {

            security = 0;

            status = GetSecurityState(host, sckt, ch, dimm, &security, &mbStatus);
            if (status == SUCCESS) {
              if ((security & SECURITY_ENABLED) && (security & SECURITY_LOCKED)) {
                for (i = 0; i < PASSPHRASE_LENGTH; i++) {
                  passphrase[i] = 0;
                }

                secStatus = SecureEraseUnit(host, sckt, ch, dimm, passphrase, &secMbStatus);
                if (secStatus != SUCCESS) {
                  MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
                  //log data format:
                  //Byte 0 (Bit 0 – DIMM Slot Number, Bits 1-3 – Channel Number, Bits 4-6 – Socket Number,Bit 7 - Reserved)
                  //Byte 1 – FW MB Opcode,Byte 2 – FW MB Sub-Opcode,Byte 3 – FW MB Status Code
                  EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, sckt, ch, dimm, NO_RANK);
                }
              }
            }
            else {
              MemDebugPrint((host, SDBG_MINMAX, sckt, ch, dimm, NO_RANK, NO_STROBE, NO_BIT, "Communication with the NGN DIMM failed\n"));
              //log data format:
              //Byte 0 (Bit 0 – DIMM Slot Number, Bits 1-3 – Channel Number, Bits 4-6 – Socket Number,Bit 7 - Reserved)
              //Byte 1 – FW MB Opcode,Byte 2 – FW MB Sub-Opcode,Byte 3 – FW MB Status Code
              EwlOutputType1 (host, WARN_DIMM_COMM_FAILED, WARN_MINOR_DIMM_COMM_FAILED_STATUS, sckt, ch, dimm, NO_RANK);
            }
          }
#ifdef MEMMAPSIM_BUILD
        }
#endif
      }//dimm
    }//ch
  } //socket

  //Init variables for blk window size
  InitBlkCtrlRegion(host, MEM_TYPE_BLK_WINDOW);

  //Init variables for controil region window size
  InitBlkCtrlRegion(host, MEM_TYPE_CTRL);
}


void
PopulateMemorySizeFields(PSYSHOST host)
/**

  This routine initializes the memory size fields in the structures
  for DIMMs, Channels, and Nodes.  The sizes are calculated from the sizes in
  the rank structures.

  @param host  - Pointer to sysHost

  @retval status

**/
{
  UINT8 sckt;
  UINT8 ch;
  UINT8 i;
  UINT8 dimm;
  UINT8 mc;
  struct memNvram *nvramMem;
  struct memVar *mem;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrChannel (*channelList)[MAX_CH];
  struct ddrRank (*rankList)[MAX_RANK_DIMM];
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  struct dimmDevice (*chdimmList)[MAX_DIMM];
  struct IMC *imc;
  struct imcNvram *imcNv;

  nvramMem = &host->nvram.mem;

  mem = &host->var.mem;

  mem->memSize = 0;
#ifdef NVMEM_FEATURE_EN
  mem->NVmemSize = 0;
#endif

  //Based on setup option dimmManagement, we decide how we populate size infomation for NGN dimms.
  if(host->var.mem.dfxMemVars.dfxDimmManagement == DIMM_MGMT_CR_MGMT_DRIVER) {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"CR mgmt Driver is available..\n"));

    //if(host->nvram.mem.Force1LMOnly != TRUE) {
    //Initialize NGN dimm by populating the necessary fields in host struct
    InitializeNGNDIMM(host);
    if(host->var.mem.MemMapState != MEM_MAP_STATE_RESOURCE_CALCULATION_DONE) {
      OemUpdatePlatformConfig(host);
      //Validate the CugCfg and CfgIn tables read from the PCD
      ValidateNGNDimmData(host);
      ValidateCurrentConfigOemHook(host);
    }
    //Apply Partitioning Request
    HandlePartitionRequests(host);
    //}
  } else {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"CR mgmt Driver is NOT available. Using setup options!!\n"));
    //Update NVMDIMM related fields in  DimmNVRAM struct
    UpdateDdrtFields (host);
  }

  //Adjust volap of each channel to align with nearest GB. Witohout this we will be broken when SNC is enabled.
  //HSD 5330197: SNC + 2LM currently broken
  AlignVolCaptoGB(host);

  if (host->setup.mem.dfxMemSetup.dfxMaxNodeInterleave != 0) {
    mem->socketInter = host->setup.mem.dfxMemSetup.dfxMaxNodeInterleave;
  }

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) { // Loop for each CPU socket
    if (nvramMem->socket[sckt].enabled == 0) continue;

    channelNvList = GetChannelNvList(host, sckt);

    mem->socket[sckt].memSize = 0;
    mem->socket[sckt].volSize = 0;
    mem->socket[sckt].perSize = 0;
    mem->socket[sckt].blkSize = 0;
#ifdef NVMEM_FEATURE_EN
    mem->socket[sckt].NVmemSize = 0;
#endif
    channelList = &mem->socket[sckt].channelList;

    // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;

      chdimmList = &mem->socket[sckt].channelList[ch].dimmList;

      (*channelList)[ch].memSize = 0;
      (*channelList)[ch].volSize = 0;
      (*channelList)[ch].perSize = 0;
      (*channelList)[ch].blkSize = 0;

      (*channelList)[ch].ddr4RemSize = 0;
      (*channelList)[ch].volRemSize = 0;
      (*channelList)[ch].perRemSize = 0;
      (*channelList)[ch].ctrlRemSize = 0;
      (*channelList)[ch].blkWinRemSize = 0;
#ifdef NVMEM_FEATURE_EN
      (*channelList)[ch].NVmemSize = 0;
#endif  //MEM_NVDIMM_EN

      // For each DIMM
      for (dimm = 0; dimm < (*channelNvList)[ch].maxDimm; dimm++) {
        dimmNvList = GetDimmNvList(host, sckt, ch);
        if ((*dimmNvList)[dimm].dimmPresent == 0) continue;

        rankList = GetRankNvList(host, sckt, ch, dimm);

        // DIMM size equals the sum of rank sizes in the DIMM
        // Note: Ranksize may be zero if it is the spare rank

        (*chdimmList)[dimm].memSize = 0;
        (*chdimmList)[dimm].volSize = 0;
#ifdef NVMEM_FEATURE_EN
        (*chdimmList)[dimm].NVmemSize = 0;
#endif  //MEM_NVDIMM_EN

        for (i = 0;  i < (*dimmNvList)[dimm].numDramRanks; i++) {
          // Check if this rank has been mapped out
          if ((*dimmNvList)[dimm].mapOut[i]) {
            /* SKX does not lock-step or mirror two channels literally like previous generations
             Hence for mirroring DIMM to DIMM match between mirror channels is NOT required
             For lock-step, two DDR channels are not in lock-step, LS in SKX is internal to each DDR channel.
             // Check if a RAS mode is enabled
             if (nvramMem->RASmode & (STAT_VIRT_LOCKSTEP + CH_MIRROR)) {
             if (ch == 0) {
             // Make sure the cooresponding DIMM on channel 1 is mapped out as well
             nvramMem->socket[sckt].channelList[1].dimmList[dimm].mapOut[i] = 1;
             } else if (ch == 2) {
             // Make sure the cooresponding DIMM on channel 3 is mapped out as well
             nvramMem->socket[sckt].channelList[3].dimmList[dimm].mapOut[i] = 1;
             }
             }*/
            // Continue to the next rank
            continue;
          }

          if ((*rankList)[i].enabled == 0) {

            continue; // Continue to the next rank
          }
          if (!(*dimmNvList)[dimm].aepDimmPresent)
#ifdef NVMEM_FEATURE_EN
          {
#endif
            (*chdimmList)[dimm].memSize = (*chdimmList)[dimm].memSize + (*rankList)[i].rankSize;
#ifdef NVMEM_FEATURE_EN
            MemDebugPrint((host, 0xFFFF, sckt, ch, dimm, i, NO_STROBE, NO_BIT,
              "ranksize = %x, NVranksize = %x\n", (*rankList)[i].rankSize, (*rankList)[i].NVrankSize));
            (*chdimmList)[dimm].NVmemSize = (*chdimmList)[dimm].NVmemSize + (*rankList)[i].NVrankSize;
          }
#endif
#ifdef NVMEM_FEATURE_EN
          // Reset remaining size for rank
          if ((*rankList)[i].NVrankSize != 0)
            (*rankList)[i].remSize = 0; // This is needed to in RankInterleave so that first normal dimm ranks are interleaved and then NVDIMMs.
          else
#endif
          // Reset remaining size for rank
          (*rankList)[i].remSize = (*rankList)[i].rankSize;

        }

        // Channel memory sizes equals the sum of the all DIMM sizes on this channel
        (*channelList)[ch].memSize = (*channelList)[ch].memSize + (*chdimmList)[dimm].memSize;
#ifdef NVMEM_FEATURE_EN
        (*channelList)[ch].NVmemSize = (*channelList)[ch].NVmemSize + (*chdimmList)[dimm].NVmemSize;
#endif
        if ((*dimmNvList)[dimm].aepDimmPresent) {
          if ((*dimmNvList)[dimm].mapOut[0] || (*rankList)[0].enabled == 0)  continue;
        }
        (*channelList)[ch].volSize = (*channelList)[ch].volSize + (*dimmNvList)[dimm].volCap;
        (*channelList)[ch].NonVolSize = (*channelList)[ch].NonVolSize + (*dimmNvList)[dimm].nonVolCap;
        (*channelList)[ch].perSize = (*channelList)[ch].perSize + (*dimmNvList)[dimm].perCap;
        (*channelList)[ch].blkSize = (*channelList)[ch].blkSize + (*dimmNvList)[dimm].blkCap;

      } // DIMM

      //Init remsize variables for all the modes.
#ifdef NVMEM_FEATURE_EN
//     (*channelList)[ch].ddr4RemSize = (*channelList)[ch].memSize - (*channelList)[ch].NVmemSize;
      (*channelList)[ch].ddr4RemSize = (*channelList)[ch].memSize;
     (*channelList)[ch].NVmemRemSize =(*channelList)[ch].NVmemSize;

#else
     (*channelList)[ch].ddr4RemSize =  (*channelList)[ch].memSize;
#endif
       (*channelList)[ch].volRemSize =  (*channelList)[ch].volSize;
       (*channelList)[ch].perRemSize =  (*channelList)[ch].perSize;
       (*channelList)[ch].ctrlRemSize =  (*channelList)[ch].ctrlSize;
       (*channelList)[ch].blkWinRemSize = (*channelList)[ch].blkWinSize;

      // The memory size for this socket equals the sum of all channel sizes on this socket
      mem->socket[sckt].memSize = mem->socket[sckt].memSize + (*channelList)[ch].memSize;
      mem->socket[sckt].volSize = mem->socket[sckt].volSize + (*channelList)[ch].volSize;
      mem->socket[sckt].perSize = mem->socket[sckt].perSize + (*channelList)[ch].perSize;
      mem->socket[sckt].blkSize = mem->socket[sckt].blkSize + (*channelList)[ch].blkSize;
#ifdef NVMEM_FEATURE_EN
      mem->socket[sckt].NVmemSize = mem->socket[sckt].NVmemSize + (*channelList)[ch].NVmemSize;
#endif

    } // Ch

    // Initialize mc structures
    for (mc = 0; mc <  host->var.mem.maxIMC; mc++) {
      imcNv = &nvramMem->socket[sckt].imc[mc];
      imc = &mem->socket[sckt].imc[mc];

      // Initialize mc nvram structure
      imcNv->enabled = 0;

      // Initialize mc mem structure
      imc->imcNum = mc;
      imc->memSize = 0;
      imc->imcChannelListStartIndex = mc * host->var.mem.numChPerMC;

      // Initialize imc memory size vairables
      for (ch = 0; ch < host->var.mem.numChPerMC; ch++) {
        imc->memSize = imc->memSize + host->var.mem.socket[sckt].channelList[ch + imc->imcChannelListStartIndex].memSize;
        imc->volSize = imc->volSize + host->var.mem.socket[sckt].channelList[ch + imc->imcChannelListStartIndex].volSize;
        imc->NonVolSize = imc->NonVolSize + host->var.mem.socket[sckt].channelList[ch + imc->imcChannelListStartIndex].NonVolSize;
        imc->perSize = imc->perSize + host->var.mem.socket[sckt].channelList[ch + imc->imcChannelListStartIndex].perSize;
        imc->blkSize = imc->blkSize + host->var.mem.socket[sckt].channelList[ch + imc->imcChannelListStartIndex].blkSize;
#ifdef NVMEM_FEATURE_EN
        imc->NVmemSize = imc->NVmemSize + host->var.mem.socket[sckt].channelList[ch + imc->imcChannelListStartIndex].NVmemSize;
#endif
      }// for channel per mc

      // Enable mc if memsize not zero
      if (imc->memSize != 0) {
         imcNv->enabled = 1;
      }// if Enable mc

    }// for mc

    // The total system memory size equals the summ of all socket sizes
    mem->memSize = mem->memSize + mem->socket[sckt].memSize;
#ifdef NVMEM_FEATURE_EN
  mem->NVmemSize = mem->NVmemSize + mem->socket[sckt].NVmemSize;
#endif

  } // Node

  //
  // Halt if there is no memory
  //
  if (mem->memSize == 0) {
    MemDebugPrint((host, 0xFFFF, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                   "No Memory to map!\n"));
    OutputError (host, ERR_NO_MEMORY, ERR_NO_MEMORY_MINOR_ALL_CH_DISABLED, 0xFF, 0xFF, 0xFF, 0xFF);
  }


 return;
} // PopulateMemorySizeFields

/**

  Description: Encode Limit field for all DRAM rules
  Limit is encoded such that valid address range <= Limit

  @param Limit - Limit to encode

  @retval Encoded Limit

**/
UINT32
SADTADEncodeLimit (
                  UINT32 Limit
                  )
{
  return(Limit - 1);
}

UINT8
SADEncodeTargetInterleaveGranularity(
                  PSYSHOST host,
                  UINT8 tgtGranularity
                  )
/**

Routine Description

  Description:  Encode target interleave granularity based on the target granularity

  @param host - Pointer to sysHost
  @param tgtGranularity - Target granularity

  @retval Encoded granularity
**/
{
  UINT8 encodedGranularity = 0 ;

  switch( tgtGranularity ) {
    case MEM_CTRL_TGT_INTERLEAVE_64B:
      encodedGranularity = 0 ;
      break;
    case MEM_CTRL_TGT_INTERLEAVE_256B:
      encodedGranularity = 1 ;
      break;
    case MEM_CTRL_TGT_INTERLEAVE_4KB:
      encodedGranularity = 2 ;
      break;
    case MEM_CTRL_TGT_INTERLEAVE_1GB:
      encodedGranularity = 3 ;
      break;
  }

  return( encodedGranularity ) ;
}

/**

Routine Description

  Description:  Encode interleave granularity based on the SAD entry type and granularity

  @param host - Pointer to sysHost

  type - SAD entry type
  granularity - SAD entry granularity

  @retval Encoded interleave

**/
EncodedInterleaveGranularity
SADEncodeInterleaveGranularity (
                  PSYSHOST host,
                  UINT8 type,
                  UINT8 granularity
                  )
{
  EncodedInterleaveGranularity encodedInterleaveGranularity = {0};

  switch( type )  {
    case MEM_TYPE_1LM:

      switch( granularity ) {
        case MEM_INT_GRAN_1LM_256B_256B:
          encodedInterleaveGranularity.chaTgt = 1 ;
          encodedInterleaveGranularity.chaCh = 1 ;
          break ;
        case MEM_INT_GRAN_1LM_64B_64B:
          encodedInterleaveGranularity.chaTgt = 0 ;
          encodedInterleaveGranularity.chaCh = 0 ;
          break ;
      }
      break ;

    case MEM_TYPE_2LM:
    case MEM_TYPE_PMEM_CACHE:
      switch( granularity ) {
        case MEM_INT_GRAN_2LM_NM_4KB_256B_FM_4KB:
          encodedInterleaveGranularity.chaTgt = 2 ;
          encodedInterleaveGranularity.chaCh = 1 ;
          encodedInterleaveGranularity.fmCh = 2 ;
          break ;
        case MEM_INT_GRAN_2LM_NM_4KB_64B_FM_256B:
          encodedInterleaveGranularity.chaTgt = 2 ;
          encodedInterleaveGranularity.chaCh = 0 ;
          encodedInterleaveGranularity.fmCh = 1 ;
          break ;
        case MEM_INT_GRAN_2LM_NM_4KB_64B_FM_4KB:
          encodedInterleaveGranularity.chaTgt = 2 ;
          encodedInterleaveGranularity.chaCh = 0 ;
          encodedInterleaveGranularity.fmCh = 2 ;
          break ;
      }
      break;

    case MEM_TYPE_PMEM:
      switch( granularity ) {
        case MEM_INT_GRAN_PMEM_NUMA_4KB_4KB:
          encodedInterleaveGranularity.chaTgt = 2 ;
          encodedInterleaveGranularity.chaCh = 2 ;
          break ;
        case MEM_INT_GRAN_PMEM_NUMA_4KB_256B:
          encodedInterleaveGranularity.chaTgt = 2 ;
          encodedInterleaveGranularity.chaCh = 1 ;
          break ;
      }
      break;

    case MEM_TYPE_BLK_WINDOW:
    case MEM_TYPE_CTRL:
      encodedInterleaveGranularity.chaTgt = 2 ;
      encodedInterleaveGranularity.chaCh = 1 ;
      break;
  }

  return( encodedInterleaveGranularity ) ;

}

/**

  Description: Encode Mod3AsMod2 field for all DRAM rules

  @param ChannelInterleaveBitMap - Channel interleave bitmap to encode

  @retval Encoded Mod3AsMod2

**/
UINT32
SADEncodeMod3AsMod2 (
                  UINT8 ChannelInterleaveBitmap
                  )
{
  UINT32 mod3AsMod2 ;

  switch( ChannelInterleaveBitmap ) {

    case BITMAP_CH0_CH1_CH2:
      mod3AsMod2 = 0x0 ;
      break ;

    case BITMAP_CH0_CH1:
      mod3AsMod2 = 0x1 ;
      break ;

    case BITMAP_CH0_CH2:
      mod3AsMod2 = 0x3 ;
      break ;

    case BITMAP_CH1_CH2:
      mod3AsMod2 = 0x2 ;
      break ;

    default:
      mod3AsMod2 = 0x0 ;
  }
  return( mod3AsMod2 ) ;
}

UINT32 
SADEncodeInterleaveList ( 
  SYSHOST *Host, 
  UINT8   Sckt, 
  UINT8   SadIndex, 
  UINT8   MemType 
  )
/**

  Description: Encode interleave list for SAD rule

  @param Host  - Pointer to sysHost
  @param Sckt  - Socket Id
  @param SadIndex - Index of SAD entry to encode
  @param MemType - memory type

  @retval Encoded InterleaveList

**/
{
  SAD_TABLE                     *SadEntry;
  UINT32                        InterleaveList = 0;
  UINT32                        InterleaveEntry;
  UINT8                         j;
  UINT8                         Mc;
  UINT8                         Ch;
  UINT8                         Channel[MAX_MC_CH] = { 0 };
  UINT8                         PkgCount;
  UINT8                         ChannelWays;
  UINT8                         SadIntListEntry;
  EncodedInterleaveGranularity  EncodedGranularity;
  UINT8                         ChannelInterleaveBitMap;
  UINT8                         ChannelEntry;
  UINT8                         RemoteCount;
  UINT8                         LocalCount;

  SadEntry = &Host->var.mem.socket[Sckt].SAD[SadIndex];

  if (SadEntry->Attr == SAD_NXM_ATTR) {
     InterleaveList = 0xFFFFFFFF;
     return InterleaveList;
  }

  // Get encoded interleave granularities based on the SADEntry->Type
  EncodedGranularity = SADEncodeInterleaveGranularity(Host, SadEntry->type, SadEntry->granularity);

  ChannelWays = 0;

  // Determine which IMC channel bitmap to use
  Mc = GetMcIndexFromBitmap(SadEntry->imcInterBitmap);
  
  // Compute channelWays for this SAD entry
  if (MemType == NMEM) {
    ChannelInterleaveBitMap = SadEntry->channelInterBitmap[Mc];
  } else {
    ChannelInterleaveBitMap = SadEntry->FMchannelInterBitmap[Mc];
  }

  for (Ch = 0; Ch < MAX_MC_CH; Ch++) {
    //Find the channel that is populated
    if(ChannelInterleaveBitMap & (BIT0 << Ch)) {
      ChannelWays++;
      Channel[ChannelWays - 1] = Ch;
    }
  }

  if ((SadEntry->type == MEM_TYPE_1LM ) || (SadEntry->type == MEM_TYPE_PMEM )) {
    if ((SadEntry->ways > 1) && (ChannelWays == 2) && (EncodedGranularity.chaCh == EncodedGranularity.chaTgt)) {
      // Check if enough target entries in the interleave list for this SAD entry
      if ((SadEntry->ways * 2) > MAX_SAD_WAYS) {
        MemDebugPrint((Host, SDBG_MINMAX, Sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
          "Not enough target entries to encode SAD rule\n"));
        OutputWarning(Host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, Sckt, 0xFF, 0xFF, 0xFF);
#if SMCPKG_SUPPORT
        OutputError (Host, ERR_NGN, INTERLEAVE_EXCEED, Sckt, NO_CH, NO_DIMM, NO_RANK);
#else
        FatalError(Host, ERR_NGN, INTERLEAVE_EXCEED);
#endif
      }
    }
  } else if ((SadEntry->type == MEM_TYPE_2LM) || (SadEntry->type == MEM_TYPE_PMEM_CACHE)) {
    if ((SadEntry->ways > 1) && (ChannelWays == 2) && (EncodedGranularity.fmCh == EncodedGranularity.chaTgt)) {
      // Check if enough target entries in the interleave list for this SAD entry
      if ((SadEntry->ways * 2) > MAX_SAD_WAYS) {
        MemDebugPrint((Host, SDBG_MINMAX, Sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, 
          "Not enough target entries to encode SAD rule\n"));
        OutputWarning(Host, WARN_INTERLEAVE_FAILURE, WARN_INTERLEAVE_EXCEEDED, Sckt, 0xFF, 0xFF, 0xFF);
#if SMCPKG_SUPPORT
        OutputError (Host, ERR_NGN, INTERLEAVE_EXCEED, Sckt, NO_CH, NO_DIMM, NO_RANK);
#else
        FatalError(Host, ERR_NGN, INTERLEAVE_EXCEED);
#endif
      }
    }
  }

  //Clear the bit fields
  PkgCount = 0;
  ChannelEntry = 0;
  RemoteCount = 0;
  LocalCount = 0;
  do {
    for (j = 0; j < MC_MAX_NODE; j++) {
      SadIntListEntry = Host->var.mem.socket[Sckt].SADintList[SadIndex][j];

      //Skip for Imc that are not part of this interleave
      if (SadIntListEntry == 0) {
        continue;
      }

      // Interleave entry
      InterleaveEntry = 0;

      //Determine if this mc is a remote target or local
      if (j / Host->var.mem.maxIMC != Sckt) {
        //BIT3 denotes remote or local target
        InterleaveEntry |= (j / Host->var.mem.maxIMC);   //BIT0, 1 & 2 denotes socket number
        RemoteCount++;
      } else {  //local
        //BIT3 denotes remote or local target
        InterleaveEntry |=  BIT3;

        //Fill the imc info in BIT0
        InterleaveEntry |= (j % Host->var.mem.maxIMC);

        //if mod3 disabled, fill the channel info in BIT1 & 2
        if (ChannelWays == 1) {
          InterleaveEntry |= Channel[ChannelWays - 1] << 1;
        }
        
        LocalCount++;
      }//local

      // Add interleaveEntry to interleaveList
      //
      //Bit0-3 - MC0
      //Bit4-7 - MC1
      //..
      //Bit28-31 - MC7
      InterleaveList |= (InterleaveEntry << (4 * PkgCount));

      if ((SadEntry->ways > 1) && (ChannelWays == 2)) { 
        if ((((SadEntry->type == MEM_TYPE_2LM) || (SadEntry->type == MEM_TYPE_PMEM_CACHE)) && 
              (EncodedGranularity.fmCh == EncodedGranularity.chaTgt)) ||
            (((SadEntry->type == MEM_TYPE_1LM) || (SadEntry->type == MEM_TYPE_PMEM)) && 
              (EncodedGranularity.chaCh == EncodedGranularity.chaTgt))) {
          
          // Refer HSD 4930405 : If the channelWays is 2 and the channel interleave granularity equals the socket interleave granularity, then the
          // interleave entry programming needs to be done differently.  This is becuase of the algorithm to map logical to physical
          // channel ids.
          if (((SadEntry->type == MEM_TYPE_1LM) || (SadEntry->type == MEM_TYPE_PMEM)) && 
              (Host->var.common.cpuType == CPU_SKX) && (Host->var.common.stepping < B0_REV_SKX) && 
              ((Host->setup.kti.KtiPrefetchEn == KTI_ENABLE ) || (Host->setup.kti.XptPrefetchEn == KTI_ENABLE))) {

            PkgCount++;
            InterleaveList |= (InterleaveEntry << (4 * PkgCount));
          } else {
            //WA for Slilcon issue: HSD 4929385
            //If the FM ch ways is 2 way and the FM channel granularity is equal socket granularity
            //1. program mod3 =0 in mcFMDramRule
            //2. Program the FM interleave list so that channel numbers are represented in Bit 1 & 2 in local packages.
            //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
            //
            //HSD 5331714 - 1LM if the NM ch ways is 2 way and the NM channel granularity is equal socket granularity then
            //1. program mod3 =0 in DRAM rule
            //2. Program the interleave list so that channel numbers are represented in Bit 1 & 2 in local packages.
            //
            // For NUMA:
            //  Package 0, 1, 4, and 5 should be encoded with Channel[0]
            //  Package 2, 3, 6, and 7 should be encoded with Channel[1]
            //
            // For UMA, the packages cycle between sockets, so only encode the channels on the local packages
            //  For 1 MC per socket:
            //   For Socket 0:
            //    Package 0 and 4 should be encoded with Channel[0]
            //    Package 2 and 6 should be encoded with Channel[1]
            //    Package 1, 3, 5, and 7 are remote
            //   For Socket 1:
            //    Package 1 and 5 should be encoded with Channel[0]
            //    Package 3 and 7 should be encoded with Channel[1]
            //    Package 0, 2, 3, and 8 are remote
            //  For 2 MCs per socket:
            //   For Socket 0:
            //    Package 0 and 1 should be encoded with Channel[0]
            //    Package 4 and 5 should be encoded with Channel[1]
            //    Package 2, 3, 6, and 7 are remote
            //   For Socket 1:
            //    Package 2 and 3 should be encoded with Channel[0]
            //    Package 6 and 7 should be encoded with Channel[1]
            //    Package 0, 1, 4, and 5 are remote
            //  For 4 sockets:
            //   For Socket 0:
            //    Package 0 should be encoded with Channel[0]
            //    Package 4 should be encoded with Channel[1]
            //    Package 1, 2, 3, 5, 6, and 7 are remote
            //   For Socket 1:
            //    Package 1 should be encoded with Channel[0]
            //    Package 5 should be encoded with Channel[1]
            //    Package 0, 2, 3, 4, 6, and 7 are remote
            //   For Socket 2:
            //    Package 2 should be encoded with Channel[0]
            //    Package 6 should be encoded with Channel[1]
            //    Package 0, 1, 3, 4, 5, and 7 are remote
            //   For Socket 3:
            //    Package 3 should be encoded with Channel[0]
            //    Package 7 should be encoded with Channel[1]
            //    Package 0, 1, 2, 4, 5, and 6 are remote
            if (InterleaveEntry & BIT3) {
              // Local entry
              
              // For NUMA switch Channel field used before each even package
              if ((RemoteCount == 0) && (PkgCount % 2 == 0) && (PkgCount != 0)) {
                ChannelEntry ^= 0x01;
              }
              
              // Encode with channel number
              InterleaveList |= Channel[ChannelEntry] << ((4 * PkgCount) + 1);
            } 

            // For UMA switch Channel field used when Remote Count and Local Count match
            // For sockets 2 and 3, switch when Remote Count reaches 5
            if ((RemoteCount == LocalCount) || (RemoteCount == 5)) {
              ChannelEntry ^= 0x01;
            }
          }
        }
      }
      
      PkgCount++;
    }//SAD_Ways
  } while (PkgCount < MAX_SAD_WAYS);

  return InterleaveList;
}
/**

  Description: Fill in SAD2TAD CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteMesh2MemCSRs (
             PSYSHOST      host,
             UINT8         sckt
             )
{
  UINT8                                 i, tad, mc, memMode;
  struct memVar                         *mem;
  struct SADTable                       *SADEntry;
  struct channelNvram                  (*channelNvList)[MAX_CH];
  SAD2TAD_M2MEM_MAIN_STRUCT             meshSad2Tad;
  MODE_M2MEM_MAIN_STRUCT                meshCRMode;
  SYSFEATURES0_M2MEM_MAIN_STRUCT        sysFeatures0;
  SYSFEATURES0_MC_2LM_STRUCT            sysFeatures0_2lmCtrl;
  UINT8                                 ch;

  channelNvList = GetChannelNvList(host, sckt);
  meshCRMode.Data = 0;
  sysFeatures0.Data = 0;

  mem = &host->var.mem;
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tWrite MESH2MEM CSRs for Socket: %d\n", sckt));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));

  // Loop through each memory controller
  for(mc=0; mc< host->var.mem.maxIMC; mc++) {

    memMode = CheckMemModes( host, sckt, mc );

    meshCRMode.Data = MemReadPciCfgMC (host, sckt, mc, MODE_M2MEM_MAIN_REG);

    //
    // 1. Program MODE_M2MEM_MAIN_REG
    //
    if(memMode & MEM_TYPE_2LM ) {
      meshCRMode.Bits.nmcaching = 1;
    }

    if(memMode & MEM_TYPE_PMEM ) {
      meshCRMode.Bits.pmem = 1;
    }

    if(memMode & MEM_TYPE_PMEM_CACHE )  {
      meshCRMode.Bits.pmemcaching = 1;
    }

    if( ( memMode & MEM_TYPE_BLK_WINDOW ) || ( memMode & MEM_TYPE_CTRL ) )  {
      meshCRMode.Bits.blockregion = 1;
      //4930392: Cloned From SKX Si Bug Eco: m2m seems to expect Pmem bit in mode register set for 1lm+block mode (Workaround)
      meshCRMode.Bits.pmem = 1;
    }

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nMODE_M2MEM_MAIN_REG\n" ));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tnmcaching:%x  pmem:%x  pmemcaching:%x  blockregion:%x\n", meshCRMode.Bits.nmcaching, meshCRMode.Bits.pmem, meshCRMode.Bits.pmemcaching, meshCRMode.Bits.blockregion ));

    MemWritePciCfgMC (host, sckt, mc, MODE_M2MEM_MAIN_REG, meshCRMode.Data);

    //
    // 2. Program SYSFEATURES0_M2MEM_MAIN_REG
    //
    if (memMode & (MEM_TYPE_2LM | MEM_TYPE_PMEM | MEM_TYPE_PMEM_CACHE | MEM_TYPE_BLK_WINDOW | MEM_TYPE_CTRL)) {
      sysFeatures0.Data = MemReadPciCfgMC(host, sckt, mc, SYSFEATURES0_M2MEM_MAIN_REG);

      sysFeatures0.Bits.prefdisable = 1;

      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nSYSFEATURES0_M2MEM_MAIN_REG\n" ));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tprefdisable:%x\n", sysFeatures0.Bits.prefdisable ));

      MemWritePciCfgMC (host, sckt, mc, SYSFEATURES0_M2MEM_MAIN_REG, sysFeatures0.Data);
    }
  } // mc

  for (i = 0; i < MAX_SAD_RULES; i++) {
    SADEntry = &mem->socket[sckt].SAD[i];

    //Break if we have reached the end of the SAD table
    if(SADEntry->Enable == 0) break;

    // Skip if this is a remote entry or an NXM SAD rule
    if( SADEntry->local == 0 || SADEntry->Attr == SAD_NXM_ATTR) {
      continue;
    }

    for(mc=0; mc< host->var.mem.maxIMC; mc++){

      // Skip if this mc is not a part of the interleave
      if(((SADEntry->imcInterBitmap) & (BIT0<<mc)) != 0){
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"MC: %d\n", mc ));

        // Clear both read Enable and write Enable bits
        meshSad2Tad.Data = 0;
        meshSad2Tad.Bits.sad2tadwren = 0;
        meshSad2Tad.Bits.sad2tadrden = 0;
        MemWritePciCfgMC (host, sckt, mc, SAD2TAD_M2MEM_MAIN_REG, meshSad2Tad.Data);

        // Set relevant bits based on SAD and TAD tables
        meshSad2Tad.Bits.sadid = i;
        meshSad2Tad.Bits.sadvld = 1;
        meshSad2Tad.Bits.pmemvld = 0;

        for (tad=0; tad<TAD_RULES; tad++) {
          if(host->var.mem.socket[sckt].imc[mc].TAD[tad].SADId == i) break;
        }

        // 1LM and 2LM SAD entries have associated DDR4 TAD entries
        if( ( SADEntry->type == MEM_TYPE_1LM ) || ( SADEntry->type == MEM_TYPE_2LM ) || ( SADEntry->type == MEM_TYPE_PMEM_CACHE ) ) {
          meshSad2Tad.Bits.ddr4tadid = tad;
        }

        // 2LM and CTRL SAD entries have associated NVMDIMM TAD entries
        if( ( SADEntry->type == MEM_TYPE_2LM ) || ( SADEntry->type == MEM_TYPE_PMEM ) || ( SADEntry->type == MEM_TYPE_PMEM_CACHE ) || ( SADEntry->type == MEM_TYPE_BLK_WINDOW ) || ( SADEntry->type == MEM_TYPE_CTRL )) {
          meshSad2Tad.Bits.ddrttadid = tad;
        }

        // PMEM & PMEM$ SAD entries have pmemvld set
        if( ( SADEntry->type == MEM_TYPE_PMEM ) || ( SADEntry->type == MEM_TYPE_PMEM_CACHE )) {
          meshSad2Tad.Bits.pmemvld = 1;
        }

        // CTRL SAD entries have blkvld set
        if( (SADEntry->type == MEM_TYPE_CTRL) || ( SADEntry->type == MEM_TYPE_BLK_WINDOW ) )  {
          meshSad2Tad.Bits.blkvld = 1 ;
        } else  {
          meshSad2Tad.Bits.blkvld = 0 ;
        }

        ConfigMesh2MemCsrForMirrorRASHook (host, SADEntry, &meshSad2Tad); // in MemRAS.c

        //Set the write Enable bit
        meshSad2Tad.Bits.sad2tadwren = 1;

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tsadid:%x sadvld:%x  pmemvld:%x blkvld:%x  ddr4tadid:%x ddrttadid:%x mirrorddr4:%x\n", meshSad2Tad.Bits.sadid, meshSad2Tad.Bits.sadvld, meshSad2Tad.Bits.pmemvld, meshSad2Tad.Bits.blkvld, meshSad2Tad.Bits.ddr4tadid, meshSad2Tad.Bits.ddrttadid, meshSad2Tad.Bits.mirrorddr4  ));

        MemWritePciCfgMC (host, sckt, mc, SAD2TAD_M2MEM_MAIN_REG, meshSad2Tad.Data);

        // clear the write Enable bit
        meshSad2Tad.Bits.sad2tadwren = 0;
        MemWritePciCfgMC (host, sckt, mc, SAD2TAD_M2MEM_MAIN_REG, meshSad2Tad.Data);
      } // (SADEntry->imcInterBitmap) & (1<<mc)
      //
      // Program SYSFEATURES0_M2MEM_MAIN_REG based on mirrorddr4
      //
      meshSad2Tad.Data = MemReadPciCfgMC (host, sckt, mc, SAD2TAD_M2MEM_MAIN_REG);
      if (meshSad2Tad.Bits.mirrorddr4 == 1) {
        sysFeatures0.Data = MemReadPciCfgMC(host, sckt, mc, SYSFEATURES0_M2MEM_MAIN_REG);

        sysFeatures0.Bits.prefdisable = 1;

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nSYSFEATURES0_M2MEM_MAIN_REG\n" ));
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tmirrorddr4 is enabled; prefdisable:%x\n", sysFeatures0.Bits.prefdisable ));

        MemWritePciCfgMC (host, sckt, mc, SYSFEATURES0_M2MEM_MAIN_REG, sysFeatures0.Data);
      }
    }//mc
  } //sad rules

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //If we are in 2lm mode & in single socket config; set FrcDirI to 1.
    if ( (host->var.kti.SysConfig == SYS_CONFIG_1S) && (host->var.mem.volMemMode == VOL_MEM_MODE_2LM)) {
      //Read the 2lm ctrl register for each channel
      sysFeatures0_2lmCtrl.Data = MemReadPciCfgEp(host, sckt, ch, SYSFEATURES0_MC_2LM_REG);
      sysFeatures0_2lmCtrl.Bits.frcdiri = 1;
      MemWritePciCfgEp (host, sckt, ch, SYSFEATURES0_MC_2LM_REG, sysFeatures0_2lmCtrl.Data);
    }
  } // ch loop

  return ;
}
/**

  Description: Fill in SAD CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteSADCSRs (
             PSYSHOST      host,
             UINT8         sckt
             )
{

  UINT8                                 i, mc, channelWays=0;
  struct memVar                         *mem;
  struct SADTable                       *SADEntry;
  DRAM_RULE_CFG_0_CHABC_SAD_STRUCT      cboDramRule;
  NM_DRAM_RULE_CFG0_MC_MAIN_STRUCT      mcNMDramRule;
  DRAM_RULE_CFG0_MC_MAIN_STRUCT         mcFMDramRule;
  UINT8 maxSADRules = MAX_SAD_RULES ;
  UINT32 nmcboInterleaveList ;
  UINT32 fmcboInterleaveList ;
  EncodedInterleaveGranularity          encodedInterleaveGranularity;
  UINT8 ChannelInterleaveBitMap;
  UINT32 DRAMRuleCfgCHA[MAX_SAD_RULES] =
    { DRAM_RULE_CFG_0_CHABC_SAD_REG, DRAM_RULE_CFG_1_CHABC_SAD_REG, DRAM_RULE_CFG_2_CHABC_SAD_REG, DRAM_RULE_CFG_3_CHABC_SAD_REG,
      DRAM_RULE_CFG_4_CHABC_SAD_REG, DRAM_RULE_CFG_5_CHABC_SAD_REG, DRAM_RULE_CFG_6_CHABC_SAD_REG, DRAM_RULE_CFG_7_CHABC_SAD_REG,
      DRAM_RULE_CFG_8_CHABC_SAD_REG, DRAM_RULE_CFG_9_CHABC_SAD_REG, DRAM_RULE_CFG_10_CHABC_SAD_REG, DRAM_RULE_CFG_11_CHABC_SAD_REG,
      DRAM_RULE_CFG_12_CHABC_SAD_REG, DRAM_RULE_CFG_13_CHABC_SAD_REG, DRAM_RULE_CFG_14_CHABC_SAD_REG, DRAM_RULE_CFG_15_CHABC_SAD_REG,
      DRAM_RULE_CFG_16_CHABC_SAD_REG, DRAM_RULE_CFG_17_CHABC_SAD_REG, DRAM_RULE_CFG_18_CHABC_SAD_REG, DRAM_RULE_CFG_19_CHABC_SAD_REG,
      DRAM_RULE_CFG_20_CHABC_SAD_REG, DRAM_RULE_CFG_21_CHABC_SAD_REG, DRAM_RULE_CFG_22_CHABC_SAD_REG, DRAM_RULE_CFG_23_CHABC_SAD_REG } ;

  UINT32 InterleaveListCfgCHA[MAX_SAD_RULES] =
    { INTERLEAVE_LIST_CFG_0_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_1_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_2_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_3_CHABC_SAD_REG,
      INTERLEAVE_LIST_CFG_4_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_5_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_6_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_7_CHABC_SAD_REG,
      INTERLEAVE_LIST_CFG_8_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_9_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_10_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_11_CHABC_SAD_REG,
      INTERLEAVE_LIST_CFG_12_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_13_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_14_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_15_CHABC_SAD_REG,
      INTERLEAVE_LIST_CFG_16_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_17_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_18_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_19_CHABC_SAD_REG,
      INTERLEAVE_LIST_CFG_20_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_21_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_22_CHABC_SAD_REG, INTERLEAVE_LIST_CFG_23_CHABC_SAD_REG } ;

  UINT32 NMDRAMRuleCfgMC[MAX_SAD_RULES] =
    { NM_DRAM_RULE_CFG0_MC_MAIN_REG, NM_DRAM_RULE_CFG1_MC_MAIN_REG, NM_DRAM_RULE_CFG2_MC_MAIN_REG, NM_DRAM_RULE_CFG3_MC_MAIN_REG,
      NM_DRAM_RULE_CFG4_MC_MAIN_REG, NM_DRAM_RULE_CFG5_MC_MAIN_REG, NM_DRAM_RULE_CFG6_MC_MAIN_REG, NM_DRAM_RULE_CFG7_MC_MAIN_REG,
      NM_DRAM_RULE_CFG8_MC_MAIN_REG, NM_DRAM_RULE_CFG9_MC_MAIN_REG, NM_DRAM_RULE_CFG10_MC_MAIN_REG, NM_DRAM_RULE_CFG11_MC_MAIN_REG,
      NM_DRAM_RULE_CFG12_MC_MAIN_REG, NM_DRAM_RULE_CFG13_MC_MAIN_REG, NM_DRAM_RULE_CFG14_MC_MAIN_REG, NM_DRAM_RULE_CFG15_MC_MAIN_REG,
      NM_DRAM_RULE_CFG16_MC_MAIN_REG, NM_DRAM_RULE_CFG17_MC_MAIN_REG, NM_DRAM_RULE_CFG18_MC_MAIN_REG, NM_DRAM_RULE_CFG19_MC_MAIN_REG,
      NM_DRAM_RULE_CFG20_MC_MAIN_REG, NM_DRAM_RULE_CFG21_MC_MAIN_REG, NM_DRAM_RULE_CFG22_MC_MAIN_REG, NM_DRAM_RULE_CFG23_MC_MAIN_REG } ;

  UINT32 NMInterleaveListCfgMC[MAX_SAD_RULES] =
    { NM_INTERLEAVE_LIST_CFG0_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG1_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG2_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG3_MC_MAIN_REG,
      NM_INTERLEAVE_LIST_CFG4_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG5_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG6_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG7_MC_MAIN_REG,
      NM_INTERLEAVE_LIST_CFG8_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG9_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG10_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG11_MC_MAIN_REG,
      NM_INTERLEAVE_LIST_CFG12_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG13_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG14_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG15_MC_MAIN_REG,
      NM_INTERLEAVE_LIST_CFG16_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG17_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG18_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG19_MC_MAIN_REG,
      NM_INTERLEAVE_LIST_CFG20_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG21_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG22_MC_MAIN_REG, NM_INTERLEAVE_LIST_CFG23_MC_MAIN_REG } ;

  UINT32 DRAMRuleCfgMC[MAX_SAD_RULES] =
    { DRAM_RULE_CFG0_MC_MAIN_REG, DRAM_RULE_CFG1_MC_MAIN_REG, DRAM_RULE_CFG2_MC_MAIN_REG, DRAM_RULE_CFG3_MC_MAIN_REG,
      DRAM_RULE_CFG4_MC_MAIN_REG, DRAM_RULE_CFG5_MC_MAIN_REG, DRAM_RULE_CFG6_MC_MAIN_REG, DRAM_RULE_CFG7_MC_MAIN_REG,
      DRAM_RULE_CFG8_MC_MAIN_REG, DRAM_RULE_CFG9_MC_MAIN_REG, DRAM_RULE_CFG10_MC_MAIN_REG, DRAM_RULE_CFG11_MC_MAIN_REG,
      DRAM_RULE_CFG12_MC_MAIN_REG, DRAM_RULE_CFG13_MC_MAIN_REG, DRAM_RULE_CFG14_MC_MAIN_REG, DRAM_RULE_CFG15_MC_MAIN_REG,
      DRAM_RULE_CFG16_MC_MAIN_REG, DRAM_RULE_CFG17_MC_MAIN_REG, DRAM_RULE_CFG18_MC_MAIN_REG, DRAM_RULE_CFG19_MC_MAIN_REG,
      DRAM_RULE_CFG20_MC_MAIN_REG, DRAM_RULE_CFG21_MC_MAIN_REG, DRAM_RULE_CFG22_MC_MAIN_REG, DRAM_RULE_CFG23_MC_MAIN_REG } ;

  UINT32 InterleaveListCfgMC[MAX_SAD_RULES] =
    { INTERLEAVE_LIST_CFG0_MC_MAIN_REG, INTERLEAVE_LIST_CFG1_MC_MAIN_REG, INTERLEAVE_LIST_CFG2_MC_MAIN_REG, INTERLEAVE_LIST_CFG3_MC_MAIN_REG,
      INTERLEAVE_LIST_CFG4_MC_MAIN_REG, INTERLEAVE_LIST_CFG5_MC_MAIN_REG, INTERLEAVE_LIST_CFG6_MC_MAIN_REG, INTERLEAVE_LIST_CFG7_MC_MAIN_REG,
      INTERLEAVE_LIST_CFG8_MC_MAIN_REG, INTERLEAVE_LIST_CFG9_MC_MAIN_REG, INTERLEAVE_LIST_CFG10_MC_MAIN_REG, INTERLEAVE_LIST_CFG11_MC_MAIN_REG,
      INTERLEAVE_LIST_CFG12_MC_MAIN_REG, INTERLEAVE_LIST_CFG13_MC_MAIN_REG, INTERLEAVE_LIST_CFG14_MC_MAIN_REG, INTERLEAVE_LIST_CFG15_MC_MAIN_REG,
      INTERLEAVE_LIST_CFG16_MC_MAIN_REG, INTERLEAVE_LIST_CFG17_MC_MAIN_REG, INTERLEAVE_LIST_CFG18_MC_MAIN_REG, INTERLEAVE_LIST_CFG19_MC_MAIN_REG,
      INTERLEAVE_LIST_CFG20_MC_MAIN_REG, INTERLEAVE_LIST_CFG21_MC_MAIN_REG, INTERLEAVE_LIST_CFG22_MC_MAIN_REG, INTERLEAVE_LIST_CFG23_MC_MAIN_REG } ;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tWrite SAD CSRs for Socket: %d\n", sckt));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  mem = &host->var.mem;


  for (i = 0; i < maxSADRules; i++) {
    SADEntry = &mem->socket[sckt].SAD[i];

    // Clear the data fields first
    cboDramRule.Data                  = 0;
    mcNMDramRule.Data                 = 0;
    mcFMDramRule.Data                 = 0;

    //Update the fields based on the SAD table
    cboDramRule.Bits.rule_enable      = SADEntry->Enable;
    cboDramRule.Bits.attr             = SADEntry->Attr;
    cboDramRule.Bits.limit            = SADTADEncodeLimit (SADEntry->Limit);

    mcNMDramRule.Bits.rule_enable      = SADEntry->Enable;
    mcNMDramRule.Bits.limit            = SADTADEncodeLimit (SADEntry->Limit);

    // Get encoded interleave granularities based on the SADEntry->Type
    encodedInterleaveGranularity = SADEncodeInterleaveGranularity( host, SADEntry->type, SADEntry->granularity ) ;

    // Determine which IMC channel bitmap to use
    mc = GetMcIndexFromBitmap (SADEntry->imcInterBitmap);

    // Compute channelWays for this SAD entry
    ChannelInterleaveBitMap = SADEntry->channelInterBitmap[mc] | SADEntry->FMchannelInterBitmap[mc];
    channelWays = 0;
    channelWays = CalculateWaysfromBitmap (ChannelInterleaveBitMap);

    // If the SAD Entry is a local entry, then encode the target and channel interleave information
    if( SADEntry->local )  {
      cboDramRule.Bits.interleave_mode  = encodedInterleaveGranularity.chaTgt;
      mcNMDramRule.Bits.interleave_mode  = encodedInterleaveGranularity.chaTgt;

      if (((SADEntry->type == MEM_TYPE_1LM) || (SADEntry->type == MEM_TYPE_PMEM )) && (SADEntry->ways > 1) && (channelWays == 2)
        && (encodedInterleaveGranularity.chaCh == encodedInterleaveGranularity.chaTgt)) {
        // HSD 5331714 - To correctly support sparing, do not use the mod3_asamod2 in the case below.  Instead the interleaved channels will be described in the encoded interleave list
        //Make sure the prefetch knobs are disabled for SKX A0, else donot apply this WA
        if((host->var.common.cpuType == CPU_SKX) && (host->var.common.stepping < B0_REV_SKX) 
          && ((host->setup.kti.KtiPrefetchEn == KTI_ENABLE ) || (host->setup.kti.XptPrefetchEn == KTI_ENABLE)))
        {
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
               "A stepping prefetch enabled WA: CH  gets higher interleave precedence over MC. Ch numbers not encoded in the interleave list.\n"));
          cboDramRule.Bits.mod3           = 1;
          cboDramRule.Bits.mod3_mode      = encodedInterleaveGranularity.chaCh;
          cboDramRule.Bits.mod3_asamod2   = SADEncodeMod3AsMod2( SADEntry->FMchannelInterBitmap[mc] );

          if (SADEntry->type == MEM_TYPE_1LM) {
            mcNMDramRule.Bits.mod3          = 1;
            mcNMDramRule.Bits.mod3_mode     = encodedInterleaveGranularity.chaCh;
            mcNMDramRule.Bits.mod3_asamod2  = SADEncodeMod3AsMod2( SADEntry->channelInterBitmap[mc] );
          }
        } else {
          // When an even number of channels are populated at the MC and the MC and CH interleave granularity are the same, Mod3 = 0
          MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
               "A stepping (prefetch disabled)/  B stepping - MC gets higher interleave precedence. CH numbers encoded in the interleave list\n"));
          cboDramRule.Bits.mod3 = 0;
          cboDramRule.Bits.mod3_mode = 0;
          cboDramRule.Bits.mod3_asamod2 = 0;

          if (SADEntry->type == MEM_TYPE_1LM) {
            mcNMDramRule.Bits.mod3 = 0;
            mcNMDramRule.Bits.mod3_mode = 0;
            mcNMDramRule.Bits.mod3_asamod2 = 0;
          }
        }
      } else if((SADEntry->type == MEM_TYPE_1LM) || (SADEntry->type == MEM_TYPE_2LM)|| (SADEntry->type == MEM_TYPE_PMEM_CACHE)) {
        // One channel in this SAD entry
        if (IsPowerOfTwo(SADEntry->channelInterBitmap[mc])) {
          cboDramRule.Bits.mod3           = 0;
          cboDramRule.Bits.mod3_mode      = 0;
          cboDramRule.Bits.mod3_asamod2   = 0;

          mcNMDramRule.Bits.mod3          = 0;
          mcNMDramRule.Bits.mod3_mode     = 0;
          mcNMDramRule.Bits.mod3_asamod2  = 0;
        } else {
          // More than one channel in this SAD entry
          cboDramRule.Bits.mod3           = 1;
          cboDramRule.Bits.mod3_mode      = encodedInterleaveGranularity.chaCh;
          cboDramRule.Bits.mod3_asamod2   = SADEncodeMod3AsMod2( SADEntry->channelInterBitmap[mc] );

          mcNMDramRule.Bits.mod3          = 1;
          mcNMDramRule.Bits.mod3_mode     = encodedInterleaveGranularity.chaCh;
          mcNMDramRule.Bits.mod3_asamod2  = SADEncodeMod3AsMod2( SADEntry->channelInterBitmap[mc] );
        }
      } else if ((SADEntry->type == MEM_TYPE_CTRL) || (SADEntry->type == MEM_TYPE_PMEM) || (SADEntry->type == MEM_TYPE_BLK_WINDOW)) {

        // One channel in this SAD entry
        if (IsPowerOfTwo(SADEntry->FMchannelInterBitmap[mc])) {
          cboDramRule.Bits.mod3           = 0;
          cboDramRule.Bits.mod3_mode      = 0;
          cboDramRule.Bits.mod3_asamod2   = 0;
        } else {
          // More than one channel in this SAD entry
          cboDramRule.Bits.mod3           = 1;
          cboDramRule.Bits.mod3_mode      = encodedInterleaveGranularity.chaCh;
          cboDramRule.Bits.mod3_asamod2   = SADEncodeMod3AsMod2( SADEntry->FMchannelInterBitmap[mc] );
        }
      }
    } else {
      // If the SAD Entry is a remote entry, then encode the target interleave information
      cboDramRule.Bits.interleave_mode = SADEncodeTargetInterleaveGranularity( host, SADEntry->tgtGranularity );
    }

    //Programmings only for 2LM, PMEM & PMEM$ mode
    if ((SADEntry->type == MEM_TYPE_2LM) || (SADEntry->type == MEM_TYPE_PMEM) || (SADEntry->type == MEM_TYPE_PMEM_CACHE)) {

      //Program DRAM_RULE_CFG0_MC_MAIN_REG that needs to be filled only for 2LM mode.
      mcFMDramRule.Bits.rule_enable      = SADEntry->Enable;
      mcFMDramRule.Bits.interleave_mode  = encodedInterleaveGranularity.chaTgt;
      mcFMDramRule.Bits.limit            = SADTADEncodeLimit (SADEntry->Limit);

      if( SADEntry->local )  {
        //Update the Target interleave granularity for 2LM.
        cboDramRule.Bits.interleave_mode  = encodedInterleaveGranularity.chaTgt;

        if ((SADEntry->type == MEM_TYPE_PMEM ) && (SADEntry->ways > 1) && (channelWays == 2) && (encodedInterleaveGranularity.chaCh == encodedInterleaveGranularity.chaTgt)) {
          // When an even number of channels are populated at the MC and the MC and CH interleave granularity are the same, Mod3 = 0
          mcFMDramRule.Bits.mod3            =   0;
          mcFMDramRule.Bits.mod3_mode       =   0;
          mcFMDramRule.Bits.mod3_asamod2    =   0;
        }
        else if ( IsPowerOfTwo( SADEntry->FMchannelInterBitmap[mc] ) ) {
          mcFMDramRule.Bits.mod3            =   0;
          mcFMDramRule.Bits.mod3_mode       =   0;
          mcFMDramRule.Bits.mod3_asamod2    =   0;
        }
        // More than one channel in this SAD entry
        else {
          mcFMDramRule.Bits.mod3            = 1;
          if((SADEntry->type == MEM_TYPE_2LM ) || ( SADEntry->type == MEM_TYPE_PMEM_CACHE ))
            mcFMDramRule.Bits.mod3_mode       = encodedInterleaveGranularity.fmCh;
          else
            mcFMDramRule.Bits.mod3_mode       = encodedInterleaveGranularity.chaCh;
          mcFMDramRule.Bits.mod3_asamod2    = SADEncodeMod3AsMod2( SADEntry->FMchannelInterBitmap[mc] );
        }
        //WA for Slilcon issue: HSD 4929385
        //If the FM ch ways is 2 way and the FM channel interleave is equal socket granularity
        //1. program mod3 =0 in mcFMDramRule
        //2. Program the FM interleave list so that channel numbers are represented in Bit 1 & 2 in local packages.
        //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
        if (( SADEntry->type == MEM_TYPE_2LM ) || ( SADEntry->type == MEM_TYPE_PMEM_CACHE )) {
          if( (SADEntry->ways >1)  && (channelWays == 2) && (encodedInterleaveGranularity.fmCh == encodedInterleaveGranularity.chaTgt) ) {
            mcFMDramRule.Bits.mod3            = 0;
            mcFMDramRule.Bits.mod3_asamod2    = 0;
          }
        }
      }
    }

    // Display enabled SAD rules in the serial log
    if( SADEntry->Enable ) {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Program CSRs for SAD Rule %d ", i));
      if (SADEntry->Attr != SAD_NXM_ATTR) {
          if( SADEntry->local ) {
            switch( SADEntry->type ) {
              case MEM_TYPE_1LM:
                MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(1LM)"));
                break;
              case MEM_TYPE_2LM:
                MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(2LM)"));
                break;
              case MEM_TYPE_PMEM:
                MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(PMEM)"));
                break;
              case MEM_TYPE_PMEM_CACHE:
                MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(PMEM_CACHE)"));
                break;
              case MEM_TYPE_BLK_WINDOW:
                MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(BLK_WINDOW)"));
                break;
              case MEM_TYPE_CTRL:
                MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(NVMDIMM_CTRL)"));
                break;
            }
          }
          else {
            MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(Remote)"));
          }
      }
      else {
        MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"(NXM)"));
      }

      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));

      //Print the data to serial debug log
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"          En   Base  Limit  IntMode  Attr    Md3  Md3AsMd2  Md3Mode\n"));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"CHA     :  %x  %05x  %05x     %x       %x      %x      %x         %x\n",cboDramRule.Bits.rule_enable,(i==0 || cboDramRule.Bits.limit == 0) ? 0 : mem->socket[sckt].SAD[i-1].Limit, cboDramRule.Bits.limit, cboDramRule.Bits.interleave_mode, cboDramRule.Bits.attr, cboDramRule.Bits.mod3, cboDramRule.Bits.mod3_asamod2, cboDramRule.Bits.mod3_mode ));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"MC NM   :  %x  %05x  %05x     %x       -      %x      %x         %x\n",mcNMDramRule.Bits.rule_enable,(i==0 || mcNMDramRule.Bits.limit == 0) ? 0 : mem->socket[sckt].SAD[i-1].Limit, mcNMDramRule.Bits.limit, mcNMDramRule.Bits.interleave_mode, mcNMDramRule.Bits.mod3, mcNMDramRule.Bits.mod3_asamod2, mcNMDramRule.Bits.mod3_mode ));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"MC FM   :  %x  %05x  %05x     %x       -      %x      %x         %x\n",mcFMDramRule.Bits.rule_enable,(i==0 || mcFMDramRule.Bits.limit == 0 ) ? 0 : mem->socket[sckt].SAD[i-1].Limit, mcFMDramRule.Bits.limit, mcFMDramRule.Bits.interleave_mode, mcFMDramRule.Bits.mod3, mcFMDramRule.Bits.mod3_asamod2, mcFMDramRule.Bits.mod3_mode ));
    }

    //Update the CHA CSR
    WriteCpuPciCfgEx (host, sckt, 0, DRAMRuleCfgCHA[i], cboDramRule.Data);

    //Update the MC CSRs
    for( mc=0; mc< host->var.mem.maxIMC; mc++ ) {
      MemWritePciCfgMC (host, sckt, mc, NMDRAMRuleCfgMC[i], mcNMDramRule.Data);

      if( ( host->var.mem.volMemMode == VOL_MEM_MODE_2LM )  || ( host->var.mem.dfxMemVars.dfxPerMemMode ) ) {
         MemWritePciCfgMC (host, sckt, mc, DRAMRuleCfgMC[i], mcFMDramRule.Data);
      }
    }

    //Break if we have reached the end of the SAD table
    if(SADEntry->Enable == 0) continue;

    // Compute SAD interleave list
    nmcboInterleaveList = SADEncodeInterleaveList( host, sckt, i, NMEM);
    fmcboInterleaveList = SADEncodeInterleaveList( host, sckt, i, FMEM);
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nNM INTERLEAVE LIST : %08x\n", nmcboInterleaveList));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nFM INTERLEAVE LIST : %08x\n", fmcboInterleaveList));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"_______________________________________________________________________\n"));

    if( (SADEntry->type == MEM_TYPE_1LM) || (SADEntry->type == MEM_TYPE_2LM) || (SADEntry->type == MEM_TYPE_PMEM_CACHE)){
      // Update the CHA CSR
      WriteCpuPciCfgEx (host, sckt, 0, InterleaveListCfgCHA[i], nmcboInterleaveList);
    } else {
      WriteCpuPciCfgEx (host, sckt, 0, InterleaveListCfgCHA[i], fmcboInterleaveList);
    }
    // Update the MC CSRs
    for( mc=0; mc< host->var.mem.maxIMC; mc++ ) {
      if( (SADEntry->type == MEM_TYPE_1LM) || (SADEntry->type == MEM_TYPE_2LM) || (SADEntry->type == MEM_TYPE_PMEM_CACHE)){
        MemWritePciCfgMC (host, sckt, mc, NMInterleaveListCfgMC[i], nmcboInterleaveList);
      } else {
        MemWritePciCfgMC (host, sckt, mc, NMInterleaveListCfgMC[i], fmcboInterleaveList);
      }

      if( SADEntry->type != MEM_TYPE_1LM )  {
        MemWritePciCfgMC (host, sckt, mc, InterleaveListCfgMC[i], fmcboInterleaveList);
      }
    } // mc loop
  } // i loop for maxSADRules
  return;
}

/**

  Description: Fill in route table CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteRouteTableCSRs (
             PSYSHOST      host,
             UINT8         sckt
             )
{
  MC_ROUTE_TABLE_CHA_PMA_STRUCT         chaRouteTable;
  MC_NM_ROUTE_TABLE_MC_MAIN_STRUCT      nmRouteTable;
  MC_ROUTE_TABLE_MC_MAIN_STRUCT         fmRouteTable;
  UINT8  cha, mc, totCha ;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tWrite Route Table CSRs for Socket: %d\n", sckt));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));

  //MC_ROUTE_TABLE_CHA
  chaRouteTable.Data = 0;
  chaRouteTable.Bits.ringid0 = 0;
  chaRouteTable.Bits.ringid1 = 1;
  chaRouteTable.Bits.ringid2 = 0;
  chaRouteTable.Bits.ringid3 = 1;
  chaRouteTable.Bits.ringid4 = 0;
  chaRouteTable.Bits.ringid5 = 1;
  chaRouteTable.Bits.channelid0 = 0;
  chaRouteTable.Bits.channelid1 = 0;
  chaRouteTable.Bits.channelid2 = 1;
  chaRouteTable.Bits.channelid3 = 1;
  chaRouteTable.Bits.channelid4 = 2;
  chaRouteTable.Bits.channelid5 = 2;
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"CSR : MC_ROUTE_TABLE_CHA  (per socket)\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tRingid  0  1  2  3  4  5\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t        %x  %x  %x  %x  %x  %x\n",chaRouteTable.Bits.ringid0, chaRouteTable.Bits.ringid1, chaRouteTable.Bits.ringid2, chaRouteTable.Bits.ringid3, chaRouteTable.Bits.ringid4, chaRouteTable.Bits.ringid5  ));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tChnlid  0  1  2  3  4  5\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t        %x  %x  %x  %x  %x  %x\n\n", chaRouteTable.Bits.channelid0, chaRouteTable.Bits.channelid1, chaRouteTable.Bits.channelid2, chaRouteTable.Bits.channelid3, chaRouteTable.Bits.channelid4, chaRouteTable.Bits.channelid5));

  nmRouteTable.Data = 0;
  nmRouteTable.Bits.channelid0 = 0;
  nmRouteTable.Bits.channelid1 = 0;
  nmRouteTable.Bits.channelid2 = 1;
  nmRouteTable.Bits.channelid3 = 1;
  nmRouteTable.Bits.channelid4 = 2;
  nmRouteTable.Bits.channelid5 = 2;
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tCSR : MC_NM_ROUTE_TABLE  (per socket per IMC)\n "));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tChnlid  0  1  2  3  4  5\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t        %x  %x  %x  %x  %x  %x\n\n", nmRouteTable.Bits.channelid0, nmRouteTable.Bits.channelid1, nmRouteTable.Bits.channelid2, nmRouteTable.Bits.channelid3, nmRouteTable.Bits.channelid4, nmRouteTable.Bits.channelid5));

  fmRouteTable.Data = 0;
  if(IsDDRTPresent(host, BIT0<<sckt)) {
    fmRouteTable.Bits.channelid0 = 0;
    fmRouteTable.Bits.channelid1 = 0;
    fmRouteTable.Bits.channelid2 = 1;
    fmRouteTable.Bits.channelid3 = 1;
    fmRouteTable.Bits.channelid4 = 2;
    fmRouteTable.Bits.channelid5 = 2;
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tCSR : MC_ROUTE_TABLE_MC - FM (per socket per IMC only in 2 LM mode)\n "));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tChnlid  0  1  2  3  4  5\n"));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t        %x  %x  %x  %x  %x  %x\n\n", fmRouteTable.Bits.channelid0, fmRouteTable.Bits.channelid1, fmRouteTable.Bits.channelid2, fmRouteTable.Bits.channelid3, fmRouteTable.Bits.channelid4, fmRouteTable.Bits.channelid5));
  }

  totCha = host->var.kti.CpuInfo[sckt].TotCha ;

  for (cha = 0; cha < totCha ; cha++) {
    WriteCpuPciCfgEx (host, sckt, cha, MC_ROUTE_TABLE_CHA_PMA_REG, chaRouteTable.Data);
  }
  for (mc = 0; mc < host->var.mem.maxIMC; mc++) {
    MemWritePciCfgMC (host, sckt, mc, MC_NM_ROUTE_TABLE_MC_MAIN_REG, nmRouteTable.Data);
  }

  if(IsDDRTPresent(host, BIT0<<sckt)) {
    for (mc = 0; mc < host->var.mem.maxIMC; mc++)
      MemWritePciCfgMC (host, sckt, mc, MC_ROUTE_TABLE_MC_MAIN_REG, fmRouteTable.Data);
  }
  return;
}

/**

  Description: Fill in CHA CRSs for memory map from config info in host
  structure.

  @param host  - Pointer to sysHost
  @param socket  - Socket Id

  @retval N/A

**/
void
WriteCHACSRs (
             PSYSHOST      host,
             UINT8         sckt
             )
{
  TWO_LM_CONFIG_CHABC_SAD_STRUCT cboTwoLMConfig;
  HA_COH_CHA_MISC_STRUCT haCoh;
  HA_COH_CFG_TOR_CHA_PMA_STRUCT haCohCfgTor;
  MCNMCACHINGINTLV_MC_2LM_STRUCT  mcNMCachingIntlv;
  struct ddrChannel *channelList;
  UINT8 ch = 0, mc = 0, bitsToShift = 0, foundSad = 0, cha = 0, totCha = 0, minScktIntlv = 0;
  UINT8 minChIntlv = 0, nmChCount = 0, memMode = 0, TargetIntlv = 0, ChannelIntlv = 0;
  UINT8  disSpecRdValue = 0;
  UINT16 minNMSize, minNMSizeCh, minNMSizeMc, minNMSizeSkt ;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tWrite CHA CSRs for Socket: %d\n", sckt));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));

  // Is there a DRAM rule on this socket that represents 2LM or PMEM$ memory?
  for( mc = 0, foundSad = FALSE ; mc < host->var.mem.maxIMC; mc++ )  {
    memMode = CheckMemModes( host, sckt, mc );
    if( ( memMode & MEM_TYPE_2LM ) || ( memMode & MEM_TYPE_PMEM_CACHE ) ) {
      foundSad = TRUE;
      break;
    }
  }

    totCha = host->var.kti.CpuInfo[sckt].TotCha;

  // 2LM DRAM Rule on this Socket
  if( foundSad )  {

    //
    //  1. Program TWO_LM_CONFIG register for this socket
    //
    cboTwoLMConfig.Data = 0;

    // Normal 2LM Configuration Mask
    if( host->var.mem.dfxMemVars.dfxCfgMask2LM == CFG_MASK_2LM_NORMAL ) {

      minNMSizeCh = 0 ;

      channelList = &host->var.mem.socket[sckt].channelList[0];

      for(mc = 0 ; mc < host->var.mem.maxIMC ; mc++ )  {

        memMode = CheckMemModes( host, sckt, mc );

        // No 2LM or PMEM$ on this memory controller
        if( !( ( memMode & MEM_TYPE_2LM ) || ( memMode & MEM_TYPE_PMEM_CACHE ) ) ) continue ;

        for(ch = 0 ; ch < host->var.mem.numChPerMC ; ch++) {

          // No near memory on this channel
          if(channelList[(mc*MAX_MC_CH)+ch].memSize == 0) continue ;

          // Find minimal NM population on this socket's memory channels
          if(minNMSizeCh == 0)  {
            minNMSizeCh = channelList[(mc*MAX_MC_CH)+ch].memSize;
          }
          else if(channelList[(mc*MAX_MC_CH)+ch].memSize < minNMSizeCh) {
            minNMSizeCh = channelList[(mc*MAX_MC_CH)+ch].memSize;
          }
        }
      }
      minNMSize = minNMSizeCh ;
    }

    // Aggressive 2LM Configuration Mask
    else  {

      minNMSizeSkt = 0 ;

      channelList = &host->var.mem.socket[sckt].channelList[0];

      for(mc = 0 ; mc < host->var.mem.maxIMC ; mc++ )  {

        minNMSizeCh = 0 ;
        minNMSizeMc = 0 ;
        nmChCount = 0 ;

        memMode = CheckMemModes( host, sckt, mc );

        // No 2LM or PMEM$ on this memory controller
        if( !( ( memMode & MEM_TYPE_2LM ) || ( memMode & MEM_TYPE_PMEM_CACHE ) ) ) continue ;

        for(ch = 0 ; ch < host->var.mem.numChPerMC ; ch++) {

          // No near memory on this channel
          if(channelList[(mc*MAX_MC_CH)+ch].memSize == 0) continue ;

          // Increment count of populated NM channels
          nmChCount++;

          // Find minimal NM population on this memory controller channels
          if(minNMSizeCh == 0)  {
            minNMSizeCh = channelList[(mc*MAX_MC_CH)+ch].memSize;
          }
          else if(channelList[(mc*MAX_MC_CH)+ch].memSize < minNMSizeCh) {
            minNMSizeCh = channelList[(mc*MAX_MC_CH)+ch].memSize;
          }
        }

        // Compute the minimum NM size available for this memory controller
        minNMSizeMc = minNMSizeCh * nmChCount ;

        // If there is NM on this memory controller, then adjust the minimum NM socket size if necessary
        if( minNMSizeMc )  {

          if( ( minNMSizeSkt == 0 ) || ( minNMSizeMc < minNMSizeSkt ) ) {
            minNMSizeSkt = minNMSizeMc;
          }
        }
      }
      minNMSize = minNMSizeSkt ;
    }

    // Convert minimal nmSize on this socket to GB
    minNMSize = minNMSize >> CONVERT_64MB_TO_GB_GRAN ;

    // Mask is 14 bits, so remove bits based on the difference between the nmSize in GB (to the nearest power of two) and 4GB
    bitsToShift = 14 - ( Log2x32(GetPowerOfTwox32( minNMSize )) - Log2x32( CONVERT_64MB_TO_GB_GRAN ) ) ;

    // Set minScktIntlv to maximum socket way encoding
    minScktIntlv = TADEncodeSkWays( MAX_SAD_WAYS ) ;
    minChIntlv = 0;
    // Loop through each memory controller and channel
    for (mc = 0; mc < host->var.mem.maxIMC; mc++) {

      memMode = CheckMemModes( host, sckt, mc );

      // No 2LM or PMEM$ on this memory controller
      if( !( ( memMode & MEM_TYPE_2LM ) || ( memMode & MEM_TYPE_PMEM_CACHE ) ) ) continue ;

      for(ch = 0 ; ch < host->var.mem.numChPerMC ; ch++) {

        // Check if NM for this channel is populated
        if(channelList[(mc*MAX_MC_CH)+ch].memSize == 0) continue ;

        // Read the NM caching interleave register for this channel
        mcNMCachingIntlv.Data = MemReadPciCfgEp( host, sckt, ch + (mc * MAX_MC_CH), MCNMCACHINGINTLV_MC_2LM_REG );
        TargetIntlv = (UINT8)mcNMCachingIntlv.Bits.mcnmcachingsocketintlv;
        ChannelIntlv = (UINT8)mcNMCachingIntlv.Bits.mcnmcachingchanintlvval;

        // Pick the smallest socket interleave
        if (TargetIntlv < minScktIntlv)  {
          minScktIntlv = TargetIntlv;
        }
        // Pick the smallest ch interleave.
        //mcNMCachingIntlv.Bits.mcnmcachingchanintlvval is programmed only for 2 way NM ch interleave
        //Will be programmed to 1 in one of the interleave channel and 0 in other interleaved channel. for 1 way and 3 way its programmed to 0.
        if (ChannelIntlv > minChIntlv)  {
          minChIntlv = ChannelIntlv;
        }
      }//ch
    }//mc

    // Remove bits based on the minScktIntlv.  Remove 0 bits for 1-way interleaving, 1 bit for 2-way interleaving, 2 bits for 4-way interleaving, 3 bits for 8-way interleaving.
    // Note that minScktIntlv is encoded as 1-way (00b), 2-way (01b), 4-way (10b), and 8-way (11b), so the number of bits to shift matches the value of the minScktIntlv
    bitsToShift = bitsToShift - minScktIntlv;

    // For Safe option only, Remove bits based on the minChIntlv too. minChIntlv will have 1 for 2 way NM ch interleave alone. else 0.
    //Remove 1 bits for 2 way NM ch interleave and 0 bits for 1 way or 3 NM ch ways.
    if(host->var.mem.dfxMemVars.dfxCfgMask2LM == CFG_MASK_2LM_NORMAL)
      bitsToShift = bitsToShift - minChIntlv;

    cboTwoLMConfig.Bits.enable = 1;
    cboTwoLMConfig.Bits.mask = 0x3FFF >> bitsToShift;

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"TWO_LM_CONFIG_CHABC_SAD_REG\n"));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tenable: %x\tmask: %x\n", cboTwoLMConfig.Bits.enable, cboTwoLMConfig.Bits.mask));

    WriteCpuPciCfgEx (host, sckt, 0, TWO_LM_CONFIG_CHABC_SAD_REG, cboTwoLMConfig.Data);

    //
    //  2. Update HA_COH_CHA_MISC_REG for all CHAs on this socket
    //
    for (cha = 0; cha < totCha ; cha++) {

      // Update the HA_COH_CFG_TOR_CHA_PMA_REG
      haCohCfgTor.Data = ReadCpuPciCfgEx(host, sckt, cha, HA_COH_CFG_TOR_CHA_PMA_REG);

      haCohCfgTor.Bits.twolm = 1;

      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"HA_COH_CFG_TOR_CHA_PMA_REG\n"));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\ttwolm: %x\n", haCohCfgTor.Bits.twolm ));

      WriteCpuPciCfgEx (host, sckt, cha, HA_COH_CFG_TOR_CHA_PMA_REG, haCohCfgTor.Data);
    }
  }

  // 2LM DRAM Rule not on this Socket
  else  {
    //
    //  HA_COH.dis_spec_rd should only be set to 1 if non-2LM and HitMe$ is enabled
    //
    if (host->var.kti.OutHitMeEn == TRUE) {
      disSpecRdValue = 1;
    }
  }

  for (cha = 0; cha < totCha ; cha++) {
    //
    // Update the HA_COH_CHA_MISC_REG
    //
    haCoh.Data = ReadCpuPciCfgEx(host, sckt, cha, HA_COH_CHA_MISC_REG);

    haCoh.Bits.dis_spec_rd = disSpecRdValue;

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"HA_COH_CHA_MISC_REG\n"));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tdis_spec_rd: %x\n", haCoh.Bits.dis_spec_rd ));

    WriteCpuPciCfgEx (host, sckt, cha, HA_COH_CHA_MISC_REG, haCoh.Data);
  }
}

/**

  Encode TAD channel "ways" for TAD rule CSR in imc (memory controller)

  @param ways  - ways to encode

  @retval Encoded ways

**/
UINT8
imcTADEncodeChWays (
                   UINT8 ways
                   )
{
  return(ways - 1);
}

/**

  Encode TAD socket "ways" for TAD rule CSR.

  @param ways  - ways to encode

  @retval Encoded ways

**/
UINT8
TADEncodeSkWays(
               UINT8 ways
               )
{
  switch (ways) {
  case 1:
    ways = 0;
    break;
  case 2:
    ways = 1;
    break;
  case 4:
    ways = 2;
    break;
  case 8:
    ways = 3;
    break;
  default:
    ways = 0;
  }
  return ways;
}

/**

  Find the MC index to use for calculating channel ways.
  imcInterBitmap must be a non-zero value in input. Routine assumes value has BIT0 and/or BIT1 set.

  @param imcInterBitmap : bitmap of IMCs in the interleave
  @retval IMC number to use for calculating channel ways

**/
UINT8
GetMcIndexFromBitmap (
  UINT8 imcInterBitmap
  )
{
  UINT8 mc;

  // if this entry is IMC0/IMC1 interleaved then use MC0 since number of channels have to be the same > 1 IMC way
  if ((imcInterBitmap & BIT0) && (imcInterBitmap & BIT1)) {
    mc = 0;
  } else if (imcInterBitmap & BIT1) { // if this entry for MC1 only
    mc = 1;
  } else { // if this entry for MC0 only
    mc = 0;
  }
  return mc;
}

/**

  Find the interleave ways from the bitmap

  @param bitmap : bitmap of the interleave
  @retval number of ways interleaved

**/
UINT8
CalculateWaysfromBitmap(
                        UINT8 bitmap
                       )
{
  UINT8 i, channelWays;

  channelWays = 0;

  //Calculate Channel ways from the SAD entry (Channel interleave already calculated during SAD)
  for(i=0; i< MAX_MC_CH; i++) {
    if(bitmap & (BIT0<<i))
      channelWays++;
  }
  return channelWays;
}

/**

  Write the TAD Interleave Offset register

  @param host                    - Pointer to sysHost
  @param mc                      - mc/iMC number (0-based)
  @param ch                      - Channel number (0-based)
  @param rule                    - Rule number
  @param imcTADChnlIlvOffset0Reg - TAD Channel Interleave Offset Register
  @param TADOffsetList           - TAD Offset list
  @param TADChnIndexList         - TAD channel Index list


  @retval N/A

**/
void
WriteTADOffsetForChannel(
                        PSYSHOST host,
                        UINT8 sckt,
                        UINT8 mc,
                        UINT8 ch,
                        UINT8 rule,
                        struct TADTable *TADEntry,
                        UINT32 *TADOffsetList,
                        UINT8 *TADChnIndexList
                        )
{

  UINT8 sadIndex;
  TADCHNILVOFFSET_0_MC_MAIN_STRUCT imcTADChnlIlvOffset;
  FMTADCHNILVOFFSET_0_MC_2LM_STRUCT  fmTADChnIlvoffset;
  struct SADTable *SADEntry;
  EncodedInterleaveGranularity encodedInterleaveGranularity;

  UINT32 TADChnIlvOffset[TAD_RULES] =
    { TADCHNILVOFFSET_0_MC_MAIN_REG, TADCHNILVOFFSET_1_MC_MAIN_REG, TADCHNILVOFFSET_2_MC_MAIN_REG, TADCHNILVOFFSET_3_MC_MAIN_REG,
      TADCHNILVOFFSET_4_MC_MAIN_REG, TADCHNILVOFFSET_5_MC_MAIN_REG, TADCHNILVOFFSET_6_MC_MAIN_REG, TADCHNILVOFFSET_7_MC_MAIN_REG } ;

  UINT32 FMTADChnIlvOffset[TAD_RULES] =
    { FMTADCHNILVOFFSET_0_MC_2LM_REG, FMTADCHNILVOFFSET_1_MC_2LM_REG, FMTADCHNILVOFFSET_2_MC_2LM_REG, FMTADCHNILVOFFSET_3_MC_2LM_REG,
      FMTADCHNILVOFFSET_4_MC_2LM_REG, FMTADCHNILVOFFSET_5_MC_2LM_REG, FMTADCHNILVOFFSET_6_MC_2LM_REG, FMTADCHNILVOFFSET_7_MC_2LM_REG } ;

  sadIndex = TADEntry->SADId;
  SADEntry = &host->var.mem.socket[sckt].SAD[sadIndex];

  // Only write CSRs if channel is part of the interleave
  if ((SADEntry->channelInterBitmap[mc] & (BIT0 << ch)) || (SADEntry->FMchannelInterBitmap[mc] & (BIT0 << ch))) {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"  Channel: %d\n", ch));
    // Get encoded interleave granularities for the memmode
    encodedInterleaveGranularity = SADEncodeInterleaveGranularity( host, SADEntry->type, SADEntry->granularity ) ;

    //Skip programming TADCHNILVOFFSET_0_MC_MAIN_STRUCT for memory types that do not concern DDR4 (NM)
    if((SADEntry->type != MEM_TYPE_PMEM ) && (SADEntry->type != MEM_TYPE_BLK_WINDOW ) && (SADEntry->type != MEM_TYPE_CTRL ) ) {
      if ((SADEntry->channelInterBitmap[mc] & (BIT0 << ch))) {
        //
        //1. Update TADCHNILVOFFSET_0_MC_MAIN_STRUCT
        //
        imcTADChnlIlvOffset.Data = 0;
        imcTADChnlIlvOffset.Bits.skt_ways  = TADEncodeSkWays (TADEntry->socketWays);
        imcTADChnlIlvOffset.Bits.chn_ways = imcTADEncodeChWays(CalculateWaysfromBitmap(SADEntry->channelInterBitmap[mc]));
        imcTADChnlIlvOffset.Bits.skt_granularity  = encodedInterleaveGranularity.chaTgt;
        imcTADChnlIlvOffset.Bits.ch_granularity  = encodedInterleaveGranularity.chaCh;
        imcTADChnlIlvOffset.Bits.tad_offset = TADOffsetList[ch];
        imcTADChnlIlvOffset.Bits.chn_idx_offset = TADChnIndexList[ch];

        if((SADEntry->type == MEM_TYPE_2LM ) || (SADEntry->type == MEM_TYPE_PMEM_CACHE ))
          imcTADChnlIlvOffset.Bits.tad_offset = 0;

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    NM - skt_ways:%d  chn_ways:%d  skt_granularity:%d  ch_gran:%d tad_offset:%x chn_idx:%x\n", imcTADChnlIlvOffset.Bits.skt_ways,imcTADChnlIlvOffset.Bits.chn_ways, imcTADChnlIlvOffset.Bits.skt_granularity,imcTADChnlIlvOffset.Bits.ch_granularity, imcTADChnlIlvOffset.Bits.tad_offset, imcTADChnlIlvOffset.Bits.chn_idx_offset));
        MemWritePciCfgEp (host, sckt, ch +  (mc * MAX_MC_CH), TADChnIlvOffset[rule], imcTADChnlIlvOffset.Data);
      }
    }
    if (SADEntry->FMchannelInterBitmap[mc] & (BIT0 << ch)) {
      //
      //2. Update FMTADCHNILVOFFSET_0_MC_2LM_STRUCT
      //
      fmTADChnIlvoffset.Data = 0;
      fmTADChnIlvoffset.Bits.skt_ways  = TADEncodeSkWays (TADEntry->socketWays);
      fmTADChnIlvoffset.Bits.chn_ways  = imcTADEncodeChWays (TADEntry->channelWays);
      fmTADChnIlvoffset.Bits.skt_granularity  = encodedInterleaveGranularity.chaTgt;
      if((SADEntry->type != MEM_TYPE_PMEM ) && (SADEntry->type != MEM_TYPE_BLK_WINDOW ) && (SADEntry->type != MEM_TYPE_CTRL ) )
        fmTADChnIlvoffset.Bits.ch_granularity  = encodedInterleaveGranularity.fmCh;
      else
        fmTADChnIlvoffset.Bits.ch_granularity  = encodedInterleaveGranularity.chaCh;
    if(TADOffsetList[ch] & BIT31)
    fmTADChnIlvoffset.Bits.tad_offset_sign = 1;
    else
    fmTADChnIlvoffset.Bits.tad_offset_sign = 0;
    fmTADChnIlvoffset.Bits.tad_offset = TADOffsetList[ch] & ~BIT31;
      fmTADChnIlvoffset.Bits.chn_idx_offset = TADChnIndexList[ch];
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    FM - skt_ways:%d  chn_ways:%d  skt_granularity:%d  ch_gran:%d tad_offset:%x chn_idx:%x\n", fmTADChnIlvoffset.Bits.skt_ways,fmTADChnIlvoffset.Bits.chn_ways, fmTADChnIlvoffset.Bits.skt_granularity,fmTADChnIlvoffset.Bits.ch_granularity, fmTADChnIlvoffset.Bits.tad_offset, fmTADChnIlvoffset.Bits.chn_idx_offset));
      MemWritePciCfgEp (host, sckt, ch +  (mc * MAX_MC_CH), FMTADChnIlvOffset[rule], fmTADChnIlvoffset.Data);
    }
  }
  return;
}

/**

  Find NM interleave ways.

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC index

  @retval NM interleave ways

**/
UINT8
FindNMInterleaveWays(
             PSYSHOST  host,
             UINT8     sckt,
             UINT8     mc
             )
{
  struct ddrChannel *channelList;
  UINT8 counter = 0, ch;

  //Scan through all the channels in this mc
  for(ch =0; ch< MAX_MC_CH; ch++) {
    channelList = &host->var.mem.socket[sckt].channelList[0];
    if(channelList[(mc*MAX_MC_CH)+ch].memSize != 0) counter++;
  }

  return counter;
}

/**

  Find minimum sckt ways for this MC.

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC index

  @retval Min Socket ways for this MC

**/
UINT8
FindMinScktWays(
             PSYSHOST  host,
             UINT8     sckt,
             UINT8     mc
             )
{
  UINT8 i, minScktWays = MAX_SAD_WAYS;
  struct TADTable *TADEntry;
  //Scan through all the TAD entries to find the minimum socketways for this imc
  for (i = 0; i < TAD_RULES; i++) {
    TADEntry = &host->var.mem.socket[sckt].imc[mc].TAD[i];
    if(TADEntry->Enable == 0) break;
    if(TADEntry->socketWays < minScktWays)
      minScktWays = TADEntry->socketWays;
  }
  return minScktWays;
}

/**

  Check for the memmodes in this MC and update the variable memMode

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC Index

  @retval Bitmap of memory modes

**/
UINT8
CheckMemModes (
             PSYSHOST  host,
             UINT8     sckt,
             UINT8     mc
             )
{
  struct  Socket *socket;
  UINT8 i, memMode = 0 ;
  struct SADTable *SADEntry;

  socket = &host->var.mem.socket[sckt];
  for( i=0; i<MAX_SAD_RULES; i++) {
  SADEntry = &socket->SAD[i];
    //Break if we have reached the end of the SAD table
    if(SADEntry->Enable == 0) break;

    // Continue if entry is a remote entry
    if( SADEntry->local == 0 ) continue ;

    //Update the memMode variable based on memtype of the SAD rule it participates.
    if(socket->SADintList[i][(sckt * host->var.mem.maxIMC) + mc])
      memMode |= socket->SAD[i].type ;
  }

  return( memMode ) ;
}
/**

  Find the target id of the IMC in a SAD.

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC Index
  @param SadId - SAD ID in which tagget id need to be calculated

  @retval Target id of the IMC in the given SAD

**/
UINT8
FindTargetId (
             PSYSHOST  host,
             UINT8     sckt,
             UINT8     mc,
             UINT8     sadId
             )
{
  struct  Socket *socket;
  UINT8 targetCount =0, mcIndex=0;

  socket = &host->var.mem.socket[sckt];
  for(mcIndex=0; mcIndex<= mc; mcIndex++) {
    if( socket->SADintList[sadId][mcIndex]==1)
      targetCount++;
  }
  return targetCount-1;
}

/**

  Find minimum NM size per channel  for this MC.

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC index

  @retval Min NM size per channel for this MC

**/
UINT16
FindMinNMSize(
             PSYSHOST  host,
             UINT8     sckt,
             UINT8     mc
             )
{
  UINT8 ch ;
  UINT16  minNMSize = 0xFFFF;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrChannel (*channelList)[MAX_CH];
  struct memVar *mem;

  mem = &host->var.mem;
  channelNvList = GetChannelNvList(host, sckt);
  channelList = &mem->socket[sckt].channelList;

  // Scan each channel for the smallest NM size.
  for (ch = mc * host->var.mem.numChPerMC; ch < (mc * host->var.mem.numChPerMC) + MAX_MC_CH; ch++) {
    //Skip if the channel is not enabled.
    if ((*channelNvList)[ch].enabled == 0) continue;

    if( ((*channelList)[ch].memSize != 0) && ((*channelList)[ch].memSize < minNMSize) )
      minNMSize = (*channelList)[ch].memSize;
  }

  if (minNMSize == 0xFFFF)
    minNMSize = 0x0;
  return minNMSize;
}

/**

  Find data related to memory range cached by NM

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC index
  @param cacheRangeHigh    - Pointer to store High address of the memory range being cached
  @param cacheRangeLow    - Pointer to store Low address of the memory range being cached

  @retval EFI_SUCCESS

**/
UINT8
FindCachedRange(
             PSYSHOST  host,
             UINT8     sckt,
             UINT8     mc,
             UINT32    *cacheRangeHigh,
             UINT32    *cacheRangeLow
             )
{
  UINT8 i, maxSADRules = MAX_SAD_RULES;
  struct SADTable *SADEntry ;
  struct memVar  *mem;

  *cacheRangeHigh = 0;
  *cacheRangeLow = 0;
  mem = &host->var.mem;

  //Point Mc to the correct index in the SAD interleave table
  mc = mc + (sckt * host->var.mem.maxIMC);

  //Scan through the SADs to find the lowest address of the memory range that is being cached.
  for (i = 0; i < maxSADRules; i++) {
    SADEntry = &mem->socket[sckt].SAD[i];
    //Skip remote SADs
    if(SADEntry->local == 0) continue;
    //Skip if the MC is not part of the SAD
    if(mem->socket[sckt].SADintList[i][mc] != 1) continue;
    //Skip SADs that are not of memtype 2LM or PMEM$
    if((SADEntry->type != MEM_TYPE_2LM) && (SADEntry->type != MEM_TYPE_PMEM_CACHE)) continue;
    //If the SAD id is 0, then the base address for the SAD has to be 0.
    if(i==0)
      *cacheRangeLow =0;
    else
      *cacheRangeLow = mem->socket[sckt].SAD[i-1].Limit - 1;
    break;
  }

  //Scan through the SADs to find the highest address of the memory range that is being cached.
  for (i = 0; i < maxSADRules; i++) {
    SADEntry = &mem->socket[sckt].SAD[i];
    //Skip remote SADs
    if(SADEntry->local == 0) continue;
    //Skip if the MC is not part of the SAD
    if(mem->socket[sckt].SADintList[i][mc] != 1) continue;
    //Skip SADs that are not of memtype 2LM or PMEM$
    if((SADEntry->type != MEM_TYPE_2LM) && (SADEntry->type != MEM_TYPE_PMEM_CACHE)) continue;

    //if (SADEntry->Limit > cacheRangeHigh)
    *cacheRangeHigh = SADEntry->Limit - 1;
  }
  return EFI_SUCCESS;
}


/**
  Find if the target id of an IMC differs between two SADs with same imc ways.

  @param host  - Pointer to sysHost
  @param sckt  - Socket number
  @param mc    - MC Index
  @param minImcWays - minimum imc ways for thats socket

  @retval TRUE = If there is a target is conflict
          FALSE= If there is no conflict

**/
BOOLEAN
CheckForTgtIdConflicts (
             PSYSHOST  host,
             UINT8     sckt,
             UINT8     mc,
             UINT8     minImcWays
             )
{
  struct Socket *socket;
  UINT8 sadid, targetId = 0xff;
  struct SADTable *SADEntry;

  socket = &host->var.mem.socket[sckt];
  for (sadid = 0; sadid < MAX_SAD_RULES; sadid++) {
    SADEntry = &socket->SAD[sadid];

    //Break if we have reached the end of the SAD table
    if(SADEntry->Enable == 0) break;

    //Continue if the socket is not part of this SAD
    if(SADEntry->local == 0) continue;

    //continue if the SAD type is not 2lm or PMEM cache
    if((SADEntry->type != MEM_TYPE_2LM) && (SADEntry->type != MEM_TYPE_PMEM_CACHE)) continue;

    //Continue if this is not the SAD with min imc ways.
    //if(SADEntry->ways != minImcWays) continue;

    if(targetId == 0xff) {
      targetId = FindTargetId(host, sckt, mc, sadid);
    } else {
      if(targetId != FindTargetId(host, sckt, mc, sadid) ) {
        return TRUE;
      }
    }
  }
  return FALSE;
}
/**

  Write the TAD CSRs

  @param host  - Pointer to sysHost
  @param sckt  - Socket number

  @retval N/A

**/
void
WriteTADCSRs (
             PSYSHOST  host,
             UINT8     sckt
             )
{

  UINT8 i,j, minImcWays, sadid, minImcWaysSadId;
  UINT8 mc, ch, imcCounter;
  UINT16 minNMSize;
  UINT32 nmCacheHigh, nmCacheRangeSize, nmCacheLow;
  struct TADTable *TADEntry;
  struct Socket *socket;
  struct SADTable *SADEntry;
  struct ddrChannel *channelList;
  UINT32 *TADOffsetList;
  UINT8 *TADChnIndexList;
  UINT8 memMode;
  struct IMC* imc;
  EncodedInterleaveGranularity  encodedInterleaveGranularity;
  TADWAYNESS_0_MC_MAIN_STRUCT mcTADWayness;
  TADBASE_0_MC_MAIN_STRUCT  mcTADBase;
  MCNMCACHINGCFG_MC_MAIN_STRUCT  mcNMCachingCfg;
  MCNMCACHINGCFG2_MC_2LM_STRUCT  mcNMCachingCfg2;
  MCNMCACHINGINTLV_MC_2LM_STRUCT  mcNMCachingIntlv;
  MCNMCACHINGOFFSET0_MC_2LM_STRUCT  mcNMCachingOffset;
  // MEM_SIZE_CONTROL_MC_MAIN_STRUCT mcMemSizeCtrl;
  UINT8 logBase2ChMemSize ;
  UINT16 temp;
  // BOOLEAN progMemSizeCtrl;
  UINT16  TOLM;

  UINT32 TADWayness[TAD_RULES] =
    { TADWAYNESS_0_MC_MAIN_REG, TADWAYNESS_1_MC_MAIN_REG, TADWAYNESS_2_MC_MAIN_REG, TADWAYNESS_3_MC_MAIN_REG,
      TADWAYNESS_4_MC_MAIN_REG, TADWAYNESS_5_MC_MAIN_REG, TADWAYNESS_6_MC_MAIN_REG, TADWAYNESS_7_MC_MAIN_REG } ;

  UINT32 TADBase[TAD_RULES] =
    { TADBASE_0_MC_MAIN_REG, TADBASE_1_MC_MAIN_REG, TADBASE_2_MC_MAIN_REG, TADBASE_3_MC_MAIN_REG,
      TADBASE_4_MC_MAIN_REG, TADBASE_5_MC_MAIN_REG, TADBASE_6_MC_MAIN_REG, TADBASE_7_MC_MAIN_REG } ;

  socket = &host->var.mem.socket[sckt];
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tUpdating TAD CSRs for Socket %d\n", sckt));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));

  // TOLM in 64MB units
  //
  TOLM = (UINT16)(FOUR_GB_MEM - host->setup.common.lowGap);
  // Has to be on a 256MB boundary
  TOLM = TOLM & ~0x3;

  // Write TADs for each mc
  for (mc = 0; mc <  host->var.mem.maxIMC; mc++) {

    if (host->nvram.mem.socket[sckt].imc[mc].enabled == 0) continue;

    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Memory Controller : %d\n", mc));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"------------------------\n"));

    imc = &socket->imc[mc];

    //
    // 1. Program MCNMCACHINGCFG_MC_MAIN_REG
    //
    mcNMCachingCfg.Data = 0;

    memMode = CheckMemModes(host, sckt, mc );

    //Set the corresponding bit in the CSR
    if(memMode & MEM_TYPE_2LM )
      mcNMCachingCfg.Bits.mcnmcachingenb = 1;

    if(memMode & MEM_TYPE_PMEM_CACHE )
      mcNMCachingCfg.Bits.mcnmcachingpmemwt = 1;

    //4930392: Cloned From SKX Si Bug Eco: m2m seems to expect Pmem bit in mode register set for 1lm+block mode (Workaround)
    if(memMode & MEM_TYPE_CTRL )
      mcNMCachingCfg.Bits.mcpmemenb = 1;

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"MCNMCACHINGCFG_MC_MAIN_REG\n"));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tmcnmcachingenb: %x  mcpmemenb: %x  mcnmcachingpmemwt: %x \n\n",  mcNMCachingCfg.Bits.mcnmcachingenb,  mcNMCachingCfg.Bits.mcpmemenb, mcNMCachingCfg.Bits.mcnmcachingpmemwt));

    MemWritePciCfgMC (host, sckt, mc, MCNMCACHINGCFG_MC_MAIN_REG, mcNMCachingCfg.Data);

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Writing Channel level CSRs\n"));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));

    //
    //2. Program registers required for NM caching support
    //
    if( (memMode & MEM_TYPE_2LM) || (memMode & MEM_TYPE_PMEM_CACHE ) )  {

      //Scan through all the channels in this mc
      for(ch =0; ch< MAX_MC_CH; ch++) {
        channelList = &host->var.mem.socket[sckt].channelList[0];

        //Check if NM for this channel is populated
        if(channelList[(mc*MAX_MC_CH)+ch].memSize == 0) continue ;
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"  Channel - %d\n", ch));

        //
        // Fix to HSD-4926276:For 2LM, shall allow a maximum of 512GB of near memory capacity on each channel
        // Checks if NM for each channel doesn't exceed 512GB, otherwise a warning will be thrown
        //
        if(channelList[(mc*MAX_MC_CH)+ch].memSize >= (MAX_2LM_NM_IN_MB / MEM_GRANULARITY))  {
           MemDebugPrint((host, SDBG_MINMAX, sckt,(mc*MAX_MC_CH)+ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "DDR4 cache greater than 512GB\n"));
           OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_NM_MAX_SIZE_EXCEEDED, sckt, (mc*MAX_MC_CH)+ch, 0xFF, 0xFF);
        }

        //
        // Fix to HSD-4926277:For 2LM, the minimum per channel NM size supported shall be 4GB
        // Checks if NM for each channel is at least 4GB, otherwise a warning will be thrown
        //
        if(channelList[(mc*MAX_MC_CH)+ch].memSize < (MIN_2LM_NM_IN_MB / MEM_GRANULARITY))  {
           MemDebugPrint((host, SDBG_MINMAX, sckt,(mc*MAX_MC_CH)+ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "DDR4 cache less than 4GB\n"));
           OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_NM_SIZE_BELOW_MIN_LIMIT, sckt,(mc*MAX_MC_CH)+ch, 0xFF, 0xFF);
#if SMCPKG_SUPPORT
           OutputError (host, ERR_NGN, INTERLEAVE_EXCEED, sckt, (mc*MAX_MC_CH)+ch, NO_DIMM, NO_RANK);
#else
           FatalError(host, ERR_NGN, INTERLEAVE_EXCEED);
#endif
        }

        //
        // Fix to HSD-4926278:For 2LM, the  per channel NM size shall be a power of 2
        // Checks if NM size is a power of 2, otherwise a warning will be thrown
        //
        temp = channelList[(mc*MAX_MC_CH)+ch].memSize;
        while ((( temp % 2) == 0) && temp > 1) {
           temp /= 2;
        }
        if (temp != 1) {
           MemDebugPrint((host, SDBG_MINMAX, sckt,(mc*MAX_MC_CH)+ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "DDR4 cache not power of two\n"));
           OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_NM_SIZE_NOT_POWER_OF_TWO, sckt, (mc*MAX_MC_CH)+ch, 0xFF, 0xFF);
        }

        //
        //2A. Program MCNMCACHINGCFG2_MC_2LM_REG
        //
        mcNMCachingCfg2.Data = 0;

        // mcnmcachingnmchncap - Bits[3:0] 0-4GB 1-8GB 2-16GB 3-32GB 4-64GB 5-128GB 6-256GB 7-512GB
        // Find the largest integer that is both a power of two and less than or equal to channel size in GB, then determine log2 of that integer
        logBase2ChMemSize = Log2x32( GetPowerOfTwox32( channelList[(mc*MAX_MC_CH)+ch].memSize >> CONVERT_64MB_TO_GB_GRAN ) ) ;

        // Greater than 512GB, then force to 512GB
        if( logBase2ChMemSize > 9 )  {
          logBase2ChMemSize = 9 ;
        }

        // Encode mcnmcachingnmchncap bits
        if( logBase2ChMemSize > 2 )  {
          mcNMCachingCfg2.Bits.mcnmcachingnmchncap = logBase2ChMemSize - 2 ;
        }
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    MCNMCACHINGCFG2_MC_2LM_REG.Bits.mcnmcachingnmchncap - %x\n", mcNMCachingCfg2.Bits.mcnmcachingnmchncap));

        MemWritePciCfgEp (host, sckt, ch + (mc * MAX_MC_CH), MCNMCACHINGCFG2_MC_2LM_REG, mcNMCachingCfg2.Data);
        //
        //2B. Program MCNMCACHINGINTLV_MC_2LM_REG
        //
        mcNMCachingIntlv.Data = 0;
        minImcWaysSadId = 0;
        minImcWays = MAX_SAD_WAYS;
        for (sadid = 0; sadid < MAX_SAD_RULES; sadid++) {
          SADEntry = &socket->SAD[sadid];
          //Break if we have reached the end of the SAD table
          if(SADEntry->Enable == 0) break;
          //Find the local SAD rule has the smallest imc ways, store it.
          if (((SADEntry->local == 1) && (SADEntry->Attr == 0)) && (minImcWays > SADEntry->ways)){
            minImcWays = SADEntry->ways;    //This will be used o set c.mcnmcachingsocketintlv
            minImcWaysSadId = sadid;
          }
        }

        //a. mcnmcaching_tad_il_granular_ximc
        //b. mcnmcachingchanintlvval

        //HSD4929646: Commenting out the below code as a work around for a silicon issue.
        /*if(FindNMInterleaveWays(host, sckt, mc) == 2) {
          //Set this to one for 2 way NM channel interleave
          mcNMCachingIntlv.Bits.mcnmcaching_tad_il_granular_ximc = 1;
          //If two way NM interleave, set this bit to 0 for one ch and one for the other.
          if((FirstInChInterleave == 0) && (channelList[(mc*MAX_MC_CH)+ch].memSize != 0)){
            mcNMCachingIntlv.Bits.mcnmcachingchanintlvval = 1;
            FirstInChInterleave = 1;
          } else
              mcNMCachingIntlv.Bits.mcnmcachingchanintlvval = 0;
        } else {*/
          //init this bit to Zero, if not 2 way NM interleave
          mcNMCachingIntlv.Bits.mcnmcaching_tad_il_granular_ximc = 0;
        //}
        //c. mcnmcachingsocketintlv
        //find the minimum imc ways this socket is interleaved.
        mcNMCachingIntlv.Bits.mcnmcachingsocketintlv = TADEncodeSkWays(minImcWays);

        //d. mcnmcachingsocketintlvval
        if(minImcWays != 1 ) {
          //Check for Target id conflicts
          //If there is a target id conflict between different SADs, program the failsafe values.
          if(CheckForTgtIdConflicts(host, sckt, mc, minImcWays)) {
            mcNMCachingIntlv.Bits.mcnmcachingsocketintlv = TADEncodeSkWays(ONE_WAY);
            mcNMCachingIntlv.Bits.mcnmcachingsocketintlvval = 0;
          } else {
            mcNMCachingIntlv.Bits.mcnmcachingsocketintlvval = FindTargetId(host, sckt, mc, minImcWaysSadId);
          }
        } else {
          mcNMCachingIntlv.Bits.mcnmcachingsocketintlvval = 0;
        }

        //e. mcnmcaching_tad_ch_way
        // Removed in 14ww07a CSR XML:
        //mcNMCachingIntlv.Bits.mcnmcaching_tad_ch_way = FindNMInterleaveWays(host, sckt, mc) - 1;

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    MCNMCACHINGINTLV_MC_2LM_REG\n"));
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    \tgranular_ximc: %x chn_int_val: %x skt_int: %x skt_int_val: %x\n", mcNMCachingIntlv.Bits.mcnmcaching_tad_il_granular_ximc, mcNMCachingIntlv.Bits.mcnmcachingchanintlvval, mcNMCachingIntlv.Bits.mcnmcachingsocketintlv, mcNMCachingIntlv.Bits.mcnmcachingsocketintlvval ));

        MemWritePciCfgEp (host, sckt, ch + (mc * MAX_MC_CH), MCNMCACHINGINTLV_MC_2LM_REG, mcNMCachingIntlv.Data);

        //
        //2C. Update MCNMCACHINGOFFSET0_MC_2LM_REG
        //
        mcNMCachingOffset.Data = 0;

        //Calculate the smallest NM size  per channel in this MC
        minNMSize = FindMinNMSize (host, sckt, mc);

        //Calculate the NM$_Range_Hi & NM$_Range_Lo
        FindCachedRange(host, sckt, mc, &nmCacheHigh, &nmCacheLow);
        nmCacheRangeSize = (nmCacheHigh - nmCacheLow) >>  4; //Store it in Gb for easier use below.

        mcNMCachingOffset.Bits.mcnmcachingoffset0 = 0;

        //If the minimim NM size per channel is 4GB in this MC
        if( (minNMSize >> 4) == 4) {
          //If highest address cached by NM is >= 16TB AND NMCachingSocketInterleave = 1
          if( ((nmCacheHigh >> 4) >= MEM_SIZE_16TB) && (mcNMCachingIntlv.Bits.mcnmcachingsocketintlv == 0) ) {
            //If the range that is cached is greater than 16TB, HALT.
            if(nmCacheRangeSize >= MEM_SIZE_16TB){
              MemDebugPrint((host, SDBG_MINMAX, sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Unsupported memory size. DDR4 cache too small for the given NVMDIMM memory.\n"));
              OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_NM_SIZE_BELOW_MIN_LIMIT, sckt, 0xFF, 0xFF, 0xFF);
#if SMCPKG_SUPPORT
              OutputError (host, ERR_NGN, INTERLEAVE_EXCEED, sckt, NO_CH, NO_DIMM, NO_RANK);
#else
              FatalError(host, ERR_NGN, INTERLEAVE_EXCEED);
#endif
            }
            mcNMCachingOffset.Bits.mcnmcachingoffset0 = nmCacheLow & minNMSize;
          }
          //If highest address cached by NM is >= 32TB AND NMCachingSocketInterleave = 2
          else if ( ((nmCacheHigh >> 4) >= MEM_SIZE_32TB) && (mcNMCachingIntlv.Bits.mcnmcachingsocketintlv == 1) ) {
            //If the range that is cached is greater than 32TB, HALT.
            if(nmCacheRangeSize >= MEM_SIZE_32TB){
              MemDebugPrint((host, SDBG_MINMAX, sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Unsupported memory size. DDR4 cache too small for the given NVMDIMM memory.\n"));
              OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_NM_SIZE_BELOW_MIN_LIMIT, sckt, 0xFF, 0xFF, 0xFF);
#if SMCPKG_SUPPORT
              OutputError (host, ERR_NGN, INTERLEAVE_EXCEED, sckt, NO_CH, NO_DIMM, NO_RANK);
#else
              FatalError(host, ERR_NGN, INTERLEAVE_EXCEED);
#endif
            }
            mcNMCachingOffset.Bits.mcnmcachingoffset0 = nmCacheLow & minNMSize;
          }
        } else if ((minNMSize >> 4) == 8) {
          //If highest address cached by NM is >= 32TB AND NMCachingSocketInterleave = 1
          if( ((nmCacheHigh >> 4) >= MEM_SIZE_32TB) && (mcNMCachingIntlv.Bits.mcnmcachingsocketintlv == 0) ) {
            //If the range that is cached is greater than 32TB, HALT.
            if(nmCacheRangeSize >= MEM_SIZE_32TB){
              MemDebugPrint((host, SDBG_MINMAX, sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "Unsupported memory size. DDR4 cache too small for the given NVMDIMM memory.\n"));
              OutputWarning(host, WARN_INTERLEAVE_FAILURE, WARN_NM_SIZE_BELOW_MIN_LIMIT, sckt, 0xFF, 0xFF, 0xFF);
#if SMCPKG_SUPPORT
              OutputError (host, ERR_NGN, INTERLEAVE_EXCEED, sckt, NO_CH, NO_DIMM, NO_RANK);
#else
              FatalError(host, ERR_NGN, INTERLEAVE_EXCEED);
#endif
            }
            mcNMCachingOffset.Bits.mcnmcachingoffset0 = nmCacheLow & minNMSize;
          }
        }

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    MCNMCACHINGOFFSET0_MC_2LM_REG : %x \n", mcNMCachingOffset.Bits.mcnmcachingoffset0));

        MemWritePciCfgEp (host, sckt, ch + (mc * MAX_MC_CH), MCNMCACHINGOFFSET0_MC_2LM_REG, mcNMCachingOffset.Data);
      }
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
    }

    //
    // HSD 5332036 - Removed support for setting up fastDiv3 on 3-way configurations.
    //
    //3. Program registers for MEM_SIZE_CONTROL
    //
    //if( memMode & MEM_TYPE_1LM )  {
    //
    //  channelList = &host->var.mem.socket[sckt].channelList[0];
    //
    //  // Check if memory population is valid to program MEM_SIZE_CONTROL
    //  progMemSizeCtrl = TRUE ;
    //  for(ch = 0; ch< MAX_MC_CH; ch++) {
    //
    //    // If any channel is not populated or is populated with memory that is not a power of 2 in GB then MEM_SIZE_CONTROL are not programmed
    //    if( !( ( channelList[(mc*MAX_MC_CH)+ch].memSize ) && ( IsPowerOfTwo(channelList[(mc*MAX_MC_CH)+ch].memSize >> 4) ) ) ) {
    //      progMemSizeCtrl = FALSE ;
    //      break;
    //    }
    //  }
    //
    //  // Program register for each channel
    //  if( progMemSizeCtrl == TRUE ) {
    //    for(ch = 0; ch< MAX_MC_CH; ch++) {
    //
    //      mcMemSizeCtrl.Data = 0 ;
    //      mcMemSizeCtrl.Bits.enable = 1 ;
    //      mcMemSizeCtrl.Bits.nm_chn_cap = Log2x32( channelList[(mc*MAX_MC_CH)+ch].memSize >> 4 ) - 2 ;
    //
    //      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"  Channel - %d\n", ch));
    //
    //      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    MEM_SIZE_CONTROL_MC_MAIN_REG\n" ));
    //      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    \tenable : %x nm_chn_cap : %x\n", mcMemSizeCtrl.Bits.enable, mcMemSizeCtrl.Bits.nm_chn_cap ));
    //
    //      MemWritePciCfgEp (host, sckt, ch + (mc * MAX_MC_CH), MEM_SIZE_CONTROL_MC_MAIN_REG, mcMemSizeCtrl.Data);
    //    }
    //  }
    //}

    //
    //4. Program registers for TADWAYNESS, TADBASE, TADCHNILVOFFSET, and FMTADCHNILVOFFSET
    //
    for (i = 0; i < TAD_RULES; i++) {
      if(imc->TAD[i].Enable == 0) break;
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Updating TAD level CSRs for TAD %x\n", i));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
      TADEntry = &imc->TAD[i];
      TADOffsetList = imc->TADOffset[i];
      TADChnIndexList = imc->TADChnIndex[i];

      //
      //4A. Update TADWAYNESS_0_MC_MAIN_REG
      //
      mcTADWayness.Data = 0;
      if (TADEntry->Enable) {
        //ch_way should represent the Near mem channel ways. But TADEntry->channelWays will have FM channel ways in 2lm/pmem$ case.
        if( (memMode & MEM_TYPE_2LM) || (memMode & MEM_TYPE_PMEM_CACHE ))
          mcTADWayness.Bits.tad_ch_way = imcTADEncodeChWays(FindNMInterleaveWays(host, sckt, mc));
        else
          mcTADWayness.Bits.tad_ch_way = imcTADEncodeChWays (TADEntry->channelWays);

        //skt_way should represent the near memory socket ways
        mcTADWayness.Bits.tad_skt_way = TADEncodeSkWays( TADEntry->socketWays );
      }
      mcTADWayness.Bits.tad_limit = SADTADEncodeLimit (TADEntry->Limit);
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"TADWAYNESS : \n\tChway : %x  SktWay : %x  Limit : %x \n", mcTADWayness.Bits.tad_ch_way, mcTADWayness.Bits.tad_skt_way, mcTADWayness.Bits.tad_limit));

      MemWritePciCfgMC (host, sckt, mc, TADWayness[i], mcTADWayness.Data);

      //
      //4B. Update TADBASE_0_MC_MAIN_REG
      //
      mcTADBase.Data = 0;

      // Get encoded interleave granularities for the memmode
      encodedInterleaveGranularity = SADEncodeInterleaveGranularity( host, socket->SAD[TADEntry->SADId].type, socket->SAD[TADEntry->SADId].granularity ) ;
      mcTADBase.Bits.skt_granularity = encodedInterleaveGranularity.chaTgt;
      mcTADBase.Bits.chn_granularity = encodedInterleaveGranularity.chaCh;

      if( TADEntry->SADId == 0 ) {
        mcTADBase.Bits.base = 0;
      }
      else {
        mcTADBase.Bits.base = socket->SAD[TADEntry->SADId -1].Limit;
      }

      imcCounter = 0;

      //Calculate the position of imc in the SADINTlist.
      for(j=0; j<(host->var.mem.maxIMC*sckt)+mc; j++) {
        if(socket->SADintList[TADEntry->SADId][j] == 1) imcCounter++;
      }
      //
      mcTADBase.Bits.base_offset = imcCounter;

      // In 3-way or 6-way configurations, some memory may be hidden behind the MMIO hole, so need to
      // downgrade patrol scrub errors for this memory
      if ((sckt == 0) && (TADEntry->SADId == 0) && (mcTADWayness.Bits.tad_ch_way == 2)) {
        if( TOLM % (socket->SAD[TADEntry->SADId].ways * 3)) {
          mcTADBase.Bits.ign_ptrl_uc = 1;
        }
      }

      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"TADBASE : \n \tbase : %x  base_offset : %x  skt_gran : %x  ch_gran : %x  ign_ptr_uc : %x\n", mcTADBase.Bits.base, mcTADBase.Bits.base_offset, mcTADBase.Bits.skt_granularity, mcTADBase.Bits.chn_granularity, mcTADBase.Bits.ign_ptrl_uc ));

      MemWritePciCfgMC (host, sckt, mc, TADBase[i], mcTADBase.Data);

      //
      //4C. Update TADCHNILVOFFSET_0_MC_MAIN_REG & FMTADCHNILVOFFSET_0_MC_2LM_REG
      //
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"TADCHNILVOFFSET :\n"));
      //Fill Channel Interleave list for each channel
      for (ch = 0; ch <  host->var.mem.numChPerMC; ch++) {
        WriteTADOffsetForChannel (host, sckt, mc, ch, i, TADEntry, TADOffsetList, TADChnIndexList);
      }
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"  \n\n"));
    } // TAD loop


  } // mc loop

  return;
}

UINT32
RIREncodeLimit (UINT32 Limit)
/**

  Encode Limit field for RIR DRAM rules

  @param Limit - Limit to encode

  @retval Encoded Limit

**/
{
  // Adjust for 64BM -> 512MB granularity and for upper range Limit being <= Limit
  return((Limit - 1) >> 3);
}

UINT8
RIREncodeRIRWays(UINT8 ways)
/**

  Encode RIR "ways" for RIR rule CSR.

  @param ways  - ways to encode

  @retval Encoded ways

**/
{
  switch (ways) {
  case 1:
    ways = 0;
    break;
  case 2:
    ways = 1;
    break;
  case 4:
    ways = 2;
    break;
  case 8:
    ways = 3;
    break;
  default:
    ways = 0;
  }

  return ways;

}

void
WriteRIRForChannel(PSYSHOST host,
                   UINT8 sckt,
                   UINT8 ch)
/**

  Write the RIR CSRs

  @param host                    - Pointer to sysHost
  @param sckt                  - Socket number
  @param ch                      - Channel number (0-based)

  @retval N/A

**/
{
  UINT8 rir, i;
  RIRWAYNESSLIMIT_0_MC_MAIN_STRUCT  imcRIRWaynessLimit;
  RIRILV0OFFSET_0_MC_MAIN_STRUCT    imcRIRIlvOffset;
  FMRIRWAYNESSLIMIT_0_MC_2LM_STRUCT imcFMRIRWaynessLimit;
  FMRIRILV0OFFSET_0_MC_2LM_STRUCT   imcFMRIRIlvOffset;

  struct ddrChannel *channel;
  struct channelNvram *channelNvList;
  struct dimmNvram (*dimmNvList)[MAX_DIMM];
  UINT8  aepPresent;
  UINT8  dimm;

  UINT32 RIRWaynessLimit[MAX_RIR] =
    { RIRWAYNESSLIMIT_0_MC_MAIN_REG, RIRWAYNESSLIMIT_1_MC_MAIN_REG, RIRWAYNESSLIMIT_2_MC_MAIN_REG, RIRWAYNESSLIMIT_3_MC_MAIN_REG } ;

  UINT32 RIRIlvOffset[MAX_RIR][MAX_RIR_WAYS] =
    { { RIRILV0OFFSET_0_MC_MAIN_REG, RIRILV1OFFSET_0_MC_MAIN_REG, RIRILV2OFFSET_0_MC_MAIN_REG, RIRILV3OFFSET_0_MC_MAIN_REG, RIRILV4OFFSET_0_MC_MAIN_REG, RIRILV5OFFSET_0_MC_MAIN_REG, RIRILV6OFFSET_0_MC_MAIN_REG, RIRILV7OFFSET_0_MC_MAIN_REG },
      { RIRILV0OFFSET_1_MC_MAIN_REG, RIRILV1OFFSET_1_MC_MAIN_REG, RIRILV2OFFSET_1_MC_MAIN_REG, RIRILV3OFFSET_1_MC_MAIN_REG, RIRILV4OFFSET_1_MC_MAIN_REG, RIRILV5OFFSET_1_MC_MAIN_REG, RIRILV6OFFSET_1_MC_MAIN_REG, RIRILV7OFFSET_1_MC_MAIN_REG },
      { RIRILV0OFFSET_2_MC_MAIN_REG, RIRILV1OFFSET_2_MC_MAIN_REG, RIRILV2OFFSET_2_MC_MAIN_REG, RIRILV3OFFSET_2_MC_MAIN_REG, RIRILV4OFFSET_2_MC_MAIN_REG, RIRILV5OFFSET_2_MC_MAIN_REG, RIRILV6OFFSET_2_MC_MAIN_REG, RIRILV7OFFSET_2_MC_MAIN_REG },
      { RIRILV0OFFSET_3_MC_MAIN_REG, RIRILV1OFFSET_3_MC_MAIN_REG, RIRILV2OFFSET_3_MC_MAIN_REG, RIRILV3OFFSET_3_MC_MAIN_REG, RIRILV4OFFSET_3_MC_MAIN_REG, RIRILV5OFFSET_3_MC_MAIN_REG, RIRILV6OFFSET_3_MC_MAIN_REG, RIRILV7OFFSET_3_MC_MAIN_REG } };

  UINT32 FMRIRWaynessLimit[MAX_RIR_DDRT] =
    { FMRIRWAYNESSLIMIT_0_MC_2LM_REG, FMRIRWAYNESSLIMIT_1_MC_2LM_REG, FMRIRWAYNESSLIMIT_2_MC_2LM_REG, FMRIRWAYNESSLIMIT_3_MC_2LM_REG } ;

  UINT32 FMRIRIlvOffset[MAX_RIR_DDRT][MAX_RIR_DDRT_WAYS] =
    { { FMRIRILV0OFFSET_0_MC_2LM_REG, FMRIRILV1OFFSET_0_MC_2LM_REG },
      { FMRIRILV0OFFSET_1_MC_2LM_REG, FMRIRILV1OFFSET_1_MC_2LM_REG },
      { FMRIRILV0OFFSET_2_MC_2LM_REG, FMRIRILV1OFFSET_2_MC_2LM_REG },
      { FMRIRILV0OFFSET_3_MC_2LM_REG, FMRIRILV1OFFSET_3_MC_2LM_REG } };

  channelNvList = &(*GetChannelNvList(host, sckt))[ch];

  // Only write CSRs if channel is enabled
  if (channelNvList->enabled != 0) {

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"  Channel - %d\n", ch));

    channel = &host->var.mem.socket[sckt].channelList[ch];

    for (rir = 0; rir < MAX_RIR; rir++) {

      //Update RIR wayness register
      imcRIRWaynessLimit.Data = 0;
      imcRIRWaynessLimit.Bits.rir_val = channel->rirValid[rir];
      imcRIRWaynessLimit.Bits.rir_limit = RIREncodeLimit (channel->rirLimit[rir]);
      imcRIRWaynessLimit.Bits.rir_way = RIREncodeRIRWays(channel->rirWay[rir]);

      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    RIRWAYNESSLIMIT_%d_MC_MAIN_REG\n", rir));
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    \trir_val: %x\trir_limit: %x\trir_way: %x\n\n", imcRIRWaynessLimit.Bits.rir_val, imcRIRWaynessLimit.Bits.rir_limit, imcRIRWaynessLimit.Bits.rir_way ));

      MemWritePciCfgEp (host, sckt, ch, RIRWaynessLimit[rir], imcRIRWaynessLimit.Data);

      //Update RIR Interleave offset register
      for (i = 0; i < MAX_RIR_WAYS; i++) {
        imcRIRIlvOffset.Data = 0;
        imcRIRIlvOffset.Bits.rir_rnk_tgt0 = channel->rirIntList[rir][i];
        imcRIRIlvOffset.Bits.rir_offset0 = channel->rirOffset[rir][i];

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"        RIRILV%dOFFSET_%d_MC_MAIN_REG\n", i, rir));
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"        \trir_rnk_tgt0: %x\trir_offset0: %x\n\n", imcRIRIlvOffset.Bits.rir_rnk_tgt0, imcRIRIlvOffset.Bits.rir_offset0 ));

        MemWritePciCfgEp (host, sckt, ch, RIRIlvOffset[rir][i], imcRIRIlvOffset.Data);
      } /* rank way loop */
    } /* rir loop */

    // Find NVMCTLR DIMM on this channel - only a single NVMCTLR DIMM per channel is supported
    aepPresent = 0 ;
    dimmNvList = GetDimmNvList(host, sckt, ch);
    for (dimm = 0; dimm < channelNvList->maxDimm; dimm++) {
      if((*dimmNvList)[dimm].dimmPresent == 0) continue;
      if((*dimmNvList)[dimm].aepDimmPresent)  {
        aepPresent = 1 ;
        break;
      }
    }

    // Write the FM RIR registers only if an NVMCTLR DIMM exists on this channel
    if( aepPresent == 1 ) {

      for (rir = 0; rir < MAX_RIR_DDRT; rir++) {

        // Update FM RIR wayness register
        imcFMRIRWaynessLimit.Data = 0;
        imcFMRIRWaynessLimit.Bits.rir_val = channel->rirDDRTValid[rir];
        imcFMRIRWaynessLimit.Bits.rir_limit = RIREncodeLimit(channel->rirDDRTLimit[rir]);
        imcFMRIRWaynessLimit.Bits.rir_way = RIREncodeRIRWays(channel->rirDDRTWay[rir]);

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    FMRIRWAYNESSLIMIT_%d_MC2LM_REG\n", rir));
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"    \trir_val: %x\trir_limit: %x\trir_way: %x\n\n", imcFMRIRWaynessLimit.Bits.rir_val, imcFMRIRWaynessLimit.Bits.rir_limit, imcFMRIRWaynessLimit.Bits.rir_way ));

        MemWritePciCfgEp (host, sckt, ch, FMRIRWaynessLimit[rir], imcFMRIRWaynessLimit.Data);

        //Update FM RIR Interleave offset register
        for (i = 0; i < MAX_RIR_DDRT_WAYS; i++) {
          imcFMRIRIlvOffset.Data = 0;
          imcFMRIRIlvOffset.Bits.rir_rnk_tgt0 = channel->rirDDRTIntList[rir][i];
          imcFMRIRIlvOffset.Bits.rir_offset0 = channel->rirDDRTOffset[rir][i];

          MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"        FMRIRILV%dOFFSET_%d_MC2LM_REG\n", i, rir));
          MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"        \trir_rnk_tgt0: %x\trir_offset0: %x\n\n", imcFMRIRIlvOffset.Bits.rir_rnk_tgt0, imcFMRIRIlvOffset.Bits.rir_offset0 ));

          MemWritePciCfgEp (host, sckt, ch, FMRIRIlvOffset[rir][i], imcFMRIRIlvOffset.Data);
        } /* rank way loop */
      } /* rir loop */
    } /* if NVMCTLR Present */
  } /* if channel enabled */
  return;

}

//-----------------------------------------------------------------------------
// WriteRIRCSRs
//
// Description: Fill in RIR CRSs for memory map from config info in host
// structure.
//
//-----------------------------------------------------------------------------

void
WriteRIRCSRs (PSYSHOST host, UINT8 sckt)
/**

  Write the RIR CSRs

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
{
  UINT8 ch;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tWrite RIR CSRs for Socket: %d\n", sckt));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));

  // RIRs for channels
  for (ch = 0; ch < MAX_CH; ch++) {
    WriteRIRForChannel (host, sckt, ch);
  }

  return;
}

void
WriteSpareRankCSRs (PSYSHOST host, UINT8 sckt)
/**

  Write logical rank ids of spare ranks to non-sticky scratchpad register.
  HSD 3612093.

  @param host    - Pointer to sysHost
  @param sckt  - socket to process

  @retval N/A

**/
{
/*
  UINT8 ch;
  UINT32 spareranks = 0;
  struct channelNvram *channelNvList;

  //
  // Check if sparing enabled
  //
  if (host->nvram.mem.RASmode & RK_SPARE) {
    // bits 0:2   logical rank id of ch 0 spare or failed rank
    // bit  3     ch0 enabled
    // bits 4:6   logical rank id of ch 1 spare or failed rank
    // bit  7     ch1 enabled
    // bits 8:10  logical rank id of ch 2 spare or failed rank
    // bit  11    ch 2 enabled
    // bits 12:14 logical rank id of ch 3 spare or failed rank
    // bit  15    ch 3 enabled
    // bit  16    ch 0 sparing occurred
    // bit  17    ch 1 sparing occurred
    // bit  18    ch 2 sparing occurred
    // bit  19    ch 3 sparing occurred

    for (ch = 0; ch < MAX_CH; ch++) {
     channelNvList = &(*GetChannelNvList(host, sckt))[ch];

      //
      // Only include enabled channels
      //
      if (channelNvList->enabled != 0) {
        //
        // Bit 3 of nibble is channel enabled bit, bits 2:0 of nibble are logical id of spare rank
        //
        spareranks |= (0x8 | (channelNvList->spareLogicalRank & 0x7)) << ch*4;
        MemDebugPrint((host, SDBG_MAX, sckt, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                      "SpareLogRank:0x%x ", channelNvList->spareLogicalRank));
      }
    }
  } else {
    spareranks = 0;
  }

  //
  // Write CSR
  //
  MemDebugPrint((host, SDBG_MAX, sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "BIOSNONSTICKYSCRATCHPAD4 :0x%x\n", spareranks));
  return;
  */
}


void
WriteTolmTohmCSRs (PSYSHOST host)
/**

  Fill in TOLM/TOHM CRSs for memory map from config info in host
  structure. For all sockets.

  @param host  - Pointer to sysHost

  @retval N/A

**/
{

  UINT8 sckt=0, stack;
  UINT8 socketsEn = 0;
  UINT32 SADLimit = 0;
  UINT32 TOLMLimit;
  UINT32 tolmTmp;
  struct memVar *mem;
  struct memNvram *nvramMem;
  TOLM_IIO_VTD_STRUCT tolm_5_0;
  TOHM_0_IIO_VTD_STRUCT  tohm_0_5_0;
  TOHM_1_IIO_VTD_STRUCT  tohm_1_5_0;
  UINT8 maxSADRules = MAX_SAD_RULES;

  nvramMem = &host->nvram.mem;
  mem = &host->var.mem;

  tolm_5_0.Data = 0;
  tohm_0_5_0.Data = 0;
  tohm_1_5_0.Data = 0;

  // Program the TOLM/TOHM based on the SAD rule limits
  // Note: IIO tolm/tohm (device 0/5)  -- addr range <= Limit
  //       CBO tolm/tohm (device 12/7) -- addr range <  Limit
  maxSADRules =  FindSadIndex(host, sckt);

  // Compute SADLimit and TOLM such that  "addr range" <= {SAD,TOLM}Limit (64MB granularity)
  if ( (maxSADRules > 0) && (maxSADRules < MAX_SAD_RULES))
    SADLimit = mem->socket[sckt].SAD[maxSADRules - 1].Limit - 1;

  TOLMLimit = ((FOUR_GB_MEM - host->setup.common.lowGap) & ~0x3) - 1;

  if (TOLMLimit < SADLimit) {
    tolm_5_0.Bits.addr = TOLMLimit;
  } else {
    if ((SADLimit + 1) & 0x3) {
      tolmTmp = (0x4 - ((SADLimit + 1) & 0x3)) + SADLimit;
    } else {
      tolmTmp = SADLimit;
    }
    tolm_5_0.Bits.addr = tolmTmp;
  }

  host->var.mem.lowMemBase = 0;
  host->var.mem.lowMemSize = tolm_5_0.Bits.addr + 1;

  // highMemBase initialized in InitMemoryMap

  // Calculating the number of sockets on which IOT is enabled
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if ((host->nvram.mem.socket[sckt].enabled == 0) || (host->nvram.mem.socket[sckt].maxDimmPop == 0) || (host->var.mem.iotDisabled[sckt] == 1)) continue;
    socketsEn +=1;
  }

  if (SADLimit < FOUR_GB_MEM) {
    // No memory above 4GB
    host->var.mem.highMemSize = 0;
  } else {
      if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN)
        host->var.mem.highMemSize = (SADLimit -  (BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB)) + 1);
      else
        host->var.mem.highMemSize = (SADLimit - FOUR_GB_MEM + 1); // i.e. For 1 socket IOTMemory*1
  }

  if (SADLimit >= FOUR_GB_MEM) {
     tohm_0_5_0.Bits.addr = SADLimit;
     tohm_1_5_0.Bits.addr = SADLimit >> 6;
  } else {
     tohm_0_5_0.Bits.addr = 0x3F;
     tohm_1_5_0.Bits.addr = 0x3F >> 6;
  }
  //
  // Save TOLM into host structure for later use
  //
  host->var.common.tolmLimit = (UINT16)(tolm_5_0.Bits.addr + 1);
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "lowMemBase: 0x%x\nlowMemSize: 0x%x\n", host->var.mem.lowMemBase, host->var.mem.lowMemSize));
  MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                "highMemBase: 0x%x\nhighMemSize: 0x%x\nTOLM: 0x%x\nTOHM: 0x%x\n", host->var.mem.highMemBase
                , host->var.mem.highMemSize, tolm_5_0.Bits.addr, (((UINT32)tohm_1_5_0.Bits.addr << 6) | tohm_0_5_0.Bits.addr)));

  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (nvramMem->socket[sckt].enabled == 0) continue;
    for(stack=0; stack < MAX_IIO_STACK; stack++) {
      if (host->var.common.stackPresentBitmap[sckt] & (BIT0 << stack)) {
        WriteCpuPciCfgEx (host, sckt, stack, TOLM_IIO_VTD_REG, tolm_5_0.Data);
        WriteCpuPciCfgEx (host, sckt, stack, TOHM_0_IIO_VTD_REG, tohm_0_5_0.Data);
        WriteCpuPciCfgEx (host, sckt, stack, TOHM_1_IIO_VTD_REG, tohm_1_5_0.Data);
      }
    } // stack loop
  } // socket loop
  return;
}



void
WriteWorkAroundCSRs(PSYSHOST host, UINT8 sckt)
/**

  Program CSRs that are suggested as workarounds.

  @param host  - Pointer to sysHost

  @retval N/A

**/
{
  struct channelNvram                 (*channelNvList)[MAX_CH];
  SCRATCHPAD_2LMCNTL_MC_2LM_STRUCT    ScratchPad_2lmCtrl;
  UINT8                               ch;

  channelNvList = &host->nvram.mem.socket[sckt].channelList;

  for (ch = 0; ch < MAX_CH; ch++) {
    if ((*channelNvList)[ch].enabled == 0) continue;
    //Read the 2lm ctrl register for each channel
    ScratchPad_2lmCtrl.Data = MemReadPciCfgEp(host, sckt, ch, SCRATCHPAD_2LMCNTL_MC_2LM_REG);
    //If we are in 1lm mode set the bit 0 in the bitfields else clear it.
    if(host->var.mem.volMemMode == VOL_MEM_MODE_1LM)
      ScratchPad_2lmCtrl.Bits.bit_field |= BIT0;
    else
      ScratchPad_2lmCtrl.Bits.bit_field &= ~BIT0;

    MemWritePciCfgEp (host, sckt, ch, SCRATCHPAD_2LMCNTL_MC_2LM_REG, ScratchPad_2lmCtrl.Data);
    MemDebugPrint ((host, SDBG_MAX, sckt, ch, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"CSR : SCRATCHPAD_2LMCNTL_MC_2LM_REG: %x\n", ScratchPad_2lmCtrl.Data));
  }
}

void
WriteMemoryMapCSRs(PSYSHOST host)
/**

  Fill in SAD/TAD/RIR CRSs for memory map from config info in host structure

  @param host  - Pointer to sysHost

  @retval N/A

**/
{
  UINT8 sckt;
  struct memNvram *nvramMem;

  nvramMem = &host->nvram.mem;

  // Write the SAD data from structures to CSRs
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (nvramMem->socket[sckt].enabled == 0) continue;
    WriteRouteTableCSRs (host, sckt);
    OutputExtendedCheckpoint((host, STS_DDR_MEMMAP, SUB_WRITE_SAD, sckt));
    WriteSADCSRs (host, sckt);
    WriteMesh2MemCSRs (host, sckt);
    OutputExtendedCheckpoint((host, STS_DDR_MEMMAP, SUB_WRITE_TAD, sckt));
    WriteTADCSRs (host, sckt);
    OutputExtendedCheckpoint((host, STS_DDR_MEMMAP, SUB_WRITE_RIR, sckt));
    WriteRIRCSRs (host, sckt);
    WriteCHACSRs (host, sckt);
    WriteWorkAroundCSRs (host, sckt);
  }

  WriteTolmTohmCSRs (host);

  return;
}

UINT8
InitStructForNewRegion(PSYSHOST host, UINT8 memType)
/**

  Init memory size variables based on the memtype

  @param host  - Pointer to sysHost
  @param memType  - - Type of memory region to be mapped

  @retval N/A

**/
{
  UINT8 sckt, ch;
  struct channelNvram (*channelNvList)[MAX_CH];
  struct ddrChannel (*channelList)[MAX_CH];
  struct memVar *mem;

  mem = &host->var.mem;

  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {

    if (host->nvram.mem.socket[sckt].enabled == 0) continue;

    channelList = &mem->socket[sckt].channelList;
    channelNvList = GetChannelNvList(host, sckt);

    // For each channel
    for (ch = 0; ch < MAX_CH; ch++) {
      if ((*channelNvList)[ch].enabled == 0) continue;
      if(memType == MEM_TYPE_BLK_WINDOW){
        (*channelList)[ch].remSize = (*channelList)[ch].blkWinSize;
      }
      else {
        (*channelList)[ch].remSize = (*channelList)[ch].ctrlSize;
      }
    }
  }
  return SUCCESS;
}


UINT32
CreateNewRegionSAD (
  PSYSHOST        host, 
  MEMORY_MAP_DATA *MemMapData, 
  UINT16          TOLM, 
  UINT8           memType
)
/**

  Create SADs for Block or NVMCTLR CSR/Mailbox/Ctrl region

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  
  @param memType  - Type of memory region to be mapped
  @param memType  - Type of memory - 1LM or 2LM
  
  @retval status

**/
{
  UINT8 socketNum, ch;
  UINT32 status = SUCCESS;
  //Call the funtion that would fill ethe remsize of all the channels with the size of Block window or NVMDIMM dimm mailbox/ctrl/csr region for that channel
  InitStructForNewRegion(host, memType);

  //Call SAD interleave routine to create SADs
  status = SADInterleave(host, MemMapData, TOLM, memType);
  if(status != SUCCESS) return status;

  //Reload the Channel size in the structures
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++){
  for (ch=0; ch < MAX_CH; ch++)
      host->var.mem.socket[socketNum].channelList[ch].remSize = host->var.mem.socket[socketNum].channelList[ch].memSize;
  }

 return SUCCESS;
}

//HSD-4928083:Unused SAD Rules should have limit set
void SetLimitForUnusedSADRules(PSYSHOST host)
/**
  Adjust the Limit of the unused SAD rules to the limit of the
  last used SAD rule.

  @param host - Pointer to sysHost
  @retval N/A

**/
{
  UINT8 socketNum, sadIndex;
  struct Socket  *socket;

  //Parse through each socket to set unused SAD rules' limit
  // to the limit of the last used SAD rule
  for(socketNum=0; socketNum<MAX_SOCKET; socketNum++){
    socket = &host->var.mem.socket[socketNum];
    for(sadIndex=0; sadIndex <MAX_SAD_RULES; sadIndex++){
      if(socket->SAD[sadIndex].Enable == 0){
        if (sadIndex > 0)
          socket->SAD[sadIndex].Limit = socket->SAD[sadIndex - 1].Limit;
      }
    }
  }
  return;
}


#ifdef   SERIAL_DBG_MSG
void DisplayRIRMapInfo(PSYSHOST host, UINT8 sckt, UINT8 ch)
/**

  Display RIR Info

  @param host  - Pointer to sysHost
  @param socket  - Socket number
  @param ch    - Channel number (0-based)

  @retval N/A

**/
{
  UINT8 rir, i;
  struct ddrChannel *channel;

  channel = &host->var.mem.socket[sckt].channelList[ch];

  // Display Interleave map
  // Display RIR table

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n----------- RIR Info ----------------\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n RIR Table (Socket %d, Channel %d)\n", sckt, ch));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"----------------------------------------------\n"));

  for (rir = 0; rir < MAX_RIR; rir++) {
    if (channel->rirValid[rir] == 0) continue;

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n Rule \tEnable \tLimit(Ch Space) \tWays"));

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n %d\t%d\t0x%x", rir, channel->rirValid[rir], channel->rirLimit[rir]));
    if (channel->rirValid[rir] == 1) {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\t\t%d", channel->rirWay[rir]));
      // Print rank interleave targets and offsets
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n\tRank Interleave List\n\tWay \tTarget \tOffset"));
      for (i = 0; i < MAX_RIR_WAYS; i++) {
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\t%d\t%d\t%d", i, channel->rirIntList[rir][i], channel->rirOffset[rir][i]));
      } // rank way loop
    }
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
  } // rir loop
  return;
}

void DisplayChannelInfo(PSYSHOST host, UINT8 sckt)
/**

  Display Channel Info

  @param host    - Pointer to sysHost
  @param sckt  - Socket number

  @retval N/A

**/
{

  UINT8 ch;
  struct Socket *socket;
  struct ddrChannel *channel;

  socket = &host->var.mem.socket[sckt];

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n----------- Channel Info ----------------"));

  for (ch = 0; ch < MAX_CH; ch++) {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n----------- Channel %d\n", ch));

    if (host->nvram.mem.socket[sckt].channelList[ch].enabled == 1) {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Channel Enabled\n"));
    } else {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Channel not enabled\n"));
      continue;
    }

    channel = &socket->channelList[ch];

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Channel mem size (64MB) = 0x%x\n", channel->memSize));

    DisplayRIRMapInfo (host, sckt, ch);
  } // ch
}

void DisplayTADMapInfo(PSYSHOST host, UINT8 sckt)
/**

  Display TAD Info

  @param host    - Pointer to sysHost
  @param socket  - Socket number

  @retval N/A

**/
{
  UINT8 i,j;
  UINT8 mc;
  struct Socket *socket;
  struct TADTable *TADEntry;
  struct IMC *imc;

  socket = &host->var.mem.socket[sckt];

  // Display Interleave map

  // Display TAD table

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n----------- TAD Info ----------------\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n TAD Table (Socket %d)\n", sckt));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Rule \tEnable \tLimit \tMode \tCh ways\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"---------------------------------------"));

  for (mc = 0; mc <  host->var.mem.maxIMC; mc++) {
    if (host->nvram.mem.socket[sckt].imc[mc].enabled == 0) continue;

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n Home Agent %d\n", mc));

    imc = &socket->imc[mc];

    for (i = 0; i < TAD_RULES; i++) {
      TADEntry = &imc->TAD[i];
      if (TADEntry->Enable == 0) continue;

      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n %d\t%d\t0x%x", i, TADEntry->Enable, TADEntry->Limit));
      if (TADEntry->Enable == 1) {
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t%d\t%d\n", TADEntry->mode, TADEntry->channelWays));
        // Print TAD interleave targets and offsets
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tTAD Interleave List\n\tWay \tTarget \tOffset \tChIndex"));
        for (j = 0; j < MAX_TAD_WAYS; j++) {
          MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\t%d\t%d\t0x%x\t%d", j, imc->TADintList[i][j], imc->TADOffset[i][j], imc->TADChnIndex[i][j]));
        } // TAD way loop
        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
      }
    } // TAD loop
  } // mc loop
  return;
}

void DisplaySADMapInfo(PSYSHOST host, UINT8 sckt)
/**

  Display SAD Info

  @param host  - Pointer to sysHost
  @param sckt  - Socket number

  @retval N/A

**/
{
  UINT8 sad ;
  struct memVar *mem;
  struct SADTable *SADEntry;
  UINT8 maxSADRules = MAX_SAD_RULES;
  EncodedInterleaveGranularity encodedInterleaveGranularity ;

  // Display Interleave map

  mem = &host->var.mem;

  // Display SAD table
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nCHA SAD (Skt %d)\n", sckt ));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Rule\tEnable\tLimit\tIntMode\tMd3\tMd3AsMd2\tMd3Mode\tInterleave\n" ));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"--------------------------------------------------------------------------"));

  for (sad = 0; sad < maxSADRules; sad++) {

    SADEntry = &mem->socket[sckt].SAD[sad];

    if (SADEntry->Enable == 0) continue;

    encodedInterleaveGranularity = SADEncodeInterleaveGranularity( host, SADEntry->type, SADEntry->granularity ) ;

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n %d\t%d\t0x%05x", sad, SADEntry->Enable, SADEntry->Limit )) ;
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT, "\t%d\t%d\t%d\t%d\t%d\t\t%d", encodedInterleaveGranularity.chaTgt, IsPowerOfTwo(SADEntry->channelInterBitmap[IMC0]), SADEncodeMod3AsMod2(SADEntry->channelInterBitmap[IMC1]), IsPowerOfTwo(SADEntry->channelInterBitmap[IMC0]), SADEncodeMod3AsMod2(SADEntry->channelInterBitmap[IMC1]), encodedInterleaveGranularity.chaCh));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t0x%08x", SADEncodeInterleaveList( host, sckt, sad,NMEM ) ));
  }
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n" ));
  return;
}

void DisplaySAD2TADInfo(PSYSHOST host, UINT8 sckt)
/**

  Display SAD2TAD Info

  @param host  - Pointer to sysHost
  @param sckt  - Socket number

  @retval N/A

**/
{
  UINT8 ddr4tad, mc, sad;
  struct memVar *mem;
  struct SADTable *SADEntry;

  // Display Interleave map
  mem = &host->var.mem;

  // Display SAD2TAD tables


  for( mc = 0 ; mc < host->var.mem.maxIMC ; mc++ ) {

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nM2M SAD2TAD (Skt %d MC %d)\n", sckt, mc));
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Rule\tValid\tDDR4Tad\n" ));
    MemDebugPrint((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"--------------------------------------------------------------------------"));
    for( sad = 0 ; sad < MAX_SAD_RULES ; sad++ ) {

      SADEntry = &mem->socket[sckt].SAD[sad];
      if( SADEntry->Enable == 0 ) break;

      //Skip if this mc is not a part of the interleave
      if(((SADEntry->imcInterBitmap) & (BIT0<<mc)) != 0){

        MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n %d\t%d\t", sad, SADEntry->Enable )) ;

        for( ddr4tad=0 ; ddr4tad < TAD_RULES ; ddr4tad++ )
        {
          if( host->var.mem.socket[sckt].imc[mc].TAD[ddr4tad].SADId == sad ) {
            MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%d\t", ddr4tad )) ;
            break ;
          }
        }
      }
    }
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n" )) ;
  }
  return;
}

void DisplayNodeInfo(PSYSHOST host)
/**

  Display Node Info

  @param host  - Pointer to sysHost

  @retval N/A

**/
{
  UINT8 sckt;
  struct memVar *mem;
  struct Socket *socket;

  mem = &host->var.mem;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\n----------- Socket Info ----------------"));

  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n\nSocket %d\n", sckt));
    socket = &mem->socket[sckt];

    if (host->nvram.mem.socket[sckt].enabled == 1) {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Socket enabled\n"));
    } else {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Socket not enabled\n"));
      continue;
    }

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Socket max DIMM pop count = %d\n", host->nvram.mem.socket[sckt].maxDimmPop));

    if (host->nvram.mem.socket[sckt].maxDimmPop == 0)
      continue;

    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Socket mem size (64MB) = 0x%x\n", socket->memSize));

    DisplaySADMapInfo (host, sckt);

    DisplaySAD2TADInfo (host, sckt);

    DisplayTADMapInfo (host, sckt);

    DisplayChannelInfo (host, sckt);
  }
}

void DisplaySystemInfo(PSYSHOST host)
/**

  Display Syste, Info

  @param host  - Pointer to sysHost

  @retval N/A

**/
{
  struct memVar *mem;
  struct memNvram *nvMem;
  UINT8 i ;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tDisplay Memory Map Parameters\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"));

  mem = &host->var.mem;
  nvMem = &host->nvram.mem;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," options = %d\n", host->setup.mem.options ));
  if (host->setup.mem.options & NUMA_AWARE)
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," NUMA Config\n"));
  else
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Non-NUMA Config\n"));

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," volMemMode = %d\n", host->setup.mem.volMemMode )) ;
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," dfxPerMemMode = %d\n", host->setup.mem.dfxMemSetup.dfxPerMemMode )) ;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," memInterleaveGran1LM = %d\n", host->setup.mem.memInterleaveGran1LM )) ;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," socketInter = %d\n", host->setup.mem.socketInter));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," imcInter = %d\n", host->setup.mem.imcInter ));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," chInter = %d\n", host->setup.mem.chInter));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," rankInter = %d\n", host->setup.mem.rankInter));

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," dfxCfgMask2LM = %d\n", host->setup.mem.dfxMemSetup.dfxCfgMask2LM));

  if( host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN ) {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\tHigh Address Enable\n" ));
  }
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," dfxHighAddrBitStart = %d\n", host->setup.mem.dfxMemSetup.dfxHighAddrBitStart));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," dfxLowMemChannel = %d\n", host->setup.mem.dfxMemSetup.dfxLowMemChannel));

  //
  //**Memmap related options for SV
  //
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," dfxDimmManagement = %02X\n", host->setup.mem.dfxMemSetup.dfxDimmManagement )) ;
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," dfxPartitionDDRTDimm = %02X\n", host->setup.mem.dfxMemSetup.dfxPartitionDDRTDimm )) ;
  for( i = 0 ; i < ( MAX_SOCKET * MAX_IMC ) ; i++ )  {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," dfxPartitionRatio[%02X] = %02X\n", i, host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] )) ;
  }

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," Socket RAS Config = "));

  if (nvMem->RASmode & CH_MIRROR)   MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Channel Mirror  "));
  if (nvMem->RASmode & STAT_VIRT_LOCKSTEP) MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Static Virtual Lockstep"));
  if (!(nvMem->RASmode & CH_ML))    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Channel Independent"));
  if (nvMem->RASmode & RK_SPARE)    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Rank Sparing"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));




  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT," System Mem Size (64MB granularity): 0x%x\n", mem->memSize));

  return;
}

void DisplayMemoryMapInfo(PSYSHOST host)
/**

  Display Memory map Info

  @param host  - Pointer to sysHost

  @retval N/A

**/
{

  // Display system information
  DisplaySystemInfo (host);

  // Display node info (SAD, M2M, TAD, and RIR tables)
  DisplayNodeInfo (host);

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n"));
}

void PrintSize(PSYSHOST host, UINT16 size)
/**

  Print the provided size in MB or GB as appropriate

  @param host  - Pointer to sysHost
  @param size  - Memory size in 64MB units

  @retval N/A

**/
{
  UINT16 tmp = 0;

  tmp = (size & 0xF) * 64;
  size = size & 0xFFF0;
  if (size) {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%3d",size >> 4));
    if (tmp) {
      tmp = (1000*tmp)/1024;
      while (!(tmp%10)) tmp = tmp/10;
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,".%iGB",tmp));
    } else MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"GB "));
  } else if (tmp) {
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"%3iMB ",tmp));
  }

  if (!(size || tmp)) MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"   0  "));
}
#endif // SERIAL_DBG_MSG

void
DisplayMemoryMap (PSYSHOST host)
/**

  This routine provides a hook to add routines to display memory map created.

  @param host  - Pointer to sysHost

  @retval N/A

**/
{

  DisplaySADTable(host);
  DisplayTADTable(host);
}

UINT32
UpdateMemMapState (PSYSHOST host)
/**

  This routine Updates the memory mapping state and returns proper status.

  @param host  - Pointer to sysHost

  @retval status

**/
{

  //if(host->var.mem.ResourceCalculationDone)
  if(host->var.mem.MemMapState == MEM_MAP_STATE_RESOURCE_CALCULATION_DONE)
    return SUCCESS;
  else {
    //If we are here it means, that we have completed initial run of memmap and found that we have all silicon resources
    //So set the resourceCalcDone variable and rerun the memory mapping algorithm.
    //host->var.mem.ResourceCalculationDone = 1;
    host->var.mem.MemMapState = MEM_MAP_STATE_RESOURCE_CALCULATION_DONE;

    //Clear Memory map structures.
    ClearMemMapStructs(host);

    return ERROR_RESOURCE_CALCULATION_COMPLETED;
  }
}

VOID
FinalMemoryMapOemHook(void)
/**
  EMPTY Oem hook
  @param void

  @retval none
**/
{

}

UINT32
FinalizeMemoryMap (PSYSHOST host)
/**

  This routine provides a hook to perform final adjustments to the internal strructure related to memory map.
  One important function that happens here is, that this is where we set the resourceCalculationFlagDone flag and rerun the memap algorithm.

  @param host  - Pointer to sysHost

  @retval status

**/
{
  UINT32 status = SUCCESS;

  FinalMemoryMapOemHook();
  //HSD-4928083:Unused SAD Rules should have limit set
  SetLimitForUnusedSADRules(host);


  //Update the memory mapping state. If we are here for the first time,
  //set the variable and return an error, so that the memmap will be rerun.
  status = UpdateMemMapState(host);
  if(status != SUCCESS) return status;


  //If we are in degraded memorymap level, update proper response to the requests from CR driver since they are ignored.
  if(host->var.mem.MemMapDegradeLevel) {
    status = UpdateRequestforDegradedMemoryMapping(host);
    if(status != SUCCESS) return status;
  }

  //Update CCUR structs with proper current status.
  UpdateCfgCurAfterMemmap(host);

  //Update CFGOUT structs with proper validation status.
  UpdateCfgOutAfterMemmap(host);

  return SUCCESS;
}

UINT32
MemoryMapRASHook (PSYSHOST host)
/**

  This routine provides a hook for RAS module to patch memory map before being written to CSRs.

  @param host  - Pointer to sysHost

  @retval status

**/
{
  UINT32 Status = SUCCESS;

  Status = AdjustMemAddrMapForMirror(host);
  return Status;
}

UINT32
CreateTADRules (PSYSHOST host)
/**

  This routine Creates TAD rules in each IMC for the memory to be mapped into the system address space.

  @param host  - Pointer to sysHost

  @retval status

**/
{

  UINT16  TOLM;
  UINT32  status = SUCCESS;
  // Top of Low Memory equals 4GB - size of the low Gap
  // TOLM in 64MB units
  //
  TOLM = (UINT16)(FOUR_GB_MEM - host->setup.common.lowGap);
  // Has to be on a 256MB boundary
  TOLM = TOLM & ~0x3;
  OutputExtendedCheckpoint((host, STS_DDR_MEMMAP, SUB_TAD_NONINTER, 0));
  status = TADInterleave (host, TOLM);
  if(status != SUCCESS) return status;
  return SUCCESS;
}

UINT32
CreateRIRRules (PSYSHOST host)
/**

  This routine Creates RIRs in each channel for the memory to be mapped into the system address space.

  @param host  - Pointer to sysHost

  @retval status

**/
{
  UINT32 status = SUCCESS;
  // Do rank interleaving
  OutputExtendedCheckpoint((host, STS_DDR_MEMMAP, SUB_RANK_INTER, 0));
  status = RankInterleave(host);
  if(status != SUCCESS) return status;
  return SUCCESS;
}

UINT32
CreateSADRules (
  PSYSHOST        host, 
  MEMORY_MAP_DATA *MemMapData
)
/**

  This routine Creates SAD rules for the memory to be mapped into the system address space.

  @param host  - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)  

  @retval status

**/
{
#if MAX_SOCKET > 1
  UINT8   numSockets, sckt;
#endif
  UINT8 socketBitMap = 0;
  UINT32 status = SUCCESS;

  UINT16  TOLM;

  // TOLM in 64MB units
  //
  TOLM = (UINT16)(FOUR_GB_MEM - host->setup.common.lowGap);
  // Has to be on a 256MB boundary
  TOLM = TOLM & ~0x3;

#if MAX_SOCKET > 1
  numSockets = 0;
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0)
      continue;

    if (host->var.mem.socket[sckt].memSize != 0) {
      numSockets += 1;
    }
  }
  //Map the volatile memory present in the system (SAD). (Also, PMEM/PMEM$ if no CR mgmt driver present - SV only)
  if ((!(host->setup.mem.options & NUMA_AWARE) || (host->var.mem.socketInter > 1)) && (numSockets > 1)) {
    status = SocketInterleave(host, MemMapData, TOLM) ;
    if(status != SUCCESS) return status;
  } else
#endif
  {
    status = SocketNonInterleave(host, MemMapData, TOLM) ;
    if(status != SUCCESS) return status;
  }
  if(host->var.mem.dfxMemVars.dfxDimmManagement == DIMM_MGMT_CR_MGMT_DRIVER) {
    //Handle CCUR records
    status = ApplyCCURRecords(host);
    if(status != SUCCESS) return status;

    if(!host->var.mem.MemMapDegradeLevel) {
      //Handle CFGIn interleave requests
      status = ApplyInterleaveRequests(host);
      if(status != SUCCESS) return status;
    }
  }
  //Set the bit map to represent all the sockets.
  socketBitMap = 0xFF;

  // Create SAD entries for Block Window for all NVMDIMM dimms present in the system

  if(IsDDRTPresent(host, socketBitMap)){
    status = CreateNewRegionSAD(host, MemMapData, TOLM, MEM_TYPE_BLK_WINDOW);
    if(status != SUCCESS) return status;
  }


  // Create SAD entries for NVMCTLR CSR/CTRL/MAILBOX for all NVMDIMM dimms present in the system
  if(IsDDRTPresent(host, socketBitMap)){
    status = CreateNewRegionSAD(host, MemMapData, TOLM, MEM_TYPE_CTRL);
    if(status != SUCCESS) return status;
  }

  return SUCCESS;
}

UINT32
AdjustVolatileMemMode (PSYSHOST host)
/**

  This routine checks if the system configuration has capability to run in 2LM mode.
  If not, it sets the volMemMode variable to 1lm and returns an error.
  This error caus the system to restart the memory mapping code in 1LM mode.

  @param host  - Pointer to sysHost

  @retval SUCCESS - If 2lm mode can be supported
  @retval FAILURE - If 2lm mode cannot be supported and we need to restart the memory mapping in 1lm mode.

**/
{
  UINT8 sckt;
  UINT8 mc;
  struct memVar *mem;
  struct IMC *imc;

  mem = &host->var.mem;

  // For each socket
  for (sckt = 0; sckt < MAX_SOCKET; sckt++) {
    if (host->nvram.mem.socket[sckt].enabled == 0) continue;
    //For each IMC in the socket
    for (mc = 0; mc <  host->var.mem.maxIMC; mc++) {
      imc = &mem->socket[sckt].imc[mc];

      //if this IMC has DDR4 memory and one NVMDIMM dimm with volatile memory, then 2LM is possible in this system configuration
      if( imc->memSize && imc->volSize )
        return SUCCESS;
      else
        continue;
    }
  }

  //If we are here, it means we havent found a single MC in the system, that has a DDR4 and an NVMDIMM dimm with some volatile memory populate in the system.
  //This configuration cannot support 2LM memory mode, so we will have to reinit the volMemmode to 1LM and restart memory mapping.
  host->var.mem.volMemMode = VOL_MEM_MODE_1LM;
  MemDebugPrint ((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"***ERROR: 2LM could not be supported in this memory configuration. So Initializing VolMemMode to 1LM.***\n Restarting Memory mapping again...\n\n"));

  //If we are falling back to 1LM based on memory config, we  have to change the permemmode type which is dependant on volmemmode.
  if(host->setup.mem.dfxMemSetup.dfxPerMemMode == PER_MEM_REGION) {
    mem->dfxMemVars.dfxPerMemMode = PER_MEM_MODE ;
  }  else  {
    mem->dfxMemVars.dfxPerMemMode = NON_PER_MEM_MODE ;
  }

  return FAILURE;
}

VOID
InitMemMapParams (PSYSHOST host)
/**

  This routine initializes memory mapping parameters based on setup options and system capability.

  @param host  - Pointer to sysHost

  @retval VOID

**/
{
  UINT8   sckt;
  CAPID1_PCU_FUN3_STRUCT    pcuCAPID1;
  UINT64_STRUCT mmiohBase, temp;
   //Check if memory address map setup options values are valid
  CheckMemAddrMapSetupOptionsValues(host);

  //Initialize MMIOH base from setup option and store it in 64MB granularity.
  mmiohBase.hi = host->setup.common.mmiohBase;
  mmiohBase.lo = 0;
  temp =  RShiftUINT64(mmiohBase, CONVERT_B_TO_64MB);
  host->var.mem.mmiohBase =  temp.lo;
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"mmiohbasefrom setup: %x MMIOH base = %x (64mb)\n", host->setup.common.mmiohBase, host->var.mem.mmiohBase));

  //Adjust the memory modes if the the silicon has reduced capability
  for(sckt = 0; sckt < MAX_SOCKET; sckt++){
    if (host->nvram.mem.socket[sckt].enabled == 0)
      continue;
    pcuCAPID1.Data = host->var.common.procCom[sckt].capid1;
    if (pcuCAPID1.Bits.disable_2lm == 1) {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"SKX Capability does not support 2LM, forcing to 1LM.\n"));
      host->var.mem.volMemMode = VOL_MEM_MODE_1LM;
    }
    if ((pcuCAPID1.Bits.disable_cr_pmem == 1) || (pcuCAPID1.Bits.disable_cr_pmem_wt == 1)) {
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"SKX Capability does not support persistent modes, forcing to non-persistent mode.\n"));
      host->var.mem.dfxMemVars.dfxPerMemMode = NON_PER_MEM_MODE;
    }
  }

  //Init TOHM
  // maxAddrMem and highGap are in 256MB units. tohmLimit is 64MB granularity.
  host->var.common.tohmLimit = (host->setup.common.maxAddrMem - host->setup.common.highGap) * 4;

  // Extends the MMIO hole to force memory to high address region
  // Note: Tests must not generate addresses in the hole
  if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN) {
    // Adjust for 64MB (26 bits) alignment. Start 64MB below address indicated by bit start
    host->var.mem.highMemBase = BIT0 << (host->var.mem.dfxMemVars.dfxHighAddrBitStart-CONVERT_B_TO_64MB);
  } else {
    host->var.mem.highMemBase = FOUR_GB_MEM;
  }
}

VOID
CheckMemAddrMapSetupOptionsValues(PSYSHOST host)
 /**

  This routine checks the values of the Memory address mapping setup options.

  @param host  - Pointer to sysHost

  @retval N/A

**/
{
  UINT8 i,j;
  UINT32 socketIndex = 0;

 //Set the Interleave parameters based on the setup options
  if ( host->setup.mem.chInter == CH_1WAY || host->setup.mem.chInter == CH_2WAY || host->setup.mem.chInter == CH_3WAY)
    host->var.mem.chInter = host->setup.mem.chInter;

  else {
  MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\nInvalid option value for Channel Interleave!!!\n"));
    host->var.mem.chInter = CH_INTER_DEFAULT;
  }

  if ( host->setup.mem.rankInter == RANK_1WAY || host->setup.mem.rankInter == RANK_2WAY ||
       host->setup.mem.rankInter == RANK_4WAY || host->setup.mem.rankInter == RANK_8WAY)
    host->var.mem.rankInter = host->setup.mem.rankInter;

  else {
  MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
       "\nWARNING!!! Invalid option value for Rank Interleave.\n"));
    host->var.mem.rankInter = RANK_INTER_DEFAULT;
  }

  if ( host->setup.mem.socketInter == SOCKET_1WAY || host->setup.mem.socketInter == SOCKET_2WAY || host->setup.mem.socketInter == SOCKET_4WAY )
    host->var.mem.socketInter = host->setup.mem.socketInter;

  else {
  MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\nWARNING!!! Invalid option value for Socket Interleave.\n"));
    host->var.mem.socketInter = SOCKET_INTER_AUTO;
  }

  if ( host->setup.mem.imcInter == IMC_1WAY || host->setup.mem.imcInter == IMC_2WAY )
    host->var.mem.imcInter = host->setup.mem.imcInter;

  else {
  MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\nWARNING!!! Invalid option value for IMC Interleave.\n"));
    host->var.mem.imcInter = IMC_INTER_DEFAULT;
  }

  if (host->setup.mem.dfxMemSetup.dfxOptions & HIGH_ADDR_EN)  {
    if ( (host->setup.mem.dfxMemSetup.dfxHighAddrBitStart >= HIGH_ADDR_START_BIT_POSITION_33) &&
         (host->setup.mem.dfxMemSetup.dfxHighAddrBitStart <= HIGH_ADDR_START_BIT_POSITION_45))
      host->var.mem.dfxMemVars.dfxHighAddrBitStart = host->setup.mem.dfxMemSetup.dfxHighAddrBitStart;

    else {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\nWARNING!!! Invalid option value for HighAddressStartBitPosition.\n"));
      host->setup.mem.dfxMemSetup.dfxOptions &= ~HIGH_ADDR_EN;
      host->var.mem.dfxMemVars.dfxHighAddrBitStart = 0;
    }
  }

  if ( host->setup.mem.dfxMemSetup.dfxDimmManagement == DIMM_MGMT_BIOS_SETUP || host->setup.mem.dfxMemSetup.dfxDimmManagement == DIMM_MGMT_CR_MGMT_DRIVER )
    host->var.mem.dfxMemVars.dfxDimmManagement = host->setup.mem.dfxMemSetup.dfxDimmManagement;

  else {
  MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\nWARNING!!! Invalid option value for DIMM Management.\n"));
    host->var.mem.dfxMemVars.dfxDimmManagement = DIMM_MGMT_AUTO;
  }

    if(host->setup.mem.setSecureEraseAllDIMMs == DISABLE_SECURE_ERASE || host->setup.mem.setSecureEraseAllDIMMs == ENABLE_SECURE_ERASE) {
    host->var.mem.setSecureEraseAllDIMMs = host->setup.mem.setSecureEraseAllDIMMs;
  } else {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\nWARNING!!! Invalid option value for Erase All DIMMs.\n"));
    host->var.mem.setSecureEraseAllDIMMs =  DISABLE_SECURE_ERASE;
  }

  for(i = 0; i < MAX_SOCKET; i++) {
    for(j = 0; j < MAX_CH; j++) {
        if(host->setup.mem.setSecureEraseSktCh[i][j] == DISABLE_SECURE_ERASE || host->setup.mem.setSecureEraseSktCh[i][j] == ENABLE_SECURE_ERASE)
           host->var.mem.setSecureEraseSktCh[i][j] = host->setup.mem.setSecureEraseSktCh[i][j];
        else {
           MemDebugPrint((host, SDBG_MINMAX, i, NO_CH, j, NO_RANK, NO_STROBE, NO_BIT,
         "\nWARNING!!! Invalid option value for secure erase of S%d CH%d.\n", i, j));
           host->var.mem.setSecureEraseSktCh[i][j] = DISABLE_SECURE_ERASE;
        }
    }
  }

  //Set various memory modes based on the setup option selected.
  if (host->var.mem.volMemMode == MEM_MAP_VOL_MEM_MODE_UNDEFINED){
    if(host->setup.mem.volMemMode == VOL_MEM_MODE_AUTO){
      //Volmemmode is set to Auto, we set it to 2LM and review if 2lm is possible in this memory configuration a little later in memory mapping.
      //If 2lm is not possible, we reinitialize it to 1lm and restart the memory mapping again.
      host->var.mem.volMemMode = VOL_MEM_MODE_2LM;
    } else if (host->setup.mem.volMemMode == VOL_MEM_MODE_1LM || host->setup.mem.volMemMode == VOL_MEM_MODE_2LM)
      host->var.mem.volMemMode = host->setup.mem.volMemMode;

    else {
      MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
              "\nWARNING!!! Invalid option value for Volatile Memory Mode.\n"));
      host->var.mem.volMemMode = VOL_MEM_MODE_AUTO;
    }
  }

  //If Persistent memory mode is enabled in setup, set the persistent memmode to PMEM or PMEM$ based on volatile memory mode.
  if( ( host->setup.mem.dfxMemSetup.dfxPerMemMode == PER_MEM_MODE ) || ( host->setup.mem.dfxMemSetup.dfxPerMemMode == PER_MEM_CACHE_MODE ) || ( host->setup.mem.dfxMemSetup.dfxPerMemMode == NON_PER_MEM_MODE ) )  {
    host->var.mem.dfxMemVars.dfxPerMemMode = host->setup.mem.dfxMemSetup.dfxPerMemMode ;
  }
  else {
    MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
      "\nWARNING!!! Invalid option value for Persistent Memory Mode.\n"));
    host->var.mem.dfxMemVars.dfxPerMemMode = NON_PER_MEM_MODE;
  }

  for(i = 0; i < MAX_SOCKET * MAX_IMC; i++) {
    if (host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_20_80_0 || host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_80_20_0 ||
        host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_50_50_0 || host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_60_0_40 ||
        host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_40_10_50 || host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_0_40_60 ||
        host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_0_30_70 || host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_100_0_0 ||
        host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_0_100_0 || host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_0_0_100 ||
        host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_30_60_10 || host->setup.mem.dfxMemSetup.dfxPartitionRatio[i] == RATIO_70_0_30)

       host->var.mem.dfxMemVars.dfxPartitionRatio[i] = host->setup.mem.dfxMemSetup.dfxPartitionRatio[i];
    else {
        socketIndex = i / 2;
        MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
           "\nWARNING!!! Invalid option value for S%i MC%i Partition Ratio.\n", socketIndex, i));
       host->var.mem.dfxMemVars.dfxPartitionRatio[i] = RATIO_40_10_50;
    }

  }


  if (host->setup.mem.memInterleaveGran1LM == MEM_INT_GRAN_1LM_256B_256B ||
      host->setup.mem.memInterleaveGran1LM == MEM_INT_GRAN_1LM_64B_64B)

    host->var.mem.memInterleaveGran1LM = host->setup.mem.memInterleaveGran1LM;
  else {
     MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\nWARNING!!! Invalid option value for 1LM Memory Interleave Granularity.\n"));
    host->var.mem.memInterleaveGran1LM = MEM_INT_GRAN_1LM_DEFAULT;
  }

  // Set 2LM interleave granularity to validation POR
  host->var.mem.memInterleaveGran2LM = MEM_INT_GRAN_2LM_NM_4KB_256B_FM_4KB;

  // Set PMem interleave granularity to validation POR
  host->var.mem.dfxMemVars.dfxMemInterleaveGranPMemNUMA = MEM_INT_GRAN_PMEM_NUMA_4KB_4KB;

  if ( host->setup.mem.dfxMemSetup.dfxPartitionDDRTDimm == PARTITION_DDRT_DIMM_EN ||
       host->setup.mem.dfxMemSetup.dfxPartitionDDRTDimm == PARTITION_DDRT_DIMM_DIS) {
    host->var.mem.dfxMemVars.dfxPartitionDDRTDimm = host->setup.mem.dfxMemSetup.dfxPartitionDDRTDimm;
  } else {
     MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
        "\nWARNING!!! Invalid option value for NVMDIMM DIMM partitioning.\n"));
    host->var.mem.dfxMemVars.dfxPartitionDDRTDimm = PARTITION_DDRT_DIMM_DIS;
  }

  if( host->setup.mem.dfxMemSetup.dfxCfgMask2LM == CFG_MASK_2LM_NORMAL || host->setup.mem.dfxMemSetup.dfxCfgMask2LM == CFG_MASK_2LM_AGGRESSIVE)
    host->var.mem.dfxMemVars.dfxCfgMask2LM = host->setup.mem.dfxMemSetup.dfxCfgMask2LM;
  else {
     MemDebugPrint((host, SDBG_MINMAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
       "\nWARNING!!! Invalid option value for Configuration Mask for 2LM.\n"));
    host->var.mem.dfxMemVars.dfxCfgMask2LM = CFG_MASK_2LM_NORMAL;
  }
}

/**
  
  This routine is to get/retrieve SKU limit value per socket in the system from PCU
  @param host        - Pointer to sysHost
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  
  @retval void

**/
VOID InitializeSkuLimits (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData
)
{
  UINT32 MailboxStatus;
  UINT32 MailboxData;
  UINT8  Socket;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    MemMapData->MemMapSkuLimit[Socket].SkuLimit = 0xFFFF;
    MemMapData->MemMapSkuLimit[Socket].MemoryAllocated = 0;
    MemMapData->MemMapSkuLimit[Socket].SkuLimitValid = FALSE;
    MemMapData->MemMapSkuLimit[Socket].Violation = FALSE;

    if (host->nvram.mem.socket[Socket].enabled == 0) {
      continue;
    }

    MailboxStatus = WriteBios2PcuMailboxCommand(host, Socket, MAILBOX_BIOS_CMD_READ_SKU_LIMIT, 0x0);
    if (MailboxStatus == SUCCESS) {  // success
      MailboxData = ReadCpuPciCfgEx (host, Socket, 0, BIOS_MAILBOX_DATA);
      MemMapData->MemMapSkuLimit[Socket].SkuLimit = (MailboxData & 0xFFFF); // LSB 16 bits are valid; 64GB units; 
      MemMapData->MemMapSkuLimit[Socket].SkuLimitValid = TRUE;

      MemDebugPrint((host, SDBG_MAX, Socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                     "SkuLimit value in 64GB units: 0x%x \n", MemMapData->MemMapSkuLimit[Socket].SkuLimit));
    } else {
      MemDebugPrint((host, SDBG_MINMAX, Socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
                       "Error reading SkuLimit from PCU \n"));
    }
  } //Socket loop 
} //InitializeSkuLimits()

/**
  
  This routine is to check whether there is a SKU limit  
  violation per socket basis  
 
  @param host        - Pointer to sysHost  
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  @param MemoryToBeAllocated - size of memory to be allocated 
  @param MemType     - Type of memory  
  @param McList      - List of Memory controllers populated 
  
  @retval BOOLEAN - TRUE (SKU Limit violation) FALSE (No SKU  
         Limit violation)

**/
BOOLEAN IsSkuLimitViolation(
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData,
  UINT32          MemoryToBeAllocated,
  UINT8           MemType,
  UINT8           McList[MAX_SOCKET*MAX_IMC]
) 
{
  UINT32 MemoryToBeAllocatedPerSoc[MAX_SOCKET] = {0};
  UINT32 MemoryToBeAllocatedPerMc;
  UINT8  McWays=0;
  UINT8 Socket;
  UINT8 Mc;
  UINT8 Index;

  if ((MemType == MEM_TYPE_PMEM_CACHE) || (MemType == MEM_TYPE_BLK_WINDOW) || (MemType == MEM_TYPE_CTRL)) {
    return (FALSE);
  }

  // Calculate number of MC populated across all sockets
  for (Index = 0; Index < (MAX_SOCKET*MAX_IMC); Index++) {
    if (McList[Index] == 0x1) {
      McWays++;
    }
  }
  // total memory allocated per MC
  MemoryToBeAllocatedPerMc = MemoryToBeAllocated / McWays;

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (host->nvram.mem.socket[Socket].enabled == 0) {
      continue;
    }

    if (MemMapData->MemMapSkuLimit[Socket].SkuLimitValid == FALSE) {
      continue;
    }

    for (Mc = 0; Mc < MAX_IMC; Mc++) {
      if (host->nvram.mem.socket[Socket].imc[Mc].enabled == 0){
        continue;
      }
      if (McList[(Socket*MAX_IMC) + Mc] == 1) {
        MemoryToBeAllocatedPerSoc[Socket] += MemoryToBeAllocatedPerMc;
      }
    } // Mc loop

    if (MemoryToBeAllocatedPerSoc[Socket]) {
      if ((MemoryToBeAllocatedPerSoc[Socket] + MemMapData->MemMapSkuLimit[Socket].MemoryAllocated) > 
          (MemMapData->MemMapSkuLimit[Socket].SkuLimit * 1024)) { //convert SkuLimit from 64GB to 64MB units
	    MemDebugPrint((host, SDBG_MAX, Socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
				"Memory Allocation Violation occurred: 0x%x \n",(MemoryToBeAllocatedPerSoc[Socket] + 
                                                                 MemMapData->MemMapSkuLimit[Socket].MemoryAllocated)));
        MemMapData->MemMapSkuLimit[Socket].Violation = TRUE;
        if (host->setup.mem.options & NUMA_AWARE) {
          continue;
        } else { // UMA case
          return TRUE;
        }
      }
    } // MemoryToBeAllocatedPerSoc
  } // Socket loop

  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (host->nvram.mem.socket[Socket].enabled == 0) {
      continue;
    }
    MemMapData->MemMapSkuLimit[Socket].MemoryAllocated += MemoryToBeAllocatedPerSoc[Socket];
	MemDebugPrint((host, SDBG_MAX, Socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
				"MemMapData->MemMapSkuLimit[Socket].MemoryAllocated: 0x%x \n", 
                 MemMapData->MemMapSkuLimit[Socket].MemoryAllocated));
  } // Socket loop

  if (host->setup.mem.options & NUMA_AWARE) {
    for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
      if (host->nvram.mem.socket[Socket].enabled == 0) {
        continue;
      }
      if (MemMapData->MemMapSkuLimit[Socket].Violation == TRUE) {
        MemDebugPrint((host, SDBG_MAX, Socket, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,
		    		  "NUMA enabled, SKU violation skipping DRAM rule allocation \n"));
        return TRUE;
      }
    } // Socket loop
  } // NUMA

  return FALSE;
} //IsSkuLimitViolation()

/**
  
  This routine is to initilize value per socket for  
  SKU limit values  
   
  @param host        - Pointer to sysHost  
  @param MemMapData  - Pointer to MemMapData (Private DS maintained by Memory decoder)
  
  @retval void

**/
VOID ResetSkuLimitsViolation (
  PSYSHOST        host,
  MEMORY_MAP_DATA *MemMapData
)
{
  UINT8 Socket;
   
  for (Socket = 0; Socket < MAX_SOCKET; Socket++) {
    if (host->nvram.mem.socket[Socket].enabled == 0) {
      continue;
    }

    MemMapData->MemMapSkuLimit[Socket].MemoryAllocated = 0x0;
    MemMapData->MemMapSkuLimit[Socket].Violation = FALSE;
  } // Socket loop
} //ResetSkuLimitsViolation()

//
//-----------------------------------------------------------------------------
// Initialize memory map
//
// Description: Compute contents for memory map CSRs to allocate physical address space to sockets,
// channels, and dimm ranks. Writes necessary memory map CSRs. Displays memory map
// configuration for debug.
//
//-----------------------------------------------------------------------------

UINT32 InitMemoryMap(PSYSHOST host)
/**

  Compute contents for memory map CSRs to allocate physical address space to sockets,
  channels, and dimm ranks. Writes necessary memory map CSRs. Displays memory map
  configuration for debug.

  @param host  - Pointer to sysHost

  @retval SUCCESS

**/
{
  UINT32 status = SUCCESS;
  MEMORY_MAP_DATA MemMapData;
  //
  // Make sure there is still memory present after running MemTest
  //
  CheckMemPresentStatus (host);

#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  SetMemPhase(host, MEM_MAPPING);
#endif
#endif  //  DEBUG_PERFORMANCE_STATS

  //Clear volatile mem mode.
  host->var.mem.volMemMode = MEM_MAP_VOL_MEM_MODE_UNDEFINED;
  host->var.mem.MemMapState = MEM_MAP_STATE_RESOURCE_CALCULATION;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\n***BEGIN MEMORY MAPPING***\n"));
  // get CPU SKU limit from PCU
  InitializeSkuLimits (host, &MemMapData);

  //Initialize paramaters for memory mapping only for MEM_MAP_STATE_RESOURCE_CALCULATION
  InitMemMapParams (host);
  do {

    // clear the SKU limit values, if there is a recomputation due to not successfull in 1st pass
    ResetSkuLimitsViolation(host, &MemMapData);

    //Skip populating memory sizes again if we are running because of resource unavailablity.
    if(host->var.mem.MemMapState != MEM_MAP_STATE_RESOURCE_CALCULATION_FAILURE){
      // Populate memory size fields
      PopulateMemorySizeFields(host);
    }

    //If volMemmode set to Auto check if 2LM can be supported, If we cannot support 2lm
    //restart memory mapping with volMemMode set to 1lm.
    if (host->var.mem.volMemMode == VOL_MEM_MODE_2LM){
      status = AdjustVolatileMemMode(host);
      if(status != SUCCESS) continue;
    }
#ifdef SERIAL_DBG_MSG
    if(host->var.mem.MemMapState == MEM_MAP_STATE_RESOURCE_CALCULATION)
      PrintDimmConfig(host);
#endif // SERIAL_DBG_MSG

    //Create DRAM rules based on memory populaton
    status = CreateSADRules(host, &MemMapData);
    if(status != SUCCESS) {
      //Degrade the memory map level and rerun the memory map
      DegradeMemoryMapLevel(host);
      continue;
    }

    // Create TAD rules for each IMC in the system
    status = CreateTADRules(host);
    if(status != SUCCESS)  {
      //Degrade the memory map level and rerun the memory map
      DegradeMemoryMapLevel(host);
      continue;
    }
    // Create RIR rules for each Channel in the system
    CreateRIRRules(host);

    //Call RAS hook to patch memory map before updating the CSRs.

    status = MemoryMapRASHook(host);

    if(status != SUCCESS) continue;

    //Finalize memory map
    status = FinalizeMemoryMap(host);
    if(status != SUCCESS) continue;

    //Display the memory map created
    DisplayMemoryMap(host);

    //Write Memory map CSRs
    WriteMemoryMapCSRs(host);

    //Wrie FPGA Memory map CSRs
    InitFPGAMemoryMap(host);

  } while(status != SUCCESS);

  AdjustMemorySizeFieldsforMirror(host);

#ifdef DEBUG_PERFORMANCE_STATS
#ifdef SERIAL_DBG_MSG
  ClearMemPhase(host, MEM_MAPPING);
#endif
#endif  //  DEBUG_PERFORMANCE_STATS

  OemAfterAddressMapConfigured(host);

  return SUCCESS;
}
