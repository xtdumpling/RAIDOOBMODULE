/**
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c) 2016 - 2017 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

  @file FpgaMemMap.c

  Reference Code Module.

**/

#include "MemAddrMap.h"

void
InitFPGAMemoryMap(PSYSHOST host)
/**

  Fill in FPGA SAD CRSs for memory map from config info in host structure

  @param host  - Pointer to sysHost

  @retval N/A

**/
{
  UINT8 sckt;
  UINT8                                 i=0;
  struct memVar                         *mem;
  struct SADTable                       *SADEntry;
  UINT8 maxSADRules = MAX_SAD_RULES ;
  UINT32 LocalLimit, RemoteLimit;
  UINT32 LocalInterleaveList, RemoteInterleaveList;
  UINT32 FPGAInterleaveList0, FPGAInterleaveList1;
  UINT32 FpgaPresentBitMap;
  DRAM_RULE0_FPGA_CCI_STRUCT FpgaDramRule0;
  DRAM_RULE1_FPGA_CCI_STRUCT FpgaDramRule1;
  UINT32 DRAMRuleCfgFPGA[] =
    { DRAM_RULE0_FPGA_CCI_REG, DRAM_RULE1_FPGA_CCI_REG} ;
  UINT32 InterleaveListCfgFPGA[] =
    { INTERLEAVE_LIST0_FPGA_CCI_REG, INTERLEAVE_LIST1_FPGA_CCI_REG} ;

  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Initiate FPGA SAD CSRs\n"));
  //Only program when FPGA is active
  FpgaPresentBitMap = host->var.common.FpgaPresentBitMap;
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"host->var.common.FpgaPresentBitMap: %8X\n", FpgaPresentBitMap));
  if (FpgaPresentBitMap == 0){
    MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"No Active FPGA\n"));
    return;
  }
  mem = &host->var.mem;
  // Clear the data fields first
  LocalLimit                          = 0;
  RemoteLimit                         = 0;
  FpgaDramRule0.Data                  = 0;
  FpgaDramRule1.Data                  = 0;
  LocalInterleaveList                 = 0;
  RemoteInterleaveList                = 0;
  FPGAInterleaveList0                 = 0;
  FPGAInterleaveList1                 = 0;

  //Caculate Sbsp's rule
  sckt = host->nvram.common.sbspSocketId;
  for (i = 0; i < maxSADRules; i++) {
    SADEntry = &mem->socket[sckt].SAD[i];
    if( SADEntry->Enable ) {
      if( SADEntry->local )  {
        //If this is a local Rule, continue until find the highest limit
        LocalLimit = SADTADEncodeLimit (SADEntry->Limit);
      }
      else {
        //FPGA currently assumes that each CPU only has one remote rule
        //If this is a remote Rule, log it.
        RemoteLimit = SADTADEncodeLimit (SADEntry->Limit);
        RemoteInterleaveList = SADEncodeInterleaveList( host, sckt, i, NMEM);
      }
    }
  }

  if( LocalLimit== 0 && RemoteLimit==0){
    MemDebugPrint ((host, SDBG_MINMAX, sckt, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"error.Unable to Program FPGA Rules. no sbsp SAD enabled\n"));
    return;
  }

  //Start to build FPGA Rules and Interleave list
  //Enable FPGA Rule0 if we get here.
  FpgaDramRule0.Bits.ruleenable = 1;
  //FPGA Interleave0
  //bit[2:0] : Target NID = sckt
  //bit 3    : 0 - remote.
  for(i=0; i<MAX_SAD_WAYS; i++) {
    //
    //Bit0-3 - Target0
    //Bit4-7 - Target1
    //..
    //Bit28-31 - Target7
    LocalInterleaveList |= (sckt << (4 * i)) ;
  }

  if (LocalLimit==0){
    //if there's no local Rule on CPU, FPGA Rule0 is the only Rule that need be written.
    FpgaDramRule0.Bits.addrlimit = RemoteLimit;
    //And it is a remote Rule
    FPGAInterleaveList0 = RemoteInterleaveList;
  }else if (RemoteLimit==0){
    //if there's no remote Rule on CPU, FPGA Rule0 is the only Rule that need be written.
    FpgaDramRule0.Bits.addrlimit = LocalLimit;
    //And it is still a remote Rule to FPGA
    FPGAInterleaveList0 = LocalInterleaveList;
  }else {
    //CPU have both local and Remote Rules. So we need writer both FPGA Rule0 and Rule1.
    //Rule0's limit is Rule1's base. Rule1 must have the higher limit.
     if (LocalLimit < RemoteLimit) {
       FpgaDramRule0.Bits.addrlimit = LocalLimit;
       FpgaDramRule1.Bits.addrlimit = RemoteLimit;
       FPGAInterleaveList0 = LocalInterleaveList;
       FPGAInterleaveList1 = RemoteInterleaveList;
     } else {
       FpgaDramRule0.Bits.addrlimit = RemoteLimit;
       FpgaDramRule1.Bits.addrlimit = LocalLimit;
       FPGAInterleaveList0 = RemoteInterleaveList;
       FPGAInterleaveList1 = LocalInterleaveList;
     }
     FpgaDramRule1.Bits.ruleenable = 1;
  }


  //Print the data to serial debug log
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"          En   Base  Limit\n"));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Rule0  :  %x   %05x  %05x\n",FpgaDramRule0.Bits.ruleenable, 0, FpgaDramRule0.Bits.addrlimit));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"Rule1  :  %x   %05x  %05x\n",FpgaDramRule1.Bits.ruleenable, FpgaDramRule0.Bits.addrlimit+1, FpgaDramRule1.Bits.addrlimit));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\nINTERLEAVE0 : %08x\n", FPGAInterleaveList0));
  MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"INTERLEAVE1 : %08x\n", FPGAInterleaveList1));

  //Update the FPGA CSR

  sckt = 0;
  //Only program when FPGA is active
  while(FpgaPresentBitMap){
    if(FpgaPresentBitMap & BIT0){
      MemDebugPrint ((host, SDBG_MAX, NO_SOCKET, NO_CH, NO_DIMM, NO_RANK, NO_STROBE, NO_BIT,"\t\tWrite FPGA SAD CSRs for Socket: %d\n", sckt));
      WriteCpuPciCfgEx (host, sckt, 0, DRAMRuleCfgFPGA[0], FpgaDramRule0.Data);
      WriteCpuPciCfgEx (host, sckt, 0, InterleaveListCfgFPGA[0], FPGAInterleaveList0);
      if (FpgaDramRule1.Bits.ruleenable) {
        WriteCpuPciCfgEx (host, sckt, 0, DRAMRuleCfgFPGA[1], FpgaDramRule1.Data);
        WriteCpuPciCfgEx (host, sckt, 0, InterleaveListCfgFPGA[1], FPGAInterleaveList1);
      }
    }
    FpgaPresentBitMap >>= 1;
    sckt++;
  }
}
