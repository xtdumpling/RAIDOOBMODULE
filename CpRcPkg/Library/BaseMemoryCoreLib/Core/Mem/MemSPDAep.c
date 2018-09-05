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

#include "SysFunc.h"

extern  UINT8 primaryWidth[MAX_TECH]; // Primary SDRAM device width
extern  UINT8 rowBits[MAX_TECH];      // Number of row address bits
extern  UINT8 columnBits[MAX_TECH];   // Number of column address bits
extern  UINT8 internalBanks[MAX_TECH];// Number of internal SDRAM banks
extern  UINT8 primaryWidthDDR4[MAX_TECH];   // Primary SDRAM device width
extern  UINT8 rowBitsDDR4[MAX_TECH];        // Number of row address bits
extern  UINT8 columnBitsDDR4[MAX_TECH];     // Number of column address bits
extern  UINT8 bankGroupsDDR4[MAX_TECH];     // Number of internal SDRAM banks

const   UINT32 aepDensity[16] =  {32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,13107,262144,524288,1048576}; // MBytes
const   UINT32 dataWidth[4] = {8,16,32,64};
const   UINT32 aepSizeGB[MAX_TECH_AEP] = {8,16,32,64,128,256,512,1024}; // NVMDIMM size in GB

/**

  Detect DIMM population

  @param host  - Point to sysHost

  @retval SUCCESS

**/
UINT32
DetectAepDIMMConfig (
                     PSYSHOST         host,
                     UINT8            socket,
                     UINT8            ch,
                     UINT8            dimm
                  )
{
  UINT8  SPDReg;
  UINT8  ngnCapacity;      // NVMDIMM capacity
  UINT8  ngnBusWidth;      // NVMDIMM bus width
  UINT8  ngnDeviceType;    // NVMDIMM device type
  UINT8  ngnModuleOrg;     // NVMDIMM module organization
  UINT8  ti;
  UINT8  revision;
  UINT32 size;
  struct dimmNvram    (*dimmNvList)[MAX_DIMM];
  struct channelNvram (*channelNvList)[MAX_CH];

  dimmNvList    = GetDimmNvList(host, socket, ch);
  channelNvList = GetChannelNvList(host, socket);

  GetCommonDDR4DIMMConfig (host, socket, ch, dimm);
  host->nvram.mem.socket[socket].aepDimmPresent = 1;
  //
  // Get SPD Spec revision
  //
  ReadSpd (host, socket, ch, dimm, SPD_REVISION, &SPDReg);
  revision = SPDReg;

  //
  // Read SPD Bytes containing DDR module info
  //
  ReadSpd(host, socket, ch, dimm, SPD_KEY_BYTE2, &SPDReg);
  (*dimmNvList)[dimm].keyByte2 = SPDReg & 0xF;
  (*dimmNvList)[dimm].actKeyByte2 = SPDReg & 0xF;

  // Take RDIMM path to minimize changes
  (*dimmNvList)[dimm].keyByte2 = SPD_RDIMM;

  // If the SPD image says this NVM DIMM is an RDIMM, then treat it as such
  if( (*dimmNvList)[dimm].actKeyByte2 == SPD_RDIMM ) {
    (*dimmNvList)[dimm].lrDimmPresent = 0;
    (*channelNvList)[ch].lrDimmPresent = 0;
    host->nvram.mem.socket[socket].lrDimmPresent = 0;
  }
  else  {
    (*dimmNvList)[dimm].lrDimmPresent = 1;
    (*channelNvList)[ch].lrDimmPresent = 1;
    host->nvram.mem.socket[socket].lrDimmPresent = 1;
  }


  (*dimmNvList)[dimm].SPDDeviceType = 0;
  (*dimmNvList)[dimm].SPDMemBusWidth = 0x0B;
  (*dimmNvList)[dimm].sdramCapacity = 4;

  // Hard code to x4 and 1 package rank
  (*dimmNvList)[dimm].SPDModuleOrg = 0;
  (*dimmNvList)[dimm].actSPDModuleOrg = 0;

  if (revision <= 1) {
    //
    // Get NVDIMM Capacity
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_DEV_DENSITY_R1, &SPDReg);
    ngnCapacity = SPDReg;

    //
    // Get NGN Device Type
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_DEV_TYPE_R1, &SPDReg);
    ngnDeviceType = SPDReg;

    //
    // Get NGN Number of ranks and device width
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_MOD_ORGANIZATION_R1, &SPDReg);
    ngnModuleOrg = SPDReg;   

    //
    // Get NGN Bus Width
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_MOD_BUS_WIDTH_R1, &SPDReg);
    ngnBusWidth = SPDReg;

    ReadSpd (host, socket, ch, dimm, SPD_AEP_WRITE_CREDIT_R1, &SPDReg);
    (*channelNvList)[ch].aepWrCreditLimit = SPDReg;
  } else {
    //
    // Get NGN Capacity
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_DEV_DENSITY, &SPDReg);
    ngnCapacity = SPDReg;

    //
    // Get NGN Number of ranks and device width
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_MOD_ORGANIZATION, &SPDReg);
    ngnModuleOrg = SPDReg;

    //
    // Get NGN Device Type
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_DEV_TYPE, &SPDReg);
    ngnDeviceType = SPDReg;

    //
    // Get NGN Bus Width
    //
    ReadSpd (host, socket, ch, dimm, SPD_AEP_MOD_BUS_WIDTH, &SPDReg);
    ngnBusWidth = SPDReg;

    ReadSpd (host, socket, ch, dimm, SPD_AEP_WRITE_CREDIT, &SPDReg);
    (*channelNvList)[ch].aepWrCreditLimit = SPDReg;

    ReadSpd(host, socket, ch, dimm, SPD_AEP_LSB_FMC_TYPE, &SPDReg);
    (*dimmNvList)[dimm].fmcType = SPDReg;

    ReadSpd(host, socket, ch, dimm, SPD_AEP_FMC_REV, &SPDReg);
    (*dimmNvList)[dimm].fmcRev = SPDReg;
  }

  ReadSpd(host, socket, ch, dimm, SPD_AEP_BACKSIDE_SWIZZLE_0, &SPDReg);
  (*dimmNvList)[dimm].lrbufswizzle = SPDReg;
  ReadSpd(host, socket, ch, dimm, SPD_AEP_BACKSIDE_SWIZZLE_1, &SPDReg);
  (*dimmNvList)[dimm].lrbufswizzle |= SPDReg << 8;

  // Size = Die Density * Effective NVMDIMM Channel Data Width * Package Die Count / Independent ports per package * NVMDIMM Channels per dimm
  // Divide by 8 to convert Bits to Bytes
  // Divide by 1024 to convert MB to GB
  // AepDensity in units of MBytes
  size = aepDensity[ngnCapacity & 0xF] *
         dataWidth[ngnBusWidth & 7] *
        (((ngnDeviceType & 0x70) >> 4) + 1) / (((ngnDeviceType & 0xC) >> 2) + 1) *
        (((ngnModuleOrg & 0x80) >> 7) + 1) / (8 * 1024);

  for(ti = 0;ti < MAX_TECH_AEP;ti++){
    if (aepSizeGB[ti] == size) break;
  }

  if(ti >= MAX_TECH_AEP){
    MemDebugPrint((host, SDBG_MINMAX, socket, ch, dimm, NO_RANK, NO_STROBE, NO_BIT,
                  "SPD ERROR - AEP DIMM not supported!\n"));
    DisableChannel(host, socket, ch);
#if SMCPKG_SUPPORT
    OutputWarning (host, ERR_DIMM_COMPAT, ERR_NVMDIMM_NOT_SUPPORTED, socket, ch, dimm, NO_RANK);
#else
    OutputWarning (host, ERR_DIMM_COMPAT, ERR_NVMDIMM_NOT_SUPPORTED, socket, ch, NO_DIMM, NO_RANK);
#endif
  }

  //
  // Save NVM size
  // Note: this is only a temporary workaround for Emulation and CTE execution
  // because NVMCTLR FW command mailbox is not available.
  //
  (*dimmNvList)[dimm].aepTechIndex = ti;

  return SUCCESS;
}

